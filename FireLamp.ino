// ---------- МАТРИЦА ---------
#define BRIGHTNESS 10         // Startup brightness (0-255)
#define CURRENT_LIMIT 2000    // лимит по току в миллиамперах, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит
#define VOLT_LIMIT 5          // лимит по напряжению

#define WIDTH 16              // ширина матрицы
#define HEIGHT 16             // высота матрицы

#define COLOR_ORDER GRB       // порядок цветов на ленте. Если цвет отображается некорректно - меняйте. Начать можно с RGB

#define MATRIX_TYPE 0         // тип матрицы: 0 - зигзаг, 1 - параллельная
#define CONNECTION_ANGLE 0    // угол подключения: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
#define STRIP_DIRECTION 0     // направление ленты из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
// при неправильной настройке матрицы вы получите предупреждение "Wrong matrix parameters! Set to default"

// --------- ESP --------
byte IP_STA[] = {192, 168, 1, 66};  // статический IP локальный (менять только последнюю цифру)

// -------- Менеджер WiFi ---------
#define AC_SSID "FireLampAP"
#define AC_PASS "12345678"
#define LOCAL_PORT 8888

// ============= ДЛЯ РАЗРАБОТЧИКОВ =============
#define LED_PIN 2             // пин ленты
#define BTN_PIN 4

#define NUM_LEDS WIDTH * HEIGHT
#define MIDDLE_LED NUM_LEDS / 2
#define SEGMENTS 1            // диодов в одном "пикселе" (для создания матрицы из кусков ленты)
// ---------------- БИБЛИОТЕКИ -----------------
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_ESP8266_RAW_PIN_ORDER

#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <GyverButton.h>
#include <Ticker.h>

// ------------------- ТИПЫ --------------------
CRGB leds[NUM_LEDS];
WiFiServer server(80);
WiFiUDP Udp;
Ticker ticker;
GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);

// ----------------- ПЕРЕМЕННЫЕ ------------------
const char* autoConnectSSID = AC_SSID;
const char* autoConnectPass = AC_PASS;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1]; //buffer to hold incoming packet
String inputBuffer;
static const byte maxDim = max(WIDTH, HEIGHT);
byte brightness[2];

byte currentMode = 0;
boolean ONflag = true;
uint32_t eepromTimer;
boolean settChanged = false;

unsigned char matrixValue[8][16];

void setup() {
  Serial.begin(115200);
  Serial.println();

  ESP.wdtDisable();

  // ЛЕНТА
  Serial.println("Init LED matrix...");
  FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(VOLT_LIMIT, CURRENT_LIMIT);
  FastLED.clear();
  FastLED.show();

  // Start progress bar
  ticker.attach(0.03, progressTick);

  touch.setStepTimeout(100);
  touch.setClickTimeout(500);

  // WI-FI
  Serial.println("Init WiFi...");
  WiFiManager wifiManager;
  wifiManager.setDebugOutput(false);

  wifiManager.autoConnect(autoConnectSSID, autoConnectPass);
  WiFi.config(IPAddress(IP_STA[0], IP_STA[1], IP_STA[2], IP_STA[3]),
              IPAddress(192, 168, 1, 1),
              IPAddress(255, 255, 255, 0));
  
  // UDP
  Serial.println("Init UDP listener...");
  Udp.begin(LOCAL_PORT);

  // EEPROM
  Serial.println("Init EEPROM...");
  initStorage();
  if (isFirstRun()) saveFirstRun();

  brightness[0] = readModeBrightness(0);
  brightness[1] = readModeBrightness(1);

  currentMode = readCurrentMode();
  FastLED.setBrightness(brightness[currentMode]);

  // Init randomizer 
  randomSeed(micros());

  // Configuration info
  Serial.println("-= Current settings =-");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("UDP port: %d\n", LOCAL_PORT);
  Serial.printf("Current mode: %d\n", currentMode/* ? "Fire" : "Light"*/);
  Serial.printf("Light brightness: %d\n", brightness[0]);
  Serial.printf("Fire brightness: %d\n", brightness[1]);

  // Turn off progress bar
  ticker.detach();
}

void loop() {
  parseUDP();
  buttonTick();
  effectsTick();
  eepromTick();
  ESP.wdtFeed();   // пнуть собаку
  yield();
}
