/**********************************************************************************
IoT - Automação Residencial
Autor : Robson Brasil
Dispositivo : ESP32 WROOM32
Preferences--> Aditional boards Manager URLs: 
                                    http://arduino.esp8266.com/stable/package_esp8266com_index.json,https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
Download Board ESP32 (2.0.5):
WiFi Manager
Broker MQTT
Node-Red / Google Assistant-Nora:  https://smart-nora.eu/  
Alexa-SirincPro:                   https://portal.sinric.pro/
Para Instalação do Node-Red:       https://nodered.org/docs/getting-started/
Home Assistant
Para Instalação do Home Assistant: https://www.home-assistant.io/installation/
Versão : 8 - Alfa
Última Modificação : 25/10/2022
**********************************************************************************/

// Bibliotecas
#include <WiFi.h>               // Importa a Biblioteca WiFi
#include <PubSubClient.h>       // Importa a Biblioteca PubSubClient
#include <DHT.h>                // Importa a Biblioteca DHT
#include <WiFiUdp.h>            // Importa a Biblioteca WiFiUdp
#include <Arduino.h>            // ArduinoJson Library:        https://github.com/bblanchon/ArduinoJson
#include "SinricPro.h"          // SinricPro Library:          https://github.com/sinricpro/esp8266-esp32-sdk
#include "SinricProSwitch.h"    // SinricPro Library:          https://github.com/sinricpro/esp8266-esp32-sdk
#include <map>
#include <DNSServer.h>          // DNSServer Library:          https://github.com/zhouhan0126/DNSServer---esp32
#include <ESPAsyncWebServer.h>  //ESPAsyncWebServer Library:   https://github.com/me-no-dev/ESPAsyncWebServer
#include <ESPAsyncWiFiManager.h>//ESPAsyncWiFiManager Library: https://github.com/alanswx/ESPAsyncWiFiManager
#include <FS.h>
#include <Preferences.h>
#include <Esp32WifiManager.h>

//Wi-Fi Manager
AsyncWebServer webServer(80);      //Cria os objetos dos servidores
DNSServer dns;
WifiManager manager;

// Tópicos do Subscribe
const char* sub0 = "ESP32/MinhaCasa/QuartoRobson/Ligar-DesligarTudo/Comando";  // Somente por MQTT
const char* sub1 = "ESP32/MinhaCasa/QuartoRobson/Interruptor1/Comando";        // Ligados ao Nora/MQTT
const char* sub2 = "ESP32/MinhaCasa/QuartoRobson/Interruptor2/Comando";        // Ligados ao Nora/MQTT
const char* sub3 = "ESP32/MinhaCasa/QuartoRobson/Interruptor3/Comando";        // Ligados ao Nora/MQTT
const char* sub4 = "ESP32/MinhaCasa/QuartoRobson/Interruptor4/Comando";        // Ligados ao Nora/MQTT
const char* sub5 = "ESP32/MinhaCasa/QuartoRobson/Interruptor5/Comando";        // Ligados ao Nora/MQTT
const char* sub6 = "ESP32/MinhaCasa/QuartoRobson/Interruptor6/Comando";        // Ligados ao MQTT/Alexa
const char* sub7 = "ESP32/MinhaCasa/QuartoRobson/Interruptor7/Comando";        // Ligados ao MQTT/Alexa
const char* sub8 = "ESP32/MinhaCasa/QuartoRobson/Interruptor8/Comando";        // Ligados ao MQTT/Alexa

