#include <HardwareSerial.h>
#include <ModbusMaster.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <Wire.h>

HardwareSerial MITSUBISHI( 2 ); // DECLARAÇÃO DO OBJETO HARRDWARE SERIAL -> MITSUBISHI

// -----------------------[MODBUSMASTER - SETUP]-------------------- //

ModbusMaster node; // DECLARAÇÃO DO OBJETO ModbusMaster -> node (VFD)
ModbusMaster node2; // DECLARAÇÃO DO OBJETO ModbusMaster -> node (VFD)

uint8_t deviceID = 1;
uint8_t resultMain;
String c, texto;
String ID, REGISTER, VALUE;


uint8_t deviceIDpf = 1;
uint8_t resultMainpf;
String  textopf;
String IDpf, REGISTERpf, VALUEpf;

// -----------------------[/MODBUSMASTER - SETUP]-------------------- //

// SSID/PASSWORD DA REDE

// const char* ssid = "HotspotLimas";
// const char* password = "Lolista22";

const char* ssid = "Atena_IoT";
const char* password = "atenaiot20";

void callback(char* topic, byte* payload, unsigned int length);

//PROCESSOS
TaskHandle_t Task0;
TaskHandle_t Task1;

// const char* mqtt_server = "192.168.137.125"; // BROKER'S IP ADDRESS
 const char* mqtt_server = "192.168.1.68"; // BROKER'S IP ADDRESS

WiFiClient espClient;
PubSubClient client(mqtt_server, 1883, callback, espClient);
long lastMsg = 0;
char Msg[50];
int value = 0;

#define RE_DE 21 // RE_DE PIN
#define TXD 17 // TX PIN
#define RXD 16 // RX PIN

#define BaudRate 9600 // BAUDRATE 
#define Protocol SERIAL_8O1 // SÉRIE 8 BITS C/ 1 STOP BIT

void preTransmission() {
  digitalWrite(RE_DE, HIGH);
}

void postTransmission() {
  digitalWrite(RE_DE, LOW);
}

int bits_per_byte = 8;
int us_to_wait = (12 * bits_per_byte) * 1000000 / BaudRate; // = (1+8+1) bytes
int start_timer = 1000;
int end_timer = (6 * 11 * bits_per_byte) * 1000000 / BaudRate; // = 4x(1+9+1) bytes

byte MSGtoMITSUBISHI[8]; // BYTE - QUERY DE LEITURA
byte MSGtoMITSUBISHI2[8]; // BYTE - QUERY DE ESCRITA
byte MSGtoESP32[20]; // MENSAGEM ENVIADA AO ESP
byte MsgReceived[7]; // MENSAGEM RECEBIDA VARIADOR -> ESP

float f = 0.00;

//--------------------------------------------- [FORMATO DE UMA MENSAGEM SERIAL] ------------------------------------------------- //

// Slave Address // Function Code // Byte Count //First Register Hi //First Register Lo //Error Check Lo (CRC) //Error Check Hi (CRC)

int start_index = 0;
int msgF = 0;
int msgI = 0;
int msgT = 0;
int location = 3;

void setup() {

  Serial.begin(9600);
  MITSUBISHI.begin(BaudRate, Protocol, RXD, TXD);
  Serial2.begin(BaudRate, Protocol, RXD, TXD);

  pinMode(RE_DE, OUTPUT);
  digitalWrite(RE_DE, LOW);

  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  node2.preTransmission(preTransmission);
  node2.postTransmission(postTransmission);


  setup_wifi();

  // -----------------------[CABEÇALHO DA QUERY DE LEITURA]-------------------- //

  //Slave Address - 0x01
  MSGtoMITSUBISHI[0] = 0x01;
  //Function Code - 0x03
  MSGtoMITSUBISHI[1] = 0x03;
  //Number of Points Hi - 0x00
  MSGtoMITSUBISHI[4] = 0x00;
  //Number of Points Lo - 0x01
  MSGtoMITSUBISHI[5] = 0x01;

  // -----------------------[/CABEÇALHO DA QUERY DE LEITURA]-------------------- //

  // -----------------------[CABEÇALHO DA QUERY DE ESCRITA]--------------------- //

  //Slave Address - 0x01
  MSGtoMITSUBISHI2[0] = 0x01;
  //Function Code - 0x06
  MSGtoMITSUBISHI2[1] = 0x06;
  //Number of Points Hi - 0x00
  MSGtoMITSUBISHI2[2] = 0x00;
  //Number of Points Lo - 0x0D
  MSGtoMITSUBISHI2[3] = 0x0D;

  // ----------------------[/CABEÇALHO DA QUERY DE ESCRITA]--------------------- //

  delay(500);

  xTaskCreatePinnedToCore(
      Task1code, /* Function to implement the task */
      "Task1", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task1,  /* Task handle. */
      0); /* Core where the task should run */
      
}

