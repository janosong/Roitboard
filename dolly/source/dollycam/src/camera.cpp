#include "camera.h"
#include "webService.h"
#include <stdio.h>

#define PIPE_NAME "/tmp/myFIFO"

static int _gp_lookup_widget(CameraWidget*widget, const char *key, CameraWidget **child);
static int _gp_get_config_value_string(Camera *camera, const char *key, char **str, GPContext *context);
static void capture_to_memory(Camera *camera, GPContext *context, const char **ptr, unsigned long int *size);
static void capture_to_camera(Camera *camera, GPContext *context);
void capture_to_file(Camera *canon, GPContext *canoncontext, char *fn);

///
static GPPortInfoList		*portinfolist = NULL;
static CameraAbilitiesList	*abilities = NULL;
///


CCamera::CCamera(void)
{
	pCameraThread = NULL;
	pEvents = NULL;
	cameraState = CAMERA_NOT_PRESENT;
	pWebService = NULL;
	streamPipe = 0;
	cameraMode = MODE_UNKNOWN;
	afMode = false;
}

CCamera::~CCamera(void)
{
	if (pCameraThread)
	{
		delete pCameraThread;
	}
	
	if (pWebService)
	{
		delete pWebService;
	}
	
	if (pEvents)
	{
		delete pEvents;
	}
	if (streamPipe)
	{
		close(streamPipe);
	}
}

CCamera *CCamera::GetCamera()
{
	return this;
}

bool CCamera::Initialize(void)
{
	printf("DBG003: new CWebService()\n");
	pWebService = new CWebService(this);
	if (!pWebService)
	{
		return false;
	}

	printf("DBG004: config.Initialize()\n");
	
	if (!config.Initialize())
	{
		delete pWebService;
		pWebService = NULL;
		return false;	
	}

	printf("DBG005: new CCamera::CAMERA_EVENT()\n");	
	pEvents = new CCamera::CAMERA_EVENT();
	if (!pEvents)
	{
		delete pWebService;
		pWebService = NULL;
		return false;
	}

	printf("DBG006: Opening stream pipe\n");
	if ((streamPipe = open(PIPE_NAME, O_WRONLY)) < 0)
	{
		printf("error opening streaming pipe\n");
		delete pEvents;
		pEvents = NULL;
		return false;
	}
	
	printf("DBG007: pWebService->Initialize()\n");
	if (!pWebService->Initialize())
	{
		delete pWebService;
		
		pWebService = NULL;
		return false;
	}

	printf("DBG008: new pCameraThread\n");
	pCameraThread = new boost::thread(&CCamera::RunProc, this);
	if (!pCameraThread)
	{
		pWebService->Run();
		pWebService->Quit();
		
		delete pWebService;
		pWebService = NULL;
		return false;
	}
	
	captInProgress = false;
	
	return true;
}

void CCamera::Run(void)
{
	
	boost::bind(&CCamera::RunProc, pCameraThread);
	pWebService->Run();
}

void CCamera::Quit()
{
	pWebService->Quit();
	pEvents->quit = true;
	pEvents->condition.notify_one();
	pCameraThread->join();
}

