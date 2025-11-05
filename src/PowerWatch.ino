#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#define BUZZER1_PIN D5
#define BUZZER2_PIN D6
#define RED_PIN     D1
#define GREEN_PIN   D2
#define BLUE_PIN    D7
#define OFF_PIN     D0


const char* SSID     = "EXAMPLE_WIFI";
const char* PASSWORD = "EXAMPLE_WIFI_PASSWORD";

String NOTIFY_MESSAGE = "⚡ Електрохарчування навалили ! Час Гунити 🚨!!!";

String BOT_TOKEN = "EXAMPLE_BOT_TOKEN";
String CHAT_ID   = "-EXAMPLE_CHAT_ID";

WiFiClientSecure client;

bool messageSent = false;
unsigned long lastAttempt = 0;
unsigned long attemptInterval = 5000;
const unsigned long maxAttemptInterval = 60000;

unsigned long lastWiFiCheck = 0;
const unsigned long wifiCheckInterval = 30000;

int melody[] = {
  400, 800, 1200, 1000, 600, 1400, 900, 500,
  1300, 700, 1100, 450, 1500, 600, 1000, 800
};
const int melodyLength = sizeof(melody) / sizeof(melody[0]);

int colors[16][3] = {
  {255,   0,   0}, {0,   255,   0}, {0,     0, 255}, {255, 255,   0},
  {0,   255, 255}, {255,   0, 255}, {255, 128,   0}, {128,   0, 255},
  {255, 255, 255}, {128, 128, 128}, {64,  255,  64}, {255,  64, 128},
  {64,   64, 255}, {192, 192,   0}, {0,   128, 192}, {32,   0,  128}
};

int colorIndex = 0;
int melodyIndex = 0;

unsigned long lastMelody = 0;
unsigned long lastColor = 0;
const unsigned long melodyInterval = 150; // sound speed
const unsigned long colorInterval  = 200; // color change speed


void setColor(int r, int g, int b) {
  analogWrite(RED_PIN,   255 - r);
  analogWrite(GREEN_PIN, 255 - g);
  analogWrite(BLUE_PIN,  255 - b);
}

bool sendTelegramMessage(String text) {
  if (!client.connect("api.telegram.org", 443)) return false;
  String url = "/bot" + BOT_TOKEN + "/sendMessage?chat_id=" + CHAT_ID + "&text=" + text;
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: api.telegram.org\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      client.stop();
      return false;
    }
  }
  while (client.available()) {
    String line = client.readStringUntil('\n');
    if (line.indexOf("\"ok\":true") != -1) return true;
  }
  return false;
}

void off(){
  messageSent = false;
  digitalWrite(BUZZER1_PIN, LOW);
  digitalWrite(BUZZER2_PIN, LOW);

  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
}

// --- setup ---
void setup() {
  Serial.begin(115200);
  pinMode(BUZZER1_PIN, OUTPUT);
  pinMode(BUZZER2_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(OFF_PIN, INPUT);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  client.setInsecure();
}

// --- loop ---
void loop() {
  unsigned long now = millis();
  unsigned int offState = digitalRead(OFF_PIN);

  if(!offState) {
    // sound
    if (now - lastMelody >= melodyInterval) {
      lastMelody = now;
      int f1 = melody[melodyIndex];
      int f2 = melody[(melodyIndex + 8) % melodyLength];
      tone(BUZZER1_PIN, f1);
      tone(BUZZER2_PIN, f2);
      melodyIndex = (melodyIndex + 1) % melodyLength;
    }

    // color
    if (now - lastColor >= colorInterval) {
      lastColor = now;
      setColor(colors[colorIndex][0], colors[colorIndex][1], colors[colorIndex][2]);
      colorIndex = (colorIndex + 1) % 16;
    }

    // Wi-Fi
    if (WiFi.status() != WL_CONNECTED) {
      if (now - lastWiFiCheck > wifiCheckInterval) {
        lastWiFiCheck = now;
        WiFi.disconnect();
        WiFi.begin(SSID, PASSWORD);
      }
    } else {
      if (!messageSent && now - lastAttempt > attemptInterval) {
        lastAttempt = now;
        if (sendTelegramMessage()) {
          messageSent = true;
        } else {
          attemptInterval = min(attemptInterval * 2UL, maxAttemptInterval);
        }
      }
    }
  } else {
    off();
  }
}
