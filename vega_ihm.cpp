#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <QtGui>
#include <rude/config.h>
#include "vega_ihm.h"
#include "switch.h"
#include "debug.h"
#include "configuration.h"
#include "printer.h"
#include "liveEvents.h"	

// GLOBALES //
LiveEvents						m_liveEvents;
controlLink						m_controlLink;
VegaConfig *					VegaConfig::pRunning;
VegaConfig *					VegaConfig::pCurrent;

QPixmap* pixmcrm;// = QPixmap("/home/operateur/vega/imgs/crm4.png");
QPixmap* pixmradio;// = QPixmap("/home/operateur/vega/imgs/radio.png");
QPixmap* pixmjup;// = QPixmap("/home/operateur/vega/imgs/jupiter.png");



int Ui_MainWindow::confirmBox(const char*fmt, ...)
{
	va_list ap;
	char str[1024];
	va_start(ap, fmt);
	vsprintf(str, fmt, ap);
	va_end(ap);
	QMessageBox msgBox(this);
	msgBox.setText(QString::fromUtf8(str));
	msgBox.setStandardButtons(QMessageBox::Apply|QMessageBox::Cancel);
	return msgBox.exec();
}

void Ui_MainWindow::msgBox(const char*fmt, ...)
{
	va_list ap;
	char str[1024];
	va_start(ap, fmt);
	vsprintf(str, fmt, ap);
	va_end(ap);	
	QMessageBox msgBox(this);
	msgBox.setText(QString::fromUtf8(str));
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.exec();
}

/*
const char *Ui_MainWindow::getDeviceUiEltNameByNumber(int number)
{
	//QList<DeviceUiElt*>::iterator i;
	//if ( pDevicesUiElts)  
	//	for (i = pDevicesUiElts->begin(); i!= pDevicesUiElts->end(); i++)
		foreach(DeviceUiElt* pui, VegaConfig::pCurrent->m_pDevicesUiElts ) 
			if ( pui->getDeviceNumber() == number)
				return pui->getName();
	return NULL;
}

int Ui_MainWindow::getDeviceUiEltNumberByName(const char*name)
{
	//QList<DeviceUiElt*>::iterator i;
	//if ( pDevicesUiElts)  
	//	for (i = pDevicesUiElts->begin(); i!= pDevicesUiElts->end(); i++)
		foreach(DeviceUiElt* pui, VegaConfig::pCurrent->m_pDevicesUiElts ) 
			if (!strcmp( pui->getName(),name))
				return  pui->getDeviceNumber();
	return -1;
}
*/

void Ui_MainWindow::slot_showTime()
{
  QTime time = QTime::currentTime();
  QDate date = QDate::currentDate();
  QString text = date.toString("dd/MM/yy") + " - " +  time.toString("hh:mm");
  
  lcdDate->display(text);
}

void Ui_MainWindow::slot_getRunningConfig()
{
	if ( !m_controlLink.isConnected() ) {
		msgBox("Deconnecte du PC de controle. Impossible de recuperer la config en cours! ");
	}else{
		//msgBox("Connecte au PC de controle: recuperation de la config en cours! ");
		m_controlLink.getFichierConfigTCP();
	}
}

void Ui_MainWindow::printConfiguration()
{
	if (VegaConfig::pCurrent == NULL){
		dbgBox("charger une configuration");
		return;
	}
  
	VegaPrinter printer;
	int ret = printer.execDialogBoxPrint(this);
	if (ret == 0) {
		printer.printText( qPrintable(VegaConfig::pCurrent->m_filepath) );//VegaConfig::pCurrent->dumpConfigurationToStr());
	}
}

void Ui_MainWindow::restartControl()
{
	if (m_controlLink.isConnected() == false) {
		msgBox("Deconnecte du PC de controle. Impossible de demander un redemarrage ! ");
		return;
	}
	if (m_readOnly){dbgBox("mode lecture seule");return;}

	if (confirmBox("Le controle va redemarrer et le systeme de conferences interrompu 1 minute, est ce que vous confirmez ?") == QMessageBox::Cancel) {
		return;
	}

	m_controlLink.sendRestartRequestTCP();
	//   msgBox("Demande de redemarrage soumise au contrôle !");

	QTimer *timer2 = new QTimer(this);
	connect(timer2, SIGNAL(timeout()), this, SLOT(slot_getRunningConfig()));
	timer2->setSingleShot(true);
	timer2->start(70000);

	return;
}

void Ui_MainWindow::exportConfigurations()
{
	bool ret = false;
	QString dir;
 
	dir = QFileDialog::getExistingDirectory(this, tr("Export des configurations - Choisissez un repertoire"), "/tmp");
	if (dir == "")
		return;
    
	foreach(VegaConfig* pvcfg, VegaConfig::m_qlist)
	{
		char dstDir[512];
		sprintf(dstDir, "%s/%s.conf", dir.latin1(), pvcfg->getName());
		ret = pvcfg->exportConfigurationFiles(dstDir);  
		if (false == ret) 
			msgBox("L'export de la configuration %s est impossible !", pvcfg->getName());
	}
	msgBox(tr("L'export des configurations est fini !"));
}

