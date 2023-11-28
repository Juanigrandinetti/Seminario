#include <SoftwareSerial.h>
#include <TinyGPS++.h>

//V_sonido = 343 m/s = 34300 cm/s.
//[t] = us = 1x10^(-6)s.
//x = Vs*t = (343 / 10000) cm.
//2 porque la onda ultrasónica incide sobre el ojbeto y regresa al sensor, pero solo nos interesa
//el tiempo que tarda en llegar al objeto, entonces dividimos el tiempo total, que tarda en ir y
//volver por dos.
#define V_sonido 0.0343

//Echo esta conectado en el ESP a D1 (GPIO5).
const int echo = 5;
//Trig esta conectado en el ESP a D2 (GPIO4).
const int trig = 4;
//tiempo que Echo se mantiene en ALTO.
float t_echo_H;
//distancia que recorre el sonido en ir y volver.
float x_rec_comp;

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
  //Trig emite una señal.
  pinMode(trig, OUTPUT);
  //Echo recibe la señal reflejada.
  pinMode(echo, INPUT);
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

  //Limpiamos el Trig.
  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  //Seteamos Trig en ALTO por 10us.
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  //Seteamos Trig en BAJO.
  digitalWrite(trig, LOW);

  //Leemos el tiempo en us que echo se mantiene en ALTO.
  t_echo_H = pulseIn(echo, HIGH);
  //Calculamos la distancia que tarda el sonido en incidir en el objeto.
  x_rec_comp = (V_sonido) * (t_echo_H / 2);
  Serial.println("Sensor ultrasonido");
  Serial.print("Tiempo ida y vuelta: ");
  Serial.print(t_echo_H);
  Serial.println(" us");

  //Imprimimos los valores calculados.
  Serial.print("Distancia: ");
  Serial.print(x_rec_comp);
  Serial.println(" cm");
  Serial.println("----------------------------------------");
  Serial.println("GPS");
  Serial.print("Latitud: ");
  Serial.println(Latitud);
  Serial.print("Longitud: ");
  Serial.println(Longitud);
  Serial.println("----------------------------------------");
  delay(5000);
}
