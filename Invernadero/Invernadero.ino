// Código desarrollado por Ing. Juan Felipe Rodriguez Valencia
// para controlar un invernadero con un ESP8266, incluyendo funciones para leer sensores, controlar luces y el ventilador.
// Fecha: 11/04/2024

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

#define SCREEN_WIDTH 128  // Ancho de la pantalla OLED en píxeles
#define SCREEN_HEIGHT 64  // Alto de la pantalla OLED en píxeles

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHT_SENSOR_PIN D6
#define DHT_SENSOR_TYPE DHT11

#define Relay1 D7
#define Relay2 D8

ThreeWire myWire(D4, D5, D3);
RtcDS1302<ThreeWire> Rtc(myWire);

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

// Macro para calcular el número de elementos en un array
#define countof(a) (sizeof(a) / sizeof(a[0]))

// Función para leer los sensores de temperatura y humedad
void readSensors(float& humedad, float& temperatura_C, float& temperatura_F) {
  humedad = dht_sensor.readHumidity();
  temperatura_C = dht_sensor.readTemperature();
  temperatura_F = dht_sensor.readTemperature(true);
}

// Función para imprimir la fecha y hora en el monitor serial
void printDateTime(const RtcDateTime& dt, size_t bufferSize) {
  char datestring[bufferSize];

  snprintf_P(datestring,
             bufferSize,
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
  Serial.print(datestring);
}

// Función para mostrar los datos en la pantalla OLED
void displayDataOnOLED(float humedad, float temperatura_C, const RtcDateTime& now) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  // Mostrar título
  display.println("INVERNADERO ESP8266");
  display.drawLine(0, 10, 127, 10, WHITE);
  display.println();

  // Mostrar hora
  display.print("Hora: ");
  display.print(now.Hour());
  display.print(":");
  display.print(now.Minute());
  display.println();

  // Mostrar humedad
  display.print("Humedad: ");
  display.print(humedad);
  display.println("%");

  // Mostrar temperatura en Celsius
  display.print("Temperatura: ");
  display.print(temperatura_C);
  display.println("°C");

  display.display(); // Mostrar en la pantalla OLED
}

// Función para controlar las luces del invernadero según el tiempo
void controlLights(const RtcDateTime& now) {
  bool lightsOn = false;

  // Opción 1: Encender de 6am a 10am y de 5pm a 11pm
  if ((now.Hour() >= 6 && now.Hour() < 10) || (now.Hour() >= 17 && now.Hour() < 23)) {
    lightsOn = true;
  }

  // Opción 2: Encender de 6am a 11pm
  /*
  if (now.Hour() >= 6 && now.Hour() < 23) {
    lightsOn = true;
  }
  */

  // Control de luces según la opción seleccionada
  digitalWrite(Relay1, lightsOn ? LOW : HIGH); 
}

// Función para controlar el ventilador del invernadero según el tiempo
void controlFan(const RtcDateTime& now) {
  // Controlar el ventilador: Encenderlo durante los primeros 15 minutos de cada hora
  digitalWrite(Relay2, now.Minute() < 15 ? LOW : HIGH);
}

// Función para mostrar advertencias en la pantalla OLED
void warnings(float humedad, float temperatura_C) {
  // Verificar si la temperatura supera los 35 grados Celsius
  if (temperatura_C > 35) {
    display.clearDisplay();
    display.setTextSize(1.5);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.println("¡ALERTA!");
    display.setCursor(10, 40);
    display.println("Temperatura alta");
    display.display();
    delay(2000);
    display.clearDisplay();
  }

  // Verificar si la humedad cae por debajo del 25%
  if (humedad < 25) {
    display.clearDisplay();
    display.setTextSize(1.5);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.println("¡ALERTA!");
    display.setCursor(10, 40);
    display.println("Humedad baja");
    display.display();
    delay(2000);
    display.clearDisplay();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Error al asignar SSD1306"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.display();

  dht_sensor.begin();
  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);

  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled, sizeof(compiled));
  Serial.println();

  if (!Rtc.IsDateTimeValid()) {
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
  float humedad, temperatura_C, temperatura_F;
  RtcDateTime now = Rtc.GetDateTime();

  printDateTime(now, sizeof(now));

  readSensors(humedad, temperatura_C, temperatura_F);

  if (!isnan(temperatura_C) && !isnan(temperatura_F) && !isnan(humedad)) {
    warnings(humedad, temperatura_C); // Verificar advertencias
    displayDataOnOLED(humedad, temperatura_C, now);
  } else {
    Serial.println("Error al leer el sensor DHT!");
  }

  controlLights(now);
  controlFan(now); // Controlar el ventilador

  delay(1000);
}
