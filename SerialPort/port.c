//
//  port.c
//  SerialPort
//
//  Created by Wade on 9/14/18.
//  Copyright © 2018 Lanto_EE. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

#include "port.h"

//打开串口
int OpenDev(char *Dev)
{
    int fd = open(Dev,O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);//文件描述（//file description structure）
    if(fd == -1)
    {
        perror("Can't Open Serial Port\n");
        return -1;
    }
    else
    {
        printf("Open com success!\n");
        return fd;
    }
}

//设置波特率
void setBaudrate(int fd, int baudrate)
{
    int i;
//    int status;
    struct termios Opt;
    tcgetattr(fd, &Opt);
    for ( i= 0; i < sizeof(speed_arr) / sizeof(int); i++)
    {
        if (baudrate == name_arr[i])
        {
            //tcflush(fd, TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
//            status = tcsetattr(fd, TCSANOW, &Opt);
//            if (status != 0) perror("tcsetattr fd1");
//            return;
        }
        
        tcflush(fd,TCIOFLUSH);
    }
}

//设置数据位
void setDataBits(int fd,int databits)
{
    struct termios opt;
    tcgetattr(fd,&opt);
    
    switch (databits)
    {
        case 5:
            opt.c_cflag |= CS5;
            break;
        case 6:
            opt.c_cflag |= CS6;
            break;
        case 7:
            opt.c_cflag |= CS7;
            break;
        case 8:
            opt.c_cflag |= CS8;
            break;
        default:
            fprintf(stderr,"Unsupported data size\n");
            break;
    }
}

//设置校验位
void setParity(int fd, int parity)
{
    struct termios opt;
    tcgetattr(fd,&opt);
    
    switch (parity)
    {
        case 'n':
        case 'N':
            opt.c_cflag &= ~PARENB;
            opt.c_iflag &= ~INPCK;
            break;
        case 'o':
        case 'O':
            opt.c_cflag |= (PARODD | PARENB);
            opt.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'e':
        case 'E':
            opt.c_cflag |= PARENB;
            opt.c_cflag &= ~PARODD;
            opt.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'S':
        case 's':
            opt.c_cflag &= ~PARENB;
            opt.c_cflag &= ~CSTOPB;
            break;
        default:
            fprintf(stderr,"Unsupported parity\n");
            break;
    }
}

//设置停止位
void setStopBits(int fd, int stopbits)
{
    struct termios opt;
    tcgetattr(fd,&opt);
    
    switch (stopbits)
    {
        case 1:
            opt.c_cflag &= ~CSTOPB;
            break;
        case 2:
            opt.c_cflag |= CSTOPB;
            break;
        default:
            fprintf(stderr,"Unsupported stop bits\n");
            break;
    }
}



//设置数据位置，停止位，奇偶校验位
int set_Parity(int fd,int databits,int stopbits,int parity)
{
    struct termios options;
    
    if (tcgetattr(fd,&options) != 0)
    {
        perror("SetupSerial 1");
        return(FALSE);
    }
    
    bzero(&options,sizeof(options));
    options.c_cflag |= CLOCAL | CREAD;
    options.c_cflag &= ~CSIZE;
    switch (databits)
    {
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default: fprintf(stderr,"Unsupported data size\n");
            return (FALSE);
    }
    
    switch (parity)
    {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;
            options.c_iflag &= ~INPCK;
            break;
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            options.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'S':
        case 's':
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;
        default: fprintf(stderr,"Unsupported parity\n");
            return (FALSE);
    }
    
    switch (stopbits)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default: fprintf(stderr,"Unsupported stop bits\n");
            return (FALSE);
    }
    
    if (parity != 'n')
        options.c_iflag |= INPCK;
    
    options.c_cc[VTIME] = 0;
    options.c_cc[VMIN] = 0;
    tcflush(fd,TCIFLUSH);
    
    if (tcsetattr(fd,TCSANOW,&options) != 0)
    {
        perror("SetupSerial 3");
        return (FALSE);
    }
    
    return (TRUE);
}

int analysis (char *buff)
{
    int i;
    char *p;
    p=buff;
    for(i=0;i<255;i++)
    {
        printf("%c ",p[i]);
    }
    return 0;
}
              
int main(void)
{
    int fd;
    struct termios opt;
    long nwrite;
    long nread;
    char buff[255];//缓存，读取串口数据
    char *dev_name = "/dev/tty.usbserial-FTY3J9US";//根据实际情况选择串口
    
   
    do{
        fd = OpenDev(dev_name);
        if (fd>0)
        {
            tcgetattr(fd,&opt);
            
            opt.c_cc[VTIME] = 0;
            opt.c_cc[VMIN] = 1;
            opt.c_cflag |= (CLOCAL | CREAD);
            
            setBaudrate(fd,9600);
            setDataBits(fd,8);
            setParity(fd,'n');
            setStopBits(fd,1);
            
            if (tcsetattr(fd,TCSANOW,&opt) != 0)
            {
                perror("Open Serial");
                exit(1);
            }
            else
            {
                printf("Please write...\n");
                char*cmd = fgets(buff,255,stdin);
                strcat (cmd,"\r\n ");
                nwrite = write(fd,cmd,strlen(cmd));
            }
        }
        else
        {
            printf("Can't Open Serial Port!\n"); 
            sleep(1);//
            continue; 
        }
        
        break;
    } while (fd < 0);
    
//    if(set_Parity(fd,8,1,'N')==FALSE) //设置校验位 
//    {
//        printf("Set Parity Error\n"); 
//        exit(1);
//    }
    
    do
    { 
        sleep(3);
        memset(buff, 0, 255);//清空缓存
        nread = read(fd,buff,sizeof(buff));//返回读到的字节数
        if((nread>0))
        {		
            printf("Success!\n"); 
        }
        
        //analysis(buff);
        printf("%s",buff);
        
    }while(nread < 0);
}