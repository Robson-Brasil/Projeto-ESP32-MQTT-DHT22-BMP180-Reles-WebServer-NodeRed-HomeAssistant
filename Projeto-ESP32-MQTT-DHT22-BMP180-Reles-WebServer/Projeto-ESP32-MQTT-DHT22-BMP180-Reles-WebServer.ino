/******************************************************************************************************************************************
  IoT - Automa????o Residencial
  Autor : Robson Brasil

  Dispositivos : ESP32 WROOM32, DHT22, BMP180, M??dulo Rel?? de 8 Canais
  Preferences--> URLs adicionais do Gerenciador de placas:
                                    ESP8266: http://arduino.esp8266.com/stable/package_esp8266com_index.json,
                                    ESP32  : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  Download Board ESP32 (x.x.x):
  WebServer (Dashboard)
  OTA
  Broker MQTT
  Node-Red / Google Assistant-Nora:  https://smart-nora.eu/
  Para Instala????o do Node-Red:       https://nodered.org/docs/getting-started/
  Home Assistant
  Para Instala????o do Home Assistant: https://www.home-assistant.io/installation/
  Vers??o : 1.0 - Release Candidate
  ??ltima Modifica????o : 05/09/2024
******************************************************************************************************************************************/

//Bibliotecas // <-- Se for cirar mais uma aba .h e ela fizer alguma alus??o ao c??digo, n??o esque??a de declar ela aqui nas Bibliotecas
#include "LoginsSenhas.h"
#include "TopicosMQTT.h"
#include "Bibliotecas.h"
#include "GPIOs.h"
#include "Sensores.h"
#include "MQTT.h"
#include "VariaveisGlobais.h"
#include "Array.h"

// Vari??vel global
float diff = 1.0;

// Configura????o do servidor DNS
DNSServer dns;

// Configura????o de IP est??tico
IPAddress local_IP(192, 168, 10, 10); // <-- Altere aqui pro IP da sua rede
IPAddress gateway(192, 168, 10, 1);   // <-- Altere aqui pro Gateay da sua rede
IPAddress subnet(255, 255, 255, 0);   // <-- Aqui, normalmente n??o se altera

// Configura????o de DNS est??tico
IPAddress primaryDNS(1, 1, 1, 1);     // <-- Aqui, normalmente n??o se altera
IPAddress secondaryDNS(8, 8, 8, 8);   // <-- Aqui, normalmente n??o se altera

// Configura????o do servidor web
AsyncWebServer server(80); // <-- Comulmente essa porta ?? usada por algum recurso do Windows e muitas vezes at?? mesmo pelo roteador que voc?? estiver usando, mude-a se precisar!
AsyncWebSocket ws("/ws");

const int numButtons = 8;
bool buttonStates[numButtons] = { false };

// Par??metros do servidor web
const char* PARAM_INPUT_1 = "relay";
const char* PARAM_INPUT_2 = "state";

// Fun????es do Core 1 do ESP32
void setup1();  // <-- Declara????o da fun????o setup1()
void loop1();   // <-- Declara????o da fun????o loop1()

// Configura????o das fun????es dos bot??es da p??gina WebServer
const char serverIndex[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
  %BUTTONPLACEHOLDER%
</html>)rawliteral";

int relayPins[numButtons] = { RelayPin1, RelayPin2, RelayPin3, RelayPin4, RelayPin5, RelayPin6, RelayPin7, RelayPin8 };

void notifyClients() {
  String stateString = "";

  // Criar uma string que representa o estado de todos os bot??es
  for (int i = 0; i < numButtons; i++) {
    stateString += String(buttonStates[i]) + ",";
  }

  // Remover a ??ltima v??rgula da string
  stateString.remove(stateString.length() - 1);

  // Enviar a string com o estado para todos os clientes WebSocket conectados
  ws.textAll(stateString);
}

