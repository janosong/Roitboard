#ifndef _WEBSERVICE_H_
#define _WEBSERVICE_H_

#include "soapH.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include "camera.h"
#include "robo.h"

#define COMMAND_SET_FOCUS 0x80  // 128
#define COMMAND_SET_WB 0x81     // 129
#define COMMAND_SET_TV 0x82     // 130
#define COMMAND_SET_AV 0x83     // 131
#define COMMAND_SET_EXPCOMP 0x84  // 132
#define COMMAND_SET_ISO 0x85  // 133
#define COMMAND_SET_IMGFORMAT 0x86  // 134
#define COMMAND_SET_DOLLY 0x87  // 135
#define COMMAND_IMG_CAPTURE 0x88  // 136
#define COMMAND_SET_SHUTTER 0x89  // 137
#define COMMAND_SET_APERTURE 0x8A  // 138
#define COMMAND_SERVO_HORIZONTAL 0x1b
#define COMMAND_SERVO_VERTICAL 0x1a
#define COMMAND_MOTOR_1 0x0a
#define COMMAND_MOTOR_2 0x0b
#define COMMAND_MOTOR_3 0x0c
#define COMMAND_MOTOR_4 0x0d

class CCamera;

class CWebService{
	
public:
	/* constructor */
	CWebService(CCamera *camera);
	/* destructor */
	~CWebService(void);
	bool Initialize();
	void Run();
	void Quit();
	
	typedef struct _WEBSERVICE_REQUEST
	{
		struct soap *soap;
		boost::thread *thisThread;
		char *buffer;
		CCamera *pCamera;
		CRobo *pRobo;
	}WEBSERVICE_REQUEST;
private:
	/* events structure */
	typedef struct _WEBSERVICE_EVENT
	{
		bool quit;
		boost::condition_variable condition;
		boost::mutex mutex;
	}WEBSERVICE_EVENT, *PWEBSERVICE_EVENT;
	
	
	
	void RunProc(void);
	void ProcessRequest(WEBSERVICE_REQUEST *reguest);
	WEBSERVICE_EVENT *pEvents;
	boost::thread *pWebServiceThread;
	SOAP_SOCKET soapMasterSocket;
	CCamera *pCamera;
	CRobo *pRobo;
};

#endif // _WEBSERVICE_H_
