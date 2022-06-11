#include "cartesian_location.h"

#include <cmath>

#include "direction.h"
#include "error_utils.h"
#include "quaternion.h"
#include "vector.h"

CartesianLocation::CartesianLocation(double x, double y, double z, ReferenceFrame referenceFrame)
  : x(x), y(y), z(z), referenceFrame(referenceFrame) {}

CartesianLocation CartesianLocation::fixed(double x, double y, double z) {
  return CartesianLocation(x, y, z, ReferenceFrame::EARTH_FIXED);
}

Vector CartesianLocation::towards(CartesianLocation other) {
  checkArgumentOrReset(referenceFrame == other.referenceFrame, "mismatched reference frames");
  return Vector(other.x - x, other.y - y, other.z - z).normalized();
}

Direction CartesianLocation::directionTowards(CartesianLocation other, Vector up) {
  checkArgumentOrReset(referenceFrame == other.referenceFrame, "mismatched reference frames");
  checkArgumentOrReset(
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
