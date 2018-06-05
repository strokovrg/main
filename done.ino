#include <Ultrasonic.h>
Ultrasonic sonarL(37,39);
Ultrasonic sonarC(41,43);
Ultrasonic sonarR(40,42);
Ultrasonic sonarSideL(31,33);
Ultrasonic sonarSideR(50,52);
Ultrasonic sonarB(30,32);

#include <AFMotor.h>
AF_DCMotor m_FL(4, MOTOR34_64KHZ);
AF_DCMotor m_FR(3, MOTOR34_64KHZ);
AF_DCMotor m_BL(1, MOTOR12_64KHZ);
AF_DCMotor m_BR(2, MOTOR12_64KHZ);

enum Command {
  NONE_CMD = 0,
  STOP_CMD = 1,
  BACKWARD_CMD = 2,
  FORWARD_CMD = 3,
  LEFT_TURN_CMD = 4,
  RIGHT_TURN_CMD = 5,
  AUTOPILOT_CMD = 6
};

Command currentCommand; //команда, которая выполняется в данный момент
bool isAutopilot = false; //текущий режим: автопилот/ручное управление

float distL, distC, distR, distSideR, distSideL, distB; 
int sFL, sFR, sBL, sBR, pulsR, pulsL, direction = 0;

void setup() {
    Serial.begin(9600); 
    Serial1.begin(9600);
    Serial2.begin(115200);
    currentCommand = STOP_CMD;
}

void ultrazvuk(){                                                                  // считтывание расстояния датчиками ультразвука
  distL = sonarL.Ranging(CM);
  distR = sonarR.Ranging(CM);
  distC = sonarC.Ranging(CM);
  distSideL = sonarSideL.Ranging(CM);
  distSideR = sonarSideR.Ranging(CM);
  distB = sonarB.Ranging(CM);
}
 
void dvig (int FL, int FR,int BL, int BR) {                                     // установка ориентации направления моторов
    if (FL > 0){
        m_FL.run(FORWARD);
    }
    else {
        m_FL.run(BACKWARD);
        FL=-FL;
    }
    if (FR > 0){
        m_FR.run(BACKWARD);
    }
    else {
        m_FR.run(FORWARD);
        FR=-FR;
    }
    if (BL > 0){
        m_BL.run(BACKWARD);
    }
    else {
        m_BL.run(FORWARD);
        BL=-BL;
    }
    if (BR > 0){
        m_BR.run(FORWARD);
    }
    else {
        m_BR.run(BACKWARD);
        BR=-BR;
    }
  if (FL < 0 && FR > 0 && BL < 0 && BR > 0){    // влево
      direction -= 1;
  }
  else if (FL > 0 && FR < 0 && BL > 0 && BR < 0){    // вправо
      direction += 1;
  }  
  m_FL.setSpeed(FL);
  m_FR.setSpeed(FR);
  m_BL.setSpeed(BL);
  m_BR.setSpeed(BR);
  }

void vdolSR (int D){                                                                    // едем на расстоянии 6 см от стены  
     int x = 30;
     int Kr = (distSideR - D)*x;
     
     sBL+=Kr;
     sFL+=Kr;
     sFL-=Kr;
     sBR-=Kr;
}
void vdolSL (int D){                                                                    // едем на расстоянии 6 см от стены  
     int x = 30;
     int Kl = (distSideL - D)*x;
     sBR+=Kl;
     sFR+=Kl;
     sFL-=Kl;
     sBL-=Kl;
}

/*void gogo(int FL, int FR, int BL, int BR, int delay){                              // робот едет указанное количество меток (1 метка = 1 см)

        int x = pulsR + ticks;
        while (pulsR <= x ){ 
          dvig(FL, FR, BL, BR);
        }
        dvig(0,0,0,0);
}*/

void turnLEFT(){                                                                   // поворот на 90 гр влево
        sFR = 220;
        sFL = -220;
        sBR = 220;
        sBL = -220; 
        delay(500);
       
}
void turnRIGHT(){                                                                  // поворот на 90 гр вправо
        
        sFR = -220;
        sFL = 220;
        sBR = -220;
        sBL = 220; 
        delay(500);
}

