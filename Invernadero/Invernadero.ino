#include <WiFi.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <RTClib.h>

// Definición de la red WiFi
const char* ssid = "Alex";
const char* password = "Palmeras";

// Definición de los pines de los sensores, relé y luces
const int sensorHum1 = 34;  // Sensor de humedad de suelo
const int DHTPin = 4;       // El pin al que está conectado el DHT11
const int relayPin = 33;    // Relé conectado al pin digital
const int lightPin = 32;    // Pin de las luces

// Umbrales de humedad
const int umbralSeco = 30;    // Umbral bajo de humedad (30%)
const int umbralHumedo = 70;  // Umbral alto de humedad (70%)

// Rango válido para los valores de temperatura y humedad ambiental
const float tempMin = 0.0;
const float tempMax = 80.0;
const float humAmbientalMin = 0.0;
const float humAmbientalMax = 100.0;

// Horarios para control de luces
const int horaEncendido = 6;  // Hora de encendido (6 am)
const int horaApagado = 21;   // Hora de apagado (9 pm)

// Tipo de sensor DHT
#define DHTTYPE DHT11
DHT dht(DHTPin, DHTTYPE);

// Objeto para el servidor web
WiFiServer server(80);

// Definición del RTC
RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup() {
  // Configuración de los pines
  pinMode(sensorHum1, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(lightPin, OUTPUT);  // Configuración del pin de las luces como salida

  // Inicialización del sensor DHT
  dht.begin();

  // Inicialización del puerto serie
  Serial.begin(115200);
  delay(10);

  // Inicialización del RTC
  initRTC();

  // Conexión a la red WiFi
  connectToWiFi();

  // Imprimir la dirección IP
  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Iniciar el servidor web
  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    handleClient(client);
  }

  controlLights();  // Controlar las luces basado en la hora actual
}

void connectToWiFi() {
  int attempts = 0;
  Serial.println("Conectando a WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("");
    Serial.println("Error al conectar a WiFi. Reiniciando...");
    ESP.restart();
  }
}

void handleClient(WiFiClient client) {
  Serial.println("Nuevo Cliente.");
  String currentLine = "";
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      Serial.write(c);
      if (c == '\n') {
        if (currentLine.length() == 0) {
          sendSensorData(client);
          break;
        } else {
          currentLine = "";
        }
      } else if (c != '\r') {
        currentLine += c;
      }
    }
  }
  client.stop();
  Serial.println("Cliente desconectado.");
}

void sendSensorData(WiFiClient client) {
  // Leer la humedad del suelo
  int humedad1 = analogRead(sensorHum1);
  int porcentajeHum1 = map(humedad1, 4095, 2237, 0, 100);
  porcentajeHum1 = constrain(porcentajeHum1, 0, 100);  // Asegura que el valor está entre 0% y 100%

  // Leer la temperatura y la humedad ambiental
  float temp1 = dht.readTemperature();
  float humAmbiental1 = dht.readHumidity();

  // Inicializar variable de error
  String error = "";

  // Verificar si la temperatura y la humedad están dentro de los rangos válidos
  if (temp1 < tempMin || temp1 > tempMax || humAmbiental1 < humAmbientalMin || humAmbiental1 > humAmbientalMax) {
    error = "Error: Datos fuera de rango";
  }

  // Control del relé basado en la humedad de suelo
  if (porcentajeHum1 < umbralSeco) {
    digitalWrite(relayPin, HIGH);  // Activa el relé
  } else if (porcentajeHum1 > umbralHumedo) {
    digitalWrite(relayPin, LOW);  // Desactiva el relé
  }

  // Obtener fecha y hora actual
  String currentDateTime = getCurrentDateTime();

  // Crear el objeto JSON con los datos del sensor
  DynamicJsonDocument doc(1024);
  doc["humedadSuelo"] = porcentajeHum1;
  doc["Temperatura"] = static_cast<int>(temp1 * 10) / 10.0;   // Redondeo a 1 decimal
  doc["HumedadAmbiental"] = static_cast<int>(humAmbiental1);  // Redondeo a entero
  doc["FechaHora"] = currentDateTime;
  if (error != "") {
    doc["error"] = error;  // Añadir mensaje de error si existe
  }

  // Enviar la respuesta JSON al cliente
  sendJsonResponse(client, doc);
}

void sendJsonResponse(WiFiClient client, DynamicJsonDocument doc) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:application/json");
  client.println();

  serializeJson(doc, client);
  client.println();

  serializeJson(doc, Serial);
  Serial.println();
}

void initRTC() {
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1);
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Ajusta la fecha y hora del RTC a la fecha y hora de compilación
}

String getCurrentDateTime() {
  DateTime now = rtc.now();
  String dateTime = "";
  dateTime += String(now.year()) + "/";
  dateTime += String(now.month()) + "/";
  dateTime += String(now.day()) + " (";
  dateTime += String(daysOfTheWeek[now.dayOfTheWeek()]) + ") ";
  dateTime += String(now.hour()) + ":";
  dateTime += String(now.minute()) + ":";
  dateTime += String(now.second());
  return dateTime;
}

void controlLights() {
  DateTime now = rtc.now();
  int currentHour = now.hour();

  if (currentHour >= horaEncendido && currentHour < horaApagado) {
    digitalWrite(lightPin, HIGH);  // Encender las luces
  } else {
    digitalWrite(lightPin, LOW);   // Apagar las luces
  }
}

