/* 
 * File:   rs232.h
 * Author: Hakan
 *
 * Created on 22 Nisan 2015 Çarşamba, 12:50
 */

#ifndef RS232_H
#define	RS232_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>



#ifdef __linux__

#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#else

#include <windows.h>

#endif

int OpenComport(int, int);
int PollComport(int,  char *, int);
int SendByte(int,  char);
int SendBuf(int, char *, int);
void CloseComport(int);
void cprintf(int, const char *);
int IsCTSEnabled(int);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif	/* RS232_H */

