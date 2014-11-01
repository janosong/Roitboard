#include "cameraConfig.h"

CCameraConfig::CCameraConfig(void)
{
	pXmlDoc = NULL;
	root_node = NULL;
	invalid = true;
}

CCameraConfig::~CCameraConfig(void)
{

}

bool CCameraConfig::Initialize(void)
{
	/* create xml document */
	pXmlDoc = xmlNewDoc(BAD_CAST "1.0");
	if (!pXmlDoc)
	{
		return false;
	}
	
	root_node = xmlNewNode(NULL, BAD_CAST "dolly");
    xmlDocSetRootElement(pXmlDoc, root_node);
	controls = xmlNewChild(root_node, NULL, BAD_CAST "controls", NULL);
	/* mode item */
	modeRoot = xmlNewChild(controls, NULL, BAD_CAST "item", NULL);
	xmlNewProp(modeRoot, BAD_CAST "id", BAD_CAST "0");
	xmlNewProp(modeRoot, BAD_CAST "name", BAD_CAST "Mode");
	xmlNewProp(modeRoot, BAD_CAST "type", BAD_CAST "info");
	/* mode label */
	modeNode = xmlNewChild(modeRoot, NULL, BAD_CAST "label", BAD_CAST "unknown");
	xmlNewProp(modeNode, BAD_CAST "icon", BAD_CAST "mode.jpg");
	/* AF */
	nodeAf = xmlNewChild(controls, NULL, BAD_CAST "item", NULL);
	xmlNewProp(nodeAf, BAD_CAST "id", BAD_CAST "1");
	xmlNewProp(nodeAf, BAD_CAST "name", BAD_CAST "AF");
	nodeAfStatus = xmlNewProp(nodeAf, BAD_CAST "status", BAD_CAST "disabled");
	xmlNewProp(nodeAf, BAD_CAST "type", BAD_CAST "pushbutton");
	xmlNewProp(nodeAf, BAD_CAST "action", BAD_CAST "topbar");
	
	xmlNewChild(nodeAf, NULL, BAD_CAST "label", BAD_CAST "AF");
	nodeAfValues = xmlNewChild(nodeAf, NULL, BAD_CAST "values", NULL);
	
	xmlNodePtr val = xmlNewChild(nodeAfValues, NULL, BAD_CAST "val", BAD_CAST "&lt;&lt;&lt;");
	xmlNewProp(val, BAD_CAST "id", BAD_CAST "0");
	val = xmlNewChild(nodeAfValues, NULL, BAD_CAST "val", BAD_CAST "&lt;&lt;");
	xmlNewProp(val, BAD_CAST "id", BAD_CAST "1");
	val = xmlNewChild(nodeAfValues, NULL, BAD_CAST "val", BAD_CAST "&lt;");
	xmlNewProp(val, BAD_CAST "id", BAD_CAST "2");
	val = xmlNewChild(nodeAfValues, NULL, BAD_CAST "val", BAD_CAST "AF");
	xmlNewProp(val, BAD_CAST "id", BAD_CAST "3");
	val = xmlNewChild(nodeAfValues, NULL, BAD_CAST "val", BAD_CAST "CAF");
	xmlNewProp(val, BAD_CAST "id", BAD_CAST "4");
	val = xmlNewChild(nodeAfValues, NULL, BAD_CAST "val", BAD_CAST "&gt;");
	xmlNewProp(val, BAD_CAST "id", BAD_CAST "5");
	val = xmlNewChild(nodeAfValues, NULL, BAD_CAST "val", BAD_CAST "&gt;&gt;");
	xmlNewProp(val, BAD_CAST "id", BAD_CAST "6");
	val = xmlNewChild(nodeAfValues, NULL, BAD_CAST "val", BAD_CAST "&gt;&gt;&gt;");
	xmlNewProp(val, BAD_CAST "id", BAD_CAST "7");

	/* WB */
	nodeWb = xmlNewChild(controls, NULL, BAD_CAST "item", NULL);
	xmlNewProp(nodeWb, BAD_CAST "id", BAD_CAST "2");
	xmlNewProp(nodeWb, BAD_CAST "name", BAD_CAST "WB");
	nodeWbStatus = xmlNewProp(nodeWb, BAD_CAST "status", BAD_CAST "disabled");
	xmlNewProp(nodeWb, BAD_CAST "type", BAD_CAST "pushbutton");
	xmlNewProp(nodeWb, BAD_CAST "action", BAD_CAST "list");
	nodeWbLabel = xmlNewChild(nodeWb, NULL, BAD_CAST "label", BAD_CAST "N/A");
	nodeWbValues = xmlNewChild(nodeWb, NULL, BAD_CAST "values", NULL);	
  
  /* Dolly control */
	nodeDc = xmlNewChild(controls, NULL, BAD_CAST "item", NULL);
	xmlNewProp(nodeDc, BAD_CAST "id", BAD_CAST "4");
	xmlNewProp(nodeDc, BAD_CAST "name", BAD_CAST "Dolly control");
	xmlNewProp(nodeDc, BAD_CAST "status", BAD_CAST "enabled");
	xmlNewProp(nodeDc, BAD_CAST "type", BAD_CAST "pushbutton");
	xmlNewProp(nodeDc, BAD_CAST "action", BAD_CAST "topbar");
	xmlNewChild(nodeDc, NULL, BAD_CAST "label", BAD_CAST "Dolly control");
	nodeDcValues = xmlNewChild(nodeDc, NULL, BAD_CAST "values", NULL);
  val = xmlNewChild(nodeDcValues, NULL, BAD_CAST "val", BAD_CAST "Up");
  xmlNewProp(val, BAD_CAST "id", BAD_CAST "0");
  val = xmlNewChild(nodeDcValues, NULL, BAD_CAST "val", BAD_CAST "Down");
  xmlNewProp(val, BAD_CAST "id", BAD_CAST "1");
  val = xmlNewChild(nodeDcValues, NULL, BAD_CAST "val", BAD_CAST "Left");
  xmlNewProp(val, BAD_CAST "id", BAD_CAST "2");
  val = xmlNewChild(nodeDcValues, NULL, BAD_CAST "val", BAD_CAST "Right");
  xmlNewProp(val, BAD_CAST "id", BAD_CAST "3");
  val = xmlNewChild(nodeDcValues, NULL, BAD_CAST "val", BAD_CAST "Forward");
  xmlNewProp(val, BAD_CAST "id", BAD_CAST "4");
  val = xmlNewChild(nodeDcValues, NULL, BAD_CAST "val", BAD_CAST "Backward");
  xmlNewProp(val, BAD_CAST "id", BAD_CAST "5");
  
	/* Shutter */
	nodeSh = xmlNewChild(controls, NULL, BAD_CAST "item", NULL);
	xmlNewProp(nodeSh, BAD_CAST "id", BAD_CAST "7");
	xmlNewProp(nodeSh, BAD_CAST "name", BAD_CAST "Shutter");
	nodeShStatus = xmlNewProp(nodeSh, BAD_CAST "status", BAD_CAST "disabled");
	xmlNewProp(nodeSh, BAD_CAST "type", BAD_CAST "pushbutton");
	xmlNewProp(nodeSh, BAD_CAST "action", BAD_CAST "list");
	nodeShLabel = xmlNewChild(nodeSh, NULL, BAD_CAST "label", BAD_CAST "N/A");
	nodeShValues = xmlNewChild(nodeSh, NULL, BAD_CAST "values", NULL);
  
	/* Aperture */
	nodeAp = xmlNewChild(controls, NULL, BAD_CAST "item", NULL);
	xmlNewProp(nodeAp, BAD_CAST "id", BAD_CAST "9");
	xmlNewProp(nodeAp, BAD_CAST "name", BAD_CAST "Aperture");
	nodeApStatus = xmlNewProp(nodeAp, BAD_CAST "status", BAD_CAST "disabled");
	xmlNewProp(nodeAp, BAD_CAST "type", BAD_CAST "pushbutton");
	xmlNewProp(nodeAp, BAD_CAST "action", BAD_CAST "list");
	nodeApLabel = xmlNewChild(nodeAp, NULL, BAD_CAST "label", BAD_CAST "N/A");
	nodeApValues = xmlNewChild(nodeAp, NULL, BAD_CAST "values", NULL);    
  
	/* Exposure compensation */
	nodeEx = xmlNewChild(controls, NULL, BAD_CAST "item", NULL);
	xmlNewProp(nodeEx, BAD_CAST "id", BAD_CAST "10");
	xmlNewProp(nodeEx, BAD_CAST "name", BAD_CAST "Exposure Compensation");
	nodeExStatus = xmlNewProp(nodeEx, BAD_CAST "status", BAD_CAST "disabled");
	xmlNewProp(nodeEx, BAD_CAST "type", BAD_CAST "pushbutton");
	xmlNewProp(nodeEx, BAD_CAST "action", BAD_CAST "list");
	nodeExLabel = xmlNewChild(nodeEx, NULL, BAD_CAST "label", BAD_CAST "N/A");
	nodeExValues = xmlNewChild(nodeEx, NULL, BAD_CAST "values", NULL);	   	
	
	/* ISO */
	nodeIso = xmlNewChild(controls, NULL, BAD_CAST "item", NULL);
	xmlNewProp(nodeIso, BAD_CAST "id", BAD_CAST "11");
	xmlNewProp(nodeIso, BAD_CAST "name", BAD_CAST "ISO");
	nodeIsoStatus = xmlNewProp(nodeIso, BAD_CAST "status", BAD_CAST "disabled");
	xmlNewProp(nodeIso, BAD_CAST "type", BAD_CAST "pushbutton");
	xmlNewProp(nodeIso, BAD_CAST "action", BAD_CAST "list");
	nodeIsoLabel = xmlNewChild(nodeIso, NULL, BAD_CAST "label", BAD_CAST "N/A");
	nodeIsoValues = xmlNewChild(nodeIso, NULL, BAD_CAST "values", NULL);
		
	/* Camera info */
	nodeCi = xmlNewChild(controls, NULL, BAD_CAST "item", NULL);
	xmlNewProp(nodeCi, BAD_CAST "id", BAD_CAST "12");
	xmlNewProp(nodeCi, BAD_CAST "name", BAD_CAST "Info");
	xmlNewProp(nodeCi, BAD_CAST "status", BAD_CAST "enabled");
	xmlNewProp(nodeCi, BAD_CAST "type", BAD_CAST "pushbutton");
	xmlNewProp(nodeCi, BAD_CAST "action", BAD_CAST "info");
	xmlNewChild(nodeCi, NULL, BAD_CAST "label", BAD_CAST "Info");
	nodeCiValues = xmlNewChild(nodeCi, NULL, BAD_CAST "values", NULL);	
	
	/* Battery level */
	nodeBt = xmlNewChild(controls, NULL, BAD_CAST "item", NULL);
	xmlNewProp(nodeBt, BAD_CAST "id", BAD_CAST "13");
	xmlNewProp(nodeBt, BAD_CAST "name", BAD_CAST "Battery Level");
	xmlNewProp(nodeBt, BAD_CAST "type", BAD_CAST "info");
	nodeBtLabel = xmlNewChild(nodeBt, NULL, BAD_CAST "label", BAD_CAST "unknown");	
		
	/* Image format */
	nodeIf = xmlNewChild(controls, NULL, BAD_CAST "item", NULL);
	xmlNewProp(nodeIf, BAD_CAST "id", BAD_CAST "14");
	xmlNewProp(nodeIf, BAD_CAST "name", BAD_CAST "Image format");
	nodeIfStatus = xmlNewProp(nodeIf, BAD_CAST "status", BAD_CAST "disabled");
	xmlNewProp(nodeIf, BAD_CAST "type", BAD_CAST "pushbutton");
	xmlNewProp(nodeIf, BAD_CAST "action", BAD_CAST "list");
	nodeIfLabel = xmlNewChild(nodeIf, NULL, BAD_CAST "label", BAD_CAST "unknown");
	nodeIfValues = xmlNewChild(nodeIf, NULL, BAD_CAST "values", NULL);

	/* Settings */
	nodeSt = xmlNewChild(controls, NULL, BAD_CAST "item", NULL);
	xmlNewProp(nodeSt, BAD_CAST "id", BAD_CAST "16");
	xmlNewProp(nodeSt, BAD_CAST "name", BAD_CAST "Settings");
	xmlNewProp(nodeSt, BAD_CAST "status", BAD_CAST "enabled");
	xmlNewProp(nodeSt, BAD_CAST "type", BAD_CAST "pushbutton");
	xmlNewProp(nodeSt, BAD_CAST "action", BAD_CAST "menu");
	xmlNewChild(nodeSt, NULL, BAD_CAST "label", BAD_CAST "Settings");
	
	/* Image capture */
	nodeIc = xmlNewChild(controls, NULL, BAD_CAST "item", NULL);
	xmlNewProp(nodeIc, BAD_CAST "id", BAD_CAST "18");
	xmlNewProp(nodeIc, BAD_CAST "name", BAD_CAST "Image capture");
	xmlNewProp(nodeIc, BAD_CAST "status", BAD_CAST "enabled");
	xmlNewProp(nodeIc, BAD_CAST "type", BAD_CAST "pushbutton");
	xmlNewProp(nodeIc, BAD_CAST "action", BAD_CAST "request");
	xmlNewChild(nodeIc, NULL, BAD_CAST "label", BAD_CAST "Image capture");	
		
  return true;	
}

