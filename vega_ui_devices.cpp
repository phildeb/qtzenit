#include <iostream>
#include "vega_ihm.h"

#include "switch.h"
#include "debug.h"
#include "configuration.h"
#include "printer.h"
#include "liveEvents.h"

static QPushButton *				pCurrentButtonKeyToConfigure	= NULL;
static int							currentDeviceKeyNumberToConfigure = 0;
static Device *						pCurrentDeviceKeysInEdition;

char Device::deviceGains[MAX_GAINS] ={-14,-12,-10,-8,-6,-4,-2,0,2,4,6,8,10,12,14};

void Ui_MainWindow::showDeviceKeysMapConfiguration()
{
	fprintf(stderr, "Ui_MainWindow::showDeviceKeysMapConfiguration\n");
	if ( VegaConfig::pCurrent==NULL ) return;

	// affichage du text sur les 38 premiers boutons ( C1... C8 ... appel general en 38 )
	for (int k = 1; k <= 38; k++)
	{
		key_action_t action = VegaConfig::pCurrent->m_keysConfig.getKeyAction(k);
		int value = VegaConfig::pCurrent->m_keysConfig.getKeyValue(k);

		char objName[128];	sprintf(objName, "btnCRM4Key%d", k);
		QPushButton *pPushButton = (QPushButton*)groupBoxMapCRM4->child(objName) ; //centralwidget->child(objName);
    
		if (pPushButton == NULL)
			continue;
		if (action == ACTION_INVALID)
			continue;
    
		char buttonText[256] = {0};

		switch (action) 
		{
		case ACTION_ACTIVATE_CONF:
			sprintf(buttonText, "Act");
			pPushButton->setText(buttonText);
		break;
		case  ACTION_SELECT_CONF:
			sprintf(buttonText, "C%d", value);
			pPushButton->setText(buttonText);
		break;
		case ACTION_ON_OFF_CONF:
			sprintf(buttonText, "On/Off");
			pPushButton->setText(buttonText);
		break;
		case ACTION_SELECT_DEVICE:
		{
			if (VegaConfig::pCurrent) 
			{
				Device *d = VegaConfig::pCurrent->getDeviceByNumber(value);//Device::getDeviceByNumber(pDevices, value);
				if (d) {
					sprintf(buttonText, "%s", d->getName());
				}else {
					sprintf(buttonText, "Terminal %d", value);
				}
				pPushButton->setText(buttonText);
			}
		}
		break;

		case ACTION_EXCLUDE_INCLUDE:
			sprintf(buttonText, "E/I");
			pPushButton->setText(buttonText);
		break;
		case ACTION_GENERAL_CALL:
			sprintf(buttonText, "Appel Gen");
			pPushButton->setText(buttonText);
		break;
		case ACTION_GROUP_CALL:
			sprintf(buttonText, "Appel Grp %d", value);
			pPushButton->setText(buttonText);
		break;
		default:
		break;
    
		}
	}// for

	
	if (pCurrentDeviceKeysInEdition != NULL)
	{
		for (int k = 39 ; k <= 52; k ++ ) 
		{
			char objName[32];
			sprintf(objName, "btnCRM4Key%d", k);
			
			QPushButton *pButton = (QPushButton*)groupBoxMapCRM4->child(objName);   
			if ( pButton ) {
				if (( pCurrentDeviceKeysInEdition->getGroupNumberByKeyNumber(k)) == -1) {
					pButton->setText("");
				}else {
					int groupNumber =  pCurrentDeviceKeysInEdition->getGroupNumberByKeyNumber(k);
					Group *pG =  VegaConfig::pCurrent->findGroupByNumber(groupNumber);
					if ( pG ) {
						pButton->setFont(QFont("Times", 8, QFont::Bold));
						pButton->setText(pG->getName());
					}
				}
			}
		}
	}
}
  
void Ui_MainWindow::slot_deviceKeysConfigurationDeviceChanged(int index)
{
	fprintf(stderr, "Ui_MainWindow::slot_deviceKeysConfigurationDeviceChanged %d\n",index);
	if ( VegaConfig::pCurrent==NULL ) return;

	if (pCurrentButtonKeyToConfigure)  {
		pCurrentButtonKeyToConfigure->setStyleSheet("QPushButton { background-color: white }"); 
		pCurrentButtonKeyToConfigure = NULL;
	}

	if (index == 0) {
		groupBoxMapCRM4->setVisible(false);
		groupBoxKeyConfiguration->setVisible(false);
		return;
	}

	QString deviceName = comboBoxDeviceName->text(index);
	int deviceNumber = VegaConfig::pCurrent->getDeviceUiEltNumberByName(deviceName.latin1());

	pCurrentDeviceKeysInEdition = VegaConfig::pCurrent->getDeviceByNumber(deviceNumber);//(pDevices, deviceNumber);
	if ( pCurrentDeviceKeysInEdition )
	{
		groupBoxMapCRM4->setVisible(true);
		groupBoxKeyConfiguration->setVisible(false);  
		showDeviceKeysMapConfiguration();
	}
	return;
}

