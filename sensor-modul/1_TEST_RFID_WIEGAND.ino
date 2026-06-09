// ============================================================
//  TEST #1 — RFID WIEGAND READER
//  Tujuan  : Memastikan modul RFID Wiegand terbaca dengan benar
//  Library : Wiegand (install via Library Manager Arduino IDE)
//            Cari: "Wiegand" oleh Samy Kamkar / Paulo Pinto
//
//  KONEKSI HARDWARE:
//  ┌──────────────┬────────────────┐
//  │  RFID Reader │  Arduino Mega  │
//  ├──────────────┼────────────────┤
//  │  D0 (Hijau)  │  Pin 2         │
//  │  D1 (Putih)  │  Pin 3         │
//  │  GND (Hitam) │  GND           │
//  │  VCC (Merah) │  5V atau 12V*  │
//  └──────────────┴────────────────┘
//  * Cek datasheet reader kamu, ada yang butuh 12V
//
//  CARA TES:
//  1. Upload sketch ini ke Arduino
//  2. Buka Serial Monitor (baud 9600)
//  3. Tempelkan kartu/tag RFID ke reader
//  4. Angka UID kartu akan muncul di Serial Monitor
//  5. Coba beberapa kartu berbeda, pastikan angkanya berbeda
// ============================================================

#include <Wiegand.h>

// --- Definisi pin D0 dan D1 dari reader RFID ---
// D0 = data bit 0, D1 = data bit 1 (protokol Wiegand 26/34-bit)
const byte pinD0 = 2;
const byte pinD1 = 3;

// Buat objek Wiegand bernama "wg"
WIEGAND wg;

// Variabel untuk mencatat waktu tap terakhir (anti-ghost / debounce)
unsigned long lastTapTime    = 0;
const unsigned long DEBOUNCE = 1000; // jeda minimal antar baca = 1 detik

void setup() {
  // Mulai komunikasi Serial ke komputer (Serial Monitor)
  Serial.begin(9600);

  // Inisialisasi Wiegand dengan pin D0 dan D1
  wg.begin(pinD0, pinD1);

  Serial.println(F("=============================="));
  Serial.println(F("  TEST RFID WIEGAND AKTIF"));
  Serial.println(F("  Tempelkan kartu ke reader..."));
  Serial.println(F("=============================="));
}

void loop() {
  // Cek apakah ada data baru dari reader RFID
  if (wg.available()) {

    unsigned long now = millis();

    // Debounce: abaikan jika tap terlalu cepat (< 1 detik)
    if (now - lastTapTime < DEBOUNCE) {
      Serial.println(F("[SKIP] Tap terlalu cepat, diabaikan."));
      return;
    }
    lastTapTime = now;

    // Ambil kode UID dari kartu yang ditap
    unsigned long uid = wg.getCode();

    // Tampilkan info bit-length protokol (biasanya 26 atau 34)
    int bitLength = wg.getWiegandType();

    Serial.println(F("------------------------------"));
    Serial.print(F("  Kartu terdeteksi!"));
    Serial.println();
    Serial.print(F("  UID (Desimal) : "));
    Serial.println(uid);                     // format angka biasa
    Serial.print(F("  UID (HEX)     : 0x"));
    Serial.println(uid, HEX);               // format hexadecimal
    Serial.print(F("  Bit-length    : "));
    Serial.print(bitLength);
    Serial.println(F(" bit"));
    Serial.println(F("------------------------------"));
  }
}

// ============================================================
//  HASIL YANG DIHARAPKAN (di Serial Monitor):
//
//  ==============================
//    TEST RFID WIEGAND AKTIF
//    Tempelkan kartu ke reader...
//  ==============================
//  ------------------------------
//    Kartu terdeteksi!
//    UID (Desimal) : 1234567890
//    UID (HEX)     : 0x499602D2
//    Bit-length    : 26 bit
//  ------------------------------
//
//  JIKA TIDAK ADA OUTPUT:
//  - Cek koneksi pin D0/D1
//  - Pastikan VCC reader sesuai (5V atau 12V)
//  - Pastikan GND reader terhubung ke GND Arduino
//  - Coba ganti library Wiegand jika perlu
// ============================================================
