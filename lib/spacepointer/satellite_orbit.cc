#include "satellite_orbit.h"

#include <functional>

#include "cartesian_location.h"
#include "vector.h"
#include "sgp4_orbital_elements.h"
#include "sgp4_propagator.h"

const std::string CELESTRAK_URL_CATALOG_NUMBER =
    "https://celestrak.org/NORAD/elements/gp.php?FORMAT=JSON&CATNR=";

// The current orbit starts off empty.
std::string SatelliteOrbit::currentCatalogNumber = "";
std::optional<SGP4::Sgp4State> SatelliteOrbit::currentSgp4State = std::nullopt;

SatelliteOrbit::SatelliteOrbit(std::string catalogNumber)
    : catalogNumber(catalogNumber),
      sgp4OrbitalElements(std::nullopt) {
}

bool SatelliteOrbit::fetchElements(
    std::function<std::optional<std::string>(std::string)> urlFetchFunction) {
  if (sgp4OrbitalElements.has_value()) {
    return true;
  }
  std::string url = CELESTRAK_URL_CATALOG_NUMBER + catalogNumber;
  std::optional<std::string> json = urlFetchFunction(url);
  if (!json.has_value()) {
    return false;
  }
  std::optional<OmmMessage> ommMessage = OmmMessage::fromJson(json.value());
  if (!ommMessage.has_value() || !ommMessage.value().hasSgp4Elements) {
    return false;
  }
  sgp4OrbitalElements = SGP4::Sgp4OrbitalElements(ommMessage.value());
  return true;
}

std::string SatelliteOrbit::getName() {
  if (sgp4OrbitalElements.has_value()) {
    return sgp4OrbitalElements->name;
  }
  return "";
}

double SatelliteOrbit::getOrbitalPeriodSeconds() {
  if (sgp4OrbitalElements.has_value()) {
    return 86400.0 / sgp4OrbitalElements->meanMotionRevsPerDay;
  }
  return 0.0;
}

CartesianLocation SatelliteOrbit::toCartesian(int64_t timeMillis) {
  if (currentCatalogNumber != catalogNumber) {
    currentCatalogNumber = catalogNumber;
    if (!sgp4OrbitalElements.has_value()) {
      return CartesianLocation::fixed(Vector(0, 0, 0));
    }
    currentSgp4State =
        SGP4::initialiseSgp4(
            SGP4::WgsVersion::WGS_72, SGP4::OperationMode::AFSPC, sgp4OrbitalElements.value());
  }
  double timeSinceEpochMinutes =
      SGP4::findTimeSinceEpochMinutes(currentSgp4State.value(), timeMillis);
  SGP4::Sgp4Result result = SGP4::runSgp4(currentSgp4State.value(), timeSinceEpochMinutes);
  if (result.code != SGP4::ResultCode::SUCCESS) {
    return CartesianLocation::fixed(Vector(0, 0, 0));
  }
  // Convert from kilometres to metres.
  Vector vector(result.x * 1000, result.y * 1000, result.z * 1000);
  return CartesianLocation(vector, ReferenceFrame::EARTH_EQUATORIAL);
}
