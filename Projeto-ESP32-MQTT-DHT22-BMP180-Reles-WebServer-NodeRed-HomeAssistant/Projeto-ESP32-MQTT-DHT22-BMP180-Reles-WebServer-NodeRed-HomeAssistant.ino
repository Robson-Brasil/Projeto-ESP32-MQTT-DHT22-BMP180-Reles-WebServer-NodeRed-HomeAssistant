/******************************************************************************************************************************************
  IoT - Automação Residencial
  Autor : Robson Brasil

  Dispositivos : ESP32 WROOM32, DHT22, BMP180, Módulo Relé de 8 Canais
  Preferences--> URLs adicionais do Gerenciador de placas:
                                    ESP8266: http://arduino.esp8266.com/stable/package_esp8266com_index.json,
                                    ESP32  : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  Download Board ESP32 (x.x.x):
  WebServer (Dashboard)
  OTA
  Broker MQTT
  Node-Red / Google Assistant-Nora:  https://smart-nora.eu/
  Para Instalação do Node-Red:       https://nodered.org/docs/getting-started/
  Home Assistant
  Para Instalação do Home Assistant: https://www.home-assistant.io/installation/
  Versão : 1.0 - Release Candidate
  Última Modificação : 05/09/2024
******************************************************************************************************************************************/

//Bibliotecas // <-- Se for cirar mais uma aba .h e ela fizer alguma alusão ao código, não esqueça de declar ela aqui nas Bibliotecas
#include "LoginsSenhas.h"
#include "TopicosMQTT.h"
#include "Bibliotecas.h"
#include "GPIOs.h"
#include "Sensores.h"
#include "MQTT.h"
#include "VariaveisGlobais.h"
#include "Array.h"

// Variável global
float diff = 1.0;

// Configuração do servidor DNS
DNSServer dns;

// Configuração de IP estático
IPAddress local_IP(192, 168, 10, 10); // <-- Altere aqui pro IP da sua rede
IPAddress gateway(192, 168, 10, 1);   // <-- Altere aqui pro Gateay da sua rede
IPAddress subnet(255, 255, 255, 0);   // <-- Aqui, normalmente não se altera

// Configuração de DNS estático
IPAddress primaryDNS(1, 1, 1, 1);     // <-- Aqui, normalmente não se altera
IPAddress secondaryDNS(8, 8, 8, 8);   // <-- Aqui, normalmente não se altera

// Configuração do servidor web
AsyncWebServer server(80); // <-- Comulmente essa porta é usada por algum recurso do Windows e muitas vezes até mesmo pelo roteador que você estiver usando, mude-a se precisar!
AsyncWebSocket ws("/ws");

const int numButtons = 8;
bool buttonStates[numButtons] = { false };

// Parâmetros do servidor web
const char* PARAM_INPUT_1 = "relay";
const char* PARAM_INPUT_2 = "state";

// Funções do Core 1 do ESP32
void setup1();  // <-- Declaração da função setup1()
void loop1();   // <-- Declaração da função loop1()

// Configuração das funções dos botões da página WebServer
const char serverIndex[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
  %BUTTONPLACEHOLDER%
</html>)rawliteral";

int relayPins[numButtons] = { RelayPin1, RelayPin2, RelayPin3, RelayPin4, RelayPin5, RelayPin6, RelayPin7, RelayPin8 };

void notifyClients() {
  String stateString = "";

  // Criar uma string que representa o estado de todos os botões
  for (int i = 0; i < numButtons; i++) {
    stateString += String(buttonStates[i]) + ",";
  }

  // Remover a última vírgula da string
  stateString.remove(stateString.length() - 1);

  // Enviar a string com o estado para todos os clientes WebSocket conectados
  ws.textAll(stateString);
}

