// Tópicos do Subscribe

//Módulo Relé de 8 Canais
const char* sub0 = "ESP32/MinhaCasa/QuartoRobson/Ligar-DesligarTudo/Comando"; // Somente por MQTT
const char* sub1 = "ESP32/MinhaCasa/QuartoRobson/Interruptor1/Comando";       // Ligados ao Nora/MQTT
const char* sub2 = "ESP32/MinhaCasa/QuartoRobson/Interruptor2/Comando";       // Ligados ao Nora/MQTT
const char* sub3 = "ESP32/MinhaCasa/QuartoRobson/Interruptor3/Comando";       // Ligados ao Nora/MQTT
const char* sub4 = "ESP32/MinhaCasa/QuartoRobson/Interruptor4/Comando";       // Ligados ao Nora/MQTT
const char* sub5 = "ESP32/MinhaCasa/QuartoRobson/Interruptor5/Comando";       // Ligados ao Nora/MQTT
const char* sub6 = "ESP32/MinhaCasa/QuartoRobson/Interruptor6/Comando";       // Ligados ao MQTT/Alexa
const char* sub7 = "ESP32/MinhaCasa/QuartoRobson/Interruptor7/Comando";       // Ligados ao MQTT/Alexa
const char* sub8 = "ESP32/MinhaCasa/QuartoRobson/Interruptor8/Comando";       // Ligados ao MQTT/Alexa

//Sensor DHT22 (Temperatura e Umidade)
const char* sub9  = "ESP32/MinhaCasa/QuartoRobson/Temperatura";               // Somente por MQTT
const char* sub10 = "ESP32/MinhaCasa/QuartoRobson/Umidade";                   // Somente por MQTT
const char* sub11 = "ESP32/MinhaCasa/QuartoRobson/SensacaoTermica";           // Somente por MQTT

//Sensor de Movimento PIR
const char* sub12 = "ESP32/MinhaCasa/QuartoRobson/SensorMovimento";           // Somente por MQTT

//Sensor BMP180 (Temperatura, Pressão Atmosférica e Altitude)
const char* sub13 = "ESP32/MinhaCasa/QuartoRobson/BMP180/Temperatura";                  // Somente por MQTT
const char* sub14 = "ESP32/MinhaCasa/QuartoRobson/BMP180/PressaoAtmosferica/Real";  // Somente por MQTT
const char* sub15 = "ESP32/MinhaCasa/QuartoRobson/BMP180/PressaoAtmosferica/NivelMar";  // Somente por MQTT
const char* sub16 = "ESP32/MinhaCasa/QuartoRobson/BMP180/AltitudeReal";                     // Somente por MQTT
const char* sub17 = "ESP32/MinhaCasa/QuartoRobson/BMP180/AltitudeNivelMar";                 // Somente por MQTT

// Tópicos do Publish

//Módulo Relé de 8 Canais
const char* pub0 = "ESP32/MinhaCasa/QuartoRobson/Ligar-DesligarTudo/Estado";  // Somente por MQTT
const char* pub1 = "ESP32/MinhaCasa/QuartoRobson/Interruptor1/Estado";        // Ligados ao Nora/MQTT
const char* pub2 = "ESP32/MinhaCasa/QuartoRobson/Interruptor2/Estado";        // Ligados ao Nora/MQTT
const char* pub3 = "ESP32/MinhaCasa/QuartoRobson/Interruptor3/Estado";        // Ligados ao Nora/MQTT
const char* pub4 = "ESP32/MinhaCasa/QuartoRobson/Interruptor4/Estado";        // Ligados ao Nora/MQTT
const char* pub5 = "ESP32/MinhaCasa/QuartoRobson/Interruptor5/Estado";        // Ligados ao Nora/MQTT
const char* pub6 = "ESP32/MinhaCasa/QuartoRobson/Interruptor6/Estado";        // Ligados ao MQTT/Alexa
const char* pub7 = "ESP32/MinhaCasa/QuartoRobson/Interruptor7/Estado";        // Ligados ao MQTT/Alexa
const char* pub8 = "ESP32/MinhaCasa/QuartoRobson/Interruptor8/Estado";        // Ligados ao MQTT/Alexa

//Sensor DHT22 (Temperatura e Umidade)
const char* pub9  = "ESP32/MinhaCasa/QuartoRobson/Temperatura";               // Somente por MQTT
const char* pub10 = "ESP32/MinhaCasa/QuartoRobson/Umidade";                   // Somente por MQTT
const char* pub11 = "ESP32/MinhaCasa/QuartoRobson/SensacaoTermica";           // Somente por MQTT

//Sensor de Movimento PIR
const char* pub12 = "ESP32/MinhaCasa/QuartoRobson/SensorMovimento";           // Somente por MQTT

//Sensor BMP180 (Temperatura, Pressão Atmosférica e Altitude)
const char* pub13 = "ESP32/MinhaCasa/QuartoRobson/BMP180/Temperatura";                  // Somente por MQTT
const char* pub14 = "ESP32/MinhaCasa/QuartoRobson/BMP180/PressaoAtmosferica/Real";  // Somente por MQTT
const char* pub15 = "ESP32/MinhaCasa/QuartoRobson/BMP180/PressaoAtmosferica/NivelMar";  // Somente por MQTT
const char* pub16 = "ESP32/MinhaCasa/QuartoRobson/BMP180/AltitudeReal";                     // Somente por MQTT
const char* pub17 = "ESP32/MinhaCasa/QuartoRobson/BMP180/AltitudeNivelMar";                 // Somente por MQTT
