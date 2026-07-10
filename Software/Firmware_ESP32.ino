#include <WiFi.h>
#define MQTT_MAX_PACKET_SIZE 512   // Deve vir antes do PubSubClient
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_PCF8574.h>

// =====================================================
// WIFI
// =====================================================
const char* ssid = "Mamadjam";
const char* password = "12345678";

// =====================================================
// MQTT
// =====================================================
const char* mqtt_server = "broker.hivemq.com";
const char* topic_pub = "mamadjam123/iot/sensores";
const char* topic_sub = "mamadjam123/iot/comandos";

WiFiClient espClient;
PubSubClient client(espClient);

// =====================================================
// HARDWARE (PINOS)
// =====================================================
#define RELAY_AC_PIN 25
#define LED_LAMP_PIN 2
#define SWITCH_PIN   14
#define LDR_PIN      34

// =====================================================
// CONFIGURAÇÕES DA AUTOMAÇÃO
// =====================================================
const float TEMP_LIGA_AC = 26.0;
const float TEMP_DESLIGA_AC = 24.0;
const int LIMIAR_ESCURO = 40;           // Agora em percentagem (0 a 100%)

const long TEMPO_ABERTA_ALERTA = 10000; // 10 segundos
const long TEMPO_CORTESIA_LUZ = 3000;   // 3 segundos

// =====================================================
// OBJETOS E VARIÁVEIS DE CONTROLE
// =====================================================
Adafruit_BMP280 bmp;
LiquidCrystal_PCF8574 lcd(0x27);

unsigned long timerPorta = 0;
unsigned long timerLuz = 0;
unsigned long lastUpdate = 0;
unsigned long ultimoEnvio = 0;

bool acBloqueado = false;
bool luzAtiva = false;

// Controle de Modos
bool modoManual = false;
bool estadoManualAC = false;
bool estadoManualLuz = false;

// =====================================================
// CONEXÃO WIFI
// =====================================================
void setup_wifi() {
  delay(10);
  Serial.println("\n[WIFI] Inicializando conexao...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n[WIFI] Conectado!");
  Serial.print("[WIFI] IP: ");
  Serial.println(WiFi.localIP());
}

// =====================================================
// CALLBACK MQTT (RECEBIMENTO DE COMANDOS)
// =====================================================
void callback(char* topic, byte* payload, unsigned int length) {
  String mensagem;
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }

  Serial.println("\n==============================");
  Serial.print("[MQTT] Mensagem Recebida: ");
  Serial.println(mensagem);
  Serial.println("==============================");

  if (mensagem == "AUTO_ON") {
    modoManual = false;
    Serial.println("[MODO] AUTOMATICO ATIVADO");
  }
  else if (mensagem == "MANUAL_ON") {
    modoManual = true;
    Serial.println("[MODO] MANUAL ATIVADO");
  }
  else if (mensagem == "AC_ON") {
    modoManual = true;
    estadoManualAC = true;
    Serial.println("[AC] LIGADO MANUALMENTE");
  }
  else if (mensagem == "AC_OFF") {
    modoManual = true;
    estadoManualAC = false;
    Serial.println("[AC] DESLIGADO MANUALMENTE");
  }
  else if (mensagem == "LUZ_ON") {
    modoManual = true;
    estadoManualLuz = true;
    Serial.println("[LUZ] LIGADA MANUALMENTE");
  }
  else if (mensagem == "LUZ_OFF") {
    modoManual = true;
    estadoManualLuz = false;
    Serial.println("[LUZ] DESLIGADA MANUALMENTE");
  }
}

