/*
 * main.c
 */
#include "rs232.h"
#ifdef _WIN32
#include <windows.h>
#elif __linux
#include <unistd.h> // usleep
#endif  

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "rs232.h"
#define BAUDRATE 115200
#define DELAY_TIME_US 100000
#define RECONNECT_TIME 100000
#define BUFFER_SIZE 4096

int connectLedCube();
int sendArrayLedCube(int portNo, unsigned char ledArray[8][8][1]);

int main(void) {
	unsigned char ledArray[8][8][1];
    unsigned char i, j, k =0, layer;
	int portNo;
	int status; 
	srand(time(NULL));

	while( (portNo = connectLedCube()) < 0){
		printf("Cannot open any port!\n");
		
		#ifdef _WIN32
			Sleep(1000);
		#elif __linux
			usleep(1000000);
		#endif 
	}

	// Clear array
	for (i = 0; i < 8; ++i) {
		for (j = 0; j < 8; ++j) {
			ledArray[i][j][0]= 0;
		}
	}

    for(;;){
		
		// Shift the layers
		for (i = 7; i > 0 ; --i) {
			for (j = 0; j < 8; ++j) {
				ledArray[i][j][0] = ledArray[i-1][j][0];
			}
		}

		// Generate random numbers to light up 
		for(i=0; i<8; ++i){
			ledArray[0][i][0] = rand() % 255;
			ledArray[0][i][0] &= ~(rand() % 255);
			ledArray[0][i][0] &= ~(rand() % 255);
			ledArray[0][i][0] &= ~(rand() % 255);
		}

		status = sendArrayLedCube(portNo, ledArray);

		usleep(80000);


		if(status == -1)
			return -1;
	}

	CloseComport(portNo);

	printf("Program finished...\n");

	#ifdef _WIN32
	system("PAUSE");
	#endif
    
    return 0;
}
int connectLedCube(){
	char buffer[BUFFER_SIZE];
	int portNum;
	int openedFlag = 0;

	for(portNum = 0; portNum < 6 ; ++portNum){
		if (OpenComport(portNum, BAUDRATE) == 0){
			printf("Port ttyACM%d opened succesfully\n", portNum);
			return portNum;
		}
	}
	
	return -1;
}

int sendArrayLedCube(int portNo, unsigned char ledArray[8][8][1]){

	int i, j;

	
	//send terminate character
	if (SendByte(portNo, '+') == 1){
		perror("SendByte");
	}else{
		printf("start gönderildi\n");
	}
	
	//usleep(100000);
	

	for (i = 0; i < 8; ++i) {
		for (j = 0; j < 8; ++j) {

			if (SendByte(portNo, ledArray[i][j][0]) == 1){
				perror("SendByte");
				
				#ifdef _WIN32
				system("PAUSE");
				#endif			
				
				return -1;
			}
		//usleep(100);	
		}
	}

	
	

	// send terminate character
	if (SendByte(portNo, '-') == 1){
		perror("SendByte");
	}else{
		printf("terminate gönderildi\n");
	}
	
	//usleep(100000);
	

	return 1;
}