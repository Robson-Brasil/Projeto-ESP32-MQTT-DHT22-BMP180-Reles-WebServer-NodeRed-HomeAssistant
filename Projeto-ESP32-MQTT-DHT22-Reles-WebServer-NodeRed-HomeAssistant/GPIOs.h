// Defines - Mapeamento de pinos do NodeMCU Relays
#define RelayPin1 26  // D26 Ligados ao Nora/MQTT
#define RelayPin2 13  // D13 Ligados ao Nora/MQTT
#define RelayPin3 14  // D14 Ligados ao Nora/MQTT
#define RelayPin4 32  // D32 Ligados ao Nora/MQTT
#define RelayPin5 16  // D16 Ligados ao Nora/MQTT
#define RelayPin6 17  // D17 Ligados ao MQTT
#define RelayPin7 18  // D18 Ligados ao MQTT
#define RelayPin8 19  // D19 Ligados ao MQTT

bool RelayPin1Estado = false; // State of RELÉ 1
bool RelayPin2Estado = false; // State of RELÉ 2
bool RelayPin3Estado = false; // State of RELÉ 3
bool RelayPin4Estado = false; // State of RELÉ 4
bool RelayPin5Estado = false; // State of RELÉ 5
bool RelayPin6Estado = false; // State of RELÉ 6
bool RelayPin7Estado = false; // State of RELÉ 7
bool RelayPin8Estado = false; // State of RELÉ 8

// WiFi Status Relé
#define wifiLed   0  //D0

// DHT11 ou DHT22 para leitura dos valores  de Temperatura e Umidade
#define DHTPIN    25  //D25

//Sennsor PIR
#define SensorPIR 27 //D27

/*Sensor BMP180 Ligação I2C
SDA               21
SLC               22
*/
