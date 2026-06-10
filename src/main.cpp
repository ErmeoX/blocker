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

// split channels between two modules
const byte channelGroup1[] = {2, 5, 8, 11, 32, 34, 46, 48, 50};
const byte channelGroup2[] = {52, 0, 1, 4, 6, 22, 24, 26, 28, 30, 74, 76, 78, 80};

const int group1Count = sizeof(channelGroup1) / sizeof(channelGroup1[0]);
const int group2Count = sizeof(channelGroup2) / sizeof(channelGroup2[0]);

const char p[] = "xxxxxxxxxxxxxxxx"; // same payload as nrfbox

void setupRadio(RF24 &radio) {
  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);  // back to 2MBPS
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
  radio1.setChannel(channelGroup1[0]); radio1.writeFast(&p, 1);
  radio2.setChannel(channelGroup2[0]); radio2.writeFast(&p, 1);
  radio1.setChannel(channelGroup1[1]); radio1.writeFast(&p, 1);
  radio2.setChannel(channelGroup2[1]); radio2.writeFast(&p, 1);
  radio1.setChannel(channelGroup1[2]); radio1.writeFast(&p, 1);
  radio2.setChannel(channelGroup2[2]); radio2.writeFast(&p, 1);
  radio1.setChannel(channelGroup1[3]); radio1.writeFast(&p, 1);
  radio2.setChannel(channelGroup2[3]); radio2.writeFast(&p, 1);
  radio1.setChannel(channelGroup1[4]); radio1.writeFast(&p, 1);
  radio2.setChannel(channelGroup2[4]); radio2.writeFast(&p, 1);
  radio1.setChannel(channelGroup1[5]); radio1.writeFast(&p, 1);
  radio2.setChannel(channelGroup2[5]); radio2.writeFast(&p, 1);
  radio1.setChannel(channelGroup1[6]); radio1.writeFast(&p, 1);
  radio2.setChannel(channelGroup2[6]); radio2.writeFast(&p, 1);
  radio1.setChannel(channelGroup1[7]); radio1.writeFast(&p, 1);
  radio2.setChannel(channelGroup2[7]); radio2.writeFast(&p, 1);
  radio1.setChannel(channelGroup1[8]); radio1.writeFast(&p, 1);
  radio2.setChannel(channelGroup2[8]); radio2.writeFast(&p, 1);

static int sweepCount = 0;
  static unsigned long startTime = millis();
  sweepCount++;  // ← this was missing
  if (sweepCount >= 500) {
    if (!radio1.isChipConnected() || !radio2.isChipConnected()) {
      Serial.println("ERROR: Module disconnected! Halting.");
      while (1) {}
    }
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