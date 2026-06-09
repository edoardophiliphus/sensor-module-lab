// ============================================================
//  TEST #4 — MODUL RELAY (3 Channel)
//  Tujuan  : Memastikan setiap relay bekerja dengan benar
//            (bunyi "klik", LED relay nyala, beban tersambung)
//
//  Sketch ini menggunakan pin yang SAMA seperti di proyek utama:
//    Pin 44 → Relay Lampu SAVE  (hijau / sukses)
//    Pin 45 → Relay Lampu FAIL  (merah / gagal)
//    Pin 46 → Relay Lampu IDLE  (kuning / standby)
//
//  PENTING — LOGIKA RELAY:
//  Kebanyakan modul relay "active LOW" (aktif saat diberi LOW):
//    LOW  (0V) → relay ON  → kontak tertutup → beban menyala
//    HIGH (5V) → relay OFF → kontak terbuka  → beban mati
//
//  Jika relaymu "active HIGH", balik logika LOW/HIGH di bawah.
//
//  KONEKSI HARDWARE:
//  ┌──────────────┬────────────────┐
//  │  Modul Relay │  Arduino Mega  │
//  ├──────────────┼────────────────┤
//  │  IN1         │  Pin 44        │
//  │  IN2         │  Pin 45        │
//  │  IN3         │  Pin 46        │
//  │  VCC         │  5V            │
//  │  GND         │  GND           │
//  └──────────────┴────────────────┘
//
//  CARA TES:
//  1. Upload sketch ini
//  2. Buka Serial Monitor (baud 9600)
//  3. Dengarkan bunyi "klik" pada setiap relay
//  4. Perhatikan LED pada modul relay menyala bergantian
//  5. Jika ada lampu/beban yang terhubung ke relay, akan menyala
// ============================================================

// ─── PIN RELAY ────────────────────────────────────────────
// Sesuaikan dengan pin di proyek kamu
const byte PIN_RELAY_SAVE = 44; // Relay 1 – Lampu SAVE (hijau)
const byte PIN_RELAY_FAIL = 45; // Relay 2 – Lampu FAIL (merah)
const byte PIN_RELAY_IDLE = 46; // Relay 3 – Lampu IDLE (kuning)

// ─── LOGIKA AKTIF ─────────────────────────────────────────
// Ubah ke HIGH jika modul relaymu active HIGH
const byte RELAY_ON  = LOW;  // sinyal untuk NYALAKAN relay
const byte RELAY_OFF = HIGH; // sinyal untuk MATIKAN relay

// ─── DURASI TES ───────────────────────────────────────────
const int DELAY_ON  = 800;  // relay ON selama 800ms
const int DELAY_OFF = 400;  // relay OFF selama 400ms
const int DELAY_GAP = 500;  // jeda antar relay = 500ms

// ──────────────────────────────────────────────────────────

void setup() {
  Serial.begin(9600);

  // Set semua pin relay sebagai OUTPUT
  pinMode(PIN_RELAY_SAVE, OUTPUT);
  pinMode(PIN_RELAY_FAIL, OUTPUT);
  pinMode(PIN_RELAY_IDLE, OUTPUT);

  // Pastikan semua relay OFF di awal (kondisi aman)
  matikanSemua();

  Serial.println(F("=============================="));
  Serial.println(F("  TEST MODUL RELAY 3 CHANNEL"));
  Serial.println(F("  Pin: 44 (SAVE), 45 (FAIL), 46 (IDLE)"));
  Serial.println(F("  Logika: Active LOW"));
  Serial.println(F("=============================="));
  Serial.println();
  delay(1000);

  // Jalankan semua tes satu kali saat boot
  tesSemuaRelay();
}

