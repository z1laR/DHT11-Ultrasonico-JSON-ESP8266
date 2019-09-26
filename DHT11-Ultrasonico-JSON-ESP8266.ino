#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <DHT.h>

const char* ssid = "";
const char* password = "";

#define trigPin D6
#define echoPin D5

#define DHTTYPE DHT11
#define DHTPIN D2

DHT dht(DHTPIN, DHTTYPE);

int duracion;
int distancia;

void setup()
{
  Serial.begin(115200);

  dht.begin();
  delay(2000);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Conectado inalambricamente");  
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  higrometro();
  ultrasonico();
}

int higrometro()
{
  int h = dht.readHumidity();
  int t = dht.readTemperature();

  Serial.println("");
  Serial.println("Temperatura: ");
  Serial.print(t);
  Serial.println(" ° C");
  Serial.println("Humedad: ");
  Serial.print(h);
  Serial.println(" %");

  postHigrometro(t, h);

  delay(2000);
}

int ultrasonico()
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(1);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duracion = pulseIn(echoPin, HIGH);

    distancia = (duracion/2) / 29.1;

    Serial.println("Distancia: ");
    Serial.print(distancia);
    Serial.print(" cm");
    Serial.print("\n");

    postUltrasonico(distancia);

    delay(2000);
}

void postHigrometro(int t, int h)
{
  HTTPClient http;
  String json = "{}";
  String server = "http://192.168.1.104:3000/api/higrometro/temperatura/";
  server.concat(t);
  server.concat("/");
  server.concat("humedad");
  server.concat("/");
  server.concat(h);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  Serial.println(server);
  http.begin(server);

  http.addHeader("Content-Type", "application/json");
  Serial.println("");
  http.POST(json);
  http.writeToStream(&Serial);
  http.end();
  
  Serial.println("FIN MyPost");
  Serial.println("");
}

void postUltrasonico(int distancia)
{
  HTTPClient http;
  String json = "{}";
  String server = "http://192.168.1.104:3000/api/ultrasonico/";
  server.concat(distancia);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  Serial.println(server);
  http.begin(server);

  http.addHeader("Content-Type", "application/json");
  Serial.println("");
  http.POST(json);
  http.writeToStream(&Serial);
  http.end();
  
  Serial.println("FIN MyPost");
  Serial.println("");
}
