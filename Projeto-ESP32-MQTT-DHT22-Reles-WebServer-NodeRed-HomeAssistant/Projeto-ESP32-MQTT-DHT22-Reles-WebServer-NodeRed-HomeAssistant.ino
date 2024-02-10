/**********************************************************************************
  IoT - Automação Residencial
  Autor : Robson Brasil
  
  Dispositivos : ESP32 WROOM32, DHT22, BMP180, Módulo Relé de 8 Canais e Sensor PIR (Sensor de Movimento)
  Preferences--> URLs adicionais do Gerenciador de placas:
                                    ESP8266: http://arduino.esp8266.com/stable/package_esp8266com_index.json,
                                    ESP32  : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  Download Board ESP32 (x.x.x):
  OTA e OTA WebServer
  Broker MQTT
  Node-Red / Google Assistant-Nora:  https://smart-nora.eu/
  Para Instalação do Node-Red:       https://nodered.org/docs/getting-started/
  Home Assistant
  Para Instalação do Home Assistant: https://www.home-assistant.io/installation/
  Versão : 17 - Beta Tester
  Última Modificação : 10/02/2024
**********************************************************************************/

//Bibliotecas
#include "LoginsSenhas.h"
#include "TopicosMQTT.h"
#include "Bibliotecas.h"
#include "GPIOs.h"
#include "WebServerOTA.h"
#include "Sensores.h"
#include "MQTT.h"
#include "VariaveisGlobais.h"

void setup1();  // Declaração da função setup1()
void loop1();   // Declaração da função loop1()

// Configuração das funções dos botões da página WebServer
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

const int numButtons = 8;
bool buttonStates[numButtons];

String processor(const String& var) {
  //Serial.println(var);
  if (var == "BUTTONPLACEHOLDER1") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 1</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"26\" " + outputState(26) + "><span class=\"slider\"></span></label>";
    return buttons;
  }

  if (var == "BUTTONPLACEHOLDER2") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 2</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"13\" " + outputState(13) + "><span class=\"slider\"></span></label>";
    return buttons;
  }

  if (var == "BUTTONPLACEHOLDER3") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 3</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"14\" " + outputState(14) + "><span class=\"slider\"></span></label>";
    return buttons;
  }

  if (var == "BUTTONPLACEHOLDER4") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 4</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"32\" " + outputState(32) + "><span class=\"slider\"></span></label>";
    return buttons;
  }

  if (var == "BUTTONPLACEHOLDER5") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 5</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"16\" " + outputState(16) + "><span class=\"slider\"></span></label>";
    return buttons;
  }

  if (var == "BUTTONPLACEHOLDER6") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 6</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"17\" " + outputState(17) + "><span class=\"slider\"></span></label>";
    return buttons;
  }

  if (var == "BUTTONPLACEHOLDER7") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 7</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"18\" " + outputState(18) + "><span class=\"slider\"></span></label>";
    return buttons;
  }

  if (var == "BUTTONPLACEHOLDER8") {
    String buttons = "";
    buttons += "<div id=\"buttonContainer\"></div>";
    buttons += "<h4>Interruptor 8</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"19\" " + outputState(19) + "><span class=\"slider\"></span></label>";
    return buttons;
  }

  return String();
}

// Função para processar a mudança de estado do botão e atualizar no mapa
void toggleCheckbox(int checkboxId, bool checkboxState) {
  // Atualiza o estado no array
  buttonStates[checkboxId] = checkboxState;
}

String outputState(int buttonId) {
  if (buttonStates[buttonId]) {
    return "checked";
  } else {
    return "";
  }
}

#define MSG_BUFFER_SIZE (1000)

//Função MILLIS
unsigned long lastMsgDHT = 0;
unsigned long lastMsgBMP180 = 0;
unsigned long lastMsgMQTT = 0;
unsigned long lastMsgPIR = 0;
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
void initESPmDNS();
void initSPIFFS();

