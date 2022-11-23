#ifndef COSMIC_SIGNPOST_LIB_TRACKING_LOCATION_H_
#define COSMIC_SIGNPOST_LIB_TRACKING_LOCATION_H_

#include "cartesian_location.h"
#include "direction.h"

class Location {
  private:
    // Latitude in degrees, positive means north.
    double latitude;

    // Longitude in degrees from the prime meridian [-180, 180).
    double longitude;

    // Elevation above Earth's ellipsoid in metres.
    double elevation;

  public:
    Location(double latitude, double longitude, double elevation);
    double getLatitude();
    double getLongitude();
    double getElevation();
    CartesianLocation getCartesian();
    Vector getNormal();
    Direction directionTo(Location other);
};

#endif
