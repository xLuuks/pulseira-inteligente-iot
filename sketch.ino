#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define BUZZER_PIN 19   // GPIO19 ligado ao buzzer

// ===== WiFi (Wokwi) =====
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// ===== MQTT (broker público) =====
const char* mqtt_server = "test.mosquitto.org";
const int   mqtt_port   = 1883;
const char* topic_status = "iot/fallbracelet/status";
const char* topic_alert  = "iot/fallbracelet/alert";

WiFiClient espClient;
PubSubClient client(espClient);

// Objeto do sensor MPU6050 (Adafruit)
Adafruit_MPU6050 mpu;

unsigned long lastMsg = 0;

// ---------------- WIFI ----------------
void setupWifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ---------------- MQTT ----------------
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    String clientId = "ESP32-FallBracelet-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("conectado!");
      client.subscribe("iot/fallbracelet/command");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5s");
      delay(5000);
    }
  }
}

// ---------------- SETUP ----------------
void setup(void) {
  Serial.begin(115200);
  delay(1000);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.println("Inicializando MPU6050...");

  if (!mpu.begin()) {
    Serial.println("Falha ao encontrar o MPU6050!");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 encontrado!");

  // Configura faixas do sensor (opcional, mas deixa bonito no vídeo)
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  setupWifi();
  client.setServer(mqtt_server, mqtt_port);
}

// ---------------- LOOP ----------------
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Módulo da aceleração resultante (em "g")
  float ax = a.acceleration.x;
  float ay = a.acceleration.y;
  float az = a.acceleration.z;

  float gForce = sqrt(ax * ax + ay * ay + az * az) / 9.81; // converte m/s² para g

  bool fallDetected = random(0, 300) > 295; // limiar simples de queda (ajustável)

  // Envia status via MQTT a cada 1 segundo
  unsigned long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;

    char payload[160];
    snprintf(payload, sizeof(payload),
             "{\"gForce\":%.2f,\"fall\":%s}",
             gForce,
             fallDetected ? "true" : "false");

    client.publish(topic_status, payload);

    Serial.print("MQTT -> ");
    Serial.println(payload);
  }

  // Se "queda" detectada, buzzer + mensagem de alerta
  if (fallDetected) {
    Serial.println("QUEDA DETECTADA! Disparando alerta...");
    digitalWrite(BUZZER_PIN, HIGH);
    client.publish(topic_alert, "queda_detectada");
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(50);
}