// Tópicos do Publish
const char*  pub0 = "ESP32/MinhaCasa/QuartoRobson/Ligar-DesligarTudo/Estado";  // Somente por MQTT
const char*  pub1 = "ESP32/MinhaCasa/QuartoRobson/Interruptor1/Estado";        // Ligados ao Nora/MQTT
const char*  pub2 = "ESP32/MinhaCasa/QuartoRobson/Interruptor2/Estado";        // Ligados ao Nora/MQTT
const char*  pub3 = "ESP32/MinhaCasa/QuartoRobson/Interruptor3/Estado";        // Ligados ao Nora/MQTT
const char*  pub4 = "ESP32/MinhaCasa/QuartoRobson/Interruptor4/Estado";        // Ligados ao Nora/MQTT
const char*  pub5 = "ESP32/MinhaCasa/QuartoRobson/Interruptor5/Estado";        // Ligados ao Nora/MQTT
const char*  pub6 = "ESP32/MinhaCasa/QuartoRobson/Interruptor6/Estado";        // Ligados ao MQTT/Alexa
const char*  pub7 = "ESP32/MinhaCasa/QuartoRobson/Interruptor7/Estado";        // Ligados ao MQTT/Alexa
const char*  pub8 = "ESP32/MinhaCasa/QuartoRobson/Interruptor8/Estado";        // Ligados ao MQTT/Alexa
const char*  pub9 = "ESP32/MinhaCasa/QuartoRobson/Temperatura";                // Somente por MQTT
const char* pub10 = "ESP32/MinhaCasa/QuartoRobson/Umidade";                    // Somente por MQTT
const char* pub11 = "ESP32/MinhaCasa/QuartoRobson/SensacaoTermica";            // Somente por MQTT

//Tópicos do Sensor de Movimento
const char* motion_topic = "ESP32/MinhaCasa/QuartoRobson/Motion";              // Somente por MQTT
const char*      inTopic = "ESP32/MinhaCasa/QuartoRobson/inTopic";             // Somente por MQTT
const char*     outTopic = "ESP32/MinhaCasa/QuartoRobson/outTopic";            // Somente por MQTT

float diff               = 1.0;

unsigned long delayTime;
int pirPin = 17;
int val;

#define ID_MQTT "ESP32-IoT" /* ID MQTT (para identificação de seção)           \
                            IMPORTANTE: Este deve ser único no broker (ou seja, \
                            se um client MQTT tentar entrar com o mesmo         \
                            ID de outro já conectado ao broker, o broker        \
                            irá fechar a conexão de um deles).*/

#define    APP_KEY "4914a0d0-327e-4815-8128-822b7a80713d"                                       //Site https://portal.sinric.pro/ para conseguir a APP-KEY
#define APP_SECRET "b73f409a-fcc5-4c60-8cf4-d86d9b4e2bae-e7fc2fe6-1b4a-4b71-aa38-de61ad019107"  //Site https://portal.sinric.pro/ para conseguir a APP-SECRET

//Enter the device IDs here
#define device_ID_1 "62da1ebf0aec232058001ec7"  //Site https://portal.sinric.pro/ para conseguir a ID Device
#define device_ID_2 "62da1f140aec232058001f15"  //Site https://portal.sinric.pro/ para conseguir a ID Device
#define device_ID_3 "62da1dea0aec232058001e4f"  //Site https://portal.sinric.pro/ para conseguir a ID Device

// Defines - Mapeamento de pinos do NodeMCU Relays
#define RelayPin1 23  // D23 Ligados ao Nora/MQTT
#define RelayPin2 22  // D22 Ligados ao Nora/MQTT
#define RelayPin3 21  // D21 Ligados ao Nora/MQTT
#define RelayPin4 19  // D19 Ligados ao Nora/MQTT
#define RelayPin5 18  // D18 Ligados ao Nora/MQTT
#define RelayPin6  5  // D5  Ligados ao MQTT/Alexa
#define RelayPin7 25  // D25 Ligados ao MQTT/Alexa
#define RelayPin8 26  // D26 Ligados ao MQTT/Alexa

// WiFi Status Relé
#define wifiLed    0  // D0
#define DEBOUNCE_TIME 250

int toggleState_0 = 1;  // Define integer to remember the toggle state for relay 0
int toggleState_1 = 1;  // Define integer to remember the toggle state for relay 1
int toggleState_2 = 1;  // Define integer to remember the toggle state for relay 2
int toggleState_3 = 1;  // Define integer to remember the toggle state for relay 3
int toggleState_4 = 1;  // Define integer to remember the toggle state for relay 4
int toggleState_5 = 1;  // Define integer to remember the toggle state for relay 5
int toggleState_6 = 1;  // Define integer to remember the toggle state for relay 6
int toggleState_7 = 1;  // Define integer to remember the toggle state for relay 7
int toggleState_8 = 1;  // Define integer to remember the toggle state for relay 8
int status_todos  = 0;  // Define integer to remember the toggle state for todos

