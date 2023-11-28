#include <ESP8266WiFi.h>

const char* ssid = "Juani";
const char* password = "JIG11111010001";

WiFiClient client;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  connectToWiFi();
}

void loop() 
{
  // put your main code here, to run repeatedly:
}

void connectToWiFi() 
{
  //Conectarse a la red WiFi.
  Serial.println();
  Serial.println();
  Serial.print("Conectando a la red WiFi: ");
  Serial.println(ssid);

  Serial.println("...");
  WiFi.begin(ssid, password);
  int intentos = 0;
  //El ESP se tratará de conectar a la red WiFi 15 veces
  //siempre y cuando no se haya establecido la conexión.
  while (WiFi.status() != WL_CONNECTED) 
  {
    intentos++;
    delay(1000);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) 
  {
    Serial.println();
    Serial.print(F("Conectado a "));
    Serial.println(ssid);
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Intentos: ");
    Serial.println(intentos);
  }
}