// Função: Inicializa o output em nível lógico baixo
void initOutput() {

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

  MQTT.setServer(BrokerMQTT, PortaBroker);    // Informa qual broker e porta deve ser conectado
  MQTT.setCallback(MQTT_CallBack);            // Atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
  MQTT.setKeepAlive(MQTT_KeepAlive);          // Defina o keep-alive
}

//Função: Inicializa o EPmDNS para usar o Hostname
void initESPmDNS() {
  // Configuração do mDNS
  if (MDNS.begin(hostname)) {
    Serial.println("mDNS iniciado");
    Serial.println("");
  } else {
    Serial.println("Erro ao iniciar o mDNS");
    Serial.println("");
  }
}

//Função: Inicializa o SPIFFS
void initSPIFFS() {

  if (!SPIFFS.begin(true)) {
    Serial.println("Ocorreu um erro ao montar o SPIFFS");
    return;
  }

  // Rota para main.html
  server.on("/principal", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/main.html", "text/html");
  });

  // Rota para access.html
  server.on("/access", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/access.html", "text/html");
  });

  // Rota para upload.html
  server.on("/upload", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/upload.html", "text/html");
  });

  // Rota para o processamento do formulário de upload
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    response->addHeader("Connection", "close");
    request->send(response);
  }, [](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index) {
      Serial.printf("Update: %s\n", filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
        Update.printError(Serial);
      }
    }
    if (Update.write(data, len) != len) {
      Update.printError(Serial);
    }
    if (final) {
      if (Update.end(true)) {
        Serial.printf("Update Success: %u\nRebooting...\n", index + len);
      } else {
        Update.printError(Serial);
      }
    }

    // Inicia a atualização OTA
    ArduinoOTA.begin();

    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  });

  // Rota para avatar.png
  server.on("/avatar", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/avatar.png", "image/png");
  });

  // Rota para style.css
  server.on("/styleota", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/styleota.css", "text/css");
  });

  /*OBSERVAÇÃO: Se estiver atualizando o SPIFFS, este seria o local para desmontar o SPIFFS usando SPIFFS.end()*/
  server.on("/ota", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/html", loginIndex);
  });

  server.on("/serverIndex", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/html", serverIndex);
  });

  server.on("/update", HTTP_POST, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, [](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index) {
      Serial.printf("Update: %s\n", filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
        Update.printError(Serial);
      }
    }
    if (Update.write(data, len) != len) {
      Update.printError(Serial);
    }
    if (final) {
      if (Update.end(true)) {
        Serial.printf("Update Success: %u\nRebooting...\n", index + len);
      } else {
        Update.printError(Serial);
      }
    }

    // Inicia a atualização OTA
    ArduinoOTA.begin();

    Serial.println("Ready");
    Serial.print("Endereço de IP: ");
    Serial.println(WiFi.localIP());

    request->send(200, "text/plain", "Atualização OTA iniciada. Isso pode levar alguns minutos. Acesse o monitor serial para obter mais informações.");
  });

  // Rota para o WebServer.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(LoginDoHTTP, SenhaDoHTTP))
      return request->requestAuthentication();
    request->send(SPIFFS, "/WebServer.html", String(), false, processor);
  });

  server.serveStatic("/", SPIFFS, "/");

  // Rota para o CSS
  server.on("/WebServer.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/WebServer.css", "text/css");
  });

  // Rota para o JavaScript
  server.on("/darkmode.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/darkmode.js", "application/javascript");
  });

  // Rota para o DarkMode
  server.on("/darkmode.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/darkmode.png", "image/png");
  });

  // Rota para o LightMode
  server.on("/lightmode.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/lightmode.png", "image/png");
  });

  // Rota para o Logo
  server.on("/logo-1.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/logo-1.png", "image/png");
  });

  // Rota para o ações dos botões do WebServer
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
    Serial.print("GPIO: ");
    Serial.print(inputMessage1);
    Serial.print(" - Comando Ligar - Desligar: ");
    Serial.println(inputMessage2);
    request->send(200, "text/plain", "OK");
  });

  File file = SPIFFS.open("/WebServer.txt", "r");
  if (!file) {
    Serial.println("Falha ao abrir o arquivo WebServer.txt");
    return;
  }

  String code = file.readString();
  file.close();

  // Executar o código lido do arquivo
  if (code.length() > 0) {
    Serial.println("SPIFF : Executando código do arquivo WebServer.txt");
    Serial.println("");
  } else {
    Serial.println("O arquivo WebServer.txt está vazio");
  }
}

