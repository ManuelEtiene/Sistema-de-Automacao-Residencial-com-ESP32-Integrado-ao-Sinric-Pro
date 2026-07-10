<img width="684" height="624" alt="image" src="https://github.com/user-attachments/assets/cdfa40ad-4eac-48ab-b29a-87b3ec90b765" />
```markdown
# Configuração de Hardware e Mapeamento de Pinos

## 📌 Pinout do ESP32

| Periférico | Pino ESP32 | Tipo de Sinal / Observação |
| :--- | :--- | :--- |
| **BMP280 (VCC)** | 3.3V | Alimentação do sensor |
| **BMP280 (GND)** | GND | Referência de terra |
| **BMP280 (SDA)** | GPIO 21 | Barramento I2C (Dados Compartilhados) |
| **BMP280 (SCL)** | GPIO 22 | Barramento I2C (Clock Compartilhado) |
| **LCD 16x2 (VCC)** | 5V (VIN) | Alimentação do Display (Adaptador I2C) |
| **LCD 16x2 (GND)** | GND | Referência de terra |
| **LCD 16x2 (SDA)** | GPIO 21 | Barramento I2C (Dados Compartilhados) |
| **LCD 16x2 (SCL)** | GPIO 22 | Barramento I2C (Clock Compartilhado) |
| **Sensor Porta** | GPIO 14 | Digital INPUT com PULLUP Interno |
| **Sensor LDR** | GPIO 34 | Analógico INPUT (Mapeado de 0 a 100%) |
| **Módulo Relé** | GPIO 25 | Digital OUTPUT (Controlo do AC) |
| **LED Lâmpada** | GPIO 2 | Digital OUTPUT (Iluminação da Sala) |

## 🔌 Diagrama Elétrico e Conexões
<img width="530" height="389" alt="image" src="https://github.com/user-attachments/assets/2e5ee2c7-7fdf-447b-9a46-320258b22925" />

