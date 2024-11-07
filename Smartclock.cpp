#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>  // For date and time calculations

// Wi-Fi credentials
const char* ssid = "Tech";
const char* password = "12345678";

// Initialize the LCD (20x4)
LiquidCrystal_I2C lcd(0x27, 20, 4);

// DHT11 Sensor
#define DHTPIN 4  // DHT11 data pin connected to GPIO 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Mutex for LCD access control using Priority Inheritance Protocol (PIP)
SemaphoreHandle_t lcdMutex;

// Task handles
TaskHandle_t tempTaskHandle;
TaskHandle_t displayTaskHandle;

// Variables to hold previous values for comparison
int prevHours = -1, prevMinutes = -1, prevSeconds = -1;
float prevTemp = NAN;

// NTP Client setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000); // UTC timezone, update every 60 seconds

// Time and date variables (renamed to avoid conflict with TimeLib functions)
int currentHours, currentMinutes, currentSeconds;
int currentDay, currentMonth, currentYear;

void setup() {
  // Start serial communication
  Serial.begin(115200);

  // Initialize the LCD
  lcd.begin(20, 4);
  lcd.init();
  lcd.backlight();

  // Initialize the DHT sensor
  dht.begin();

  // Connect to Wi-Fi
  connectToWiFi();

  // Initialize NTP client
  timeClient.begin();

  // Create a mutex for LCD access control
  lcdMutex = xSemaphoreCreateMutex();

  // Create tasks for temperature reading and data display
  xTaskCreate(readTemperature, "TempTask", 2048, NULL, 2, &tempTaskHandle);  // Higher priority for temperature task
  xTaskCreate(displayData, "DisplayTask", 4096, NULL, 1, &displayTaskHandle);
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");
}

void readTemperature(void *parameter) {
  while (true) {
    float temp = dht.readTemperature();
    if (!isnan(temp) && temp != prevTemp) {
      prevTemp = temp;

      // Lock LCD with mutex to prevent data corruption
      if (xSemaphoreTake(lcdMutex, portMAX_DELAY) == pdTRUE) {
        lcd.setCursor(6, 2);
        lcd.print(temp);
        lcd.print(" C  ");
        xSemaphoreGive(lcdMutex);  // Release LCD mutex
      }
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);  // Read temperature every 2 seconds
  }
}

void displayData(void *parameter) {
  while (true) {
    // Update the NTP time
    timeClient.update();

    // Extract time and date
    unsigned long epochTime = timeClient.getEpochTime();

    // Set time and date from epoch
    currentHours = hour(epochTime);
    currentMinutes = minute(epochTime);
    currentSeconds = second(epochTime);
    currentDay = day(epochTime);
    currentMonth = month(epochTime);
    currentYear = year(epochTime);

    // Update the time display if it has changed
    if (currentSeconds != prevSeconds || currentMinutes != prevMinutes || currentHours != prevHours) {
      prevSeconds = currentSeconds;
      prevMinutes = currentMinutes;
      prevHours = currentHours;

      // Lock LCD with mutex to display time
      if (xSemaphoreTake(lcdMutex, portMAX_DELAY) == pdTRUE) {
        lcd.setCursor(6, 0);
        if (currentHours < 10) lcd.print('0');
        lcd.print(currentHours);
        lcd.print(":");
        if (currentMinutes < 10) lcd.print('0');
        lcd.print(currentMinutes);
        lcd.print(":");
        if (currentSeconds < 10) lcd.print('0');
        lcd.print(currentSeconds);
        xSemaphoreGive(lcdMutex);  // Release LCD mutex
      }
    }

    // Display the date
    if (xSemaphoreTake(lcdMutex, portMAX_DELAY) == pdTRUE) {
      lcd.setCursor(6, 1);
      lcd.printf("%02d/%02d/%04d", currentDay, currentMonth, currentYear);
      xSemaphoreGive(lcdMutex);  // Release LCD mutex
    }

    // Display a scrolling message below the temperature
    String message = "Smart Clock!";
    scrollText(message, 3);

    vTaskDelay(500 / portTICK_PERIOD_MS);  // Check for updates twice a second
  }
}

void scrollText(String message, int row) {
  int len = message.length();
  if (len <= 20) {
    if (xSemaphoreTake(lcdMutex, portMAX_DELAY) == pdTRUE) {
      lcd.setCursor(0, row);
      lcd.print(message);
      xSemaphoreGive(lcdMutex);
    }
  } else {
    for (int i = 0; i < len + 20; i++) {
      if (xSemaphoreTake(lcdMutex, portMAX_DELAY) == pdTRUE) {
        lcd.setCursor(0, row);
        String displayText = message.substring(i, min(i + 20, len)) + "                    ";
        lcd.print(displayText.substring(0, 20));  // Ensure only 20 characters are displayed
        xSemaphoreGive(lcdMutex);
      }
      vTaskDelay(100 / portTICK_PERIOD_MS);  // Reduce delay for smoother scrolling
    }
  }
}

void loop() {
  // Empty, as everything is managed by FreeRTOS tasks and timers
}
