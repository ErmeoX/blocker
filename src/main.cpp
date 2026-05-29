#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

SPIClass spi1(VSPI);
SPIClass spi2(HSPI);

RF24 radio1(5, 17);
RF24 radio2(4, 15);

const byte address[6] = "00001";

const int BT_CH_START1 = 2;
const int BT_CH_END1   = 40;
const int BT_CH_START2 = 41;
const int BT_CH_END2   = 80;

byte p = 0xFF;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting Bluetooth band jammer...");

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
  Serial.println("Chip1 connected!");
  radio1.setAutoAck(false);
  radio1.openWritingPipe(address);
  radio1.stopListening();
  radio1.setPALevel(RF24_PA_HIGH);
  radio1.setDataRate(RF24_1MBPS);
  radio1.setRetries(0, 0);

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
  Serial.println("Chip2 connected!");
  radio2.setAutoAck(false);
  radio2.openWritingPipe(address);
  radio2.stopListening();
  radio2.setPALevel(RF24_PA_HIGH);
  radio2.setDataRate(RF24_1MBPS);
  radio2.setRetries(0, 0);

  Serial.println("Both modules ready — starting sweep!");
}

void loop() {
  if (!radio1.isChipConnected()) {
    Serial.println("ERROR: Module1 disconnected! Halting.");
    while (1) {}
  }
  if (!radio2.isChipConnected()) {
    Serial.println("ERROR: Module2 disconnected! Halting.");
    while (1) {}
  }

  unsigned long start = millis();

  for (int ch = 0; ch <= 39; ch++) {
    radio1.setChannel(BT_CH_START1 + ch);
    radio1.writeFast(&p, 1);

    radio2.setChannel(BT_CH_START2 + ch);
    radio2.writeFast(&p, 1);
  }

  Serial.print("Sweep took: ");
  Serial.print(millis() - start);
  Serial.println("ms");
}