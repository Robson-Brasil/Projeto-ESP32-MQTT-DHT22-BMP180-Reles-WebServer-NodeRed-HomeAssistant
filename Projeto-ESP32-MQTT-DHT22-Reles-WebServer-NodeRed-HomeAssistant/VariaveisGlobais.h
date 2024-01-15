float diff = 1.0;

int val;

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
int status_todos = 0;   // Definir inteiro para lembrar o estado de alternância para todos

DNSServer dns;

// IP Estático
IPAddress local_IP(192, 168, 15, 50);
IPAddress gateway(192, 168, 15, 1);
IPAddress subnet(255, 255, 255, 0);

// DNS Estático
IPAddress primaryDNS(1, 1, 1, 1);
IPAddress secondaryDNS(8, 8, 8, 8);

// Configuração da Porta do WebServer Usada Pelo AsyncWebServer
AsyncWebServer server(3232);

// Status do WebServer
const char* PARAM_INPUT_1 = "relay";
const char* PARAM_INPUT_2 = "state";