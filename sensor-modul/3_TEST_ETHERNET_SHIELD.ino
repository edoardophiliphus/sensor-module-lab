// ============================================================
//  TEST #3 — ETHERNET SHIELD (W5100 / W5500)
//  Tujuan  : Memastikan Ethernet Shield terkoneksi ke jaringan
//            dan bisa komunikasi HTTP ke server
//
//  KONEKSI HARDWARE:
//  - Pasang Ethernet Shield langsung di atas Arduino Mega
//  - Colokkan kabel LAN dari Shield ke switch/router
//  - Shield pakai SPI: otomatis pakai pin 50(MISO),51(MOSI),
//    52(SCK),53(SS) di Arduino Mega
//
//  Library: Ethernet (bawaan Arduino IDE, tidak perlu install)
//
//  CARA TES:
//  1. Sesuaikan konfigurasi IP di bawah dengan jaringan kamu
//  2. Upload sketch, buka Serial Monitor (baud 9600)
//  3. Perhatikan output: IP, ping ke server, HTTP GET
//  4. Jika berhasil, MAC Address dan IP akan tampil
// ============================================================

#include <SPI.h>
#include <Ethernet.h>

// ─── KONFIGURASI JARINGAN ──────────────────────────────────
// MAC Address: harus unik di jaringan kamu
// Ubah nilai ini jika ada konflik dengan perangkat lain
byte mac[] = { 0xC2, 0x16, 0x95, 0xF4, 0x3C, 0xC1 };

// Pilih Mode IP:
//   true  = DHCP (otomatis dari router) — lebih mudah
//   false = Static (manual) — lebih stabil untuk produksi
const bool USE_DHCP = true;

// Jika USE_DHCP = false, isi IP manual di bawah ini:
IPAddress staticIP(10, 213, 2, 50);    // IP Arduino
IPAddress gateway(10, 213, 2, 1);     // Gateway / Router
IPAddress subnet(255, 255, 255, 0);   // Subnet mask
IPAddress dns(8, 8, 8, 8);            // DNS (Google)

// ─── TARGET SERVER UNTUK TES HTTP ─────────────────────────
// Ganti dengan IP server yang ada di jaringan kamu
const IPAddress serverIP(10, 213, 2, 17);
const char serverHost[] = "10.213.2.17";
const int  serverPort   = 80;
// Path yang mau di-GET (harus ada di server, atau pakai "/")
const char testPath[]   = "/";

// ─── OBJEK CLIENT ─────────────────────────────────────────
EthernetClient client;

// Timer tes berulang
unsigned long lastTestTime = 0;
const unsigned long TEST_INTERVAL = 10000; // tes setiap 10 detik

// ──────────────────────────────────────────────────────────

void setup() {
  Serial.begin(9600);
  SPI.begin(); // Wajib untuk Ethernet Shield (pakai bus SPI)

  Serial.println(F("=============================="));
  Serial.println(F("  TEST ETHERNET SHIELD AKTIF"));
  Serial.println(F("=============================="));

  // ─── Inisialisasi Ethernet ───────────────────────────────
  Serial.print(F("[ETH] Mode: "));

  if (USE_DHCP) {
    Serial.println(F("DHCP (otomatis)"));
    Serial.println(F("[ETH] Meminta IP dari router..."));

    if (Ethernet.begin(mac) == 0) {
      // DHCP gagal — coba diagnosa
      Serial.println(F("[ETH] DHCP GAGAL!"));
      if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println(F("[ETH] Shield tidak terdeteksi!"));
        Serial.println(F("      Cek apakah Shield terpasang dengan benar."));
      } else if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println(F("[ETH] Kabel LAN tidak tersambung!"));
        Serial.println(F("      Colokkan kabel LAN ke Shield."));
      }
      // Berhenti di sini jika gagal total
      while (true) { delay(1000); }
    }
  } else {
    // Static IP
    Serial.println(F("Static IP (manual)"));
    Ethernet.begin(mac, staticIP, dns, gateway, subnet);
  }

  // ─── Tampilkan info koneksi ───────────────────────────────
  delay(500); // beri waktu shield siap
  Serial.println(F("[ETH] Ethernet berhasil diinisialisasi!"));
  Serial.print(F("[ETH] IP Address  : "));
  Serial.println(Ethernet.localIP());
  Serial.print(F("[ETH] Subnet Mask : "));
  Serial.println(Ethernet.subnetMask());
  Serial.print(F("[ETH] Gateway     : "));
  Serial.println(Ethernet.gatewayIP());
  Serial.print(F("[ETH] DNS Server  : "));
  Serial.println(Ethernet.dnsServerIP());

  // ─── Cek status link fisik ───────────────────────────────
  if (Ethernet.linkStatus() == LinkON) {
    Serial.println(F("[ETH] Status Link : TERHUBUNG (kabel OK)"));
  } else {
    Serial.println(F("[ETH] Status Link : TIDAK TERHUBUNG"));
    Serial.println(F("      Periksa kabel LAN atau port switch."));
  }

  Serial.println();
  Serial.println(F("[INFO] Akan tes HTTP GET setiap 10 detik..."));
  Serial.println(F("=============================="));

  // Langsung tes pertama kali
  tesHTTPGet();
}