void CCameraConfig::GetXmlConfig(unsigned char **buffer, int *size)
{
  xmlMutex.lock();
	xmlDocDumpFormatMemory(pXmlDoc, buffer, size, 1);
	xmlMutex.unlock();
}

void CCameraConfig::SetCameraMode(CAMERA_MODE mode)
{
  xmlMutex.lock();
	switch (mode)
	{
		case MODE_AV:
		{
			xmlNodeSetContent(modeNode, BAD_CAST "Av");
			break;
		}
		case MODE_TV:
		{
			xmlNodeSetContent(modeNode, BAD_CAST "Tv");
			break;
		}
		case MODE_MANUAL:
		{
			xmlNodeSetContent(modeNode, BAD_CAST "M");
			break;
		}
		case MODE_P:
		{
			xmlNodeSetContent(modeNode, BAD_CAST "P");
			break;
		}
		case MODE_ADEP:
		{
			xmlNodeSetContent(modeNode, BAD_CAST "A-DEP");
			break;
		}
		case MODE_UNKNOWN:
		{
			xmlNodeSetContent(modeNode, BAD_CAST "-");
			break;
		}
		default:
		{
			xmlNodeSetContent(modeNode, BAD_CAST "-");
			break;	
		}
	}
	xmlMutex.unlock();
}

