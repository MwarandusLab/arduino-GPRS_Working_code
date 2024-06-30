#include <SoftwareSerial.h>
#include <AltSoftSerial.h>

#define rxPin 13
#define txPin 12
SoftwareSerial sim800L(rxPin, txPin); 

AltSoftSerial neogps;

unsigned long previousMillis = 0;
long interval = 60000;

void setup() {
  Serial.begin(9600);
  sim800L.begin(9600);
  neogps.begin(9600);

  Serial.println("Initializing...");

  sendATcommand("AT", "OK", 2000);
  sendATcommand("AT+CMGF=1", "OK", 2000);
}

void loop() {
  while (sim800L.available()) {
    Serial.println(sim800L.readString());
  }
  while (Serial.available()) {
    sim800L.println(Serial.readString());
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    sendGpsToServer();
  }
}

void sendGpsToServer() {
  // // Use fixed latitude and longitude values for demonstration
  // String latitude = "12.3456";
  // String longitude = "78.9101";

  String url = "http://shopokoacommunitycard.co.ke/update.php?amount=200&tagID=211138108163150&pin=1234";

  // Serial.print("Latitude= ");
  // Serial.print(latitude);
  // Serial.print(" Longitude= ");
  // Serial.println(longitude);

  Serial.println(url);
  delay(300);

  sendATcommand("AT+CFUN=1", "OK", 2000);
  sendATcommand("AT+CGATT=1", "OK", 2000);
  sendATcommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"", "OK", 2000);
  sendATcommand("AT+SAPBR=3,1,\"APN\",\"internet\"", "OK", 2000);
  sendATcommand("AT+SAPBR=1,1", "OK", 2000);
  sendATcommand("AT+HTTPINIT", "OK", 2000);
  sendATcommand("AT+HTTPPARA=\"CID\",1", "OK", 1000);
  sim800L.print("AT+HTTPPARA=\"URL\",\"");
  sim800L.print(url);
  sendATcommand("\"", "OK", 1000);
  sendATcommand("AT+HTTPACTION=0", "0,200", 1000);
  sendATcommand("AT+HTTPTERM", "OK", 1000);
  sendATcommand("AT+CIPSHUT", "SHUT OK", 1000);
}

int8_t sendATcommand(const char* ATcommand, const char* expected_answer, unsigned int timeout) {
  uint8_t x = 0, answer = 0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);
  delay(100);

  while (sim800L.available() > 0) sim800L.read();

  if (ATcommand[0] != '\0') {
    sim800L.println(ATcommand);
  }

  x = 0;
  previous = millis();

  do {
    if (sim800L.available() != 0) {
      response[x] = sim800L.read();
      x++;

      if (strstr(response, expected_answer) != NULL) {
        answer = 1;
      }
    }
  } while ((answer == 0) && ((millis() - previous) < timeout));

  Serial.println(response);
  return answer;
}
