#include <iostream>
#include "vega_ihm.h"
#include "switch.h"
#include "debug.h"
#include "configuration.h"
#include "printer.h"
#include "liveEvents.h"

static int conferencesAvailability[10] = { 1,1,1,1,1,1,1,1,1,1};

static bool isConferenceNumberEnabled(int n)
{
	if ( n > 9 ) return false;  
	return conferencesAvailability[n-1] == 1 ? true : false;
}

void Ui_MainWindow::conferenceNameChanged()
{
  if ( VegaConfig::pCurrent == NULL ) 
    return ;
  if (VegaConfig::pCurrent->pCurrentConference == NULL)	
    return;

  if (VegaConfig::pCurrent == VegaConfig::pRunning) {

    disconnect(lineEditConferenceName, SIGNAL(editingFinished()), this, SLOT(conferenceNameChanged()));
    
    
    msgBox("Impossible de changer le nom d'une conference sur la configuration en cours !");
    lineEditConferenceName->clearFocus();
    connect(lineEditConferenceName, SIGNAL(editingFinished()), this, SLOT(conferenceNameChanged()));
    return;
  }


  fprintf(stderr,"Ui_MainWindow::conferenceNameChanged %s\n", qPrintable(lineEditConferenceName->text()) );
  
  //if ( lineEditConferenceName->text().length() > 2 ) 
  VegaConfig::pCurrent->pCurrentConference->m_conference_name = lineEditConferenceName->text() ;
  
  fprintf(stderr,"pCurrentConference->getName %s\n",VegaConfig::pCurrent->pCurrentConference->getName() );
}


void Ui_MainWindow::conferenceRadio1Changed(int index)
{
	if ( VegaConfig::pCurrent == NULL ) return ;
	if (VegaConfig::pCurrent->pCurrentConference == NULL)	return;
  QString text;
  int number;
  const char *name;

  if (index == -1)
    return ;

//   if (VegaConfig::pCurrent == VegaConfig::pRunning) {
//     msgBox("Impossible de changer la radio1 sur la configuration en cours d'utilisation !");
//     disconnect(comboBoxRadio1, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceRadio1Changed(int)));
//     if (VegaConfig::pCurrent->pCurrentConference->getRadio1() == 0)  {
//       comboBoxRadio1->setCurrentIndex(0);
//       connect(comboBoxRadio1, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceRadio1Changed(int)));
//       return;
//     }
//     const char *str = getDeviceUiEltNameByNumber(VegaConfig::pCurrent->pCurrentConference->getRadio1());
//     for (int k = 0; k < comboBoxRadio1->count(); k++) {
//       QString t = comboBoxRadio1->itemText(k);
//       if (strcmp(t.latin1(), str)) 
// 	continue;
//       comboBoxRadio1->setCurrentIndex(k);
//       connect(comboBoxRadio1, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceRadio1Changed(int)));
//       return;
//     }
//   }

  if (index == 0) {
    VegaConfig::pCurrent->pCurrentConference->setRadio1(0);
    return;
  }

  text = comboBoxRadio1->itemText(index);

  number = VegaConfig::pCurrent->getDeviceUiEltNumberByName(text.latin1());
  name = VegaConfig::pCurrent->radioIsInOneConference(number);
  if (name != NULL) {
    msgBox("Cette interface RADIO est deja selectionnee dans la conference %s !", name);
    comboBoxRadio1->setCurrentIndex(0);
    VegaConfig::pCurrent->pCurrentConference->setRadio1(0);
    return;
  }
  
  if (VegaConfig::pCurrent->pCurrentConference->getRadio2() == number) {
    comboBoxRadio1->setCurrentIndex(0);
    VegaConfig::pCurrent->pCurrentConference->setRadio1(0);
    return;
  }

  VegaConfig::pCurrent->pCurrentConference->setRadio1(number);
  
}

