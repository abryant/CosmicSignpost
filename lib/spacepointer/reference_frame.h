#ifndef SPACEPOINTER_LIB_SPACEPOINTER_REFERENCE_FRAME_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_REFERENCE_FRAME_H_

#include <string>

enum class ReferenceFrame {
  // https://en.wikipedia.org/wiki/Earth-centered,_Earth-fixed_coordinate_system
  EARTH_FIXED,
  // https://en.wikipedia.org/wiki/Equatorial_coordinate_system
  // (like EARTH_FIXED, but with a rotating primary direction based on J2000.0 instead of the prime
  // meridian)
  EARTH_EQUATORIAL,
  // https://en.wikipedia.org/wiki/Ecliptic_coordinate_system
  // (like EARTH_EQUATORIAL, but with the plane of the ecliptic instead of the equator)
  EARTH_ECLIPTIC,
  // https://en.wikipedia.org/wiki/Ecliptic_coordinate_system
  // (like EARTH_ECLIPTIC, but with the sun as the centre instead of the earth)
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