void Ui_MainWindow::applyCurrentConfigAsRunningConfig()
{
	fprintf(stderr,"applyCurrentConfigAsRunningConfig\n");

	if (VegaConfig::pCurrent == NULL){
		dbgBox("chargez d'abord une configuration");
		return;
	}
	if (m_controlLink.isConnected() == false) 
	{
		msgBox("Deconnecte du PC de controle. Impossible de soumettre la configuration !");
	}
	else
	{
		int ret;
		if (VegaConfig::pCurrent && (VegaConfig::pCurrent != VegaConfig::pRunning) )
		{
			ret = confirmBox("Cette soumission entrainera le redemarrage du controle ! Est ce que vous confirmez ?");
		}else {
			ret = confirmBox("Est ce que vous confirmez ?");
		}
		if (ret == QMessageBox::Cancel) 
			return;

		foreach(Conference* pc, VegaConfig::pCurrent->m_conferences)
		{
			if( pc->getDirector() == 0) 
			{
				msgBox("La conference %s n'a pas de directeur ! impossible de soumettre", pc->m_conference_name.latin1());
				return;
			}
		}

		if (m_readOnly){dbgBox("mode lecture seule");return;}

		m_controlLink.sendFichierConfigTCP("modif",qPrintable(VegaConfig::pCurrent->m_filepath));

		if ( VegaConfig::pRunning != VegaConfig::pCurrent ){
			dbgBox("faites un redemarrage du controle afin que la configuration soumise soit activee");
		}

		VegaConfig::pRunning = VegaConfig::pCurrent;

		lineEditConfigurationName->setText(VegaConfig::pRunning->getName());  

		goOnlineMode();

		// affichage du mot running dans la combo des configurations		
		MAJ_comboConfiguration();
		{
			int index = comboBoxChooseEditConfiguration->findData(VegaConfig::pRunning->getNumber());
			if ( -1 != index ) {
				//dbgBox("found index %d for C%d", VegaConfig::pRunning->getNumber());
				comboBoxChooseEditConfiguration->setCurrentIndex(index);
			}
		}

		//msgBox("Configuration soumise!");	
	}
	return;
}

void Ui_MainWindow::keyActionChanged(int index)
{
  if (index == 0)
    comboBoxKeyGroupName->setEnabled(0);
  else
    comboBoxKeyGroupName->setEnabled(1);
}


void Ui_MainWindow::updateStatus()
{
  /// Etat de la connection avec le control + etat de la connection avec la base de donnée
  QString msg;
  
  if (VegaConfig::pRunning) 
  {
    msg = QString("Configuration sur PC controle: [%1] ").arg(VegaConfig::pRunning->getName());
  }
  //msg = msg + QString::fromUtf8("   -  Connexion avec PC controle : ");
  
  if (m_controlLink.isConnected() == true) 
    msg = msg + QString(" - PC de controle: CONNECTE - ");
  else
    msg = msg + QString(" - PC de controle: DECONNECTE - ");

  if (VegaConfig::pCurrent ) 
	  msg += QString(" [configuration %1]\n").arg(VegaConfig::pCurrent->getName() );

  //if (VegaConfig::pCurrent->pCurrentConference ) 
	//  msg+= QString(" conference %1\n").arg(VegaConfig::pCurrent->pCurrentConference->getName() );

  statusBar->message(msg);
}


void Ui_MainWindow::goOfflineMode()
{
	fprintf(stderr,"Ui_MainWindow::goOfflineMode\n");
	labelImageVega->setPixmap(*imageVegaRouge);
	//if (VegaConfig::pRunning != VegaConfig::pCurrent) if (liveEvents->isRunning() == true)      liveEvents->stop();
}

void Ui_MainWindow::goOnlineMode()
{
	fprintf(stderr,"Ui_MainWindow::goOnlineMode\n");
	if (VegaConfig::pCurrent == VegaConfig::pRunning) {
		labelImageVega->setPixmap(*imageVegaVert);
		//lineEditConfigurationName->setVisible(false);
		//labelConferenceConfigurationName->setVisible(false);
	}
	pushButtonPrintConfiguration->setVisible(true);    
	//if (liveEvents->isRunning() == false)    liveEvents->start(); 
}

void Ui_MainWindow::configurationChanged(int index)
{
	dbgBox("Ui_MainWindow::configurationChanged %d",index);
	if (index == 0 || index == -1)  
		return;
	//QString itemText = comboBoxCurrentConfiguration->itemText(index);
	//dbgBox("change to config %s",itemText.latin1() );
	lineEditConfigurationName->setText(VegaConfig::pCurrent->getName());
}

void Ui_MainWindow::creationConfiguration()
{
	fprintf(stderr,"Ui_MainWindow::createConfiguration\n");

	if (m_readOnly){dbgBox("mode lecture seule");return;}

	bool ok;
	QString str2="entrez le nom de la nouvelle configuration:";
	QString str1="Vega configuration";
	//QDir::home().dirName()
	QString cfgname = QInputDialog::getText(str1,str2, QLineEdit::Normal );//,str3, &ok);
	if (cfgname.isEmpty()) return;

	struct timeval tv;
	struct tm* ptm;
	char time_string[128];
	gettimeofday (&tv, NULL);
	ptm = localtime (&tv.tv_sec);
	strftime (time_string, sizeof (time_string), "%Y-%m-%d_%Hh%Mm%Ss", ptm);

	char filepath[1024];
	//char cfgname[1024];
	int count = VegaConfig::m_qlist.count() + 1;
	//sprintf(name, "/usr/vega/conf/nouvelleconfig%d", count);
	sprintf(filepath, "%s/vega-%s-%s.conf", CONFIGURATION_DIR, cfgname.latin1(), time_string);
	//sprintf(cfgname, "nouvelleconfig%d", count);

	//dbgBox("creationConfiguration name=%s file=%s",cfgname.latin1(),filepath);

	{
		//char filetemplate[1024]={0};
		//sprintf(filetemplate, "%s/%s", CONFIGURATION_DIR, TEMPLATE);
		char cmd[1024];
		sprintf(cmd,"cp %s %s", TEMPLATE,filepath);
		int ret_cmd = system(cmd);
		//dbgBox("%d: %s",ret_cmd, cmd);
		fprintf(stderr,"%d: %s\n",ret_cmd, cmd);
	
		if ( ret_cmd != 0 ) {

			msgBox("Impossible de creer une nouvelle configuration !");

		}else{

			VegaConfig *pNewConfig= new VegaConfig(filepath,cfgname.latin1());
			
			if (pNewConfig->loadConfig() == true) 
			{
				VegaConfig::m_qlist.append(pNewConfig);
				VegaConfig::pCurrent = pNewConfig; // devient la courante !
				pNewConfig->setNewName(cfgname.latin1());

				{
					disconnect(comboBoxChooseEditConfiguration, SIGNAL(currentIndexChanged(int)), this, SLOT(editConfiguration(int)));
					
					comboBoxChooseEditConfiguration->addItem(pNewConfig->getName(), pNewConfig->getNumber());
					comboBoxChooseEditConfiguration->setCurrentIndex(comboBoxChooseEditConfiguration->count()  - 1);
					
					connect(comboBoxChooseEditConfiguration, SIGNAL(currentIndexChanged(int)), this, SLOT(editConfiguration(int)));
				}

				lineEditConfigurationName->setText(pNewConfig->getName());
				 slot_comboConfiguration(comboBoxChooseEditConfiguration->count()  - 1);

			}else
				dbgBox("erreur create New Configuration");
		}  
	}
}


