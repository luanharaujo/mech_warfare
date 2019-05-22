#include <math.h>
//bibliotecas para conexão wifi e programaçao pela rede
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//bibliotecas para uso da PCA9685 via i2c
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN  200 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  645 // this is the 'maximum' pulse length count (out of 4096)

// mapeando os servos
uint8_t qfe = 0;
uint8_t cfe = 1;
uint8_t jfe = 2;
uint8_t qfd = 4;
uint8_t cfd = 5;
uint8_t jfd = 6;
uint8_t qte = 8;
uint8_t cte = 9;
uint8_t jte = 10;
uint8_t qtd = 12;
uint8_t ctd = 13;
uint8_t jtd = 14;

//conexão com a rede
#ifndef STASSID
#define STASSID "601D"
#define STAPSK  "men1nate"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

//calibra?ao da posi?ao dos servos
#define V0QFE 218
#define V90QFE 445
#define MAXQFE 530
#define MINQFE 140

#define V90CFE 195
#define V0CFE 415
#define MAXCFE 520
#define MINCFE 195

#define V0JFE 545
#define V90JFE 320
#define MAXJFE 630
#define MINJFE 210 //240


#define V0QFD 486
#define V90QFD 255
#define MAXQFD 560
#define MINQFD 220//tem que tirar a placa e medir de novo, algum dia


#define V90CFD 525
#define V0CFD 307
#define MAXCFD 550
#define MINCFD 215


#define V0JFD 293
#define V90JFD 513
#define MAXJFD 620
#define MINJFD 120

#define V0QTE 517
#define V90QTE 290
#define MAXQTE 590
#define MINQTE 195


#define V90CTE 540
#define V0CTE 310
#define MAXCTE 615
#define MINCTE 210


#define V0JTE 291
#define V90JTE 517
#define MAXJTE 623
#define MINJTE 170


#define V0QTD 195
#define V90QTD 435
#define MAXQTD 510 //tem o lance da madeira também
#define MINQTD 125

 
#define V0CTD 503
#define V90CTD 283
#define MAXCTD 610
#define MINCTD 230

 
#define V0JTD 535
#define V90JTD 308
#define MAXJTD 630
#define MINJTD 200

//tudo em milimetro
#define COMP_CANELA 125
#define COMP_COXA 559
#define COMP_QUADRIL 285
#define COMP_CORPOR 109

typedef struct angulos_perna
{
  int ang_coxa, ang_canela, ang_quadril;
}Angulos;

//declaração das funções
void setPos(uint8_t servoMotor, int pos);
int inv_ang_coxa(int delta_z, int delta_i);
int inv_ang_canela (int delta_z ,int delta_i, int ang_da_coxa);

void setup() {
  pwm.begin();//iniciando comunicação com a PCA
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  delay(10);
  
  //conectando com a rede
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  ArduinoOTA.setPort(4242);//mudamos a porta para não confundir com o ESP dos interrupitores aqui de casa
  // Hostname defaults to esp8266-[ChipID]
   ArduinoOTA.setHostname("MelhorRobozaoDeTodos");//mudando o nome do dispositivo pelo mesmo motivo
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
  });
  ArduinoOTA.onEnd([](){});
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total){});
  ArduinoOTA.onError([](ota_error_t error) {});
  ArduinoOTA.begin();
}

int ang_coxa;

void loop() {
  ArduinoOTA.handle();//função da conexão

  ang_coxa = inv_ang_coxa(110, 105);


  
  setPos(qfe,45);
  setPos(qfd,45);
  setPos(qte,45);
  setPos(qtd,45);

  //setPos(cfe, ang_coxa);
  setPos(cfe, ang_coxa);
  setPos(cfd,0);
  setPos(cte,0);
  setPos(ctd,0);

  //setPos(jfe,-inv_ang_canela(110, 105,ang_coxa));
  setPos(jfe,90);
  setPos(jfd,90);
  setPos(jte,90);
  setPos(jtd,90);
  delay(1000);
  
  
}

