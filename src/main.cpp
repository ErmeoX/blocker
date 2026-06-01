#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

SPIClass spi1(VSPI);
SPIClass spi2(HSPI);

RF24 radio1(5, 17);
RF24 radio2(4, 15);

// Bluetooth classic channels from nrfbox
const byte bluetooth_channels[] = {32, 34, 46, 48, 50, 52, 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 74, 76, 78, 80};
const byte ble_channels[] = {2, 26, 80};

const int btCount  = sizeof(bluetooth_channels) / sizeof(bluetooth_channels[0]);
const int bleCount = sizeof(ble_channels) / sizeof(ble_channels[0]);

// skip every other channel — adjacent channels bleed into each other at 1MBPS
const byte channelGroup1[] = {0, 4, 8, 12, 16, 20, 24, 28, 32, 36};
const byte channelGroup2[] = {40, 44, 48, 52, 56, 60, 64, 68, 72, 76};

const int group1Count = sizeof(channelGroup1) / sizeof(channelGroup1[0]);
const int group2Count = sizeof(channelGroup2) / sizeof(channelGroup2[0]);

byte p = 0xFF; // single byte instead of "xxxxxxxxxxxxxxxx"

void setupRadio(RF24 &radio) {
  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.setRetries(0, 0);
  radio.setCRCLength(RF24_CRC_DISABLED);
  radio.stopListening();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting nrfbox-style jammer...");

  spi1.begin(18, 19, 23, 17);
  spi1.setFrequency(20000000);
  delay(100);

  if (!radio1.begin(&spi1)) {
    Serial.println("ERROR: Radio1 not responding!");
    while (1) {}
  }
  if (!radio1.isChipConnected()) {
    Serial.println("ERROR: Chip1 not connected!");
    while (1) {}
  }
  Serial.println("Radio1 OK!");
  setupRadio(radio1);

  spi2.begin(14, 12, 13, 15);
  spi2.setFrequency(20000000);
  delay(100);

  if (!radio2.begin(&spi2)) {
    Serial.println("ERROR: Radio2 not responding!");
    while (1) {}
  }
  if (!radio2.isChipConnected()) {
    Serial.println("ERROR: Chip2 not connected!");
    while (1) {}
  }
  Serial.println("Radio2 OK!");
  setupRadio(radio2);

  Serial.println("Both modules ready!");
}

void loop() {
  if (!radio1.isChipConnected()) {
    Serial.println("ERROR: Radio1 disconnected! Halting.");
    while (1) {}
  }
  if (!radio2.isChipConnected()) {
    Serial.println("ERROR: Radio2 disconnected! Halting.");
    while (1) {}
  }

  for (int i = 0; i < group1Count; i++) {
    radio1.setChannel(channelGroup1[i]);
    radio1.writeFast(&p, 1);

    radio2.setChannel(channelGroup2[i]);
    radio2.writeFast(&p, 1);
  }

  static int sweepCount = 0;
  static unsigned long startTime = millis();
  sweepCount++;

  if (sweepCount >= 500) {
    unsigned long elapsed = millis() - startTime;
    Serial.print("Still running — 500 sweeps in ");
    Serial.print(elapsed);
    Serial.print("ms | avg sweep: ");
    Serial.print(elapsed / 500);
    Serial.println("ms");
    sweepCount = 0;
    startTime = millis();
  }
}