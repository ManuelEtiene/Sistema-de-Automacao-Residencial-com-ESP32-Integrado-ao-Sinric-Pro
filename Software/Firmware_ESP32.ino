
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

// --- Configurações de Rede e MQTT ---
const char* ssid = "NOME_DA_SUA_REDE_WIFI";
const char* password = "SENHA_DO_SEU_WIFI";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

const char* topic_publish = "mamadjam123/iot/sensores";
const char* topic_subscribe = "mamadjam123/iot/comandos";

// --- Definição dos Pinos ---
#define PINO_PORTA 14
#define PINO_LDR   34
#define PINO_RELE  25
#define PINO_LED   2

// --- Instâncias dos Periféricos ---
Adafruit_BMP280 bmp;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Verifique se o endereço do seu LCD é 0x27 ou 0x3F
WiFiClient espClient;
PubSubClient client(espClient);

// --- Variáveis de Controle Global ---
float temperatura = 0.0;
int luminosidade = 0;
bool portaAberta = false;
bool estadoAC = false;
bool estadoLuz = false;
bool modoManual = false;

unsigned long tempoUltimaPublicacao = 0;
unsigned long tempoPortaAberta = 0;
unsigned long tempoLuzCortesia = 0;
bool cronometroPortaAtivo = false;
bool cortesiaAtiva = false;

// --- Função de Inicialização do Wi-Fi ---
void setup_wifi() {
  delay(10);
  lcd.clear();
  lcd.print("Conectando Wi-Fi...");
  Serial.print("Conectando em: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Conectado!");
  lcd.clear();
  lcd.print("Wi-Fi Conectado!");
}

// --- Callback: Processamento de Comandos Recebidos via MQTT ---
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Comando recebido no tópico [");
  Serial.print(topic);
  Serial.print("]: ");
  
  String mensagem = "";
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }
  Serial.println(mensagem);

  // Parse do comando JSON vindo do Dashboard
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, mensagem);
  if (error) {
    Serial.print("Falha no parse do JSON: ");
    Serial.println(error.c_str());
    return;
  }

  // Processamento das Flags de Controle Manual/Automático
  if (doc.containsKey("cmd")) {
    String cmd = doc["cmd"];
    
    if (cmd == "AUTO_ON") {
      modoManual = false;
      Serial.println("Modo Automático Ativado.");
    } else {
      modoManual = true; // Qualquer outro comando ativa o modo manual
      
      if (cmd == "AC_ON") {
        estadoAC = true;
        digitalWrite(PINO_RELE, HIGH);
      } else if (cmd == "AC_OFF") {
        estadoAC = false;
        digitalWrite(PINO_RELE, LOW);
      } else if (cmd == "LUZ_ON") {
        estadoLuz = true;
        digitalWrite(PINO_LED, HIGH);
      } else if (cmd == "LUZ_OFF") {
        estadoLuz = false;
        digitalWrite(PINO_LED, LOW);
      }
    }
  }
}

// --- Reconexão com o Broker MQTT ---
void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    String clientId = "ESP32Client-" + String(random(0, 10000));
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado ao HiveMQ!");
      client.subscribe(topic_subscribe);
    } else {
      Serial.print("Falhou, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Configuração dos Pinos I/O
  pinMode(PINO_PORTA, INPUT_PULLUP);
  pinMode(PINO_LDR, INPUT);
  pinMode(PINO_RELE, OUTPUT);
  pinMode(PINO_LED, OUTPUT);
  
  digitalWrite(PINO_RELE, LOW);
  digitalWrite(PINO_LED, LOW);

  // Inicialização do LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Iniciando SMARTHOME");

  // Inicialização do Sensor BMP280
  if (!bmp.begin(0x76)) {
    Serial.println("Erro: BMP280 não encontrado!");
    lcd.clear();
    lcd.print("Erro: BMP280");
    while (1);
  }

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long tempoAtual = millis();

  // --- 1. LEITURA DOS SENSORES ---
  temperatura = bmp.readTemperature();
  
  int leituraLDR = analogRead(PINO_LDR);
  luminosidade = map(leituraLDR, 0, 4095, 0, 100); // Mapeia para 0-100%
  
  // Porta com PULLUP: LOW significa fechada (GND), HIGH significa aberta
  portaAberta = (digitalRead(PINO_PORTA) == HIGH); 

  // --- 2. LÓGICA DE AUTOMAÇÃO LOCAL (MODO AUTOMÁTICO) ---
  if (!modoManual) {
    
    // Gestão Térmica (Ar-Condicionado por Histerese)
    if (temperatura >= 26.0) {
      estadoAC = true;
    } else if (temperatura <= 24.0) {
      estadoAC = false;
    }

    // Trava de Segurança: Se a porta ficar aberta > 10 segundos, desliga o AC
    if (portaAberta) {
      if (!cronometroPortaAtivo) {
        tempoPortaAberta = tempoAtual;
        cronometroPortaAtivo = true;
      } else if (tempoAtual - tempoPortaAberta >= 10000) {
        estadoAC = false; // Força o desligamento por segurança energética
      }
    } else {
      cronometroPortaAtivo = false;
    }
    digitalWrite(PINO_RELE, estadoAC ? HIGH : LOW);

    // Gestão de Iluminação Inteligente com Luz de Cortesia
    if (portaAberta && luminosidade < 40) {
      estadoLuz = true;
      cortesiaAtiva = false;
    } else if (!portaAberta && estadoLuz && !cortesiaAtiva) {
      // Inicia temporizador de cortesia de 3 segundos ao fechar a porta
      tempoLuzCortesia = tempoAtual;
      cortesiaAtiva = true;
    }

    if (cortesiaAtiva && (tempoAtual - tempoLuzCortesia >= 3000)) {
      estadoLuz = false;
      cortesiaAtiva = false;
    }
    digitalWrite(PINO_LED, estadoLuz ? HIGH : LOW);
  }

  // --- 3. ATUALIZAÇÃO DO DISPLAY LCD LOCAL ---
  static unsigned long tempoUltimoLCD = 0;
  if (tempoAtual - tempoUltimoLCD >= 1000) {
    tempoUltimoLCD = tempoAtual;
    lcd.setCursor(0, 0);
    lcd.print("T: " + String(temperatura, 1) + "C  L: " + String(luminosidade) + "% ");
    lcd.setCursor(0, 1);
    lcd.print(modoManual ? "M:MANUAL " : "M:AUTO   ");
    lcd.print(portaAberta ? "P:ABER" : "P:FECH");
  }

  // --- 4. ENVIO DE TELEMETRIA (JSON VIA MQTT) A CADA 5 SEGUNDOS ---
  if (tempoAtual - tempoUltimaPublicacao >= 5000) {
    tempoUltimaPublicacao = tempoAtual;

    JsonDocument docSaida;
    docSaida["temperatura"] = temperatura;
    docSaida["luminosidade"] = luminosidade;
    docSaida["porta"] = portaAberta;
    docSaida["ac"] = estadoAC;
    docSaida["modoManual"] = modoManual;
    docSaida["luz"] = estadoLuz;

    char bufferJson[256];
    serializeJson(docSaida, bufferJson);
    
    client.publish(topic_publish, bufferJson);
    Serial.print("Telemetria publicada: ");
    Serial.println(bufferJson);
  }
}
