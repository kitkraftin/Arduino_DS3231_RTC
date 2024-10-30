#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <math.h>  // For trigonometric functions

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Create an RTC object
RTC_DS3231 rtc;

const int CLOCK_RADIUS = 30;
const int CLOCK_CENTER_X = SCREEN_WIDTH / 2;
const int CLOCK_CENTER_Y = SCREEN_HEIGHT / 2;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED allocation failed"));
    for (;;);
  }
  
  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
  // Check if RTC lost power and set time to compile time if necessary
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  display.clearDisplay();
  display.display();
}

void loop() {
  DateTime now = rtc.now();
  
  // Draw the analog clock
  drawClockFace();
  drawHands(now.hour(), now.minute(), now.second());
  
  delay(1000);  // Update every second
}

// Function to draw the clock face
void drawClockFace() {
  display.clearDisplay();
  
  // Draw clock circle
  display.drawCircle(CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_RADIUS, WHITE);
  
  // Draw tick marks for hours
  for (int i = 0; i < 12; i++) {
    float angle = (i * 30) * (PI / 180);  // 30 degrees per hour tick
    int x1 = CLOCK_CENTER_X + (CLOCK_RADIUS - 2) * cos(angle);
    int y1 = CLOCK_CENTER_Y + (CLOCK_RADIUS - 2) * sin(angle);
    int x2 = CLOCK_CENTER_X + CLOCK_RADIUS * cos(angle);
    int y2 = CLOCK_CENTER_Y + CLOCK_RADIUS * sin(angle);
    display.drawLine(x1, y1, x2, y2, WHITE);
  }

  display.display();
}

// Function to draw clock hands
void drawHands(int hours, int minutes, int seconds) {
  // Convert hours, minutes, and seconds to angles in radians
  float second_angle = (seconds * 6) * (PI / 180);  // 360° / 60 seconds = 6° per second
  float minute_angle = (minutes * 6) * (PI / 180);  // 360° / 60 minutes = 6° per minute
  float hour_angle = ((hours % 12) * 30 + minutes * 0.5) * (PI / 180);  // 360° / 12 hours = 30° per hour

  // Draw second hand (longest, red if color supported)
  drawHand(second_angle, CLOCK_RADIUS - 2, WHITE);
  
  // Draw minute hand (medium length)
  drawHand(minute_angle, CLOCK_RADIUS - 4, WHITE);
  
  // Draw hour hand (shortest)
  drawHand(hour_angle, CLOCK_RADIUS - 8, WHITE);

  display.display();
}

// Function to draw individual clock hands
void drawHand(float angle, int length, int color) {
  int x2 = CLOCK_CENTER_X + length * cos(angle - PI / 2);  // -PI/2 to start at top of clock
  int y2 = CLOCK_CENTER_Y + length * sin(angle - PI / 2);
  display.drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, x2, y2, color);
}