void handleWebSocketMessage(void* arg, uint8_t* data, size_t len) {
  AwsFrameInfo* info = (AwsFrameInfo*)arg;

  // Verificar se a mensagem WebSocket está completa, é texto e os tamanhos correspondem
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    // Garantir que o buffer de dados tem espaço para o terminador nulo
    if (len < 128) {  // <-- Defina um limite de tamanho para evitar estouro de buffer
      data[len] = 0;  // <-- Adicionar o terminador nulo ao final dos dados recebidos

      // Converter os dados recebidos para um número inteiro
      int relayId = atoi((char*)data);

      // Verificar se o ID do relé é válido
      if (relayId >= 0 && relayId < numButtons) {
        // Alternar o estado do botão associado ao relé
        buttonStates[relayId] = !buttonStates[relayId];

        // Acionar o relé correspondente
        digitalWrite(relayPins[relayId], buttonStates[relayId] ? HIGH : LOW);

        // Notificar todos os clientes conectados sobre a mudança de estado
        notifyClients();
      } else {
        Serial.printf("ID do relé inválido: %d\n", relayId);
      }
    } else {
      Serial.println("Mensagem WebSocket muito longa para processar");
    }
  } else {
    Serial.println("Mensagem WebSocket inválida ou incompleta recebida");
  }
}

void onEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type,
             void* arg, uint8_t* data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      // Cliente conectado
      Serial.printf("WebSocket client #%lu connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      // Cliente desconectado
      Serial.printf("WebSocket client #%lu disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      // Dados recebidos do cliente
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
      // Resposta ao ping (opcional)
      break;
    case WS_EVT_ERROR:
      // Tratamento de erro (opcional)
      Serial.printf("Erro no WebSocket client #%lu\n", client->id());
      break;
  }
}

// Função: Inicializa o WebSocket
void initWebSocket() {
  // Configurar o evento do WebSocket
  ws.onEvent(onEvent);

  // Adicionar o WebSocket ao servidor
  server.addHandler(&ws);
}


String outputState(int buttonId) {
  return buttonStates[buttonId] ? "checked" : "";
}

String processor(const String& var) {
  if (var == "BUTTONPLACEHOLDER1") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 1</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"0\" " + outputState(0) + "><span class=\"slider\"></span></label>";
    return buttons;
  }

  if (var == "BUTTONPLACEHOLDER2") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 2</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"1\" " + outputState(1) + "><span class=\"slider\"></span></label>";
    return buttons;
  }

  if (var == "BUTTONPLACEHOLDER3") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 3</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"2\" " + outputState(2) + "><span class=\"slider\"></span></label>";
    return buttons;
  }

  if (var == "BUTTONPLACEHOLDER4") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 4</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"3\" " + outputState(3) + "><span class=\"slider\"></span></label>";
    return buttons;
  }

  if (var == "BUTTONPLACEHOLDER5") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 5</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"4\" " + outputState(4) + "><span class=\"slider\"></span></label>";
    return buttons;
  }

  if (var == "BUTTONPLACEHOLDER6") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 6</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"5\" " + outputState(5) + "><span class=\"slider\"></span></label>";
    return buttons;
  }

  if (var == "BUTTONPLACEHOLDER7") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 7</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"6\" " + outputState(6) + "><span class=\"slider\"></span></label>";
    return buttons;
  }

  if (var == "BUTTONPLACEHOLDER8") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 8</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"7\" " + outputState(7) + "><span class=\"slider\"></span></label>";
    return buttons;
  }

  return String();
}

void toggleCheckbox(int checkboxId, bool checkboxState) {
  buttonStates[checkboxId] = checkboxState;
  digitalWrite(relayPins[checkboxId], checkboxState ? HIGH : LOW);
}

#define MSG_BUFFER_SIZE (1000)

//Função MILLIS
unsigned long lastMsgDHT = 0;
unsigned long lastMsgBMP180 = 0;
unsigned long lastMsgMQTT = 0;
unsigned long delayTime = 0;
int value = 0;

