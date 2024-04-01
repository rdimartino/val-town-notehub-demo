# Val Town Notehub Demo

A demo application connecting [Val Town](https://val.town) with [Notehub](https://notehub.io) to add remote control and monitoring of an ESP32 microcontroller.

## Overview

Provides a simple web application that allows sending commands to a microcontroller to toggle an on-board LED. The web app also allows viewing of recorded events in the database.

Events from the microcontroller (health reports and command log output) are sent to Notehub by the Notecard and routed to Val Town. They are then saved into a Val Town SQLite database.

A cron-like job exports the saved records and emails them to the Val Town account holder. It also deletes older saved records to stay within Val Towns 10mb free-tier.

An [Arduino IDE](https://www.arduino.cc/en/software) `.ino` file for the microcontroller is also included.

## Hardware

 - [Espressif ESP32](https://www.espressif.com/en/products/socs/esp32)
 - [Blue's Notecard](https://dev.blues.io/datasheets/notecard-datasheet/note-wbglw/)
 - [Blue's Notecarrier](https://dev.blues.io/datasheets/notecarrier-datasheet/notecarrier-f-v1-3/)

## Val Town

The frontend and the user-interfacing part of the backend are hosted on [Val Town](https://www.val.town) - a social website to write and run code.

There are several "vals" that coordinate to support the application.

Some vals have relative references to other vals so they will need to be organized in a folder in your account. Val Town does not currently (as of April 2024) allow sharing of a folder of vals, so you'll need to recreate the structure yourself.

Here is a summary of the vals that comprise this project, they each have READMEs included on Val Town:

 - [notehub_demo_setup](https://www.val.town/v/rdimartino/notehub_demo_setup) - a script val for project setup: create the database and provide instructions for configuring authentication for the Notehub routes
 - [notehub_demo_web](https://www.val.town/v/rdimartino/notehub_demo_web) - an HTTP val for the web interface for the application
 - [notehub](https://www.val.town/v/rdimartino/notehub) - a minimal (and incomplete) val script library for authenticating and push notes to Notehub
 - [notehub_demo_webhook](https://www.val.town/v/rdimartino/notehub_demo_webhook) - an HTTP val for responding to the routed events from Notehub
 - [notehub_demo_db](https://www.val.town/v/rdimartino/notehub_demo_db) - a script val with helper methods for setting up the database and some CR~U~D operations (no updates to existing records)
 - [notehub_demo_jobs](https://www.val.town/v/rdimartino/notehub_demo_jobs) - an interval val to export the db and delete old records
 - [basic_auth](https://www.val.town/v/rdimartino/basic_auth) - an HTTP val that provides basic HTTP authentication used in both the web interface and the webhook

Most of the vals are unlisted. They should be forked (or copied) and placed in you designated folder for this project.

The `notehub` and `basic_auth` vals are public, so they can imported directly via their `esm.town` links, e.g.
```
import { authMiddleware } from "https://esm.town/v/rdimartino/basic_auth"
```

## Notehub

[Notehub](https://notehub.io) is a SaaS platform for interacting with Notecards, including configuring and routing event data.

Commands to toggle the microcontroller's LED are sent from Val Town to Notehub. The commands are added as notes to the configured Notecard device; Notehub handles syncing the notes to the Notecard. The microcontroller reads the notes from the Notecard and processes the signal accordingly.

The microcontroller also pushes notes to Notehub:

 - Health events - health check data that currently just includes the onboard voltage reading from the Notecard
 - Output events - log messages that are pushed to Notehub as part of the process of responding to commands

In Notehub, routes need to be configured to handle these events to forward them to Val Town for processing.

See the `notehub_demo_setup`, `notehub_demo_webhook`, and `notehub` vals for more information.

## Arduino Sketch

The `PRODUCT_UID` will need to be updated to match your Notehub project UID.

Modify `fetch_commands` to allow the microcontroller to respond to more commands pushed down from Notehub.

`report_health` can be modified to add more device health details. Check the Blues developer documents for more information about the kinds of health information is already available on the Notecard, e.g. [`card.status`](https://dev.blues.io/api-reference/notecard-api/card-requests/#card-status) and [`card.temp`](https://dev.blues.io/api-reference/notecard-api/card-requests/#card-temp)

`dfu.cpp` was copied from an example provided by Blues. It enables OTA DFU (over-the-air device firmware updates) from Notehub. DFUs use information from `version.cpp` to manage which version of the firmware is currently installed.

### Libraries

 - Arduino IDE v2.3.2
 - Blues Wireless Notecard by Blues v1.5.4: https://github.com/blues/note-arduino