#include <Arduino.h>
#include <ESP8266WiFi.h>

const char* host = "https://www.peka.poznan.pl/vm/method.vm";
const char* ssid = "***REMOVED***";
const char* password = "***REMOVED***";

const int buttonPin = D7;
volatile bool wasPressed = false;


// struct bus_stop {
//   String symbol;
//   String name;
// } typedef bus_stop;

/* VM API works like that:
curl -d "method=getTimes&p0={'symbol':'SWRZ01'}" \
     -H "Content-Type: application/x-www-form-urlencoded" \
     -X POST https://www.peka.poznan.pl/vm/method.vm
 */

// bus_stop stops[] = {
//   { .symbol = "SWRZ01", .name = "Świerzawska->Ławica" },
//   { .symbol = "SWRZ02", .name = "Świerzawska->Kaponiera" },
//   { .symbol = "RYCE04", .name = "Rycerska->Kaponiera" },
// };
// int stopsCount = 2;
// volatile int currentStop = 0;
// String postParams = "method=getTimes&p0={'symbol':'BIAL02'}";



void handleKeyPress();
void connect();


void setup()
{
  Serial.begin(115200);
  Serial.println();

  pinMode(buttonPin, INPUT); // TODO use internal pullup to omit ext resistor
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleKeyPress, RISING);

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
}


void loop()
{
  if (wasPressed) {
    connect();
  }
  
  delay(5000);
}


void connect() {
  WiFiClient client;
  Serial.printf("\n[Connecting to %s ... ", host);
  if (client.connect(host, 80))
  {
    Serial.println("connected]");

    Serial.println("[Sending a request]");
    client.print(String("GET /") + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n" +
                 "\r\n"
                );

    Serial.println("[Response:]");
    while (client.connected() || client.available())
    {
      if (client.available())
      {
        String line = client.readStringUntil('\n');
        Serial.println(line);
      }
    }
    client.stop();
    Serial.println("\n[Disconnected]");
  }
  else
  {
    Serial.println("connection failed!]");
    client.stop();
  }
}



// void loop() {
//   if (wasPressed) {
//     Serial.print("["); Serial.print(millis());
//     Serial.println("] Button was pressed");
//     wasPressed = false;
    
//     WiFiClientSecure client;
//     if (client.connect(host, 80)) {
//     // we are connected to the host!
//       Serial.println("connected]");

//       Serial.println("[Sending a request]");
//       client.print(String("GET /") + " HTTP/1.1\r\n" +
//                  "Host: " + host + "\r\n" +
//                  "Connection: close\r\n" +
//                  "\r\n"
//                 );
//       Serial.println("[Response:]");
//       while (client.connected() || client.available())
//       {
//         if (client.available())
//         {
//           String line = client.readStringUntil('\n');
//           Serial.println(line);
//         }
//       }
//       client.stop();
//       Serial.println("\n[Disconnected]");
//     } else {
//       // connection failure
//       Serial.println("Failed to connect to PEKA VM");
//     }
//   }
// }

void handleKeyPress() {
  // currentStop = (currentStop + 1) % stopsCount;
  wasPressed = true;
}