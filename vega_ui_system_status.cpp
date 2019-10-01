#include <QtGui>
#include <iostream>
#include "switch.h"
#include "vega_ihm.h"
#include "configuration.h"

void Ui_MainWindow::setElementNewStatus(const char *name, int number, board_state_t newState)
{
  QPushButton *pButton = NULL;
  QString str;

  
  if (!strcmp(name,"board"))
    str = "buttonBoardStatus" + QString::number(number);
  else if (!strcmp(name,"conference"))
    str = "buttonConferenceStatus" + QString::number(number);
  else if (!strcmp(name, "device")) 
    str = "buttonDeviceStatus" + QString::number(number);
  else if (!strcmp(name, "link")) 
    str = "buttonLinkState";
  
  if ((pButton = (QPushButton*)child(str) ) == NULL)
    return;
  
  if (newState == BOARD_STATE_OK) {
    pButton->setStyleSheet("QPushButton { background-color: " COLOR_GREEN " }");

  }
  else if (newState == BOARD_STATE_ANOMALY) {
    pButton->setStyleSheet("QPushButton { background-color: " COLOR_RED " }");

  }
  else if (newState == BOARD_STATE_ABSENT){
    pButton->setStyleSheet("QPushButton { background-color: white }");
  }
 
}
  
void Ui_MainWindow::initSystemStatus()
{
	int i = 0;
	QPushButton *pButton = NULL;
	QString str;
	//QList<Device*>::iterator id;

	QWidget *pannelStatus = new QWidget();

	QPushButton *buttonBoardColorOK;
	QPushButton *buttonBoardColorNOK;
	QPushButton *buttonBoardColorAbsent;
	QPushButton *buttonDeviceColorAvailable;
	QPushButton *buttonDeviceColorUnavailable;
	QGroupBox *groupBoxBoardsStatus =  new QGroupBox(pannelStatus);
	QGroupBox *groupBoxLinkAlpha =  new QGroupBox(pannelStatus);
	QGroupBox *groupBoxDevicesAvailability = new QGroupBox(pannelStatus);
	QGroupBox *groupBoxLegend= new QGroupBox(pannelStatus);


	groupBoxBoardsStatus->setGeometry(QRect(10, 50, 1025, 250));
	groupBoxDevicesAvailability->setGeometry(QRect(10, 300, 1025, 125));
	groupBoxLegend->setGeometry(QRect(10, 430, 1025, 35));
	groupBoxLinkAlpha->setGeometry(QRect(10, 5, 1025, 40));
	groupBoxBoardsStatus->setTitle(QString::fromUtf8("Etat des cartes"));
	groupBoxDevicesAvailability->setTitle(QString::fromUtf8("Disponibité"));
	groupBoxLegend->setTitle(QString::fromUtf8("Légende"));
	groupBoxLinkAlpha->setTitle("Lien entre commutateur et control");
  
	QPushButton *pButtonLink;
	QLabel *labelLink = new QLabel(groupBoxLinkAlpha);
	pButtonLink = new QPushButton(groupBoxLinkAlpha);
	pButtonLink->setStyleSheet("QPushButton { background-color: " COLOR_GREEN "  }"); 
	pButtonLink->setGeometry(QRect(100, 20, 200, 8));
	pButtonLink->setObjectName("buttonLinkState");
	pButtonLink->setEnabled(false);
	labelLink->setGeometry(10, 15, 80, 20);
	labelLink->setText("Etat du lien:");

  for (i = 0; i < 26; i++) 
  {  
    pButton = new QPushButton(groupBoxBoardsStatus);
    str = "buttonBoardStatus" + QString::number(i+1);
    pButton->setObjectName(str);
    pButton->setGeometry(QRect(20+(i*38), 40, 30, 200));
    pButton->setText(QString::number(i+1));
    setElementNewStatus("board", i+1, BOARD_STATE_ABSENT);
  }

	buttonBoardColorOK = new QPushButton(groupBoxBoardsStatus);
	buttonBoardColorOK->setStyleSheet("QPushButton { background-color: " COLOR_GREEN "  }"); 
	buttonBoardColorOK->setGeometry(QRect(20, 260, 20, 15));
	buttonBoardColorOK->setEnabled(false);

	QLabel *labelBoardOK = new QLabel(groupBoxBoardsStatus);
	labelBoardOK->setGeometry(QRect(45, 260, 80, 15));
	labelBoardOK->setText(QString::fromUtf8("carte OK"));
	buttonBoardColorNOK = new QPushButton(groupBoxBoardsStatus);
	buttonBoardColorNOK->setStyleSheet("QPushButton { background-color: " COLOR_RED "  }"); 
	buttonBoardColorNOK->setGeometry(QRect(115, 260, 20, 15));
	buttonBoardColorNOK->setEnabled(false);
	QLabel *labelBoardNOK = new QLabel(groupBoxBoardsStatus);
	labelBoardNOK->setGeometry(QRect(140, 260, 115, 15));
	labelBoardNOK->setText(QString::fromUtf8("carte en anomalie"));
	buttonBoardColorAbsent = new QPushButton(groupBoxBoardsStatus);
	buttonBoardColorAbsent->setGeometry(QRect(270, 260, 20, 15));
	buttonBoardColorAbsent->setEnabled(false);
	buttonBoardColorAbsent->setStyleSheet("QPushButton { background-color: white }"); 
	QLabel *labelBoardAbsent = new QLabel(groupBoxBoardsStatus);
	labelBoardAbsent->setGeometry(QRect(295, 260, 100, 15));  
	labelBoardAbsent->setText(QString::fromUtf8("carte absente"));

	//// conferences et devices
	QLabel *labelConferencesAvailabity = new QLabel(groupBoxDevicesAvailability);
	QLabel *labelDevicesAvailability = new QLabel(groupBoxDevicesAvailability);

	labelDevicesAvailability->setGeometry(QRect(10, 65, 200, 15));
	labelConferencesAvailabity->setGeometry(QRect(10, 25, 200, 15));
	labelDevicesAvailability->setText(QString::fromUtf8("Disponibilite postes :"));
	labelConferencesAvailabity->setText(QString::fromUtf8("Disponibilite conférences:"));

  for (i = 0; i < 10; i++) {  
    pButton = new QPushButton(groupBoxDevicesAvailability);
    str = "buttonConferenceStatus" + QString::number(i+1);
    pButton->setObjectName(str);
    pButton->setGeometry(QRect(200+(i*30), 20, 25, 25));
    pButton->setText(QString::number(i+1));
    setElementNewStatus("conference", i+1, BOARD_STATE_OK);
  }
  
  for (i = 0  ; i < 46; i++) {
    str = "buttonDeviceStatus" + QString::number(i+1);
    pButton = new QPushButton(groupBoxDevicesAvailability);
    pButton->setObjectName(str);
    pButton->setGeometry(QRect(150+((i%24)*36) , 60 + ((i/24) * 35), 27, 27));
    pButton->setText("D" + QString::number(i+1));
    setElementNewStatus("device", i+1, BOARD_STATE_OK);
  }


	buttonDeviceColorAvailable = new QPushButton(groupBoxLegend);
	buttonDeviceColorAvailable->setGeometry(QRect(10, 15, 20, 15));
	buttonDeviceColorAvailable->setEnabled(false);
	buttonDeviceColorAvailable->setStyleSheet("QPushButton { background-color: " COLOR_GREEN " }"); 

	QLabel *labelDeviceOK = new QLabel(groupBoxLegend);
	labelDeviceOK->setGeometry(QRect(35, 15, 130, 15));
	labelDeviceOK->setText(QString::fromUtf8("disponible"));

	buttonDeviceColorUnavailable = new QPushButton(groupBoxLegend);
	buttonDeviceColorUnavailable->setGeometry(QRect(130, 15, 20, 15));
	buttonDeviceColorUnavailable->setEnabled(false);
	buttonDeviceColorUnavailable->setStyleSheet("QPushButton { background-color: white } "); 

	QLabel *labelDeviceNOK = new QLabel(groupBoxLegend);
	labelDeviceNOK->setGeometry(QRect(160, 15, 130, 15));
	labelDeviceNOK->setText(QString::fromUtf8("indisponible"));

	QLabel *labelDeviceError = new QLabel(groupBoxLegend);
	labelDeviceError->setGeometry(QRect(300, 15, 130, 15));
	labelDeviceError->setText(QString::fromUtf8("en erreur"));

	QPushButton *buttonDeviceColorError = new QPushButton(groupBoxLegend);
	buttonDeviceColorError->setGeometry(QRect(275, 15, 20, 15));
	buttonDeviceColorError->setEnabled(false);

	buttonDeviceColorError->setStyleSheet("QPushButton { background-color: " COLOR_RED "}; "); 

	propagateBoardStatus();
	tabWidgetGeneral->addTab(pannelStatus, QString::fromUtf8("Etat du systeme"));
}