void CCamera::RunProc()
{

  CameraList		*xlist = NULL;
  GPContext *pPresentContext;
  pPresentContext = gp_context_new();
  
  gp_list_new (&xlist);
  if (!portinfolist) {      		
    gp_port_info_list_new (&portinfolist);
    gp_port_info_list_load (portinfolist);
    gp_port_info_list_count (portinfolist);
  } 
  gp_abilities_list_new (&abilities);	
  gp_abilities_list_load (abilities, pContext);    	

  bool currentAF;  
  printf("mutex: %p\n", pEvents);
	boost::unique_lock<boost::mutex> lock(pEvents->eventMutex);
	
	pCamera = 0;
	pContext = 0;               
	
	cameraState = CAMERA_NOT_PRESENT;
	
	for(;;)
	{
		if (cameraState == CAMERA_NOT_PRESENT || cameraState == CAMERA_NOT_CONNECTED)
		{
			pEvents->condition.timed_wait(lock, boost::posix_time::milliseconds(500)); 			
		}
		/* block inside capturing image from camera */
		else
		{	
			pEvents->condition.timed_wait(lock, boost::posix_time::milliseconds(0));
		}
		
		if (pEvents->quit)
		{      
			printf("received quit event\n");
			break;
		}
		/* timeout */
		else
		{ 		  	
      gp_abilities_list_detect (abilities, portinfolist, xlist, pPresentContext);
      if(gp_list_count(xlist) == 0) {
        cameraState = CAMERA_NOT_CONNECTED;
        if (pCamera && pContext) {
          gp_camera_exit(pCamera, pContext);
        	gp_camera_unref(pCamera);
        	pCamera = 0;        
        }
        if (pContext) {
      	  gp_context_unref(pContext);
      	  //free(pContext);
      	  pContext = 0;
        }        
      }
      else {
        if (cameraState != CAMERA_INITIALIZED) {              
          cameraState = CAMERA_NOT_PRESENT;
        }
      }
                      

			switch (cameraState)
			{
				case CAMERA_NOT_PRESENT:
				{                               
				
				  if(pCamera == 0) {
				    gp_camera_new (&pCamera);
				  }
				  if(pContext == 0) {
            pContext = gp_context_new();
	        }
					int result = gp_camera_init(pCamera, pContext);          
					
					if (result == GP_OK)
					{
						printf("Camera initialized.\n");
						cameraState = CAMERA_INITIALIZED;
						CaptureTargetOn();
						LiveViewStart();
					}
					else
					{
						printf("camera not installed result=0x%X\n", result);   						
					}
					break;
				}
				case CAMERA_NOT_CONNECTED:
				{
				  printf("Camera is not connected to the USB port.\n");
				  break;
        }				
				default:
				{
					cameraMutex.lock();
					CAMERA_MODE currentMode = GetCameraMode();
					if (currentMode != cameraMode)
					{
						cameraMode = currentMode;
						config.SetCameraMode(cameraMode);
            if (currentMode != MODE_UNKNOWN) {
  						UpdateIso();
              UpdateWb();
              UpdateSh();
              UpdateAp();
              UpdateEx();
              UpdateIf();
              UpdateCi();					
						}
						else {
						  config.SetIsoMode(FALSE);
						  config.SetWbMode(FALSE);
						  config.SetShMode(FALSE);
						  config.SetApMode(FALSE);
						  config.SetExMode(FALSE);						  						  
            }
            config.Invalidate();
					}
					else
					{
						//UpdateIso();
            //UpdateWb();
            //UpdateSh();
            //UpdateAp();
            //UpdateEx();
            //UpdateIf();
            //UpdateCi();
            UpdateBt();
					}
					currentAF = GetFocusMode();
          //printf("== %d ==\n", currentAF);          
					if (currentAF != afMode)
					{                          
						afMode = currentAF;
						config.SetAfMode(afMode);
						config.Invalidate();
					}
					cameraMutex.unlock();
					if (cameraMode != MODE_UNKNOWN)
					{             
						//printf("before live view %d\n", cameraMode);
						LiveViewStreamImage();
						//printf("after live view\n");
            
					}
					else
					{
						printf("unknown camera mode\n");
					}
					
					break;
				}
			}
		
		}
	}
	
}

bool CCamera::LiveViewStart()
{
	CameraWidget *widget = NULL, *child = NULL;
	CameraWidgetType type;
	/* check camera state */
	int ret;

	ret = gp_camera_get_config(pCamera, &widget, pContext);
	if (ret >= GP_OK)
	{
		ret = _gp_lookup_widget(widget, "capture", &child);
	}
	if (ret >= GP_OK)
	{
		ret = gp_widget_get_type(child, &type);
	}
	if (ret >= GP_OK)
	{
		if (type != GP_WIDGET_TOGGLE)
		{
			ret = GP_ERROR_BAD_PARAMETERS;
			}
	}
	if (ret >= GP_OK)
	{
		/* Now set the toggle to the wanted value */
		int onoff = 1;
		ret = gp_widget_set_value (child, &onoff);
	}
	if (ret >= GP_OK)
	{
		ret = gp_camera_set_config(pCamera, widget, pContext);
	}
	if (ret >= GP_OK)
	{
//		LiveViewStarted = true;
	}
	if (widget)
	{
		gp_widget_free (widget);
	}
	return ret >= GP_OK;
}

bool CCamera::LiveViewStreamImage()
{

	CameraFile *gpfile = 0;
	const char* ptr = 0;
	unsigned long int data_size = 0;
	
	cameraMutex.lock();
	int ret = gp_file_new(&gpfile);
	if (ret == GP_OK)
	{     
		ret = gp_camera_capture_preview(pCamera, gpfile, pContext);    
	}
	if (ret == GP_OK)
	{
		// owner of 'data' is gpfile
		ret = gp_file_get_data_and_size(gpfile, &ptr, &data_size);
	}
	if (ret == GP_OK)
	{
		int wr = write(streamPipe, ptr, data_size);
		printf("stream capture %lu  %d\n", data_size, wr);
	}
	/* free camera file */
	if (gpfile)
	{
		gp_file_free(gpfile);
	}
	cameraMutex.unlock();
	return ret >= GP_OK;
}

bool CCamera::LiveViewStop()
{
}

