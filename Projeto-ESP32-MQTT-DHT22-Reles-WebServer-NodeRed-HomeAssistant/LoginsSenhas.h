// Configurações do WIFI
const char* ssid     =  "IoT";                // SSID nome da rede WI-FI que deseja se conectar
const char* password =  "@IoT@S3nh@S3gur@";   // Senha da rede WI-FI que deseja se conectar

// Configurações do Broker MQTT
const char* BrokerMQTT   = "192.168.15.30";  // URL do broker MQTT que se deseja utilizar
const char* LoginDoMQTT  = "RobsonBrasil";   // MQTT UserName
const char* SenhaMQTT    = "loboalfa";       // MQTT Password

// Configurações do Broker MQTT 2
//const char* BrokerMQTT2  = "192.168.15.70";  // URL do broker MQTT que se deseja utilizar
//const char* LoginDoMQTT2 = "RobsonBrasil"; // MQTT UserName
//const char* SenhaMQTT2   = "loboalfa";          // MQTT Password

// WebServer
const char* LoginDoHTTP = "RobsonBrasil";   // Login da página do WebServer
const char* SenhaDoHTTP = "loboalfa";       // Senha da página do WebServer

// Host Name do ESP32 http://esp32.iot.local
const char* hostname = "esp32-iot";

const char* firmwareUpdatePassword = "loboalfa";
