#include <SoftwareSerial.h>
#include <TinyGPS++.h>

//Tx conectado a D3 (GPIO0).
static const int Tx = 0;
//Rx conectado a D0 (GPIO16).
static const int Rx = 16;

float Latitud, Longitud;

//Objeto TinyGPS++.
TinyGPSPlus gps;

//Seteamos los pines de transmición y recepción del módulo GPS.
SoftwareSerial SerialGPS(Tx, Rx);

void setup() {
  //Inicializamos la comunicación serial a 9600 bps.
  Serial.begin(9600);
  SerialGPS.begin(9600);
}

void loop() {
  while (SerialGPS.available() > 0)
  {
    if(gps.encode(SerialGPS.read()))
    {
      if(gps.location.isValid())
      {
        Latitud = gps.location.lat();
        Longitud = gps.location.lng();
      }
    }
  }

  Serial.print("Latitud: ");
  Serial.println(Latitud);
  Serial.print("Longitud: ");
  Serial.println(Longitud);
  delay(1000);
}