// Protótipos
void initSerial();
void initWiFi();
void initMQTT();
void reconectWiFi();
void MQTT_CallBack(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIeMQTT();
void initOutput();
void initSPIFFS();
void initOTA();
void SensoresMQTT();
void RelayMQTT();

// Função: Inicializa o output em nível lógico baixo
void initOutput() {
  for (int i = 0; i < numRelays; i++) {
    pinMode(RelayPins[i], OUTPUT);
    digitalWrite(RelayPins[i], HIGH);  // <-- Durante a partida, todos os Relés iniciam desligados
  }

  // Adiciona um pequeno delay para estabilizar o estado inicial dos pinos
  delay(1000);

  // Durante a partida o LED WiFI, inicia desligado
  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, HIGH);
}

//Função: Inicializa comunicação serial com baudrate 115200 (para fins de monitorar no terminal serial
void initSerial() {
  Serial.begin(115200);
}

//Função: Inicializa e conecta-se na rede WI-FI desejada
void initWiFi() {

  delay(1000);

  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(ssid);
  Serial.println("Aguarde");

  reconectWiFi();
}

// Função: Inicializa parâmetros de conexão MQTT(endereço do broker, porta e seta função de callback)
void initMQTT() {

  MQTT.setServer(BrokerMQTT, PortaBroker);  // <-- Informa qual broker e porta deve ser conectado
  MQTT.setCallback(MQTT_CallBack);          // <-- Atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
  MQTT.setKeepAlive(MQTT_KeepAlive);        // <-- Defina o keep-alive
}

// Função: Inicializa o SPIFFS
void initSPIFFS() {
  // Inicializa o SPIFFS e verifica se ocorreu algum erro
  if (!SPIFFS.begin(true)) {
    Serial.println("Ocorreu um erro ao montar o SPIFFS");
    return;
  }

  // Rota para a página de índice do servidor
  server.on("/serverIndex", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/html", serverIndex);
  });

  // Rota para o arquivo WebServer.html (exige autenticação)
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(LoginDoHTTP, SenhaDoHTTP))
      return request->requestAuthentication();
    request->send(SPIFFS, "/WebServer.html", String(), false, processor); // <-- Se for usar WebServer, atente-se para o nome do arquivo, tem de ser o mesmo delarado aqui!
  });

  // Serve arquivos estáticos diretamente do SPIFFS
  server.serveStatic("/", SPIFFS, "/");

  // Rota para servir o arquivo CSS
  server.on("/WebServer.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/WebServer.css", "text/css");	// <-- Se for usar WebServer, atente-se para o nome do arquivo, tem de ser o mesmo delarado aqui!
  });

  // Rota para servir o arquivo JavaScript
  server.on("/WebServer.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/WebServer.js", "application/javascript");	// <-- Se for usar WebServer, atente-se para o nome do arquivo, tem de ser o mesmo delarado aqui!
  });

  // Rota para servir o logo
  server.on("/logo.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/logo.png", "image/png");	// <-- Se for usar WebServer, atente-se para o nome do arquivo, tem de ser o mesmo delarado aqui!
  });

  // Rota para controlar as ações dos botões no WebServer
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest * request) {
    String inputMessage1;
    String inputParam1;
    String inputMessage2;
    String inputParam2;
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      inputParam1 = PARAM_INPUT_1;
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      inputParam2 = PARAM_INPUT_2;
      digitalWrite(inputMessage1.toInt(), !inputMessage2.toInt());
    } else {
      inputMessage1 = "Mensagem não enviada";
      inputMessage2 = "Mensagem não enviada";
    }
    request->send(200, "text/plain", "OK");
  });

  // Carrega e executa o código do arquivo WebServer.txt
  File file = SPIFFS.open("/WebServer.txt", "r"); // <-- Obrigatório ter um arquivo na memória pra poder o SPIFSS verifica-lo
  if (!file) {
    Serial.println("Falha ao abrir o arquivo WebServer.txt");
    return;
  }

  String code = file.readString();
  file.close();

  // Executa o código lido do arquivo
  if (code.length() > 0) {
    Serial.println("SPIFFS: Executando código do arquivo WebServer.txt");
    Serial.println("");
  } else {
    Serial.println("O arquivo WebServer.txt está vazio");
  }
}