void handleWebSocketMessage(void* arg, uint8_t* data, size_t len) {
  AwsFrameInfo* info = (AwsFrameInfo*)arg;

  // Verificar se a mensagem WebSocket est?? completa, ?? texto e os tamanhos correspondem
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    // Garantir que o buffer de dados tem espa??o para o terminador nulo
    if (len < 127) {  // <-- Defina um limite de tamanho para evitar estouro de buffer (127 to leave space for null terminator)
      uint8_t tempData[128];  // <-- Criar um buffer tempor??rio
      memcpy(tempData, data, len);  // <-- Copiar os dados para o buffer tempor??rio
      tempData[len] = 0;  // <-- Adicionar o terminador nulo ao final dos dados recebidos

      // Converter os dados recebidos para um n??mero inteiro
      int relayId = atoi((char*)tempData);

      // Verificar se o ID do rel?? ?? v??lido
      if (relayId >= 0 && relayId < numButtons) {
        // Alternar o estado do bot??o associado ao rel??
        buttonStates[relayId] = !buttonStates[relayId];

        // Acionar o rel?? correspondente
        digitalWrite(relayPins[relayId], buttonStates[relayId] ? HIGH : LOW);

        // Notificar todos os clientes conectados sobre a mudan??a de estado
        notifyClients();
      } else {
        Serial.printf("ID do rel?? inv??lido: %d\n", relayId);
      }
    } else {
      Serial.println("Mensagem WebSocket muito longa para processar");
    }
  } else {
    Serial.println("Mensagem WebSocket inv??lida ou incompleta recebida");
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

// Fun????o: Inicializa o WebSocket
void initWebSocket() {
  // Configurar o evento do WebSocket
  ws.onEvent(onEvent);

  // Adicionar o WebSocket ao servidor
  server.addHandler(&ws);
  
  // Iniciar o WebSocket
}


String outputState(int buttonId) {
  return buttonStates[buttonId] ? "checked" : "";
}

String processor(const String& var) {
  if (var == "BUTTONPLACEHOLDER") {
    String buttons = "";
    
    // Interruptor 1
    buttons += "<div class=\"switch-row\">";
    buttons += "<label class=\"switch-button\">";
    buttons += "<span class=\"button-name\">Quarto 1</span>";
    buttons += "<input type=\"checkbox\" id=\"26\" " + outputState(0) + ">";
    buttons += "<span class=\"slider-button\"></span>";
    buttons += "</label>";
    
    // Interruptor 2
    buttons += "<label class=\"switch-button\">";
    buttons += "<span class=\"button-name\">Bancada</span>";
    buttons += "<input type=\"checkbox\" id=\"13\" " + outputState(1) + ">";
    buttons += "<span class=\"slider-button\"></span>";
    buttons += "</label>";
    
    // Interruptor 3
    buttons += "<label class=\"switch-button\">";
    buttons += "<span class=\"button-name\">Cooler</span>";
    buttons += "<input type=\"checkbox\" id=\"14\" " + outputState(2) + ">";
    buttons += "<span class=\"slider-button\"></span>";
    buttons += "</label>";
    
    // Interruptor 4
    buttons += "<label class=\"switch-button\">";
    buttons += "<span class=\"button-name\">Varanda</span>";
    buttons += "<input type=\"checkbox\" id=\"19\" " + outputState(3) + ">";
    buttons += "<span class=\"slider-button\"></span>";
    buttons += "</label>";
    buttons += "</div>";
    
    // Interruptor 5
    buttons += "<div class=\"switch-row\">";
    buttons += "<label class=\"switch-button\">";
    buttons += "<span class=\"button-name\">Quarto 2</span>";
    buttons += "<input type=\"checkbox\" id=\"16\" " + outputState(4) + ">";
    buttons += "<span class=\"slider-button\"></span>";
    buttons += "</label>";
    
    // Interruptor 6
    buttons += "<label class=\"switch-button\">";
    buttons += "<span class=\"button-name\">Quarto 3</span>";
    buttons += "<input type=\"checkbox\" id=\"17\" " + outputState(5) + ">";
    buttons += "<span class=\"slider-button\"></span>";
    buttons += "</label>";
    
    // Interruptor 7
    buttons += "<label class=\"switch-button\">";
    buttons += "<span class=\"button-name\">Cozinha</span>";
    buttons += "<input type=\"checkbox\" id=\"18\" " + outputState(6) + ">";
    buttons += "<span class=\"slider-button\"></span>";
    buttons += "</label>";
    
    // Interruptor 8
    buttons += "<label class=\"switch-button\">";
    buttons += "<span class=\"button-name\">Corredor</span>";
    buttons += "<input type=\"checkbox\" id=\"32\" " + outputState(7) + ">";
    buttons += "<span class=\"slider-button\"></span>";
    buttons += "</label>";
    buttons += "</div>";
    
    return buttons;
  }

  return String();
}



// Vari??veis MILLIS para controle de tempo
unsigned long lastMsgDHT = 0;     // ??ltimo tempo de envio de dados do sensor DHT
unsigned long lastMsgBMP180 = 0;  // ??ltimo tempo de envio de dados do sensor BMP180
unsigned long lastMsgMQTT = 0;    // ??ltimo tempo de envio de estados dos rel??s
unsigned long delayTime = 0;      // Vari??vel para controle de delay
int value = 0;                    // Vari??vel auxiliar

// Prot??tipos das fun????es
void initSerial();                           // Inicializa a comunica????o serial
void initWiFi();                             // Inicializa a conex??o WiFi
void initMQTT();                             // Inicializa a conex??o MQTT
void reconectWiFi();                         // Reconecta ao WiFi se a conex??o cair
void MQTT_CallBack(char* topic, byte* payload, unsigned int length);  // Callback para mensagens MQTT
void VerificaConexoesWiFIeMQTT();            // Verifica e reconecta WiFi e MQTT se necess??rio
void initOutput();                           // Inicializa os pinos de sa??da (rel??s)
void initSPIFFS();                           // Inicializa o sistema de arquivos SPIFFS
void initOTA();                              // Inicializa a atualiza????o Over-The-Air
void SensoresMQTT();                         // L?? os sensores e envia dados via MQTT
void RelayMQTT();                            // Envia estados dos rel??s via MQTT

// Fun????o: Inicializa o output em n??vel l??gico baixo
void initOutput() {
  for (int i = 0; i < numRelays; i++) {
    pinMode(RelayPins[i], OUTPUT);
    digitalWrite(RelayPins[i], HIGH);  // <-- Durante a partida, todos os Rel??s iniciam desligados
  }

  // Adiciona um pequeno delay para estabilizar o estado inicial dos pinos
  delay(1000);

  // Durante a partida o LED WiFI, inicia desligado
  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, HIGH);
}

