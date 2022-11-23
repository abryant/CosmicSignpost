#ifndef COSMIC_SIGNPOST_LIB_TRACKING_ERROR_UTILS_H_
#define COSMIC_SIGNPOST_LIB_TRACKING_ERROR_UTILS_H_

#include <string>

void checkArgument(bool check, std::string errorMessage);

void failWithError(std::string errorMessage);

#endif