void CCameraConfig::SetAfMode(bool state)
{
  xmlMutex.lock();
	if (state)
	{
		xmlSetProp(nodeAf, BAD_CAST "status", BAD_CAST "enabled");
	}
	else
	{
		xmlSetProp(nodeAf, BAD_CAST "status", BAD_CAST "disabled");
	}
	xmlMutex.unlock();
}

// Battery level ///////////////////////////////////////////////////////////////
void CCameraConfig::ChangeBtLabel(char *value)
{  
  xmlMutex.lock();
  xmlNodeSetContent(nodeBtLabel, (xmlChar*)value);
	xmlMutex.unlock();
}
                                                                             
// Camera information //////////////////////////////////////////////////////////
void CCameraConfig::AddCiMode(int position, char *value)
{
  xmlMutex.lock();
	xmlNodePtr val = xmlNewChild(nodeCiValues, NULL, BAD_CAST "val", BAD_CAST value);
	char buffer[256] = {0,};
	sprintf(buffer, "%d", position);
	xmlNewProp(val, BAD_CAST "id", BAD_CAST buffer);
	xmlMutex.unlock();
}
void CCameraConfig::RemoveCiValues(void)
{
  xmlMutex.lock();
	xmlUnlinkNode(nodeCiValues);
	xmlFreeNodeList(nodeCiValues);
	nodeCiValues = xmlNewChild(nodeCi, NULL, BAD_CAST "values", NULL);
	xmlMutex.unlock();
}
// Image Format ///////////////////////////////////////////////////////
void CCameraConfig::SetIfMode(bool state)
{
  xmlMutex.lock();
	if (state)
	{
		xmlSetProp(nodeIf, BAD_CAST "status", BAD_CAST "enabled");
	}
	else
	{
		xmlSetProp(nodeIf, BAD_CAST "status", BAD_CAST "disabled");
	}
	xmlMutex.unlock();
}

