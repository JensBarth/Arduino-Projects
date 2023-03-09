// benoetigte Bibliotheken
#include <Ethernet.h>
#include <EthernetUdp.h>

// Netzwerk Einstellungen
//MAC-Adresse bitte anpassen
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEB };
// UDP Port zum Datenempfang Lox MS -> Arduino
unsigned int ARDUPORT = 7013;

// IP Lox MS
IPAddress MSIP(192, 168, 178, 145);
// UDP Port zum Datenversand Arduino -> Lox MS
unsigned int MSPORT = 7014;

// Daten-Buffer
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

// EthernetUDP Instanz um UDP zu senden/empfangen
EthernetUDP Udp;

// Pin fure HC SR04 festlegen
#define echoPin 2 // Echo Pin
#define trigPin 3 // Trigger Pin

// Konstanten
int maxReichweite = 300; // Maximum Reichweite der Messung
int minReichweite = 0; // Minimum Recihweite der Messung

// Variablen
long dauer;
float distanz;
char chardistanz[10];

void setup() {
  Serial.begin(9600);

  if (!Ethernet.begin(mac)) Serial.println("DHCP fehlgeschlagen");
  else {

    Serial.println ("Netzwerkdaten");
    Serial.println ("---------------");
    Serial.print("Arduino MAC: ");
    Serial.print(mac[0], HEX);
    Serial.print(":");
    Serial.print(mac[1], HEX);
    Serial.print(":");
    Serial.print(mac[2], HEX);
    Serial.print(":");
    Serial.print(mac[3], HEX);
    Serial.print(":");
    Serial.print(mac[4], HEX);
    Serial.print(":");
    Serial.println(mac[5], HEX);
    Serial.print("Arduino IP: ");
    Serial.println(Ethernet.localIP());
    Serial.print("Arduino UDP Port: ");
    Serial.println(ARDUPORT);
    Serial.println ("---------------");
    Serial.print ("Miniserver IP: ");
    Serial.println(MSIP);
    Serial.print("Miniserver UDP Port: ");
    Serial.println(MSPORT);
    Serial.println ("---------------");
  }

  Udp.begin(ARDUPORT); // Start UDP

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

}

void loop() {
  // Distanz lesen
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  dauer = pulseIn(echoPin, HIGH);

  //Distanz in cm umrechnen
  distanz = dauer / 58.2;

  // min max auswerten, wenn in der Range wird gesendet
  if (distanz >= maxReichweite || distanz <= minReichweite) {
    Serial.println("Messung fehlerhaft!");
  }
  else {
    Serial.print(distanz), Serial.println(" cm");
    dtostrf(distanz, 4, 0, chardistanz); //Distanz fuer UDP Versand umwandeln (1 steht fue  r Nachkommastellen)
    sendUDP(chardistanz); //Messung an MS senden
  }
  delay(2000); // Wartet x Sekunden bis zur naechsten Messung, z.B. 5000 = 5 Sekunden
}

//UDP-Befehl senden
void sendUDP(String text)
{
  Udp.beginPacket(MSIP, MSPORT);
  Udp.print(text);
  Udp.endPacket();
  delay(10);
}
