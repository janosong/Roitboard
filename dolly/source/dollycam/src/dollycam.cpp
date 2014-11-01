#include <stdio.h>
#include "camera.h"

CCamera *pCamera;
static bool run = true;
void quitHandler(const boost::system::error_code& error, int signal_number)
{
	if (!error)
	{
		run = false;
	}
	
}

int
main(int argc, char *argv[])
{
	printf("DBG001: New Camera\n");
	CCamera *pCamera = new CCamera();
	if (!pCamera)
	{
		printf("Not enough memory!\n");
		return -1;
	}
	printf("DBG002: Camera->Initialize()\n");
	if (!pCamera->Initialize())
	{
		printf("Camera initialization failed!\n");
		delete pCamera;
		return -1;
	}
	printf("initialize end\n");
	/* install quit handler */
	boost::asio::io_service io_service;
	boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
	signals.async_wait(quitHandler);
	io_service.run();
	
	pCamera->Run();
	while(run)
	{
		 boost::this_thread::yield();
	}
	pCamera->Quit();
	delete pCamera;
	return 0;
} 
