int PortaBroker = 1883;  // Porta do Broker MQTT

#define ID_MQTT "ESP32-IoT-Broker1" /* ID MQTT (para identificação de seção)
                                        IMPORTANTE: Este deve ser único no broker (ou seja,
                                        se um client MQTT tentar entrar com o mesmo
                                        ID de outro já conectado ao broker, o broker
                                        irá fechar a conexão de um deles).*/

// Variáveis e objetos globais
WiFiClient espClient;          // Cria o objeto espClient
PubSubClient MQTT(espClient);  // Instância o Cliente MQTT passando o objeto espClient
