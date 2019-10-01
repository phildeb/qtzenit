#include <iostream>
#include "vega_ihm.h"
#include "switch.h"
#include "debug.h"
#include "configuration.h"
#include "printer.h"
#include "liveEvents.h"

#define NB_GROUPS	20

static Group *		pCurrentGroupInEdition			= NULL;

void Ui_MainWindow::createNewGroup()
{
	if (m_readOnly)
		return;
	if ( VegaConfig::pCurrent==NULL ) return;

	if (VegaConfig::pCurrent == VegaConfig::pRunning) {
	  dbgBox("Impossible d'ajouter un groupe sur la configuration en cours !");
	  return;
	}
	if ( VegaConfig::pCurrent->m_groups.size() == NB_GROUPS ) {
		msgBox("Le nombre de groupe maximal est %d !", NB_GROUPS);
		return;
	}
	for ( int i = 1; i < NB_GROUPS + 1; i ++)
	{
		if ( ! VegaConfig::pCurrent->m_groups.contains(i) ) 
		{
			char name[256];  sprintf(name, "G%d", i );	 // nom par defaut lors de la creation

			Group *pg = new Group();  // placer le numero du groupe
			pg->setNumber(i);
			pg->setGroupName(name);
			VegaConfig::pCurrent->m_groups[i] = pg;

			comboBoxKeyGroupName->clear(); // choix du groupe possible dans la combo des touches CRM4
			foreach(Group* pg, VegaConfig::pCurrent->m_groups )   {
				comboBoxKeyGroupName->addItem(pg->getName());
			}
 
			// ajouter dans la combo groupes principale et selectionner par defaut
			comboBoxChooseEditGroup->addItem( pg->getName() + QString(" (G%1)").arg( pg->getNumber() ) , pg->getNumber());
			comboBoxChooseEditGroup->setCurrentIndex(comboBoxChooseEditGroup->count()  - 1);
			
			updateUIElement("comboBoxKeyGroupName");

			break;
		}
	}
}

void Ui_MainWindow::deleteGroup()
{
	int idx = 0;

	if (m_readOnly)
		return;

	if (VegaConfig::pCurrent == VegaConfig::pRunning) {
	  dbgBox("Impossible de supprimer un groupe sur la configuration en cours !");
	  return;
	}
	if (confirmBox("Supprimer le groupe ?")==QMessageBox::Cancel) 
		return;
  
	if ((idx = comboBoxChooseEditGroup->currentIndex()) == 0)
		return;

	if (pCurrentGroupInEdition == NULL)
		return;

	dbgBox("supprimer G%d", pCurrentGroupInEdition->getNumber());

	foreach(Device* d, VegaConfig::pCurrent->m_devices ) {
		if (d->getKeyNumberByGroupNumber(pCurrentGroupInEdition->getNumber()) != -1) {
			msgBox("Ce groupe est utilise sur une touche du terminal %d", d->getNumber());
			return;
		}
	}

	if (VegaConfig::pCurrent->m_groups.remove( pCurrentGroupInEdition->getNumber() ) ) 
		dbgBox("G%d supprime de m_groups",pCurrentGroupInEdition->getNumber());

	
	comboBoxKeyGroupName->clear();/// reinitialize le combo box selection groupe / touche
	foreach(Group* pg, VegaConfig::pCurrent->m_groups )   
		comboBoxKeyGroupName->addItem(pg->getName());	//dbgBox("reste G%d", pg->getNumber());
	comboBoxKeyGroupName->setCurrentIndex(0);
	

	comboBoxChooseEditGroup->removeItem(idx);
	//comboBoxChooseEditGroup->setCurrentIndex(0);
	//slot_comboGroup(idx - 1);
};


