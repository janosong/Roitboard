#include "robo.h"

#include <iostream>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

CRobo::CRobo() : portname_("/dev/ttymxc2"), commandHorizontal_('\x1b'),
commandVertical_('\x1a'), commandMotor1_('\x0a'), commandMotor2_('\x0b'),
commandMotor3_('\x0c'), commandMotor4_('\x0d')
{
	std::cout << "OPEN PORT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    fd_ = open(portname_.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (fd_ < 0)
    {
            //error_message ("error %d opening %s: %s", errno, portname, strerror (errno));
            return;
    }

    set_interface_attribs (fd_, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
    set_blocking (fd_, 0);                // set no blocking
	gettimeofday(&lt_,NULL);
}

CRobo::~CRobo()
{
	std::cout << "CLOSE PORT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
	close(fd_);
}

int CRobo::set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                //error_message ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // ignore break signal
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5*40;            // 40*0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        //tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                //error_message ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void CRobo::set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                //error_message ("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 40*5;            // 40*0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
            return;
//                error_message ("error %d setting term attributes", errno);
}

bool CRobo::CheckTime()
{
	struct timeval t;

	gettimeofday(&t,NULL);

	long unsigned diff = t.tv_usec-lt_.tv_usec;
	printf("TIME: %lu \n", diff);
	lt_ = t;
	if (diff <= 10000)
	{
		return false;
	}
	return true;
}

void CRobo::Write(char command, int position)
{
	//char val = position;
	char val = position; //120;
	std::cout << "Position " << position << " " << val << std::endl;
    write (fd_, &command, 1);
    write (fd_, &val, 1);
		//char val = (char)(((int)'0')+90);
	/*
	char valy = 80;
	char y = '\x31';
    write (fd, &y, 1);
    write (fd, &valy , 1);
    char buf [100];
	*/
}

int CRobo::ServoHorizontal(int position)
{
	if (CheckTime())
	{
		Write(commandHorizontal_, position);
	}
	return 1;
}

int CRobo::ServoVertical(int position)
{
	if (CheckTime())
	{
		Write(commandVertical_, position);
	}
	return 1;
} 

int CRobo::Motor1(int position)
{
	Write(commandMotor1_, position);
	return 1;
} 

int CRobo::Motor2(int position)
{
	Write(commandMotor2_, position);
	return 1;
}

int CRobo::Motor3(int position)
{
	Write(commandMotor3_, position);
	return 1;
} 

int CRobo::Motor4(int position)
{
	Write(commandMotor4_, position);
	return 1;
}
