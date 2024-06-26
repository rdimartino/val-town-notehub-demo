#include "main.h"

// Definitions used by firmware update
#define PRODUCT_ORG_NAME "Robert DiMartino"
#define PRODUCT_DISPLAY_NAME "Val Town Notehub Demo"
#define PRODUCT_FIRMWARE_ID "val-town-notehub-demo-esp32"
#define PRODUCT_DESC ""
#define PRODUCT_MAJOR 0
#define PRODUCT_MINOR 0
#define PRODUCT_PATCH 1
#define PRODUCT_BUILD 0
#define PRODUCT_BUILT __DATE__ " " __TIME__
#define PRODUCT_BUILDER ""
#define PRODUCT_VERSION STRINGIFY(PRODUCT_MAJOR) "." STRINGIFY(PRODUCT_MINOR) "." STRINGIFY(PRODUCT_PATCH)

// This is a product configuration JSON structure that enables the Notehub to recognize this
// firmware when it's uploaded, to help keep track of versions and so we only ever download
// firmware buildss that are appropriate for this device.
#define QUOTE(x) "\"" x "\""
#define FIRMWARE_VERSION_HEADER "firmware::info:"
#define FIRMWARE_VERSION \
  FIRMWARE_VERSION_HEADER \
  "{" QUOTE("org") ":" QUOTE(PRODUCT_ORG_NAME) "," QUOTE("product") ":" QUOTE(PRODUCT_DISPLAY_NAME) "," QUOTE("description") ":" QUOTE(PRODUCT_DESC) "," QUOTE("firmware") ":" QUOTE(PRODUCT_FIRMWARE_ID) "," QUOTE("version") ":" QUOTE(PRODUCT_VERSION) "," QUOTE("built") ":" QUOTE(PRODUCT_BUILT) "," QUOTE("ver_major") ":" STRINGIFY(PRODUCT_MAJOR) "," QUOTE("ver_minor") ":" STRINGIFY(PRODUCT_MINOR) "," QUOTE("ver_patch") ":" STRINGIFY(PRODUCT_PATCH) "," QUOTE("ver_build") ":" STRINGIFY(PRODUCT_BUILD) "," QUOTE("builder") ":" QUOTE(PRODUCT_BUILDER) "}"

// In the Arduino IDE, the ino is built regardless of whether or not it is modified.  As such, it's a perfect
// place to serve up the build version string because __DATE__ and __TIME__ are updated properly for each build.
const char *productVersion() {
  return ("Ver " PRODUCT_VERSION " " PRODUCT_BUILT);
}

// Return the firmware's version, which is both stored within the image and which is verified by DFU
const char *firmwareVersion() {
  return &FIRMWARE_VERSION[strlen(FIRMWARE_VERSION_HEADER)];
}

const char *productVersionShort() {
  return PRODUCT_VERSION;
}