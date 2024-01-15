
//Parametros do Sensor BMP180
Adafruit_BMP085 bmp;  //Define objeto sensor na classe SFE_BMP180 da biblioteca

// DHT11 ou DHT22 para leitura dos valores  de Temperatura e Umidade
#define DHTTYPE DHT22  // DHT11 ou DHT22
DHT dht(DHTPIN, DHTTYPE);

char str_hum_data[7];
char str_temp_data[7];
char str_tempterm_data[7];
char str_tempF_data[7];