void Ui_MainWindow::deleteCurrentConfiguration()
{
	fprintf(stderr,"deleteCurrentConfiguration\n");

	if (m_readOnly){dbgBox("mode lecture seule");return;}

	int index = comboBoxChooseEditConfiguration->currentIndex();    
	if (index < 0) return;

	fprintf(stderr,"Ui_MainWindow::deleteCurrentConfiguration %d\n",index);
	if ( index < 0 ) return;
	
	//QString itemText = comboBoxChooseEditConfiguration->itemText(index);
	int itemData  = comboBoxChooseEditConfiguration->itemData(index).toInt();
	//dbgBox( QString::number(itemData) + itemText);

	VegaConfig* pCfg = NULL;
	if ( itemData >= 0 && itemData < VegaConfig::m_qlist.size() ) {
		pCfg = VegaConfig::m_qlist[itemData];
		if ( pCfg ) {
			//itemText = pCfg->getName();
			//VegaConfig::pCurrent= pCfg;
			fprintf(stderr,"config choisie %s\n", pCfg->getName());


			QString prompt = QString("Etes vous sur de vouloir supprimer la configuration %1 ?").arg(pCfg->getName());
			if (confirmBox(prompt.latin1())==QMessageBox::Cancel) {
				return;
			}
			else
			{
				char cmd[1024];
				sprintf(cmd,"rm %s", pCfg->m_filepath.latin1());
				int ret_cmd = system(cmd);
				//dbgBox("%d: %s",ret_cmd, cmd);

				if (pCfg)
				comboBoxChooseEditConfiguration->removeItem(index);
				comboBoxChooseEditConfiguration->setCurrentIndex(0); 
				VegaConfig::pCurrent = NULL;

				if ( VegaConfig::m_qlist.contains(pCfg) ) {
					VegaConfig::m_qlist.remove( VegaConfig::m_qlist.indexOf( pCfg )  ) ;
					//dbgBox("removed from VegaConfig::m_qlist");
				}
			}					
		}
	}
}


void Ui_MainWindow::updateUIElement(const char* name)
{
	fprintf(stderr,"Ui_MainWindow::updateUIElement %s\n",name);
	if (!strcmp(name, "comboBoxKeyGroupName")) {
		int idx;
		QList<Group*>::iterator ig;
		idx = comboBoxKeyGroupName->currentIndex();
		comboBoxKeyGroupName->clear();
		
		foreach(Group* pg,VegaConfig::pCurrent->m_groups ) 
		//for (ig = pGroups->begin() ; ig != pGroups->end(); ig++)
			comboBoxKeyGroupName->addItem(pg->getName());
		
		comboBoxKeyGroupName->setCurrentIndex(0);
		comboBoxKeyGroupName->setCurrentIndex(idx);
	}
}

void Ui_MainWindow::tabGeneralChanged(int tabIndex)
{
	//msgBox("Ui_MainWindow::tabGeneralConfChanged %d",tabIndex);
	fprintf(stderr,"Ui_MainWindow::tabGeneralConfChanged %d\n",tabIndex);
	
	if ( NULL == VegaConfig::pCurrent ) return ;//&& VegaConfig::pCurrent->pCurrentConference )
			
	if (tabIndex == tabWidgetGeneral->indexOf(pannel_terminaux) ) {
		tabDevicesChanged(0);
	}
	if (tabIndex == tabWidgetGeneral->indexOf(pannel_conference) ) {
		slot_comboConference(0);
	}
	if (tabIndex == tabWidgetGeneral->indexOf(pannel_groupes) ) {
		slot_comboGroup(0);
	}
}

