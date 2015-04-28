  /*
  int A7 = 10; // 7.sütun  
  int A6 = 9;  // 6.sütun
  int A5 = 8;  // 5.sütun
  int A4 = 7;  // 4.sütun
  int C7 = 34; // 3.sütun
  int C6 = 35; // 2.sütun
  int C5 = 36; // 1.sütun
  int C4 = 37; // 0.sütun
  
  int E0 = 18; // 0. satır seçici
  int E1 = 27; // 1. satır seçici
  int E2 = 28; // 2. satır seçici
  
  int E3 = 19; // Enable/disable output
 
  int B7 = 15; // 0. layer
  int B6 = 14; // 1. layer
  int B5 = 2;  // 2. layer
  int B4 = 7;  // 3. layer
  int B3 = 38; // 4. layer
  int B2 = 19; // 5. layer
  int B1 = 4;  // 6. layer
  int B0 = 3;  // 7. layer
  */
  // ledCubeMatrix[layer][satır][sütun]
  unsigned char ledCubeMatrix[8][8][8];
  unsigned char incoming;
  
void setup()
{
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
  
  initLedCube(1);
  
}

void loop()
{
    //showLedCubeMatrixLayer();
    delay(1000);
    
    //if(Serial.available() > 0){
      incoming = Serial.read();
      Serial.print("I received: ");
      Serial.println(incoming, DEC);
    //}
    
}

void initLedCube(unsigned char value){
  
  unsigned char i;

  for(i=0 ; i<8 ; i++){
    initLayer(i,value);
  }
  
}

void initLayer(unsigned char layer, unsigned char value){
  unsigned char j,k;

  for(j=0 ; j<8 ; j++){
    for(k=0 ; k<8 ; k++){
      ledCubeMatrix[layer][j][k] = value;
    }
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
  
        digitalWrite(PA_7, ledCubeMatrix[layer][col][7] ? HIGH : LOW);
        digitalWrite(PA_6, ledCubeMatrix[layer][col][6] ? HIGH : LOW);
        digitalWrite(PA_5, ledCubeMatrix[layer][col][5] ? HIGH : LOW);
        digitalWrite(PA_4, ledCubeMatrix[layer][col][4] ? HIGH : LOW);
        digitalWrite(PC_7, ledCubeMatrix[layer][col][3] ? HIGH : LOW);
        digitalWrite(PC_6, ledCubeMatrix[layer][col][2] ? HIGH : LOW);
        digitalWrite(PC_5, ledCubeMatrix[layer][col][1] ? HIGH : LOW);
        digitalWrite(PC_4, ledCubeMatrix[layer][col][0] ? HIGH : LOW);
      	
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
/*
void initTimer() { 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); 
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); 
    unsigned long ulPeriod = (SysCtlClockGet() / 1000); 
    TimerLoadSet(TIMER0_BASE, TIMER_A, ulPeriod -1); 
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT); 
    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0IntHandler); 
    TimerEnable(TIMER0_BASE, TIMER_A); 
    IntMasterEnable();
}

void Timer0IntHandler() { 
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT); 
    showLedCubeMatrixLayer();
}*/
