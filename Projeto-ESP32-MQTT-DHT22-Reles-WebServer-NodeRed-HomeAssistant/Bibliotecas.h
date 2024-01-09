// Bibliotecas
#include <WiFi.h>               // Função: Fornece suporte para comunicação Wi-Fi em projetos Arduino. Permite a conexão a redes Wi-Fi e a criação de servidores ou clientes.
#include <PubSubClient.h>       // Função: Biblioteca para implementação do protocolo MQTT em projetos Arduino. É comumente utilizada para comunicação entre dispositivos IoT.
#include <DHT.h>                // Função: Utilizada para trabalhar com sensores de temperatura e umidade da série DHT (por exemplo, DHT11, DHT22).
#include <WiFiUdp.h>            // Função: Fornece funcionalidades relacionadas a comunicação UDP (User Datagram Protocol) sobre Wi-Fi.
#include <Arduino.h>            // Função: Cabeçalho padrão do Arduino, contém definições essenciais e macros necessárias para o desenvolvimento em Arduino.
#include <DNSServer.h>          // Função: Implementa um servidor DNS que pode ser usado em conjunto com a biblioteca ESPAsyncWebServer para criar servidores web.   https://github.com/zhouhan0126/DNSServer---esp32
#include <AsyncTCP.h>           // Função: Biblioteca para comunicação TCP assíncrona, usada principalmente em conjunto com o ESPAsyncWebServer.
#include <ESPAsyncWebServer.h>  // Função: Permite a criação de servidores web assíncronos no ESP8266/ESP32.
#include <SPIFFS.h>             // Função: Fornece acesso ao sistema de arquivos SPIFFS (SPI Flash File System), que permite armazenar e ler arquivos no flash da placa.
#include <TimeLib.h>            // Função: Manipulação de tempo e data em projetos Arduino.
#include <freertos/FreeRTOS.h>  // Função: Inclui a biblioteca FreeRTOS, um sistema operacional de tempo real para microcontroladores.
#include <freertos/task.h>      // Função: Contém definições para tarefas do FreeRTOS.
#include <EEPROM.h>             // Função: Permite a leitura e escrita de dados na EEPROM da placa Arduino.
#include <Wire.h>               // Função: Permite a comunicação I2C entre dispositivos.
#include <Adafruit_BMP085.h>    // Função: Biblioteca para interagir com o sensor de pressão atmosférica BMP180.
#include <ESPmDNS.h>            // Função: Essa biblioteca implementa o suporte mDNS no ESP8266/ESP32. Isso permite que você atribua um nome de host ao seu dispositivo na rede local, facilitando a descoberta de dispositivos na mesma rede por meio de um nome amigável, como "esp8266.local" ou "esp32.local", em vez de depender de endereços IP.
#include <ArduinoOTA.h>
