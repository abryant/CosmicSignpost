#ifndef SPACEPOINTER_LIB_TRACKING_REFERENCE_FRAME_H_
#define SPACEPOINTER_LIB_TRACKING_REFERENCE_FRAME_H_

#include <string>

enum class ReferenceFrame {
  // https://en.wikipedia.org/wiki/Earth-centered,_Earth-fixed_coordinate_system
  // X axis: always towards the prime meridian at the equator (rotating, not inertial)
  // Z axis: towards the north pole
  // Y axis: towards 90 degrees east at the equator
  EARTH_FIXED,
  // https://en.wikipedia.org/wiki/Equatorial_coordinate_system
  // X axis: towards the vernal equinox at J2000
  // Z axis: towards the north pole
  // Y axis: such that Z = X cross Y (right handed coordinate system)
  EARTH_EQUATORIAL,
  // https://en.wikipedia.org/wiki/Ecliptic_coordinate_system
  // X axis: towards the vernal equinox at J2000 (like EARTH_EQUATORIAL)
  // Z axis: perpendicular to the earth's orbital plane, in the same general direction as the equatorial Z axis
  // Y axis: on the earth's orbital plane, such that Z = X cross Y (right handed coordinate system)
  EARTH_ECLIPTIC,
  // https://en.wikipedia.org/wiki/Ecliptic_coordinate_system
  // Axes are the same as EARTH_ECLIPTIC, but centred on the Solar System Barycentre.
  SUN_ECLIPTIC,
};

inline std::string referenceFrameToString(ReferenceFrame value) {
  switch (value)
  {
  case ReferenceFrame::EARTH_FIXED:
    return "EARTH_FIXED";
  case ReferenceFrame::EARTH_EQUATORIAL:
    return "EARTH_EQUATORIAL";
  case ReferenceFrame::EARTH_ECLIPTIC:
    return "EARTH_ECLIPTIC";
  case ReferenceFrame::SUN_ECLIPTIC:
    return "SUN_ECLIPTIC";
  default:
    return "Unknown ReferenceFrame";
  }
}

#endif
