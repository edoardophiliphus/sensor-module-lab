/*
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * -----------------------------------------------------------------------------------------
 *             MFRC522      ESP8266 (NodeMCU / Wemos D1 Mini)
 * Signal      Pin          Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          D3  (GPIO 0)
 * SPI SS      SDA(SS)      D4  (GPIO 2)
 * SPI MOSI    MOSI         D7  (GPIO 13)
 * SPI MISO    MISO         D6  (GPIO 12)
 * SPI SCK     SCK          D5  (GPIO 14)
 * VCC         3.3V         (gunakan 3.3V, jangan 5V!)
 * GND         GND
 *
 * -----------------------------------------------------------------------------------------
 *             MFRC522      ESP32
 * Signal      Pin          Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          22   (bisa diganti)
 * SPI SS      SDA(SS)      21   (bisa diganti)
 * SPI MOSI    MOSI         23
 * SPI MISO    MISO         19
 * SPI SCK     SCK          18
 * VCC         3.3V
 * GND         GND
 *
 * Catatan:
 * - ESP8266 & ESP32 menggunakan tegangan logika 3.3V, cocok langsung dengan MFRC522.
 * - Kamu bisa ubah pin SS dan RST di kode dengan mengganti definisi:
 *     #define SS_PIN <nomor_pin>
 *     #define RST_PIN <nomor_pin>
 * - Pastikan library MFRC522 sudah diinstal dari Library Manager.
 */

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10 // Ganti sesuai board (lihat tabel di atas)
#define RST_PIN 9 // Ganti sesuai board (lihat tabel di atas)

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key;

// Init array that will store new NUID
byte nuidPICC[4];

void setup()
{
    Serial.begin(115200);
    SPI.begin();     // Init SPI bus
    rfid.PCD_Init(); // Init MFRC522

    for (byte i = 0; i < 6; i++)
    {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("This code scans the MIFARE Classic NUID."));
    Serial.print(F("Using the following key:"));
    printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void loop()
{
    if (!rfid.PICC_IsNewCardPresent())
        return;

    if (!rfid.PICC_ReadCardSerial())
        return;

    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    Serial.println(rfid.PICC_GetTypeName(piccType));

    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
        piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
        piccType != MFRC522::PICC_TYPE_MIFARE_4K)
    {
        Serial.println(F("Your tag is not of type MIFARE Classic."));
        return;
    }

    if (rfid.uid.uidByte[0] != nuidPICC[0] ||
        rfid.uid.uidByte[1] != nuidPICC[1] ||
        rfid.uid.uidByte[2] != nuidPICC[2] ||
        rfid.uid.uidByte[3] != nuidPICC[3])
    {
        Serial.println(F("A new card has been detected."));

        for (byte i = 0; i < 4; i++)
        {
            nuidPICC[i] = rfid.uid.uidByte[i];
        }

        Serial.println(F("The NUID tag is:"));
        Serial.print(F("In hex: "));
        printHex(rfid.uid.uidByte, rfid.uid.size);
        Serial.println();
        Serial.print(F("In dec: "));
        printDec(rfid.uid.uidByte, rfid.uid.size);
        Serial.println();
    }
    else
        Serial.println(F("Card read previously."));

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}

void printHex(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

void printDec(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], DEC);
    }
}