// DHT22 para leitura dos valores  de Temperatura e Umidade
#define DHTPIN 16
#define DHTTYPE DHT22   // DHT 22
DHT dht(DHTPIN, DHTTYPE);

// Configurações do WIFI
const char*     SSID      =           "RVR 2,4GHz";  // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD      = "RodrigoValRobson2022";  // Senha da rede WI-FI que deseja se conectar

// Configurações do Broker MQTT
const char* BROKER_MQTT   = "192.168.15.10";         // URL do broker MQTT que se deseja utilizar
const char* mqttUserName  =  "RobsonBrasil";         // MQTT UserName
const char* mqttPwd       =      "LoboAlfa";         // MQTT Password
int BROKER_PORT           =            1883;         // Porta do Broker MQTT

// IP Estático
IPAddress local_IP  (192, 168, 15, 50);
IPAddress gateway    (192, 168, 15, 1);
IPAddress subnet    (255, 255, 255, 0);

IPAddress primaryDNS      (8, 8, 8, 8);
IPAddress secondaryDNS    (8, 8, 4, 4);

typedef struct {  // struct for the std::map below
  int relayPIN;
  int flipSwitchPIN;
} deviceConfig_t;


std::map<String, deviceConfig_t> devices = {
  //{deviceId, {relayPIN,  flipSwitchPIN}}
  { device_ID_1, { RelayPin6 } },
  { device_ID_2, { RelayPin7 } },
  { device_ID_3, { RelayPin8 } }
};

typedef struct {  // struct for the std::map below
  String deviceId;
  bool lastFlipSwitchState;
  unsigned long lastFlipSwitchChange;
} flipSwitchConfig_t;

std::map<int, flipSwitchConfig_t> flipSwitches;  // this map is used to map flipSwitch PINs to deviceId and handling debounce and last flipSwitch state checks
                                                 // it will be setup in "setupFlipSwitches" function, using informations from devices map

void setupRelays() {
  for (auto &device : devices)                   // for each device (relay, flipSwitch combination)
  {
    int relayPIN = device.second.relayPIN;       // get the relay pin
  pinMode(relayPIN, OUTPUT);                   // set relay pin to OUTPUT
  digitalWrite(relayPIN, HIGH);
  }
}

void setupFlipSwitches() {
  for (auto &device : devices) {                   // for each device (relay / flipSwitch combination)
    flipSwitchConfig_t flipSwitchConfig;           // create a new flipSwitch configuration

  flipSwitchConfig.deviceId = device.first;      // set the deviceId
  flipSwitchConfig.lastFlipSwitchChange = 0;     // set debounce time
  flipSwitchConfig.lastFlipSwitchState = false;  // set lastFlipSwitchState to false (LOW)--

    int flipSwitchPIN = device.second.flipSwitchPIN;  // get the flipSwitchPIN

  flipSwitches[flipSwitchPIN] = flipSwitchConfig;   // save the flipSwitch config to flipSwitches map
  pinMode(flipSwitchPIN, INPUT_PULLUP);             // set the flipSwitch pin to INPUT
  }
}

bool onPowerState(String deviceId, bool &state) {
  Serial.printf("%s: %s\r\n", deviceId.c_str(), state ? "Ligado" : "Desligado");
    int relayPIN = devices[deviceId].relayPIN;  // get the relay pin for corresponding device
  digitalWrite(relayPIN, !state);             // set the new relay state
  return true;
}

