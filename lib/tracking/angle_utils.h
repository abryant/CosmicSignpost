#ifndef COSMIC_SIGNPOST_LIB_TRACKING_ANGLE_UTILS_H_
#define COSMIC_SIGNPOST_LIB_TRACKING_ANGLE_UTILS_H_

// Wraps degrees to [-180,180).
double wrapDegrees(double degrees);
// Wraps radians to [-pi,pi).
double wrapRadians(double radians);

double degreesToRadians(double degrees);

#endif
