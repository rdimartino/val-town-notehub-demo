#include <Notecard.h>
#include <Wire.h>
#include "main.h"

#define usbSerial Serial

#define LED_PIN 13

// Please change
#define PRODUCT_UID "dev.dimartino.robert:valtownnotehubdemo"

#define INBOUND_NOTE_FILE "commands.qis"
#define OUTBOUND_NOTE_FILE "output.qos"
#define OUTBOUND_HEALTH_FILE "health.qos"
#define HUB_SET_RETRY_SECONDS 10

Notecard notecard;
Config config;

void notecard_config() {
  J *req = notecard.newRequest("hub.set");
  if (req == NULL) {
    Serial.println("hub.set req is null");
    return;
  }
  JAddStringToObject(req, "product", PRODUCT_UID);
  JAddStringToObject(req, "mode", "continuous");
  JAddBoolToObject(req, "sync", true);
  if (!notecard.sendRequestWithRetry(req, HUB_SET_RETRY_SECONDS)) {
    Serial.println("hub.set request failed.");
    return;
  }
}

void notecard_config_wifi() {
  J *req = notecard.newRequest("card.wifi");
  JAddStringToObject(req, "ssid", "<ssid>");
  JAddStringToObject(req, "password", "<password>");
  notecard.sendRequest(req);
}

void setup_notecard() {
  notecard.setDebugOutputStream(usbSerial);
  notecard.begin();

  notecard_config();
  // notecard_config_wifi();

  // Notify the Notehub of our current firmware version
  J *req = notecard.newRequest("dfu.status");
  if (req == NULL) {
    Serial.println("dfu.status req is null");
    return;
  }
  JAddStringToObject(req, "version", firmwareVersion());
  notecard.sendRequest(req);
}

void set_led(bool on) {
  // Set state of LED
  if (on) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  // Report action to Notehub
  J *req = notecard.newRequest("note.add");
  if (req == NULL) {
    Serial.println("note.add set led is null");
    return;
  }
  JAddStringToObject(req, "file", OUTBOUND_NOTE_FILE);
  JAddBoolToObject(req, "sync", true);
  J *body = JAddObjectToObject(req, "body");
  if (body == NULL) {
    Serial.println("note.add set led body is null");
    return;
  }

  JAddBoolToObject(body, "led_active", on);
  if (on) {
    JAddStringToObject(body, "message", "LED activated");
  } else {
    JAddStringToObject(body, "message", "LED deactivated");
  }

  notecard.sendRequest(req);
}

// Check for commands from notehub
// Returns true when there was a command to process
// Returns false when there were no commands to process
bool fetch_commands() {
  J *req = notecard.newRequest("note.get");
  if (req == NULL) {
    Serial.println("note.get commands req is null");
    return false;
  }
  JAddStringToObject(req, "file", "commands.qis");
  JAddBoolToObject(req, "delete", true);
  J *resp = notecard.requestAndResponse(req);
  if (resp == NULL) {
    Serial.println("note.get commands resp is null");
    return false;
  }
  bool out = false;
  if (!JContainsString(resp, "err", "{note-noexist}")) {
    out = true;
    J *body = JGetObject(resp, "body");
    if (JContainsString(body, "message", "host.reset")) {
      Serial.println("Received remote reset command");
      esp_restart();
    }
    if (JContainsString(body, "message", "led.on")) {
      Serial.println("Received remote LED activation");
      set_led(true);
    }
    if (JContainsString(body, "message", "led.off")) {
      Serial.println("Received remote LED deactivation");
      set_led(false);
    }
  }
  notecard.deleteResponse(resp);
  return out;
}

// Fetch the voltage readings from the notecard
double card_voltage() {
  J *req = notecard.newRequest("card.voltage");
  if (req == NULL) {
    Serial.println("card.voltage req is null");
    return -1;
  }
  JAddNumberToObject(req, "hours", 1);  // Get readings for last hour
  J *resp = notecard.requestAndResponse(req);
  if (resp == NULL) {
    Serial.println("card.voltage resp is null");
    return -1;
  }
  int voltage = JGetNumber(resp, "value");
  notecard.deleteResponse(resp);
  return voltage;
}

void report_health() {
  J *req = notecard.newRequest("note.add");
  if (req == NULL) {
    Serial.println("note.add health req is null");
    return;
  }
  JAddStringToObject(req, "file", OUTBOUND_HEALTH_FILE);
  JAddBoolToObject(req, "sync", true);
  J *body = JAddObjectToObject(req, "body");
  if (body == NULL) {
    Serial.println("note.add health body is null");
    return;
  }

  JAddNumberToObject(body, "card_voltage", card_voltage());

  notecard.sendRequest(req);
}

void setup() {
  delay(3 * ms1Sec);
  usbSerial.begin(57600);

  pinMode(LED_PIN, OUTPUT);

  setup_notecard();
  dfuShowPartitions();

  // Force a sync of env vars during setup
  sync_environment(true);
}

void loop() {
  static uint32_t loopTimer = 0;

  // Fetch commands until all are processed
  while (fetch_commands()) {
    delay(3 * ms1Sec);  // delay between commands in case they get bunched up
  };

  // Report health every five minutes
  if (millis() > loopTimer + ms1Min * 5) {
    loopTimer = millis();
    report_health();
  }

  sync_environment(false);

  // Poll subsystems that need periodic servicing
  dfuPoll(false);

  delay(config.loopDelay * ms1Sec);
}