void Ui_MainWindow::deviceKeyConfiguredOK()
{
	fprintf(stderr, "Ui_MainWindow::deviceKeyConfiguredOK\n");

	if ( VegaConfig::pCurrent==NULL ) 
		return;

	if (pCurrentButtonKeyToConfigure == NULL)
		return;

	if (VegaConfig::pCurrent == VegaConfig::pRunning) {
	  dbgBox("Impossible de modifier la configuration des touches sur la configuration courante !");
	  return;
	}


	int index  = comboBoxChooseKeyType->currentIndex();  
	key_action_t action  = ( index == 0 ? ACTION_NONE : ACTION_GROUP_CALL );

	const char *groupName = (comboBoxKeyGroupName->text(comboBoxKeyGroupName->currentIndex())).latin1();

	Group *pg = VegaConfig::pCurrent->findGroupByName(groupName);

	if (pg == NULL && action == ACTION_GROUP_CALL)
		return;
	else 
		if (pg && action == ACTION_GROUP_CALL) 
		{
			int groupNumber = pg->getNumber();

			if (action == ACTION_GROUP_CALL) {
				if (pCurrentDeviceKeysInEdition->getKeyNumberByGroupNumber(groupNumber) != -1) {
					msgBox("Une touche est deja configuree avec ce groupe !");
					return;
				}
				pCurrentDeviceKeysInEdition->configureKey(currentDeviceKeyNumberToConfigure, action, groupNumber);      
				pCurrentButtonKeyToConfigure->setFont(QFont("Times", 8, QFont::Bold));
				pCurrentButtonKeyToConfigure->setText(pg->getName());
      
			}
		}
		else if (action  == ACTION_NONE) 
		{
			pCurrentDeviceKeysInEdition->configureKey(currentDeviceKeyNumberToConfigure, action, -1); 
			pCurrentButtonKeyToConfigure->setText("");
		}

	groupBoxKeyConfiguration->setVisible(false);
	pCurrentButtonKeyToConfigure->setStyleSheet("QPushButton { background-color: white }"); 
	pCurrentButtonKeyToConfigure = NULL;
}



// declenche sur un appui bouton representant une touche de terminal
void Ui_MainWindow::slot_crm4KeyConfigure(int key) 
{
 	fprintf(stderr, "Ui_MainWindow::slot_crm4KeyConfigure %d\n",key);
	if ( VegaConfig::pCurrent==NULL ) return;

	if (m_readOnly)
		return;

	/*if (VegaConfig::pCurrent &&    VegaConfig::pRunning &&  VegaConfig::pRunning == VegaConfig::pCurrent) {
		msgBox("Impossible de configurer une touche sur la configuration en cours !");
		return;
	}*/


	char objName[256] = { 0 };
	sprintf(objName, "btnCRM4Key%d", key);
	fprintf(stderr,"btnCRM4Key%d\n", key);

	//groupBoxKeyConfiguration->setVisible(true);

	QObject *pObj = groupBoxMapCRM4->child(objName);

	if ( pObj == NULL ) return;
  
	if (pObj && pCurrentButtonKeyToConfigure &&  (pObj == pCurrentButtonKeyToConfigure) ) {
		//   pCurrentButtonKeyToConfigure->setStyleSheet("QPushButton { background-color: white }"); 
		groupBoxKeyConfiguration->setVisible(false);
		pCurrentButtonKeyToConfigure = NULL;
		return;
	}
    
	//   if (pCurrentButtonKeyToConfigure)
	//     pCurrentButtonKeyToConfigure->setStyleSheet("QPushButton { background-color: white }"); 

	comboBoxChooseKeyType->setCurrentIndex(0);
	comboBoxKeyGroupName->clear();

	pCurrentButtonKeyToConfigure = (QPushButton*)pObj;
	pCurrentButtonKeyToConfigure->setStyleSheet("QPushButton { background-color: red }"); 
	groupBoxKeyConfiguration->setVisible(true);

	int group =  pCurrentDeviceKeysInEdition->getGroupNumberByKeyNumber(key);
	if (group != -1)  {
		key_action_t action = ACTION_GROUP_CALL;
		comboBoxChooseKeyType->setCurrentIndex(1);
		char value[128] = { 0 };
		sprintf(value, "%d", group);
		pCurrentButtonKeyToConfigure = (QPushButton*)pObj;
		comboBoxKeyGroupName->setEnabled(1);
	
	}	else  {
    
		key_action_t action = ACTION_NONE;
		comboBoxChooseKeyType->setCurrentIndex(0);
		comboBoxKeyGroupName->setEnabled(0);
	}
  
	int i = 0;
	foreach(Group* pg,VegaConfig::pCurrent->m_groups){
		int pos = 0;
		if (group != -1 && pg->getNumber() == group) 
			i = pos;
		comboBoxKeyGroupName->addItem(pg->getName());
	}
	comboBoxKeyGroupName->setCurrentIndex(i);  
	currentDeviceKeyNumberToConfigure = key;
}


