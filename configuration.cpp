#include <QtGui>
#include <sys/stat.h>
#include <dirent.h>
#include <rude/config.h>

#include "switch.h"
#include "debug.h"
#include "configuration.h"
#include "debug.h"
#include "controlLink.h"

void VegaConfig::loadGroups(const char *fileName)
{
	rude::Config config;
	if (config.load(fileName) == false) {
		dbgBox("cannot load %s file", fileName);
		return ;
	}
	for (int i = 1; i <= 10; i++)  // MAX_GROUPS
	{
		QString strG = QString("G%1").arg(i);
        if ( true == config.setSection(strG.toStdString().c_str(),false) )
		{
			Group *pG = new Group();
			m_groups[i]=(pG);	
			pG->setNumber(i);

			if ( config.getStringValue("name") )
				pG->setGroupName(config.getStringValue("name") );
			else
                pG->setGroupName(strG.toStdString().c_str());

			char *devices = strdup(config.getStringValue("members"));
			char *device = strtok(devices, ",");
			while (device) {
				pG->addMember(atoi(device));
				device = strtok(NULL, ",");
			}
            //dbgBox("append %s number %d", pG->m_name.toStdString().c_str(), pG->m_number);
		}
	}
}


char* Group::dump()
{
/*
  rude::Config config;
  QString membersStr;
  QList<int>::iterator i;
  char desc[512];
  
  for (i = m_members.begin(); i != m_members.end(); i++ ) {
      if (i != m_members.begin())
		membersStr += ", ";
      
      Device *pDevice = Device::getDeviceByNumber(pDevices, *i);
      char deviceName[32] = {0};
      sprintf(deviceName, "%s", pDevice->getName());
      membersStr += deviceName;
  }
  sprintf(desc, "[%s]\nmembres=%s\n", getName(), membersStr.toStdString().c_str());
  return strdup(desc);
*/
}

bool VegaConfig::saveGroups(const char *fileName)
{
	rude::Config config;
	if (config.load(fileName) == false) {
		dbgBox("saveGroups: cannot load %s file", fileName);
		return false;
	}
	
	if (1)
	{
		foreach(Group *pG, m_groups)
		{
			if ( pG == NULL)  continue;
			fprintf(stderr,"saving G%d\n", pG->getNumber());

			char section[128];
			sprintf(section,"G%d",pG->getNumber());
			config.setSection(section);
			config.setIntValue("number", pG->getNumber());
			config.setStringValue("name", pG->getName());

			QString membersStr;
			membersStr = "";

			QList<int> *pMembers = NULL;
			pMembers = pG->getMembers();

			if (pMembers->count()) 
			{
				  QList<int>::iterator i;
					for (i = pMembers->begin(); i != pMembers->end(); i++ ) 
					{
						if (i != pMembers->begin())		  membersStr += ",";

						char number[4] = {0};
						sprintf(number, "%d", *i);
						membersStr += number;
					}
                    config.setStringValue("members" , membersStr.toStdString().c_str());
			}
		}
	}
	return config.save(fileName);
}


char *Device::dumpKeysConfiguration()
{
  int i;
  char conf[128] = {0};
  
  for (i = 0; i < 10; i++) {
    if (m_keys[i].number == 0) 
      continue;
    
    if (strlen(conf)>0) 
      strcat(conf, ";");
    
    sprintf(conf + strlen(conf), "%d,%d", m_keys[i].number, m_keys[i].group_number);
  }
  
 if (!strcmp(conf, ""))
   return NULL;

 return strdup(conf);
}

bool Device::configureKey(int key_number, key_action_t action, int group_number)
{
  int i;
  if (action == ACTION_NONE) {
    for (i = 0; i < 10; i++) {
      if (m_keys[i].number == key_number) {
	m_keys[i].group_number = 0;
	m_keys[i].number = 0;
	return true;
      }
    }
    return false;
  }
  for (i = 0; i < 10; i++) {
    if (m_keys[i].number == key_number) {
      m_keys[i].group_number = group_number;
      return true;
    }
  }
  for (i = 0; i < 10; i++) {
    if (m_keys[i].number == 0) {
      m_keys[i].number = key_number;
      m_keys[i].group_number = group_number;
      return true;
    }
  }
  return false;
}

int Device::getGroupNumberByKeyNumber(int key_number)
{
  for (int i = 0; i < 10; i++)  
    if (m_keys[i].number == key_number)
      return m_keys[i].group_number;
       
  return -1;
}

