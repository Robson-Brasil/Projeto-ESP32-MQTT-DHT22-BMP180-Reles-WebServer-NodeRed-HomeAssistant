/**********************************************************************************
  IoT - Automação Residencial
  Autor : Robson Brasil
  Dispositivos : ESP32 WROOM32, DHT22 e Módulo Relé de 8 Canais
  Preferences--> URLs adicionais do Gerenciador de placas:
                                  http://arduino.esp8266.com/stable/package_esp8266com_index.json,https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  Download Board ESP32 (2.0.5):
  Broker MQTT
  Node-Red / Google Assistant-Nora:  https://smart-nora.eu/
  Para Instalação do Node-Red:       https://nodered.org/docs/getting-started/
  Home Assistant
  Para Instalação do Home Assistant: https://www.home-assistant.io/installation/
  Versão : 1 - Beta Tester
  Última Modificação : 04/03/2023
**********************************************************************************/

//Bibliotecas
#include "LoginsSenhas.h"
#include "TopicosMQTT.h"
#include "Bibliotecas.h"
#include "GPIOs.h"

void setup1();  // declaração da função setup1()
void loop1();   // declaração da função loop1()

int PortaBroker1 = 1883;  // Porta do Broker MQTT

float diff = 1.0;

int val;

#define ID_MQTT1 "ESP32-IoT-Broker1" /* ID MQTT (para identificação de seção)           \
                            IMPORTANTE: Este deve ser único no broker (ou seja, \
                            se um client MQTT tentar entrar com o mesmo         \
                            ID de outro já conectado ao broker, o broker        \
irá fechar a conexão de um deles).*/

#define DEBOUNCE_TIME 250

int toggleState_0 = 1;  // Definir inteiro para lembrar o estado de alternância para o relé 0
int toggleState_1 = 1;  // Definir inteiro para lembrar o estado de alternância para o relé 1
int toggleState_2 = 1;  // Definir inteiro para lembrar o estado de alternância para o relé 2
int toggleState_3 = 1;  // Definir inteiro para lembrar o estado de alternância para o relé 3
int toggleState_4 = 1;  // Definir inteiro para lembrar o estado de alternância para o relé 4
int toggleState_5 = 1;  // Definir inteiro para lembrar o estado de alternância para o relé 5
int toggleState_6 = 1;  // Definir inteiro para lembrar o estado de alternância para o relé 6
int toggleState_7 = 1;  // Definir inteiro para lembrar o estado de alternância para o relé 7
int toggleState_8 = 1;  // Definir inteiro para lembrar o estado de alternância para o relé 8
int toggleState_9 = 1;  // Definir inteiro para lembrar o estado de alternância para o relé 8 via sensor PIR
int status_todos = 0;   // Definir inteiro para lembrar o estado de alternância para todos

// DHT11 ou DHT22 para leitura dos valores  de Temperatura e Umidade
#define DHTTYPE DHT11  // DHT11 ou DHT22
DHT dht(DHTPIN, DHTTYPE);

DNSServer dns;

// IP Estático
IPAddress local_IP(192, 168, 15, 50);
IPAddress gateway(192, 168, 15, 1);
IPAddress subnet(255, 255, 255, 0);

// DNS Estático
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

// Variáveis e objetos globais
WiFiClient espClient;          // Cria o objeto espClient
PubSubClient MQTT(espClient);  // Instância o Cliente MQTT passando o objeto espClient

char str_hum_data[7];
char str_temp_data[7];
char str_tempterm_data[7];
char str_tempF_data[7];

#define MSG_BUFFER_SIZE (1000)

//Função MILLIS
unsigned long lastMsgDHT = 0;
unsigned long lastMsgMQTT = 0;
unsigned long lastMsgPIR = 0;
unsigned long delayTime = 0;
int value = 0;

// WebServer
const char* PARAM_INPUT_1 = "relay";
const char* PARAM_INPUT_2 = "state";

// Configuração da Porta Usada Pelo AsyncWebServer
AsyncWebServer server(3232);

const char login_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?relay="+element.id+"&state=0", true); }
  else { xhr.open("GET", "/update?relay="+element.id+"&state=1", true); }
  xhr.send();
}
</script>
</html>)rawliteral";

