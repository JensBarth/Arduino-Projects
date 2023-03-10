#include <ESP8266WiFi.h>
#include "DHT.h"

const char* ssid = "FRITZ!Box 7530 ED Barth"; //SSID aus dem Router
const char* password = "12557051313590733167"; //Passwort für den Zugang zum WLAN
 
int ledPin = D5; //digitaler PIN 5 des Wemos D1 an welchem die LED angeschlossen ist.
int ledStatus = LOW; //aktueller Status der LED (default / start  -> AUS)
WiFiServer server(80); //Port auf welchem der Server laufen soll.

 #define DHT11PIN 2 
 #define DHT11TYPE DHT11
 DHT dht11(DHT11PIN, DHT11TYPE);
 
void setup() {
  Serial.begin(115200); //Baudrate für die Serielle Geschwindigkeit.
  delay(10); //10ms. Warten damit die Seriele Kommunikation aufgebaut wurde.
  
  pinMode(ledPin, OUTPUT); //Den LEDPin als ausgang setzen.
  digitalWrite(ledPin, ledStatus); //Die LED initial auf den Status "AUS" setzen.
   
  Serial.print("Aufbau der Verbindung zu: "); //Ausgabe der SSID auf der Seriellen Schnittstelle.
  Serial.println(ssid);
  
  WiFi.begin(ssid, password); //Initialisieren der Wifi Verbindung.
 
  while (WiFi.status() != WL_CONNECTED) { //Warten bis die Verbindung aufgebaut wurde.
    delay(500);
    //Einen Punkt auf der Seriellen Schnittstelle ausgeben so das der Benutzer erkennt dass, das Sketch noch läuft.
    Serial.print("."); 
  }
  //Bei erfolgreicher Verbindung wird der folgende Text ausgeben.
  Serial.print("Mit ");
  Serial.print(ssid);
  Serial.print("erfolgreich verbunden!");

  dht11.begin();
  
  server.begin(); // Starten des Servers.
  Serial.println("Server gestartet"); //Ausgabe auf der Seriellen Schnittstelle das der Server gestartet wurde.
 
  // Ausgabe der IP Adresse 
  Serial.print("Adresse : http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

/**
 * Die Funktion gibt den HTML Kopf auf dem Client aus.
 * Dieses wird für jeden Respond verwendet.
 **/
void writeResponse(WiFiClient client, float tempValue, float humidityValue){  
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>"); 
  client.println("<head>"); 
  client.println("<script type='text/javascript' src='https://www.gstatic.com/charts/loader.js'></script>"); 
  client.println("<script type='text/javascript' src='http://progs.draeger-it.blog/wemosd1/dht11/gauges.js'></script>"); 
  client.println("<link rel='stylesheet' href='http://progs.draeger-it.blog/wemosd1/dht11/styles.css'/>");   
  client.println("</head>"); 
  client.println("<body>"); 
  client.println("<div id='tempChart' class='chartContainer'></div>"); 
  client.println("<div id='pressChart' class='chartContainer'></div>"); 
  client.println("<script>"); 
  client.println("google.charts.load('current', {'packages':['gauge']});"); 
  client.println("google.charts.setOnLoadCallback(drawCharts);"); 
  client.println("function drawCharts() {"); 
  client.print("drawTempChart(");
  client.print(tempValue); 
  client.print(");");
  client.print("drawPressChart(");
  client.print(humidityValue); 
  client.print(");");
  client.println("}"); 
  client.println("</script>"); 
  client.println("</body>"); 
  client.println("</html>"); 
}

void loop() {
  //Prüfen ob sich ein Client verbunden hat, wenn nicht die Loop "verlassen"
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wenn sich ein Client verbunden hat solange warten bis Daten gesendet werden.
  Serial.println("Neuer Client verbunden.");
  while(!client.available()){
    delay(1);
  }
 
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush(); 

  float tempValue = dht11.readTemperature(); //Temperatur vom Sensor DHT11 lesen
  float humidityValue = dht11.readHumidity(); //relative Luftfeuchtigkeit vom Sensor DHT11 lesen

 if (isnan(tempValue) || isnan(humidityValue)) {
     Serial.println("DHT11 konnte nicht ausgelesen werden");
     tempValue = 0;
     humidityValue = 0;
 }
 
  writeResponse(client, tempValue, humidityValue);
  
  delay(1); //1ms. Pause
}