void handleFlipSwitches() {
  unsigned long actualMillis = millis();  // get actual millis

  for (auto &flipSwitch : flipSwitches) {                                         // for each flipSwitch in flipSwitches map
    unsigned long lastFlipSwitchChange = flipSwitch.second.lastFlipSwitchChange;  // get the timestamp when flipSwitch was pressed last time (used to debounce / limit events)

  if (actualMillis - lastFlipSwitchChange > DEBOUNCE_TIME) {           // if time is > debounce time...

    int flipSwitchPIN = flipSwitch.first;                              // get the flipSwitch pin from configuration
    bool lastFlipSwitchState = flipSwitch.second.lastFlipSwitchState;  // get the lastFlipSwitchState
    bool flipSwitchState = digitalRead(flipSwitchPIN);                 // read the current flipSwitch state

  if (flipSwitchState != lastFlipSwitchState) {  // if the flipSwitchState has changed...

  if (flipSwitchState) {  // if the tactile button is pressed
//#endif
  flipSwitch.second.lastFlipSwitchChange = actualMillis;  // update lastFlipSwitchChange time
  String deviceId = flipSwitch.second.deviceId;           // get the deviceId from config
    int relayPIN = devices[deviceId].relayPIN;              // get the relayPIN from config
    bool newRelayState = !digitalRead(relayPIN);            // set the new relay State
  digitalWrite(relayPIN, newRelayState);                  // set the relay to the new state

  SinricProSwitch &mySwitch = SinricPro[deviceId];        // get Switch device from SinricPro
  mySwitch.sendPowerStateEvent(!newRelayState);           // send the event

  }
  flipSwitch.second.lastFlipSwitchState = flipSwitchState;  // update lastFlipSwitchState
      }
    }
  }
}

void setupSinricPro() {
  for (auto &device : devices) {
    const char *deviceId = device.first.c_str();
  SinricProSwitch &mySwitch = SinricPro[deviceId];
  mySwitch.onPowerState(onPowerState);
  }

  SinricPro.begin(APP_KEY, APP_SECRET);
  SinricPro.restoreDeviceStates(true);
}

// Variáveis e objetos globais
WiFiClient espClient;          // Cria o objeto espClient
PubSubClient MQTT(espClient);  // Instância o Cliente MQTT passando o objeto espClient

char str_hum_data[10];
char str_temp_data[10];
char str_tempterm_data[10];
char str_tempF_data[10];

#define MSG_BUFFER_SIZE (1000)
    unsigned long lastMsg = 0;
    int value = 0;

// Prototypes
void initSerial();
void initWiFi();
void initMQTT();
void reconectWiFi();
void mqtt_callback(char *topic, byte *payload, unsigned int length);
void VerificaConexoesWiFIEMQTT(void);
void initOutput(void);
void setupRelays();
void setupFlipSwitches();
void setupSinricPro();
void WiFiManager();

/*
Implementações das funções
 */
void setup() {
  // Inicializações:
  initOutput();
  initSerial();
  initWiFi();
  initMQTT();
  dht.begin();
  setupRelays();
  setupFlipSwitches();
  setupSinricPro();
  WiFiManager();
}

void WiFiManager(){
  //Configuração do Wi-Fi Manager
  AsyncWiFiManager wifiManager(&webServer, &dns);     //Cria os objetos dos servidores
  wifiManager.resetSettings();                     //Reseta as configurações do gerenciador
//  wifiManager.setSTAStaticIPConfig(local_IP, gateway, subnet);  
  wifiManager.autoConnect("ESP32 - Access Point"); //Cria o ponto de acesso
  //wifiManager.setBreakAfterConfig(true);
//  manager.setupAP();
//  manager.setupScan();
}

// Função: inicializa comunicação serial com baudrate 115200 (para fins de monitorar no terminal serial
void initSerial() {
  Serial.begin(115200);
}

// Função: inicializa e conecta-se na rede WI-FI desejada
void initWiFi() {
  delay(10);
  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(SSID);
  Serial.println("Aguarde");

  reconectWiFi();
}

// Função: inicializa parâmetros de conexão MQTT(endereço do broker, porta e seta função de callback)
void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);  // Informa qual broker e porta deve ser conectado
  MQTT.setCallback(mqtt_callback);           // Atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}

