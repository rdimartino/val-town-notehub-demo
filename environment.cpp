#include "main.h"
#include <Notecard.h>

// Fetch notecard's last modified timestamp for the environment variables
int get_last_modified() {
  J* req = notecard.newRequest("env.modified");
  if (req == NULL) {
    Serial.println("env.modified req is null");
    return -1;
  }
  J* resp = notecard.requestAndResponse(req);
  if (resp == NULL) {
    Serial.println("env.modfified resp is null");
    return -1;
  }
  int time = JGetInt(resp, "time");
  notecard.deleteResponse(resp);
  return time;
}

// Update config.loopDelay from the notecard environment
void set_loop_delay() {
  J* req = notecard.newRequest("env.get");
  if (req == NULL) {
    Serial.println("env.get req is null");
    return;
  }
  JAddStringToObject(req, "name", "loopDelay");
  J* resp = notecard.requestAndResponse(req);
  if (resp == NULL) {
    Serial.println("env.get resp is null");
    return;
  }
  int val = atoi(JGetString(resp, "text"));
  notecard.deleteResponse(resp);
  // If we fail to parse the integer, val will be 0
  // this is also not a useful value for the loop delay so just return
  if (val == 0) {
    Serial.println("env val parsed as 0");
    return;
  }
  config.loopDelay = val;
}

void sync_environment(bool force) {
  bool sync = force;
  static uint32_t previous = 0;
  int current = get_last_modified();
  // if the environment modified ts has been updated then sync the env var
  if (current > previous) {
    previous = current;
    sync = true;
  }
  if (sync) {
    set_loop_delay();
  }
}