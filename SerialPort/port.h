//
//  port.h
//  SerialPort
//
//  Created by Wade on 9/14/18.
//  Copyright © 2018 Lanto_EE. All rights reserved.
//

#ifndef port_h
#define port_h

#define TRUE 1
#define FALSE 0

int analysis(char *buff);
int OpenDev(char *Dev);
void set_speed(int port, int speed);
int set_Parity(int port,int databits,int stopbits,int parity);

int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300, B38400, B19200, B9600, B4800, B2400, B1200, B300, };
int name_arr[] = {38400, 19200, 9600, 4800, 2400, 1200, 300, 38400, 19200, 9600, 4800, 2400, 1200, 300, };

#endif /* port_h */