void Task1code (void * parameter) {
  
  for(;;) {
    
     if (!client.connected())
         reconnect();

         client.loop();
         
  }
  

}

void setup_wifi() {


  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

} // void setup_wifi

char* topiccp;
 

void callback(char* topic, byte* payload, unsigned int length) {
  
  String messageTemp; 

  Serial.print("Message arrived in topic: ");
  Serial.println(topic); 
  

  Serial.print("MESSAGE: ");  

  for (int i = 0; i < length; i++) {

    Serial.print((char)payload[i]);   
    messageTemp += (char)payload[i];

  }

  textopf = messageTemp;
  Serial.println();
  Serial.println(textopf);
  

  
  if (strcmp(topic, "esp/pubfreq") == 0)
  {
     
    Serial.println("AQUI");

    IDpf = textopf.substring(2, 3);            // pega o número de ID enviado pela serial
    //REGISTER = texto.substring(3, 7);      // pega o endereço da varíavel que recebera a escrita ex: No inv delta ms300 a control word é 2000H
    VALUEpf = textopf.substring(3, 7);        // pega o valor q será escrito na variável Ex: o valor 0002H coloca o inv em RUN      
    deviceIDpf = 0;

    switch (deviceIDpf) {
  
      case 0:
  
        node.begin(getInt(IDpf), Serial2);
        resultMainpf = node.writeSingleRegister(0x000D, getInt(VALUEpf)); // para o inv delta a controlword é 2000H = 8192 decimal   value = 0002 run, = 0001 stop
  
        deviceIDpf = 1;
        textopf = "";
        IDpf = "";
        //REGISTER = "";
        VALUEpf = "";
        textopf = "";      
  
        if (resultMainpf == node.ku8MBSuccess) {
  
          Serial.println("O COMANDO FOI MANDADO AO VARIADOR.");
  
        } else {
  
          Serial.println("ERRO");
          Serial.println("ESTÁ A PARAR NO 1º CASE.");
          
        }      

        delay(45);

  } // switch

} // if topic == pubfreq

Serial.println();

Serial.println("-----------------------");

messageTemp = "";



} // void callback