void CCameraConfig::SetIfMode(char *mode)
{
  xmlMutex.lock();
	xmlNodeSetContent(nodeIfLabel, BAD_CAST mode);
	xmlMutex.unlock();
}

char *CCameraConfig::GetIfMode(int position)
{
	int i = 0;
	if (position + 1 > xmlChildElementCount(nodeIfValues))
	{
		return NULL;
	}
    	
	xmlNodePtr child = xmlFirstElementChild(nodeIfValues);
	
	while (child)
	{
		if (i == position)
		{
			break;
		}
		child = xmlNextElementSibling(child);
		i ++;
	}
	if (child)
	{
		return (char *)xmlNodeGetContent(child);
	}
	return NULL;
}

void CCameraConfig::RemoveIfValues(void)
{
  xmlMutex.lock();
	xmlUnlinkNode(nodeIfValues);
	xmlFreeNodeList(nodeIfValues);
	nodeIfValues = xmlNewChild(nodeIf, NULL, BAD_CAST "values", NULL);
	xmlMutex.unlock();
}
void CCameraConfig::AddIfMode(int position, char *value)
{
  xmlMutex.lock();
	xmlNodePtr val = xmlNewChild(nodeIfValues, NULL, BAD_CAST "val", BAD_CAST value);
	char buffer[256] = {0,};
	sprintf(buffer, "%d", position);
	xmlNewProp(val, BAD_CAST "id", BAD_CAST buffer);
	xmlMutex.unlock();
}