void Ui_MainWindow::tabDevicesChanged(int tabIndex)
{
	fprintf(stderr,"Ui_MainWindow::tabDevicesChanged %d\n",tabIndex);
	if( NULL == VegaConfig::pCurrent ) return;


	//dbgBox("Ui_MainWindow::tabDevicesConfChanged %d NOT IMPLEMENTED\n",tabIndex);return;
	//dbgBox("Ui_MainWindow::tabDevicesConfChanged %d IMPLEMENTED\n",tabIndex);
	//pdl  :pb de reaffichage des combos, checkbox !
	//tableDevices->clearContents();
	//while( tableDevices->rowCount() ) tableDevices->removeRow(0);
	//QPixmap pixmcrm = QPixmap("/home/operateur/vega/imgs/crm4.png");
	//fprintf(stderr,"px:%d py:%d\n", pixmcrm.width(),pixmcrm.height());//cw,rh);
	//QPixmap pixmradio = QPixmap("/home/operateur/vega/imgs/radio.png");
	//fprintf(stderr,"px:%d py:%d\n", pixmradio.width(),pixmradio.height());//cw,rh);
	//QPixmap pixmjup = QPixmap("/home/operateur/vega/imgs/jupiter.png");
	//fprintf(stderr,"px:%d py:%d\n", pixmjup.width(),pixmjup.height());//cw,rh);

	if ( VegaConfig::pCurrent ) 
	{
		for (int i = 0; i<46; i++) //foreach(Device* pd, VegaConfig::pCurrent->m_devices) 
		{
			//fprintf(stderr,"tabDevicesConfChanged D%d\n", i );

			QTableWidgetItem* pItem0 = tableDevices->item(i, 0); // "D1"
			QTableWidgetItem* pItem1 = tableDevices->item(i, 1); // "crm4"
			QTableWidgetItem* pItem2 = tableDevices->item(i, 2);
			//QTableWidgetItem* pItem4 = tableDevices->item(i, 4);
			//QTableWidgetItem* pItem5 = tableDevices->item(i, 5);
			//pItem4->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
			//pItem5->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
			QComboBox* pComboBox = (QComboBox*)tableDevices->cellWidget(i, 3);
			QCheckBox* pCheckBox1 = (QCheckBox*)tableDevices->cellWidget(i, 4);
			QCheckBox* pCheckBox2 = (QCheckBox*)tableDevices->cellWidget(i, 5);

			if ( pCheckBox1 == NULL ) return ;
			if ( pCheckBox2 == NULL ) return ;
			if ( pComboBox == NULL ) return ;
			if ( pItem0 == NULL ) return ;
			if ( pItem1 == NULL ) return ;

			//pItem0->setText("*");
			//pItem1->setText("*");
			pCheckBox1->setVisible(false);
			pCheckBox2->setVisible(false);
			pComboBox->setVisible(false);

			Device* pd = VegaConfig::pCurrent->getDeviceByNumber(i+1);
			//fprintf(stderr,"tabDevicesConfChanged %d\n", pd );
			
			if ( NULL == pd ) 	{
				//pItem0->setBackground(Qt::gray);
				continue;
			}
			
			fprintf(stderr,"tabDevicesConfChanged D%d:%s\n",i+1, pd->getName() );

			
			if ( pItem2 ) {
				pItem2->setText(pd->m_name.latin1());
				pItem2->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
			}

			if (pd->getType() == DEVICE_CRM4) 
			{
				pComboBox->setVisible(true);
				int gain  = pd->getGain();
				for (int k = 0; k < sizeof(Device::deviceGains)/*14*/; k++)
					if (Device::deviceGains[k] == gain) {
						pComboBox->setCurrentIndex(k);
						break;
					}
				if (m_readOnly) pComboBox->setEnabled(false);

				if ( pItem1 ) 		
				{
					pItem1->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
					pItem1->setText("crm4");
					QLabel * lbl = new QLabel();
					lbl->setAlignment(Qt::AlignCenter);
					QTableWidgetItem* pItem1 = tableDevices->item(i,1);//new QTableWidgetItem(" - ");  
					/*QSize sz = pItem1->sizeHint();
					int rh = sz.height() ; //tableDevices->rowHeight(); 
					int cw = sz.width() ;//tableDevices->columnWidth();
					fprintf(stderr,"x:%d y:%d\n", cw,rh);
					pixm.scaledToHeight ( 10 ) ;//rh ) ; 
					//pixm.scaled(sz,Qt::KeepAspectRatio);
					//pixm.scaled(sz,	Qt::KeepAspectRatioByExpanding);*/
					lbl->setPixmap(*pixmcrm);
					tableDevices->setCellWidget(i,1,lbl);
				}

			}
			else if (pd->getType() == DEVICE_RADIO) 
			{
				
				pCheckBox1->setVisible(true);
				pCheckBox2->setVisible(true);

				if (pd->getFullDuplex() == 0) 
					pCheckBox1->setCheckState(Qt::Unchecked);
				else 
					pCheckBox1->setCheckState(Qt::Checked);

				if (m_readOnly)	pCheckBox1->setEnabled(false);

				if (pd->getContactSec() == true) 
				  pCheckBox2->setCheckState(Qt::Checked);
				else 
				  pCheckBox2->setCheckState(Qt::Unchecked);
				
				if (m_readOnly)	pCheckBox2->setEnabled(false);

				if ( pItem1 ) 		
				{
					//pItem1->setTextAlignment(Qt::AlignHCenter);
					pItem1->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
					pItem1->setText("radio");
					QLabel * lbl = new QLabel();
					lbl->setAlignment(Qt::AlignCenter);
					QTableWidgetItem* pItem1 = tableDevices->item(i,1);//new QTableWidgetItem(" - ");  
					lbl->setPixmap(*pixmradio);
					tableDevices->setCellWidget(i,1,lbl);
				}

			}
			else if (pd->getType() == DEVICE_JUPITER2) 
			{
				pCheckBox2->setVisible(true);
				if (pd->getContactSec() == true) 
				  pCheckBox2->setCheckState(Qt::Checked);
				else 
				  pCheckBox2->setCheckState(Qt::Unchecked);
				if (m_readOnly)	pCheckBox2->setEnabled(false);

				if ( pItem1 ) 		
				{
					pItem1->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
					pItem1->setText("jupiter");
					QLabel * lbl = new QLabel();
					lbl->setAlignment(Qt::AlignCenter);
					QTableWidgetItem* pItem1 = tableDevices->item(i,1);//new QTableWidgetItem(" - ");  
					lbl->setPixmap(*pixmjup);
					tableDevices->setCellWidget(i,1,lbl);
				}

			}
		}
	}
}

