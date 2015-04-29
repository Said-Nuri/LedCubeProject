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
#define BAUDRATE 19200
#define DELAY_TIME_US 100000
#define RECONNECT_TIME 100000
#define BUFFER_SIZE 4096

int connectLedCube();
int sendArrayLedCube(int portNo, unsigned char ledArray[8][8][1]);

int main(void) {
	unsigned char ledArray[8][8][1];
    unsigned char i, j, k, layer;
	int portNo;
	int status; 
	srand(time(NULL));

	while( (portNo = connectLedCube()) < 0){
		printf("Cannot open any port!\n");
		
		#ifdef _WIN32
			Sleep(10);
		#elif __linux
			usleep(10000);
		#endif 
	}

	for (i = 0; i < 8; ++i) {
		for (j = 0; j < 8; ++j) {
			ledArray[i][j][0]= 0;
		}
	}

    for(;;){
		
		for(i=0; i<8; ++i){
			ledArray[0][i][0] = 255;
		}

		/*Her satırı bir aşağı indir*/
		//for (layer = 0; layer < 7 ; ++layer) {
		//	ledArray[layer][j] = ledArray[layer+1][j];	
		//}

		status = sendArrayLedCube(portNo, ledArray);

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

	for (i = 0; i < 8; --i) {
		for (j = 0; j < 8; ++j) {

			if (SendByte(portNo, ledArray[i][j][0]) == 1){
				perror("SendByte");
				
				#ifdef _WIN32
				system("PAUSE");
				#endif			
				
				return -1;
			}
		}
	}

	return 1;
}