int Device::getKeyNumberByGroupNumber(int group_number)
{
  for (int i = 0; i < 10; i++)  
    if (m_keys[i].group_number == group_number)
      return m_keys[i].number;
       
  return -1;
}

const char *Device::typeToStr(device_type_t type)
{
  switch (type) {
	  case DEVICE_CRM4:   return "CRM4";    break;
	  case DEVICE_RADIO:    return "RADIO";    break;
	  case DEVICE_JUPITER2:    return "JUPITER";    break;
	  default:    return "UNKNOWN"; break;
  }
  return "UNKNOWN";
}

char *Device::dump()
{
  char desc[512] = { 0 };
  sprintf(desc, "[%s]\n", getName());
  sprintf(desc + strlen(desc), "numero=%d\n", getNumber());
  sprintf(desc + strlen(desc), "type=%s\n", Device::typeToStr(this->getType()));
  switch (getType()) {
  case DEVICE_CRM4: 
    {
      sprintf(desc + strlen(desc), "gain=%d\n", getGain());
      sprintf(desc + strlen(desc), "touches=");
      int k = 0;
      int i = 0;
      for (i = 0; i < 10; i++) {
	if (m_keys[i].number == 0) 
	  continue;
	
	if (k == 1) 
	  strcat(desc + strlen(desc), ";");
	k = 1;

	/*Group *pGroup = Group::findGroupByNumber(pGroups, m_keys[i].group_number);
	if (pGroup)
	  sprintf(desc + strlen(desc), "%d:%s", m_keys[i].number, pGroup->getName());*/
      }
      printf("\n");
    }
    break;
  case DEVICE_JUPITER2:
    sprintf(desc + strlen(desc), "full_duplex=%d\n", getFullDuplex());
    break;
  case DEVICE_RADIO:
    sprintf(desc + strlen(desc), "full_duplex=%d\n", getFullDuplex());
    sprintf(desc + strlen(desc), "detection_audio=%d\n", getContactSec());
    break;
  default:
    break;
  }
  return strdup(desc);
}


bool VegaConfig::saveDevices(const char *fileName)
{
	rude::Config config;
	config.setDelimiter('=');
	if (config.load(fileName) == false) {
		dbgBox("saveGroups: cannot load %s file\n", fileName);
		return NULL;
	}

	//QList<Device*>::iterator d;
	//for ( d=devices->begin(); d != devices->end(); d++ ) 
	foreach(Device *pd, m_devices)
	{
		//Device *pd = *d;
		if ( NULL == pd ) continue;
		fprintf(stderr,"saving device D%d (%s)\n", pd->getNumber(),pd->getName());

		QString strsection = QString("D%1").arg(pd->getNumber());

        config.setSection(strsection.toStdString().c_str() );

		config.setIntValue("number", pd->getNumber());

		config.setStringValue("name", pd->getName());
    
		if (pd->getNumber() <= 24) 
		{
				config.setStringValue("type", DEVICE_CRM4_STR);
				config.setIntValue("gain", pd->getGain());      
		}
		else if (pd->getNumber() <= 35) 
		{
				config.setStringValue("type", DEVICE_RADIO_STR);
				config.setIntValue("full_duplex", pd->getFullDuplex() == true ? 1 : 0);
				config.setIntValue("contact_sec", pd->getContactSec() == true ? 1 : 0);
		}
		else //if (pd->getType() == DEVICE_JUPITER2) 
		{
				config.setStringValue("type", DEVICE_JUPITER2_STR);
				config.setIntValue("contact_sec", pd->getContactSec() == true ? 1 : 0);
		}
    
		char *keysConfig = pd->dumpKeysConfiguration();
		if (keysConfig) {
			config.setStringValue("keys", keysConfig);
			free(keysConfig);
		}
  }
  config.save();//fileName);
  return true;
}



