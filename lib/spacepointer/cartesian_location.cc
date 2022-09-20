#include "cartesian_location.h"

#include <cmath>
#include <cstdint>
#include <sstream>

#include "angle_utils.h"
#include "direction.h"
#include "earth_rotation.h"
#include "error_utils.h"
#include "moon_orbit.h"
#include "planetary_orbit.h"
#include "quaternion.h"
#include "time_utils.h"
#include "vector.h"

const double EARTH_AXIAL_TILT_DEGREES = 23.43929111;

CartesianLocation::CartesianLocation(double x, double y, double z, ReferenceFrame referenceFrame)
  : x(x), y(y), z(z), referenceFrame(referenceFrame) {}

CartesianLocation CartesianLocation::fixed(double x, double y, double z) {
  return CartesianLocation(x, y, z, ReferenceFrame::EARTH_FIXED);
}

Vector CartesianLocation::towards(CartesianLocation other) {
  checkArgument(referenceFrame == other.referenceFrame, "mismatched reference frames");
  return Vector(other.x - x, other.y - y, other.z - z).normalized();
}

Direction CartesianLocation::directionTowards(CartesianLocation other, Vector up) {
  checkArgument(referenceFrame == other.referenceFrame, "mismatched reference frames");
  checkArgument(
      referenceFrame == ReferenceFrame::EARTH_FIXED,
      "directions only make sense in EARTH_FIXED");
  Vector toOther = towards(other);
  double altitudeDegrees = 90.0 - up.angleDegrees(toOther);

  if (up.getX() == 0 && up.getY() == 0) {
    // We are at one of the poles. It doesn't make sense to talk about an azimuth here, so use 0.
    return Direction(0.0, altitudeDegrees);
  }

  // The ECEF coordinate system has the X axis towards the prime meridian, so atan2() gets us the
  // anticlockwise angle from the X axis. Facing from north to south, we need to rotate the up
  // vector by this angle clockwise.
  double angleToPrimeMeridianRadians = atan2(up.getY(), up.getX());
  // Quaternion rotations are clockwise when viewed along the axis of rotation, so for north to
  // south we need to use south (negative Z axis).
  Quaternion toPrimeMeridian = Quaternion(angleToPrimeMeridianRadians, Vector(0, 0, -1));
  Vector upAtPrimeMeridian = toPrimeMeridian.rotate(up);

  // We need to rotate towards the equator. Facing along the Y axis, the point where the prime
  // meridian touches the equator is at 0 radians, and the result from atan2() returns positive
  // values for anticlockwise rotations from that.
  double angleToEquatorRadians = atan2(upAtPrimeMeridian.getZ(), upAtPrimeMeridian.getX());
  // Quaternion rotations are clockwise when viewed along the axis of rotation, so we just use the
  // anticlockwise angle from atan2() to get back to zero.
  Quaternion toEquator = Quaternion(angleToEquatorRadians, Vector(0, 1, 0));

  // We want to rotate our toOther vector by the same rotation that takes our up vector to the
  // point where the prime meridian touches the equator. Quaternions can be combined by multiplying
  // them, but they are not commutative. The rotation on the right gets applied first, followed by
  // the rotation on the left.
  Quaternion upToReferenceLocation = toEquator * toPrimeMeridian;

  // At the reference location, we can calculate the azimuth using atan2().
  Vector toOtherAtReference = upToReferenceLocation.rotate(toOther);
  if (toOtherAtReference.getZ() == 0 && toOtherAtReference.getY() == 0) {
    // toOther is either directly above us or directly below us, so the azimuth is irrelevant.
    return Direction(0.0, altitudeDegrees);
  }
  // Now that we're at the reference location, the azimuth is defined as the angle from north.
  // Looking at the location in ECEF, the Z axis points north and the Y axis points east.
  // We subtract this from atan2(1, 0) to convert from an anticlockwise angle from east into a
  // clockwise angle from north.
  double azimuth = atan2(1, 0) - atan2(toOtherAtReference.getZ(), toOtherAtReference.getY());
  double azimuthDegrees = azimuth * 180.0 / M_PI;

  // The Direction constructor wraps the azimuth.
  return Direction(azimuthDegrees, altitudeDegrees);
}

CartesianLocation CartesianLocation::toFixed(int64_t timeMillis) {
  if (referenceFrame == ReferenceFrame::EARTH_FIXED) {
    return *this;
  }

  if (referenceFrame == ReferenceFrame::EARTH_EQUATORIAL) {
    Vector fixedPos = EarthRotation::earthEquatorialToEarthFixed(position, timeMillis);
    return CartesianLocation(fixedPos, ReferenceFrame::EARTH_FIXED);
  }

  if (referenceFrame == ReferenceFrame::EARTH_ECLIPTIC) {
    // Convert to EARTH_EQUATORIAL first.
    double axialTiltRadians = degreesToRadians(EARTH_AXIAL_TILT_DEGREES);
    Quaternion axialTiltRotation = Quaternion::rotateX(axialTiltRadians);
    Vector tiltedPos = axialTiltRotation.rotate(position);
    return CartesianLocation(tiltedPos, ReferenceFrame::EARTH_EQUATORIAL).toFixed(timeMillis);
  }

  if (referenceFrame == ReferenceFrame::SUN_ECLIPTIC) {
    CartesianLocation earthMoonBarycentreLocationFromSun =
        PlanetaryOrbit::EARTH_MOON_BARYCENTRE.toCartesian(timeMillis);
    CartesianLocation earthMoonBarycentreLocationFromEarth =
        MoonOrbit::earthMoonBarycentreAt(timeMillis);
    return CartesianLocation(
      earthMoonBarycentreLocationFromEarth.x - earthMoonBarycentreLocationFromSun.x + x,
      earthMoonBarycentreLocationFromEarth.y - earthMoonBarycentreLocationFromSun.y + y,
      earthMoonBarycentreLocationFromEarth.z - earthMoonBarycentreLocationFromSun.z + z,
      ReferenceFrame::EARTH_ECLIPTIC
    ).toFixed(timeMillis);
  }

  failWithError("Unknown reference frame");
  // unreachable
  return *this;
}

std::string CartesianLocation::toString() {
  std::ostringstream ss;
  ss << "[" << x << ", " << y << ", " << z << " in " << referenceFrameToString(referenceFrame) << "]";
  return ss.str();
}
