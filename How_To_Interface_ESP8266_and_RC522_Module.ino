#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>

#define SS_PIN 15  //D8
#define RST_PIN 12  //D4


//Nombre y contraseña de la red WiFi.
const char* ssid     = "Juani";
const char* password = "JIG11111010001";

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

void setup() 
{
  pinMode(11, OUTPUT);
  digitalWrite(11, LOW);
  Serial.begin(9600);
  connectToWiFi();
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println();
}

void loop() 
{
  digitalWrite(11, LOW);
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  card_num = getCardNumber();

  if (card_num == "833AAFA9")
  {
    digitalWrite(11, HIGH);
    digitalWrite(11, LOW);
    delay(5000);
    showData();
    connectToHost();
  }

  else
  {
    accessDenied();
    digitalWrite(11, LOW);
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

      ID = user_info[i][0];
      datatime = user_info[i][1];
      lat = user_info[i][2];
      lonG = user_info[i][3];
      level = user_info[i][4];

      Serial.print("TAG : ");
      Serial.println(card_num);

      Serial.print("ID : ");
      Serial.println(ID);

      Serial.print("datatime : ");
      Serial.println(datatime);

      Serial.print("lat : ");
      Serial.println(lat);

      Serial.print("lonG : ");
      Serial.println(lonG);

      Serial.print("nivel : ");
      Serial.println(level);

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