String processor(const String& var) {
  //Serial.println(var);
  if (var == "BUTTONPLACEHOLDER1") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 1</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"23\" " + outputState(23) + "><span class=\"slider\"></span></label>";
    return buttons;
  }

  if (var == "BUTTONPLACEHOLDER2") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 2</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"22\" " + outputState(22) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  if (var == "BUTTONPLACEHOLDER3") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 3</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"21\" " + outputState(21) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  if (var == "BUTTONPLACEHOLDER4") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 4</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"19\" " + outputState(19) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  if (var == "BUTTONPLACEHOLDER5") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 5</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"18\" " + outputState(18) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  if (var == "BUTTONPLACEHOLDER6") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Som Bluetooth</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"5\" " + outputState(5) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  if (var == "BUTTONPLACEHOLDER7") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 7</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"25\" " + outputState(25) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  if (var == "BUTTONPLACEHOLDER8") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Luz do Quarto</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"26\" " + outputState(26) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  return String();
}

// Configuração dos Botões Usados
String outputState(int output) {
  if (digitalRead(output)) {
    return "checked";
  } else {
    return "";
  }
}

// Prototypes
void initSerial();
void initWiFi();
void initMQTT();
void reconectWiFi();
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIEMQTT(void);
void initOutput(void);

// Implementações das funções
void setup() {
  // Inicializações:
  initOutput();
  initSerial();
  initWiFi();
  initMQTT();

  // Chama a função setup1()
  setup1();

  if (!SPIFFS.begin(true)) {
    Serial.println("Ocorreu um erro ao montar o SPIFFS");
    return;
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send(SPIFFS, "/WebServer.html", String(), false, processor);
  });

  // Serve static files
  server.serveStatic("/", SPIFFS, "/");

  // Route for dashboard CSS
  server.on("/WebServer.css", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/WebServer.css", "text/css");
  });

  server.on("/darklmode.js", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/darkmode.js", "application/javascript");
  });

  server.on("/darkmode.png", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/darkmode.png", "image/png");
  });

  server.on("/lightmode.png", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/lightmode.png", "image/png");
  });

  server.on("/logo-1.png", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/logo-1.png", "image/png");
  });

  server.on("/update1", HTTP_GET, [](AsyncWebServerRequest* request) {
    int state = LOW;
    String inputMessage;
    if (request->hasParam(PARAM_INPUT_1)) {
      state = HIGH;
      inputMessage = "Relé 1 ligado";
    } else {
      state = LOW;
      inputMessage = "Relé 1 desligado";
    }
    digitalWrite(RelayPin1, state);
    request->send(200, "text/plain", inputMessage);
  });

  server.on("/update2", HTTP_GET, [](AsyncWebServerRequest* request) {
    int state = LOW;
    String inputMessage;
    if (request->hasParam(PARAM_INPUT_2)) {
      state = HIGH;
      inputMessage = "Relé 2 ligado";
    } else {
      state = LOW;
      inputMessage = "Relé 2 desligado";
    }
    digitalWrite(RelayPin2, state);
    request->send(200, "text/plain", inputMessage);
  });

  server.on("/update3", HTTP_GET, [](AsyncWebServerRequest* request) {
    int state = LOW;
    String inputMessage;
    if (request->hasParam(PARAM_INPUT_1)) {
      state = HIGH;
      inputMessage = "Relé 3 ligado";
    } else {
      state = LOW;
      inputMessage = "Relé 3 desligado";
    }
    digitalWrite(RelayPin3, state);
    request->send(200, "text/plain", inputMessage);
  });

  server.on("/update4", HTTP_GET, [](AsyncWebServerRequest* request) {
    int state = LOW;
    String inputMessage;
    if (request->hasParam(PARAM_INPUT_2)) {
      state = HIGH;
      inputMessage = "Relé 4 ligado";
    } else {
      state = LOW;
      inputMessage = "Relé 4 desligado";
    }
    digitalWrite(RelayPin4, state);
    request->send(200, "text/plain", inputMessage);
  });

  server.on("/update5", HTTP_GET, [](AsyncWebServerRequest* request) {
    int state = LOW;
    String inputMessage;
    if (request->hasParam(PARAM_INPUT_1)) {
      state = HIGH;
      inputMessage = "Relé 5 ligado";
    } else {
      state = LOW;
      inputMessage = "Relé 5 desligado";
    }
    digitalWrite(RelayPin5, state);
    request->send(200, "text/plain", inputMessage);
  });

  server.on("/update6", HTTP_GET, [](AsyncWebServerRequest* request) {
    int state = LOW;
    String inputMessage;
    if (request->hasParam(PARAM_INPUT_2)) {
      state = HIGH;
      inputMessage = "Relé 6 ligado";
    } else {
      state = LOW;
      inputMessage = "Relé 6 desligado";
    }
    digitalWrite(RelayPin6, state);
    request->send(200, "text/plain", inputMessage);
  });

  server.on("/update7", HTTP_GET, [](AsyncWebServerRequest* request) {
    int state = LOW;
    String inputMessage;
    if (request->hasParam(PARAM_INPUT_1)) {
      state = HIGH;
      inputMessage = "Relé 7 ligado";
    } else {
      state = LOW;
      inputMessage = "Relé 7 desligado";
    }
    digitalWrite(RelayPin7, state);
    request->send(200, "text/plain", inputMessage);
  });

  server.on("/update8", HTTP_GET, [](AsyncWebServerRequest* request) {
    int state = LOW;
    String inputMessage;
    if (request->hasParam(PARAM_INPUT_2)) {
      state = HIGH;
      inputMessage = "Relé 8 ligado";
    } else {
      state = LOW;
      inputMessage = "Relé 8 desligado";
    }
    digitalWrite(RelayPin8, state);
    request->send(200, "text/plain", inputMessage);
  });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest* request) {
    String inputMessage1;
    String inputParam1;
    String inputMessage2;
    String inputParam2;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      inputParam1 = PARAM_INPUT_1;
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      inputParam2 = PARAM_INPUT_2;
      digitalWrite(inputMessage1.toInt(), !inputMessage2.toInt());
    } else {
      inputMessage1 = "No message sent";
      inputMessage2 = "No message sent";
    }
    Serial.print("GPIO: ");
    Serial.print(inputMessage1);
    Serial.print(" - Set to: ");
    Serial.println(inputMessage2);
    request->send(200, "text/plain", "OK");
  });

  // Start do Servidor
  server.begin();
}

