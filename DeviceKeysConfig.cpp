#include <iostream>
#include <errno.h>
#include <QtGui>
#include "switch.h"
#include "debug.h"
#include "configuration.h"
#include "rude/config.h"

DeviceKeysConfig::DeviceKeysConfig()
{
  for (int i = 0; i < DEVICE_CRM4_NB_KEYS; i++) {
    keyConfs[i].action = ACTION_INVALID;
    keyConfs[i].key_value = -1;
  }
}

bool DeviceKeysConfig::configureKey(int number, key_action_t action, int value)
{
  vega_log(VEGA_LOG_DEBUG, "configure key %d action = %s value = %d", number, actionToStr(action), value);
  this->keyConfs[number-1].action = action;
  this->keyConfs[number-1].key_value = value;
  return true;
}

key_action_t DeviceKeysConfig::getKeyAction(int number)
{
  return this->keyConfs[number-1].action;
}

int DeviceKeysConfig::getKeyValue(int number)
{
  return this->keyConfs[number-1].key_value;
}

key_configuration_t DeviceKeysConfig::getKeyConfiguration(int number)
{
  return this->keyConfs[number-1];
}

const char *DeviceKeysConfig::actionToStr(int action)
{
	switch (action) {
		case ACTION_ACTIVATE_CONF:return "activate conf";break;
		case ACTION_SELECT_CONF: return "select conf";break;
		case ACTION_SELECT_DEVICE:return "select device";break;
		case ACTION_ON_OFF_CONF:return "On/Off";break;
		case ACTION_EXCLUDE_INCLUDE:return "request exclude/include";break;
		case ACTION_GENERAL_CALL:return "general call";break;
		case ACTION_GROUP_CALL:return "group call";break;
		default:return "UNKNOWN action";break;
	}
}

const char* DeviceKeysConfig::dump()
{
	char desc[4096] = {0};
	for (int i = 0 ; i < DEVICE_CRM4_NB_KEYS; i++) {
		sprintf(desc + strlen(desc), "key %d :", i+1);
		sprintf(desc + strlen(desc), "action %s - value %d\n", actionToStr(keyConfs[i].action), keyConfs[i].key_value);
	}
	return strdup(desc);
}

/*[keys]
1 = action_activate_conf
2 = action_on_off_conf
4 = action_exclude_include_conf
5 = action_select_conf,1
6 = action_select_conf,2
7 = action_select_conf,3
8 = action_select_conf,4
9 = action_select_conf,5
10 = action_select_conf,6
11 = action_select_conf,7
12 = action_select_conf,8
13 = action_select_conf,9
14 = action_select_conf,10
15 = action_select_device,1
16 = action_select_device,2
17 = action_select_device,3
18 = action_select_device,4
19 = action_select_device,5
20 = action_select_device,6
21 = action_select_device,7
22 = action_select_device,8
23 = action_select_device,9
24 = action_select_device,10
25 = action_select_device,11
26 = action_select_device,12
27 = action_select_device,25
28 = action_select_device,26
29 = action_select_device,27
30 = action_select_device,28
38 = action_general_call*/
bool DeviceKeysConfig::saveDeviceKeys(const char *fileName)
{
	if ( NULL == VegaConfig::pCurrent ) return false;

	rude::Config config;
	if (config.load(fileName) == false) {
		dbgBox("DeviceKeysConfig::loadKeys cannot load %s file", fileName);
		return NULL;
	}

	config.setSection("keys");
	config.setStringValue("lastload", __TIME__);

	for (int i = 1; i <= 38; i++)  
	{
		QString strK = QString("%1").arg(i);
		//m_original_keymap[i] = elt2; // pour sauvegarde a l'identique ( non modifiable par IHM ) 
		config.setStringValue(qPrintable(strK),VegaConfig::pCurrent->m_original_keymap[i]);//"action_activate_conf");
		continue;
/*
		if (keyConfs[i].action == ACTION_INVALID)
			continue;


		//     printf("key = %d action  = %d value = %d\n", i, keyConfs[i].action, keyConfs[i].key_value);
		switch (keyConfs[i-1].action) 
		{
		case ACTION_ACTIVATE_CONF:
		  config.setStringValue(qPrintable(strK),"action_activate_conf");
		  break;
		case ACTION_SELECT_CONF:{
				QString s;s.sprintf("action_select_conf,%d",keyConfs[i-1].key_value);
				config.setStringValue(qPrintable(strK), qPrintable(s));
			}
			break;
		case ACTION_SELECT_DEVICE:{
				QString s;s.sprintf("action_select_device,%d",keyConfs[i-1].key_value);
				config.setStringValue(qPrintable(strK), qPrintable(s));
			}
			break;
		case ACTION_ON_OFF_CONF:
		  config.setStringValue(qPrintable(strK), "action_on_off_conf");
		  break;
		case ACTION_EXCLUDE_INCLUDE:
		  config.setStringValue(qPrintable(strK), "action_exclude_include_conf");
		  break;
		case ACTION_GENERAL_CALL:
		  config.setStringValue(qPrintable(strK), "action_general_call");
		  break;
		//case ACTION_GROUP_CALL:
		  //config.setStringValue(qPrintable(strK),"action_group_call,%d\n", keyConfs[i].key_value);
		  break;
		default:
		  break;
		}*/
	}
	return config.save();
}


