/*
 * main.c
 */
#include "rs232.h"
#if defined (_WIN32) || defined( _WIN64)
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

int connect_ledcube();
int connect_manuel_ledcube(int);
int connect_auto_ledcube();
int sendArrayLedCube(int portNo, unsigned char ledArray[8][8]);
void show_rain(unsigned char ledArray[8][8]);

int main(void) {
	unsigned char ledArray[8][8];
    unsigned char i, j, k =0, layer;
	int portNo;
	int status; 
	srand(time(NULL));


	// Connect to led cube
	portNo = connect_ledcube();
	
	// Clear array
	for (i = 0; i < 8; ++i) {
		for (j = 0; j < 8; ++j) {
			ledArray[i][j]= 0;
		}
	}
	
	//ledArray[dikey][z ekseni] = [yatay]
	
	ledArray[0][0] = 255;

    for(;;){
		
		// Yağmur animasyonu
		//show_rain(ledArray);

    	// Arrayin gönderildiği yer
		status = sendArrayLedCube(portNo, ledArray);

		usleep(100000);

		// Array gönderme hatası
		if(status == -1){
			printf("Array not sent!\n");

			CloseComport(portNo);

			#if defined (_WIN32) || defined( _WIN64)
				system("PAUSE");
			#endif
			
			return -1;
		}else
			printf("Array gönderildi %d\n", ledArray[0][0]);

	}

	CloseComport(portNo);

	printf("Program finished...\n");

	#if defined (_WIN32) || defined ( _WIN64)
	system("PAUSE");
	#endif
    
    return 0;
}

void show_rain(unsigned char ledArray[8][8]){
	int i, j;

		// Shift the layers
		for (i = 7; i > 0 ; --i) {
			for (j = 0; j < 8; ++j) {
				ledArray[i][j] = ledArray[i-1][j];
			}
		}
		
		// Generate random numbers to light up 
		for(i=0; i<8; ++i){
			ledArray[0][i] = rand() % 255;
			ledArray[0][i] &= ~(rand() % 255);
			ledArray[0][i] &= ~(rand() % 255);
			ledArray[0][i] &= ~(rand() % 255);
		}
}

int connect_ledcube(){

	int select;
	int flag = 1;
	int portNo;

	printf("Select connection type:\n");
	printf("1: Automatic connection\n");
	printf("2: Manuel connection\n\n");

	while(flag){
		printf("select: ");
		scanf("%d", &select);
		
		// Auto connection part
		if(select == 1){
			flag = 0;
			while( (portNo = connect_auto_ledcube()) < 0){
				printf("Cannot open any port!\n");
				
				#if defined (_WIN32) || defined( _WIN64)
					Sleep(1000);
				#elif __linux
					usleep(1000000);
				#endif 
			}

			return portNo;
		}
		
		// Manuel connection part
		else if(select == 2){
			flag = 0;
			printf("Enter port number: \n");
			scanf("%d", &portNo);

			portNo = connect_manuel_ledcube(portNo);
			
			if(portNo != -1)
				return portNo;
		}

		else{
			printf("Wrong selection! Try again...\n\n");
		}
	}
	
	// Any connection failed
	return -1;
}

int connect_auto_ledcube(){
	char buffer[BUFFER_SIZE];
	int portNum;
	int openedFlag = 0;

	for(portNum = 0; portNum < 6 ; ++portNum){
		if (OpenComport(portNum, BAUDRATE) == 0){
			
			#if defined (_WIN32) || defined( _WIN64)
				printf("COM%d opened succesfully\n", portNum+1);
			#elif __linux
				printf("ttyACM%d opened succesfully\n", portNum);
			#endif  
			
			return portNum;
		}
	}
	
	return -1;
}

int connect_manuel_ledcube(int portNum){

	if (OpenComport(portNum, BAUDRATE) == 0){
		printf("Port ttyACM%d opened succesfully\n", portNum);
		return portNum;
	}else{
		perror("Connection Failed: ");

		#if defined (_WIN32) || defined( _WIN64)
		system("PAUSE");
		#endif

		return -1;
	}

}

int sendArrayLedCube(int portNo, unsigned char ledArray[8][8]){

	unsigned char current[8][8] = { 0 };
	char wr[512];
	unsigned char length = 0;
	int i,j,k;
	int result;

	for ( i = 0; i < 8; i++){
		for ( j = 0; j < 8; j++){

			if (current[i][j] != ledArray[i][j]){
				wr[length++] = i * 32 + j;
				wr[length++] = ledArray[i][j];
				current[i][j] = ledArray[i][j];
			}
		}
	}
	
	result = SendBuf(portNo, wr, length);

	return result;
}