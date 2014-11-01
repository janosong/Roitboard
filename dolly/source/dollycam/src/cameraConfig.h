#pragma once
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <boost/thread/mutex.hpp>

typedef enum _CAMERA_MODE
{
	MODE_UNKNOWN,
	MODE_AV,
	MODE_TV,
	MODE_MANUAL,
	MODE_P,
	MODE_ADEP,
}CAMERA_MODE;
	
class CCameraConfig{
	
public:
	/* constructor */
	CCameraConfig(void);
	/* destructor */
	~CCameraConfig(void);
	bool Initialize();
	void GetXmlConfig(unsigned char **buffer, int *size);
	void Invalidate();
	void SetCameraMode(CAMERA_MODE mode);
	void SetAfMode(bool state);
	void SetIsoMode(bool state);
	void SetIsoMode(char *mode);
	char *GetIsoMode(int position);
	void AddIsoMode(int position, char *value);
  void RemoveIsoValues(void);  
  void SetWbMode(bool state);
	void SetWbMode(char *mode);
	char *GetWbMode(int position);
	void AddWbMode(int position, char *value);
	void RemoveWbValues(void);  
  void SetShMode(bool state);
	void SetShMode(char *mode);
	char *GetShMode(int position);
	void AddShMode(int position, char *value);
	void RemoveShValues(void);  
  void SetApMode(bool state);
	void SetApMode(char *mode);
	char *GetApMode(int position);
	void AddApMode(int position, char *value);
	void RemoveApValues(void);  	
  void SetExMode(bool state);
	void SetExMode(char *mode);
	char *GetExMode(int position);
	void AddExMode(int position, char *value);
	void RemoveExValues(void); 	
  void SetIfMode(bool state);
	void SetIfMode(char *mode);
	char *GetIfMode(int position);
	void AddIfMode(int position, char *value);
	void RemoveIfValues(void);
  void AddCiMode(int position, char *value); 
  void RemoveCiValues(void);	
  void ChangeBtLabel(char *value);	
private:
	xmlDocPtr pXmlDoc;
	xmlNodePtr root_node;
	xmlNodePtr controls;
	xmlNodePtr modeRoot;
	xmlNodePtr modeNode;
	xmlNodePtr nodeAf;
	xmlNodePtr nodeAfValues;
	xmlAttrPtr nodeAfStatus;
	xmlNodePtr nodeIso;
	xmlNodePtr nodeIsoLabel;
	xmlNodePtr nodeIsoValues;
	xmlAttrPtr nodeIsoStatus;
	xmlNodePtr nodeWb;
	xmlNodePtr nodeWbLabel;
	xmlNodePtr nodeWbValues;
	xmlAttrPtr nodeWbStatus;
  xmlNodePtr nodeDc;
  xmlNodePtr nodeDcValues;
  xmlNodePtr nodeSh;
	xmlNodePtr nodeShLabel;
	xmlNodePtr nodeShValues;
	xmlAttrPtr nodeShStatus;
  xmlNodePtr nodeAp;
	xmlNodePtr nodeApLabel;
	xmlNodePtr nodeApValues;
	xmlAttrPtr nodeApStatus;	
  xmlNodePtr nodeEx;
	xmlNodePtr nodeExLabel;
	xmlNodePtr nodeExValues;
	xmlAttrPtr nodeExStatus;		
  xmlNodePtr nodeIf;
	xmlNodePtr nodeIfLabel;
	xmlNodePtr nodeIfValues;
	xmlAttrPtr nodeIfStatus;
	xmlNodePtr nodeCi;
  xmlNodePtr nodeCiValues;
	xmlNodePtr nodeBt;
  xmlNodePtr nodeBtLabel; 
  xmlNodePtr nodeSt;
  xmlNodePtr nodeIc; 		
	bool invalid;
	boost::mutex xmlMutex;
};