// Exposure compensation ///////////////////////////////////////////////////////
void CCameraConfig::SetExMode(bool state)
{
  xmlMutex.lock();
	if (state)
	{
		xmlSetProp(nodeEx, BAD_CAST "status", BAD_CAST "enabled");
	}
	else
	{
		xmlSetProp(nodeEx, BAD_CAST "status", BAD_CAST "disabled");
	}
	xmlMutex.unlock();
}

void CCameraConfig::SetExMode(char *mode)
{
  xmlMutex.lock();
	xmlNodeSetContent(nodeExLabel, BAD_CAST mode);
	xmlMutex.unlock();
}

char *CCameraConfig::GetExMode(int position)
{
	int i = 0;
	if (position + 1 > xmlChildElementCount(nodeExValues))
	{
		return NULL;
	}
	
	xmlNodePtr child = xmlFirstElementChild(nodeExValues);
	
	while (child)
	{
		if (i == position)
		{
			break;
		}
		child = xmlNextElementSibling(child);
		i ++;
	}
	if (child)
	{
		return (char *)xmlNodeGetContent(child);
	}
	return NULL;
}

void CCameraConfig::RemoveExValues(void)
{
  xmlMutex.lock();
	xmlUnlinkNode(nodeExValues);
	xmlFreeNodeList(nodeExValues);
	nodeExValues = xmlNewChild(nodeEx, NULL, BAD_CAST "values", NULL);
	xmlMutex.unlock();
}
void CCameraConfig::AddExMode(int position, char *value)
{
  xmlMutex.lock();
	xmlNodePtr val = xmlNewChild(nodeExValues, NULL, BAD_CAST "val", BAD_CAST value);
	char buffer[256] = {0,};
	sprintf(buffer, "%d", position);
	xmlNewProp(val, BAD_CAST "id", BAD_CAST buffer);
	xmlMutex.unlock();
}
// Aperture /////////////////////////////////////////////////////////////////////
void CCameraConfig::SetApMode(bool state)
{
  xmlMutex.lock();
	if (state)
	{
		xmlSetProp(nodeAp, BAD_CAST "status", BAD_CAST "enabled");
	}
	else
	{
		xmlSetProp(nodeAp, BAD_CAST "status", BAD_CAST "disabled");
	}
	xmlMutex.unlock();
}

