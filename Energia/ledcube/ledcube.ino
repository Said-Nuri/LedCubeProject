 #include "Energia.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

  // ledCubeMatrix[layer][satır][sütun]
  unsigned char ledCubeMatrix[8][8][1];
  unsigned char incoming;

void initTimer()
{  
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);   // 32 bits Timer
  TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0Isr);    // Registering  isr       
  ROM_TimerEnable(TIMER0_BASE, TIMER_A); 
  ROM_IntEnable(INT_TIMER0A); 
  ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);  
}
 
void Timer0Isr(void)
{
  ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);  // Clear the timer interrupt
  digitalWrite(RED_LED, digitalRead(RED_LED) ^ 1);              // toggle LED pin
  showLedCubeMatrixLayer();
}

  
void setup()
{
  initTimer();
  
  unsigned long ulPeriod;
  unsigned int Hz = 2000;   // frequency in Hz  
  ulPeriod = (SysCtlClockGet() / Hz)/ 2;
  ROM_TimerLoadSet(TIMER0_BASE, TIMER_A,ulPeriod -1);
  
  Serial.begin(115200);

  // Setup columns
  pinMode(PA_7, OUTPUT);
  pinMode(PA_6, OUTPUT);
  pinMode(PA_5, OUTPUT);
  pinMode(PA_4, OUTPUT);
  pinMode(PC_7, OUTPUT);
  pinMode(PC_6, OUTPUT);
  pinMode(PC_5, OUTPUT);
  pinMode(PC_4, OUTPUT);
  
  // Setup row selector
  pinMode(PE_0, OUTPUT);
  pinMode(PE_1, OUTPUT);
  pinMode(PE_2, OUTPUT);
  
  // Setup enable/disable out
  pinMode(PE_3, OUTPUT);
  
  // Setup layers
  pinMode(PB_7, OUTPUT);
  pinMode(PB_6, OUTPUT);
  pinMode(PB_5, OUTPUT);
  pinMode(PB_4, OUTPUT);
  pinMode(PB_3, OUTPUT);
  pinMode(PB_2, OUTPUT);
  pinMode(PB_1, OUTPUT);
  pinMode(PB_0, OUTPUT);
  
  initLedCube(0);
}

void loop()
{   
    getData();
}

void getData(){
      
    int layer, j;   
    int flag = 0;   
    unsigned char data;   
        
    while(1){   
          
      while(1){   
        if(Serial.available()){   
          data = Serial.read();   
              
          if(data == '+'){    
            Serial.print("Starter arrived\n");    
            break;            
          }else{    
            //Serial.print("waiting for starter...\n");   
          }   
          delay(10);    
        }   
      }//inner while    
          
      for(layer = 0; layer < 8; ++layer){   
          for(j = 0; j < 8; ++j){   
              if(Serial.available()){   
                  data = Serial.read(); 
                  //Serial.print(layer);
                  //Serial.print(":");
                  //Serial.print(j);
                  //Serial.print("\n");   
                  ledCubeMatrix[layer][j][0] = data;                
              }   
           delay(1);        
          }     
      }   
          
      if(Serial.available()){   
        data = Serial.read();   
            
        if(data == '-')   
          Serial.print("Data transfer complated succesfully\n");    
        else{   
          Serial.print("Terminate character could not received: ");
          Serial.print(data);
          Serial.print("\n");   
        }   
      }   
    } //outer while   
        
                              
 }

void initLedCube(unsigned char value){
  
  unsigned char i;

  for(i=0 ; i<8 ; i++){
    initLayer(i,value);
  }
  
}

void initLayer(unsigned char layer, unsigned char value){
    unsigned char j;
  
    for(j=0 ; j<8 ; j++){
        ledCubeMatrix[layer][j][0] = value;
    }
}

void showLedCubeMatrixLayer(){
    unsigned char col;
    static unsigned char layer = 0;
  
    /*Disable Layers*/
    digitalWrite(PB_0,  LOW);
    digitalWrite(PB_1,  LOW);
    digitalWrite(PB_2,  LOW);
    digitalWrite(PB_3,  LOW);
    digitalWrite(PB_4,  LOW);
    digitalWrite(PB_5,  LOW);
    digitalWrite(PB_6,  LOW);
    digitalWrite(PB_7,  LOW); 
  
    setOutputColumn(1); /*Disable output columns*/
  
    for(col=0 ; col<8 ; col++){
        
        digitalWrite(PC_7, ((ledCubeMatrix[layer][col][0] & 128) == 128) ? HIGH : LOW);
        digitalWrite(PC_6, ((ledCubeMatrix[layer][col][0] & 64) == 64) ? HIGH : LOW);
        digitalWrite(PC_5, ((ledCubeMatrix[layer][col][0] & 32) == 32) ? HIGH : LOW);
        digitalWrite(PC_4, ((ledCubeMatrix[layer][col][0] & 16) == 16) ? HIGH : LOW);
        digitalWrite(PA_7, ((ledCubeMatrix[layer][col][0] & 8) == 8) ? HIGH : LOW);
        digitalWrite(PA_6, ((ledCubeMatrix[layer][col][0] & 4) == 4)  ? HIGH : LOW);
        digitalWrite(PA_5, ((ledCubeMatrix[layer][col][0] & 2) == 2) ? HIGH : LOW);
        digitalWrite(PA_4, ((ledCubeMatrix[layer][col][0] & 1) == 1) ? HIGH : LOW);
        
        
        // Select column set
        selectColumn(col);
  }
 
    // Select matrix
    setOutputColumn(0);/*Enable output*/
    
    setLayers(layer); /* From top to down*/
    
    layer = (layer+1) % 8;
}

void setLayers(unsigned char layer){
    digitalWrite(PB_0, (layer == 7) ? HIGH : LOW);
    digitalWrite(PB_1, (layer == 6) ? HIGH : LOW);
    digitalWrite(PB_2, (layer == 5) ? HIGH : LOW);
    digitalWrite(PB_3, (layer == 4) ? HIGH : LOW);
    digitalWrite(PB_4, (layer == 3) ? HIGH : LOW);
    digitalWrite(PB_5, (layer == 2) ? HIGH : LOW);
    digitalWrite(PB_6, (layer == 1) ? HIGH : LOW);
    digitalWrite(PB_7, (layer == 0) ? HIGH : LOW);
}

void setOutputColumn(unsigned char set){
    digitalWrite(PE_3, set ? HIGH : LOW);
}

void selectColumn(unsigned char col){
    digitalWrite(PE_0, col%2 ); 
    col /= 2;
    digitalWrite(PE_1, col%2 ); 
    col /= 2;
    digitalWrite(PE_2, col%2 );    
}
