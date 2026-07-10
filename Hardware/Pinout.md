# Configuração de Hardware e Mapeamento de Pinos

## 📌 Pinout do ESP32

| Periférico | Pino ESP32 | Tipo de Sinal / Observação |
| :--- | :--- | :--- |
| **BMP280 (Sensor de Pressão/Temp)** | | |
| └─ VCC | `3.3V` | Alimentação do sensor |
| └─ GND | `GND` | Referência de terra |
| └─ SDA | `GPIO 21` | Barramento I2C (Dados Compartilhados) |
| └─ SCL | `GPIO 22` | Barramento I2C (Clock Compartilhado) |
| **LCD 16x2 (com Adaptador I2C)** | | |
| └─ VCC | `5V (VIN)` | Alimentação do Display |
| └─ GND | `GND` | Referência de terra |
| └─ SDA | `GPIO 21` | Barramento I2C (Dados Compartilhados) |
| └─ SCL | `GPIO 22` | Barramento I2C (Clock Compartilhado) |
| **Sensores e Atuadores** | | |
| └─ Sensor de Porta | `GPIO 14` | Digital INPUT (com PULLUP interno) |
| └─ Sensor LDR | `GPIO 34` | Analog INPUT (Mapeado de 0 a 100%) |
| └─ Módulo Relé | `GPIO 25` | Digital OUTPUT (Controle do AC) |
| └─ LED Lâmpada | `GPIO 2` | Digital OUTPUT (Iluminação da Sala / LED Interno) |

## 🔌 Diagrama Elétrico e Conexões