void Ui_MainWindow::conferenceRadio2Changed(int index)
{
	if ( VegaConfig::pCurrent == NULL ) return ;
	if (VegaConfig::pCurrent->pCurrentConference == NULL)	return;
  QString text;
  int number;
  Device *d;
  const char *name;
  
  if (index == -1)
    return ;

//   if (VegaConfig::pCurrent == VegaConfig::pRunning) {
//     msgBox("Impossible de changer la radio2 sur la configuration en cours d'utilisation !");
//     disconnect(comboBoxRadio2, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceRadio2Changed(int)));
//     if (VegaConfig::pCurrent->pCurrentConference->getRadio2() == 0)  {
//       comboBoxRadio2->setCurrentIndex(0);
//       connect(comboBoxRadio2, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceRadio2Changed(int)));
//       return;
//     }
//     const char *str = getDeviceUiEltNameByNumber(VegaConfig::pCurrent->pCurrentConference->getRadio2());
//     for (int k = 0; k < comboBoxRadio2->count(); k++) {
//       QString t = comboBoxRadio2->itemText(k);
//       if (strcmp(t.latin1(), str)) 
// 	continue;
//       comboBoxRadio2->setCurrentIndex(k);
//       connect(comboBoxRadio2, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceRadio2Changed(int)));
//       return;
//     }
//   }


  if (index == 0) {
    VegaConfig::pCurrent->pCurrentConference->setRadio2(0);
    return;
  }

  text = comboBoxRadio2->itemText(index);
  number = VegaConfig::pCurrent->getDeviceUiEltNumberByName(text.latin1());

  name = VegaConfig::pCurrent->radioIsInOneConference(number);
  if (name != NULL) {
    msgBox("Cette interface RADIO est deja selectionnee dans la conference %s !", name);
    comboBoxRadio2->setCurrentIndex(0);
    VegaConfig::pCurrent->pCurrentConference->setRadio2(0);
    return;
  }

  d = VegaConfig::pCurrent->getDeviceByNumber(number);

  if (VegaConfig::pCurrent->pCurrentConference->getRadio1() == number) {
    comboBoxRadio2->setCurrentIndex(0);
    VegaConfig::pCurrent->pCurrentConference->setRadio2(0);
    return;
  }
  
  VegaConfig::pCurrent->pCurrentConference->setRadio2(d->getNumber());
}
void Ui_MainWindow::conferenceJupiter2Changed(int index)
{
	if ( VegaConfig::pCurrent == NULL ) return ;
	if (VegaConfig::pCurrent->pCurrentConference == NULL)	return;
  QString text;
  int number;
  Device *d;
  const char *name;

  if (index == -1)
    return ;
  
//   if (VegaConfig::pCurrent == VegaConfig::pRunning) {
//     msgBox("Impossible de changer l'interface jupiter2 sur la configuration en cours d'utilisation !");
//     disconnect(comboBoxJupiter2, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceJupiter2Changed(int)));
//     if (VegaConfig::pCurrent->pCurrentConference->getJupiter2() == 0)  {
//       comboBoxJupiter2->setCurrentIndex(0);
//       connect(comboBoxJupiter2, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceJupiter2Changed(int)));
//       return;
//     }
//     const char *str = getDeviceUiEltNameByNumber(VegaConfig::pCurrent->pCurrentConference->getJupiter2());
//     for (int k = 0; k < comboBoxJupiter2->count(); k++) {
//       QString t = comboBoxJupiter2->itemText(k);
//       if (strcmp(t.latin1(), str)) 
// 	continue;
//       comboBoxJupiter2->setCurrentIndex(k);
//       connect(comboBoxJupiter2, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceJupiter2Changed(int)));
//       return;
//     }
//   }

  if (index == 0) {
    VegaConfig::pCurrent->pCurrentConference->setJupiter2(0);
    return;
  }
  

  text = comboBoxJupiter2->itemText(index);  
  number = VegaConfig::pCurrent->getDeviceUiEltNumberByName(text.latin1());

  name = VegaConfig::pCurrent->jupiter2IsInOneConference(number);
  
  if (name != NULL) 
  {
    msgBox("Cette interface JUPITER2 est deja selectionnee la  conference %s !", name);
    comboBoxJupiter2->setCurrentIndex(0);
    VegaConfig::pCurrent->pCurrentConference->setJupiter2(0);
    return;
  }

  d = VegaConfig::pCurrent->getDeviceByNumber(number);//Device::getDeviceByNumber(pDevices, number);
  VegaConfig::pCurrent->pCurrentConference->setJupiter2(d->getNumber());
}



