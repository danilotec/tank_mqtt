#include <lib.h>

// Implementações das funções
void setup() 
{
    // Inicializações:
    InitOutput();
    initSerial();
    initWiFi();
    initMQTT();
}
  
// Função: inicializa comunicação serial com baudrate 115200
void initSerial() 
{
    Serial.begin(115200);
}
 
// Função: inicializa e conecta-se na rede WI-FI desejada
void initWiFi() 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
     
    reconectWiFi();
}
  
// Função: inicializa parâmetros de conexão MQTT
void initMQTT() 
{
    espClient.setInsecure(); // Aceita qualquer certificado
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);
    MQTT.setCallback(mqtt_callback);
}
  
// Função: função de callback 
void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg;

    // Obtém a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
        char c = (char)payload[i];
        msg += c;
    }
   
    // Toma ação dependendo da string recebida
    if (msg.equals("L"))
    {
        digitalWrite(D0, LOW);
        EstadoSaida = '1';
    }
 
    if (msg.equals("D"))
    {
        digitalWrite(D0, HIGH);
        EstadoSaida = '0';
    }
}
  
// Função: reconecta-se ao broker MQTT
void reconnectMQTT() 
{
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT, USERNAME, PASSWORD_MQTT)) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE); 
        } 
        else
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Haverá nova tentativa de conexão em 2s");
            delay(2000);
        }
    }
}
  
// Função: reconecta-se ao WiFi
void reconectWiFi() 
{
    if (WiFi.status() == WL_CONNECTED)
        return;
         
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
     
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
   
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println(" IP obtido: ");
    Serial.println(WiFi.localIP());
}
 
// Função: verifica o estado das conexões WiFi e ao broker MQTT
void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()) 
        reconnectMQTT(); // se não há conexão com o Broker, a conexão é refeita
     
    reconectWiFi(); // se não há conexão com o WiFi, a conexão é refeita
}
 
// Função: envia ao Broker o estado atual do output 
void EnviaEstadoOutputMQTT(void)
{
    if (EstadoSaida == '1')
        MQTT.publish(TOPICO_PUBLISH, "L");
}

// Função: inicializa o output em nível lógico baixo
void InitOutput(void)
{
    pinMode(D0, OUTPUT);
    digitalWrite(D0, HIGH);          
}
 
// Programa principal
void loop() 
{   
    leitura = (readAnalogAverage(A0));
    pressao = (leitura * 20 / 4095);
    char TEMP[5];
    String CUR = String(leitura, 3);
    Serial.print("Leitura: ");
    Serial.println(leitura);
    CUR.toCharArray(TEMP, 7);
    strcpy(payload, "");
    strcat(payload, TEMP);
    strcat(payload, "");
    strcat(payload, timestamp);
    strcat(payload, "");
    Serial.print("Payload pressão do tanque: ");
    Serial.println(payload);
    
    // Publica a pressão no tópico MQTT
    MQTT.publish(TOPICO_PUBLISH, payload);

    // Garante funcionamento das conexões WiFi e ao broker MQTT
    VerificaConexoesWiFIEMQTT();
 
    // Envia o status de todos os outputs para o Broker no protocolo esperado
    EnviaEstadoOutputMQTT();
 
    // Keep-alive da comunicação com broker MQTT
    MQTT.loop();
    delay(1000); // Atraso de 1 segundo entre as leituras
}