void Ui_MainWindow::slot_updateVegaConfigCombo()
{
	char pathrunning[1024]={0};
	sprintf(pathrunning, "%s/%s", CONFIGURATION_DIR, "vegactrl.conf");

	rude::Config config;
	if (config.load(pathrunning) )
	{
		config.setSection("general");
		const char* cfgname = config.getStringValue("name");
		if ( strlen(cfgname) <= 0 ) 
			config.setStringValue("name","running");
		config.save();
		cfgname = config.getStringValue("name");

		

		// todo: si la config existe deja, prevenir l'utilisateur
		bool found = false;
		foreach(VegaConfig* pvcfg, VegaConfig::m_qlist)
		{
			if( pvcfg->m_configuration_name == QString(cfgname ) ) 
			{
				msgBox(QString("La configuration en cours sur le PC de controle est :") + QString(pvcfg->m_configuration_name)  );
				VegaConfig::pRunning = pvcfg;
				found = true;
				break;
			}
		}

		if (!found ){
			dbgBox("La configuration %s en cours sur le PC de controle n'existe pas dans l'IHM",cfgname);
			return;
		}else{

			MAJ_comboConfiguration();

			int index = comboBoxChooseEditConfiguration->findData(VegaConfig::pRunning->getNumber());
			if ( -1 != index ) {
				//dbgBox("found index %d for C%d", VegaConfig::pRunning->getNumber());
				comboBoxChooseEditConfiguration->setCurrentIndex(index);
			}
		}

		/*if ( ! found ) {

			VegaConfig *pNewConfiguration = new VegaConfig(pathrunning,cfgname);
			if (pNewConfiguration->loadConfig() == true){
				VegaConfig::m_qlist.append(pNewConfiguration);
				VegaConfig::pRunning = pNewConfiguration;
				msgBox("Creation sur  IHM de la configuration '%s' en cours sur pc de control",cfgname);
			}
		}*/
	}
	
}

void Ui_MainWindow::MAJ_comboConfiguration()
{
			comboBoxChooseEditConfiguration->clear();
			comboBoxChooseEditConfiguration->addItem("",0); // C0 n'existe pas
			foreach(VegaConfig* pvcfg, VegaConfig::m_qlist)	
			{ // pdl 20091019 ajoute le numero en item data
				if ( VegaConfig::pRunning && VegaConfig::pRunning==pvcfg ) 
				{	
					comboBoxChooseEditConfiguration->addItem(QIcon("/home/operateur/vega/imgs/flag_green.png"), pvcfg->getName() + QString(" ( RUNNING )") , pvcfg->getNumber() );
					/*int index  = comboBoxChooseEditConfiguration->count()-1;//findData(pvcfg->getNumber());
					if ( -1 != index ) {
						comboBoxChooseEditConfiguration->setItemData(index, Qt::yellow, Qt::BackgroundRole);
					}*/
				}else
					comboBoxChooseEditConfiguration->addItem(QIcon("/home/operateur/vega/imgs/flag_red.png"),pvcfg->getName() , pvcfg->getNumber() );
			}
}

void Ui_MainWindow::configurationNameChanged()
{
	QString newname = lineEditConfigurationName->text();
	fprintf(stderr,"Ui_MainWindow::configurationNameChanged %s\n", qPrintable(newname) );
	if( NULL == VegaConfig::pCurrent ) return;

	if ( VegaConfig::pCurrent == NULL ) return ;

	foreach(VegaConfig* pvcfg, VegaConfig::m_qlist)	{
		if ( pvcfg->getName() == newname )	{
			dbgBox("la configuration %s existe deja");
			return;
		}
		comboBoxChooseEditConfiguration->addItem(pvcfg->getName(),pvcfg->getNumber());
	}

	/*if (VegaConfig::pCurrent == VegaConfig::pRunning) {
		msgBox("Impossible de modifier le nom de la configuration en cours d'utilisation !");
		lineEditConfigurationName->setText(VegaConfig::pCurrent->getName());
		return;
	}*/

	// changement effectif du nom
	if ( VegaConfig::pCurrent && newname.length() >= 2 ) 
	{
		dbgBox("setNew config Name %s\n", qPrintable(newname) );
		VegaConfig::pCurrent->setNewName(newname.latin1());

		// modifier le texte dans la combo de choix des conferences
		comboBoxChooseEditConfiguration->clear();
		comboBoxChooseEditConfiguration->addItem("<aucune>",-1);
		foreach( VegaConfig* pvcfg, VegaConfig::m_qlist) {
			if ( NULL==pvcfg ) continue;
			comboBoxChooseEditConfiguration->addItem(pvcfg->getName() ,pvcfg->getNumber());
		}		
	}

}