//Fun????o: Inicializa comunica????o serial com baudrate 115200 (para fins de monitorar no terminal serial
void initSerial() {
  Serial.begin(115200);
}

//Fun????o: Inicializa e conecta-se na rede WI-FI desejada
void initWiFi() {

  delay(1000);

  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(ssid);
  Serial.println("Aguarde");

  reconectWiFi();
}

// Fun????o: Inicializa par??metros de conex??o MQTT(endere??o do broker, porta e seta fun????o de callback)
void initMQTT() {

  MQTT.setServer(BrokerMQTT, PortaBroker);  // <-- Informa qual broker e porta deve ser conectado
  MQTT.setCallback(MQTT_CallBack);          // <-- Atribui fun????o de callback (fun????o chamada quando qualquer informa????o de um dos t??picos subescritos chega)
  MQTT.setKeepAlive(MQTT_KeepAlive);        // <-- Defina o keep-alive
}

// Fun????o: Inicializa o SPIFFS
void initSPIFFS() {
  // Inicializa o SPIFFS e verifica se ocorreu algum erro
  if (!SPIFFS.begin(true)) {
    Serial.println("Ocorreu um erro ao montar o SPIFFS");
    // Try to format SPIFFS and try again
    if (!SPIFFS.format()) {
      Serial.println("Falha ao formatar o SPIFFS");
      return;
    }
    
    if (!SPIFFS.begin(true)) {
      Serial.println("Falha ao montar o SPIFFS mesmo ap??s formata????o");
      return;
    }
  }

  // Rota para a p??gina de ??ndice do servidor
  server.on("/serverIndex", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/html", serverIndex);
  });

  // Rota para o arquivo WebServer.html (exige autentica????o)
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(LoginDoHTTP, SenhaDoHTTP))
      return request->requestAuthentication();
    request->send(SPIFFS, "/WebServer.html", String(), false, processor); // <-- Se for usar WebServer, atente-se para o nome do arquivo, tem de ser o mesmo delarado aqui!
  });

  // Serve arquivos est??ticos diretamente do SPIFFS
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

  // Rota para controlar as a????es dos bot??es no WebServer
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
      inputMessage1 = "Mensagem n??o enviada";
      inputMessage2 = "Mensagem n??o enviada";
    }
    request->send(200, "text/plain", "OK");
  });

  // Carrega e executa o c??digo do arquivo WebServer.txt
  File file = SPIFFS.open("/WebServer.txt", "r"); // <-- Obrigat??rio ter um arquivo na mem??ria pra poder o SPIFSS verifica-lo
  if (!file) {
    Serial.println("Falha ao abrir o arquivo WebServer.txt");
    // Create the file if it doesn't exist
    File newFile = SPIFFS.open("/WebServer.txt", "w");
    if (newFile) {
      newFile.close();
      Serial.println("Arquivo WebServer.txt criado com sucesso");
    } else {
      Serial.println("Falha ao criar o arquivo WebServer.txt");
    }
    return;
  }

  String code = file.readString();
  file.close();

  // Executa o c??digo lido do arquivo
  if (code.length() > 0) {
    Serial.println("SPIFFS: Executando c??digo do arquivo WebServer.txt");
    Serial.println("");
  } else {
    Serial.println("O arquivo WebServer.txt est?? vazio");
  }
}

