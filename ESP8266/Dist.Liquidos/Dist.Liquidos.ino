#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>

//V_sonido = 343 m/s = 34300 cm/s.
//[t] = us = 1x10^(-6)s.
//x = Vs*t = (343 / 10000) cm.
//2 porque la onda ultrasónica incide sobre el ojbeto y regresa al sensor, pero solo nos interesa
//el tiempo que tarda en llegar al objeto, entonces dividimos el tiempo total, que tarda en ir y
//volver por dos.
#define V_sonido 0.0343

//SPI.
#define SS_PIN 15  //D8 (GPIO 15).
//Resetear el ESP8266.
#define RST_PIN 2  //D3 (GPIO 4).

//Echo esta conectado en el ESP a D1 (GPIO5).
const int echo = 5;
//Trig esta conectado en el ESP a D2 (GPIO4).
const int trig = 4;
//tiempo que Echo se mantiene en ALTO.
float t_echo_H;
//distancia que recorre el sonido en ir y volver.
float x_rec_comp;

unsigned long ultima_leida = 0;

//Tx conectado a D3 (GPIO0).
static const int Tx = 0;
//Rx conectado a D0 (GPIO16).
static const int Rx = 16;

float Latitud, Longitud;

//Objeto TinyGPS++.
TinyGPSPlus gps;

//Objetos Ticker
Ticker timer_nivel;
Ticker timer_gps;

//Seteamos los pines de transmición y recepción del módulo GPS.
SoftwareSerial SerialGPS(Tx, Rx);

//Nombre y contraseña de la red WiFi.
const char* ssid     = "Juani";
const char* password = "JIG11111010001";

 int  contador_nivel = 20000,
      contador_gps   = 5000,
      flag           = 0;

//Crear un cliente de tipo WiFiClient.
WiFiClient client;

MFRC522 mfrc522(SS_PIN, RST_PIN);

//TAGS RFID.
String card_list[2] = { "3A3F12B0", 
                        "833AAFA9" };

//Atributos de los usuarios.
//ID|datatime|latitud|longitud|nivel
String user_info[2][5] =  {{ "I4", "datatime1", "latitu4", "longitu4", "nivel1" }, 
                           { "ID2", "datatime2", "latitud2", "longitud2", "nivel2" }};

int total_card;
String card_num;

void connectToWiFi();
String getCardNumber();
void showData();
void accessDenied();
void func_rfid();
void func_nivel();
void func_gps();

void setup() {
  //Inicializamos la comunicación serial a 9600 bps.
  Serial.begin(9600);
  SerialGPS.begin(9600);
  connectToWiFi();
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println();
  //Trig emite una señal.
  pinMode(trig, OUTPUT);
  //Echo recibe la señal reflejada.
  pinMode(echo, INPUT);

  timer_nivel.attach(20, func_nivel);
  timer_gps.attach(5, func_gps);
}

void loop() 
{
  func_rfid();
  if(WiFi.status() != WL_CONNECTED)
  {
    connectToWiFi();
  }
}

String getCardNumber() 
{
  String UID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    UID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    UID += String(mfrc522.uid.uidByte[i], HEX);
  }
  UID.toUpperCase();
  return UID;
}

void showData()
{
  String ID, datatime, lat, lonG, level;
  boolean user_found = false;
  total_card = sizeof(card_list) / sizeof(card_list[0]);

  for (int i = 0; i < total_card; i++) {
    String check_num = card_list[i];
    if (card_num.equals(check_num)) {
      user_found = true;

      Serial.print("TAG : ");
      Serial.println(card_num);

      Serial.println("------------");
    }
  }
  if (user_found == false) {
    Serial.print("Card ID : ");
    Serial.print(card_num);
    Serial.println(" have not registered.");
    Serial.println("------------");
  }
  delay(2000);
}

void accessDenied() 
{
  Serial.print("TAG: ");
  Serial.print(card_num);
  Serial.println(" no tiene acceso.");
  Serial.println("------------");
  delay(2000);
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

void connectToHost()
{
  String ID, datatime, lat, lonG, level;
  Serial.println("Conectando a host");
  while (!client.connect("78.108.218.45", 25244)) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Conexión establecida");

  ID       = user_info[1][0];
  datatime = user_info[1][1];
  lat      = user_info[1][2];
  lonG     = user_info[1][3];
  level    = user_info[1][4];
  
  Serial.println(ID);
  Serial.println(datatime);
  Serial.println(lat);
  Serial.println(lonG);
  Serial.println(level);

  client.println(ID);
  client.println(datatime);
  client.println(lat);
  client.println(lonG);
  client.println(level);
  client.stop();
  delay(2000);
}

void func_rfid()
{
  unsigned long t_offset = 3000;
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial() || millis() < ultima_leida + t_offset)
  { 
    return;
  }

  card_num = getCardNumber();
  ultima_leida = millis();
  Serial.print("[RFID] TAG : ");
  Serial.println(card_num);
}

void func_gps()
{
  Serial.print("[GPS] Acualizando GPS...");
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
  Serial.println("GPS");
  Serial.print("Latitud: ");
  Serial.println(Latitud);
  Serial.print("Longitud: ");
  Serial.println(Longitud);
  Serial.println("----------------------------------------");
}

void func_nivel()
{
  Serial.print("[NIVEL] Midiendo nivel...");
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
}
