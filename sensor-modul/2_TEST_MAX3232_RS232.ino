// ============================================================
//  TEST #2 — MAX3232 / RS232 SERIAL CONVERTER
//  Tujuan  : Memastikan modul MAX3232 menerima data RS232
//            (contoh: dari timbangan digital / perangkat serial)
//
//  Modul MAX3232 mengubah sinyal RS232 (±12V) menjadi
//  sinyal TTL (0-3.3V/5V) yang bisa dibaca Arduino.
//
//  KONEKSI HARDWARE:
//  ┌─────────────┬────────────────────────────────────┐
//  │  MAX3232    │  Arduino Mega                       │
//  ├─────────────┼────────────────────────────────────┤
//  │  RX (T1IN)  │  Pin TX1 / Pin 18  (Arduino kirim) │
//  │  TX (R1OUT) │  Pin RX1 / Pin 19  (Arduino terima)│
//  │  VCC        │  3.3V atau 5V (cek modul kamu)      │
//  │  GND        │  GND                               │
//  └─────────────┴────────────────────────────────────┘
//
//  Sketch ini melakukan 2 tes sekaligus:
//  A) LOOPBACK TEST : Arduino kirim data, lalu terima balik
//                     (butuh kabel jumper RX-TX modul disambung)
//  B) RECEIVE TEST  : Baca data dari perangkat eksternal
//                     (timbangan, PLC, dsb)
//
//  CARA TES:
//  1. Untuk LOOPBACK: hubungkan pin TX dan RX modul MAX3232
//     ke sisi DB9 dengan loopback connector (pin 2 ke pin 3)
//  2. Upload sketch, buka Serial Monitor (baud 9600)
//  3. Lihat apakah data yang dikirim kembali diterima
// ============================================================

// Arduino Mega punya 4 Serial hardware:
//   Serial  (pin 0,1)   → dipakai untuk komunikasi ke Serial Monitor
//   Serial1 (pin 18,19) → kita pakai untuk MAX3232
//   Serial2 (pin 16,17) → tersedia
//   Serial3 (pin 14,15) → tersedia

const long BAUD_RS232    = 4800;  // Sesuaikan dengan baud rate perangkat kamu
                                  // Timbangan sering 4800 atau 9600
const long BAUD_MONITOR  = 9600;  // Baud untuk Serial Monitor

// Timer untuk kirim data loopback secara berkala
unsigned long lastSendTime = 0;
const unsigned long SEND_INTERVAL = 2000; // kirim setiap 2 detik

// Counter paket yang dikirim
int packetCount = 0;

void setup() {
  // Serial ke komputer (Serial Monitor)
  Serial.begin(BAUD_MONITOR);

  // Serial1 ke modul MAX3232
  Serial1.begin(BAUD_RS232);

  Serial.println(F("=============================="));
  Serial.println(F("  TEST MAX3232 RS232 AKTIF"));
  Serial.println(F("  Baud RS232 : 4800"));
  Serial.println(F("  Port       : Serial1 (pin 18,19)"));
  Serial.println(F("=============================="));
  Serial.println(F("Menunggu data atau loopback..."));
  Serial.println();
}

void loop() {

  // ==============================================
  //  BAGIAN A: LOOPBACK TEST
  //  Kirim string tes secara berkala ke MAX3232
  // ==============================================
  if (millis() - lastSendTime >= SEND_INTERVAL) {
    lastSendTime = millis();
    packetCount++;

    // Format string mirip output timbangan: "wn0012.50kg"
    String testMsg = "wn00" + String(packetCount) + "2.50kg";

    Serial1.println(testMsg); // Kirim via MAX3232

    Serial.print(F("[KIRIM] Paket #"));
    Serial.print(packetCount);
    Serial.print(F(" : "));
    Serial.println(testMsg);
  }

  // ==============================================
  //  BAGIAN B: RECEIVE TEST
  //  Baca semua data yang masuk dari MAX3232
  // ==============================================
  if (Serial1.available()) {
    String received = "";

    // Baca sampai newline atau buffer habis
    while (Serial1.available()) {
      char c = Serial1.read();
      if (c == '\n') break;         // berhenti di newline
      if (c != '\r') received += c; // abaikan carriage return
      delay(2); // beri waktu byte berikutnya datang
    }

    if (received.length() > 0) {
      Serial.print(F("[TERIMA] Data masuk: \""));
      Serial.print(received);
      Serial.println(F("\""));

      // --- Coba parse format timbangan (opsional) ---
      // Format yang diharapkan: "wn<berat>kg" atau "ww<berat>lb"
      if (received.length() >= 9) {
        String prefix = received.substring(0, 2);
        String suffix = received.substring(received.length() - 2);

        if ((prefix == "wn" || prefix == "ww") &&
            (suffix == "kg" || suffix == "lb")) {
          String beratStr = received.substring(2, received.length() - 2);
          float berat = beratStr.toFloat();
          Serial.print(F("  → Berat berhasil diparsing: "));
          Serial.print(berat);
          Serial.println(suffix);
        } else {
          Serial.println(F("  → Format tidak dikenali (bukan format timbangan)"));
        }
      }

      Serial.println(F("------------------------------"));
    }
  }
}

// ============================================================
//  HASIL YANG DIHARAPKAN (loopback berhasil):
//
//  [KIRIM]  Paket #1 : wn012.50kg
//  [TERIMA] Data masuk: "wn012.50kg"
//    → Berat berhasil diparsing: 12.50 kg
//  ------------------------------
//
//  JIKA TERIMA TIDAK MUNCUL:
//  - Cek kabel RX/TX tidak terbalik
//  - Pastikan loopback connector terpasang di DB9
//  - Cek VCC modul MAX3232 sesuai (3.3V atau 5V)
//  - Coba ganti baud rate (9600 atau 19200)
//  - Periksa kapasitor 100nF pada modul MAX3232
// ============================================================