// Função: Para leitura dos Estados dos Relés e envio ao MQTT Broker
void RelayMQTT(){
  unsigned long currentTimeMQTT = millis();

  if (currentTimeMQTT - lastMsgMQTT > 100) {
    lastMsgMQTT = currentTimeMQTT;  // <-- Atualiza o último tempo de execução

    // Código executado a cada 100 milissegundos, para ler os status de cada relé
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

    if (status_todos == 1) {
      MQTT.publish(pub0, "1", true);
    } else {
      MQTT.publish(pub0, "0", true);
    }
  }
}

//Implementação das Funções Principais do Core0 do ESP32
void setup() {
  // Inicializações:
  initOutput();
  initSerial();
  initWiFi();
  initMQTT();
  initSPIFFS();
  initOTA();
  initWebSocket();

  // Adiciona um pequeno delay para estabilizar o estado inicial dos pinos
  delay(1000); // <-- Nesse código, serão pouqissímos delays usados, no mais o MILLIS é a melhor opção!

  //Chama a função setup1 do Core1()
  setup1();

  // Start do Servidor WebServer
  server.begin();

  // Inicialize os estados dos botões como falso (desligado) ao iniciar o programa
  for (int i = 0; i < numButtons; ++i) {
    buttonStates[i] = false;
  }
}

// Programa Principal do Core0 do ESP32
void loop() {
  // Garante funcionamento das conexões WiFi e ao Broker MQTT
  VerificaConexoesWiFIeMQTT();
  //Keep-Alive da comunicação com Broker MQTT
  MQTT.loop();

  ArduinoOTA.handle();

  // Inicializa o loop1 do Core 1
  loop1();

  ws.cleanupClients();

  RelayMQTT();

}