void Ui_MainWindow::conferenceDirectorChanged(int index)
{
	if ( VegaConfig::pCurrent == NULL ) return ;
	if (VegaConfig::pCurrent->pCurrentConference == NULL)	return;
  QString text;
  int number;
  int i;

  if (index == -1)
    return;

  if (VegaConfig::pCurrent == VegaConfig::pRunning) {
    
    msgBox("Impossible de changer le directeur sur la configuration en cours d'utilisation !");
  
    disconnect(comboBoxConferenceDirector,SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceDirectorChanged(int)));
    if (VegaConfig::pCurrent->pCurrentConference->getDirector() == 0)  {
      comboBoxConferenceDirector->setCurrentIndex(0);
      connect(comboBoxConferenceDirector,SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceDirectorChanged(int)));
      return;
    }
    const char *str = VegaConfig::pCurrent->getDeviceUiEltNameByNumber(VegaConfig::pCurrent->pCurrentConference->getDirector());
  
    for (int k = 0; k < comboBoxConferenceDirector->count(); k++) {
      QString t = comboBoxConferenceDirector->itemText(k);
      if (strcmp(t.latin1(), str)) 
	continue;
      comboBoxConferenceDirector->setCurrentIndex(k);
      connect(comboBoxConferenceDirector,SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceDirectorChanged(int)));
      return;
    }
  }
    

  text = comboBoxConferenceDirector->itemText(index);
  number = VegaConfig::pCurrent->getDeviceUiEltNumberByName(text.latin1());

  if (number < 0){
    VegaConfig::pCurrent->pCurrentConference->setDirector(0);
    return;
  }

//   if (VegaConfig::pCurrent->deviceIsInNbConferences(number) == 4) {
//     msgBox("Un terminal ne peut pas etre dans plus de 4 conferences !");
//     VegaConfig::pCurrent->pCurrentConference->setDirector(0);
//     comboBoxConferenceDirector->setCurrentIndex(0);
//     return;
//   }

  if (number == VegaConfig::pCurrent->pCurrentConference->getDirector())
    return;
  
//   if (VegaConfig::pCurrent->deviceIsDirectorInOneConference(number) == true) {
//     msgBox("Ce terminal est deja directeur dans une autre conference !");
//     VegaConfig::pCurrent->pCurrentConference->setDirector(0);
//     comboBoxConferenceDirector->setCurrentIndex(0);
//     return;
//   }
     
  if (VegaConfig::pCurrent->pCurrentConference->getDirector())
    listDevices->addItem(VegaConfig::pCurrent->getDeviceUiEltNameByNumber(VegaConfig::pCurrent->pCurrentConference->getDirector()));
 
  VegaConfig::pCurrent->pCurrentConference->setDirector(number);

  for (i = 0; i < listDevices->count(); i++) {
    QListWidgetItem *item = listDevices->item(i);
    
    if (!strcmp(item->text().latin1(), text.latin1())) {
      listDevices->takeItem(i);
      break;
    }  
  }
  
  for (i = 0; i < listConferenceMembers->count(); i++) {
    QListWidgetItem *item = listConferenceMembers->item(i);
    
    if (!strcmp(item->text().latin1(), text.latin1())) {
      listConferenceMembers->takeItem(i);
      break;
    }  
  }
  VegaConfig::pCurrent->pCurrentConference->delDevice(number);
}



