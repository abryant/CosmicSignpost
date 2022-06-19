#ifndef SPACEPOINTER_LIB_SPACEPOINTER_REFERENCE_FRAME_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_REFERENCE_FRAME_H_

enum ReferenceFrame {
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

#endif