#if 0
void Ui_MainWindow::newAlarm(QList<alarm_t*> *pCurrentAlarms)
{
	  fprintf(stderr,"Ui_MainWindow::newAlarm\n");

  QList<Board*>::iterator ib;
  QList<int>::iterator i;
  QList<alarm_t*>::iterator itt;
  alarm_t *pAlarm = NULL;

  /// reinitialize l'affichage
  propagateBoardStatus();

  m_nbCurrentAlarms = pCurrentAlarms->count();

  if (m_nbCurrentAlarms == 0) 
    labelImageAlarme->setEnabled(false);
  else
    labelImageAlarme->setEnabled(true);
   
  /// parcoure toutes les alarmes et place en rouge tous les devices et boards en erreur
  for (itt = pCurrentAlarms->begin() ;  itt != pCurrentAlarms->end(); itt ++) {

    pAlarm = *itt;
    
    switch (pAlarm->type) {
    case EV_ALARM_BOARD_ANOMALY:
      {
	fprintf(stderr,"newAlarm type EV_ALARM_BOARD_ANOMALY:%d\n",pAlarm->type);
	for (ib = pBoards->begin(); ib != pBoards->end(); ib ++) 
	{
	  Board *pB = *ib;
	  if (pB->getPlace() == pAlarm->board_number) 
	    setElementNewStatus("board", pAlarm->board_number, BOARD_STATE_ANOMALY);
	}
      }
      break;
    case EV_ALARM_DEVICE_ANOMALY:
	fprintf(stderr,"newAlarm type EV_ALARM_DEVICE_ANOMALY:%d\n",pAlarm->type);
      setElementNewStatus("device", pAlarm->device_number, BOARD_STATE_ANOMALY);
      break;
 
    case EV_ALARM_CONTROL_NETWORK_DOWN:
	fprintf(stderr,"newAlarm type EV_ALARM_CONTROL_NETWORK_DOWN:%d\n",pAlarm->type);
      setElementNewStatus("link", 1, BOARD_STATE_ANOMALY);
      break;
    default:
	fprintf(stderr,"newAlarm type ?????? :%d\n",pAlarm->type);

      break;
    }
  }
  return;
}
#endif