bool CCamera::PrintWidget()
{
	bool result = false;
	int ret = -1;
	CameraWidget* widget = 0;
	CameraWidget* child = 0;
	CameraWidget* child2 = 0;
	CameraWidget* settings = 0;
	const char *val;
	
	
	ret = gp_camera_get_config(pCamera, &widget, pContext);
	if (ret >= GP_OK)
	{
		int count = gp_widget_count_children(widget);
		
		for(int i = 0; i < count; i++)
		{
			gp_widget_get_child(widget, i, &settings);
			int count2 = gp_widget_count_children(settings);
			gp_widget_get_name(settings, &val);
			printf("child %d:%s\"\n", i, val);
			gp_widget_get_label(settings, &val);
			printf("child %d:%s\"\n", i, val);
			for (int j = 0; j < count2; j++)
			{
				gp_widget_get_child(settings, j, &child);
				gp_widget_get_name(child, &val);
				printf("child \t%d:%s\"\n", j, val);
				gp_widget_get_label(child, &val);
				printf("child \t%d:%s\"\n", j, val);
				
			/*	int pom = gp_widget_get_value(child, &val);
				if (pom >= 0 )
				printf("child \t%d:%s\"\n", j, val);*/
			}
		}
		printf("\n");
	}
	return result;
}

bool CCamera::GetFocusMode()
{ 
	int ret = -1;
	char *value = NULL;
	CameraWidget* widget = 0;
	CameraWidget* child = 0;
	
	// TODO: find out why is not possible to get focusmode when camera starts with MF
	//return true;
	
	ret = gp_camera_get_config(pCamera, &widget, pContext);
	if (ret >= GP_OK && widget)
	{
		ret = _gp_lookup_widget(widget, "focusmode", &child);
		//printf("== %d ==\n", ret);
		if (ret >= GP_OK && child)
		{
			ret = gp_widget_get_value(child, &value);			
			if (ret >= GP_OK && value)
			{		
				if (strcmp("One Shot", value) == 0)
				{
					gp_widget_free(widget);
					return true;
				}
			}
		}
	}
	
	if (widget)
	{
		gp_widget_free(widget);
	}
	
	return false;
	
}

CAMERA_MODE CCamera::GetCameraMode()
{
	int ret = -1;
	char *value = NULL;
	CameraWidget* widget = 0;
	CameraWidget* child = 0;
	
	ret = gp_camera_get_config(pCamera, &widget, pContext);
	if (ret >= GP_OK && widget)
	{
		ret = _gp_lookup_widget(widget, "autoexposuremode", &child);
		if (ret >= GP_OK && child)
		{
			ret = gp_widget_get_value(child, &value);
			if (ret >= GP_OK && value)
			{
				if (strcmp("AV", value) == 0)
				{
					gp_widget_free(widget);
					return MODE_AV;
				}
				if (strcmp("TV", value) == 0)
				{	
					gp_widget_free(widget);
					return MODE_TV;
				}
				if (strcmp("Manual", value) == 0)
				{
					gp_widget_free(widget);
					return MODE_MANUAL;
				}
				if (strcmp("P", value) == 0)
				{
					gp_widget_free(widget);
					return MODE_P;
				}
				if (strcmp("A_DEP", value) == 0)
				{
					gp_widget_free(widget);
					return MODE_ADEP;
				}
			}
		}
	}

	if (widget)
	{
		gp_widget_free(widget);
	}
	
	return MODE_UNKNOWN;
}

// Capture Image ///////////////////////////////////////////////////////////////

void CCamera::CaptureThread(void) {
  cameraMutex.lock();
  if (captInProgress == false) {
    captInProgress = true;
    //capture_to_file(pCamera, pContext, (char *) "/tmp/foo.jpg");
    capture_to_camera(pCamera, pContext);    
    captInProgress = false;
  }
  cameraMutex.unlock();
}


int CCamera::CaptureImage(void) {                 
  //boost::thread workerThread(captureThread);
  pCaptThread = new boost::thread(&CCamera::CaptureThread, this);
  pCaptThread->detach();  
	return 0;
}


