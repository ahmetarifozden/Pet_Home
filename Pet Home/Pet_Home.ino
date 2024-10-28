#include <HX711.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "esp_camera.h"

// ====================
// Kullanıcı Ayarları
// ====================
#define WIFI_SSID                     "iuc.misafir"
#define WIFI_PASSWORD                 "iuc.1453"
#define API_ENDPOINT                  "http://your_api_address_here.com/upload"

// Ağırlık sensörü ve su sensörü pinleri
#define WATER_SENSOR_PIN              A3
#define WEIGHT_SENSOR_FIRST_PIN       A1
#define WEIGHT_SENSOR_SECOND_PIN      A0

// Kamera ayarları
#define CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

HX711 scale;
int calibration_factor = -19750; // Kalibrasyon değeri
double water_level = 0;
double food_level = 0;

// ====================
// Yardımcı Fonksiyonlar
// ====================
void connect_to_wifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Bağlanıyor...");
  }
  Serial.println("WiFi bağlantısı başarılı.");
}

double get_water_level(int pin) {
  int raw_value = analogRead(pin);
  raw_value = constrain(raw_value, 0, 650);
  return map(raw_value, 0, 650, 0, 100) / 100.0;
}

double get_food_level() {
  double raw_weight = -scale.get_units(5);
  return constrain(raw_weight, 0, 100) / 100.0;
}

bool send_data(const String& imageData) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(API_ENDPOINT);
    http.addHeader("Content-Type", "application/json");

    String postData = "{\"water_level\":" + String(water_level) + ",\"food_level\":" + String(food_level) + ",\"image_data\":\"" + imageData + "\"}";

    int httpResponseCode = http.POST(postData);
    http.end();

    return (httpResponseCode > 0);
  } else {
    return false;
  }
}

String capture_image() {
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Kamera başarısız");
    return "";
  }

  String imageData = "";
  for (size_t i = 0; i < fb->len; i++) {
    imageData += String(fb->buf[i], HEX);
  }

  esp_camera_fb_return(fb);
  return imageData;
}

// ====================
// Kurulum
// ====================
void setup() {
  Serial.begin(115200);
  connect_to_wifi();
  
  // Ağırlık sensörü kurulumu
  scale.begin(WEIGHT_SENSOR_FIRST_PIN, WEIGHT_SENSOR_SECOND_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();

  // Kamera kurulumu
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_CIF;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Kamera başlatılamadı (hata 0x%x)", err);
  }
}

// ====================
// Döngü
// ====================
void loop() {
  water_level = get_water_level(WATER_SENSOR_PIN);
  food_level = get_food_level();
  String imageData = capture_image();

  if (send_data(imageData)) {
    Serial.println("Veri gönderildi.");
  } else {
    Serial.println("Veri gönderilemedi.");
  }

  delay(5000); // Her 5 saniyede bir veri gönderme
}
