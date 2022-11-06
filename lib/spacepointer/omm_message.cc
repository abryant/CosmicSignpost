#include "omm_message.h"

#include <ArduinoJson.h>

std::optional<OmmMessage> OmmMessage::fromJson(std::string json) {

  ArduinoJson::StaticJsonDocument<768> doc;
  ArduinoJson::DeserializationError error = ArduinoJson::deserializeJson(doc, json);
  if (error) {
    return std::nullopt;
  }

  OmmMessage message {};
  ArduinoJson::JsonObject ommJson = doc[0];
  message.objectName = std::string(ommJson["OBJECT_NAME"]);
  message.objectId = std::string(ommJson["OBJECT_ID"]);
  message.centerName = std::string(ommJson["CENTER_NAME"]); // optional
  message.referenceFrame = std::string(ommJson["REF_FRAME"]); // optional
  message.referenceFrameEpoch = std::string(ommJson["REF_FRAME_EPOCH"]); // optional
  message.timeSystem = std::string(ommJson["TIME_SYSTEM"]); // optional
  message.meanElementTheory = std::string(ommJson["MEAN_ELEMENT_THEORY"]); // optional

  message.epoch = std::string(ommJson["EPOCH"]);
  message.semiMajorAxis = ommJson["SEMI_MAJOR_AXIS"]; // optional
  message.meanMotion = ommJson["MEAN_MOTION"]; // optional (but required for SGP4)
  message.eccentricity = ommJson["ECCENTRICITY"];
  message.inclination = ommJson["INCLINATION"];
  message.rightAscensionOfAscendingNode = ommJson["RA_OF_ASC_NODE"];
  message.argumentOfPericenter = ommJson["ARG_OF_PERICENTER"];
  message.meanAnomaly = ommJson["MEAN_ANOMALY"];
  message.gravitationalCoefficient = ommJson["GM"]; // optional

  message.mass = ommJson["MASS"]; // optional
  message.solarRadiationPressureArea = ommJson["SOLAR_RAD_AREA"]; // optional
  message.solarRadiationPressureCoefficient = ommJson["SOLAR_RAD_COEFF"]; // optional
  message.dragArea = ommJson["DRAG_AREA"]; // optional
  message.dragCoefficient = ommJson["DRAG_COEFFICIENT"]; // optional

  message.ephemerisType = std::string(ommJson["EPHEMERIS_TYPE"]); // optional
  message.classificationType = std::string(ommJson["CLASSIFICATION_TYPE"]); // optional

  message.noradCatalogNumber = std::string(ommJson["NORAD_CAT_ID"]); // optional
  message.elementSetNumber = std::string(ommJson["ELEMENT_SET_NO"]); // optional
  message.revolutionNumberAtEpoch = ommJson["REV_AT_EPOCH"]; // optional
  message.bStarDragCoefficient = ommJson["BSTAR"]; // optional
  message.meanMotionDot = ommJson["MEAN_MOTION_DOT"]; // optional
  message.meanMotionDdot = ommJson["MEAN_MOTION_DDOT"]; // optional

  message.hasSgp4Elements = true
      && ommJson.containsKey("EPOCH")
      && ommJson.containsKey("MEAN_MOTION")
      && ommJson.containsKey("ECCENTRICITY")
      && ommJson.containsKey("INCLINATION")
      && ommJson.containsKey("RA_OF_ASC_NODE")
      && ommJson.containsKey("ARG_OF_PERICENTER")
      && ommJson.containsKey("MEAN_ANOMALY")
      && ommJson.containsKey("BSTAR");

  return std::optional(message);
}