// Manual focus ////////////////////////////////////////////////////////////////
int CCamera::SetManualFocus(int position) {

  const char* focus_dir = 0;
  
	switch (position)
	{
  	case 0:
	   	focus_dir = "Near 3";
			break;
		case 1:
			focus_dir = "Near 2";
			break;
		case 2:
			focus_dir = "Near 1";
			break;
		case 5:
			focus_dir = "Far 1";
			break;
		case 6:
			focus_dir = "Far 2";
			break;
		case 7:
			focus_dir = "Far 3";            			
			break;
	}

  if (focus_dir) {
    cameraMutex.lock();
  	CameraWidget* widget = 0;
  	CameraWidget* child = 0;
  	int ret;
  	
  	ret = gp_camera_get_config(pCamera, &widget, pContext);
  				printf("set gp_camera_get_config %d\n", ret);
  	if (ret >= GP_OK && widget)
  	{
  		ret = _gp_lookup_widget(widget, "manualfocusdrive", &child);
  				printf("set _gp_lookup_widget %d\n", ret);
  		if (ret >= GP_OK && child)
  		{
  			ret = gp_widget_set_value(child, focus_dir);
  				printf("set gp_widget_set_value %d\n", ret);
  			if (ret >= GP_OK)
  			{
  				ret = gp_camera_set_config(pCamera, widget, pContext);
  				printf("set gp_camera_set_config %d\n", ret);
  			}
  		}
  	}
  	if (widget)
  	{
  		gp_widget_free (widget);
  	}
  	cameraMutex.unlock();
  }
}
// Battery level ///////////////////////////////////////////////////////////////
void CCamera::UpdateBt()
{

	int ret = GP_OK;
	CameraWidget* widget = 0;
	CameraWidget* child = 0;
	char* choice = 0;
	char buffer[256] = {0,};

 	int WbListSize = 0;
		
	ret = gp_camera_get_config(pCamera, &widget, pContext);
	if (ret >= GP_OK)
	  ret = _gp_lookup_widget(widget, "batterylevel", &child);
	if (ret >= GP_OK)
    ret = gp_widget_get_value(child, &choice);  
  if (ret >= GP_OK)
    sprintf(buffer, "Camera model: %s", choice);
    config.ChangeBtLabel(choice);  
}
// Camera information //////////////////////////////////////////////////////////
void CCamera::UpdateCi()
{

	int ret = GP_OK;
	CameraWidget* widget = 0;
	CameraWidget* child = 0;
	char* choice = 0;
	char buffer[256] = {0,};
   			
	config.RemoveCiValues();
	ret = gp_camera_get_config(pCamera, &widget, pContext);
	if (ret >= GP_OK)
	  ret = _gp_lookup_widget(widget, "d402", &child);
	if (ret >= GP_OK)
    ret = gp_widget_get_value(child, &choice);  
  if (ret >= GP_OK)
    sprintf(buffer, "Camera model: %s", choice);
    config.AddCiMode(0, buffer);
  if (ret >= GP_OK)
	  ret = _gp_lookup_widget(widget, "serialnumber", &child);
	if (ret >= GP_OK)
    ret = gp_widget_get_value(child, &choice);  
  if (ret >= GP_OK)
    sprintf(buffer, "Serial number: %s", choice);
    config.AddCiMode(1, buffer);
  if (ret >= GP_OK)
	  ret = _gp_lookup_widget(widget, "shuttercounter", &child);
	if (ret >= GP_OK)
    ret = gp_widget_get_value(child, &choice);  
  if (ret >= GP_OK)
    sprintf(buffer, "Shutter counter: %s", choice);
    config.AddCiMode(2, buffer);
}
// Image format ////////////////////////////////////////////////////////////////
void CCamera::UpdateIf()
{
	if (cameraMode == MODE_MANUAL || cameraMode == MODE_TV ||
      cameraMode == MODE_P || cameraMode == MODE_AV || cameraMode == MODE_ADEP)
	{
		int ret = GP_OK;
		CameraWidget* widget = 0;
		CameraWidget* child = 0;
		char* choice = 0;

		int IfListSize = 0;    

  	ret = gp_camera_get_config(pCamera, &widget, pContext);
		if (ret >= GP_OK && widget)
		{
			ret = _gp_lookup_widget(widget, "imageformat", &child);
			if (ret >= GP_OK && child)
			{
				ret = gp_widget_count_choices(child);
				if (ret >= GP_OK)
				{
					IfListSize = ret;
					config.RemoveIfValues();
					for (int i = 0; i < IfListSize; i++)
					{
						choice = NULL;
						ret = gp_widget_get_choice(child, i, (const char**) &choice);
						if (ret >= GP_OK)
						{
							config.AddIfMode(i, choice);
						}
					}
				}	
				choice = NULL;
				ret = gp_widget_get_value(child, &choice);
				if (ret >= GP_OK && choice)
				{           
					config.SetIfMode(choice);
				}
			}
		}
	
		if (widget)
		{
			gp_widget_free(widget);
		}
		config.SetIfMode(true);
	}
	else
	{
		config.SetIfMode(false);
	}
}

