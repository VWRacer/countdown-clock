//uses code from adafruit Scrolling text example to bounce balls
/*
Published by Miles Nash on 12.30.2018
project: "countdown-clock"
file: "Arduino code"
github: miles-nash
https://github.com/miles-nash/countdown-clock/new/master

Modified by Jim Leether on 08.07.2020
project: "countdown-clock"
file: "NewYearsCountdown.ino"
github: VWRacer
https://github.com/VWRacer/countdown-clock

License:
Attribution-NonCommercial-ShareAlike 
CC BY-NC-SA
Others may remix,tweak and build upon this work non-commercially,
as long as they credit me aND liscence their creations under these terms

*/
//THIS LINE AND ALL LINES ABOVE MUST BE INCLUDED IN REMIXED, TWEAKED, OR BUILT UPON CODE


//import TimeLib.h and RTClib.h and AdafruitGFX libraries



#include <RGBmatrixPanel.h>
#include <Wire.h>
#include "RTClib.h"
RTC_DS3231 rtc;
#include <TimeLib.h>

#define dropMotor 4
#define eventLights 5
#define confettiBlower 6
#define spareRelay 7
#define CLK  8   
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2

////
//date and time of the event being counted to
int countHour = 0; //(0-23)
int countDay = 1; //(1 - 30ish)
int countMonth = 1; //(1- 12)
int countYear = 2020; // the date limited to being less than a year (364 days) away

//the message displayed once the event occurs:
String messageLine1 = "Happy"; //maximum of 5 characters
String messageLine2 = "2019"; //maximum of 4 characters

int RGBPulseSpeed = 1; //speed at which the text color will change. Keep below 10 to start
///

bool over;
int daysLeft;
int hoursLeft;
int minutesLeft;
int secondsLeft;
int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};

RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);
int hue;
int8_t ball[3][4] = {
  {  3,  0,  1,  1 }, // Initial X,Y pos & velocity for 3 bouncy balls
  { 17, 15,  1, -1 },
  { 27,  4, -1,  1 }
};
static const uint16_t PROGMEM ballcolor[3] = {
  0x0080, // Green=1
  0x0002, // Blue=1
  0x1000  // Red=1
};

int currentNum;
int countDownNum;


