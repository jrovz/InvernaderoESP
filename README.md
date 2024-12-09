# Proyecto de Monitoreo de Plantas con ESP32
## Descripción

Este proyecto tiene como objetivo crear un sistema de monitoreo de plantas utilizando un microcontrolador ESP32. El sistema recolecta datos de varios sensores ambientales y de suelo, los procesa, y los transmite a través de una conexión WiFi. Los datos se almacenan en un archivo JSON y se utilizan para interactuar con un modelo de lenguaje natural que responde de manera sarcástica y extrovertida.

![Invernadero](Invernadero.jpg)


## Componentes del Proyecto

### Hardware
- **ESP32**: Microcontrolador con conectividad WiFi y Bluetooth.
- **Sensor de Humedad de Suelo**: Mide la humedad del suelo.
- **Sensor DHT11**: Mide la temperatura y la humedad ambiental.
- **RTC DS1307**: Proporciona la fecha y la hora actual.
- **Relé**: Controla dispositivos externos como luces.

### Software
- **Arduino IDE**: Plataforma de desarrollo utilizada para programar el ESP32.
- **Python**: Lenguaje de programación utilizado para recibir y procesar los datos del sensor, y para interactuar con la API de OpenAI.
- **OpenAI API**: Utilizada para generar respuestas inteligentes y sarcásticas basadas en los datos de los sensores.

## Funcionalidades

1. **Monitoreo de Sensores**:
    - Recopila datos de humedad del suelo, temperatura y humedad ambiental.
    - Controla un relé basado en los niveles de humedad del suelo.

2. **Conexión WiFi**:
    - Conecta el ESP32 a una red WiFi para transmitir datos.

3. **Almacenamiento de Datos**:
    - Guarda los datos de los sensores en un archivo JSON para su posterior análisis.

4. **Interacción Inteligente**:
    - Utiliza un modelo de lenguaje natural (GPT-4) para responder a preguntas de manera sarcástica y extrovertida, simulando ser una planta.

## Ejecución del Proyecto

### Parte Arduino

1. **Configuración del Hardware**: Conectar los sensores y el relé al ESP32.
2. **Programación del ESP32**: Subir el código al ESP32 utilizando el Arduino IDE.

### Parte Python

1. **Configuración del Entorno**:
    - Instalar las dependencias necesarias (`requests`, `dotenv`, `openai`).
    - Configurar las variables de entorno para la API de OpenAI.

2. **Ejecución del Script**:
    - Ejecutar el script Python para recibir y procesar los datos de los sensores, y generar respuestas utilizando la API de OpenAI.

## Uso del Proyecto

1. **Conectar y Configurar el ESP32**: Asegúrate de que el ESP32 esté conectado correctamente a los sensores y al relé, y que esté conectado a la red WiFi.
2. **Ejecutar el Script Python**: Corre el script para empezar a recibir datos y generar respuestas.



## Instalación

1. Clona el repositorio a tu dispositivo local:

    ```sh
    git clone https://github.com/tu-usuario/proyecto-monitoreo-plantas.git
    cd proyecto-monitoreo-plantas
    ```

2. Abre el proyecto en Arduino IDE.

    - Abre el Arduino IDE.
    - Navega a `Archivo -> Abrir` y selecciona el archivo `.ino` del proyecto clonado.

3. Conecta tu placa ESP32 a tu computadora y carga el sketch.

    - Asegúrate de seleccionar la placa ESP32 y el puerto correcto en `Herramientas -> Placa` y `Herramientas -> Puerto`.
    - Haz clic en el botón de cargar (flecha hacia la derecha) para subir el sketch a la placa ESP32.

4. Instala las dependencias de Python:

    ```sh
    pip install requests python-dotenv openai
    ```

5. Configura las variables de entorno para la API de OpenAI:

    - Crea un archivo `.env` en el directorio raíz del proyecto.
    - Añade tu clave de API de OpenAI al archivo `.env`:

    ```env
    OPENAI_API_KEY=tu_clave_de_api
    ```

6. Ejecuta el script Python para comenzar a recibir y procesar los datos de los sensores:

    ```sh
    python script.py
    ```

    Nota: Asegúrate de que la dirección IP del ESP32 en el script Python (`data_request`) sea la correcta.

Ahora tu sistema de monitoreo de plantas está listo para funcionar. Puedes interactuar con él a través de la red WiFi y recibir respuestas inteligentes y sarcásticas simulando ser la planta monitoreada.

## Contribuciones

¡Las contribuciones son bienvenidas! Si te interesa contribuir al desarrollo de este proyecto, no dudes en enviar pull requests con mejoras, correcciones de errores o nuevas características. Tu ayuda es fundamental para hacer de este proyecto una herramienta aún más poderosa y útil para la comunidad.


## Ejemplo de Interacción

```plaintext
Human: ¿Qué tal está el clima hoy?
Plant: ¡Hey! Aquí sudando a 35 grados centígrados y con 80 por ciento de humedad. ¡UN DÍA PERFECTO para una sauna, pero NO para una planta como yo!

´´´