int CCamera::SetIf(int position)
{
	char *value = NULL;
	value = config.GetIfMode(position);
	if (!value)
	{
		return -1;
	}
	cameraMutex.lock();
	CameraWidget* widget = 0;
	CameraWidget* child = 0;
	int ret;
    
	ret = gp_camera_get_config(pCamera, &widget, pContext);
				printf("set gp_camera_get_config %d\n", ret);
	if (ret >= GP_OK && widget)
	{
		ret = _gp_lookup_widget(widget, "imageformat", &child);
				printf("set _gp_lookup_widget %d\n", ret);
		if (ret >= GP_OK && child)
		{
			ret = gp_widget_set_value(child, value);
				printf("set gp_widget_set_value %d\n", ret);
			if (ret >= GP_OK)
			{
				ret = gp_camera_set_config(pCamera, widget, pContext);
				printf("set gp_camera_set_config %d\n", ret);
			}
		}
	}
	config.SetIfMode(value);
	if (widget)
	{
		gp_widget_free (widget);
	}
	cameraMutex.unlock();
}

// Exposure compensation ///////////////////////////////////////////////////////
void CCamera::UpdateEx()
{
	if (cameraMode == MODE_TV || cameraMode == MODE_P || 
      cameraMode == MODE_AV || cameraMode == MODE_ADEP)
	{
		int ret = GP_OK;
		CameraWidget* widget = 0;
		CameraWidget* child = 0;
		char* choice = 0;

		int ExListSize = 0;
		
		ret = gp_camera_get_config(pCamera, &widget, pContext);
		if (ret >= GP_OK && widget)
		{
			ret = _gp_lookup_widget(widget, "exposurecompensation", &child);
			if (ret >= GP_OK && child)
			{
				ret = gp_widget_count_choices(child);
				if (ret >= GP_OK)
				{
					ExListSize = ret;
					config.RemoveExValues();
					for (int i = 0; i < ExListSize; i++)
					{
						choice = NULL;
						ret = gp_widget_get_choice(child, i, (const char**) &choice);
						if (ret >= GP_OK)
						{
							config.AddExMode(i, choice);
						}
					}
				}	
				choice = NULL;
				ret = gp_widget_get_value(child, &choice);
				if (ret >= GP_OK && choice)
				{           
					config.SetExMode(choice);
				}
			}
		}
	
		if (widget)
		{
			gp_widget_free(widget);
		}
		config.SetExMode(true);
	}
	else
	{
		config.SetExMode(false);
	}
}

int CCamera::SetEx(int position)
{
	char *value = NULL;
	value = config.GetExMode(position);
	if (!value)
	{
		return -1;
	}
	cameraMutex.lock();
	CameraWidget* widget = 0;
	CameraWidget* child = 0;
	int ret;

	ret = gp_camera_get_config(pCamera, &widget, pContext);
				printf("set gp_camera_get_config %d\n", ret);
	if (ret >= GP_OK && widget)
	{
		ret = _gp_lookup_widget(widget, "exposurecompensation", &child);
				printf("set _gp_lookup_widget %d\n", ret);
		if (ret >= GP_OK && child)
		{
			ret = gp_widget_set_value(child, value);
				printf("set gp_widget_set_value %d\n", ret);
			if (ret >= GP_OK)
			{
				ret = gp_camera_set_config(pCamera, widget, pContext);
				printf("set gp_camera_set_config %d\n", ret);
			}
		}
	}
	config.SetExMode(value);
	if (widget)
	{
		gp_widget_free (widget);
	}
	cameraMutex.unlock();
}

// Aperture ////////////////////////////////////////////////////////////////////
void CCamera::UpdateAp()
{
	if (cameraMode == MODE_MANUAL || cameraMode == MODE_AV)
	{
		int ret = GP_OK;
		CameraWidget* widget = 0;
		CameraWidget* child = 0;
		char* choice = 0;

		int ApListSize = 0;
		
		ret = gp_camera_get_config(pCamera, &widget, pContext);
		if (ret >= GP_OK && widget)
		{
			ret = _gp_lookup_widget(widget, "aperture", &child);
			if (ret >= GP_OK && child)
			{
				ret = gp_widget_count_choices(child);
				if (ret >= GP_OK)
				{
					ApListSize = ret;
					config.RemoveApValues();
					for (int i = 0; i < ApListSize; i++)
					{
						choice = NULL;
						ret = gp_widget_get_choice(child, i, (const char**) &choice);
						if (ret >= GP_OK)
						{
							config.AddApMode(i, choice);
						}
					}
				}	
				choice = NULL;
				ret = gp_widget_get_value(child, &choice);
				if (ret >= GP_OK && choice)
				{           
					config.SetApMode(choice);
				}
			}
		}
	
		if (widget)
		{
			gp_widget_free(widget);
		}
		config.SetApMode(true);
	}
	else
	{
		config.SetApMode(false);
	}
}

