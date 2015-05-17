 #include "Energia.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"

#define ALL_GPIO_PINS    (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)
#define MS_4_GPIO_PINS   (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)
#define LS_4_GPIO_PINS   (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)

  // ledCubeMatrix[layer][satır][sütun]
  unsigned char ledCubeMatrix[8][8];
  unsigned char d1,d2;
  unsigned char isMaster;
  
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
  //digitalWrite(RED_LED, digitalRead(RED_LED) ^ 1);     // toggle LED pin
  showLedCubeMatrixLayer();
}

void setup()
{
  
  initTimer();
  
  unsigned long ulPeriod;
  unsigned int Hz = 2000;   // frequency in Hz  
  ulPeriod = (SysCtlClockGet() / Hz)/ 2;
  ROM_TimerLoadSet(TIMER0_BASE, TIMER_A,ulPeriod -1);
  
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
  
  HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
  HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= 0x80;
  HWREG(GPIO_PORTD_BASE + GPIO_O_AFSEL) &= ~0x80;
  HWREG(GPIO_PORTD_BASE + GPIO_O_DEN) |= 0x80;
  HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;
  
  /*Communication with PC*/
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
  UARTEnable(UART0_BASE);
  
  /*Communication with other cubes*/
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
  GPIOPinConfigure(GPIO_PD6_U2RX);
  GPIOPinConfigure(GPIO_PD7_U2TX);
  GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);
  UARTConfigSetExpClk(UART2_BASE, SysCtlClockGet(), 115200, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
  UARTEnable(UART2_BASE);
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);
  GPIOPinConfigure(GPIO_PE4_U5RX);
  GPIOPinConfigure(GPIO_PE5_U5TX);
  GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);
  UARTConfigSetExpClk(UART5_BASE, SysCtlClockGet(), 115200, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
  UARTEnable(UART5_BASE);
  
  initLedCube(0x00);
  //ledCubeMatrix[0][0] = 0;
}

void loop()
{   
    while(1){
        d1 = getDataFromSerial();
        if(d1 == 0xFF){ // reset
          SysCtlDelay(SysCtlClockGet() / 600); // wait 5 ms
            if(UARTCharsAvail(UART0_BASE))
              UARTCharGet(UART0_BASE);
            else if(UARTCharsAvail(UART5_BASE))
              UARTCharGet(UART5_BASE);
              initLedCube(0x00);
              if(isMaster){
                UARTCharPut(UART2_BASE, 0xFF);
        	UARTCharPut(UART2_BASE, 0xFF);
        	UARTCharPut(UART5_BASE, 0xFF);
        	UARTCharPut(UART5_BASE, 0xFF);
        	SysCtlDelay(SysCtlClockGet() / 600); // wait 5 ms
              }
            continue;
        }
        d2 = getDataFromSerial();
        setOrSendDataToCube(d1,d2);
    }//outer while
}

unsigned char getDataFromSerial(){
	while(1){
		//Master
		if(UARTCharsAvail(UART0_BASE)){
		  isMaster = 1;
		  return UARTCharGet(UART0_BASE);
		}

		//Slave
		if(UARTCharsAvail(UART5_BASE)){
		  isMaster = 0;
		  return UARTCharGet(UART5_BASE);
		}
	}
}

void setCube(unsigned char d1, unsigned char d2){
	if(d1%32 > 15){ /* Data to third cube*/
	  UARTCharPut(UART2_BASE, d1 - 16);
	  UARTCharPut(UART2_BASE, d2);
	}
	else if(d1%32 > 7){ /* Data to second cube*/
	  UARTCharPut(UART5_BASE, d1 - 8);
	  UARTCharPut(UART5_BASE, d2);
	}
	else{ /* To our cube */
	  ledCubeMatrix[d1/32][d1%32] = d2;
	}
}

void getData(){
  		
  //if(Serial.available()){		
      //data = Serial.read();             		
  //}        		
  //Serial        	               		
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
        ledCubeMatrix[layer][j] = value;
    }
}

void showLedCubeMatrixLayer(){
    
  unsigned char col;
  static unsigned char layer = 0;
    
  GPIOPinWrite(GPIO_PORTB_BASE, ALL_GPIO_PINS, 0); /*Disable transistor array*/

  GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, GPIO_PIN_3); /*Disable output*/
  
    
    for(col=0 ; col<8 ; col++){
        
        digitalWrite(PA_7, ((ledCubeMatrix[layer][7-col] & 128) == 128) ? HIGH : LOW);
        digitalWrite(PA_6, ((ledCubeMatrix[layer][7-col] & 64) == 64) ? HIGH : LOW);
        digitalWrite(PA_5, ((ledCubeMatrix[layer][7-col] & 32) == 32) ? HIGH : LOW);
        digitalWrite(PA_4, ((ledCubeMatrix[layer][7-col] & 16) == 16) ? HIGH : LOW);
        digitalWrite(PC_4, ((ledCubeMatrix[layer][7-col] & 8) == 8) ? HIGH : LOW);
        digitalWrite(PC_5, ((ledCubeMatrix[layer][7-col] & 4) == 4)  ? HIGH : LOW);
        digitalWrite(PC_6, ((ledCubeMatrix[layer][7-col] & 2) == 2) ? HIGH : LOW);
        digitalWrite(PC_7, ((ledCubeMatrix[layer][7-col] & 1) == 1) ? HIGH : LOW);
              	
        // Select column set
        GPIOPinWrite(GPIO_PORTE_BASE, (GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2) , col);
    }
 
    // Select matrix
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, 0);/*Enable output*/
    
    GPIOPinWrite(GPIO_PORTB_BASE, ALL_GPIO_PINS, GPIO_PIN_0 << layer); /* From top to down*/
    
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
    digitalWrite(PE_0, (col%2 == 1) ? HIGH : LOW ); 
    col /= 2;
    digitalWrite(PE_1, (col%2 == 1) ? HIGH : LOW ); 
    col /= 2;
    digitalWrite(PE_2, (col%2 == 1) ? HIGH : LOW );    
}