void Ui_MainWindow::slot_comboConference(int index)
{
	fprintf(stderr,"Ui_MainWindow::slot_comboConference %d\n",index);
	//dbgBox("Ui_MainWindow::editConference %d\n",index);
	int itemData = comboBoxConferenceNumberChoose->itemData(index).toInt() ;

	groupBoxEditionConferences->setVisible(true);	
	comboBoxConferenceNumberChoose->setCurrentIndex(index);
	
	if ( NULL == VegaConfig::pCurrent ) {
		dbgBox("NULL == VegaConfig::pCurrent");
		return;
	}


	// on considere qu'on veut creer une nouvelle conference:
	VegaConfig::pCurrent->pCurrentConference = NULL;

	if ( VegaConfig::pCurrent  ) {
		foreach(Conference *pc,VegaConfig::pCurrent->m_conferences){
			if ( pc ==NULL ) continue;
			fprintf(stderr,"editConference C%d name:%s\n",pc->getNumber(),pc->getName());//,"C%d",pc->m_number);
			if ( itemData == pc->getNumber() ){
				VegaConfig::pCurrent->pCurrentConference = pc;
				fprintf(stderr,"editConference FOUND: C%d name:%s\n",pc->getNumber(),pc->getName());//,"C%d",pc->m_number);
			}
		}
	}
	
	disconnect(checkBoxEnableConference, SIGNAL(stateChanged(int)), this, SLOT(checkboxActivateConference(int))); 
	
	if (VegaConfig::pCurrent->pCurrentConference == NULL) 
	{
		//dbgBox("choix nouvelle conference");
		fprintf(stderr,"choix nouvelle conference\n");
		this->checkBoxEnableConference->setCheckState(Qt::Unchecked);
		connect(checkBoxEnableConference, SIGNAL(stateChanged(int)), this, SLOT(checkboxActivateConference(int))); 
		setConferenceUneditable();
		return;
	}

	//dbgBox("choisie: conference:%s",VegaConfig::pCurrent->pCurrentConference->getName() );
	
	fprintf(stderr,"choisie: conference:%s\n",VegaConfig::pCurrent->pCurrentConference->getName() );
	
	QString str;str.sprintf("choisie: conference:%s",VegaConfig::pCurrent->pCurrentConference->getName() );
	statusBar->message(str);

	setConferenceUneditable();
	setConferenceEditable();
	checkBoxEnableConference->setCheckState(Qt::Checked);
	connect(checkBoxEnableConference, SIGNAL(stateChanged(int)), this, SLOT(checkboxActivateConference(int))); 

	lineEditConferenceName->setText(VegaConfig::pCurrent->pCurrentConference->m_conference_name.latin1());
	if (m_readOnly) {
		lineEditConferenceName->setEnabled(false);
		checkBoxEnableConference->setEnabled(false);
	}
	// pour tous les membres de la conferences, on les insere
	{
		foreach(int dnumber, VegaConfig::pCurrent->pCurrentConference->m_conference_members)
		{
			fprintf(stderr,"C%d addItem: C%d: D%d\n", 
				VegaConfig::pCurrent->pCurrentConference->getNumber(),
				VegaConfig::pCurrent->pCurrentConference->m_number, 
				dnumber);//,"C%d",pc->m_number);


			const char *memberUiName = VegaConfig::pCurrent->getDeviceUiEltNameByNumber(dnumber);
			if (memberUiName != NULL){
				QListWidgetItem* pitem = new QListWidgetItem(memberUiName,listConferenceMembers);
				pitem->setIcon(QIcon("/home/operateur/vega/imgs/flag_green.png")) ;
				pitem->setData(Qt::UserRole,dnumber); // numero du device
				//listConferenceMembers->addItem(QIcon("/home/operateur/vega/imgs/flag_green.png"),memberUiName);
			}
		}
		//for (int k=0;k<listConferenceMembers->count();k++)
		//	listConferenceMembers->item(k)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
   	}

	fprintf(stderr,"execution : %s line %d\n", __FILE__ , __LINE__ );

	if (m_readOnly)
		listConferenceMembers->setEnabled(false);
	/// affiche tous les devices disponibles
	disconnect(comboBoxRadio1, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceRadio1Changed(int)));
	disconnect(comboBoxRadio2, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceRadio2Changed(int)));
	disconnect(comboBoxJupiter2, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceJupiter2Changed(int)));
	disconnect(comboBoxConferenceDirector,SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceDirectorChanged(int)));

	//QList<DeviceUiElt*>::iterator i1;
	//if (pDevicesUiElts ) 
	foreach(DeviceUiElt* pui, VegaConfig::pCurrent->m_pDevicesUiElts ) 
	//for (i1 = pDevicesUiElts->begin(); i1 != pDevicesUiElts->end(); i1++) 
	{
		//Device *d = VegaConfig::pCurrent->m_devices[(pui)->getDeviceNumber()];//::getDeviceByNumber(pDevices, (*i1)->getDeviceNumber());
		Device *d = VegaConfig::pCurrent->getDeviceByNumber( (pui)->getDeviceNumber());
		if (d && d->isEnabled() == true) 
		{
			if (d->getType() == DEVICE_CRM4) 
			{
				if (VegaConfig::pCurrent->pCurrentConference->deviceIsMember((pui)->getDeviceNumber()) == false) 
				{
					if (VegaConfig::pCurrent->pCurrentConference->getDirector() != (pui)->getDeviceNumber())
						listDevices->addItem((pui)->getName());

					comboBoxConferenceDirector->addItem((pui)->getName());
					
					if (m_readOnly) 
						comboBoxConferenceDirector->setEnabled(false);
				}
    		}
 			else if(d->getType() == DEVICE_RADIO) 
			{
				comboBoxRadio2->addItem((pui)->getName());
				comboBoxRadio1->addItem((pui)->getName());
				if (m_readOnly) {
					comboBoxRadio1->setEnabled(false);
					comboBoxRadio2->setEnabled(false);
				}
			}
			else if (d->getType() == DEVICE_JUPITER2) {
				comboBoxJupiter2->addItem((pui)->getName());
				if (m_readOnly)
					comboBoxJupiter2->setEnabled(false);
			}
		}
  	}
	//for (int k=0; k< listDevices->count() ;k++) // aligner proprement
		//listDevices->item(k)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

	if (m_readOnly)
		listDevices->setEnabled(false);

	// place les index pour les combo radio{1,2}, jupiter2 et directeur
	int director = VegaConfig::pCurrent->pCurrentConference->getDirector();
	if (director) {
		const char *str = VegaConfig::pCurrent->getDeviceUiEltNameByNumber(director);
		
		for (int k = 0; k < comboBoxConferenceDirector->count(); k++) {
			QString t = comboBoxConferenceDirector->itemText(k);
			if ( t == QString(str) ) {
				comboBoxConferenceDirector->setCurrentIndex(k);
				break;
			}
		}
	}

	fprintf(stderr,"execution : %s line %d\n", __FILE__ , __LINE__ );

	int radio1 = VegaConfig::pCurrent->pCurrentConference->getRadio1();
	if (radio1) {
		const char *str = VegaConfig::pCurrent->getDeviceUiEltNameByNumber(radio1);
		for (int k = 0; k < comboBoxRadio1->count(); k++) {
			QString t = comboBoxRadio1->itemText(k);
			if ( t == QString(str) ) {
				comboBoxRadio1->setCurrentIndex(k);
				break;
			}
		}
	}

	int radio2 = VegaConfig::pCurrent->pCurrentConference->getRadio2();
	if (radio2) {
		const char *str = VegaConfig::pCurrent->getDeviceUiEltNameByNumber(radio2);
		for (int k = 0; k < comboBoxRadio2->count(); k++) {
			QString t = comboBoxRadio2->itemText(k);
			if ( t == QString(str) ) {
				comboBoxRadio2->setCurrentIndex(k);
				break;
			}
		}

  	}
	
	int jup2 = VegaConfig::pCurrent->pCurrentConference->getJupiter2();
	if (jup2) {
		const char *str = VegaConfig::pCurrent->getDeviceUiEltNameByNumber(jup2);
		for (int k = 0; k < comboBoxJupiter2->count(); k++) {
			QString t = comboBoxJupiter2->itemText(k);
			if ( t == QString(str) ) {
				comboBoxJupiter2->setCurrentIndex(k);
				break;
			}
		}
  	}
	connect(comboBoxRadio1, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceRadio1Changed(int)));
	connect(comboBoxRadio2, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceRadio2Changed(int)));
	connect(comboBoxJupiter2, SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceJupiter2Changed(int)));
	connect(comboBoxConferenceDirector,SIGNAL(currentIndexChanged(int)), this, SLOT(conferenceDirectorChanged(int)));
}