int CCamera::SetAp(int position)
{
	char *value = NULL;
	value = config.GetApMode(position);	
	if (!value)
	{
		return -1;
	}
	cameraMutex.lock();
	CameraWidget* widget = 0;
	CameraWidget* child = 0;
	int ret;

	ret = gp_camera_get_config(pCamera, &widget, pContext);
				printf("set gp_camera_get_config %d\n", ret);
	if (ret >= GP_OK && widget)
	{
		ret = _gp_lookup_widget(widget, "aperture", &child);
				printf("set _gp_lookup_widget %d\n", ret);
		if (ret >= GP_OK && child)
		{
			ret = gp_widget_set_value(child, value);
				printf("set gp_widget_set_value %d\n", ret);
			if (ret >= GP_OK)
			{
				ret = gp_camera_set_config(pCamera, widget, pContext);
				printf("set gp_camera_set_config %d\n", ret);
			}
		}
	}
	config.SetApMode(value);
	if (widget)
	{
		gp_widget_free (widget);
	}
	cameraMutex.unlock();
}

// Shutter /////////////////////////////////////////////////////////////////////
void CCamera::UpdateSh()
{
	if (cameraMode == MODE_MANUAL || cameraMode == MODE_TV)
	{
		int ret = GP_OK;
		CameraWidget* widget = 0;
		CameraWidget* child = 0;
		char* choice = 0;

		int ShListSize = 0;
		
		ret = gp_camera_get_config(pCamera, &widget, pContext);
		if (ret >= GP_OK && widget)
		{
			ret = _gp_lookup_widget(widget, "shutterspeed", &child);
			if (ret >= GP_OK && child)
			{
				ret = gp_widget_count_choices(child);
				if (ret >= GP_OK)
				{
					ShListSize = ret;
					config.RemoveShValues();
					for (int i = 0; i < ShListSize; i++)
					{
						choice = NULL;
						ret = gp_widget_get_choice(child, i, (const char**) &choice);            
						if (ret >= GP_OK)
						{
							config.AddShMode(i, choice);
						}
					}
				}	
				choice = NULL;
				ret = gp_widget_get_value(child, &choice);
				if (ret >= GP_OK && choice)
				{
          //printf("======== %s\n", choice);
					config.SetShMode(choice);
				}
			}
		}
	
		if (widget)
		{
			gp_widget_free(widget);
		}
		config.SetShMode(true);
	}
	else
	{
		config.SetShMode(false);
	}
}

int CCamera::SetSh(int position)
{
	char *value = NULL;
	value = config.GetShMode(position);
	if (!value)
	{
		return -1;
	}
	cameraMutex.lock();
	CameraWidget* widget = 0;
	CameraWidget* child = 0;
	int ret;

	ret = gp_camera_get_config(pCamera, &widget, pContext);
				printf("set gp_camera_get_config %d\n", ret);
	if (ret >= GP_OK && widget)
	{
		ret = _gp_lookup_widget(widget, "shutterspeed", &child);
				printf("set _gp_lookup_widget %d\n", ret);
		if (ret >= GP_OK && child)
		{
			ret = gp_widget_set_value(child, value);
				printf("set gp_widget_set_value %d\n", ret);
			if (ret >= GP_OK)
			{
				ret = gp_camera_set_config(pCamera, widget, pContext);
				printf("set gp_camera_set_config %d\n", ret);
			}
		}
	}
	config.SetShMode(value);
	if (widget)
	{
		gp_widget_free (widget);
	}
	cameraMutex.unlock();
}    

// WB //////////////////////////////////////////////////////////////////////////
void CCamera::UpdateWb()
{
	if (cameraMode == MODE_MANUAL || cameraMode == MODE_TV ||
      cameraMode == MODE_P || cameraMode == MODE_AV || cameraMode == MODE_ADEP)
	{
		int ret = GP_OK;
		CameraWidget* widget = 0;
		CameraWidget* child = 0;
		char* choice = 0;

		int WbListSize = 0;
		
		ret = gp_camera_get_config(pCamera, &widget, pContext);
		if (ret >= GP_OK && widget)
		{
			ret = _gp_lookup_widget(widget, "whitebalance", &child);
			if (ret >= GP_OK && child)
			{
				ret = gp_widget_count_choices(child);
				if (ret >= GP_OK)
				{
					WbListSize = ret;
					config.RemoveWbValues();
					for (int i = 0; i < WbListSize; i++)
					{
						choice = NULL;
						ret = gp_widget_get_choice(child, i, (const char**) &choice);
						if (ret >= GP_OK)
						{
							config.AddWbMode(i, choice);
						}
					}
				}	
				choice = NULL;
				ret = gp_widget_get_value(child, &choice);
				if (ret >= GP_OK && choice)
				{           
					config.SetWbMode(choice);
				}
			}
		}
	
		if (widget)
		{
			gp_widget_free(widget);
		}
		config.SetWbMode(true);
	}
	else
	{
		config.SetWbMode(false);
	}
}