void CCameraConfig::SetApMode(char *mode)
{
  xmlMutex.lock();
	xmlNodeSetContent(nodeApLabel, BAD_CAST mode);
	xmlMutex.unlock();
}

char *CCameraConfig::GetApMode(int position)
{
	int i = 0;
	if (position + 1 > xmlChildElementCount(nodeApValues))
	{
		return NULL;
	}
	
	xmlNodePtr child = xmlFirstElementChild(nodeApValues);
	
	while (child)
	{
		if (i == position)
		{
			break;
		}
		child = xmlNextElementSibling(child);
		i ++;
	}
	if (child)
	{
		return (char *)xmlNodeGetContent(child);
	}
	return NULL;
}

void CCameraConfig::RemoveApValues(void)
{
  xmlMutex.lock();
	xmlUnlinkNode(nodeApValues);
	xmlFreeNodeList(nodeApValues);
	nodeApValues = xmlNewChild(nodeAp, NULL, BAD_CAST "values", NULL);
	xmlMutex.unlock();
}                                                                                          
void CCameraConfig::AddApMode(int position, char *value)
{
  xmlMutex.lock();
	xmlNodePtr val = xmlNewChild(nodeApValues, NULL, BAD_CAST "val", BAD_CAST value);
	char buffer[256] = {0,};
	sprintf(buffer, "%d", position);
	xmlNewProp(val, BAD_CAST "id", BAD_CAST buffer);
	xmlMutex.unlock();
}
// Shutter //////////////////////////////////////////////////////////////////////////
void CCameraConfig::SetShMode(bool state)
{
  xmlMutex.lock();
	if (state)
	{
		xmlSetProp(nodeSh, BAD_CAST "status", BAD_CAST "enabled");
	}
	else
	{
		xmlSetProp(nodeSh, BAD_CAST "status", BAD_CAST "disabled");
	}
  xmlMutex.unlock();
}

void CCameraConfig::SetShMode(char *mode)
{
  xmlMutex.lock();
	xmlNodeSetContent(nodeShLabel, BAD_CAST mode);
	xmlMutex.unlock();
}

char *CCameraConfig::GetShMode(int position)
{
	int i = 0;
	if (position + 1 > xmlChildElementCount(nodeShValues))
	{
		return NULL;
	}
	
	xmlNodePtr child = xmlFirstElementChild(nodeShValues);
	
	while (child)
	{
		if (i == position)
		{
			break;
		}
		child = xmlNextElementSibling(child);
		i ++;
	}
	if (child)
	{
		return (char *)xmlNodeGetContent(child);
	}
	return NULL;
}

void CCameraConfig::RemoveShValues(void)
{
  xmlMutex.lock();
	xmlUnlinkNode(nodeShValues);
	xmlFreeNodeList(nodeShValues);
	nodeShValues = xmlNewChild(nodeSh, NULL, BAD_CAST "values", NULL);
	xmlMutex.unlock();
}                                                                                          
void CCameraConfig::AddShMode(int position, char *value)
{
  xmlMutex.lock();
	xmlNodePtr val = xmlNewChild(nodeShValues, NULL, BAD_CAST "val", BAD_CAST value);
	char buffer[256] = {0,};
	sprintf(buffer, "%d", position);
	xmlNewProp(val, BAD_CAST "id", BAD_CAST buffer);
	xmlMutex.unlock();
}
// WB //////////////////////////////////////////////////////////////////////////
void CCameraConfig::SetWbMode(bool state)
{
  xmlMutex.lock();
	if (state)
	{
		xmlSetProp(nodeWb, BAD_CAST "status", BAD_CAST "enabled");
	}
	else
	{
		xmlSetProp(nodeWb, BAD_CAST "status", BAD_CAST "disabled");
	}
	xmlMutex.unlock();
}