// Função: inicializa comunicação serial com baudrate 115200 (para fins de monitorar no terminal serial
void initSerial() {
  Serial.begin(115200);
}
// Função: inicializa e conecta-se na rede WI-FI desejada
void initWiFi() {

  delay(1000);

  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(ssid);
  Serial.println("Aguarde");

  reconectWiFi();
}

// Função: inicializa parâmetros de conexão MQTT(endereço do broker, porta e seta função de callback)
void initMQTT() {

  MQTT.setServer(BrokerMQTT1, PortaBroker1);  // Informa qual broker e porta deve ser conectado
  MQTT.setCallback(mqtt_callback);            // Atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}
// Função: Função de callback, esta função é chamada toda vez que uma informação de um dos tópicos subescritos chega.
void mqtt_callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Mensagem enviada ao Broker MQTT no Tópico -> [");
  Serial.print(topic);
  Serial.print("] ");
  payload[length] = '\0';
  String data = "";

  if (strstr(topic, sub0)) {
    for (unsigned int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      data += (char)payload[i];
    }
    Serial.println();

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin1, HIGH);  // Ligua o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin2, HIGH);  // Ligua o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin3, HIGH);  // Ligua o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin4, HIGH);  // Ligua o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin5, HIGH);  // Ligua o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin6, HIGH);  // Ligua o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin7, HIGH);  // Ligua o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin8, HIGH);  // Ligua o relé. Note que HIGH é o nível de tensão.
      status_todos = 0;
      toggleState_0 = 0;
      MQTT.publish(pub0, "0", true);
    } else {
      digitalWrite(RelayPin1, LOW);  // Desligua o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin2, LOW);  // Desligua o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin3, LOW);  // Desligua o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin4, LOW);  // Desligua o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin5, LOW);  // Desligua o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin6, LOW);  // Desligua o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin7, LOW);  // Desligua o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin8, LOW);  // Desligua o Relé tornando a tensão BAIXA
      status_todos = 1;
      toggleState_0 = 1;
      MQTT.publish(pub0, "1", true);
    }
  }
  if (strstr(topic, sub1)) {
    for (unsigned int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      data += (char)payload[i];
    }
    Serial.println();

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin1, HIGH);  // Ligua o relé. Note que HIGH é o nível de tensão.
      toggleState_1 = 0;
      MQTT.publish(pub1, "0", true);
    } else {
      digitalWrite(RelayPin1, LOW);  // Desligua o Relé tornando a tensão BAIXA
      toggleState_1 = 1;
      MQTT.publish(pub1, "1", true);
    }
  }
  if (strstr(topic, sub2)) {
    for (unsigned int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      data += (char)payload[i];
    }
    Serial.println();

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin2, HIGH);  // Ligua o relé. Note que HIGH é o nível de tensão.
      toggleState_2 = 0;
      MQTT.publish(pub2, "0", true);
    } else {
      digitalWrite(RelayPin2, LOW);  // Desligua o Relé tornando a tensão BAIXA
      toggleState_2 = 1;
      MQTT.publish(pub2, "1", true);
    }
  }
  if (strstr(topic, sub3)) {
    for (unsigned int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      data += (char)payload[i];
    }
    Serial.println();

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin3, HIGH);  // Ligua o relé. Note que HIGH é o nível de tensão.
      toggleState_3 = 0;
      MQTT.publish(pub3, "0", true);
    } else {
      digitalWrite(RelayPin3, LOW);  // Desligua o Relé tornando a tensão BAIXA
      toggleState_3 = 1;
      MQTT.publish(pub3, "1", true);
    }
  }
  if (strstr(topic, sub4)) {
    for (unsigned int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      data += (char)payload[i];
    }
    Serial.println();

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin4, HIGH);  // Ligua o relé. Note que HIGH é o nível de tensão.
      toggleState_4 = 0;
      MQTT.publish(pub4, "0", true);
    } else {
      digitalWrite(RelayPin4, LOW);  // Desligua o Relé tornando a tensão BAIXA
      toggleState_4 = 1;
      MQTT.publish(pub4, "1", true);
    }
  }
  if (strstr(topic, sub5)) {
    for (unsigned int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      data += (char)payload[i];
    }
    Serial.println();

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin5, HIGH);  // Ligua o relé. Note que HIGH é o nível de tensão.
      toggleState_5 = 0;
      MQTT.publish(pub5, "0", true);
    } else {
      digitalWrite(RelayPin5, LOW);  // Desligua o Relé tornando a tensão BAIXA
      toggleState_5 = 1;
      MQTT.publish(pub5, "1", true);
    }
  }
  if (strstr(topic, sub6)) {
    for (unsigned int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      data += (char)payload[i];
    }
    Serial.println();

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin6, HIGH);  // Ligua o relé. Note que HIGH é o nível de tensão.
      toggleState_6 = 0;
      MQTT.publish(pub6, "0", true);
    } else {
      digitalWrite(RelayPin6, LOW);  // Desligua o Relé tornando a tensão BAIXA
      toggleState_6 = 1;
      MQTT.publish(pub6, "1", true);
    }
  }
  if (strstr(topic, sub7)) {
    for (unsigned int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      data += (char)payload[i];
    }
    Serial.println();

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin7, HIGH);  // Ligua o relé. Note que HIGH é o nível de tensão.
      toggleState_7 = 0;
      MQTT.publish(pub7, "0", true);
    } else {
      digitalWrite(RelayPin7, LOW);  // Desligua o Relé tornando a tensão BAIXA
      toggleState_7 = 1;
      MQTT.publish(pub7, "1", true);
    }
  }
  if (strstr(topic, sub8)) {
    for (unsigned int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      data += (char)payload[i];
    }
    Serial.println();

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin8, HIGH);  // Ligua o relé. Note que HIGH é o nível de tensão.
      toggleState_8 = 0;
      MQTT.publish(pub8, "0", true);
    } else {
      digitalWrite(RelayPin8, LOW);  // Desligua o Relé tornando a tensão BAIXA
      toggleState_8 = 1;
      MQTT.publish(pub8, "1", true);
    }
  }
  /* if (strstr(topic, motion_topic)) {
    for (unsigned int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      data += (char)payload[i];
    }
    Serial.println();

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin8, HIGH);  // Ligua o relé. Note que HIGH é o nível de tensão.
      toggleState_9 = 0;
      MQTT.publish(motion_topic, "0", true);
    } else {
      digitalWrite(RelayPin8, LOW);  // Desligua o Relé tornando a tensão BAIXA
      toggleState_9 = 1;
      MQTT.publish(motion_topic, "1", true);
    }
  }*/
}
/* Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
  em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.*/