void VegaConfig::loadDevices(const char *fileName)
{
  rude::Config config;
  if (config.load(fileName) == false) {
    dbgBox("cannot load %s file", fileName);
    return ;
  }
  for (int i = 1; i <= 46; i++) 
  {
	QString str = QString("D%1").arg(i);
    //dbgBox(str.toStdString().c_str());
    //config.setSection(config.getSectionNameAt(i));
    if ( false == config.setSection(str.toStdString().c_str(),false) ) continue;
    
	//fprintf(stderr,"loadDevices D%d(%s)\n", i, config.getStringValue("nom"));
	
    //dbgBox(str.toStdString().c_str());
    
	Device *d = new Device();
	m_devices[i] = d;
  	d->m_number = i;
  
    //d->setName(config.getSectionNameAt(i));
	if ( strlen(config.getStringValue("name")) > 0 ) 
		d->setDeviceName(config.getStringValue("name"));
	else 
        d->setDeviceName(str.toStdString().c_str());
    
	fprintf(stderr,"D%d name=(%s)\n",d->m_number, d->getName());

    //d->setNumber(config.getIntValue("number"));
    
    if ( i>=25 && i<=35 ) //!strcmp(config.getStringValue("type"), DEVICE_RADIO_STR)) 
	{
      d->setType(DEVICE_RADIO);
      d->setFullDuplex(config.getIntValue("full_duplex") == 1 ? true : false);
      d->setContactSec(config.getIntValue("contact_sec") == 1 ? true: false);
    }
    else if ( i>= 36 ) //if (!strcmp(config.getStringValue("type"), DEVICE_JUPITER2_STR)) 
	{
		d->setType(DEVICE_JUPITER2);
      d->setContactSec(config.getIntValue("contact_sec") == 1 ? true : false);
    }
    else //if (!strcmp(config.getStringValue("type"), DEVICE_CRM4_STR)) 
	{
      d->setType(DEVICE_CRM4);
      d->setGain(config.getIntValue("gain"));
    }

    const char *keysConfig = config.getStringValue("keys");
    if (keysConfig && strlen (keysConfig)  > 1) 
	{
      char number[4] = { 0 };
      int keyValue = 0;
      int groupValue;      
      int i;

      for (i = 0; i < (int) strlen(keysConfig); i++)
	  {
		if (keysConfig[i] == ';') 
		{
		  groupValue = atoi(number);
		  if (keyValue && groupValue)
			d->configureKey(keyValue, ACTION_GROUP_CALL, groupValue);

		  number[0] = 0;
		}
		else if (keysConfig[i] == ',') 
		{
		  keyValue = atoi(number);
		  number[0] = 0;
		}
		else if (isdigit(keysConfig[i])) 
		  sprintf(number + strlen(number), "%c", keysConfig[i]);
		 
	  }


		  groupValue = atoi(number);
		  if (groupValue && keyValue) d->configureKey(keyValue, ACTION_GROUP_CALL, groupValue);
    }

    
  }
}

VegaConfig *VegaConfig::createConfigurationFromTemplate(const char*Dir)
{
	char dirtemplate[1024]={0};
	sprintf(dirtemplate, "%s/%s", CONFIGURATION_DIR, TEMPLATE);

	VegaConfig *pNewConfiguration = new VegaConfig(dirtemplate,"nouvelleconfig200");
	if (pNewConfiguration->loadConfig() == false) {
		delete pNewConfiguration;
		return NULL;
	}else{
		pNewConfiguration->m_filepath = Dir;
        //dbgBox("createConfigurationFromTemplate: m_dir=%s",pNewConfiguration->m_filepath.toStdString().c_str());
	}
	//???? pNewConfiguration->exportConfigurationFiles(Dir);//CopyConfigurationAndSetDirectory(Dir);
	return pNewConfiguration;
}

bool VegaConfig::exportConfigurationFiles(const char *dst)
{ 
    dbgBox("fichier %s exportConfigurationFiles TO %s",m_filepath.toStdString().c_str(),dst);
	
	// effacer le fichier pour supprimer les conferences , groupes, terminaux detruits
	char cmd[1024];
	sprintf(cmd,"rm %s;",dst);
	//dbgBox(cmd);
	int ret = system(cmd);
	fprintf(stderr,"ret:%d cmd %s\n", ret,cmd );

	saveConferences(dst);
	saveDevices(dst);
	saveGroups( dst);
	saveKeys( dst);
	saveGeneral( dst );
	return true;
}

/*bool VegaConfig::destroyConfigurationFiles()
{
	char cmd[1024]={0};
    sprintf(cmd,"rm %s;", m_filepath.toStdString().c_str() );
	int ret = system(cmd);
	//dbgBox(cmd);
	fprintf(stderr,"ret:%d cmd %s\n", ret,cmd );
	return true;
}*/