void loop() {
  // Perpanjang DHCP lease jika sudah mau habis
  Ethernet.maintain();

  // Jalankan tes HTTP setiap interval
  if (millis() - lastTestTime >= TEST_INTERVAL) {
    lastTestTime = millis();
    tesHTTPGet();
  }
}

// ─── Fungsi tes HTTP GET ──────────────────────────────────
void tesHTTPGet() {
  Serial.println();
  Serial.println(F("--- Tes HTTP GET ---"));
  Serial.print(F("[HTTP] Mencoba koneksi ke "));
  Serial.print(serverHost);
  Serial.print(F(":"));
  Serial.println(serverPort);

  // Cek link fisik dulu sebelum connect
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println(F("[HTTP] Gagal: Kabel LAN tidak tersambung!"));
    return;
  }

  if (client.connect(serverIP, serverPort)) {
    Serial.println(F("[HTTP] Koneksi ke server BERHASIL!"));

    // Kirim HTTP GET request sederhana
    client.println("GET " + String(testPath) + " HTTP/1.1");
    client.println("Host: " + String(serverHost));
    client.println("Connection: close");
    client.println(); // baris kosong = akhir header HTTP

    // Tunggu dan baca respons dari server
    unsigned long timeout = millis();
    String responseLine = "";
    bool headerReceived = false;

    Serial.println(F("[HTTP] Respons dari server:"));
    Serial.println(F("... (5 baris pertama) ..."));

    int lineCount = 0;

    while (client.connected() && millis() - timeout < 5000) {
      if (client.available()) {
        char c = client.read();

        if (c == '\n') {
          // Hanya tampilkan 5 baris pertama header
          if (lineCount < 5) {
            Serial.println("  " + responseLine);
            lineCount++;
          } else if (!headerReceived) {
            Serial.println(F("  ... (sisa response tidak ditampilkan)"));
            headerReceived = true;
          }
          responseLine = "";
        } else if (c != '\r') {
          responseLine += c;
        }
      }
    }

    client.stop();
    Serial.println(F("[HTTP] Koneksi ditutup."));
    Serial.println(F("[HASIL] TES ETHERNET SUKSES ✓"));

  } else {
    Serial.println(F("[HTTP] GAGAL terhubung ke server!"));
    Serial.println(F("[COBA] Periksa:"));
    Serial.println(F("  1. IP server sudah benar?"));
    Serial.println(F("  2. Server menyala dan bisa diakses?"));
    Serial.println(F("  3. Port 80 tidak diblokir firewall?"));
    Serial.println(F("  4. Arduino & server di subnet yang sama?"));
  }
}

// ============================================================
//  HASIL YANG DIHARAPKAN:
//
//  ==============================
//    TEST ETHERNET SHIELD AKTIF
//  ==============================
//  [ETH] Mode: DHCP (otomatis)
//  [ETH] Meminta IP dari router...
//  [ETH] Ethernet berhasil diinisialisasi!
//  [ETH] IP Address  : 10.213.2.50
//  [ETH] Status Link : TERHUBUNG (kabel OK)
//
//  --- Tes HTTP GET ---
//  [HTTP] Koneksi ke server BERHASIL!
//  [HTTP] Respons dari server:
//    HTTP/1.1 200 OK
//    ...
//  [HASIL] TES ETHERNET SUKSES ✓
//
//  JIKA SHIELD TIDAK TERDETEKSI:
//  - Copot Shield, periksa pin header bengkok
//  - Pasang kembali dengan kuat
//  - Coba ganti ke SPI mode manual
// ============================================================