void reconnect() {

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Loop until we're reconnected
  while (!client.connected()) {

    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {

      Serial.println("connected");

      // MQTT TOPICS SUBSCRIPTION
      client.subscribe("esp/pubfreq");
      client.subscribe("Intensidade");
      client.subscribe("Frequência");
      client.subscribe("Tensão");

    } else { // IN CASE THE BROKER CONNECTION FAILS

      Serial.print("Failed connection, rc = ");
      Serial.print(client.state());
      Serial.println(" Trying again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);

    }

    delay(500);

  } // while (!client.connected)

} // void reconnect

void loop() {

  
  delay(100);


  //    --------------------------------------------------------------------------------------------------------------------------------------------------------------





  //  --------------------------------------------------------------------------------------------------------------------------------------------------------------

  // MSGtoMITSUBISHI2[4] = MSGtoMITSUBISHI2[5] = MSGtoMITSUBISHI2[6] = MSGtoMITSUBISHI2[7] = 0x00;
  // MSGtoMITSUBISHI[2] = MSGtoMITSUBISHI[3] = MSGtoMITSUBISHI[6] = MSGtoMITSUBISHI[7] = 0x00;


  //---------------------------[LER FREQUÊNCIA]-------------------------//
  //Start Timer
  delay(start_timer);

  //Write Enable
  digitalWrite(RE_DE, HIGH);
  Serial.println("WRITE: 01 03 00 0D 00 01 15 C9");

  MSGtoMITSUBISHI[2] = 0x00;
  MSGtoMITSUBISHI[3] = 0x0D;
  MSGtoMITSUBISHI[6] = 0x15;
  MSGtoMITSUBISHI[7] = 0xC9;

  MITSUBISHI.write(MSGtoMITSUBISHI, sizeof(MSGtoMITSUBISHI));
  delayMicroseconds(us_to_wait);

  //Read Enable
  digitalWrite(RE_DE, LOW);
  Serial.print("READ:");

  //End Timer
  delayMicroseconds(end_timer);
  delay(500);

  if (MITSUBISHI.available() >= sizeof(MsgReceived)) {
    for (int i = 0; i < sizeof(MSGtoESP32); i++) {
      MSGtoESP32[i] = MITSUBISHI.read();
      if ((MSGtoESP32[i - 1] == MSGtoMITSUBISHI[0]) && (MSGtoESP32[i] == MSGtoMITSUBISHI[1])) {
        start_index = i - 1;
      }
    }

    for (int i = start_index; i < sizeof(MsgReceived); i++) {
      MsgReceived[i - start_index] = MSGtoESP32[i];
      Serial.print(" ");
      int hex_value = MSGtoESP32[i];
      if (hex_value < 0x10) {
        Serial.print("0");
      }
      Serial.print(hex_value, HEX);
    }
    Serial.println();

    msgF = 0;
    msgF += MsgReceived[3];
    msgF = msgF << 8;
    msgF += MsgReceived[4];
    delay(500);

    float Frequency = ((float)msgF) / 100;

    /**
       Convert to char array to publish on the mqtt broker
    */

    // Convert the value to a char array
    char freqString[8];
    dtostrf(Frequency, 1, 2, freqString);
    client.publish("Frequência", freqString);

    long now = millis();
    if (now - lastMsg > 5000) {
      lastMsg = now;
    }

    Serial.print("Frequência: ");
    Serial.print(Frequency, 2);
    Serial.println(" Hz");
    Serial.println("-----------------------------");

  }

  //---------------------------[/ LER FREQUÊNCIA]-------------------------//

  //---------------------------[LER INTENSIDADE]-------------------------//

  //Start Timer
  delay(start_timer);

  //Write Enable
  digitalWrite(RE_DE, HIGH);
  Serial.println("WRITE:  01 03 00 C9 00 01 54 34");

  //Starting Address Hi - 0x00
  MSGtoMITSUBISHI[2] = 0x00;
  //Starting Address Lo - 0xC9
  MSGtoMITSUBISHI[3] = 0xC9;
  //Error Check Lo (CRC) - 0x54
  MSGtoMITSUBISHI[6] = 0x54;
  //Error Check Hi (CRC) - 0x34
  MSGtoMITSUBISHI[7] = 0x34;

  MITSUBISHI.write(MSGtoMITSUBISHI, sizeof(MSGtoMITSUBISHI));
  delayMicroseconds(us_to_wait);

  //Read Enable
  digitalWrite(RE_DE, LOW);
  Serial.print("READ:");

  //End Timer
  delayMicroseconds(end_timer);

  if (MITSUBISHI.available() >= sizeof(MsgReceived)) {
    for (int i = 0; i < sizeof(MSGtoESP32); i++) {
      MSGtoESP32[i] = MITSUBISHI.read();
      if ((MSGtoESP32[i - 1] == MSGtoMITSUBISHI[0]) && (MSGtoESP32[i] == MSGtoMITSUBISHI[1])) {
        start_index = i - 1;
      }
    }

    for (int i = start_index; i < sizeof(MsgReceived); i++) {
      MsgReceived[i - start_index] = MSGtoESP32[i];
      Serial.print(" ");
      int hex_value = MSGtoESP32[i];
      if (hex_value < 0x10) {
        Serial.print("0");
      }
      Serial.print(hex_value, HEX);
    }
    Serial.println();

    msgI = 0;
    msgI += MsgReceived[3];
    msgI = msgI << 8;
    msgI += MsgReceived[4];
    delay(500);


    float Current = ((float)msgI) / 100;

    char intensidadeString[8];
    dtostrf(Current, 1, 2, intensidadeString);
    client.publish("Intensidade", intensidadeString);


    Serial.print("Intesidade: ");
    Serial.print(Current, 2);
    Serial.println(" A");
    Serial.println("-----------------------------");
  }

  //---------------------------[/LER INTENSISDADE]-------------------------//

  //---------------------------[LER TENSÃO]-------------------------//

  //Start Timer
  delay(start_timer);

  //Write Enable
  digitalWrite(RE_DE, HIGH);
  Serial.println("WRITE: 01 03 00 CA 00 01 A4 34 ");

  //Starting Address Hi - 0x00
  MSGtoMITSUBISHI[2] = 0x00;
  //Starting Address Lo - 0xCA
  MSGtoMITSUBISHI[3] = 0xCA;
  //Error Check Lo (CRC) - 0x15
  MSGtoMITSUBISHI[6] = 0xA4;
  //Error Check Hi (CRC) - 0xC9
  MSGtoMITSUBISHI[7] = 0x34;

  MITSUBISHI.write(MSGtoMITSUBISHI, sizeof(MSGtoMITSUBISHI));
  delayMicroseconds(us_to_wait);

  //Read Enable
  digitalWrite(RE_DE, LOW);
  Serial.print("READ:");

  //End Timer
  delayMicroseconds(end_timer);

  if (MITSUBISHI.available() >= sizeof(MsgReceived)) {
    for (int i = 0; i < sizeof(MSGtoESP32); i++) {
      MSGtoESP32[i] = MITSUBISHI.read();
      if ((MSGtoESP32[i - 1] == MSGtoMITSUBISHI[0]) && (MSGtoESP32[i] == MSGtoMITSUBISHI[1])) {
        start_index = i - 1;
      }
    }

    for (int i = start_index; i < sizeof(MsgReceived); i++) {
      MsgReceived[i - start_index] = MSGtoESP32[i];
      Serial.print(" ");
      int hex_value = MSGtoESP32[i];
      if (hex_value < 0x10) {
        Serial.print("0");
      }
      Serial.print(hex_value, HEX);
    }
    Serial.println();

    msgT = 0;
    msgT += MsgReceived[3];
    msgT = msgT << 8;
    msgT += MsgReceived[4];
    delay(500);

    float Voltage = ((float)msgT) / 10;

    char voltString[8];
    dtostrf(Voltage, 1, 2, voltString);
    client.publish("Tensão", voltString);
    Serial.println(msgT);

    Serial.print("Tensão: ");
    Serial.print(Voltage, 1);
    Serial.println(" V");
    Serial.println("-----------------------------");
  }

  //---------------------------[/LER TENSÃO]------------------------ //


  // --------------------[LEITURA DE MENSAGENS SERIAL DO REGISTO 0x000D]------------------------ //

  //Start Timer
  delay(start_timer);

  // delay(500);


 while (Serial.available() > 0) {
      
        c = (char)Serial.read();  // READ FROM SERIAL MONITOR
        texto += c; 
    
        //    if (c == ":")
        //      break;
        //    else
        //      texto += c; // texto = texto + c;

        ID = texto.substring(2, 3);            // pega o número de ID enviado pela serial
        //REGISTER = texto.substring(3, 7);      // pega o endereço da varíavel que recebera a escrita ex: No inv delta ms300 a control word é 2000H
        VALUE = texto.substring(3, 7);        // pega o valor q será escrito na variável Ex: o valor 0002H coloca o inv em RUN
        deviceID = 0;
        
  } // while Serial.Available
    

    switch (deviceID) {
  
      // Device ID imaginário - Serve apenas para enviar comandos ao Inversor via RS485
      case 0:
  
        node.begin(getInt(ID), Serial2);
        resultMain = node.writeSingleRegister(0x000D, getInt(VALUE)); // para o inv delta a controlword é 2000H = 8192 decimal   value = 0002 run, = 0001 stop
  
        deviceID = 1;
        texto = "";
        ID = "";
        REGISTER = "";
        VALUE = "";
  
  
        if (resultMain == node.ku8MBSuccess) {
  
          Serial.println("O comando foi mandado ao variador.");
  
        } else {
  
          Serial.println("ERRO");
          Serial.println("Está a parar no 1º case");
        }
  
        delay(500);
        break;
  
    } // switch


  Serial.flush();

} //loop


// Função que realiza a conversão de um caracter em ASCII para numero inteiro
int getInt(String texto) {

  int temp = texto.length() + 1;
  char buffer[temp];
  texto.toCharArray(buffer, temp);
  int num = atoi(buffer);
  return num;

}
