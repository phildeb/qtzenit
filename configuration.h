#ifndef   	CONFIGURATION_HH_
#define   	CONFIGURATION_HH_

#define PATH_MAXLEN								1024

#define COLOR_GREEN								"#66CC33"
#define COLOR_RED								"#CC0000"

#define DEVICE_CRM4_NB_KEYS						52

#define CONFIGURATION_DIR						"/home/operateur/vega/"

#define PIXMAP_DIR								"/home/operateur/vega/imgs/"
#define SETTINGS_FILE							"/home/operateur/vega/settings.conf"
#define SWITCH_CONFIG_FILE						"/home/operateur/vega/switch.conf"
#define TEMPLATE								"/home/operateur/vega/template.conf"

class VConferences;
class ConferenceConfiguration ;


typedef enum {
  ACTION_ACTIVATE_CONF = 0,
  ACTION_SELECT_CONF, 
  ACTION_ON_OFF_CONF,
  ACTION_SELECT_DEVICE,
  ACTION_EXCLUDE_INCLUDE,
  ACTION_GENERAL_CALL,
  ACTION_GROUP_CALL,
  ACTION_NONE,
  ACTION_INVALID
} key_action_t;

typedef struct key_configuration_s {
  key_action_t action;
  int key_value;
} key_configuration_t;


class DeviceKeysConfig {
public:
  DeviceKeysConfig();
 
  const char *actionToStr(int action);

  bool configureKey(int number, key_action_t action, int value);

  key_action_t getKeyAction(int number);

  int getKeyValue(int number);

  key_configuration_t getKeyConfiguration(int number);

  const char* dump();

  bool saveDeviceKeys(const char*);

  bool loadKeys(const char *);

  key_configuration_t keyConfs[DEVICE_CRM4_NB_KEYS];
  QString m_fileName;
 
};

typedef enum {
  DEVICE_CRM4 = 0,
  DEVICE_RADIO,
  DEVICE_JUPITER2
} device_type_t;

#define DEVICE_CRM4_STR				"crm4"
#define DEVICE_RADIO_STR			"radio"
#define DEVICE_JUPITER2_STR			"jupiter"

typedef struct key_dev_configuration_s {
  int number;
  int group_number;
} device_key_conf_t;

#define MAX_GAINS 15

class Device 
{  
public :
	Device(){
	  this->m_name = "";
	  this->m_enabled = true;
	  this->m_fullDuplex = 0;
	  for (int i = 0; i < 10; i++) {m_keys[i].number = m_keys[i].group_number = 0;}
	}
	Device(int number){
	  this->m_number = number;
	  this->m_name = "";
	}
	Device(int number, device_type_t type, const char *name, int gain , bool full_duplex){
	  this->m_number = number;
	  this->m_type = type;
	  this->m_name = name;
	  this->m_gain = gain;
	  this->m_fullDuplex = full_duplex;
	  this->m_enabled = true;
	}

	device_type_t getType(){
		if ( m_number <= 24 ) return DEVICE_CRM4;//return this->m_type;
		if ( m_number <= 35 ) return DEVICE_RADIO;//return this->m_type;
		return DEVICE_JUPITER2;
	}

	int getNumber(){return m_number;}
	void setType(device_type_t type){m_type = type;}
    const char *getName(){return this->m_name.toStdString().c_str();}
	void setDeviceName(const char *s){m_name = s;}
	void setNumber(int number){m_number = number;}
	int getGain(){ return m_gain;}
	void setGain(int gain){m_gain = gain;}
	void setEnabled(){m_enabled = true;}
	void setDisabled(){m_enabled = false;}
	bool isEnabled(){return m_enabled;}

	int getKeyNumberByGroupNumber(int group_number);

	int getFullDuplex(){return this->m_fullDuplex;}
	void setFullDuplex(bool val){m_fullDuplex = val;}

	int getContactSec(){return m_contactSec;}
	void setContactSec(bool val){m_contactSec = val;}


	char *dump();
	char *dumpKeysConfiguration();
	static const char *typeToStr(device_type_t type);

	bool configureKey(int key_number, key_action_t action, int value);
  	int getGroupNumberByKeyNumber(int key_number);

	device_key_conf_t m_keys[10]; // ???? pas plus ???
	device_type_t m_type;
	QString m_name;
	int m_number;
	bool m_fullDuplex;
	int m_gain;
	bool m_enabled;
	bool m_contactSec;  

