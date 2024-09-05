#ifndef MQTT_H
#define MQTT_H

#include <WiFi.h>              // Biblioteca Wi-Fi para o ESP32
#include <PubSubClient.h>      // Biblioteca para MQTT

// Configurações do Broker MQTT
const int PortaBroker = 1883;  // Porta do Broker MQTT

#define ID_MQTT "Escolha um Nome único e diferente" /* ID MQTT (para identificação de sessão)
                                          IMPORTANTE: Este deve ser único no broker (ou seja,
                                          se um client MQTT tentar entrar com o mesmo
                                          ID de outro já conectado ao broker, o broker
                                          irá fechar a conexão de um deles). */

// Variáveis e objetos globais
WiFiClient espClient;          // Cria o objeto espClient para a conexão Wi-Fi
PubSubClient MQTT(espClient);  // Instância o Cliente MQTT passando o objeto espClient

#define MQTT_KeepAlive 120     // Intervalo de keep-alive em segundos

// Funções para conectar, enviar e receber mensagens via MQTT podem ser declaradas aqui.

#endif