// Função: Inicializa o OTA
void initOTA() {
  // Port defaults to 3232
  ArduinoOTA.setPort(3232); // <-- Atenção aqui, mesmo setando a porta 3232, se no "AsyncWebServer server(80);" 
							// a porta que você setou estiver sendo usada principalmente pelo roteador, o OTA não irá habilitar!

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("ESP32-IoT");

  // No authentication by default
  ArduinoOTA.setPassword("Sua Senha Aqui"); // <-- Altere aqui a senha para atualziar via OTA

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  })
  .onEnd([]() {
    Serial.println("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });

  ArduinoOTA.begin();
}

// Função: Inicializa o callback, esta função é chamada toda vez que uma informação de um dos tópicos subescritos chega.
void MQTT_CallBack(char* topic, byte* payload, unsigned int length) {

  payload[length] = '\0';
  String data = "";

  if (strstr(topic, sub0)) {
    for (unsigned int i = 0; i < length; i++) {
      data += (char)payload[i];
    }

    if ((char)payload[0] == '0') {

      digitalWrite(RelayPin1, HIGH);  // <-- Desliga o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin2, HIGH);  // <-- Desliga o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin3, HIGH);  // <-- Desliga o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin4, HIGH);  // <-- Desliga o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin5, HIGH);  // <-- Desliga o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin6, HIGH);  // <-- Desliga o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin7, HIGH);  // <-- Desliga o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin8, HIGH);  // <-- Desliga o relé. Note que HIGH é o nível de tensão.
      status_todos = 0;
      toggleState_0 = 0;
      MQTT.publish(pub0, "0");
    } else {
      digitalWrite(RelayPin1, LOW);  // <-- Liga o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin2, LOW);  // <-- Liga o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin3, LOW);  // <-- Liga o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin4, LOW);  // <-- Liga o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin5, LOW);  // <-- Liga o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin6, LOW);  // <-- Liga o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin7, LOW);  // <-- Liga o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin8, LOW);  // <-- Liga o Relé tornando a tensão BAIXA
      status_todos = 1;
      toggleState_0 = 1;
      MQTT.publish(pub0, "1");
    }
  }
  if (strstr(topic, sub1)) {
    for (unsigned int i = 0; i < length; i++) {
      data += (char)payload[i];
    }

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin1, HIGH);  // <-- Desliga o relé. Note que HIGH é o nível de tensão.
      toggleState_1 = 0;
      MQTT.publish(pub1, "0", true);
    } else {
      digitalWrite(RelayPin1, LOW);  // <-- Liga o Relé tornando a tensão BAIXA
      toggleState_1 = 1;
      MQTT.publish(pub1, "1", true);
    }
  }
  if (strstr(topic, sub2)) {
    for (unsigned int i = 0; i < length; i++) {
      data += (char)payload[i];
    }

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin2, HIGH);  // <-- Desliga o relé. Note que HIGH é o nível de tensão.
      toggleState_2 = 0;
      MQTT.publish(pub2, "0", true);
    } else {
      digitalWrite(RelayPin2, LOW);  // <-- Liga o Relé tornando a tensão BAIXA
      toggleState_2 = 1;
      MQTT.publish(pub2, "1", true);
    }
  }
  if (strstr(topic, sub3)) {
    for (unsigned int i = 0; i < length; i++) {
      data += (char)payload[i];
    }

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin3, HIGH);  // <-- Desliga o relé. Note que HIGH é o nível de tensão.
      toggleState_3 = 0;
      MQTT.publish(pub3, "0", true);
    } else {
      digitalWrite(RelayPin3, LOW);  // <-- Liga o Relé tornando a tensão BAIXA
      toggleState_3 = 1;
      MQTT.publish(pub3, "1", true);
    }
  }
  if (strstr(topic, sub4)) {
    for (unsigned int i = 0; i < length; i++) {
      data += (char)payload[i];
    }

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin4, HIGH);  // <-- Desliga o relé. Note que HIGH é o nível de tensão.
      toggleState_4 = 0;
      MQTT.publish(pub4, "0", true);
    } else {
      digitalWrite(RelayPin4, LOW);  // <-- Liga o Relé tornando a tensão BAIXA
      toggleState_4 = 1;
      MQTT.publish(pub4, "1", true);
    }
  }
  if (strstr(topic, sub5)) {
    for (unsigned int i = 0; i < length; i++) {
      data += (char)payload[i];
    }

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin5, HIGH);  // <-- Desliga o relé. Note que HIGH é o nível de tensão.
      toggleState_5 = 0;
      MQTT.publish(pub5, "0", true);
    } else {
      digitalWrite(RelayPin5, LOW);  // <-- Liga o Relé tornando a tensão BAIXA
      toggleState_5 = 1;
      MQTT.publish(pub5, "1", true);
    }
  }
  if (strstr(topic, sub6)) {
    for (unsigned int i = 0; i < length; i++) {
      data += (char)payload[i];
    }

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin6, HIGH);  // <-- Desliga o relé. Note que HIGH é o nível de tensão.
      toggleState_6 = 0;
      MQTT.publish(pub6, "0", true);
    } else {
      digitalWrite(RelayPin6, LOW);  // <-- Liga o Relé tornando a tensão BAIXA
      toggleState_6 = 1;
      MQTT.publish(pub6, "1", true);
    }
  }
  if (strstr(topic, sub7)) {
    for (unsigned int i = 0; i < length; i++) {
      data += (char)payload[i];
    }

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin7, HIGH);  // <-- Desliga o relé. Note que HIGH é o nível de tensão.
      toggleState_7 = 0;
      MQTT.publish(pub7, "0", true);
    } else {
      digitalWrite(RelayPin7, LOW);  // <-- Liga o Relé tornando a tensão BAIXA
      toggleState_7 = 1;
      MQTT.publish(pub7, "1", true);
    }
  }
  if (strstr(topic, sub8)) {
    for (unsigned int i = 0; i < length; i++) {
      data += (char)payload[i];
    }

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin8, HIGH);  // <-- Desliga o relé. Note que HIGH é o nível de tensão.
      toggleState_8 = 0;
      MQTT.publish(pub8, "0", true);
    } else {
      digitalWrite(RelayPin8, LOW);  // <-- Liga o Relé tornando a tensão BAIXA
      toggleState_8 = 1;
      MQTT.publish(pub8, "1", true);
    }
  }
}

/* Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
   em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.*/
void reconnectMQTT() {

  unsigned long currentTime = millis();
  unsigned long reconnectTime = 5000;  // <-- Tempo para tentar reconectar (em milissegundos)
  static unsigned long lastReconnectAttempt = 0; // <-- Armazena o último tempo de tentativa

  // Verifica o overflow de millis
  if (currentTime < lastReconnectAttempt) {
    /* Overflow ocorreu
       Lógica para lidar com o overflow, se necessário
       Por exemplo, reiniciar o último tempo para o valor atual */
    lastReconnectAttempt = currentTime;
  }

  if (!MQTT.connected()) {
    if (currentTime - lastReconnectAttempt > reconnectTime) {
      Serial.print(".....Tentando se conectar ao Broker MQTT: ");
      Serial.println(BrokerMQTT);
      if (MQTT.connect(ID_MQTT, LoginDoMQTT, SenhaMQTT)) {
        Serial.println("Conectado com sucesso ao broker MQTT!");
        Serial.println("");
        MQTT.subscribe(sub0);
        MQTT.subscribe(sub1);
        MQTT.subscribe(sub2);
        MQTT.subscribe(sub3);
        MQTT.subscribe(sub4);
        MQTT.subscribe(sub5);
        MQTT.subscribe(sub6);
        MQTT.subscribe(sub7);
        MQTT.subscribe(sub8);
      } else {
        Serial.println("Falha ao reconectar no broker.");
        Serial.print(MQTT.state());
        Serial.println(" Haverá nova tentativa de conexão em 5s");
      }
      lastReconnectAttempt = currentTime;  // <-- Atualiza o último tempo de tentativa de reconexão
    }
  } else {
    lastReconnectAttempt = currentTime;  // <-- Atualiza o último tempo caso a conexão seja bem-sucedida
  }
}

// Função: Reconectar-se ao WiFi
void reconectWiFi() {
  /* Se já está conectado a rede WI-FI, nada é feito.
    Caso contrário, são efetuadas tentativas de conexão */
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(ssid, password);  // <-- Conecta na rede WI-FI
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
  Serial.println("");
}

/* Função: Verifica o estado das conexões WiFI e ao broker MQTT.
   Em caso de desconexão (qualquer uma das duas), a conexão  é refeita. */
void VerificaConexoesWiFIeMQTT() {

  if (!MQTT.connected())

    reconnectMQTT();  // <-- Se não há conexão com o Broker, a conexão é refeita

    reconectWiFi();  	  // <-- Se não há conexão com o WiFI, a conexão é refeita "apagar essa linha depois pra testar"
}

