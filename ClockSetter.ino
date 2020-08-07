//adapted from RTClib ds3231 example by adafruit

/*
Published by Miles Nash on 12.30.2018
project: "countdown-clock"
file: "Accurate RTC Clock setter"
github: miles-nash
https://github.com/miles-nash/countdown-clock/new/master

License:
Attribution-NonCommercial-ShareAlike 
CC BY-NC-SA
Others may remix,tweak and build upon this work non-commercially,
as long as they credit me aND liscence their creations under these terms

*/
//THIS LINE AND ALL LINES ABOVE MUST BE INCLUDED IN REMIXED, TWEAKED, OR BUILT UPON CODE


#include <Wire.h>
#include "RTClib.h"
RTC_DS3231 rtc;
#include <TimeLib.h>

void setup() {
  Serial.begin(115200);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__) ) + TimeSpan(28));
  //adjust the TimeSpan number and keep re uploading until the Serial reported time matches the current time;
  //it is very important to get this exact as seconds are important to a countdown clock
}

void loop() {

  DateTime now = rtc.now();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");

    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    delay(100);
}
