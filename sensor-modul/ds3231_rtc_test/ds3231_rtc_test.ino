/*
  Program ini dibuat untuk mengetes sekaligus kalibrasi RTC DS3231
  Dapat menampilkan tanggal dan waktu di Serial Monitor
  Kalibrasi dilakukan dengan input tanggal dan waktu di serial monitor, misal "2025 10 24 15 20 00" => y, M, d, h, m, s
  Pastikan koneksi SDA, SCL sudah benar, sesuaikan dengan mikrokontroler
*/

#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

void setup()
{
    Serial.begin(9600);
    while (!Serial)
        ; // tunggu serial aktif (khusus board dengan USB native)
    rtc.begin();

    Serial.println("Ketik waktu dengan format: YYYY MM DD HH MM SS lalu tekan Enter");
    Serial.println("Contoh: 2025 10 24 15 20 00");
}

void loop()
{
    // Jika ada data masuk dari serial
    if (Serial.available() > 0)
    {
        int y = Serial.parseInt();
        int M = Serial.parseInt();
        int d = Serial.parseInt();
        int h = Serial.parseInt();
        int m = Serial.parseInt();
        int s = Serial.parseInt();

        if (y > 2000 && M > 0 && d > 0)
        {
            rtc.adjust(DateTime(y, M, d, h, m, s));
            Serial.println("RTC diset sesuai input serial!");
        }
        else
        {
            Serial.println("Input tidak valid, coba lagi.");
        }

        // Hapus buffer sisa
        while (Serial.available())
            Serial.read();
    }

    // Tampilkan waktu sekarang
    DateTime now = rtc.now();
    Serial.print("Waktu: ");
    Serial.println(now.timestamp(DateTime::TIMESTAMP_FULL));
    delay(1000);
    test;
}