void m_speed (){                                                                   // основная программа движения с нечеткой логикой


float nL, nR, nC, nSR, nSL;
 /* 
    
   

    if (distC <= 15 || distL <= 8 || distR <= 8){                                  // объезд препятствия и движение вдоль преграды
         int nL = sonarL.Ranging(CM);
         int nR = sonarR.Ranging(CM);
         int nC = sonarC.Ranging(CM);
         int nSR = sonarSideR.Ranging(CM);
         int nSL = sonarSideL.Ranging(CM);
          delay(1000);
            if (nL >= 8 && nSL >= 15 && nSR >= 10) {                                 // поворот на 90гр влево
                  turnLEFT();
                  gogo(200, 200, 200, 200, 15);   
                
                   nL = sonarL.Ranging(CM);
                   nR = sonarR.Ranging(CM);
                   nC = sonarC.Ranging(CM);
                   nSR = sonarSideR.Ranging(CM);
                   nSL = sonarSideL.Ranging(CM);
                   
                  if (nR >= 8 && nSR >= 30 && nSL >= 10){                            // поворот на 90гр вправо 
                        turnRIGHT();
                  }
                  else if (sonarSideR.Ranging(CM) <= 15){
                      vdolSR(10);
                        if (sonarC.Ranging(CM) < 10){
                            gogo(0, 250, 0, 250, 110);
                            }
                        vdolSR(10);
                      }
                   else {
                       while (sonarC.Ranging(CM) > 10){
                           dvig(200, 200, 200, 200);
                       } ;  
                   }
            }
    
              else if (nL < 8 && (nR >= 8 && nSR >= 15 && nSL >= 15)) {              // поворот на 90гр вправо
                  turnRIGHT();
                  gogo(200, 200, 200, 200, 15); 
                 
                   nL = sonarL.Ranging(CM);
                   nR = sonarR.Ranging(CM);
                   nC = sonarC.Ranging(CM);
                   nSR = sonarSideR.Ranging(CM);
                   nSL = sonarSideL.Ranging(CM);
                   
                  if (nL >= 8 && nSL > 15 && nSR > 15){                            // поворот на 90гр влево 
                        turnLEFT();
                  }
                  else if (sonarSideR.Ranging(CM) <= 15){
                      vdolSL(10);
                       if (sonarC.Ranging(CM) < 15){
                            gogo(250, 0, 250, 0, 110);
                            
                        }
                        vdolSL(10);
                      }
                  else  {
                       while (sonarC.Ranging(CM) > 10){
                           dvig(200, 200, 200, 200);
                       };
                   }
              }
    }     
    */
if  (distC <= 20){                                  // объезд препятствия и движение вдоль преграды
         int nL = sonarL.Ranging(CM);
         int nR = sonarR.Ranging(CM);
         int nC = sonarC.Ranging(CM);
         int nSR = sonarSideR.Ranging(CM);
         int nSL = sonarSideL.Ranging(CM);
          delay(1000);
            if (nL >= 8 && nSL >= 15 && nSR >= 10) {                                 // поворот на 90гр влево
                  turnLEFT();
                  dvig(200,200,200,200);
                  delay(700);
                  turnRIGHT(); }}   

if  (distC < 10 || distL < 10 || distR < 10){                                   // экстремальная остановка    
     sFL = 0; 
     sFR = 0;
     sBL = 0; 
     sBR = 0;}
     
dvig(sFL, sFR, sBL, sBR);
    
}

bool tryApplyManualCommand(Command command) {
  bool isSuccess = false;
  switch(command) {
    case FORWARD_CMD:
    if(distC>7&&distL>10&&distR>10) {
      dvig(130,130,130,130);
      isSuccess = true;
    }
    break;
    
    case BACKWARD_CMD:
    if(distB>10) {
      dvig(-100,-100,-100,-100);
      isSuccess = true;
    }
    break;
    
    case LEFT_TURN_CMD:
    if(distC>5&&distL>8) {
      dvig(-220,220,-220,220);
      isSuccess = true;
    }
    break;

    case RIGHT_TURN_CMD:
    if(distC>5&&distR>8) {
      dvig(220,-220,220,-220);
      isSuccess = true;
    }
    break;

    default:
    //Команда "стоп" выполняется единожды, после остановки робот находится в ожидании
    dvig(0,0,0,0);
    currentCommand=NONE_CMD;
    isSuccess = true;
    break;
  }
  return isSuccess;
}

void loop() {
    //Проверка на новые команды
  if(Serial1.available()>0) {
    uint8_t data = Serial1.read();
    data-=48;
    if(data < 7) {
      currentCommand = (Command)data;
    }
  }
  else if(Serial2.available()>0) {
    uint8_t data = Serial2.read();
    data-=48;
    if(data < 7) {
      currentCommand = (Command)data;
    }
  }

  //Команда "автопилот" выполняется единожды, её задача - переключение режима
  if(currentCommand == AUTOPILOT_CMD){
    if(isAutopilot) {
      isAutopilot = false;
      currentCommand = STOP_CMD;
    }
    else{
      isAutopilot = true;
      currentCommand = NONE_CMD;
    }
  }

  if(currentCommand!=NONE_CMD) {
    ultrazvuk();
    if(!tryApplyManualCommand(currentCommand)){
      //Если путь по направлению загорожен, то остановить робота
      tryApplyManualCommand(STOP_CMD);
    }
    //При поступлении команды движения, режим автопилот сбрасывается
    isAutopilot = false;
  }
  else if(isAutopilot) {
    ultrazvuk();
    m_speed();
  }
  Serial.print (pulsR);
  Serial.print ("\n\r");
  Serial.print ("\n\r");

  
}