void Ui_MainWindow::insertDeviceInConferenceClicked()
{
	if ( VegaConfig::pCurrent == NULL ) return ;
	if (VegaConfig::pCurrent->pCurrentConference == NULL)	return;
  // verifier que un element est selectionné dans le QListWidget listDevices
  int row = -1;
  QListWidgetItem *pCurrentItem;
  QString text;
  int number;
  int i;
  
  row = listDevices->currentRow();
  if (row == -1)
    return ;
  
  pCurrentItem = listDevices->currentItem();
  text = pCurrentItem->text();
  // trouver le device number
  number = VegaConfig::pCurrent->getDeviceUiEltNumberByName(text.latin1());

  if (number == VegaConfig::pCurrent->pCurrentConference->getDirector())
    return;

  Device *d = VegaConfig::pCurrent->getDeviceByNumber(number);//Device::getDeviceByNumber(pDevices, number);
  if (d == NULL)
    return;


//   if (VegaConfig::pCurrent->deviceIsInNbConferences(number) == 4) {
//     msgBox("Un terminal ne peut pas etre dans plus de 4 conferences !");
//     return;
//   }
  listConferenceMembers->addItem(text.latin1());
  
  listDevices->takeItem(row);

  VegaConfig::pCurrent->pCurrentConference->addDevice(number);

  // supprimer le poste comme directeur possible
  


  disconnect(comboBoxConferenceDirector,SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceDirectorChanged(int)));  
  for (i = 0; i < comboBoxConferenceDirector->count(); i++) {
    text = comboBoxConferenceDirector->text(i);
    int n = VegaConfig::pCurrent->getDeviceUiEltNumberByName(text.latin1());
    if (n == number) {
      comboBoxConferenceDirector->removeItem(i);
      break;
    }
  }
  connect(comboBoxConferenceDirector,SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceDirectorChanged(int)));  
}


void Ui_MainWindow::deleteDeviceInConferenceClicked()
{
	if ( VegaConfig::pCurrent == NULL ) return ;
	if (VegaConfig::pCurrent->pCurrentConference == NULL)	return;
  // verifier que un element est selectionné dans le QListWidget listDevices
  int row = -1;
  QListWidgetItem *pCurrentItem;
  QString text;
  int number;
  
  row = listConferenceMembers->currentRow();
  if (row == -1)
    return ;
  
  pCurrentItem = listConferenceMembers->currentItem();
  text = pCurrentItem->text();
  // trouver le device number
  number = VegaConfig::pCurrent->getDeviceUiEltNumberByName(text.latin1());
  Device *d = VegaConfig::pCurrent->getDeviceByNumber(number);//Device::getDeviceByNumber(pDevices, number);
  if (d == NULL)
    return;
  
  if (d->getNumber() == VegaConfig::pCurrent->pCurrentConference->getDirector())
    return;

  listDevices->addItem(text.latin1());
  listConferenceMembers->takeItem(row);

  VegaConfig::pCurrent->pCurrentConference->delDevice(number);

  comboBoxConferenceDirector->addItem(text.latin1());  
}

void Ui_MainWindow::checkboxActivateConference(int state)
{
	fprintf(stderr,"Ui_MainWindow::checkboxActivateConference state:%d\n",state);
	//conferenceNumber =  comboBoxConferenceNumberChoose->currentIndex() + 1;
	int idx = comboBoxConferenceNumberChoose->currentIndex();
	int itemData = comboBoxConferenceNumberChoose->itemData(idx).toInt() ;

	int conferenceNumber = itemData;
	fprintf(stderr,"Ui_MainWindow::checkboxActivateConference itemdata:%d\n",itemData);
	
	if ( VegaConfig::pCurrent == NULL ) return ;

	if (VegaConfig::pRunning && VegaConfig::pCurrent == VegaConfig::pRunning) 
	{
		msgBox("Impossible d'activer/desactiver une conference sur la configuration en cours d'utilisation");
		
		disconnect(checkBoxEnableConference, SIGNAL(stateChanged(int)), this, SLOT(checkboxActivateConference(int))); 

		if (VegaConfig::pCurrent->getConferenceByNumber(itemData) == NULL)
		  checkBoxEnableConference->setCheckState(Qt::Unchecked);
		else
		  checkBoxEnableConference->setCheckState(Qt::Checked);
		
		connect(checkBoxEnableConference, SIGNAL(stateChanged(int)), this, SLOT(checkboxActivateConference(int))); 
		
		return;
	}

	if (isConferenceNumberEnabled(conferenceNumber) == false && state == Qt::Checked) {
		msgBox("Conference %d indisponible !",conferenceNumber);
		return;
	}

	msgBox("(des)activer Conference C%d !",conferenceNumber);
  
	if (state == Qt::Unchecked) 
	{
		VegaConfig::pCurrent->deleteConferenceByNumber(conferenceNumber);
		dbgBox("C%d detruite",conferenceNumber);
		//if (VegaConfig::pCurrent->pCurrentConference) {
		//	delete VegaConfig::pCurrent->pCurrentConference;
		
		VegaConfig::pCurrent->pCurrentConference = NULL;	  
		this->setConferenceUneditable();
		slot_comboConference(0);

	}else {

		// teste si la conference existe deja
		if (VegaConfig::pCurrent->getConferenceByNumber(conferenceNumber)) {
			dbgBox("C%d existe deja",conferenceNumber);
			return;
		}

		char name[256]={0};
		sprintf(name, "myC%d", conferenceNumber);
		if (VegaConfig::pCurrent->getConferenceByName(name)) 
			strcat(name, "1");

		VegaConfig::pCurrent->pCurrentConference = new Conference(name);
		VegaConfig::pCurrent->m_conferences[conferenceNumber]=VegaConfig::pCurrent->pCurrentConference;
		VegaConfig::pCurrent->pCurrentConference->setNumber(conferenceNumber);

		lineEditConferenceName->setText(name);
		this->setConferenceEditable();
		slot_comboConference(conferenceNumber);
  
	}
}

