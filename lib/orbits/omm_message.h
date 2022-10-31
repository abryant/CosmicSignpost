#ifndef SPACEPOINTER_LIB_ORBITS_OMM_MESSAGE_H_
#define SPACEPOINTER_LIB_ORBITS_OMM_MESSAGE_H_

#include <string>

/**
 * An Orbital Mean Elements Message (OMM), as defined by CCSDS 502.0-B-2.
 * See https://public.ccsds.org/Pubs/502x0b2c1e2.pdf
 */
class OmmMessage {
  public:
    std::string objectName;
    std::string objectId;
    std::string centerName;
    std::string referenceFrame;
    std::string referenceFrameEpoch;
    std::string timeSystem;
    std::string meanElementTheory;

    std::string epoch; // date-time string
    double semiMajorAxis; // km
    double meanMotion; // rev/day
    double eccentricity;
    double inclination; // degrees
    double rightAscensionOfAscendingNode; // degrees
    double argumentOfPericenter; // degrees
    double meanAnomaly; // degrees
    double gravitationalCoefficient; // km^3/s^2

    double mass; // satellite mass in kg
    double solarRadiationPressureArea; // m^2
    double solarRadiationPressureCoefficient;
    double dragArea; // m^2
    double dragCoefficient;

    std::string ephemerisType;
    std::string classificationType;

    std::string noradCatalogNumber;
    std::string elementSetNumber;
    double revolutionNumberAtEpoch;
    double bStarDragCoefficient;
    double meanMotionDot; // first time derivative of mean motion, rev/day^2
    double meanMotionDdot; // second time derivative of mean motion, rev/day^3

    // See covariance matrix documentation in OMM message.
    // Indices are zero-based instead of 1-based.
    // Note that only points [x][y] where y <= x are set to non-zero values.
    double covarianceMatrix[6][6];
    std::string covarianceReferenceFrame;
};

#endif
