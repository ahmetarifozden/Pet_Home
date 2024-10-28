# İÜC Karmaşık Mühendislik Birimi - IoT Tabanlı Veri İzleme ve Gönderim Sistemi

Bu proje, **İstanbul Üniversitesi-Cerrahpaşa Karmaşık Mühendislik Birimi** tarafından geliştirilmiş olup, IoT ve gömülü sistemler tabanlı bir çözüm sunmaktadır. Sistem; ağırlık sensörü, su seviye sensörü ve kamera kullanarak çevresel verileri toplar ve bunları bir API sunucusuna belirli aralıklarla iletir. Bu proje, hem evcil hayvanların mama ve su takibini sağlamak hem de benzer izleme sistemlerinde veri kaynağı oluşturmak için kullanılabilir.

## Proje Özeti

Sistem, ağırlık, su seviyesi ve görüntü verilerini toplayarak kullanıcıya düzenli aralıklarla veri iletimi sağlar:
- **Ağırlık Sensörü**: Evcil hayvan mama seviyesini ölçer ve günceller.
- **Su Seviye Sensörü**: Su kabındaki seviyeyi takip eder.
- **Kamera Modülü**: Ortamın anlık görüntüsünü yakalayarak veri gönderimine katkı sağlar.
- **API Entegrasyonu**: Tüm veriler, bir API adresine belirli aralıklarla gönderilerek kullanıcıya gerçek zamanlı izleme imkanı sunar.

## Donanım Gereksinimleri

1. **ESP32 veya ESP8266** - WiFi bağlantısı ve veri işleme için.
2. **HX711 Ağırlık Sensörü ve Load Cell** - Mama ağırlığını ölçmek için.
3. **Su Seviye Sensörü** (analog) - Su miktarını tespit etmek için.
4. **ESP32-CAM Modülü** (veya Arduino uyumlu bir kamera modülü) - Görüntü alma ve API'ye gönderme için.

## Yazılım Gereksinimleri

- Arduino IDE (ESP32/ESP8266 için ek kütüphaneler kurulmalıdır)
- Gerekli Kütüphaneler:
  - `WiFi.h` (ESP32 için dahili)
  - `HX711.h` - Ağırlık sensörü için
  - `ESP32-CAM` kütüphanesi - Kamera modülü için

## Devre Bağlantıları

- **Ağırlık Sensörü** - HX711 modülü üzerinden ESP32'ye bağlanır.
- **Su Seviye Sensörü** - Analog pin aracılığıyla ESP32’ye bağlanır.
- **Kamera Modülü** - ESP32-CAM üzerinden bağlantı kurulur.

> **Not**: Bağlantı detayları için sensörlerin pin yapısına ve ESP32'nin bağlantı şemalarına başvurulabilir.

## Kurulum ve Kullanım

1. Arduino IDE üzerinden gerekli kütüphaneleri kurun.
2. ESP32 veya ESP8266'yı seçerek doğru kart ayarlarını yapın.
3. `config.h` dosyasında WiFi bilgilerinizi ve API adresini düzenleyin.
4. Sensörleri ve kamera modülünü ESP32'ye bağlayın.
5. Kodunuzu ESP32'ye yükleyin ve seri monitör üzerinden verilerin gönderimini gözlemleyin.

## Kod Örneği

```cpp
#include <WiFi.h>
#include <HX711.h>
#include <ESP32CAM.h>

// Kullanıcı Tanımlı API ve WiFi Bilgileri
#define WIFI_SSID "iuc.misafir"
#define WIFI_PASSWORD "iuc.1453"
#define API_URL "http://example-api.com/data"

// Sensör Pinleri
#define WATER_SENSOR_PIN A3
#define WEIGHT_SENSOR_DOUT A1
#define WEIGHT_SENSOR_CLK A0

HX711 scale;
ESP32CAM camera;

void setup() {
  Serial.begin(115200);
  connectWiFi();
  setupSensors();
  camera.begin();
}

void loop() {
  float waterLevel = readWaterSensor();
  float weight = readWeightSensor();
  String imageData = camera.captureImage();
  
  sendDataToServer(waterLevel, weight, imageData);
  delay(60000); // 60 saniye aralıklarla gönderim
}

// Diğer fonksiyonlar burada...