bool VegaConfig::saveConfiguration()
{
	//dbgBox("VegaConfig::saveConfiguration %s",VegaConfig::pCurrent->getName());
	fprintf(stderr,"VegaConfig::saveConfiguration %s\n", qPrintable(m_configuration_name) );
	fprintf(stderr,"VegaConfig::saveConfiguration %s\n", VegaConfig::pCurrent->getName() );

	char cmd[1024];
    sprintf(cmd,"rm %s;",m_filepath.toStdString().c_str());
	//dbgBox(cmd);
	int ret = system(cmd);
	fprintf(stderr,"ret:%d cmd %s\n", ret,cmd );

    saveConferences(m_filepath.toStdString().c_str());
    saveDevices(m_filepath.toStdString().c_str());
    saveGroups( m_filepath.toStdString().c_str());
    saveKeys( m_filepath.toStdString().c_str());
    saveGeneral(m_filepath.toStdString().c_str());
	
	return true;
}

bool VegaConfig::saveGeneral(const char *fileName)
{
	rude::Config config;
	if (config.load(fileName) == false) {
		dbgBox("cannot load %s file", fileName);
		return false;
	}
	config.setSection("general");
	config.setStringValue("name", qPrintable(m_configuration_name) );
	config.setStringValue("ihm-version", __DATE__" "__TIME__);
	return config.save();
}

const char*VegaConfig::dumpConfigurationToStr()
{
  char str[10000] = { 0 };
 /* QList<Device*>::iterator id;
  QList<VegaConfig*>::iterator ic;
  QList<Group*>::iterator ig;
  Conference *pConferenceConf = NULL;
  int i;

  sprintf(str, "------- configuration : %s -------\n", qPrintable(this->m_filepath)) ;
  sprintf(str + strlen(str), "----------- devices -----------\n");

  for (id = m_devices->begin() ; id != m_devices->end(); id ++) {
    char *d = (*id)->dump();
    sprintf(str + strlen(str), "%s\n\n", d);
    free (d);
  }

  sprintf(str + strlen(str), "-------- conferences ----------\n");
  
  for (i = 1 ; i < 11 ; i++) {
    pConferenceConf = m_conferences->getConferenceByNumber(i);
    if (pConferenceConf == NULL) 
      continue;
    char *d = pConferenceConf->dump();
    sprintf(str + strlen(str), "%s\n", d);
    free (d);
  }

  sprintf(str + strlen(str), "----------- groupes -----------\n");

  for (ig = m_groups->begin() ; ig != m_groups->end(); ig ++) {
    char *d = (*ig)->dump();
    sprintf(str + strlen(str), "%s\n", d);
    free (d);
  }
*/
  return strdup(str);
}

bool VegaConfig::loadConfig()
{
	{
		rude::Config config;
        if (config.load(m_filepath.toStdString().c_str()) == false) {
			dbgBox("cannot load %s file", qPrintable(m_filepath) );
			return false;
		}
		if ( true == config.setSection("general",false) ) {
			if ( strlen(config.getStringValue("name")) > 0 ) 
				m_configuration_name = config.getStringValue("name");
			else 
				m_configuration_name = m_filepath;
		}
		//dbgBox(m_configuration_name);
	}

    loadConferences(m_filepath.toStdString().c_str()) ;
	// NEW a l'interieur !!! VConferences *conferences = new VConferences();
    loadDevices(m_filepath.toStdString().c_str());
    loadGroups(m_filepath.toStdString().c_str());
    loadKeys(m_filepath.toStdString().c_str());//CONFIGURATION_DIR"/"DEVICE_KEYS_FILE);
	return true;
}


char *Conference::dump()
{
	/*
  char desc[512] = {0};
  QList<int>::iterator i;
  QString membersStr;
  
  for (i = this->m_devices.begin(); i != this->m_devices.end(); i++		) {
    if (i != this->m_devices.begin())
      membersStr += ", ";
    Device *pDevice = Device::getDeviceByNumber(pDevices, *i);
    char number[32] = {0};
    sprintf(number, "%s", pDevice->getName());
    membersStr += number;
  }
 
  Device *pDirector = Device::getDeviceByNumber(pDevices, this->m_director);
  Device *pRadio1 = Device::getDeviceByNumber(pDevices, this->m_radio1);
  Device *pRadio2 = Device::getDeviceByNumber(pDevices, this->m_radio2);
  Device *pJupiter2 = Device::getDeviceByNumber(pDevices, this->m_jupiter2);
  
  sprintf(desc, "[%s]\nnumero=%d\nmembres=%s\ndirecteur=%s\nradio1=%s\nradio2=%s\njupiter2=%s\n",
      this->m_conference_name.toStdString().c_str(),
	  getNumber(), 
      membersStr.toStdString().c_str(),
	  pDirector == NULL ? "aucun" : pDirector->getName(),
	  pRadio1 == NULL ? "aucun" : pRadio1->getName(), 
	  pRadio2 == NULL ? "aucun" : pRadio2->getName(),
	  pJupiter2 == NULL ? "aucun" : pJupiter2->getName());
  return strdup(desc);
  */
}

