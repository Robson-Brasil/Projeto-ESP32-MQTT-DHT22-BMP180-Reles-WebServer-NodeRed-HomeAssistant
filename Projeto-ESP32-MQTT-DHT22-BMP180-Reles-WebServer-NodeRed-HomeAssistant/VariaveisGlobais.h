// VariaveisGlobais.h
#ifndef VARIAVEIS_GLOBAIS_H
#define VARIAVEIS_GLOBAIS_H

#include <DNSServer.h>
#include <IPAddress.h>
#include <ESPAsyncWebServer.h>

// Variáveis de estado de alternância
int toggleState_0 = 1;  // Definir inteiro para lembrar o estado de alternância para o relé 0
int toggleState_1 = 1;  // Definir inteiro para lembrar o estado de alternância para o relé 1
int toggleState_2 = 1;  // Definir inteiro para lembrar o estado de alternância para o relé 2
int toggleState_3 = 1;  // Definir inteiro para lembrar o estado de alternância para o relé 3
int toggleState_4 = 1;  // Definir inteiro para lembrar o estado de alternância para o relé 4
int toggleState_5 = 1;  // Definir inteiro para lembrar o estado de alternância para o relé 5
int toggleState_6 = 1;  // Definir inteiro para lembrar o estado de alternância para o relé 6
int toggleState_7 = 1;  // Definir inteiro para lembrar o estado de alternância para o relé 7
int toggleState_8 = 1;  // Definir inteiro para lembrar o estado de alternância para o relé 8
int toggleState_9 = 1;  // Definir inteiro para lembrar o estado de alternância para o relé 8 via sensor PIR
int toggleState_10 = 1;  // Definir inteiro para lembrar o estado de alternância para o DesligarTudo
int status_todos = 0;   // Definir inteiro para lembrar o estado de alternância para todos
int status_desligatodos = 0;

// Variável global
extern float diff;

// Configuração do servidor DNS
extern DNSServer dns;

// Configuração de IP estático
extern IPAddress local_IP;
extern IPAddress gateway;
extern IPAddress subnet;

// Configuração de DNS estático
extern IPAddress primaryDNS;
extern IPAddress secondaryDNS;

// Configuração do servidor web
extern AsyncWebServer server;

// Parâmetros do servidor web
extern const char* PARAM_INPUT_1;
extern const char* PARAM_INPUT_2;

#endif // VARIAVEIS_GLOBAIS_H