//Implementação das Funções Principais do Core0 do ESP32
void setup() {
  // Inicializações:
  initOutput();
  initSerial();
  initWiFi();
  initMQTT();
  initESPmDNS();
  initSPIFFS();

  //Chama a função setup1()
  setup1();

  // Escolha qual porta usar no seu ESP32
  ArduinoOTA.setPort(3232);

  // Esolha qual o Hostname usar para esp32-[MAC]
  ArduinoOTA.setHostname("ESP32-IoT");

  // Senha para autenticar a atulização OTA (Se não tiver, qualquer um pode autlizar sem sua permissão)
  ArduinoOTA.setPassword("loboalfa");

  // A senha pode ser definida com o seu valor MD5 também.(Senha mais avançada)
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  // Inicio da configuração do Servidor OTA
  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    Serial.println("Iniciando a atualização. " + type);
  })
  .onEnd([]() {
    Serial.println("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progresso: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Erro[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Autenticação Falhou");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Início falhou.");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Conexão Falhou");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Recepção falhou");
    else if (error == OTA_END_ERROR) Serial.println("Encerramento falhou.");
  });

  // Start do Servidor OTA
  ArduinoOTA.begin();

  // Start do Servidor WebServer
  server.begin();

  // Inicialize os estados dos botões como falso (desligado) ao iniciar o programa
  for (int i = 0; i < numButtons; ++i) {
    buttonStates[i] = false;
  }
  // Start do Sensor BMP180
  if (!bmp.begin()) {
    Serial.println("Não foi possível encontrar um sensor BMP180 válido, por favor, verifique a conexão!");
    while (1) {}
  }
}

// Programa Principal do Core0 do ESP32
void loop() {
  // Garante funcionamento das conexões WiFi e ao Broker MQTT
  VerificaConexoesWiFIeMQTT();
  //Keep-Alive da comunicação com Broker MQTT
  MQTT.loop();

  loop1();

  // Lida com atualizações OTA
  ArduinoOTA.handle();

  unsigned long currentTimeMQTT = millis();

  // Verifica o overflow de millis
  if (currentTimeMQTT < lastMsgMQTT) {
    // Overflow ocorreu
    // Lógica para lidar com o overflow, se necessário
    // Por exemplo, reiniciar o último tempo para o valor atual
    lastMsgMQTT = currentTimeMQTT;
  } else {
    if (currentTimeMQTT - lastMsgMQTT > 100) {
      lastMsgMQTT = currentTimeMQTT;

      // Código executado a cada 100 milissegundos

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
      /*if (digitalRead(RelayPin1) == HIGH) {  // Liga o relé sem a necessidade de configuração, seja no, Node Red ou HomeAssistant
        MQTT.publish(pub12, "Sem Movimento");
        } else {
        MQTT.publish(pub12, "Movimento Detectado");
        }*/
      if (status_todos == 1) {
        MQTT.publish(pub0, "1", true);
      } else {
        MQTT.publish(pub0, "0", true);
      }
    }
  }
}

// Função: Inicializa o callback, esta função é chamada toda vez que uma informação de um dos tópicos subescritos chega.
void MQTT_CallBack(char* topic, byte* payload, unsigned int length) {

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
  // Função para acionamento das ações do Sensor PIR via MQTT
  /*if (strstr(topic, sub12)) {
    for (unsigned int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      data += (char)payload[i];
    }
    Serial.println();

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin1, HIGH);  // Ligua o relé. Note que HIGH é o nível de tensão.
      toggleState_9 = 0;
      MQTT.publish(pub12, "Sem Movimento");
    }
    else {
      digitalWrite(RelayPin1, LOW);  // Desligua o Relé tornando a tensão BAIXA
      toggleState_9 = 1;
      MQTT.publish(pub12, "Movimento Detectado");
    }
    }*/
  Serial.println("");
}

/* Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
   em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.*/
void reconnectMQTT() {

  unsigned long currentTime = millis();
  unsigned long reconnectTime = 2000;  // Tempo para tentar reconectar (em milissegundos)

  // Verifica o overflow de millis
  if (currentTime < delayTime) {
    // Overflow ocorreu
    // Lógica para lidar com o overflow, se necessário
    // Por exemplo, reiniciar o último tempo para o valor atual
    delayTime = currentTime;
  } else {
    delayTime = currentTime;

    while (!MQTT.connected()) {
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
        //MQTT.subscribe(sub9);
        //MQTT.subscribe(sub10);
        //MQTT.subscribe(sub11);
        //MQTT.subscribe(sub12);
        //MQTT.subscribe(sub13);
        //MQTT.subscribe(sub14);
        //MQTT.subscribe(sub15);
        //MQTT.subscribe(sub16);
        //MQTT.subscribe(sub17);
      } else {
        Serial.println("Falha ao reconectar no broker.");
        Serial.print(MQTT.state());
        // Verifica o overflow de millis para o próximo tempo de reconexão
        if (millis() - currentTime > reconnectTime) {
          Serial.println("Haverá nova tentativa de conexão em 2s");
          // Reinicia o último tempo para o valor atual
          currentTime = millis();
        }
      }
    }
  }
}

// Função: Reconectar-se ao WiFi
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
  Serial.println("");
}

/* Função: Verifica o estado das conexões WiFI e ao broker MQTT.
  Em caso de desconexão (qualquer uma das duas), a conexão  é refeita.*/
void VerificaConexoesWiFIeMQTT() {

  if (!MQTT.connected())

    reconnectMQTT();  // se não há conexão com o Broker, a conexão é refeita

    reconectWiFi();  // se não há conexão com o WiFI, a conexão é refeita "apagar essa linha depois pra testar"
}

//Implementação das Funções Principais do Core1 do ESP32
void setup1() {

  initMQTT();

  dht.begin();  // inicializa o sensor DHT11
}

