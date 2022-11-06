#include "satellite_orbit.h"

#include <functional>

#include "cartesian_location.h"
#include "vector.h"
#include "sgp4_orbital_elements.h"
#include "sgp4_propagator.h"

const std::string CELESTRAK_URL_CATALOG_NUMBER =
    "https://celestrak.org/NORAD/elements/gp.php?FORMAT=JSON-PRETTY&CATNR=";

SatelliteOrbit::SatelliteOrbit(std::string catalogNumber)
    : catalogNumber(catalogNumber),
      ommMessage(std::nullopt),
      sgp4PropagatorState(std::nullopt) {
}

bool SatelliteOrbit::fetchElements(std::function<std::optional<std::string>(std::string)> urlFetchFunction) {
  std::string url = CELESTRAK_URL_CATALOG_NUMBER + catalogNumber;
  std::optional<std::string> json = urlFetchFunction(url);
  if (!json.has_value()) {
    return false;
  }
  ommMessage = OmmMessage::fromJson(json.value());
  if (!ommMessage.has_value() || !ommMessage.value().hasSgp4Elements) {
    return false;
  }
  SGP4::Sgp4OrbitalElements elements = SGP4::Sgp4OrbitalElements(ommMessage.value());
  sgp4PropagatorState = SGP4::initialiseSgp4(SGP4::WgsVersion::WGS_72, SGP4::OperationMode::AFSPC, elements);
  return true;
}

CartesianLocation SatelliteOrbit::toCartesian(int64_t timeMillis) {
  if (!sgp4PropagatorState.has_value()) {
    return CartesianLocation::fixed(Vector(0, 0, 0));
  }
  double timeSinceEpochMinutes = SGP4::findTimeSinceEpochMinutes(sgp4PropagatorState.value(), timeMillis);
  SGP4::Sgp4Result result = SGP4::runSgp4(sgp4PropagatorState.value(), timeSinceEpochMinutes);
  if (result.code != SGP4::ResultCode::SUCCESS) {
    return CartesianLocation::fixed(Vector(0, 0, 0));
  }
  // Convert from kilometres to metres.
  Vector vector(result.x * 1000, result.y * 1000, result.z * 1000);
  return CartesianLocation(vector, ReferenceFrame::EARTH_EQUATORIAL);
}