void CCameraConfig::SetWbMode(char *mode)
{
  xmlMutex.lock();
	xmlNodeSetContent(nodeWbLabel, BAD_CAST mode);
	xmlMutex.unlock();
}

char *CCameraConfig::GetWbMode(int position)
{
	int i = 0;
	if (position + 1 > xmlChildElementCount(nodeWbValues))
	{
		return NULL;
	}
	
	xmlNodePtr child = xmlFirstElementChild(nodeWbValues);
	
	while (child)
	{
		if (i == position)
		{
			break;
		}
		child = xmlNextElementSibling(child);
		i ++;
	}
	if (child)
	{
		return (char *)xmlNodeGetContent(child);
	}
	return NULL;
}

void CCameraConfig::RemoveWbValues(void)
{
  xmlMutex.lock();
	xmlUnlinkNode(nodeWbValues);
	xmlFreeNodeList(nodeWbValues);
	nodeWbValues = xmlNewChild(nodeWb, NULL, BAD_CAST "values", NULL);
	xmlMutex.unlock();
}
void CCameraConfig::AddWbMode(int position, char *value)
{
  xmlMutex.lock();
	xmlNodePtr val = xmlNewChild(nodeWbValues, NULL, BAD_CAST "val", BAD_CAST value);
	char buffer[256] = {0,};
	sprintf(buffer, "%d", position);
	xmlNewProp(val, BAD_CAST "id", BAD_CAST buffer);
	xmlMutex.unlock();
}
// ISO /////////////////////////////////////////////////////////////////////////
void CCameraConfig::SetIsoMode(bool state)
{
  xmlMutex.lock();
	if (state)
	{
		xmlSetProp(nodeIso, BAD_CAST "status", BAD_CAST "enabled");
	}
	else
	{
		xmlSetProp(nodeIso, BAD_CAST "status", BAD_CAST "disabled");
	}
	xmlMutex.unlock();
}

void CCameraConfig::SetIsoMode(char *mode)
{
  xmlMutex.lock();
	xmlNodeSetContent(nodeIsoLabel, BAD_CAST mode);
	xmlMutex.unlock();
}

char *CCameraConfig::GetIsoMode(int position)
{
	int i = 0;
	if (position + 1 > xmlChildElementCount(nodeIsoValues))
	{
		return NULL;
	}
	
	xmlNodePtr child = xmlFirstElementChild(nodeIsoValues);
	
	while (child)
	{
		if (i == position)
		{
			break;
		}
		child = xmlNextElementSibling(child);
		i ++;
	}
	if (child)
	{
		return (char *)xmlNodeGetContent(child);
	}
	return NULL;
}

void CCameraConfig::RemoveIsoValues(void)
{
  xmlMutex.lock();
	xmlUnlinkNode(nodeIsoValues);
	xmlFreeNodeList(nodeIsoValues);
	nodeIsoValues = xmlNewChild(nodeIso, NULL, BAD_CAST "values", NULL);
	xmlMutex.unlock();
}
void CCameraConfig::AddIsoMode(int position, char *value)
{
  xmlMutex.lock();
	xmlNodePtr val = xmlNewChild(nodeIsoValues, NULL, BAD_CAST "val", BAD_CAST value);
	char buffer[256] = {0,};
	sprintf(buffer, "%d", position);
	xmlNewProp(val, BAD_CAST "id", BAD_CAST buffer);
	xmlMutex.unlock();
}

void CCameraConfig::Invalidate()
{
	invalid = true;
}