//funções para mexer as patas
void setPos(uint8_t servoMotor, int pos)
{
  int timeON;
  switch (servoMotor)
  {
    case 0:
      //if(pos>MAXQFE) pos = MAXQFE;
      //if(pos<MINQFE) pos = MINQFE;
      timeON = ((pos*(V90QFE-V0QFE))/90)+ V0QFE;
    break;
    case 1:
      //if(pos>MAXCFE) pos = MAXCFE;
      //if(pos<MINCFE) pos = MINCFE;
      timeON = ((pos*(V90CFE-V0CFE))/90)+ V0CFE;
    break;
    case 2:
      //if(pos>MAXJFE) pos = MAXJFE;
      //if(pos<MINJFE) pos = MINJFE;
      timeON = ((pos*(V90JFE-V0JFE))/90)+ V0JFE;
    break;
    case 4:
      //if(pos>MAXQFD) pos = MAXQFD;
      //if(pos<MINQFD) pos = MINQFD;
      timeON = ((pos*(V90QFD-V0QFD))/90)+ V0QFD;
    break;
    case 5:
      //if(pos>MAXCFD) pos = MAXCFD;
      //if(pos<MINCFD) pos = MINCFD;
      timeON = ((pos*(V90CFD-V0CFD))/90)+ V0CFD;
    break;
    case 6:
      //if(pos>MAXJFD) pos = MAXJFD;
      //if(pos<MINJFD) pos = MINJFD;
      timeON = ((pos*(V90JFD-V0JFD))/90)+ V0JFD;
    break;
    case 8:
      //if(pos>MAXQTE) pos = MAXQTE;
      //if(pos<MINQTE) pos = MINQTE;
      timeON = ((pos*(V90QTE-V0QTE))/90)+ V0QTE;
    break;
    case 9:
      //if(pos>MAXCTE) pos = MAXCTE;
      //if(pos<MINCTE) pos = MINCTE;
      timeON = ((pos*(V90CTE-V0CTE))/90)+ V0CTE;
    break;
    case 10:
      //if(pos>MAXJTE) pos = MAXJTE;
      //if(pos<MINJTE) pos = MINJTE;
      timeON = ((pos*(V90JTE-V0JTE))/90)+ V0JTE;
    break;
    case 12:
      //if(pos>MAXQTD) pos = MAXQTD;
      //if(pos<MINQTD) pos = MINQTD;
      timeON = ((pos*(V90QTD-V0QTD))/90)+ V0QTD;
    break;
    case 13:
      //if(pos>MAXCTD) pos = MAXCTD;
      //if(pos<MINCTD) pos = MINCTD;
      timeON = ((pos*(V90CTD-V0CTD))/90)+ V0CTD;
    break;
    case 14:
      //if(pos>MAXJTD) pos = MAXJTD;
      //if(pos<MINJTD) pos = MINJTD;
      timeON = ((pos*(V90JTD-V0JTD))/90)+ V0JTD;
    break;
  }
  pwm.setPWM(servoMotor, 0, timeON);
}

int inv_ang_canela (int delta_z, int delta_i, int ang_da_coxa)
{
  int aux;
  double ang_da_coxa_radiano = (ang_da_coxa*M_PI)/180;
  aux = delta_z*(COMP_COXA+COMP_CANELA*((int) cos(ang_da_coxa))) -\
    delta_i*COMP_CANELA*((int) sin(ang_da_coxa));
  aux /= (delta_i*(COMP_COXA+COMP_CANELA*((int) cos(ang_da_coxa))) +\
    delta_i*COMP_CANELA*((int) sin(ang_da_coxa)));  
  aux = delta_i - COMP_COXA*((int)sin(ang_coxa));
  aux = asin((double) aux);
  return ((aux*M_PI)/180);
}
int inv_ang_coxa(int delta_z, int delta_i)
{
  int aux;

  aux = delta_z*delta_z +\
        delta_i*delta_i -\
        COMP_CANELA*COMP_CANELA -\
        COMP_COXA*COMP_COXA;
  aux /= 2*COMP_COXA*COMP_CANELA;

  return ((int)(acos((double) aux)*180/M_PI));
}