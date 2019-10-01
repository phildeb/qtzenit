#include <iostream>
#include <QtGui>
#include <qlist.h>
#include <qstring.h>
#include <stdio.h>
#include <rude/config.h>
#include "switch.h"
#include "debug.h"
#include "configuration.h"

QList<Board*> *pBoards = NULL;

void Board::addConferenceDependence(int conference_number)
{
  this->m_conferencesDependencies.append(conference_number);
}
void Board::addDeviceDependence(int device_number)
{
  this->m_devicesDependencies.append(device_number);
}

bool Board::isDeviceDependant(int device_number)
{
  QList<int>::iterator i;

  for ( i=this->m_devicesDependencies.begin(); i != this->m_devicesDependencies.end(); i++ ) {
    if (device_number == *i)
      return true;
  }
  return false;

}
bool Board::isConferenceDependant(int conference_number)
{
  QList<int>::iterator i;

  for ( i=this->m_conferencesDependencies.begin(); i != this->m_conferencesDependencies.end(); i++ ) {
    if (conference_number == *i)
      return true;
  }
  return false;
}

const char *Board::dump()
{
  char desc[256] = { 0}; 
  QList<int>::iterator i;
  
  QString devDeps;
  QString confDeps;
  for (i = this->m_devicesDependencies.begin(); i != this->m_devicesDependencies.end(); i++ ) {
    if (i != this->m_devicesDependencies.begin())
      devDeps += ",";
    char number[4] = {0};
    sprintf(number, "%d", *i);
    devDeps += number;
  }
  for (i = this->m_conferencesDependencies.begin(); i != this->m_conferencesDependencies.end(); i++ ) {
    if (i != this->m_conferencesDependencies.begin())
      confDeps += ",";
    char number[4] = {0};
    sprintf(number, "%d", *i);
    confDeps += number;
  }
  
  sprintf(desc, "board number %d - type %s - presence : %s - devices dependencies : %s - conferences dependecies : %s",
	  this->m_place,
	  Board::typeToStr(this->m_type), 
	  this->m_present == false ? "false" : "yes", 
	  devDeps.toStdString().c_str(), 
	  confDeps.toStdString().c_str());
  
  return strdup(desc);
}

const char * Board::typeToStr(board_type_t type)
{
  switch (type) {
  case BOARD_ASLT:
    return BOARD_ASLT_STR;
    break;
  case BOARD_AGA:
    return BOARD_AGA_STR;
    break;
  case BOARD_ALPHACOM:
    return BOARD_ALPHACOM_STR;
    break;
  case BOARD_CLOCK:
    return BOARD_CLOCK_STR;
    break;
  }
  return "UNKNOWN";
}

QList<int> *Board::getConferenceDependencies()
{
  return new QList<int>(this->m_conferencesDependencies);
}


QList<int> *Board::getDeviceDependencies()
{
  return new QList<int>(this->m_devicesDependencies);
}

int Board::saveSwitchConfigToFile(QList<Board*> *boards)
{
  rude::Config config;
  config.setDelimiter('=');
  QList<Board*>::iterator d;
  
  
  for ( d=boards->begin(); d != boards->end(); d++ ) {
    Board *pb = *d;
    
    vega_log(VEGA_LOG_DEBUG, "saving board %d", pb->getPlace());
    
    char sectionName[3] = {0};

    sprintf(sectionName, "%d", pb->getPlace());
    
    config.setSection(sectionName);

    config.setStringValue("type", Board::typeToStr(pb->getType()));
    config.setIntValue("present", pb->isPresent() == true ? 1 : 0);
    
    QList<int>::iterator i;
    QList<int> *conferencesDeps = pb->getConferenceDependencies();
    QList<int> *devicesDeps = pb->getDeviceDependencies();

    QString str;

    if (conferencesDeps) {
      for ( i=conferencesDeps->begin(); i != conferencesDeps->end(); i++ ) {
	if (i != conferencesDeps->begin())
	str += ",";
	char number[4] = { 0 };
	sprintf(number, "%d", *i);
	str += number;
      }
      
      if (str != "")
	config.setStringValue("confdeps", str.toStdString().c_str());
      delete conferencesDeps;
    }
    str = "";
    if (devicesDeps) {

      for ( i=devicesDeps->begin(); i != devicesDeps->end(); i++ ) {
	if (i != devicesDeps->begin())
	  str += ",";
	char number[4] = { 0 };
	sprintf(number, "%d", *i);
	str += number;
      }
      if (str != "")
	config.setStringValue("devdeps", str.toStdString().c_str());
      delete devicesDeps;
    }
    
   
  }
  config.save(SWITCH_CONFIG_FILE);
  return 0;
}