void Ui_MainWindow::propagateBoardStatus()
{
  QList<Board*>::iterator ib;
  QList<int> *pDeviceDeps = NULL;
  QList<int>::iterator i;
  QList<int> *pConferenceDeps = NULL;
  Board *pB = NULL;  

  setElementNewStatus("link", 1, BOARD_STATE_OK);
  for (ib = pBoards->begin(); ib != pBoards->end(); ib ++) {
    pB = *ib;
  
    if (pB->isPresent() == false) 
      continue;
    
    setElementNewStatus("board", pB->getPlace(), BOARD_STATE_OK);
      
    pDeviceDeps = pB->getDeviceDependencies();
    
    if (pDeviceDeps) {	
      for (i = pDeviceDeps->begin(); i != pDeviceDeps->end(); i++) 
	setElementNewStatus("device", *i, BOARD_STATE_OK);
      
      delete pDeviceDeps; 
      pDeviceDeps = NULL;
    }
  }

  for (ib = pBoards->begin(); ib != pBoards->end(); ib ++) {
        pB = *ib;
    if (pB->isPresent() == true) 
      continue;
    
    setElementNewStatus("board", pB->getPlace(), BOARD_STATE_ABSENT);
   
    pDeviceDeps = pB->getDeviceDependencies();
    if (pDeviceDeps) {
      for (i = pDeviceDeps->begin(); i != pDeviceDeps->end(); i++) 
	setElementNewStatus("device", *i, BOARD_STATE_ABSENT);
     
      delete pDeviceDeps;
      pDeviceDeps = NULL;
    }

    pConferenceDeps = pB->getConferenceDependencies();
    if (pConferenceDeps)  {
      for (i = pConferenceDeps->begin(); i != pConferenceDeps->end(); i++)
	setElementNewStatus("conference", *i, BOARD_STATE_ABSENT);
      
      
      delete pConferenceDeps;
      pConferenceDeps = NULL;
    }
 
  }  
}