	static char deviceGains[MAX_GAINS];
};

class DeviceUiElt 
{
public :
  DeviceUiElt()	{
	  this->m_name = "";
	  this->m_deviceNumber = 0;
	}
  void setName(const char* name){m_name = name;}
  const char *getName(){ return this->m_name.toStdString().c_str();}
  int getDeviceNumber(){return this->m_deviceNumber;}
  void setDeviceNumber(int device_number){m_deviceNumber = device_number;}
  QString m_name;
  int m_deviceNumber;
};

class Group 
{
public:
  Group(){m_number = -1;}

  void setGroupName(const char *n){m_name = n;}
  const char *getName(){return m_name.toStdString().c_str();}
  void addMember(int number){m_members.append(number);}
  void delMember(int number)
  {  
	QList<int>::iterator i;
	for ( i = this->m_members.begin(); i != this->m_members.end(); i++ )
		if (*i == number) {this->m_members.erase(i);break;}
  }
  bool deviceIsMember(int number)
  {
	QList<int>::iterator i;
	bool ret = false;
	for ( i = this->m_members.begin(); i != this->m_members.end(); i++ )
		if (*i == number) {ret = true;break;}
	return ret;
  }  
  
  QList<int> *getMembers(){return new QList<int>(this->m_members);}
  void setNumber(int number){m_number = number;}
  int getNumber(){return m_number;}
  
  char *dump();
  QString		m_name; 		// nom du groupe
  QList<int>	m_members;		// membres du groupe
  int			m_number;			// numero du groupe
};

class Conference
{
public:
	int m_director;
	int m_radio1;
	int m_radio2;
	int m_jupiter2;
	QString m_conference_name;
	QList<int> m_conference_members;
	int m_number;

	Conference(const char*name){
	  this->m_conference_name = name;
	  this->m_director = 0;
	  this->m_radio1 = 0;
	  this->m_radio2 = 0;
	  this->m_jupiter2 = 0;
	}
    const char* getName(){return m_conference_name.toStdString().c_str();}
	int getNumber(){  return this->m_number;}
	void setNumber(int number){  this->m_number = number;}

	bool deviceIsMember(int device_number){	return  m_conference_members.contains(device_number);}
  
	void delDevice(int device_number){
		if ( m_conference_members.contains(device_number) ) 
			m_conference_members.removeAt(m_conference_members.indexOf(device_number));
	}
	void addDevice(int device_number){m_conference_members.append(device_number);}
	void setDirector(int device_number){this->m_director = device_number;}
	void setRadio1(int device_number){this->m_radio1 = device_number;}
	void setRadio2(int device_number){this->m_radio2 = device_number;}
	void setJupiter2(int device_number){this->m_jupiter2 = device_number;}
	char *dump();
	void delDirector(){m_director = 0;}
	void delRadio1(){m_radio1 = 0;}
	void delRadio2(){m_radio2 = 0;}
	void delJupiter2(){m_jupiter2 = 0;}
	int getDirector(){return this->m_director;}
	int getRadio1(){return this->m_radio1;}
	int getRadio2(){return this->m_radio2;}
	int getJupiter2(){return this->m_jupiter2;}
};//Conference

class VegaConfig
{
public:
	VegaConfig(const char *filepath,const char *cfgname)	{  
		m_filepath= filepath;
		m_configuration_name = cfgname;
	}
	QString m_filepath;
	QString m_configuration_name;
	void setNewName(const char *newName){m_configuration_name=newName;}
    const char *getName(){return m_configuration_name.toStdString().c_str();}
	bool saveGeneral(const char *fileName);


	
	Conference*							pCurrentConference;
	QMap<int,Conference*>				m_conferences;
	bool saveConferences(const char *fileName);
	void loadConferences(const char*fileName);
	int getNbRadioMembers(Conference* conf){
		int N=0;
		foreach(Device* pd, m_devices ) if (pd->getType() == DEVICE_RADIO && conf->m_conference_members.contains(pd->getNumber()) ) N++;  
		return N;
	}
	int getNbJupiter2Members(Conference* conf){
		int N=0;
		foreach(Device* pd, m_devices ) if (pd->getType() == DEVICE_JUPITER2   && conf->m_conference_members.contains(pd->getNumber()) ) N++;  
		return N;
	}


