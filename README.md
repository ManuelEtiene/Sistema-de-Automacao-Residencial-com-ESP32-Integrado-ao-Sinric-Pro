# Sistema de Automação Residencial com ESP32 e Sinric Pro

# Descrição

Este projeto faz parte do **Projeto Integrador I** da **UFSC (Campus Araranguá)**. O objetivo é desenvolver um sistema de automação acessível, permitindo o controle de dispositivos residenciais via web e comando de voz, e simultaneamente envolvendo o hardware (ESP32, sensores).
# Objetivos do Projeto 
* **Automatizar o Ar-Condicionado:** Controle inteligente de temperatura por histerese e segurança com corte baseado em porta aberta.
* **Monitorizar Variáveis Ambientais:** Leitura em tempo real de temperatura e luminosidade local.
* **Iluminação Inteligente:** Acionamento de cortesia temporizado baseado em sensor de presença/porta e luminosidade ambiente.
* **Controlo Duplo (Híbrido):** Permitir a comutação estável entre o Modo Automático local e o Modo Manual remoto via dashboard.


# Funcionalidades
* Controle de iluminação/cargas via interface web.
* Monitoramento em tempo real (Feedback de estado).
* Integração com assistentes de voz (Alexa).
* Interface responsiva para dispositivos móveis.

# Tecnologias Utilizadas

* ## Hardware:
* **Microcontrolador:** ESP32 (Arquitetura Xtensa Dual-Core de 32 bits, Wi-Fi e Bluetooth integrados).
* **Sensores:** Sensor de pressão e temperatura BMP280 (I2C) e Sensor de Luminosidade LDR (Analógico).
* **Atuadores:** Módulo Relé (Ar-Condicionado) e LED (Lâmpada da Sala).
* **Interface Local:** Display LCD 16x2 com módulo adaptador I2C PCF8574.
* **Plataforma Cloud:** Sinric Pro
* 
* # Software
* Programação do ESP32 utilizando Arduino IDE
* **Frontend:** HTML5, CSS3 e JavaScript
* **Comunicação:** Wi-Fi e Protocolo MQTT/Websockets
* 
### Transporte e Protocolos
* **Rede:** Wi-Fi Local.
* **Protocolo:** MQTT (Message Queuing Telemetry Transport) via HiveMQ Público (porta 1883).
  * *Tópico Telemetria:* `mamadjam123/iot/sensores`
  * *Tópico Comandos:* `mamadjam123/iot/comandos`

### Backend & Frontend
* **Servidor & API:** Node.js com Express.
* **Banco de Dados:** MongoDB com Mongoose (Armazenamento NoSQL de documentos com timestamp).
* **Interface do Utilizador:** Dashboard responsivo com HTML5, CSS3, Bootstrap 5 e FontAwesome.
* **Gráficos:** Chart.js atualizado via HTTP Polling estável a cada 3 segundos.

## Lógica de Funcionamento
### A. Modo Automático
* **Gestão Térmica:** Se a temperatura for $\ge 26.0\text{ °C}$, o AC liga. Se arrefecer para $\le 24.0\text{ °C}$, desliga. Se a porta ficar aberta por mais de 10 segundos, o AC desliga preventivamente.
* **Gestão de Iluminação:** A luz acende se a porta for aberta **E** a luminosidade for $< 40\%$. Ao fechar a porta, mantém-se ligada por 3 segundos como cortesia.

### B. Modo Manual
Qualquer comando direto vindo da Dashboard (`AC_ON`, `AC_OFF`, `LUZ_ON`, `LUZ_OFF`) assume o controlo, desativando a automação local. O sistema retorna ao modo automático apenas quando o comando `AUTO_ON` for enviado.

## Estrutura do Packet de Telemetria (JSON)
## Lógica de Funcionamento
## Como Rodar o Projeto**


1. **Hardware:**
   - Carregue o código `.ino` (Arduino IDE) para o seu ESP32.
   - Configure as credenciais do seu Wi-Fi e as chaves do **Sinric Pro** no código.

2. **Interface Web:**
   - Clone este repositório.
   - Abra o arquivo `index.html` no seu navegador.
   - 
## Estrutura do Packet de Telemetria (JSON)
Enviado a cada 5 segundos pelo ESP32 via `ArduinoJson`:
```json
{
  "temperatura": 25.43,
  "luminosidade": 35,
  "porta": false,
  "ac": true,
  "modoManual": false,
  "luz": false
}