void Ui_MainWindow::setConferenceEditable()
{
  disconnect(comboBoxRadio1, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceRadio1Changed(int)));
  disconnect(comboBoxRadio2, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceRadio2Changed(int)));
  disconnect(comboBoxJupiter2, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceJupiter2Changed(int)));
  disconnect(comboBoxConferenceDirector,SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceDirectorChanged(int)));

  labelConferenceName->setVisible(true);
  lineEditConferenceName->setVisible(true);
  labelConferenceDevices->setVisible(true);
  labelDevices->setVisible(true);
  pushButtonInsertDevice->setVisible(true);
  pushButtonDeleteDevice->setVisible(true);
  labelConferenceDirector->setVisible(true);
  labelConferenceRadio2->setVisible(true);
  label_63->setVisible(true);
  label_65->setVisible(true);
  listConferenceMembers->setVisible(true);
  listDevices->setVisible(true);
  comboBoxConferenceDirector->setVisible(true);
  comboBoxRadio1->setVisible(true);
  comboBoxRadio2->setVisible(true);
  comboBoxJupiter2->setVisible(true);
  comboBoxJupiter2->addItem("");
  comboBoxRadio1->addItem("");
  comboBoxRadio2->addItem("");
  comboBoxConferenceDirector->addItem("");

  connect(comboBoxRadio1, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceRadio1Changed(int)));
  connect(comboBoxRadio2, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceRadio2Changed(int)));
  connect(comboBoxJupiter2, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceJupiter2Changed(int)));
  connect(comboBoxConferenceDirector,SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceDirectorChanged(int)));
}




void Ui_MainWindow::setConferenceUneditable()
{
  disconnect(comboBoxRadio1, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceRadio1Changed(int)));
  disconnect(comboBoxRadio2, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceRadio2Changed(int)));
  disconnect(comboBoxJupiter2, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceJupiter2Changed(int)));
  disconnect(comboBoxConferenceDirector,SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceDirectorChanged(int)));



  labelConferenceName->setVisible(false);
  lineEditConferenceName->setVisible(false);
  lineEditConferenceName->setText("");
  labelConferenceDevices->setVisible(false);
  labelDevices->setVisible(false);
  pushButtonInsertDevice->setVisible(false);
  pushButtonDeleteDevice->setVisible(false);
  labelConferenceDirector->setVisible(false);
  labelConferenceRadio2->setVisible(false);
  label_63->setVisible(false);
  label_65->setVisible(false);

  listConferenceMembers->setVisible(false);
  listConferenceMembers->clear();
  listDevices->setVisible(false);
  listDevices->clear();
  comboBoxConferenceDirector->setVisible(false);
  comboBoxConferenceDirector->clear();
  comboBoxRadio1->setVisible(false);
  comboBoxRadio1->clear();
  comboBoxRadio2->setVisible(false);
  comboBoxRadio2->clear();
  comboBoxJupiter2->setVisible(false);
  comboBoxJupiter2->clear();

  connect(comboBoxRadio1, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceRadio1Changed(int)));
  connect(comboBoxRadio2, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceRadio2Changed(int)));
  connect(comboBoxJupiter2, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceJupiter2Changed(int)));
  connect(comboBoxConferenceDirector,SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceDirectorChanged(int)));

}