void reconnectMQTT() {

  unsigned long currentTime = millis();
  unsigned long reconnectTime = 2000;  // Tempo para tentar reconectar (em milissegundos)

  while (!MQTT.connected()) {
    Serial.print("* Tentando se conectar ao Broker MQTT: ");
    Serial.println(BrokerMQTT1);
    if (MQTT.connect(ID_MQTT1, mqttUserName1, mqttPwd1)) {
      Serial.println("Conectado com sucesso ao broker MQTT!");
      MQTT.subscribe(sub0, 1);
      MQTT.subscribe(sub1, 1);
      MQTT.subscribe(sub2, 1);
      MQTT.subscribe(sub3, 1);
      MQTT.subscribe(sub4, 1);
      MQTT.subscribe(sub5, 1);
      MQTT.subscribe(sub6, 1);
      MQTT.subscribe(sub7, 1);
      MQTT.subscribe(sub8, 1);
      MQTT.subscribe(inTopic);
    } else {
      Serial.println("Falha ao reconectar no broker.");
      Serial.print(MQTT.state());
      if (millis() - currentTime > reconnectTime) {
        Serial.println("Haverá nova tentativa de conexão em 2s");
        currentTime = millis();
      }
    }
  }
}

// Função: reconecta-se ao WiFi
void reconectWiFi() {
  /* Se já está conectado a rede WI-FI, nada é feito.
    Caso contrário, são efetuadas tentativas de conexão*/
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(ssid, password);  // Conecta na rede WI-FI
  Serial.println("\nConectando WiFi " + String(ssid));

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Conexão Falhou");
  }

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Conectado com sucesso na rede ");
  Serial.print(ssid);
  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.print("Endereço de IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS 1: ");
  Serial.println(WiFi.dnsIP(0));
  Serial.print("DNS 2: ");
  Serial.println(WiFi.dnsIP(1));
}
/* Função: verifica o estado das conexões WiFI e ao broker MQTT.
  Em caso de desconexão (qualquer uma das duas), a conexão  é refeita.*/