	QMap<int,Group*>					m_groups;
	bool saveGroups(const char *fileName);
	void loadGroups(const char*);
	Group *findGroupByName(const char *name){
		foreach(Group* pg, m_groups ) if (0==strcmp(pg->getName(), name)) return pg;
		return NULL;
	}
	Group *findGroupByNumber(int number){
		foreach(Group* pg, m_groups ) if (pg->getNumber() == number) return pg;
		return NULL;
	}

	QMap<int,DeviceUiElt*>				m_pDevicesUiElts;// = NULL;
	QMap<int,Device*>					m_devices;
	const char *getDeviceUiEltNameByNumber(int number){
		foreach(DeviceUiElt* pui, m_pDevicesUiElts ) 
			if ( pui->getDeviceNumber() == number)
				return pui->getName();
		return NULL;
	}
	int getDeviceUiEltNumberByName(const char*name){
		foreach(DeviceUiElt* pui, m_pDevicesUiElts ) 
			if (!strcmp( pui->getName(),name))
				return  pui->getDeviceNumber();
		return -1;
	}
	Device *getDeviceByNumber(int number){
		foreach(Device* pd, m_devices ) if (pd->getNumber() == number) return pd;  
		return NULL;
	}
	Device *getByName(const char *name){
		foreach(Device* pd, m_devices ) if (0==strcmp(pd->getName(), name)) return pd;  
		return NULL;
	}
	void loadDevices(const char *fileName);
	bool saveDevices(const char *fileName);

	DeviceKeysConfig				m_keysConfig;
	QMap<int,QString>				m_original_keymap;
	bool loadKeys(const char *fileName);
	bool saveKeys(const char *fileName);
	
	bool saveConfiguration(); 
	bool loadConfig();
	static VegaConfig *createConfigurationFromTemplate(const char*Dir);
	bool exportConfigurationFiles(const char *dst);
	const char *dumpConfigurationToStr();

	const char * jupiter2IsInOneConference(int device_number){
		foreach(Conference* pc, m_conferences ) if (pc->getJupiter2() == device_number) return pc->getName();
		return NULL;
	}
	const char * radioIsInOneConference(int device_number){
		foreach(Conference* pc, m_conferences ) if (pc->getRadio1() == device_number || pc->getRadio2() == device_number ) return pc->getName();
		return NULL;
	}

	Conference *getConferenceByName(const char *name){
		foreach(Conference* pc, m_conferences ) if (pc && (0==strcmp(pc->getName(), name) ) ) return pc;
		return NULL;
	}
	Conference *getConferenceByNumber(int n){
		foreach(Conference* pc, m_conferences ) if (pc && (n==pc->getNumber())) return pc;
		return NULL;
	}
	void deleteConferenceByNumber(int cn){
		//if ( m_conferences.contains(cn) ) 
		printf("\nAVANT deleteConferenceByNumber %d:\n",cn);
		foreach(Conference* pc, m_conferences ) 
			printf("C%d: %s\n", pc->getNumber(), pc->getName());
			m_conferences.remove(cn);
		printf("\nAPRES:");
		foreach(Conference* pc, m_conferences ) 
			printf("C%d: %s\n", pc->getNumber(), pc->getName());
	}

	bool deviceIsDirectorInOneConference(int device_number){
		foreach(Conference* pc, m_conferences ) if ( pc->getDirector() == device_number ) return true;
		return false;
	}
	int deviceIsInNbConferences(int device_number){
		int N=0;
		foreach(Conference* pc, m_conferences ) if ( pc->deviceIsMember(device_number) ) N++;
		return N;
	}

	int getNumber() { 
		return m_qlist.indexOf(this); // -1 si non present
		/*for ( int k=0;k<m_qlist.count() ; k++)  {
			if ( m_qlist[k]->m_configuration_name == this->m_configuration_name )
				return k;
		}*/
		return -1;
	}

	static void loadDirectory(const char *baseDir);
	static QVector<VegaConfig*>			m_qlist; // append OK
	static VegaConfig *					pRunning; // VegaConfig::pRunning
	static VegaConfig *					pCurrent;

};//VegaConfig

extern QList<Board*> *				pBoards;

extern void PropagateBoardStatusToDevices();
extern void PropagateBoardStatusToConferences();


#endif	    /* !CONFIGURATION_HH_ */
