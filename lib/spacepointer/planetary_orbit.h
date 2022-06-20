#ifndef SPACEPOINTER_LIB_SPACEPOINTER_PLANETARY_ORBIT_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_PLANETARY_ORBIT_H_

#include "cartesian_location.h"
#include "reference_frame.h"

double METRES_PER_AU = 149597870700.0;

// Planetary orbit tracker, based on https://ssd.jpl.nasa.gov/planets/approx_pos.html
class PlanetaryOrbit {
  private:
    double semiMajorAxisJ2000; // metres, at J2000
    double semiMajorAxisDelta; // metres per century
    double eccentricityJ2000; // unitless, at J2000
    double eccentricityDelta; // change per century
    double inclinationJ2000; // radians, at J2000
    double inclinationDelta; // radians per century
    double meanLongitudeJ2000; // radians, at J2000
    double meanLongitudeDelta; // radians per century
    double longitudeOfPerihelionJ2000; // radians, at J2000
    double longitudeOfPerihelionDelta; // radians per century
    double longitudeOfAscendingNodeJ2000; // radians, at J2000
    double longitudeOfAscendingNodeDelta; // radians per century
    // extra terms for outer planets
    double bTimeSquaredCoefficient; // radians per century^2
    double cCosineCoefficient; // radians
    double sSineCoefficient; // radians
    double fFrequencyMultiplier; // radians per century

    // Finds the eccentric anomaly in radians, given mean anomaly in radians.
    double findEccentricAnomaly(double meanAnomalyRadians, double eccentricity) const;

  public:
    PlanetaryOrbit(
        double semiMajorAxisAuJ2000,
        double semiMajorAxisAuDelta,
        double eccentricityJ2000,
        double eccentricityDelta,
        double inclinationDegreesJ2000,
        double inclinationDegreesDelta,
        double meanLongitudeDegreesJ2000,
        double meanLongitudeDegreesDelta,
        double longitudeOfPerihelionDegreesJ2000,
        double longitudeOfPerihelionDegreesDelta,
        double longitudeOfAscendingNodeDegreesJ2000,
        double longitudeOfAscendingNodeDegreesDelta,
        // extra terms for outer planets
        double bTimeSquaredCoefficientDegrees,
        double cCosineCoefficientDegrees,
        double sSineCoefficientDegrees,
        double fFrequencyMultiplierDegrees);

    // Finds the position of the orbiting body at the specified point in time.
    CartesianLocation toCartesian(long timeMillis) const;

    static const PlanetaryOrbit MERCURY;
    static const PlanetaryOrbit VENUS;
    static const PlanetaryOrbit EARTH_MOON_BARYCENTRE;
    static const PlanetaryOrbit MARS;
    static const PlanetaryOrbit JUPITER;
    static const PlanetaryOrbit SATURN;
    static const PlanetaryOrbit URANUS;
    static const PlanetaryOrbit NEPTUNE;
};

#endif
