#define BLYNK_TEMPLATE_ID "TMPL611PFrx_1"
#define BLYNK_TEMPLATE_NAME "Polusi Udara MQ135"
#define BLYNK_AUTH_TOKEN "U_DqwMZe4RAOIHIJvPngotLezf9wrs7g"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Konfigurasi Blynk // Ganti dengan Auth Token Anda dari Blynk
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Wokwi-GUEST"; //nama hotspot yang digunakan
char pass[] = ""; //password hotspot yang digunakan

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

void scanNetworks() {
  Serial.println("Memulai scan jaringan WiFi...");
  int n = WiFi.scanNetworks();
  Serial.println("Scan selesai.");
  if (n == 0) {
    Serial.println("Tidak ditemukan jaringan WiFi.");
  } else {
    Serial.print(n);
    Serial.println(" jaringan WiFi ditemukan:");
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (RSSI: ");
      Serial.print(WiFi.RSSI(i));
      Serial.print(" dBm)");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " Terbuka" : " Terenkripsi");
      delay(10);
    }
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);

  scanNetworks();

  Serial.print("Menghubungkan ke SSID: ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  int max_attempts = 20;
  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < max_attempts) {
    delay(500);
    Serial.print(".");
    attempt++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Terhubung ke WiFi!");
    Serial.print("Alamat IP: ");
    Serial.println(WiFi.localIP());

    // Mulai koneksi Blynk setelah WiFi terhubung
    Blynk.begin(auth, ssid, pass);
  } else {
    Serial.println("Gagal terhubung ke WiFi.");
  }

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
      Blynk.logEvent("bahaya", "Gas level tinggi!");
      Serial.println("Notifikasi: Bahaya untuk keluar!");
      highPollutionNotified = true; // Set flag agar tidak notifikasi lagi
    }
  } else {
    if (highPollutionNotified) { // Kirim notifikasi aman hanya sekali saat polusi turun di bawah threshold
      Blynk.logEvent("aman", "Gas level normal");
      Serial.println("Notifikasi: Aman untuk keluar rumah");
      highPollutionNotified = false; // Reset flag
    }
  }
}