void setup() {
  //set pin mode for relay pins
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  
  Serial.begin(9600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  //generic setup
  over == false;
  hue = 0;
  currentNum = 20;
  matrix.begin(); 
  
  //set the date and time based off of the RTC
  DateTime now = rtc.now();
  setTime(now.unixtime());
}

void loop() {
  matrix.setTextColor(matrix.ColorHSV(hue, 255, 255, true));
  
  int lastSecondsLeft = secondsLeft;
  int lastMinutesLeft = minutesLeft;
  int lastHoursLeft = hoursLeft;
  int lastDaysLeft = daysLeft;
  countdownClock(countHour, countDay, countMonth, countYear); //update countdown
  //hour(0-23),day, month(1-12), year

  //clear the display when a digit being displayed changes
  if(hoursLeft < 1 && lastSecondsLeft != secondsLeft){
    matrix.fillScreen(0);
  }else if(daysLeft < 1 && lastMinutesLeft!= minutesLeft){
    matrix.fillScreen(0);
  }else if(lastHoursLeft != hoursLeft){
    matrix.fillScreen(0);
  }else if(lastDaysLeft != daysLeft){
    matrix.fillScreen(0);
  }

  //start ball drop motor at appropriate time
  if(secondsLeft == 90){
    digitalWrite(4, HIGH); //turns on motor
  }

  //start confetti blower at 1 second to give it time to spin up
  if(secondsLeft == 1){
    digitalWrite(6, HIGH); //turns on blower
  }

  if(secondsLeft == -1){
    //stop ball drop motor
    digitalWrite(4, LOW);
    //turn on additional even lighting
    digitalWrite(5, HIGH);
    //event has occured, display message and RGB animation
    bounceBalls();
    matrix.setCursor(1,0);
    matrix.setTextSize(1);
    matrix.setTextColor(matrix.Color333(7,7,7));
    matrix.print(messageLine1);
    matrix.setCursor(5,9);
    matrix.print(messageLine2);
    delay(3);
  }else if(daysLeft > 0 && daysLeft < 364){
    //day and hour display
    matrix.setTextSize(1);
    if(daysLeft < 10){
      matrix.setCursor(4,1);
    }else{
      matrix.setCursor(1,0);
    }
    matrix.print(daysLeft);
    matrix.setCursor(14,1);
    matrix.print("day");

    if(hoursLeft < 10){
      matrix.setCursor(4,9);
    }else{
      matrix.setCursor(1,9);
    }

    matrix.print(hoursLeft);
    matrix.setCursor(14,9);
    matrix.print("hrs");


    
  }else if(hoursLeft > 0){
    //hour and min display
    matrix.setTextSize(1);
    if(hoursLeft < 10){
      matrix.setCursor(4,1);
    }else{
      matrix.setCursor(1,0);
    }
    matrix.print(hoursLeft);
    matrix.setCursor(14,1);
    matrix.print("hrs");

    if(minutesLeft < 10){
      matrix.setCursor(4,9);
    }else{
      matrix.setCursor(1,9);
    }

    matrix.print(minutesLeft);
    matrix.setCursor(14,9);
    matrix.print("min");
    
  }else if(minutesLeft > 0){
    //min and sec display
    matrix.setTextSize(1);
    if(minutesLeft < 10){
      matrix.setCursor(4,1);
    }else{
      matrix.setCursor(1,0);
    }
    matrix.print(minutesLeft);
    matrix.setCursor(14,1);
    matrix.print("min");

    if(secondsLeft < 10){
      matrix.setCursor(4,9);
    }else{
      matrix.setCursor(1,9);
    }

    matrix.print(secondsLeft);
    matrix.setCursor(14,9);
    matrix.print("sec");
    
  }else if(secondsLeft < 10 && secondsLeft > 0){

    matrix.setCursor(11,1);
    matrix.setTextSize(2);
    matrix.print(secondsLeft);
      
  }  else if(secondsLeft < 60 && secondsLeft > 9){
    
    matrix.setCursor(4,1);
    matrix.setTextSize(2);
    matrix.print(secondsLeft);

  
  } else if(secondsLeft < -1){
    bounceBalls();
  }
  

  
  hue += RGBPulseSpeed; //adjust to increase/decrease speed of pulsing
  
  if(hue >= 1536) hue -= 1536;
}

void bounceBalls(){
  //matrix.fillScreen(0);
  byte i;
  for(i=0; i<3; i++) {
    // Draw 'ball'
    matrix.fillCircle(ball[i][0], ball[i][1], 5, pgm_read_word(&ballcolor[i]));
    // Update X, Y position
    ball[i][0] += ball[i][2];
    ball[i][1] += ball[i][3];
    // Bounce off edges
    if((ball[i][0] == 0) || (ball[i][0] == (matrix.width() - 1)))
      ball[i][2] *= -1;
    if((ball[i][1] == 0) || (ball[i][1] == (matrix.height() - 1)))
      ball[i][3] *= -1;
  }
}

void countdownClock(int h, int m, int d, int y){
  //takes in hour(24 hour time), month, date, and year
  //displays time left until that date
  int currentDate = totalDate(month(),day(),year());
  int countdownDate = totalDate(m, d, y);
  daysLeft = countdownDate - currentDate -1;
  

  if (hour() >= h){
    hoursLeft = 23 + h - hour() ;
  }else{
    hoursLeft = h - hour() - 1;
  }

  minutesLeft = 59 - minute();
  secondsLeft = 59 - second();

  if((hoursLeft < 0 || daysLeft < 0) || (daysLeft == 0 && hoursLeft == 0 && minutesLeft == 0 && secondsLeft == 0)){
    //return negative seconds
    Serial.println("event has occured");
    secondsLeft = -1;

  }
  
  Serial.println(String(daysLeft) + "." + String(hoursLeft) + "." + String(minutesLeft) + "." + String(secondsLeft));

}

int totalDate(int m, int d, int y){
  //takes in a month date and year
  //calculates and returns the number of days since the December 31 1999
  //1/1/2000 = 1
 int days = 0;
 for(int monthID = 1; monthID < m; monthID++){
   days += daysInMonth[monthID -1];
 }
 days += d;
 days += (y - 2000)*365;
 return days;
}