////////////////////////////////////////////////////////////////////////////////////

// VConferences

////////////////////////////////////////////////////////////////////////////////////
bool VegaConfig::saveConferences(const char *fileName)
{
	rude::Config config;
	config.load(fileName);
	foreach(Conference *pc,m_conferences)
	{
		if ( pc ==NULL ) continue;

		char strsection[128]={0};
		sprintf(strsection,"C%d",pc->m_number);
		config.setSection(strsection);

        fprintf(stderr,"VegaConfig::saveConferences [%s] [%s] file[%s]\n",pc->m_conference_name.toStdString().c_str(), strsection, fileName);
    
		QString membersStr;
		int i=0;
		foreach( int nodev, pc->m_conference_members ) {
			if (i++ != 0 ) membersStr += ",";
			membersStr += QString::number(nodev);
		}

		//dbgBox("saveConferences C%d: devices=%s\n", pc->getNumber(), membersStr.toStdString().c_str() );
		fprintf(stderr,"saveConferences C%d: devices=%s\n", pc->getNumber(), membersStr.toStdString().c_str() );
		fprintf(stderr,"director %d radio1 %d\n",
			 pc->getDirector(),
			 pc->getRadio1());
		
		fprintf(stderr,"jupiter %d radio2 %d\n",
			 pc->getJupiter2(),
			 pc->getRadio2());
		

		config.setStringValue("devices" , membersStr.toStdString().c_str() );
		config.setStringValue("name" , pc->m_conference_name.toStdString().c_str() );

		config.setIntValue("director", pc->getDirector());
		config.setIntValue("radio1", pc->getRadio1());
		config.setIntValue("radio2", pc->getRadio2());
		config.setIntValue("jupiter", pc->getJupiter2());   
		config.setIntValue("number", pc->getNumber());

	}  
	config.save();
	return true;
}

void VegaConfig::loadConferences(const char*fileName)
{
	//dbgBox(fileName);
	rude::Config config;
	if (config.load(fileName) == false) {
		dbgBox("cannot load %s file", fileName);
		return;
	}
	config.setSection("general");
	config.setStringValue("lastload", __TIME__);

	for (int i = 1; i < 10; i++)
	{
		QString strConf = QString("C%1").arg(i);
        if ( false == config.setSection(strConf.toStdString().c_str(),false) ) continue;

        Conference *pc = new Conference(strConf.toStdString().c_str());//"unknown");
		m_conferences[i]=pc;// ajoute la conference a la liste
		pc->setNumber(i);

        //dbgBox(strConf.toStdString().c_str());
		//dbgBox(config.getStringValue("name"));
		//dbgBox(config.getStringValue("devices"));
		//pc->setName(config.getStringValue("name"));
		if ( NULL != config.getStringValue("name") ) {;
			//dbgBox(config.getStringValue("name"));
			pc->m_conference_name = config.getStringValue("name");
		}

		char *devices = strdup(config.getStringValue("devices"));
		char *device = strtok(devices, ",");
		while (device != NULL){
			pc->addDevice(atoi(device)); // lecture des devices !
			device = strtok(NULL, ",");
		}

		if (devices)
			free (devices); // todo: utiliser QString::split
    
		//config.getIntValue("number")); 
		pc->setDirector(config.getIntValue("director"));
		pc->setRadio1(config.getIntValue("radio1"));
		pc->setRadio2(config.getIntValue("radio2"));
		pc->setJupiter2(config.getIntValue("jupiter"));
		
		QString msgb = QString("Ajout de la Conference %1  membres %2").arg(pc->m_conference_name).arg(config.getStringValue("devices"));
		//dbgBox(msgb);
		qDebug()<< msgb<<endl;
		
	}
	config.save();

	return; 
}




