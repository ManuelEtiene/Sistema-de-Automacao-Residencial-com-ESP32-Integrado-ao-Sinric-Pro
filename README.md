# Sistema de Automação Residencial com ESP32, Integrado ao Sirinc Pro

<div align="center">

![GitHub Language Count](https://img.shields.io/github/languages/count/ManuelEtiene/Sistema-de-Automacao-Residencial-com-ESP32-Integrado-ao-Sinric-Pro?style=for-the-badge)
![GitHub Top Language](https://img.shields.io/github/languages/top/ManuelEtiene/Sistema-de-Automacao-Residencial-com-ESP32-Integrado-ao-Sinric-Pro?style=for-the-badge&color=blue)
![Backend](https://img.shields.io/badge/Backend-Node.js-339933?style=for-the-badge&logo=nodedotjs)
![Database](https://img.shields.io/badge/Database-MongoDB-47A248?style=for-the-badge&logo=mongodb)

---

<img src="images/dashboard_preview.jpeg" alt="Dashboard Preview" width="800"/>

</div>

---

## 📋 Sobre o Projeto

Este projeto foi desenvolvido como parte do **Projeto Integrador I** do curso de **Engenharia de Computação** da **UFSC (Campus Araranguá)**. O objetivo principal é implementar um ecossistema de automação residencial de ponta a ponta (Full-Stack IoT), controlando atuadores e monitorando variáveis ambientais.

O sistema destaca-se pela sua arquitetura híbrida robusta (*Edge/Cloud*). O processamento crítico de automação roda localmente no microcontrolador (garantindo resiliência), enquanto o histórico e a monitorização global ocorrem na nuvem através de um servidor Node.js próprio integrado ao MongoDB, com interface via Dashboard Web.

---

## ✨ Funcionalidades

- **Monitoramento Ambiental Constante:** Leitura em tempo real de temperatura, umidade (BME280) e luminosidade (LDR).
- **Gestão Térmica com Histerese:** Controle inteligente do ar-condicionado operando em janelas seguras de temperatura, evitando desgaste do compressor.
- **Corte Preventivo de Energia:** Desligamento automático do ar-condicionado caso a porta permaneça aberta por mais de 10 segundos.
- **Iluminação de Cortesia Inteligente:** Ativação automática da lâmpada ao abrir a porta em ambientes escuros (< 40%), com temporizador de desligamento suave de 3 segundos após o fechamento.
- **Controle Híbrido Estável:** Transição perfeita entre o Modo Automático local e o Modo Manual via requisições do Dashboard.

---

## 📂 Repository Structure

The project is structured into modular directories to separate cloud services from edge computing logic:

```text
📦 SmartHome-IoT
├── 📂 Backend_Frontend/            # Módulo de Nuvem e Interface Web
│   ├── 📂 public/                  # Interface do Dashboard (HTML, CSS, JS)
│   ├── 📄 server.js                # Servidor backend Node.js e API REST
│   ├── 📄 sensorData.js            # Schema do banco de dados MongoDB
│   └── 📄 package.json             # Dependências do Node
├── 📂 Hardware/                    # Documentação da Camada Física
│   ├── 📂 BMP280/                  # Especificações do sensor ambiental
│   ├── 📂 LDR/                     # Especificações do sensor de luminosidade
│   └── 📄 Pinout.md                # Guia completo de conexões do ESP32
├── 📂 Software/                    # Módulo de Edge Computing
│   ├── 📄 Firmware_ESP32.ino       # Código embarcado principal em C++
│   └── 📄 Tutorial_Instalacao.md   # Guia de instalação e upload
└── 📂 images/                      # Imagens para o README.md
```

---

## 🛠️ Tecnologias Utilizadas

### ⚙️ Hardware (Componentes Utilizados)

| Componente | Quantidade | Função / Especificação |
| :--- | :---: | :--- |
| **ESP32** | 1x | Microcontrolador principal |
| **Sensor BME280** | 1x | Leitura de Temperatura, Umidade e Pressão (I2C) |
| **Fotorresistor LDR** | 1x | Sensor de luminosidade (5mm) |
| **Micro-chave (Endstop)** | 1x | Chave fim de curso com alavanca (Sensor de porta) |
| **Módulo Relé (5V)** | 1x | Controle de cargas de potência AC (1 Canal) |
| **Display LCD 16x2** | 1x | Interface visual com Módulo Adaptador I2C (PCF8574) |
| **LED** | 1x | Indicador visual de status da iluminação de cortesia |
| **Resistores de 1kΩ** | 2x | Divisor de tensão (LDR) e limitador de corrente (LED) |
| **Protoboard & Jumpers** | - | Matriz de contatos e cabeamento geral |

### 💻 Software e Protocolos

| Camada / Serviço | Tecnologia | Descrição |
| :--- | :--- | :--- |
| **Backend** | Node.js com Express | Servidor web e API |
| **Banco de Dados** | MongoDB (via Mongoose) | Armazenamento de registros |
| **Frontend** | HTML5, CSS3, JavaScript | Interface com HTTP Polling constante a cada 3s |
| **Mensageria** | Protocolo MQTT | Broker Público HiveMQ (Porta 1883) |

---

## 🗺️ Implementação do Hardware

### Arquitetura de Circuitos e PCB

<div align="center">
  <table border="0">
    <tr>
      <td align="center"><b>Circuito Digital (Wokwi)</b><br/><img src="images/digital_circuit_wowki.jpeg" width="380"/></td>
      <td align="center"><b>Diagrama Elétrico</b><br/><img src="images/electric_diagram.png" width="380"/></td>
    </tr>
    <tr>
      <td align="center"><b>Esquemático da PCB</b><br/><img src="images/schematic_pcb.png" width="380"/></td>
      <td align="center"><b>PCB Visualização Real (3D)</b><br/><img src="images/3d_pcb.png" width="380"/></td>
    </tr>
  </table>
</div>

### Mapeamento de Conexões (Pinout)

*Consulte a pasta `/Hardware/Pinout.md` para as especificações completas.*

| Periférico | Pino VCC | Pino GND | Pino Sinal / Barramento | Descrição |
| :--- | :--- | :--- | :--- | :--- |
| **Display LCD 16x2** | 5V (VIN) | GND | **SDA: GPIO 21 / SCL: GPIO 22** | Interface local (I2C). |
| **Sensor BME280** | 3.3V | GND | **SDA: GPIO 21 / SCL: GPIO 22** | Dados ambientais (I2C). |
| **Módulo Relé (AC)** | 5V (VIN) | GND | **IN: GPIO 25** | Digital OUTPUT. |
| **Chave Switch** | N/A | GND | **Sinal: GPIO 14** | INPUT c/ Pull-up interno. |
| **LDR (Luminosidade)**| 3.3V | GND | **Sinal: GPIO 34** | Analog INPUT. |
| **LED Indicador** | N/A | GND | **Sinal: GPIO 2** | Digital OUTPUT. |

---

## 🧠 Lógica de Automação (Edge Computing)

Toda a lógica temporal e de histerese roda de forma assíncrona usando `millis()`, garantindo que o sistema não congele as leituras.

```text
AUTOMAÇÃO DO AR-CONDICIONADO
├── TEMPERATURA ≥ 26ºC ──> AC ON
├── TEMPERATURA < 24ºC ──> AC OFF
└── PORTA ABERTA > 10s ──> AC OFF (Bloqueio de Segurança)

AUTOMAÇÃO DA ILUMINAÇÃO
├── PORTA ABERTA + LUMINOSIDADE < 40% ──> LUZ ON (Ambiente Escuro)
└── PORTA FECHADA ──────────────────────> LUZ OFF (Após 3 segundos)
```

---

## 🎬 Demonstração do Funcionamento Prático

<div align="center">

Assista ao vídeo demonstrativo completo da automação residencial em funcionamento:

[![Assista no YouTube](https://img.shields.io/badge/YouTube-Assistir_Demonstração-red?style=for-the-badge&logo=youtube&logoColor=white)](https://www.youtube.com/watch?v=j-FZEJOe_pA)

</div>

---

## 🚀 Como Rodar o Projeto

### 1. Hardware e Firmware (ESP32)
1. Abra `/Software/Firmware_ESP32.ino` na Arduino IDE.
2. Instale as bibliotecas (`PubSubClient`, `ArduinoJson`, `Adafruit_BMP280`).
3. Configure o Wi-Fi e certifique-se de que o Broker MQTT está apontando para o HiveMQ.
4. Faça o upload do código para a placa.

### 2. Backend (Servidor e Banco de Dados)
1. Certifique-se de ter o Node.js instalado.
2. Abra o terminal na pasta `Backend_Frontend/`.
3. Execute `npm install` para instalar as dependências (Express, Mongoose, MQTT).
4. Configure o seu arquivo `.env` com a sua URL de conexão do MongoDB.
5. Inicie o servidor com `node server.js`.

### 3. Frontend (Interface Web)
1. Com o servidor Node rodando, a interface estará servida.
2. Acesse pelo navegador a porta local definida no Express (ex: `http://localhost:3000`) ou abra o arquivo `index.html` caso esteja testando o layout isoladamente.

---

## Autores

* **Adir Leonardo Arruda Brasil** ─ [GitHub](https://github.com/leunz)
* **Mamadjan Jaló** ───────── [GitHub](https://github.com/JaloMamadjam)
* **Manuel Etiene da Silva João** ─ [GitHub](https://github.com/ManuelEtiene) 

---