void Ui_MainWindow::devicesContactSecChanged(int row)
{
	fprintf(stderr,"devicesContactSecChanged %d\n",row);
	if ( VegaConfig::pCurrent==NULL ) return;


	QTableWidgetItem* pItem0 = tableDevices->item(row, 0); // 0: "D1" 1:"crm4" 2:"Philippe"
	QString strNo = pItem0->text(); 
	fprintf(stderr,"devicesContactSecChanged pItem2=%s\n",strNo.latin1());
	strNo = strNo.mid(1);
	Device *d = VegaConfig::pCurrent->getDeviceByNumber(strNo.toInt() );

	QCheckBox *pCheckBox = (QCheckBox*)this->tableDevices->cellWidget(row, 5);
	if ( pCheckBox && d) 
	{
		Qt::CheckState   newState = pCheckBox->checkState();
		fprintf(stderr,"D%d = %s checked:%d\n",d->getNumber(), d->getName() , newState);
		if (newState == Qt::Checked) 
			d->setContactSec(true);
		else 
			d->setContactSec(false);    
	}else{
		fprintf(stderr,"err tableDevices->cellWidget(%d,5)\n",row);
	}
}


void Ui_MainWindow::devicesDuplexChanged(int row)
{
	fprintf(stderr,"devicesDuplexChanged %d\n",row);
	if ( VegaConfig::pCurrent==NULL ) return;

	QTableWidgetItem* pItem0 = tableDevices->item(row, 0); // 0: "D1" 1:"crm4" 2:"Philippe"
	QString strNo = pItem0->text(); 
	fprintf(stderr,"devicesContactSecChanged pItem2=%s\n",strNo.latin1());
	strNo = strNo.mid(1);
	Device *d = VegaConfig::pCurrent->getDeviceByNumber(strNo.toInt() );

	QCheckBox* pCheckBox = (QCheckBox*)this->tableDevices->cellWidget(row, 4);
	if ( pCheckBox && d ) 
	{
		Qt::CheckState newState = pCheckBox->checkState();
		fprintf(stderr,"D%d = %s checked:%d\n",d->getNumber(), d->getName() , newState);
		if (newState == Qt::Checked) 
			d->setFullDuplex(true);
		else 
			d->setFullDuplex(false);    
	}
}

void Ui_MainWindow::devicesGainChanged(int row)
{
	fprintf(stderr,"devicesDuplexChanged %d\n",row);
	if ( VegaConfig::pCurrent==NULL ) return;

	QTableWidgetItem* pItem0 = tableDevices->item(row, 0); // 0: "D1" 1:"crm4" 2:"Philippe"
	QString strNo = pItem0->text(); 
	fprintf(stderr,"devicesDuplexChanged pItem2=%s\n",strNo.latin1());
	strNo = strNo.mid(1);
	Device *d = VegaConfig::pCurrent->getDeviceByNumber(strNo.toInt() );

	QComboBox *pComboBox = (QComboBox*)this->tableDevices->cellWidget(row, 3);
	
	if (pComboBox && d ){
		int comboxIndex = pComboBox->currentIndex();
		if ( comboxIndex < sizeof(Device::deviceGains) ) {
			fprintf(stderr,"D%d = %s setGain:%d\n",d->getNumber(), d->getName() , Device::deviceGains[comboxIndex]);
			d->setGain ( Device::deviceGains[comboxIndex] );
		}
	}
}

