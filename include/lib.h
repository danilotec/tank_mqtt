// Programa: NodeMCU e MQTT - Controle e Monitoramento IoT
// Autor: Danilo Andrade

#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

#define TOPICO_SUBSCRIBE "MQTT_TANK_IN"     
#define TOPICO_PUBLISH   "TANQUE_TESTE_CR" 
#define ID_MQTT  "TANQUE_teste"                    

#define D0    16
#define D1    5
#define D2    4
#define D3    0
#define D4    2
#define D5    14
#define D6    12
#define D7    13
#define D8    15
#define D9    3
#define D10   1
#define A0    35 // Pino do sensor A0

#define NUM_READINGS 30  // Número de leituras para calcular a média
 
// WIFI
const char* SSID = "4G-MIFI-6AC"; // SSID da rede WI-FI
const char* PASSWORD = "1234567890"; // Senha da rede WI-FI
  
// MQTT
const char* BROKER_MQTT = "#"; // URL do broker MQTT
int BROKER_PORT = 8883; // Porta do Broker MQTT para conexões seguras
const char* USERNAME = "#"; // Substitua pelo seu usuário
const char* PASSWORD_MQTT = "#"; // Substitua pela sua senha

// Variáveis e objetos globais
WiFiClientSecure espClient; // Cria o objeto espClient para conexões seguras
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT
char EstadoSaida = '0';  // variável que armazena o estado atual da saída
float leitura;
float pressao;
char payload[23] = "00.0";
char timestamp[] = "";

// Função para calcular a média das leituras analógicas
int readAnalogAverage(int pin) {
    long sum = 0;  // Usamos long para evitar overflow
    for (int i = 0; i < NUM_READINGS; i++) {
        sum += analogRead(pin);
        delay(10);  // Pequeno atraso para estabilização
    }
    return sum / NUM_READINGS;  // Retorna a média
}

// Prototypes
void initSerial();
void initWiFi();
void initMQTT();
void reconectWiFi(); 
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIEMQTT(void);
void InitOutput(void);
void EnviaEstadoOutputMQTT(void);