// Fun????o: Para leitura dos Estados dos Rel??s e envio ao MQTT Broker
void RelayMQTT(){
  unsigned long currentTimeMQTT = millis();

  if (currentTimeMQTT - lastMsgMQTT > 100) {
    lastMsgMQTT = currentTimeMQTT;  // <-- Atualiza o ??ltimo tempo de execu????o

    // C??digo executado a cada 100 milissegundos, para ler os status de cada rel??
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

//Implementa????o das Fun????es Principais do Core0 do ESP32
void setup() {
  // Inicializa????es:
  initOutput();
  initSerial();
  initWiFi();
  initMQTT();
  initSPIFFS();
  initOTA();
  initWebSocket();

  // Adiciona um pequeno delay para estabilizar o estado inicial dos pinos
  delay(1000); // <-- Nesse c??digo, ser??o pouqiss??mos delays usados, no mais o MILLIS ?? a melhor op????o!

  //Chama a fun????o setup1 do Core1()
  setup1();

  // Start do Servidor WebServer
  server.begin();

  // Inicialize os estados dos bot??es como falso (desligado) ao iniciar o programa
  for (int i = 0; i < numButtons; ++i) {
    buttonStates[i] = false;
  }
}

// Programa Principal do Core0 do ESP32
void loop() {
  // Garante funcionamento das conex??es WiFi e ao Broker MQTT
  VerificaConexoesWiFIeMQTT();
  //Keep-Alive da comunica????o com Broker MQTT
  MQTT.loop();

  ArduinoOTA.handle();

  // Handle WebSocket clients
  ws.cleanupClients();

  // Send relay states to MQTT
  RelayMQTT();
  
  // Small delay to prevent watchdog timeout
  delay(10);
}

// Fun????o: Inicializa o OTA
void initOTA() {
  // Port defaults to 3232
  ArduinoOTA.setPort(3232); // <-- Aten????o aqui, mesmo setando a porta 3232, se no "AsyncWebServer server(80);" 
							// a porta que voc?? setou estiver sendo usada principalmente pelo roteador, o OTA n??o ir?? habilitar!

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

// Fun????o: Inicializa o callback, esta fun????o ?? chamada toda vez que uma informa????o de um dos t??picos subescritos chega.
void MQTT_CallBack(char* topic, byte* payload, unsigned int length) {

  // Ensure we don't overflow the payload buffer
  if (length > 0) {
    // Create a temporary buffer to avoid modifying the original payload
    char* tempPayload = new char[length + 1];
    memcpy(tempPayload, payload, length);
    tempPayload[length] = '\0';
    
    String data = "";

    if (strstr(topic, sub0)) {
    for (unsigned int i = 0; i < length; i++) {
      data += (char)tempPayload[i];
    }

    if (tempPayload[0] == '0') {

      digitalWrite(RelayPin1, HIGH);  // <-- Desliga o rel??. Note que HIGH ?? o n??vel de tens??o.
      digitalWrite(RelayPin2, HIGH);  // <-- Desliga o rel??. Note que HIGH ?? o n??vel de tens??o.
      digitalWrite(RelayPin3, HIGH);  // <-- Desliga o rel??. Note que HIGH ?? o n??vel de tens??o.
      digitalWrite(RelayPin4, HIGH);  // <-- Desliga o rel??. Note que HIGH ?? o n??vel de tens??o.
      digitalWrite(RelayPin5, HIGH);  // <-- Desliga o rel??. Note que HIGH ?? o n??vel de tens??o.
      digitalWrite(RelayPin6, HIGH);  // <-- Desliga o rel??. Note que HIGH ?? o n??vel de tens??o.
      digitalWrite(RelayPin7, HIGH);  // <-- Desliga o rel??. Note que HIGH ?? o n??vel de tens??o.
      digitalWrite(RelayPin8, HIGH);  // <-- Desliga o rel??. Note que HIGH ?? o n??vel de tens??o.
      status_todos = 0;
      toggleState_0 = 0;
      MQTT.publish(pub0, "0");
    } else {
      digitalWrite(RelayPin1, LOW);  // <-- Liga o Rel?? tornando a tens??o BAIXA
      digitalWrite(RelayPin2, LOW);  // <-- Liga o Rel?? tornando a tens??o BAIXA
      digitalWrite(RelayPin3, LOW);  // <-- Liga o Rel?? tornando a tens??o BAIXA
      digitalWrite(RelayPin4, LOW);  // <-- Liga o Rel?? tornando a tens??o BAIXA
      digitalWrite(RelayPin5, LOW);  // <-- Liga o Rel?? tornando a tens??o BAIXA
      digitalWrite(RelayPin6, LOW);  // <-- Liga o Rel?? tornando a tens??o BAIXA
      digitalWrite(RelayPin7, LOW);  // <-- Liga o Rel?? tornando a tens??o BAIXA
      digitalWrite(RelayPin8, LOW);  // <-- Liga o Rel?? tornando a tens??o BAIXA
      status_todos = 1;
      toggleState_0 = 1;
      MQTT.publish(pub0, "1");
    }
  }
  if (strstr(topic, sub1)) {
    for (unsigned int i = 0; i < length; i++) {
      data += (char)tempPayload[i];
    }

    if (tempPayload[0] == '0') {
      digitalWrite(RelayPin1, HIGH);  // <-- Desliga o rel??. Note que HIGH ?? o n??vel de tens??o.
      toggleState_1 = 0;
      MQTT.publish(pub1, "0", true);
    } else {
      digitalWrite(RelayPin1, LOW);  // <-- Liga o Rel?? tornando a tens??o BAIXA
      toggleState_1 = 1;
      MQTT.publish(pub1, "1", true);
    }
  }
  if (strstr(topic, sub2)) {
    for (unsigned int i = 0; i < length; i++) {
      data += (char)tempPayload[i];
    }

    if (tempPayload[0] == '0') {
      digitalWrite(RelayPin2, HIGH);  // <-- Desliga o rel??. Note que HIGH ?? o n??vel de tens??o.
      toggleState_2 = 0;
      MQTT.publish(pub2, "0", true);
    } else {
      digitalWrite(RelayPin2, LOW);  // <-- Liga o Rel?? tornando a tens??o BAIXA
      toggleState_2 = 1;
      MQTT.publish(pub2, "1", true);
    }
  }
  if (strstr(topic, sub3)) {
    for (unsigned int i = 0; i < length; i++) {
      data += (char)tempPayload[i];
    }

    if (tempPayload[0] == '0') {
      digitalWrite(RelayPin3, HIGH);  // <-- Desliga o rel??. Note que HIGH ?? o n??vel de tens??o.
      toggleState_3 = 0;
      MQTT.publish(pub3, "0", true);
    } else {
      digitalWrite(RelayPin3, LOW);  // <-- Liga o Rel?? tornando a tens??o BAIXA
      toggleState_3 = 1;
      MQTT.publish(pub3, "1", true);
    }
  }
  if (strstr(topic, sub4)) {
    for (unsigned int i = 0; i < length; i++) {
      data += (char)tempPayload[i];
    }

    if (tempPayload[0] == '0') {
      digitalWrite(RelayPin4, HIGH);  // <-- Desliga o rel??. Note que HIGH ?? o n??vel de tens??o.
      toggleState_4 = 0;
      MQTT.publish(pub4, "0", true);
    } else {
      digitalWrite(RelayPin4, LOW);  // <-- Liga o Rel?? tornando a tens??o BAIXA
      toggleState_4 = 1;
      MQTT.publish(pub4, "1", true);
    }
  }
  if (strstr(topic, sub5)) {
    for (unsigned int i = 0; i < length; i++) {
      data += (char)tempPayload[i];
    }

    if (tempPayload[0] == '0') {
      digitalWrite(RelayPin5, HIGH);  // <-- Desliga o rel??. Note que HIGH ?? o n??vel de tens??o.
      toggleState_5 = 0;
      MQTT.publish(pub5, "0", true);
    } else {
      digitalWrite(RelayPin5, LOW);  // <-- Liga o Rel?? tornando a tens??o BAIXA
      toggleState_5 = 1;
      MQTT.publish(pub5, "1", true);
    }
  }
  if (strstr(topic, sub6)) {
    for (unsigned int i = 0; i < length; i++) {
      data += (char)tempPayload[i];
    }

    if (tempPayload[0] == '0') {
      digitalWrite(RelayPin6, HIGH);  // <-- Desliga o rel??. Note que HIGH ?? o n??vel de tens??o.
      toggleState_6 = 0;
      MQTT.publish(pub6, "0", true);
    } else {
      digitalWrite(RelayPin6, LOW);  // <-- Liga o Rel?? tornando a tens??o BAIXA
      toggleState_6 = 1;
      MQTT.publish(pub6, "1", true);
    }
  }
  if (strstr(topic, sub7)) {
    for (unsigned int i = 0; i < length; i++) {
      data += (char)tempPayload[i];
    }

    if (tempPayload[0] == '0') {
      digitalWrite(RelayPin7, HIGH);  // <-- Desliga o rel??. Note que HIGH ?? o n??vel de tens??o.
      toggleState_7 = 0;
      MQTT.publish(pub7, "0", true);
    } else {
      digitalWrite(RelayPin7, LOW);  // <-- Liga o Rel?? tornando a tens??o BAIXA
      toggleState_7 = 1;
      MQTT.publish(pub7, "1", true);
    }
  }
  if (strstr(topic, sub8)) {
    for (unsigned int i = 0; i < length; i++) {
      data += (char)tempPayload[i];
    }

    if (tempPayload[0] == '0') {
      digitalWrite(RelayPin8, HIGH);  // <-- Desliga o rel??. Note que HIGH ?? o n??vel de tens??o.
      toggleState_8 = 0;
      MQTT.publish(pub8, "0", true);
    } else {
      digitalWrite(RelayPin8, LOW);  // <-- Liga o Rel?? tornando a tens??o BAIXA
      toggleState_8 = 1;
      MQTT.publish(pub8, "1", true);
    }
  }
  
  // Clean up the temporary payload
  delete[] tempPayload;
  }
}

/* Fun????o: reconecta-se ao broker MQTT (caso ainda n??o esteja conectado ou em caso de a conex??o cair)
   em caso de sucesso na conex??o ou reconex??o, o subscribe dos t??picos ?? refeito.*/
void reconnectMQTT() {

  unsigned long currentTime = millis();
  unsigned long reconnectTime = 5000;  // <-- Tempo para tentar reconectar (em milissegundos)
  static unsigned long lastReconnectAttempt = 0; // <-- Armazena o ??ltimo tempo de tentativa

  // Verifica o overflow de millis
  if (currentTime < lastReconnectAttempt) {
    /* Overflow ocorreu
       L??gica para lidar com o overflow, se necess??rio
       Por exemplo, reiniciar o ??ltimo tempo para o valor atual */
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
        Serial.println(" Haver?? nova tentativa de conex??o em 5s");
      }
      lastReconnectAttempt = currentTime;  // <-- Atualiza o ??ltimo tempo de tentativa de reconex??o
    }
  } else {
    lastReconnectAttempt = currentTime;  // <-- Atualiza o ??ltimo tempo caso a conex??o seja bem-sucedida
  }
}

// Fun????o: Reconectar-se ao WiFi
void reconectWiFi() {
  /* Se j?? est?? conectado a rede WI-FI, nada ?? feito.
    Caso contr??rio, s??o efetuadas tentativas de conex??o */
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(ssid, password);  // <-- Conecta na rede WI-FI
  Serial.println("\nConectando WiFi " + String(ssid));

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Conex??o Falhou");
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
  Serial.print("Endere??o de IP: ");
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

/* Fun????o: Verifica o estado das conex??es WiFI e ao broker MQTT.
   Em caso de desconex??o (qualquer uma das duas), a conex??o  ?? refeita. */
void VerificaConexoesWiFIeMQTT() {

  if (!MQTT.connected()) {

    reconnectMQTT();  // <-- Se n??o h?? conex??o com o Broker, a conex??o ?? refeita

    reconectWiFi();  	  // <-- Se n??o h?? conex??o com o WiFI, a conex??o ?? refeita "apagar essa linha depois pra testar"
  }
}

// Fun????o: Para leitura dos sensores DHT22 e BMP180 e envio pro MQTT Broker
void SensoresMQTT(){
  // Sensor DHT22  - Temperatura e Umidade
  unsigned long currentTimeDHT = millis();

  if (currentTimeDHT - lastMsgDHT > 60000) {
    lastMsgDHT = currentTimeDHT;  // <-- Atualiza o ??ltimo tempo de execu????o

    // C??digo executado a cada 60000 milissegundos (1 minuto)
    float temp_data = dht.readTemperature();      // <-- Leitura da temperatura em graus Celsius (??C) pelo sensor DHT22
    dtostrf(temp_data, 6, 2, str_temp_data);      // <-- Convers??o do valor float de temperatura para string com 6 caracteres e 2 casas decimais

    float hum_data = dht.readHumidity();          // <-- Leitura da umidade relativa do ar (%)
    dtostrf(hum_data, 6, 2, str_hum_data);        // <-- Convers??o do valor float de umidade para string com 6 caracteres e 2 casas decimais

    float tempF_data = dht.readTemperature(true); // <-- Leitura da temperatura em graus Fahrenheit (??F)
    dtostrf(tempF_data, 6, 2, str_tempF_data);    // <-- Convers??o do valor float de temperatura em Fahrenheit para string com 6 caracteres e 2 casas decimais

    float tempterm_data = dht.computeHeatIndex(tempF_data, hum_data); // <-- C??lculo da sensa????o t??rmica (heat index) com base na temperatura em Fahrenheit e umidade
    tempterm_data = dht.convertFtoC(tempterm_data);   // <-- Convers??o da sensa????o t??rmica de Fahrenheit para Celsius
    dtostrf(tempterm_data, 6, 2, str_tempterm_data);  // <-- Convers??o do valor float de sensa????o t??rmica para string com 6 caracteres e 2 casas decimais

    // Publica os dados no MQTT
    MQTT.publish(pub9, str_temp_data);
    MQTT.publish(pub10, str_hum_data);
    MQTT.publish(pub11, str_tempterm_data);
  }

  // Sensor BMP180
  // Leitura da Temperatura, Altitude e Press??o Atmosf??rica
  unsigned long currentTimeBMP180 = millis();

  if (currentTimeBMP180 - lastMsgBMP180 > 120000) {
    lastMsgBMP180 = currentTimeBMP180;  // <-- Atualiza o ??ltimo tempo de execu????o

    // C??digo executado a cada 120000 milissegundos (2 minutos)
    float pressaoNivelMar = 1012;  // <-- Press??o ao n??vel do mar em hPa
    float altitudeNivelMar = 92;   // <-- Altitude da cidade em metros

    char buffer[10];  // <-- Buffer para armazenar a string convertida

    // Leitura da press??o atmosf??rica (hPa) a partir do sensor BMP180
    dtostrf(bmp.readPressure() / 100.0, 2, 2, buffer);  // <-- A press??o lida ?? convertida de Pascal para hectopascal (hPa) ao dividir por 100.0
    MQTT.publish(pub14, buffer);

    // Leitura da press??o ao n??vel do mar (calculada)
    dtostrf(bmp.readSealevelPressure(pressaoNivelMar) / 100.0, 2, 2, buffer); // <-- Utiliza o valor de press??o ao n??vel do mar definido (pressaoNivelMar) para o c??lculo
    MQTT.publish(pub15, buffer);

    float altitudeReal = bmp.readAltitude(pressaoNivelMar * 100); // <-- Leitura da altitude real (metros) com base na press??o lida e na press??o ao n??vel do mar
    dtostrf(altitudeReal, 2, 2, buffer);  // <-- Convers??o do valor da altitude para string com 2 casas decimais
    MQTT.publish(pub16, buffer);

    // C??lculo da altitude ajustada ao n??vel do mar, somando a altitude real com a altitude da cidade (altitudeNivelMar)
    float altitudeAoNivelDoMar = altitudeReal + altitudeNivelMar; // <-- Isso ajusta o valor da altitude com base na localiza????o espec??fica da cidade
    dtostrf(altitudeAoNivelDoMar, 2, 2, buffer);  // <-- Convers??o do valor da altitude ajustada para string com 2 casas decimais
    MQTT.publish(pub17, buffer);
  }
}

//Implementa????o das Fun????es Principais do Core1 do ESP32
void setup1() {
  // Initialize sensors
  dht.begin();  // Inicializa o sensor DHT22

  // Start do Sensor BMP180
  if (!bmp.begin()) {
    Serial.println("N??o foi poss??vel encontrar um sensor BMP180 v??lido, por favor, verifique a conex??o!");
    // Em vez de entrar em loop infinito, apenas pule a inicializa????o do sensor e continue a execu????o
  } else {
    Serial.println("Sensor BMP180 encontrado e inicializado com sucesso.");
  }
}

// loop do Core1 do ESP32
void loop1() {
  // Garante funcionamento das conex??es WiFi e ao Broker MQTT
  VerificaConexoesWiFIeMQTT();
  // Keep-Alive da comunica????o com Broker MQTT
  MQTT.loop();  // <-- Verifica se h?? novas mensagens no Broker MQTT
  // Garante a leitura dos sensores DHT22 e BMP180
  SensoresMQTT();
  
  // Small delay to prevent watchdog timeout
  delay(10);
} 