void Ui_MainWindow::slot_comboConfiguration(int Index)
{
	fprintf(stderr,"Ui_MainWindow::slot_comboConfiguration %d\n",Index);
	//dbgBox("Ui_MainWindow::slot_comboConfiguration %d\n",Index);
	if ( index < 0 ) return;
	
	int idx = comboBoxChooseEditConfiguration->currentIndex();
	if ( idx < 0 ) return;
	int itemData  = comboBoxChooseEditConfiguration->itemData(idx).toInt(); // data = index de la configuration dans vecteur VegaConfig::m_qlist
	//dbgBox( QString::number(itemData) + itemText);

	VegaConfig* pCfg = NULL;
	if ( itemData >= 0  && itemData < VegaConfig::m_qlist.size() ) {
		pCfg = VegaConfig::m_qlist[itemData];
		if ( pCfg ) {
			//itemText = pCfg->getName();
			VegaConfig::pCurrent= pCfg;
			fprintf(stderr,"config choisie %s\n", VegaConfig::pCurrent->getName());
		}
	}
	//dbgBox("Ui_MainWindow::editConfiguration %d",index);

	if (index <= 0) 
	{
		//labelConferenceConfigurationName->setVisible(false);
		//lineEditConfigurationName->setVisible(false);
		tabWidgetGeneral->setVisible(false);
		pushButtonDeleteConfiguration->setVisible(false);
		pushButtonPrintConfiguration->setVisible(false);
		VegaConfig::pCurrent = NULL;
		goOfflineMode();
		return;
	}
	else {
		tabWidgetGeneral->setVisible(true); 
		//labelConferenceConfigurationName->setVisible(true);
		//lineEditConfigurationName->setVisible(true);
		pushButtonDeleteConfiguration->setVisible(true);
		pushButtonPrintConfiguration->setVisible(true);
	}


	//disconnect(tableDevices, SIGNAL(cellChanged(int, int)), this, SLOT(devicesNameChanged(int, int))); // ??? pdl 20091017
	//disconnect(signalMapperGains, SIGNAL(mapped(int)), this, SLOT(devicesGainChanged(int)));
	//disconnect(signalMapperDuplex, SIGNAL(mapped(int)), this, SLOT(devicesDuplexChanged(int)));
	//disconnect(signalMapperDeviceKeys, SIGNAL(mapped(int)), this, SLOT(deviceKeyConfigure(int)));
	//disconnect(signalMapperContactSec, SIGNAL(mapped(int)), this, SLOT(devicesContactSecChanged(int)));
	//disconnect(checkBoxEnableConference, SIGNAL(stateChanged(int)), this, SLOT(checkboxActivateConference(int)));
	// retrouve la configuration choisie dans la combobox dediee
	/*fprintf(stderr,"config choisie %s", itemText.latin1());
	{
		foreach( VegaConfig* pvcfg,VegaConfig::m_qlist) 
			if (!strcmp(pvcfg->getName(), itemText.latin1())){
				VegaConfig::pCurrent = pvcfg;
				break;
			}
	}*/

	if ( NULL!=VegaConfig::pCurrent ) {

		//dbgBox("choisie configuration: %s", VegaConfig::pCurrent->getName() );
		fprintf(stderr,"choisie configuration: %s\n", VegaConfig::pCurrent->getName() );
		statusBar->message(	QString(" configuration choisie: %1\n").arg(VegaConfig::pCurrent->getName() ) );


		lineEditConfigurationName->setText(VegaConfig::pCurrent->getName());
		//pDevices = VegaConfig::pCurrent->m_devices;
		//pGroups =  VegaConfig::pCurrent->m_groups;
		//gConferences = VegaConfig::pCurrent->m_conferences;
		//QMap<int,Conference*> m_conferences
		if ( VegaConfig::pCurrent->m_conferences.size() ){
			
			VegaConfig::pCurrent->pCurrentConference = VegaConfig::pCurrent->getConferenceByNumber(1);
			
			if ( VegaConfig::pCurrent->pCurrentConference ) 
				fprintf(stderr,"choisie par defaut conference C1:%s\n", VegaConfig::pCurrent->pCurrentConference->getName() );
		}
	}


	/* 	reinitialize les strings representant les devices dans l'IHM 	*/
	//if (pDevicesUiElts == NULL)
	//	pDevicesUiElts = new QList<DeviceUiElt*>();
	if ( VegaConfig::pCurrent ) 
	{
		VegaConfig::pCurrent->m_pDevicesUiElts.clear();
		foreach(Device* pd, VegaConfig::pCurrent->m_devices) 
		{
			DeviceUiElt *pElt = new DeviceUiElt();
			char name[128];
			sprintf(name, "%s(%d)", pd->getName(), pd->getNumber());
			pElt->setName(name);
			pElt->setDeviceNumber(pd->getNumber());

			VegaConfig::pCurrent->m_pDevicesUiElts[pd->getNumber()] = pElt;//pDevicesUiElts->append(pElt);
		}

		// remplir la combo des groupes
		comboBoxChooseEditGroup->clear(); 
		comboBoxChooseEditGroup->addItem("",0);
		foreach(Group* pg, VegaConfig::pCurrent->m_groups){
			if ( pg ) comboBoxChooseEditGroup->addItem( pg->getName() + QString(" (G%1)").arg( pg->getNumber() ) , pg->getNumber());
		}

		// remplir la combo des devices
		// todo: a faire dans tabDevicesConfChanged(0);
		comboBoxDeviceName->clear();
		comboBoxDeviceName->addItem("");
		foreach(Device* pd, VegaConfig::pCurrent->m_devices) {	
			if (pd && pd->getType() == DEVICE_CRM4 && pd->isEnabled()) 	{
				comboBoxDeviceName->addItem( VegaConfig::pCurrent->getDeviceUiEltNameByNumber( pd->getNumber() ) , pd->getNumber() );
			}
		}

		/*	reinitialize les combo box pour choisir conferencen, groupe, poste dans la	configuration touches	*/
		comboBoxConferenceNumberChoose->clear();//("",0);
		comboBoxConferenceNumberChoose->addItem("",0);
		for (int i =1 ; i <=8; i++)	{	
			comboBoxConferenceNumberChoose->addItem( QString("C") + QString::number(i) , i); 	
		}

		tabDevicesChanged(0); // affichage du panneau des terminaux de la configuration par defaut
		//slot_editConference(0); 
		//slot_editGroupConfiguration(0); 
		//slot_comboConference(0);
		//showDeviceKeysMapConfiguration();
	}
	
	//PropagateBoardStatusToConferences(); 
	//PropagateBoardStatusToDevices();

	connect(checkBoxEnableConference, SIGNAL(stateChanged(int)), this, SLOT(checkboxActivateConference(int))); 
	if (VegaConfig::pCurrent && VegaConfig::pRunning && (VegaConfig::pCurrent == VegaConfig::pRunning) ) 
		goOnlineMode();
	else 
		goOfflineMode();
}