bool VegaConfig::saveKeys(const char *fileName)
{
	//if ( VegaConfig::pCurrent )		VegaConfig::pCurrent->m_keysConfig.saveDeviceKeys(fileName);
	if ( NULL == VegaConfig::pCurrent ) return false;

	rude::Config config;
	if (config.load(fileName) == false) {
		dbgBox("DeviceKeysConfig::loadKeys cannot load %s file", fileName);
		return NULL;
	}

	config.setSection("keys");
	config.setStringValue("lastload", __TIME__);

	for (int i = 1; i <= 38; i++)  
	{
		QString strK = QString("%1").arg(i);
		//m_original_keymap[i] = elt2; // pour sauvegarde a l'identique ( non modifiable par IHM ) 
		config.setStringValue(qPrintable(strK),VegaConfig::pCurrent->m_original_keymap[i]);//"action_activate_conf");
	}

	return config.save();
}

bool VegaConfig::loadKeys(const char *fileName)
{
	//dbgBox("VegaConfig::loadKeys");
	rude::Config config;
	if (config.load(fileName) == false) {
		dbgBox("DeviceKeysConfig::loadKeys cannot load %s file", fileName);
		return NULL;
	}

	config.setSection("keys");
	config.setStringValue("lastload", __TIME__);

	for (int i = 1; i <= 38; i++)  
	{
		QString strK = QString("%1").arg(i);
		int key_number = i;
		char *elt2 = strdup(config.getStringValue(strK)); // ce qu'il y a apres le "="

		vega_log(VEGA_LOG_INFO, "key_number=%d elt2=%s\n", key_number, elt2);
		fprintf(stderr,"key_number=%d elt2=%s\n", key_number, elt2);

		m_original_keymap[i] = elt2; // pour sauvegarde a l'identique ( non modifiable par IHM ) 

		if (strstr(elt2, "action_activate_conf")) {
      
		  m_keysConfig.keyConfs[key_number-1].action = ACTION_ACTIVATE_CONF ;
		  m_keysConfig.keyConfs[key_number-1].key_value = -1;
		}
		else if (strstr(elt2, "action_on_off_conf")) {
		  m_keysConfig.keyConfs[key_number-1].action = ACTION_ON_OFF_CONF;
		  m_keysConfig.keyConfs[key_number-1].key_value = -1;
		}
		else if (strstr(elt2, "action_exclude_include_conf")) {
		  m_keysConfig.keyConfs[key_number-1].action = ACTION_EXCLUDE_INCLUDE ;
		  m_keysConfig.keyConfs[key_number-1].key_value = -1;
		}
		else if (strstr(elt2, "action_select_conf")) {

	//       vega_log(VEGA_LOG_INFO, "action_select_conf %d %d", nb_keys, key_number );
		  m_keysConfig.keyConfs[key_number-1].action = ACTION_SELECT_CONF;
		  char* e1 = strtok(elt2, ",");
		  char* e2 = strtok(NULL, ",");
		  m_keysConfig.keyConfs[key_number-1].key_value = atoi(e2);
		}
		else if(strstr(elt2, "action_select_device")) {
		  m_keysConfig.keyConfs[key_number-1].action = ACTION_SELECT_DEVICE;
		  char* e1 = strtok(elt2, ",");
		  char* e2 = strtok(NULL, ",");
		  m_keysConfig.keyConfs[key_number-1].key_value = atoi(e2);
		}
		/*else if(strstr(elt2, "action_group_call")) {
		  m_keysConfig.keyConfs[key_number-1].action = ACTION_GROUP_CALL;
		  char* e1 = strtok(elt2, ",");
		  char* e2 = strtok(NULL, ",");
		  m_keysConfig.keyConfs[key_number-1].key_value = atoi(e2);
		}*/
		else if(strstr(elt2, "action_general_call")) {
		  m_keysConfig.keyConfs[key_number-1].action = ACTION_GENERAL_CALL;
		  m_keysConfig.keyConfs[key_number-1].key_value = -1;
		}
	}// for
	return config.save();
}
