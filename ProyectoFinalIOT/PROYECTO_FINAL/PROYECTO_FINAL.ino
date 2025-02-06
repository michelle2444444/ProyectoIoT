// 📌 Colocar sus credenciales ORIGINALES
#define BLYNK_TEMPLATE_ID "TMPL2WgwudIxY"
#define BLYNK_TEMPLATE_NAME "ProyectoFinalIOT"
#define BLYNK_AUTH_TOKEN "NMI9BorKtYi53_qUGxuv0106zGmU-J_l"
#define BLYNK_PRINT Serial

// 📌 Librerías necesarias
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

// 📌 SSID y password red WiFi
char ssid[] = "NETLIFE Cris";
char pass[] = "1712053295OK"; 
char token[] = BLYNK_AUTH_TOKEN;

// 📌 Conexión ESP8266 con Arduino
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(2, 3);  // RX, TX
ESP8266 wifi(&EspSerial);

// 📌 Pines de Sensores
#define SENSOR_LLUVIA A0   // Sensor de Agua (V10)
#define FOTORESISTENCIA A1  // FotoResistencia (V9)

// 📌 Pines de Luces
int pinRojo = 4;       // Luz ROJA - Patio (V4)
int pinVerde = 5;      // Luz VERDE - Sala (V5)
int pinBlanco = 6;     // Luz BLANCA - Cocina (V6)
int pinAzul = 7;       // Luz AZUL - Habitación (V7)
int pinAmarillo = 8;   // Luz AMARILLA - Baño (V8)

// 📌 Variables de Estado
bool lluviaDetectada = false; // Para evitar notificaciones repetitivas

// 📌 Timer para ejecución estable
BlynkTimer timer;

// 📌 Función para enviar datos de sensores cada 5 segundos
void enviarDatosSensores() {
    if (Blynk.connected()) {
        int valorLuz = analogRead(FOTORESISTENCIA);
        int lluvia = analogRead(SENSOR_LLUVIA);

        // Enviar datos a Blynk
        Blynk.virtualWrite(V9, valorLuz);
        Blynk.virtualWrite(V10, lluvia);

        // Mostrar datos en consola
        Serial.print("Luz: ");
        Serial.print(valorLuz);
        Serial.print(" | Lluvia: ");
        Serial.println(lluvia);

        // 📌 Iluminación automática del Patio
        if (valorLuz < 300) {  
            digitalWrite(pinRojo, HIGH);
            Blynk.virtualWrite(V15, 1);  
        } else {
            digitalWrite(pinRojo, LOW);
            Blynk.virtualWrite(V15, 0);  
        }

        // 📌 Notificación de lluvia (Solo se envía una vez cuando comienza a llover)
        if (lluvia < 200 && !lluviaDetectada) {  
            Serial.println("🌧️ Lluvia detectada. Enviando notificación...");
            Blynk.logEvent("lluvia_detectada", "¡Está lloviendo!"); // Notificación en Blynk
            lluviaDetectada = true;  // Marcar que ya se notificó la lluvia
        }
    }
}

// 📌 Configuración Inicial
void setup() {
    pinMode(pinRojo, OUTPUT);
    pinMode(pinVerde, OUTPUT);
    pinMode(pinBlanco, OUTPUT);
    pinMode(pinAzul, OUTPUT);
    pinMode(pinAmarillo, OUTPUT);

    Serial.begin(9600);
    EspSerial.begin(9600);  
    delay(100);

    // 📌 Intentar conectar a Blynk
    Blynk.begin(token, wifi, ssid, pass);

    // 📌 Timers
    timer.setInterval(5000L, enviarDatosSensores);
}

void loop() {
    if (Blynk.connected()) {
        Blynk.run();
    }
    timer.run();
}
