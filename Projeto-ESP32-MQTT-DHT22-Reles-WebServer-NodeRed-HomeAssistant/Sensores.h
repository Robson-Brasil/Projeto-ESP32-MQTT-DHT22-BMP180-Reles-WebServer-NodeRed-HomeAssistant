#ifndef SENSORES_H
#define SENSORES_H

#include <Adafruit_BMP085.h>  // Biblioteca para o sensor BMP180
#include <DHT.h>              // Biblioteca para o sensor DHT11/DHT22

// Parâmetros do Sensor BMP180
Adafruit_BMP085 bmp;          // Define o objeto sensor BMP180

// DHT11 ou DHT22 para leitura dos valores  de Temperatura e Umidade
#define DHTTYPE DHT22         // Defina o tipo de sensor: DHT11 ou DHT22
DHT dht(DHTPIN, DHTTYPE);     // Instancia o objeto do sensor DHT

// Variáveis para armazenar os dados dos sensores em formato de string
char str_hum_data[7];        // Para armazenar a umidade em formato string
char str_temp_data[7];       // Para armazenar a temperatura (Celsius)
char str_tempterm_data[7];   // Para armazenar dados de temperatura de outro sensor, se necessário
char str_tempF_data[7];      // Para armazenar a temperatura (Fahrenheit)

// Funções para iniciar os sensores, ler os dados, etc., podem ser declaradas aqui.

#endif