void Ui_MainWindow::saveModifications()
{
	if (VegaConfig::pCurrent == NULL){
		msgBox("chargez d'abord une configuration");
		return;
	}
	int ret = confirmBox("Sauvegarder les modifications pour la configuration %s ?",VegaConfig::pCurrent->getName());
	if (ret == QMessageBox::Cancel)
		return;  
	else{
		if (m_readOnly){dbgBox("mode lecture seule");return;}
		VegaConfig::pCurrent->saveConfiguration();
	}
	if (VegaConfig::pRunning && VegaConfig::pCurrent == VegaConfig::pRunning)
	{
		/*dbgBox("TODO: envoi fichier par TCP au controle");
		dbgBox("%s sera remplacee par %s",
			VegaConfig::pRunning->getName(),
			VegaConfig::pCurrent->getName()
		);*/
		/*const char *name = VegaConfig::pRunning->getName();
		char *newName = getConfigurationNamePlusDate(name);
		if (newName) 
		{
			VegaConfig::pRunning->setNewName(newName);
			comboBoxChooseEditConfiguration->setItemText(comboBoxChooseEditConfiguration->currentIndex(), newName);
			free (newName);
		}*/
		statusBar->message(" modifications enregistrees, pensez a soumettre a nouveau", 10);
	}
}
void Ui_MainWindow::setReadOnlyMode(bool val)
{
  m_readOnly = val;

}
void Ui_MainWindow::setReadOnlyModeUI()
{
  
  lineEditPasswordOperator->setEnabled(false);
  lineEditPasswordManager->setEnabled(false);

  lineEditPasswordOperatorVerify->setEnabled(false);
  lineEditPasswordManagerVerify->setEnabled(false);
  pushButtonDeleteConfiguration->setEnabled(false);
  pushButtonCreateConfiguration->setEnabled(false);
  pushButtonDeleteGroup->setEnabled(false);
  pushButtonCreateGroup->setEnabled(false);
  lineEditConfigurationName->setEnabled(false);
  pushButtonSaveModifications->setEnabled(false);
  pushBtnApplyCurrentAsRunning->setEnabled(false);
  pushButtonRestartControl->setEnabled(false);
  pushButtonModifyPasswordOperator->setEnabled(false);
  pushButtonModifyPasswordManager->setEnabled(false);
	
}

