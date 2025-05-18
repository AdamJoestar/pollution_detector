#define BLYNK_TEMPLATE_ID "TMPL611PFrx_1"
#define BLYNK_TEMPLATE_NAME "Polusi Udara MQ135"
#define BLYNK_AUTH_TOKEN "U_DqwMZe4RAOIHIJvPngotLezf9wrs7g"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Konfigurasi Blynk // Ganti dengan Auth Token Anda dari Blynk
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Kobokanaeru"; //nama hotspot yang digunakan
char pass[] = "Kobosibocil01"; //password hotspot yang digunakan

// Pin sensor gas (AOUT)
const int GAS_SENSOR_PIN = 34; // GPIO34 (ADC1_CH6)

// Ambang batas polusi (sesuaikan nilai ini setelah kalibrasi sensor)
const int POLUSI_MAX_THRESHOLD = 2000; // Contoh nilai, kalibrasi diperlukan

// Variabel untuk menyimpan nilai sensor
int gasValue = 0;

// Timer untuk mengirim data ke Blynk secara berkala
BlynkTimer timer;

// Flag untuk mencegah notifikasi berulang terus-menerus
bool highPollutionNotified = false;

void setup() {
  Serial.begin(115200);

  // Mulai koneksi Blynk
  Blynk.begin(auth, ssid, pass);

  // Konfigurasi timer untuk membaca sensor dan mengirim data setiap 3 detik
  timer.setInterval(3000L, sendSensorData);

  Serial.println("Koneksi ke Blynk...");
}

void loop() {
  Blynk.run();
  timer.run();
}

void sendSensorData() {
  gasValue = analogRead(GAS_SENSOR_PIN); // Baca nilai analog dari sensor

  Serial.print("Nilai Sensor Gas (ADC): ");
  Serial.println(gasValue);

  // Kirim nilai sensor ke Blynk Virtual Pin V0
  Blynk.virtualWrite(V0, gasValue);

  // Cek jika polusi mencapai batas maksimal
  if (gasValue >= POLUSI_MAX_THRESHOLD) {
    if (!highPollutionNotified) { // Kirim notifikasi hanya sekali saat ambang batas tercapai
      Blynk.logEvent("PERINGATAN! Polusi udara tinggi. Jangan keluar ruangan.");
      Serial.println("Notifikasi: Polusi tinggi!");
      highPollutionNotified = true; // Set flag agar tidak notifikasi lagi
    }
  } else {
    // Reset flag ketika polusi kembali normal
    highPollutionNotified = false;
  }
}