// Implementação do Programa Principal no Loop do Core1 do ESP32
void loop1() {
  // Garante funcionamento das conexões WiFi e ao Broker MQTT
  VerificaConexoesWiFIeMQTT();
  // Keep-Alive da comunicação com Broker MQTT
  MQTT.loop();  // Verifica se há novas mensagens no Broker MQTT

  //Sensor DHT11  - Temperatua e Umidade  unsigned long currentTimeDHT = millis();
  unsigned long currentTimeDHT = millis();

  if (currentTimeDHT - lastMsgDHT > 60000) {

    // Verifica o overflow de millis
    if (currentTimeDHT < lastMsgDHT) {
      // Overflow ocorreu
      // Lógica para lidar com o overflow, se necessário
      // Por exemplo, reiniciar o último tempo para o valor atual
      lastMsgDHT = currentTimeDHT;
    } else {
      lastMsgDHT = currentTimeDHT;

      // Código executado a cada 60000 milissegundos (1 minuto)

      float temp_data = dht.readTemperature();
      dtostrf(temp_data, 6, 2, str_temp_data);

      float hum_data = dht.readHumidity();
      dtostrf(hum_data, 6, 2, str_hum_data);

      float tempF_data = dht.readTemperature(true);
      dtostrf(tempF_data, 6, 2, str_tempF_data);

      float tempterm_data = dht.computeHeatIndex(tempF_data, hum_data);
      tempterm_data = dht.convertFtoC(tempterm_data);
      dtostrf(tempterm_data, 6, 2, str_tempterm_data);

      // Publica os dados no MQTT
      MQTT.publish(pub9, str_temp_data);
      MQTT.publish(pub10, str_hum_data);
      MQTT.publish(pub11, str_tempterm_data);
    }
  }

  //Sensor PIR - Detector de Presença
  unsigned long currentTimePIR = millis();
  unsigned long motionDetectedTime = 0;
  unsigned long relayDuration = 100;  // Tempo de duração do relé em milissegundos (5 segundos)

  if (currentTimePIR - lastMsgPIR > 100) {

    // Verifica o overflow de millis
    if (currentTimePIR < lastMsgPIR) {
      // Overflow ocorreu
      // Lógica para lidar com o overflow, se necessário
      // Por exemplo, reiniciar o último tempo para o valor atual
      lastMsgPIR = currentTimePIR;
    } else {
      lastMsgPIR = currentTimePIR;

      val = digitalRead(SensorPIR);
      if (val == LOW) {
        // Serial.println("Sem Movimento");
        MQTT.publish(pub12, "Sem Movimento");
        digitalWrite(RelayPin1, HIGH);  // Desliga o relé
      } else {
        MQTT.publish(pub12, "Movimento Detectado");
        //Serial.println("Movimento Detectado");
        MQTT.publish(pub12, "0");        // Publica mensagem MQTT indicando que o relé foi ligado
        motionDetectedTime = currentTimePIR;  // Armazena o momento em que o movimento foi detectado
        digitalWrite(RelayPin1, LOW);         // Liga o relé
      }
    }
  }

  if (motionDetectedTime > 0 && millis() - motionDetectedTime > relayDuration) {
    motionDetectedTime = 0;
    digitalWrite(RelayPin1, HIGH);  // Desliga o relé após o tempo de duração definido
  }

  // Sensor SMP180
  // Leitura da temperatura
  unsigned long currentTimeBMP180 = millis();

  if (currentTimeBMP180 - lastMsgBMP180 > 120000) {

    // Verifica o overflow de millis
    if (currentTimeBMP180 < lastMsgBMP180) {
      // Overflow ocorreu
      // Lógica para lidar com o overflow, se necessário
      // Por exemplo, reiniciar o último tempo para o valor atual
      lastMsgBMP180 = currentTimeBMP180;
    } else {
      lastMsgBMP180 = currentTimeBMP180;

      // Código executado a cada 120000 milissegundos (2 minutos)

      float pressaoNivelMar = 1013.0;  // Pressão ao nível do mar

      Serial.print("Temperatura do Sensor BMP180 = ");
      Serial.print(bmp.readTemperature());
      Serial.println(" *C");

      Serial.print("Pressão Real = ");
      Serial.print(bmp.readPressure() / 100.0, 2);  // Convertendo para milibares e usando 2 casas decimais
      Serial.println(" mb");

      Serial.print("Pressão ao Nível do Mar = ");
      Serial.print(bmp.readSealevelPressure() / 100.0);  // Convertendo para milibares e usando 2 casas decimais
      Serial.println(" mb");

      /* Calcule a altitude assumindo uma pressão barométrica 'padrão'
         de 1013,25 milibares = 101325 pascals.
         Você pode obter uma medida mais precisa da altitude
         se souber a pressão ao nível do mar atual, que
         muda com o clima e outros fatores. Se for 1015 milibares
         isso é equivalente a 101500 pascals.  */

      Serial.print("Altitude Real = ");
      Serial.print(bmp.readAltitude());
      Serial.println(" metros");

      Serial.print("Altitude ao Nível do Mar = ");
      Serial.print(bmp.readAltitude(1013.0 * 100));
      Serial.println(" metros");

      Serial.println();

      Serial.println();

      char buffer[10];  // Buffer para armazenar a string convertida

      dtostrf(bmp.readPressure() / 100.0, 2, 2, buffer);  //Pressão Real
      MQTT.publish(pub14, buffer);

      dtostrf(bmp.readSealevelPressure(pressaoNivelMar) / 100.0, 2, 2, buffer);  //Pressão ao nível do mar (calculada)
      MQTT.publish(pub15, buffer);

      dtostrf(bmp.readAltitude(), 2, 2, buffer);  //Altitude Real
      MQTT.publish(pub16, buffer);

      dtostrf(bmp.readAltitude(1013.0 * 100), 2, 2, buffer);  //Altitude ao Nível do Mar
      MQTT.publish(pub17, buffer);
    }
  }
}
