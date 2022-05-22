#ifndef SPACEPOINTER_SRC_LOCATION_H_
#define SPACEPOINTER_SRC_LOCATION_H_

class Location {
  private:
    // Latitude in degrees, positive means north.
    double latitude;

    // Longitude in degrees from the prime meridian [-180, 180).
    double longitude;

    // Elevation above sea level in metres.
    double elevation;

  public:
    Location(double latitude, double longitude, double elevation);
    double getLatitude();
    double getLongitude();
    double getElevation();
};

#endif
