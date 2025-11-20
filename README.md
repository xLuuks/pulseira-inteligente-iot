# 📘 Pulseira Inteligente para Detecção de Quedas em Idosos — IoT

Projeto desenvolvido para a disciplina **Objetos Inteligentes Conectados**, utilizando ESP32, sensor MPU-6050, buzzer e o protocolo MQTT para comunicação em tempo real.

---

## 🎯 Objetivo do Projeto

Desenvolver uma pulseira inteligente capaz de:

- Detectar quedas por meio de um acelerômetro/giroscópio  
- Emitir alerta local através de um buzzer  
- Enviar alerta remoto via MQTT para cuidadores e familiares  
- Contribuir para segurança e bem-estar de pessoas idosas  

---

## 🛠 Tecnologias Utilizadas

- **ESP32** – Microcontrolador com Wi-Fi integrado  
- **MPU-6050** – Sensor acelerômetro + giroscópio  
- **Buzzer piezoelétrico** – Alerta sonoro local  
- **MQTT** – Protocolo leve de comunicação (Mosquitto / HiveMQ)  
- **Wokwi** – Simulação do hardware  
- **Fritzing** – Diagrama do circuito  

---

## 📡 Comunicação MQTT

Tópicos utilizados:

- **Status:** `iot/fallbracelet/status`  
- **Alerta:** `iot/fallbracelet/alert`

Formato das mensagens:

```json
{
  "gForce": 1.00,"fall": false
}