void loop() {
  // ─── DEMO BERULANG: Simulasi lampu status proyek ─────────
  //
  //  Urutan ini meniru perilaku di proyek asli:
  //  IDLE → SAVE → IDLE → FAIL → IDLE → OK → IDLE
  //
  Serial.println(F("=== DEMO SIKLUS STATUS LAMPU ==="));

  // Status IDLE: lampu kuning nyala (relay SAVE & FAIL mati)
  Serial.println(F("[STATUS] IDLE - lampu kuning standby"));
  digitalWrite(PIN_RELAY_SAVE, RELAY_OFF);
  digitalWrite(PIN_RELAY_FAIL, RELAY_OFF);
  digitalWrite(PIN_RELAY_IDLE, RELAY_ON);
  delay(1500);

  // Status SAVE: semua lampu mati (relay aktif = bukan lampu)
  // Di proyek asli: lampu(LOW, LOW, LOW) → semua relay ON
  Serial.println(F("[STATUS] SAVE - berhasil simpan data"));
  nyalakanSemua();
  delay(DELAY_ON);
  matikanSemua();
  delay(300);
  // Kembali IDLE
  digitalWrite(PIN_RELAY_IDLE, RELAY_ON);
  delay(1500);

  // Status FAIL: relay SAVE & IDLE nyala (FAIL mati)
  Serial.println(F("[STATUS] FAIL - kartu/data gagal"));
  digitalWrite(PIN_RELAY_SAVE, RELAY_ON);
  digitalWrite(PIN_RELAY_FAIL, RELAY_OFF);
  digitalWrite(PIN_RELAY_IDLE, RELAY_ON);
  delay(DELAY_ON);
  matikanSemua();
  delay(300);
  // Kembali IDLE
  digitalWrite(PIN_RELAY_IDLE, RELAY_ON);
  delay(1500);

  // Status OK: relay FAIL & IDLE nyala (SAVE mati)
  Serial.println(F("[STATUS] OK - data terkirim ke server"));
  digitalWrite(PIN_RELAY_SAVE, RELAY_OFF);
  digitalWrite(PIN_RELAY_FAIL, RELAY_ON);
  digitalWrite(PIN_RELAY_IDLE, RELAY_ON);
  delay(DELAY_ON);
  matikanSemua();
  delay(300);
  // Kembali IDLE
  digitalWrite(PIN_RELAY_IDLE, RELAY_ON);
  delay(2000);
}

// ─── Fungsi: Tes setiap relay satu per satu ───────────────
void tesSemuaRelay() {
  Serial.println(F("--- Tes Individual Setiap Relay ---"));

  tesRelay("SAVE (Pin 44)", PIN_RELAY_SAVE);
  delay(DELAY_GAP);

  tesRelay("FAIL (Pin 45)", PIN_RELAY_FAIL);
  delay(DELAY_GAP);

  tesRelay("IDLE (Pin 46)", PIN_RELAY_IDLE);
  delay(DELAY_GAP);

  // Tes semua relay nyala bersamaan
  Serial.println(F("[TES] Semua relay ON bersamaan..."));
  nyalakanSemua();
  delay(1000);
  Serial.println(F("[TES] Semua relay OFF..."));
  matikanSemua();
  delay(500);

  Serial.println(F("--- Tes Individual Selesai ---"));
  Serial.println();
}

// ─── Fungsi: Tes satu relay (nyala lalu mati) ─────────────
void tesRelay(const char* nama, byte pin) {
  Serial.print(F("[TES] Relay "));
  Serial.print(nama);
  Serial.print(F(" → ON "));
  digitalWrite(pin, RELAY_ON);   // nyalakan relay
  delay(DELAY_ON);               // tahan sebentar

  Serial.println(F("→ OFF"));
  digitalWrite(pin, RELAY_OFF);  // matikan relay
  delay(DELAY_OFF);
}

// ─── Fungsi: Nyalakan semua relay ─────────────────────────
void nyalakanSemua() {
  digitalWrite(PIN_RELAY_SAVE, RELAY_ON);
  digitalWrite(PIN_RELAY_FAIL, RELAY_ON);
  digitalWrite(PIN_RELAY_IDLE, RELAY_ON);
}

// ─── Fungsi: Matikan semua relay ──────────────────────────
void matikanSemua() {
  digitalWrite(PIN_RELAY_SAVE, RELAY_OFF);
  digitalWrite(PIN_RELAY_FAIL, RELAY_OFF);
  digitalWrite(PIN_RELAY_IDLE, RELAY_OFF);
}

// ============================================================
//  HASIL YANG DIHARAPKAN:
//
//  --- Tes Individual Setiap Relay ---
//  [TES] Relay SAVE (Pin 44) → ON → OFF
//  [TES] Relay FAIL (Pin 45) → ON → OFF
//  [TES] Relay IDLE (Pin 46) → ON → OFF
//  [TES] Semua relay ON bersamaan...
//  [TES] Semua relay OFF...
//  --- Tes Individual Selesai ---
//
//  ==> Kamu harus mendengar bunyi "klik-klik" 
//      sebanyak 3 kali per relay saat tes individual.
//
//  JIKA RELAY TIDAK BUNYI / TIDAK NYALA:
//  1. Cek tegangan VCC modul (harus 5V)
//  2. Cek apakah relay butuh power terpisah (JD-VCC)
//     → beberapa modul punya jumper JD-VCC, cabut jumper
//       dan beri tegangan terpisah ke JD-VCC
//  3. Coba ganti RELAY_ON dari LOW ke HIGH (active HIGH)
//  4. Ukur tegangan pin IN saat LOW (harus mendekati 0V)
//  5. Cek LED pada modul — jika LED nyala tapi relay tidak
//     klik, relay kemungkinan rusak
// ============================================================