void VerificaConexoesWiFIEMQTT(void) {

  if (!MQTT.connected())

    reconnectMQTT();  // se não há conexão com o Broker, a conexão é refeita

  reconectWiFi();  // se não há conexão com o WiFI, a conexão é refeita "apagar essa linha depois pra testar"
}

// Função: inicializa o output em nível lógico baixo
void initOutput(void) {

  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);
  pinMode(RelayPin5, OUTPUT);
  pinMode(RelayPin6, OUTPUT);
  pinMode(RelayPin7, OUTPUT);
  pinMode(RelayPin8, OUTPUT);

  // Durante a partida o LED WiFI, inicia desligado
  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, HIGH);

  // Durante a partida, todos os Relés iniciam desligados
  digitalWrite(RelayPin1, HIGH);
  digitalWrite(RelayPin2, HIGH);
  digitalWrite(RelayPin3, HIGH);
  digitalWrite(RelayPin4, HIGH);
  digitalWrite(RelayPin5, HIGH);
  digitalWrite(RelayPin6, HIGH);
  digitalWrite(RelayPin7, HIGH);
  digitalWrite(RelayPin8, HIGH);
}
// Programa Principal
void loop() {
  // Garante funcionamento das conexões WiFi e ao Broker MQTT
  VerificaConexoesWiFIEMQTT();
  //Keep-Alive da comunicação com Broker MQTT
  MQTT.loop();

  loop1();

  unsigned long currentTimeMQTT = millis();
  if (currentTimeMQTT - lastMsgMQTT > 500) {

    lastMsgMQTT = currentTimeMQTT;

    if (digitalRead(RelayPin1) == HIGH) {
      MQTT.publish(pub1, "0", true);
    } else {
      MQTT.publish(pub1, "1", true);
    }
    if (digitalRead(RelayPin2) == HIGH) {
      MQTT.publish(pub2, "0", true);
    } else {
      MQTT.publish(pub2, "1", true);
    }
    if (digitalRead(RelayPin3) == HIGH) {
      MQTT.publish(pub3, "0", true);
    } else {
      MQTT.publish(pub3, "1", true);
    }
    if (digitalRead(RelayPin4) == HIGH) {
      MQTT.publish(pub4, "0", true);
    } else {
      MQTT.publish(pub4, "1", true);
    }
    if (digitalRead(RelayPin5) == HIGH) {
      MQTT.publish(pub5, "0", true);
    } else {
      MQTT.publish(pub5, "1", true);
    }
    if (digitalRead(RelayPin6) == HIGH) {
      MQTT.publish(pub6, "0", true);
    } else {
      MQTT.publish(pub6, "1", true);
    }
    if (digitalRead(RelayPin7) == HIGH) {
      MQTT.publish(pub7, "0", true);
    } else {
      MQTT.publish(pub7, "1", true);
    }
    if (digitalRead(RelayPin8) == HIGH) {
      MQTT.publish(pub8, "0", true);
    } else {
      MQTT.publish(pub8, "1", true);
    }
    /*if (digitalRead(RelayPin8) == HIGH) {
      MQTT.publish(motion_topic, "0", true);
    } else {
      MQTT.publish(motion_topic, "1", true);
    }*/
    if (status_todos == 1) {
      MQTT.publish(pub0, "1", true);
    } else {
      MQTT.publish(pub0, "0", true);
    }
  }
}