QList<Board*> *Board::loadSwitchConfigFromFile()
{
	//dbgBox("Board::loadSwitchConfigFromFile");

  QList<Board*> *boards = new QList<Board*>();
  rude::Config config;
  int nbSections;
  int i;

  if (access(SWITCH_CONFIG_FILE, R_OK) == -1) {
    dbgBox( "cannot access file %s",SWITCH_CONFIG_FILE);
    return NULL;
  }
  
  if (config.load(SWITCH_CONFIG_FILE) == false) {
    dbgBox( "cannot load %s file", SWITCH_CONFIG_FILE);
    return NULL;
  }

  nbSections = config.getNumSections();

  for (i = 1; i < nbSections; i++) 
  {
    Board *pb = new Board();

    config.setSection(config.getSectionNameAt(i));
    // place 
    pb->setPlace(atoi(config.getSectionNameAt(i)));
    // type de carte
    const char *boardType =  config.getStringValue("type");

    if (!strcmp(boardType, BOARD_ASLT_STR)) 
      pb->setType(BOARD_ASLT);
    else if (!strcmp(boardType, BOARD_AGA_STR))
      pb->setType(BOARD_AGA);
    else if (!strcmp(boardType, BOARD_ALPHACOM_STR))
      pb->setType(BOARD_ALPHACOM);
    else if (!strcmp(boardType, BOARD_CLOCK_STR))
      pb->setType(BOARD_CLOCK);
    
    // presence
    pb->setPresent(config.getIntValue("present"));
    // conference dependencies
    char *confs = strdup(config.getStringValue("confdeps"));

    if (strlen(confs)) {

      char *conf = strtok(confs, ",");
      while (conf != NULL) {

		pb->addConferenceDependence(atoi(conf));
		conf = strtok(NULL, ",");
      }
    }  
    // devices dependencies
    char *devices = strdup(config.getStringValue("devdeps"));
    if (strlen(devices)) 
	{
      char *device = strtok(devices, ",");
      while (device != NULL) {
		pb->addDeviceDependence(atoi(device));
		device = strtok(NULL, ",");
      }
    }
    
	//dbgBox( "boards->append %s %s",config.getSectionNameAt(i), config.getStringValue("devdeps") );
    boards->append(pb);
 //     char * desc = pb->dump();
//     printf("%s\n", desc);
//     free (desc);
  }
  
  return boards;
}

void PropagateBoardStatusToDevices()
{/*
  QList<Board*>::iterator ib;
  QList<int> *pDeviceDeps = NULL;
  QList<int>::iterator i;


  if ( pBoards ) 
  for (ib = pBoards->begin(); ib != pBoards->end(); ib ++) 
  {
    Board *pB = *ib;
    if (pB && pB->isPresent() == false) 
	{
		//dbgBox("---- Propagate board %d status to devices ----", pB->getPlace());
      
		pDeviceDeps = pB->getDeviceDependencies();
      for (i = pDeviceDeps->begin(); i != pDeviceDeps->end(); i++) 
	  {	
		Device *pD = Device::getDeviceByNumber(pDevices, *i);
		if ( pD)  pD->setDisabled();
			//vega_log(VEGA_LOG_DEBUG, "device %d is %d", pD->getNumber(), pD->isEnabled());
      }
   
    }
  }*/
}

void PropagateBoardStatusToConferences()
{
	/*
  QList<Board*>::iterator ib;
  QList<int> *pConferenceDeps = NULL;
  QList<int>::iterator i;

  if ( pBoards ) 
  for (ib = pBoards->begin(); ib != pBoards->end(); ib ++) 
  {
    Board *pB = *ib;  
    if (pB->isPresent() == false) 
	{    
      //dbgBox("---- Propagate board %d status to conferences ----", pB->getPlace());
      
      pConferenceDeps = pB->getConferenceDependencies();
      if (pConferenceDeps) {
			for (i = pConferenceDeps->begin(); i != pConferenceDeps->end(); i++) {
				dbgBox( "conference %d is now disabled", *i);
				if ( (*i - 1 )<=9 ) conferencesAvailability[*i - 1] = 0;
			}      
			delete pConferenceDeps;
			pConferenceDeps = NULL;
      }
    }
  }
  */
} 