
/*
    Se importan varias bibliotecas necesarias para el funcionamiento del código, 
    incluyendo bibliotecas para el manejo de la pantalla OLED, el sensor DHT11 y el RTC DS1302.
*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

#define SCREEN_WIDTH 128  // Ancho de la pantalla OLED en píxeles
#define SCREEN_HEIGHT 64  // Alto de la pantalla OLED en píxeles

// Declaración para una pantalla SSD1306 conectada a I2C (pines SDA, SCL)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHT_SENSOR_PIN D6  // El pin D6 del ESP8266 conectado al sensor DHT11
#define DHT_SENSOR_TYPE DHT11

// Pines disponibles para los modulos Relé
#define Relay1 D7
#define Relay2 D8

ThreeWire myWire(D4, D5, D3);  // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);


DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

void setup() {
  Serial.begin(115200);
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Dirección 0x3D para 128x64
    Serial.println(F("Error al asignar SSD1306"));
    for (;;)
      ;
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.display();

  dht_sensor.begin();  // Inicializar el sensor DHT
  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);

  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();

  if (!Rtc.IsDateTimeValid()) {
    // Common Causes:
    //    1) first time you ran and the device wasn't running yet
    //    2) the battery on the device is low or even missing

    Serial.println("RTC lost confidence in the DateTime!");
    Rtc.SetDateTime(compiled);
  }

  if (Rtc.GetIsWriteProtected()) {
    Serial.println("RTC was write protected, enabling writing now");
    Rtc.SetIsWriteProtected(false);
  }

  if (!Rtc.GetIsRunning()) {
    Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled) {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  } else if (now > compiled) {
    Serial.println("RTC is newer than compile time. (this is expected)");
  } else if (now == compiled) {
    Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }
}

void loop() {
  // Leer humedad
  float humedad = dht_sensor.readHumidity();
  // Leer temperatura en Celsius
  float temperatura_C = dht_sensor.readTemperature();
  // Leer temperatura en Fahrenheit
  float temperatura_F = dht_sensor.readTemperature(true);

  RtcDateTime now = Rtc.GetDateTime();

  printDateTime(now);
  Serial.println();

  if (!now.IsValid()) {
    // Common Causes:
    //    1) the battery on the device is low or even missing and the power line was disconnected
    Serial.println("RTC lost confidence in the DateTime!");
  }

  delay(1000);  // ten seconds
  // Comprobar si la lectura es exitosa o no
  if (isnan(temperatura_C) || isnan(temperatura_F) || isnan(humedad)) {
    Serial.println("Error al leer el sensor DHT!");
  } else {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);  // Mover el cursor a la posición (0, 0)

    // Mostrar la humedad
    display.println("INVERNADERO ESP8266");
    // Dibujar una línea horizontal
    display.drawLine(0, 10, 127, 10, WHITE);
    display.println();

    // Mostrar la hora
    display.print("Hora: ");
    if (now.Hour() < 10) {
      display.print("0");  // Agregar un cero adelante si la hora es menor que 10
    }
    display.print(now.Hour());
    display.print(":");
    if (now.Minute() < 10) {
      display.print("0");  // Agregar un cero adelante si el minuto es menor que 10
    }
    display.print(now.Minute());
    display.println();

    // Mostrar la humedad
    display.print("Humedad: ");
    display.print(humedad);
    display.println("%");

    // Mostrar la temperatura en Celsius
    display.print("Temperatura: ");
    display.print(temperatura_C);
    display.println("°C");
  }

  display.drawLine(0, 40, 127, 40, WHITE);
  display.println();
  digitalWrite(Relay2, HIGH);

  if (now.Hour() >= 6 && now.Hour() < 21) {
    digitalWrite(Relay1, LOW);
    display.print("Luces Encendidas");
    display.println("!");
  } else {
    digitalWrite(Relay1, HIGH);
    display.print("Luces Apagadas");
    display.println("!");
  }
  display.display();  // Mostrar todo en la pantalla OLED
  // Esperar 2 segundos entre lecturas
  delay(1000);
}


#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt) {
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
  Serial.print(datestring);
}