void Ui_MainWindow::groupNameChanged()
{
	fprintf(stderr,"Ui_MainWindow::groupNameChanged\n");
	if ( NULL == VegaConfig::pCurrent ) return;
	
	if (VegaConfig::pCurrent == VegaConfig::pRunning) {
	  disconnect(lineEditGroupName,	SIGNAL(editingFinished()), this, SLOT(groupNameChanged()));
	  lineEditGroupName->setText(pCurrentGroupInEdition->getName());	
	  lineEditGroupName->clearFocus();
	  dbgBox("Impossible de modifier un groupe sur la configuration courante !");
	  connect(lineEditGroupName, SIGNAL(editingFinished()), this, SLOT(groupNameChanged()));
	  return;
	}

	if (pCurrentGroupInEdition == NULL)   
	  return;
  
	pCurrentGroupInEdition->setGroupName(lineEditGroupName->text().latin1());    
	updateUIElement("comboBoxKeyGroupName");

	// modifier dans la combo le nom qui vient d'etre modifie
	for (int i = 0; i < comboBoxChooseEditGroup->count(); i++) 
	{
		int itemData = comboBoxChooseEditGroup->itemData(i).toInt();
		if ( itemData == pCurrentGroupInEdition->getNumber() )
			comboBoxChooseEditGroup->setItemText(i,pCurrentGroupInEdition->getName() + QString(" (G%1)").arg( pCurrentGroupInEdition->getNumber() ) );
	}
}


void Ui_MainWindow::groupAddMember()
{
	fprintf(stderr,"groupRemoveMember\n");
	if ( NULL == VegaConfig::pCurrent ) return;

	if (VegaConfig::pCurrent == VegaConfig::pRunning) {
	  dbgBox("Impossible de modifier un groupe sur la configuration courante !");
	  return;
	}
	int row = listWidgetGroupDevices->currentRow();
	if (row == -1)
		return ;

	QListWidgetItem *pCurrentItem = listWidgetGroupDevices->currentItem();
	QString text = pCurrentItem->text();

	int number = VegaConfig::pCurrent->getDeviceUiEltNumberByName(text.latin1());

	Device *pD = NULL;  
	if ((pD = VegaConfig::pCurrent->getDeviceByNumber(number)) ==NULL ) //Device::getDeviceByNumber(pDevices, number)) == NULL)
		return;
	listWidgetGroupMembers->addItem(text.latin1());
	listWidgetGroupDevices->takeItem(row);
	pCurrentGroupInEdition->addMember(number);
}


void Ui_MainWindow::groupRemoveMember()
{
	fprintf(stderr,"groupRemoveMember\n");
	if ( NULL == VegaConfig::pCurrent ) return;

	if (VegaConfig::pCurrent == VegaConfig::pRunning) {
	  dbgBox("Impossible de modifier un groupe sur la configuration courante !");
	  return;
	}

	int row = listWidgetGroupMembers->currentRow();
	if (row == -1)
		return ;
  
	QListWidgetItem *pCurrentItem = listWidgetGroupMembers->currentItem();
	QString text = pCurrentItem->text();

	int number = VegaConfig::pCurrent->getDeviceUiEltNumberByName(text.latin1());

	Device *pD = NULL;  
	if ((pD = VegaConfig::pCurrent->getDeviceByNumber(number)) ==NULL ) //Device::getDeviceByNumber(pDevices, number)) == NULL)
		return;

	listWidgetGroupDevices->addItem(text.latin1());
	listWidgetGroupMembers->takeItem(row);
	pCurrentGroupInEdition->delMember(number);
}



void Ui_MainWindow::slot_comboGroup(int index)
{
	int itemData  = comboBoxChooseEditGroup->itemData(index).toInt();
	fprintf(stderr,"slot_comboGroup index %d itemdata %d\n" , index , itemData);//QString::number(itemData) );

	// tout effacer !
	listWidgetGroupDevices->clear();
	listWidgetGroupMembers->clear();
	lineEditGroupName->setText("");

	if ( NULL == VegaConfig::pCurrent ) return;

	Group* pg = VegaConfig::pCurrent->findGroupByNumber(itemData);
	if ( pg == NULL ) return;

	pCurrentGroupInEdition = pg;
	if (m_readOnly) {
		listWidgetGroupMembers->setEnabled(false);
		listWidgetGroupDevices->setEnabled(false);
	}

	lineEditGroupName->setText(pCurrentGroupInEdition->getName());
	if (m_readOnly)
		lineEditGroupName->setEnabled(false);

	foreach(Device* pD, VegaConfig::pCurrent->m_devices ) 	
	{
		if (pD->getType() != DEVICE_CRM4)
		  continue;
		if (pD->isEnabled() == false)
		  continue;
		const char *deviceUIName= VegaConfig::pCurrent->getDeviceUiEltNameByNumber(pD->getNumber());
		if (pCurrentGroupInEdition->deviceIsMember(pD->getNumber())) 
			listWidgetGroupMembers->addItem(deviceUIName);
		else {
			listWidgetGroupDevices->addItem(deviceUIName);
		}
	}
}