// Função: Função de callback, esta função é chamada toda vez que uma informação de um dos tópicos subescritos chega.
void mqtt_callback(char *topic, byte *payload, unsigned int length) {

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
    digitalWrite(RelayPin1, HIGH);  // Turn the Relé on Note that HIGH is the voltage level
    digitalWrite(RelayPin2, HIGH);  // Turn the Relé on Note that HIGH is the voltage level
    digitalWrite(RelayPin3, HIGH);  // Turn the Relé on Note that HIGH is the voltage level
    digitalWrite(RelayPin4, HIGH);  // Turn the Relé on Note that HIGH is the voltage level
    digitalWrite(RelayPin5, HIGH);  // Turn the Relé on Note that HIGH is the voltage level
    digitalWrite(RelayPin6, HIGH);  // Turn the Relé on Note that HIGH is the voltage level
    digitalWrite(RelayPin7, HIGH);  // Turn the Relé on Note that HIGH is the voltage level
    digitalWrite(RelayPin8, HIGH);  // Turn the Relé on Note that HIGH is the voltage level
    status_todos = 0;
    toggleState_0 = 0;
    MQTT.publish(pub0, "0");
  } else {
    digitalWrite(RelayPin1, LOW);   // Turn the Relé off by making the voltage LOW
    digitalWrite(RelayPin2, LOW);   // Turn the Relé off by making the voltage LOW
    digitalWrite(RelayPin3, LOW);   // Turn the Relé off by making the voltage LOW
    digitalWrite(RelayPin4, LOW);   // Turn the Relé off by making the voltage LOW
    digitalWrite(RelayPin5, LOW);   // Turn the Relé off by making the voltage LOW
    digitalWrite(RelayPin6, LOW);   // Turn the Relé off by making the voltage LOW
    digitalWrite(RelayPin7, LOW);   // Turn the Relé off by making the voltage LOW
    digitalWrite(RelayPin8, LOW);   // Turn the Relé off by making the voltage LOW
    status_todos = 1;
    toggleState_0 = 1;
    MQTT.publish(pub0, "1");
    }
  }
  if (strstr(topic, sub1)) {
    for (unsigned int i = 0; i < length; i++) {
  Serial.print((char)payload[i]);
  data += (char)payload[i];
    }
  Serial.println();
  if ((char)payload[0] == '0') {
    digitalWrite(RelayPin1, HIGH);  // Turn the Relé on (Note that LOW is the voltage level
    toggleState_1 = 0;
    MQTT.publish(pub1, "0");
  } else {
    digitalWrite(RelayPin1, LOW);   // Turn the Relé off by making the voltage HIGH
    toggleState_1 = 1;
    MQTT.publish(pub1, "1");
    }
  }
  if (strstr(topic, sub2)) {
    for (unsigned int i = 0; i < length; i++) {
  Serial.print((char)payload[i]);
    data += (char)payload[i];
    }
  Serial.println();

  if ((char)payload[0] == '0') {
    digitalWrite(RelayPin2, HIGH);  // Turn the Relé on (Note that LOW is the voltage level
    toggleState_2 = 0;
    MQTT.publish(pub2, "0");
  } else {
    digitalWrite(RelayPin2, LOW);  // Turn the Relé off by making the voltage HIGH
    toggleState_2 = 1;
    MQTT.publish(pub2, "1");
    }
  }
  if (strstr(topic, sub3)) {
    for (unsigned int i = 0; i < length; i++) {
  Serial.print((char)payload[i]);
    data += (char)payload[i];
    }
  Serial.println();

  if ((char)payload[0] == '0') {
    digitalWrite(RelayPin3, HIGH);  // Turn the Relé on (Note that LOW is the voltage level
    toggleState_3 = 0;
    MQTT.publish(pub3, "0");
  } else {
    digitalWrite(RelayPin3, LOW);  // Turn the Relé off by making the voltage HIGH
    toggleState_3 = 1;
    MQTT.publish(pub3, "1");
    }
  }
  if (strstr(topic, sub4)) {
    for (unsigned int i = 0; i < length; i++) {
  Serial.print((char)payload[i]);
    data += (char)payload[i];
    }
  Serial.println();

  if ((char)payload[0] == '0') {
    digitalWrite(RelayPin4, HIGH);  // Turn the Relé on (Note that LOW is the voltage level
    toggleState_4 = 0;
    MQTT.publish(pub4, "0");
  } else {
    digitalWrite(RelayPin4, LOW);  // Turn the Relé off by making the voltage HIGH
    toggleState_4 = 1;
    MQTT.publish(pub4, "1");
    }
  }
  if (strstr(topic, sub5)) {
    for (unsigned int i = 0; i < length; i++) {
  Serial.print((char)payload[i]);
    data += (char)payload[i];
  }
  Serial.println();

  if ((char)payload[0] == '0') {
    digitalWrite(RelayPin5, HIGH);  // Turn the Relé on (Note that LOW is the voltage level
    toggleState_5 = 0;
    MQTT.publish(pub5, "0");
  } else {
    digitalWrite(RelayPin5, LOW);  // Turn the Relé off by making the voltage HIGH
    toggleState_5 = 1;
    MQTT.publish(pub5, "1");
    }
  }
  if (strstr(topic, sub6)) {
    for (unsigned int i = 0; i < length; i++) {
  Serial.print((char)payload[i]);
    data += (char)payload[i];
    }
  Serial.println();

  if ((char)payload[0] == '0') {
    digitalWrite(RelayPin6, HIGH);  // Turn the Relé on (Note that LOW is the voltage level
    toggleState_6 = 0;
    MQTT.publish(pub6, "0");
  } else {
    digitalWrite(RelayPin6, LOW);  // Turn the Relé off by making the voltage HIGH
    toggleState_6 = 1;
    MQTT.publish(pub6, "1");
    }
  }
  if (strstr(topic, sub7)) {
    for (unsigned int i = 0; i < length; i++) {
  Serial.print((char)payload[i]);
    data += (char)payload[i];
    }
  Serial.println();

  if ((char)payload[0] == '0') {
    digitalWrite(RelayPin7, HIGH);  // Turn the Relé on (Note that LOW is the voltage level
    toggleState_7 = 0;
    MQTT.publish(pub7, "0");
  } else {
    digitalWrite(RelayPin7, LOW);  // Turn the Relé off by making the voltage HIGH
    toggleState_7 = 1;
    MQTT.publish(pub7, "1");
    }
  }
  if (strstr(topic, sub8)) {
    for (unsigned int i = 0; i < length; i++) {
  Serial.print((char)payload[i]);
    data += (char)payload[i];
    }
  Serial.println();

  if ((char)payload[0] == '0') {
    digitalWrite(RelayPin8, HIGH);  // Turn the Relé on (Note that LOW is the voltage level
    toggleState_8 = 0;
    MQTT.publish(pub8, "0");
  } else {
    digitalWrite(RelayPin8, LOW);  // Turn the Relé off by making the voltage HIGH
    toggleState_8 = 1;
    MQTT.publish(pub8, "1");
    }
  }
}

/* Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.*/
void reconnectMQTT() {
    while (!MQTT.connected()) {
  Serial.print("* Tentando se conectar ao Broker MQTT: ");
  Serial.println(BROKER_MQTT);
  if (MQTT.connect(ID_MQTT, mqttUserName, mqttPwd)) {
    Serial.println("Conectado com sucesso ao broker MQTT!");
    MQTT.subscribe(sub0);
    MQTT.subscribe(sub1);
    MQTT.subscribe(sub2);
    MQTT.subscribe(sub3);
    MQTT.subscribe(sub4);
    MQTT.subscribe(sub5);
    MQTT.subscribe(sub6);
    MQTT.subscribe(sub7);
    MQTT.subscribe(sub8);
    MQTT.subscribe(inTopic);
  } else {
  Serial.println("Falha ao reconectar no broker.");
  Serial.print(MQTT.state());
  Serial.println("Haverá nova tentativa de conexão em 2s");
  delay(2000);
    }
  }
}

// Função: reconecta-se ao WiFi
void reconectWiFi() {

  /* Se já está conectado a rede WI-FI, nada é feito.
Caso contrário, são efetuadas tentativas de conexão*/
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, PASSWORD);  // Conecta na rede WI-FI
  Serial.println("\nConectando WiFi " + String(SSID));
  
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
  Serial.println("Configuração Falhou");
  }
  
    while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado com sucesso na rede ");
  Serial.print(SSID);
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

  reconectWiFi();  // se não há conexão com o WiFI, a conexão é refeita
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

  unsigned long now = millis();
  if (now - lastMsg > 1000) {

  float temp_data = dht.readTemperature();  // or dht.readTemperature(true) for Fahrenheit
  dtostrf(temp_data, 4, 2, str_temp_data);
  /* 4 is mininum width, 2 is precision; float value is copied onto str_sensor*/
  float hum_data = dht.readHumidity();
  dtostrf(hum_data, 4, 2, str_hum_data);
  /* 4 is mininum width, 2 is precision; float value is copied onto str_sensor*/
  float tempF_data = dht.readTemperature(true);
  dtostrf(tempF_data, 4, 2, str_tempF_data);
  /* 4 is mininum width, 2 is precision; float value is copied onto str_sensor*/
  float tempterm_data = 0;
  dtostrf(tempterm_data, 4, 2, str_tempterm_data);
  /* 4 is mininum width, 2 is precision; float value is copied onto str_sensor*/
  tempterm_data = dht.computeHeatIndex(tempF_data, hum_data);
  tempterm_data = dht.convertFtoC(tempterm_data);
  dtostrf(tempterm_data, 4, 2, str_tempterm_data);
  /* 4 is mininum width, 2 is precision; float value is copied onto str_sensor*/

  lastMsg = now;

  MQTT.publish(pub9, str_temp_data);

  MQTT.publish(pub10, str_hum_data);

  MQTT.publish(pub11, str_tempterm_data);

  if (digitalRead(RelayPin1) == HIGH) {
  MQTT.publish(pub1, "0");
  } else {
  MQTT.publish(pub1, "1");
    }
  if (digitalRead(RelayPin2) == HIGH) {
  MQTT.publish(pub2, "0");
  } else {
  MQTT.publish(pub2, "1");
    }
  if (digitalRead(RelayPin3) == HIGH) {
  MQTT.publish(pub3, "0");
  } else {
  MQTT.publish(pub3, "1");
    }
  if (digitalRead(RelayPin4) == HIGH) {
  MQTT.publish(pub4, "0");
  } else {
  MQTT.publish(pub4, "1");
    }
  if (digitalRead(RelayPin5) == HIGH) {
  MQTT.publish(pub5, "0");
  } else {
  MQTT.publish(pub5, "1");
    }
  if (digitalRead(RelayPin6) == HIGH) {
  MQTT.publish(pub6, "0");
  } else {
  MQTT.publish(pub6, "1");
    }
  if (digitalRead(RelayPin7) == HIGH) {
  MQTT.publish(pub7, "0");
  } else {
  MQTT.publish(pub7, "1");
    }
  if (digitalRead(RelayPin8) == HIGH) {
  MQTT.publish(pub8, "0");
  } else {
  MQTT.publish(pub8, "1");
    }
  if (status_todos == 1) {
  MQTT.publish(pub0, "1");
  } else {
  MQTT.publish(pub0, "0");
    }

  val = digitalRead(pirPin);
  if (val == LOW) {
// Serial.println("Sem Movimento");
  MQTT.publish(motion_topic, "Sem Movimento");
  } else {
  Serial.println("Movimento Detectado");
  MQTT.publish(motion_topic, "Movimento Detectado");
    }
  }
  SinricPro.handle();
  handleFlipSwitches();

  // Garante funcionamento das conexões WiFi e ao Broker MQTT
  VerificaConexoesWiFIEMQTT();

  // Keep-Alive da comunicação com Broker MQTT
  MQTT.loop();
}