int CCamera::SetWb(int position)
{
	char *value = NULL;
	value = config.GetWbMode(position);
	if (!value)
	{
		return -1;
	}
	cameraMutex.lock();
	CameraWidget* widget = 0;
	CameraWidget* child = 0;
	int ret;

	ret = gp_camera_get_config(pCamera, &widget, pContext);
				printf("set gp_camera_get_config %d\n", ret);
	if (ret >= GP_OK && widget)
	{
		ret = _gp_lookup_widget(widget, "whitebalance", &child);
				printf("set _gp_lookup_widget %d\n", ret);
		if (ret >= GP_OK && child)
		{
			ret = gp_widget_set_value(child, value);
				printf("set gp_widget_set_value %d\n", ret);
			if (ret >= GP_OK)
			{
				ret = gp_camera_set_config(pCamera, widget, pContext);
				printf("set gp_camera_set_config %d\n", ret);
			}
		}
	}
	config.SetWbMode(value);
	if (widget)
	{
		gp_widget_free (widget);
	}
	cameraMutex.unlock();
}

// ISO /////////////////////////////////////////////////////////////////////////
void CCamera::UpdateIso()
{
	if (cameraMode == MODE_MANUAL || cameraMode == MODE_TV)
	{
		int ret = GP_OK;
		CameraWidget* widget = 0;
		CameraWidget* child = 0;
		char* choice = 0;

		int ISOListSize = 0;
		
		ret = gp_camera_get_config(pCamera, &widget, pContext);
		if (ret >= GP_OK && widget)
		{
			ret = _gp_lookup_widget(widget, "iso", &child);
			if (ret >= GP_OK && child)
			{
				ret = gp_widget_count_choices(child);
				if (ret >= GP_OK)
				{
					ISOListSize = ret;
					config.RemoveIsoValues();
					for (int i = 0; i < ISOListSize; i++)
					{
						choice = NULL;
						ret = gp_widget_get_choice(child, i, (const char**) &choice);
						if (ret >= GP_OK)
						{
							config.AddIsoMode(i, choice);
						}
					}
				}	
				choice = NULL;
				ret = gp_widget_get_value(child, &choice);
				if (ret >= GP_OK && choice)
				{
					config.SetIsoMode(choice);
				}
			}
		}
	
		if (widget)
		{
			gp_widget_free(widget);
		}
		config.SetIsoMode(true);
	}
	else
	{
		config.SetIsoMode(false);
	}
}

int CCamera::SetIso(int position)
{
	char *value = NULL;
	value = config.GetIsoMode(position);
	if (!value)
	{
		return -1;
	}
	cameraMutex.lock();
	CameraWidget* widget = 0;
	CameraWidget* child = 0;
	int ret;

	ret = gp_camera_get_config(pCamera, &widget, pContext);
				printf("set gp_camera_get_config %d\n", ret);
	if (ret >= GP_OK && widget)
	{
		ret = _gp_lookup_widget(widget, "iso", &child);
				printf("set _gp_lookup_widget %d\n", ret);
		if (ret >= GP_OK && child)
		{
			ret = gp_widget_set_value(child, value);
				printf("set gp_widget_set_value %d\n", ret);
			if (ret >= GP_OK)
			{
				ret = gp_camera_set_config(pCamera, widget, pContext);
				printf("set gp_camera_set_config %d\n", ret);
			}
		}
	}
	config.SetIsoMode(value);
	if (widget)
	{
		gp_widget_free (widget);
	}
	cameraMutex.unlock();
}

// Capture target //////////////////////////////////////////////////////////////////////
int CCamera::CaptureTargetOn() {

	char value[] = "Memory card";
		
	cameraMutex.lock();
	
	CameraWidget* widget = 0;
	CameraWidget* child = 0;
	int ret;

	ret = gp_camera_get_config(pCamera, &widget, pContext);
				printf("set gp_camera_get_config %d\n", ret);
	if (ret >= GP_OK && widget)
	{
		ret = _gp_lookup_widget(widget, "capturetarget", &child);
				printf("set _gp_lookup_widget %d\n", ret);
		if (ret >= GP_OK && child)
		{
			ret = gp_widget_set_value(child, value);
				printf("set gp_widget_set_value %d\n", ret);
			if (ret >= GP_OK)
			{
				ret = gp_camera_set_config(pCamera, widget, pContext);
				printf("set gp_camera_set_config %d\n", ret);
			}
		}
	}	
	if (widget)
	{
		gp_widget_free (widget);
	}               	
	cameraMutex.unlock();     	
	return ret;             	
}
////////////////////////////////////////////////////////////////////////////////////////
static int _gp_get_config_value_string(Camera *camera, const char *key, char **str, GPContext *context)
{
	CameraWidget* widget = 0;
	CameraWidget* child = 0;
	int ret;
	char *val;

	ret = gp_camera_get_config(camera, &widget, context);
	if (ret >= GP_OK)
		ret = _gp_lookup_widget(widget, key, &child);
	if (ret >= GP_OK)
		ret = gp_widget_get_value(child, &val);
	if (ret >= GP_OK)
		*str = strdup(val);
	if (widget)
		gp_widget_free(widget);
	return ret;
}