// Função: Para leitura dos sensores DHT22 e BMP180 e envio pro MQTT Broker
void SensoresMQTT(){
  // Sensor DHT22  - Temperatura e Umidade
  unsigned long currentTimeDHT = millis();

  if (currentTimeDHT - lastMsgDHT > 60000) {
    lastMsgDHT = currentTimeDHT;  // <-- Atualiza o último tempo de execução

    // Código executado a cada 60000 milissegundos (1 minuto)
    float temp_data = dht.readTemperature();      // <-- Leitura da temperatura em graus Celsius (°C) pelo sensor DHT22
    dtostrf(temp_data, 6, 2, str_temp_data);      // <-- Conversão do valor float de temperatura para string com 6 caracteres e 2 casas decimais

    float hum_data = dht.readHumidity();          // <-- Leitura da umidade relativa do ar (%)
    dtostrf(hum_data, 6, 2, str_hum_data);        // <-- Conversão do valor float de umidade para string com 6 caracteres e 2 casas decimais

    float tempF_data = dht.readTemperature(true); // <-- Leitura da temperatura em graus Fahrenheit (°F)
    dtostrf(tempF_data, 6, 2, str_tempF_data);    // <-- Conversão do valor float de temperatura em Fahrenheit para string com 6 caracteres e 2 casas decimais

    float tempterm_data = dht.computeHeatIndex(tempF_data, hum_data); // <-- Cálculo da sensação térmica (heat index) com base na temperatura em Fahrenheit e umidade
    tempterm_data = dht.convertFtoC(tempterm_data);   // <-- Conversão da sensação térmica de Fahrenheit para Celsius
    dtostrf(tempterm_data, 6, 2, str_tempterm_data);  // <-- Conversão do valor float de sensação térmica para string com 6 caracteres e 2 casas decimais

    // Publica os dados no MQTT
    MQTT.publish(pub9, str_temp_data);
    MQTT.publish(pub10, str_hum_data);
    MQTT.publish(pub11, str_tempterm_data);
  }

  // Sensor BMP180
  // Leitura da Temperatura, Altitude e Pressão Atmosférica
  unsigned long currentTimeBMP180 = millis();

  if (currentTimeBMP180 - lastMsgBMP180 > 120000) {
    lastMsgBMP180 = currentTimeBMP180;  // <-- Atualiza o último tempo de execução

    // Código executado a cada 120000 milissegundos (2 minutos)
    float pressaoNivelMar = 1012;  // <-- Pressão ao nível do mar em hPa
    float altitudeNivelMar = 92;   // <-- Altitude da cidade em metros

    char buffer[10];  // <-- Buffer para armazenar a string convertida

    // Leitura da pressão atmosférica (hPa) a partir do sensor BMP180
    dtostrf(bmp.readPressure() / 100.0, 2, 2, buffer);  // <-- A pressão lida é convertida de Pascal para hectopascal (hPa) ao dividir por 100.0
    MQTT.publish(pub14, buffer);

    // Leitura da pressão ao nível do mar (calculada)
    dtostrf(bmp.readSealevelPressure(pressaoNivelMar) / 100.0, 2, 2, buffer); // <-- Utiliza o valor de pressão ao nível do mar definido (pressaoNivelMar) para o cálculo
    MQTT.publish(pub15, buffer);

    float altitudeReal = bmp.readAltitude(pressaoNivelMar * 100); // <-- Leitura da altitude real (metros) com base na pressão lida e na pressão ao nível do mar
    dtostrf(altitudeReal, 2, 2, buffer);  // <-- Conversão do valor da altitude para string com 2 casas decimais
    MQTT.publish(pub16, buffer);

    // Cálculo da altitude ajustada ao nível do mar, somando a altitude real com a altitude da cidade (altitudeNivelMar)
    float altitudeAoNivelDoMar = altitudeReal + altitudeNivelMar; // <-- Isso ajusta o valor da altitude com base na localização específica da cidade
    dtostrf(altitudeAoNivelDoMar, 2, 2, buffer);  // <-- Conversão do valor da altitude ajustada para string com 2 casas decimais
    MQTT.publish(pub17, buffer);
  }
}

//Implementação das Funções Principais do Core1 do ESP32
void setup1() {

  initMQTT();

  dht.begin();  // Inicializa o sensor DHT11

  // Start do Sensor BMP180
  if (!bmp.begin()) {
    Serial.println("Não foi possível encontrar um sensor BMP180 válido, por favor, verifique a conexão!");
    // Em vez de entrar em loop infinito, apenas pule a inicialização do sensor e continue a execução
  } else {
    Serial.println("Sensor BMP180 encontrado e inicializado com sucesso.");
    // Continue com a configuração do sensor BMP180 aqui, se necessário
  }
}

// loop do Core1 do ESP32
void loop1() {
  // Garante funcionamento das conexões WiFi e ao Broker MQTT
  VerificaConexoesWiFIeMQTT();
  // Keep-Alive da comunicação com Broker MQTT
  MQTT.loop();  // <-- Verifica se há novas mensagens no Broker MQTT
  // Garante a leitura dos sensores DHT22 e BMP180
  SensoresMQTT();
}