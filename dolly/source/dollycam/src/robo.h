#ifndef _ROBO_H_
#define _ROBO_H_

#include <string>
#include <sys/time.h>

class CRobo {
private: // private data members
    std::string portname_;
	char commandHorizontal_;
	char commandVertical_;
	char commandMotor1_;
	char commandMotor2_;
	char commandMotor3_;
	char commandMotor4_;
	int fd_; // file descriptor
	struct timeval lt_;
public: // public contructors
	CRobo(void);
	~CRobo();

public: // public methods
int ServoHorizontal(int position);
int ServoVertical(int position);
int Motor1(int position);
int Motor2(int position);
int Motor3(int position);
int Motor4(int position);

private: // private methods
	int set_interface_attribs (int fd, int speed, int parity);
	void set_blocking (int fd, int should_block);
	void Write(char command, int position);
	bool CheckTime();
};

#endif