// =====================================================
// RECONEXÃO MQTT
// =====================================================
void reconnect() {
  while (!client.connected()) {
    Serial.println("[MQTT] Conectando ao broker...");
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("[MQTT] Conectado com sucesso!");
      client.subscribe(topic_sub);
      Serial.print("[MQTT] Inscrito no topico: ");
      Serial.println(topic_sub);
    } else {
      Serial.print("[MQTT] Erro de conexao: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

// =====================================================
// CONFIGURAÇÃO INICIAL (SETUP)
// =====================================================
void setup() {
  Serial.begin(115200);
  Serial.println("\n====================================");
  Serial.println("    SISTEMA DE AUTOMACAO INICIADO   ");
  Serial.println("====================================");

  // Configuração dos Pinos
  pinMode(RELAY_AC_PIN, OUTPUT);
  pinMode(LED_LAMP_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  // Estados Iniciais (Tudo desligado)
  digitalWrite(RELAY_AC_PIN, HIGH);
  digitalWrite(LED_LAMP_PIN, LOW);

  // Inicialização I2C e Periféricos
  Wire.begin(21, 22);
  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.print("INICIANDO...");

  // Inicialização BMP280
  if (!bmp.begin(0x76)) {
    Serial.println("[ERRO CRITICO] Sensor BMP280 nao detectado!");
    lcd.setCursor(0,1);
    lcd.print("ERRO BMP280");
    while (1); // Para o sistema aqui se o sensor falhar
  }
  Serial.println("[BMP280] Sensor pronto.");

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  lcd.clear();
  Serial.println("[SETUP] Concluido com sucesso!\n");
}

// =====================================================
// LOOP PRINCIPAL
// =====================================================
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Leituras dos sensores
  float temperatura = bmp.readTemperature();
  // Transforma o valor analógico em 0 a 100%
  int luminosidade = map(analogRead(LDR_PIN), 0, 4095, 0, 100); 
  
  // INVERSÃO DA LÓGICA DA PORTA: 
  // Solto (HIGH) = FECHADA. Apertado (LOW) = ABERTA.
  bool portaAberta = (digitalRead(SWITCH_PIN) == LOW);

  // =====================================================
  // SEGURANÇA DA PORTA (TEMPORIZADOR DO ALERTA)
  // =====================================================
  if (portaAberta) {
    if (timerPorta == 0) {
      timerPorta = millis();
    }
    if (millis() - timerPorta > TEMPO_ABERTA_ALERTA) {
      if (!acBloqueado) {
        Serial.println("[ALERTA] Porta aberta detectada por muito tempo!");
      }
      acBloqueado = true;
    }
  } else {
    timerPorta = 0;
    acBloqueado = false;
  }

  // =====================================================
  // GERENCIAMENTO DO AR-CONDICIONADO
  // =====================================================
  if (modoManual) {
    digitalWrite(RELAY_AC_PIN, estadoManualAC ? LOW : HIGH);
  } 
  else {
    if (acBloqueado) {
      digitalWrite(RELAY_AC_PIN, HIGH); // Desliga preventivamente por economia
    } else {
      if (temperatura >= TEMP_LIGA_AC)    digitalWrite(RELAY_AC_PIN, LOW);
      if (temperatura <= TEMP_DESLIGA_AC) digitalWrite(RELAY_AC_PIN, HIGH);
    }
  }

  // =====================================================
  // GERENCIAMENTO DA LÂMPADA (LED)
  // =====================================================
  if (modoManual) {
    digitalWrite(LED_LAMP_PIN, estadoManualLuz ? HIGH : LOW);
  } 
  else {
    if (portaAberta) {
      // Se a leitura for menor que o limiar, significa que está escuro
      if (luminosidade < LIMIAR_ESCURO) {
        digitalWrite(LED_LAMP_PIN, HIGH);
        luzAtiva = true;
        timerLuz = millis(); // Mantém o tempo renovado enquanto a porta continuar aberta
      } else {
        digitalWrite(LED_LAMP_PIN, LOW);
        luzAtiva = false;
      }
    } 
    else {
      // Porta Fechada
      if (luzAtiva) {
        if (millis() - timerLuz > TEMPO_CORTESIA_LUZ) {
          digitalWrite(LED_LAMP_PIN, LOW);
          luzAtiva = false;
        }
      } else {
        digitalWrite(LED_LAMP_PIN, LOW);
      }
    }
  }

  // =====================================================
  // TRANSMISSÃO DE TELEMETRIA MQTT (A CADA 5 SEGUNDOS)
  // =====================================================
  if (millis() - ultimoEnvio > 5000) {
    ultimoEnvio = millis();

    StaticJsonDocument<256> doc;
    doc["temperatura"] = temperatura;
    doc["luminosidade"] = luminosidade;
    doc["porta"] = portaAberta;
    doc["ac"] = (digitalRead(RELAY_AC_PIN) == LOW);
    doc["modoManual"] = modoManual;
    doc["luz"] = (digitalRead(LED_LAMP_PIN) == HIGH);

    char buffer[256];
    size_t tamanho = serializeJson(doc, buffer, sizeof(buffer)); 

    bool enviado = client.publish(topic_pub, (uint8_t*)buffer, tamanho, false);

    if (enviado) {
      Serial.println("[MQTT] Dados publicados com sucesso!");
    } else {
      Serial.println("[MQTT ERROR] Falha no envio — verifique MQTT_MAX_PACKET_SIZE");
    }
  }

  // =====================================================
  // MONITORAMENTO LOCAL (LCD + SERIAL A CADA 1 SEGUNDO)
  // =====================================================
  if (millis() - lastUpdate > 1000) {
    lastUpdate = millis();

    Serial.println("\n========== TELEMETRIA TEMPO REAL ==========");
    Serial.printf("Temperatura : %.2f C\n", temperatura);
    Serial.printf("Luminosidade: %d %%\n", luminosidade);
    Serial.printf("Status Porta: %s\n", portaAberta ? "ABERTA (Apertado)" : "FECHADA (Solto)");
    Serial.printf("Status AC   : %s\n", digitalRead(RELAY_AC_PIN) == LOW ? "LIGADO" : "DESLIGADO");
    Serial.printf("Status LUZ  : %s\n", digitalRead(LED_LAMP_PIN) == HIGH ? "LIGADA" : "DESLIGADA");
    Serial.printf("Modo Sistema: %s\n", modoManual ? "MANUAL" : "AUTOMATICO");
    if (acBloqueado) Serial.println("[!] ALERTA ENERGÉTICO: AC travado em OFF devido a porta aberta.");
    Serial.println("===========================================");

    // Renderização LCD
    lcd.setCursor(0, 0);
    lcd.print("T:"); lcd.print(temperatura, 1);
    lcd.print(" "); lcd.print(modoManual ? "MAN" : "AUT");
    lcd.print("   ");

    lcd.setCursor(0, 1);
    if (acBloqueado && !modoManual) {
      lcd.print("FECHE A PORTA!  ");
    } else {
      lcd.print("AC:"); lcd.print(digitalRead(RELAY_AC_PIN) == LOW ? "ON " : "OFF");
      lcd.print(" L:");  lcd.print(digitalRead(LED_LAMP_PIN) == HIGH ? "ON " : "OFF");
      lcd.print(" ");
    }
  }
}
