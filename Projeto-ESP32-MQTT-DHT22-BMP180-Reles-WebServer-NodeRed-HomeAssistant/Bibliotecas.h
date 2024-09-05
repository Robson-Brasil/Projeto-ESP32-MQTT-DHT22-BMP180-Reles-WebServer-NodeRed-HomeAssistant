// Todas as bibliotecas baixei direto do IDE do Arduino, caso você não encontre, disponibilizei o link dos repositórios oficiais de cada uma!
// Bibliotecas
#include <WiFi.h>               // Função: Fornece suporte para comunicação Wi-Fi em projetos Arduino. Permite a conexão a redes Wi-Fi e a criação de servidores ou clientes.
// Link: https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi

#include <PubSubClient.h>       // Função: Biblioteca para implementação do protocolo MQTT em projetos Arduino. É comumente utilizada para comunicação entre dispositivos IoT.
// Link: https://github.com/knolleary/pubsubclient

#include <DHT.h>                // Função: Utilizada para trabalhar com sensores de temperatura e umidade da série DHT (por exemplo, DHT11, DHT22).
// Link: https://github.com/adafruit/DHT-sensor-library

#include <WiFiUdp.h>            // Função: Fornece funcionalidades relacionadas a comunicação UDP (User Datagram Protocol) sobre Wi-Fi.
// Link: https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi/src

#include <Arduino.h>            // Função: Cabeçalho padrão do Arduino, contém definições essenciais e macros necessárias para o desenvolvimento em Arduino.
// Link: https://github.com/arduino/ArduinoCore-avr

#include <DNSServer.h>          // Função: Implementa um servidor DNS que pode ser usado em conjunto com a biblioteca ESPAsyncWebServer para criar servidores web.
// Link: https://github.com/zhouhan0126/DNSServer---esp32

#include <AsyncTCP.h>           // Função: Biblioteca para comunicação TCP assíncrona, usada principalmente em conjunto com o ESPAsyncWebServer.
// Link: https://github.com/me-no-dev/AsyncTCP

#include <ESPAsyncWebServer.h>  // Função: Permite a criação de servidores web assíncronos no ESP8266/ESP32.
// Link: https://github.com/me-no-dev/ESPAsyncWebServer

#include <SPIFFS.h>             // Função: Fornece acesso ao sistema de arquivos SPIFFS (SPI Flash File System), que permite armazenar e ler arquivos no flash da placa.
// Link: https://github.com/espressif/arduino-esp32/tree/master/libraries/SPIFFS

#include <TimeLib.h>            // Função: Manipulação de tempo e data em projetos Arduino.
// Link: https://github.com/PaulStoffregen/Time

#include <freertos/FreeRTOS.h>  // Função: Inclui a biblioteca FreeRTOS, um sistema operacional de tempo real para microcontroladores.
// Link: https://github.com/espressif/esp-idf/tree/master/components/freertos

#include <freertos/task.h>      // Função: Contém definições para tarefas do FreeRTOS.
// Link: https://github.com/espressif/esp-idf/tree/master/components/freertos

#include <EEPROM.h>             // Função: Permite a leitura e escrita de dados na EEPROM da placa Arduino.
// Link: https://github.com/espressif/arduino-esp32/tree/master/libraries/EEPROM

#include <Wire.h>               // Função: Permite a comunicação I2C entre dispositivos.
// Link: https://github.com/espressif/arduino-esp32/tree/master/libraries/Wire

#include <Adafruit_Sensor.h>    // Função: Biblioteca base para sensores da Adafruit.
// Link: https://github.com/adafruit/Adafruit_Sensor

#include <Adafruit_BMP085.h>    // Função: Biblioteca para interagir com o sensor de pressão atmosférica BMP180.
// Link: https://github.com/adafruit/Adafruit-BMP085-Library

#include <ESPmDNS.h>            // Função: Implementa o suporte mDNS no ESP8266/ESP32, facilitando a descoberta de dispositivos na rede local através de um nome amigável.
// Link: https://github.com/espressif/arduino-esp32/tree/master/libraries/ESPmDNS

#include <ArduinoOTA.h>         // Função: Facilita a implementação de atualizações de firmware OTA em projetos Arduino.
// Link: https://github.com/espressif/arduino-esp32/tree/master/libraries/ArduinoOTA

#include <Update.h>             // Função: Permite a atualização de firmware de maneira personalizada em placas compatíveis com o Arduino.
// Link: https://github.com/espressif/arduino-esp32/tree/master/libraries/Update

#include <NetworkUdp.h>         // Função: Proporciona a comunicação UDP sobre Wi-Fi.
// Link: https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi/src/WiFiUdp.cpp