static int _gp_lookup_widget(CameraWidget*widget, const char *key, CameraWidget **child)
{
	int ret;
	ret = gp_widget_get_child_by_name (widget, key, child);
	if (ret < GP_OK)
	{
		//printf("hallo\n");
		ret = gp_widget_get_child_by_label (widget, key, child);
	}
	return ret;
}

void capture_to_file(Camera *canon, GPContext *canoncontext, char *fn) {
  int retval;

  printf("Capturing.\n");
  CameraFilePath camera_file_path;

  // NOP: This gets overridden in the library to /capt0000.jpg
  strcpy(camera_file_path.folder, "/");
  strcpy(camera_file_path.name, "foo.jpg");

  retval = gp_camera_capture(canon, GP_CAPTURE_IMAGE, &camera_file_path, canoncontext);
  printf("  Retval: %d\n", retval);

  printf("Pathname on the camera: %s/%s\n", camera_file_path.folder, camera_file_path.name);

  CameraFile *canonfile;

  retval = gp_file_new(&canonfile);
  printf("  Retval: %d\n", retval);
  retval = gp_camera_file_get(canon, camera_file_path.folder, camera_file_path.name,
                     GP_FILE_TYPE_NORMAL, canonfile, canoncontext);
  printf("  Retval: %d\n", retval);

  const char *filedata;
  unsigned long int filesize;

  retval = gp_file_get_data_and_size(canonfile, &filedata, &filesize);
  printf("  Retval: %d\n", retval);

  int fd = open(fn, O_CREAT | O_WRONLY, 0644);
  write(fd, filedata, filesize);
  close(fd);

  printf("Deleting.\n");
  retval = gp_camera_file_delete(canon, camera_file_path.folder, camera_file_path.name,
                        canoncontext);
  printf("  Retval: %d\n", retval);

  gp_file_free(canonfile);
}

static void
capture_to_memory(Camera *camera, GPContext *context, const char **ptr, unsigned long int *size) {
	int retval;
	CameraFile *file;
	CameraFilePath camera_file_path;

	printf("Capturing.\n");

	/* NOP: This gets overridden in the library to /capt0000.jpg */
	strcpy(camera_file_path.folder, "/");
	strcpy(camera_file_path.name, "foo.jpg");

	retval = gp_camera_capture(camera, GP_CAPTURE_IMAGE, &camera_file_path, context);
	printf("  Retval: %d\n", retval);

	printf("Pathname on the camera: %s/%s\n", camera_file_path.folder, camera_file_path.name);

	retval = gp_file_new(&file);
	printf("  Retval: %d\n", retval);
	retval = gp_camera_file_get(camera, camera_file_path.folder, camera_file_path.name,
		     GP_FILE_TYPE_NORMAL, file, context);
	printf("  Retval: %d\n", retval);

	gp_file_get_data_and_size (file, ptr, size);

	printf("Deleting.\n");
	retval = gp_camera_file_delete(camera, camera_file_path.folder, camera_file_path.name,
			context);
	printf("  Retval: %d\n", retval);

	gp_file_free(file);
}

static void
capture_to_camera(Camera *camera, GPContext *context) {
	int retval;
	CameraFile *file;
	CameraFilePath camera_file_path;

	printf("Capturing.\n");

	/* NOP: This gets overridden in the library to /capt0000.jpg */
	//strcpy(camera_file_path.folder, "/");
	//strcpy(camera_file_path.name, "foo.jpg");

	retval = gp_camera_capture(camera, GP_CAPTURE_IMAGE, &camera_file_path, context);
	printf("  Retval: %d\n", retval);

	printf("Pathname on the camera: %s/%s\n", camera_file_path.folder, camera_file_path.name);

	//retval = gp_file_new(&file);
	//printf("  Retval: %d\n", retval);
	//retval = gp_camera_file_get(camera, camera_file_path.folder, camera_file_path.name,
	//	     GP_FILE_TYPE_NORMAL, file, context);
	//printf("  Retval: %d\n", retval);

	//gp_file_get_data_and_size (file, ptr, size);

	//printf("Deleting.\n");
	//retval = gp_camera_file_delete(camera, camera_file_path.folder, camera_file_path.name, context);
	//printf("  Retval: %d\n", retval);

	//gp_file_free(file);
}

