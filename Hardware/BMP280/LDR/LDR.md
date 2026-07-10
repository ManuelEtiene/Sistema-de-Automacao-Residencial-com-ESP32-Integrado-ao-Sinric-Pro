
# Sensor de Luminosidade - LDR

O sensor LDR é utilizado no projeto para identificar a quantidade de luz no ambiente, permitindo que o sistema gerencie a iluminação de cortesia da sala de forma inteligente e eficiente.

<img width="1119" height="1118" alt="image" src="https://github.com/user-attachments/assets/5c36adbc-e139-47a6-b1a3-a658792fc5c0" />


## Dados Técnicos do Sensor LDR

| Característica | Descrição |
| :--- | :--- |
| **Tipo** | Resistor Dependente de Luz (Fotoresistor) |
| **Tensão de Operação** | 3.3V a 5V (Operando em 3.3V) |
| **Interface de Comunicação** | Saída Analógica (Tensão proporcional à luz) |
| **Resistência no Escuro** | > 1 MΩ |
| **Resistência na Luz (10 Lux)**| 10 kΩ a 20 kΩ |
| **Mapeamento de Saída** | Processado em percentagem no firmware (0% a 100%) |

## Pinagem do LDR (Montagem com divisor de tensão)

| Pino | Função | Descrição |
| :---: | :---: | :--- |
| 1 | VCC | Alimentação de 3.3V vinda do microcontrolador |
| 2 | Saída (Sinal) | Conectado diretamente à GPIO 34 (ADC1) do ESP32 |
| 3 | GND | Referência de terra do circuito |

## Componentes Associados
* Fotoresistor LDR de 5mm
* Resistor de 10kΩ (utilizado para criar o divisor de tensão pull-down)

## Recomendações
* **Resistor Pull-down:** A utilização do resistor de 10kΩ em série com o LDR é obrigatória para obter uma leitura analógica linear de tensão na GPIO.
* **Atenuação do ADC:** No ESP32, configure a atenuação do conversor analógico-digital para `ADC_11db` caso queira realizar leituras completas na faixa de 0V a 3.3V.

## Exemplo de Código

Segue o código de teste utilizado para obter os valores de luminosidade convertidos em escala percentual:

```cpp
const int pinoLDR = 34; // GPIO selecionada para leitura analógica

void setup() {
  Serial.begin(115200);
  pinMode(pinoLDR, INPUT);
}

void loop() {
  int leituraBruta = analogRead(pinoLDR);
  
  // Mapeia o valor bruto do ADC de 12 bits (0-4095) para percentual (0-100%)
  int luminosidade = map(leituraBruta, 0, 4095, 0, 100);
  
  Serial.print("Leitura ADC: ");
  Serial.print(leituraBruta);
  Serial.print(" | Luminosidade: ");
  Serial.print(luminosidade);
  Serial.println("%");

```
## Saída no Terminal 
```text
Iniciando leitura do LDR...
Leitura ADC: 1433 | Luminosidade: 35%
---------------------------------------
Leitura ADC: 1474 | Luminosidade: 36%
---------------------------------------
Leitura ADC: 2457 | Luminosidade: 60%
---------------------------------------
Leitura ADC: 2866 | Luminosidade: 70%
---------------------------------------
Leitura ADC: 3685 | Luminosidade: 90%
---------------------------------------