//Implementação das Funções Principais do Core1 do ESP32
void setup1() {

  initMQTT();

  dht.begin();  // inicializa o sensor DHT11
}

// Implementação do Programa Principal no Core1 do ESP32
void loop1() {

  // Garante funcionamento das conexões WiFi e ao Broker MQTT
  VerificaConexoesWiFIEMQTT();
  //Keep-Alive da comunicação com Broker MQTT
  MQTT.loop();  // Verifica se há novas mensagens no Broker MQTT

  //Sensor DHT11  - Temperatua e Umidade  unsigned long currentTimeDHT = millis();
  unsigned long currentTimeDHT = millis();
  if (currentTimeDHT - lastMsgDHT > 10000) {

    lastMsgDHT = currentTimeDHT;

    float temp_data = dht.readTemperature();  // ou dht.readTemperature(true) para Fahrenheit
    dtostrf(temp_data, 6, 2, str_temp_data);
    /* 4 é largura mínima, 2 é precisão; valor flutuante é copiado para str_sensor*/
    float hum_data = dht.readHumidity();
    dtostrf(hum_data, 6, 2, str_hum_data);
    /* 4 é largura mínima, 2 é precisão; valor flutuante é copiado para str_sensor*/
    float tempF_data = dht.readTemperature(true);
    dtostrf(tempF_data, 6, 2, str_tempF_data);
    /* 4 é largura mínima, 2 é precisão; valor flutuante é copiado para str_sensor*/
    float tempterm_data = 0;
    tempterm_data = dht.computeHeatIndex(tempF_data, hum_data);
    tempterm_data = dht.convertFtoC(tempterm_data);
    dtostrf(tempterm_data, 6, 2, str_tempterm_data);
    /* 4 é largura mínima, 2 é precisão; valor flutuante é copiado para str_sensor*/

    MQTT.publish(pub9, str_temp_data);
    MQTT.publish(pub10, str_hum_data);
    MQTT.publish(pub11, str_tempterm_data);
  }
  //Sensor PIR - Detector de Presença
  unsigned long currentTimePIR = millis();
  unsigned long motionDetectedTime = 0;
  unsigned long relayDuration = 15000;  // Tempo de duração do relé em milissegundos (5 segundos)

  if (currentTimePIR - lastMsgPIR > 500) {
    lastMsgPIR = currentTimePIR;

    val = digitalRead(pirPin);
    if (val == LOW) {
      // Serial.println("Sem Movimento");
      MQTT.publish(motion_topic, "Sem Movimento");
      digitalWrite(RelayPin8, HIGH);  // Desliga o relé
    } else {
      MQTT.publish(motion_topic, "Movimento Detectado");      
      Serial.println("Movimento Detectado");
      MQTT.publish(pub8, "0", true);        // Publica mensagem MQTT indicando que o relé foi ligado
      motionDetectedTime = currentTimePIR;  // Armazena o momento em que o movimento foi detectado
      digitalWrite(RelayPin8, LOW);         // Liga o relé
    }
  }
  if (motionDetectedTime > 0 && millis() - motionDetectedTime > relayDuration) {
    motionDetectedTime = 0;
    digitalWrite(RelayPin8, HIGH);  // Desliga o relé após o tempo de duração definido
  }
}