/*  appele quand le nom d'un device change dans une cellule du tableau  d'edition des postes*/
void Ui_MainWindow::devicesNameChanged(int row , int index)
{
	//dbgBox("Ui_MainWindow::devicesNameChanged");
	fprintf(stderr,"devicesNameChanged row=%d index=%d in %s\n", row, index,VegaConfig::pCurrent->getName() );
	if ( VegaConfig::pCurrent==NULL ) return;

	  if (m_readOnly == true)
	    return;

	if ( 2!=index ) 
		return;

	QTableWidgetItem* pItem0 = tableDevices->item(row, 0); // 0: "D1" 1:"crm4" 2:"Philippe"
	QTableWidgetItem* pItem2 = tableDevices->item(row, 2); // 0: "D1" 1:"crm4" 2:"Philippe"
	QString strNo = pItem0->text(); 
	fprintf(stderr,"devicesNameChanged pItem2=%s\n",strNo.latin1());
	strNo = strNo.mid(1);
	//dbgBox(strNo+newDeviceName);
	Device *d1 = VegaConfig::pCurrent->getDeviceByNumber(strNo.toInt() );
	//if ( (d1 = Device::getByName(pDevices, qPrintable() )) == NULL) 
	if ( !d1 ) {
		dbgBox( "cannot find device with number %d", strNo.toInt() );
		vega_log(VEGA_LOG_CRITICAL, "cannot find device with name %s", qPrintable(pItem2->text()));
		return;
	}else{

		QString r = "Telephone";
		r.remove(QRegExp("[aeiou]."));
		// r == "The"

		QString newDeviceName = pItem2->text();  // todo : si un autre a le meme nom ???
		newDeviceName.remove(QChar('<'), Qt::CaseInsensitive);
		newDeviceName.remove(QChar('>'), Qt::CaseInsensitive);
		//newDeviceName.remove(QRegExp("[ιθ;,:!<>]."));
		/*QRegExp regle("^[A-Z][a-z][ ]{1,12}$");
		if (  regle.exactMatch(newDeviceName) ) {
			//dbgBox("match");
		}else{
			pItem2->setText(d1->getName());
			return;
		}*/
		bool isvalid = true;
		for ( int k=0;k< newDeviceName.length() ; k++){
			if ( 
				( 'A' <= newDeviceName[k] && newDeviceName[k] <= 'Z' ) ||
				( 'a' <= newDeviceName[k] && newDeviceName[k] <= 'z' ) ||
				( '0' <= newDeviceName[k] && newDeviceName[k] <= '9' ) ||
				( '-' == newDeviceName[k] )
				)
			{
			}else{
				pItem2->setText(d1->getName());
				return;
			}
		}

		//newDeviceName = newDeviceName.mid(0,12);
		fprintf(stderr, "set device name D%d=%s\n", strNo.toInt(), qPrintable(newDeviceName) );
		if ( newDeviceName.length() >= 2 &&  newDeviceName.length() <= 12)
			d1->setDeviceName(qPrintable(newDeviceName));
		else{
			pItem2->setText(d1->getName());
			return;
		}


		foreach(DeviceUiElt* pui, VegaConfig::pCurrent->m_pDevicesUiElts ) 
		{
			if ( pui->getDeviceNumber() == d1->getNumber()) {
				  char newname[128] = { 0 };
				  sprintf(newname, "%s(%d)", newDeviceName.latin1(), pui->getDeviceNumber());
				  pui->setName(newname);
			}
		}

		fprintf(stderr,"execution : %s line %d\n", __FILE__ , __LINE__ );

		//reinitialise le combobox dans l'edition des touches
		/*disconnect(comboBoxDeviceName, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_deviceKeysConfigurationDeviceChanged(int)));  
		comboBoxDeviceName->clear();
		//comboBoxDeviceName->addItem("");
		fprintf(stderr,"execution : %s line %d\n", __FILE__ , __LINE__ );
		foreach(Device* pd, VegaConfig::pCurrent->m_devices)
		{
			if (pd->getType() == DEVICE_CRM4 && pd->isEnabled()) {
				comboBoxDeviceName->addItem( getDeviceUiEltNameByNumber(pd->getNumber()) , pd->getNumber());
			}
		}  
		connect(comboBoxDeviceName, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_deviceKeysConfigurationDeviceChanged(int)));  */

		
		comboBoxDeviceName->setCurrentIndex(0);
		groupBoxMapCRM4->setVisible(false);
		groupBoxKeyConfiguration->setVisible(false);

	}
}
