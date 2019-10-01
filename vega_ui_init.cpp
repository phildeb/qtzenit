#include <iostream>
#include "vega_ihm.h"
#include "switch.h"
#include "debug.h"
#include "configuration.h"
#include "printer.h"
#include "liveEvents.h"
#include "configuration.h"

extern QPixmap* pixmcrm;// = QPixmap("/home/operateur/vega/imgs/crm4.png");
extern QPixmap* pixmradio;// = QPixmap("/home/operateur/vega/imgs/radio.png");
extern QPixmap* pixmjup;// = QPixmap("/home/operateur/vega/imgs/jupiter.png");

extern void * connexion_mysql_thread (void *);

/*void Ui_MainWindow::keyPressedEvent(QKeyEvent* event)
{
	dbgBox("keyPressedEvent");
}*/

void Ui_MainWindow::closeEvent(QCloseEvent* event)
{
	//dbgBox("Ui_MainWindow::closeEvent\n");
	fprintf(stderr,"Ui_MainWindow::closeEvent\n");
	if (confirmBox("L'application va se terminer, est ce que vous confirmez ?") == QMessageBox::Cancel) 
	{
		event->ignore();
		return;
	}else{
		//dbgBox("fermeture...");
		m_liveEvents.terminate();
		m_controlLink.terminate();
		// faut il sauvegarder ??
		exit (0);
	}
}

bool  Ui_MainWindow::event(QEvent* event)
{
	//fprintf(stderr,"Ui_MainWindow::event type: %d\n",event->type());
	if ( QEvent::KeyPress == event->type() ){
		QKeyEvent* key = static_cast<QKeyEvent*>(event);
		fprintf(stderr,"QEvent::KeyPress: %d\n",key->key() );
		if ( Qt::Key_Escape == key->key() ){
			//dbgBox("Qt::Key_Escape");
			return false;
		}
		if ( Qt::Key_F12 == key->key() ){
			exit(0);
		}
		if ( Qt::Key_F1 == key->key() ){
			QDate dnow = QDate::currentDate();
			if ( dnow.year() >= 2011 ) 
			{
				dbgBox("Conception du logiciel Debreuil Systems SARL 2009 www.debreuil.fr phone: +33 6 22 69 23 53");
			}
		}
	}
	QWidget::event(event);
}

void Ui_MainWindow::setupUi()//QMainWindow *MainWindow)
{
	QFileInfo fileInfo(SETTINGS_FILE);
	QRegExp rx( "([0-9]|[a-z]|[A-Z]|\\-)+" );
	QValidator* validator = new QRegExpValidator( rx, this );
	//centralwidget = new QWidget();//MainWindow);  //centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
	
	QWidget* mainw = new QWidget(this);//MainWindow);
	QGridLayout* maingrid = new QGridLayout(mainw);
	
	setCentralWidget(mainw);//centralwidget);
	setWindowTitle("VEGA "__DATE__" "__TIME__);
	setWindowIcon(QIcon("/home/operateur/vega/imgs/V.png"));

	/// DECORATION AVEC IMAGES
	{

		lcdDate = new QLCDNumber();
		//lcdDate->setObjectName(QString::fromUtf8("lcdDate"));
		lcdDate->setGeometry(QRect(65, 2, 200, 25));			
		maingrid->addWidget(lcdDate,0,1);
		lcdDate->setSegmentStyle(QLCDNumber::Filled);
		lcdDate->setNumDigits(16);
		
		pixmcrm = new QPixmap("/home/operateur/vega/imgs/crm4.png");
		pixmradio = new QPixmap("/home/operateur/vega/imgs/radio.png");
		pixmjup = new QPixmap("/home/operateur/vega/imgs/jupiter.png");
		imageVegaRouge = new QPixmap(PIXMAP_DIR"/vega_r.xpm");
		
		imageVegaVert = new QPixmap(PIXMAP_DIR"/vega_v.xpm");
		imageAlarme = new QPixmap(PIXMAP_DIR"/alarme_r.xpm");
		
		labelImageVega = new QLabel();
    
		//labelImageVega->setGeometry(QRect(350, 2 , 200, 20)); 
		maingrid->addWidget(labelImageVega,0,2);
		labelImageVega->setPixmap(*imageVegaRouge);
		labelImageVega->setVisible(false);

		pBtnLiaisonAlphacomControl = new QPushButton("LIAISON CTRL <-> ALPHACOM");
		maingrid->addWidget(pBtnLiaisonAlphacomControl,0,2);
	    //pConnexionIHM_Control->setStyleSheet("QPushButton { background-color: " COLOR_GREEN " }");
	    pBtnLiaisonAlphacomControl->setStyleSheet("QPushButton { background-color: " COLOR_RED" }");


		pBtnLiaisonIHMControl = new QPushButton("LIAISON IHM <-> CTRL");
		maingrid->addWidget(pBtnLiaisonIHMControl,0,3);
	    //pConnexionIHM_Control->setStyleSheet("QPushButton { background-color: " COLOR_GREEN " }");
	    pBtnLiaisonIHMControl->setStyleSheet("QPushButton { background-color: " COLOR_RED" }");


		labelImageAlarme = new QLabel();
		//labelImageAlarme->setGeometry(QRect(850, 2 , 200, 20));
		maingrid->addWidget(labelImageAlarme,0,4);
		labelImageAlarme->setEnabled(false);
		labelImageAlarme->setPixmap(*imageAlarme);
		labelImageAlarme->setVisible(false);


		pBtnAlimentationStatus= new QPushButton("ALARME ALIMENTATION");
		maingrid->addWidget(pBtnAlimentationStatus,0,4);
	    //pConnexionIHM_Control->setStyleSheet("QPushButton { background-color: " COLOR_GREEN " }");
	    pBtnLiaisonIHMControl->setStyleSheet("QPushButton { background-color: " COLOR_RED" }");

		labelImageLogo = new QLabel();
		//labelImageLogo->setGeometry(QRect(2, 2 , 60, 60));
		maingrid->addWidget(labelImageLogo,0,0);
		imageVegaLogo = new QPixmap(PIXMAP_DIR"/V2.xpm");
		labelImageLogo->setPixmap(*imageVegaLogo);

		QLabel* labelImageCrm4 = new QLabel();
		labelImageCrm4->setGeometry(QRect(950, 2 , 100, 20));
		labelImageCrm4->setPixmap(*pixmcrm);

		QLabel* labelImageJup = new QLabel();
		labelImageJup->setGeometry(QRect(1000, 2 , 100, 20));
		labelImageJup->setPixmap(*pixmjup);
		
		QLabel* labelImageRadio = new QLabel();
		labelImageRadio->setGeometry(QRect(1050, 2 , 100, 20));
		labelImageRadio->setPixmap(*pixmradio);


	}






	pannel_terminaux		= new QWidget();	//pannel_terminaux->setObjectName(QString::fromUtf8("tabConfigurationDevices"));
	pannel_touches			= new QWidget();	//pannel_touches->setObjectName(QString::fromUtf8("tab_32"));
	pannel_conference		= new QWidget();	//pannel_conference->setObjectName(QString::fromUtf8("tab_3"));
    pannel_groupes			= new QWidget();	//pannel_groupes->setObjectName(QString::fromUtf8("tabGroupes"));
    FenSQL* tabSQL			= new FenSQL(NULL);








	// >>>>>>>>>>>>>> pannel avec la table des terminaux

	tableDevices = new QTableWidget(pannel_terminaux);
	tableDevices->setObjectName(QString::fromUtf8("tableDevices"));
	tableDevices->setGeometry(QRect(20, 20, 670, 381));
	QStringList headers;headers << "No" << "Type"<< "Nom"<< "Gain"<< "FullDuplex" << "Detection audio";
    tableDevices->resizeColumnsToContents();
	tableDevices->setAlternatingRowColors(true);
	tableDevices->setColumnCount(6);
	
	tableDevices->setHorizontalHeaderLabels( headers ) ;
	{
		signalMapperGains = new QSignalMapper(this);
		signalMapperDuplex = new QSignalMapper(this);
		signalMapperContactSec = new QSignalMapper(this);
		// PARTIE AFFICHAGE TABLEAU DES DEVICES DE LA CONFERENCE SELECTIONNEE
		if ( 1 ) 
		{
			for (int i = 0; i<46; i++) // preparer tous les controles possibles dans la table (MAX)
			{	
				int rowcount = tableDevices->rowCount();
				int colcount = tableDevices->columnCount();
				tableDevices->insertRow(rowcount);

				int dnumber = i+1;//pd->getNumber(); // commence a 1
				for (int k=0;k<colcount;k++){
					QTableWidgetItem* pItem = new QTableWidgetItem(" - ");   
				
					//pItem->setBackground(Qt::cyan);
					tableDevices->setItem(i, k, pItem);

					//pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
				}
				{
					char str[256] = { 0 };
					sprintf(str, "D%d", dnumber);
					QTableWidgetItem* pItem = tableDevices->item(i, 0);
					if ( pItem ) 		
					{
						pItem->setText(str);
						pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
					
						  pItem->setFlags( (Qt::ItemFlag)0);
						//pItem->setEnabled(false);
						// todo : utiliser setItemData avec le numero du device
						//QLabel* plbl = new QLabel(str);
						//tableDevices->setCellWidget(i, 0, plbl);
					}
					if (m_readOnly) {
					  QTableWidgetItem* pItem = tableDevices->item(i, 2);
					  pItem->setFlags( (Qt::ItemFlag)0);
					
					}
				}
					
			


				QComboBox *	pComboBox = new QComboBox();
				pComboBox->setObjectName(QString::fromUtf8("comboBoxGain"));
				tableDevices->setCellWidget(i, 3, pComboBox);
				for (unsigned int k = 0 ; k < sizeof(Device::deviceGains) ; k++) { 
					QString str = QString::number(Device::deviceGains[k]);
					pComboBox->addItem(str);//gains[k]);
				}
				
				QCheckBox* pCheckBox1 = new QCheckBox();
				tableDevices->setCellWidget(i, 4, pCheckBox1);
				if (m_readOnly)
				  pCheckBox1->setEnabled(false);
				QCheckBox* pCheckBox2 = new QCheckBox();
				tableDevices->setCellWidget(i, 5, pCheckBox2);

				/*for (int k=0;k<colcount;k++){
					QTableWidgetItem* pItem = new QTableWidgetItem(" - ");   
					pItem->setBackground(Qt::cyan);
					pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
				}*/

				signalMapperGains->setMapping(pComboBox, i);
				connect(pComboBox, SIGNAL(currentIndexChanged(int)), signalMapperGains, SLOT(map()));
				signalMapperDuplex->setMapping(pCheckBox1, i);
				connect(pCheckBox1, SIGNAL(stateChanged(int)), signalMapperDuplex, SLOT(map()));
				signalMapperContactSec->setMapping(pCheckBox2, i);
				connect(pCheckBox2, SIGNAL(stateChanged(int)), signalMapperContactSec, SLOT(map()));
			}

		}	
		connect(tableDevices, SIGNAL(cellChanged(int, int)), this, SLOT(devicesNameChanged(int, int)));
		connect(signalMapperGains, SIGNAL(mapped(int)), this, SLOT(devicesGainChanged(int)));
		connect(signalMapperDuplex, SIGNAL(mapped(int)), this, SLOT(devicesDuplexChanged(int)));
		connect(signalMapperContactSec, SIGNAL(mapped(int)), this, SLOT(devicesContactSecChanged(int)));
	}


	// <<<<<<<<<<<<< pannel table des terminaux

	


	// >>>>>>>>>>>> TOUCHES DE GROUPES sur les CRM4




	groupBoxKeyConfiguration = new QGroupBox(pannel_touches);
	groupBoxKeyConfiguration->setObjectName(QString::fromUtf8("groupBoxKeyConfiguration"));
	groupBoxKeyConfiguration->setGeometry(QRect(690, 50, 210, 140));
	groupBoxKeyConfiguration->setVisible(true);
	groupBoxKeyConfiguration->setTitle("Configuration touches des postes CRM4");

	QLabel *label_3 = new QLabel(groupBoxKeyConfiguration);
	label_3->setObjectName(QString::fromUtf8("label_3"));
	label_3->setGeometry(QRect(10, 30, 70, 18));
	label_3->setText("Type action:");

	comboBoxChooseKeyType = new QComboBox(groupBoxKeyConfiguration);
	comboBoxChooseKeyType->setObjectName(QString::fromUtf8("comboBox"));
	comboBoxChooseKeyType->setGeometry(QRect(85, 30, 120, 23));
    comboBoxChooseKeyType->addItem("");
    comboBoxChooseKeyType->addItem("appel de groupe");


	QLabel *label_11 = new QLabel(groupBoxKeyConfiguration);
	label_11->setObjectName(QString::fromUtf8("label_11"));
	label_11->setGeometry(QRect(10, 70, 70, 18));
    label_11->setText("groupe :");
  
	comboBoxKeyGroupName = new QComboBox(groupBoxKeyConfiguration);
	comboBoxKeyGroupName->setObjectName(QString::fromUtf8("comboBoxKeyGroupName"));
	comboBoxKeyGroupName->setGeometry(QRect(60, 65, 100, 23));

	QPushButton *pushButtonKeyConfigureFinished = new QPushButton(groupBoxKeyConfiguration);
	pushButtonKeyConfigureFinished->setObjectName(QString::fromUtf8("lineEditKeyValue"));
	pushButtonKeyConfigureFinished->setGeometry(QRect(20, 110, 72, 23));
	pushButtonKeyConfigureFinished->setText("Valider");

	QLabel *labelMapDeviceType = new QLabel(pannel_touches);
	labelMapDeviceType->setObjectName(QString::fromUtf8("labelMapDeviceType"));
	labelMapDeviceType->setGeometry(QRect(20, 10, 50, 27));
    labelMapDeviceType->setText( "Poste :");

	comboBoxDeviceName = new QComboBox(pannel_touches);
	comboBoxDeviceName->setObjectName(QString::fromUtf8("comboBoxDeviceName"));
	comboBoxDeviceName->setGeometry(QRect(80, 10, 140, 27));

	groupBoxMapCRM4 = new QGroupBox(pannel_touches);
	groupBoxMapCRM4->setVisible(false);
	groupBoxMapCRM4->setTitle( "touches d'un terminal");
	groupBoxMapCRM4->setObjectName(QString::fromUtf8("groupBoxMapCRM4"));
	groupBoxMapCRM4->setGeometry(QRect(10, 50, 660, 365));


	{
	  
	  imageInterphone = new QPixmap(PIXMAP_DIR"/stentofon.xpm");
	  labelImageInterphone = new QLabel(groupBoxMapCRM4);
	  labelImageInterphone->setPixmap(*imageInterphone);
	  labelImageInterphone->setGeometry(QRect(10, 15 , 170, 160));
	    
		  

	  QFont* fontButtons = new QFont("Times", 8, QFont::Bold);
	  QPushButton *pButton = NULL;
	  int i;
	  for (i = 1; i <= 4 ; i++) {
	    pButton = new QPushButton(groupBoxMapCRM4);
	    pButton->setObjectName(QString("btnCRM4Key") + QString::number(i));
	    pButton->setGeometry(QRect(10 + (i-1)*45, 170, 40, 27));
	  }
	  signalMapperDeviceKeys = new QSignalMapper(this);
	  for (i = 0; i <= 47; i ++) { // 8 rangees de 6 touches
	    pButton = new QPushButton(groupBoxMapCRM4);
	    pButton->setObjectName(QString("btnCRM4Key") + QString::number(i+5) );//QString::fromUtf8("pushButtonDB" + QString::number(i+5)));
	    pButton->setGeometry(QRect(210 + (74 * (i%6)), 20 + (40 * (i / 6)), 72, 27));
	    pButton->setFont(*fontButtons);
	    
	    if ( i+5 >= 39 ) {
	      signalMapperDeviceKeys->setMapping(pButton, i+5);
	      connect(pButton, SIGNAL(clicked()), signalMapperDeviceKeys, SLOT(map()));
	    }
	    
	  }
	  connect(signalMapperDeviceKeys, SIGNAL(mapped(int)), this, SLOT(slot_crm4KeyConfigure(int)));
	}

    // touches clavier numerique
	QPushButton *pushButton_34 = new QPushButton(groupBoxMapCRM4);	pushButton_34->setGeometry(QRect(55, 210, 25, 27));
	QPushButton *pushButton_35 = new QPushButton(groupBoxMapCRM4);	pushButton_35->setGeometry(QRect(85, 210, 25, 27));
	QPushButton *pushButton_36 = new QPushButton(groupBoxMapCRM4);	pushButton_36->setGeometry(QRect(115, 210, 25, 27));
	QPushButton *pushButton_37 = new QPushButton(groupBoxMapCRM4);	pushButton_37->setGeometry(QRect(55, 240, 25, 27));
	QPushButton *pushButton_38 = new QPushButton(groupBoxMapCRM4);	pushButton_38->setGeometry(QRect(85, 240, 25, 27));
	QPushButton *pushButton_39 = new QPushButton(groupBoxMapCRM4);	pushButton_39->setGeometry(QRect(115, 240, 25, 27));
	QPushButton *pushButton_40 = new QPushButton(groupBoxMapCRM4);	pushButton_40->setGeometry(QRect(55, 270, 25, 27));
	QPushButton *pushButton_41 = new QPushButton(groupBoxMapCRM4);	pushButton_41->setGeometry(QRect(85, 270, 25, 27));
	QPushButton *pushButton_42 = new QPushButton(groupBoxMapCRM4);	pushButton_42->setGeometry(QRect(115, 270, 25, 27));
	QPushButton *pushButton_43 = new QPushButton(groupBoxMapCRM4);	pushButton_43->setGeometry(QRect(55, 300, 25, 27));
	QPushButton *pushButton_45 = new QPushButton(groupBoxMapCRM4);	pushButton_45->setGeometry(QRect(85, 300, 25, 27));
	QPushButton *pushButton_44 = new QPushButton(groupBoxMapCRM4);	pushButton_44->setGeometry(QRect(115, 300, 25, 27));
    pushButton_34->setText( "1");
    pushButton_35->setText( "2");
    pushButton_36->setText( "3");
    pushButton_37->setText( "4");
    pushButton_38->setText( "5");
    pushButton_39->setText( "6");
    pushButton_40->setText( "7");
    pushButton_41->setText( "8");
    pushButton_42->setText( "9");
    pushButton_43->setText( "M");
    pushButton_45->setText( "0");
    pushButton_44->setText( "C");

	QFrame *line_2 = new QFrame(groupBoxMapCRM4);
	line_2->setObjectName(QString::fromUtf8("line_2"));
	line_2->setGeometry(QRect(185, 20, 16, 341));
	line_2->setFrameShape(QFrame::VLine);
	line_2->setFrameShadow(QFrame::Sunken);



	

	// <<<<<<<<<<<<<< TOUCHES DE GROUPES sur les CRM4







	// >>>>>>>>>>>>>> panneau des conferences

	groupBoxEditionConferences = new QGroupBox(pannel_conference);
	groupBoxEditionConferences->setObjectName(QString::fromUtf8("groupBoxEditionConferences"));
	groupBoxEditionConferences->setGeometry(QRect(20, 20, 700, 310));
	groupBoxEditionConferences->setTitle("");

    listDevices = new QListWidget(groupBoxEditionConferences);
	listDevices->setAlternatingRowColors(true);
    listDevices->setObjectName(QString::fromUtf8("listDevices"));
    listDevices->setGeometry(QRect(270, 90, 171, 200));

	listConferenceMembers = new QListWidget(groupBoxEditionConferences);
	listConferenceMembers->setAlternatingRowColors(true);
	listConferenceMembers->setObjectName(QString::fromUtf8("listConferenceMembers"));
	listConferenceMembers->setGeometry(QRect(10, 90, 171, 200));
	
	labelConferenceDevices = new QLabel(groupBoxEditionConferences);
	labelConferenceDevices->setObjectName(QString::fromUtf8("labelConferenceDevices"));
	labelConferenceDevices->setGeometry(QRect(10, 70, 151, 18));
    labelConferenceDevices->setText( "Membres de la conferences");

	pushButtonInsertDevice = new QPushButton(groupBoxEditionConferences);
	pushButtonInsertDevice->setObjectName(QString::fromUtf8("pushButtonInsertDevice"));
	pushButtonInsertDevice->setGeometry(QRect(190, 120, 75, 28));
    pushButtonInsertDevice->setText( "<<");
	pushButtonDeleteDevice = new QPushButton(groupBoxEditionConferences);
	pushButtonDeleteDevice->setObjectName(QString::fromUtf8("pushButtonDeleteDevice"));
	pushButtonDeleteDevice->setGeometry(QRect(190, 170, 75, 28));
    pushButtonDeleteDevice->setText( ">>");

	labelDevices = new QLabel(groupBoxEditionConferences);
	labelDevices->setObjectName(QString::fromUtf8("labelDevices"));
	labelDevices->setGeometry(QRect(270, 70, 141, 18));
    labelDevices->setText( "Postes CRM4");

	comboBoxConferenceDirector = new QComboBox(groupBoxEditionConferences);
	comboBoxConferenceDirector->setObjectName(QString::fromUtf8("comboBoxConferenceDirector"));
	comboBoxConferenceDirector->setGeometry(QRect(450, 90, 141, 23));

	labelConferenceDirector = new QLabel(groupBoxEditionConferences);
	labelConferenceDirector->setObjectName(QString::fromUtf8("labelConferenceDirector"));
	labelConferenceDirector->setGeometry(QRect(450, 70, 61, 21));
    labelConferenceDirector->setText( "Directeur :");

	label_63 = new QLabel(groupBoxEditionConferences);
	label_63->setObjectName(QString::fromUtf8("label_63"));
	label_63->setGeometry(QRect(450, 120, 61, 18));
    label_63->setText( "Radio 1 :");

	comboBoxRadio1 = new QComboBox(groupBoxEditionConferences);
	comboBoxRadio1->setObjectName(QString::fromUtf8("comboBoxRadio1"));
	comboBoxRadio1->setGeometry(QRect(450, 140, 121, 23));
	labelConferenceRadio2 = new QLabel(groupBoxEditionConferences);
	labelConferenceRadio2->setObjectName(QString::fromUtf8("labelConferenceRadio2"));
	labelConferenceRadio2->setGeometry(QRect(450, 170, 61, 21));
    labelConferenceRadio2->setText( "Radio 2 :");

	comboBoxRadio2 = new QComboBox(groupBoxEditionConferences);
    comboBoxRadio2->setObjectName(QString::fromUtf8("comboBoxRadio2"));
    comboBoxRadio2->setGeometry(QRect(450, 190, 141, 23));

    label_65 = new QLabel(groupBoxEditionConferences);
    label_65->setObjectName(QString::fromUtf8("label_65"));
    label_65->setGeometry(QRect(450, 220, 61, 18));
    label_65->setText( "Jupiter :");

    comboBoxJupiter2 = new QComboBox(groupBoxEditionConferences);
    comboBoxJupiter2->setObjectName(QString::fromUtf8("comboBoxJupiter2"));
    comboBoxJupiter2->setGeometry(QRect(450, 240, 141, 23));

    labelConferenceNumberChoose = new QLabel(groupBoxEditionConferences);
    labelConferenceNumberChoose->setObjectName(QString::fromUtf8("labelConferenceNumberChoose"));
    labelConferenceNumberChoose->setGeometry(QRect(20, 30, 155, 18));
    labelConferenceNumberChoose->setText( "No conference:");

	// combo qui permet de choisir le numero de la conference
    comboBoxConferenceNumberChoose = new QComboBox(groupBoxEditionConferences);
    comboBoxConferenceNumberChoose->setObjectName(QString::fromUtf8("comboBoxConferenceNumberChoose"));
    comboBoxConferenceNumberChoose->setGeometry(QRect(120, 30, 80, 18));
	//comboBoxConferenceNumberChoose->setEditable(false);
    
    QLabel *labelEnableConference = new QLabel(groupBoxEditionConferences);
    labelEnableConference->setObjectName(QString::fromUtf8("labelEnableConference"));
    labelEnableConference->setGeometry(QRect(227, 32, 40, 18));
    labelEnableConference->setText( "Activer :");

    checkBoxEnableConference = new QCheckBox(groupBoxEditionConferences);
    checkBoxEnableConference->setGeometry(QRect(275, 27, 20, 24));

    labelConferenceName = new QLabel(groupBoxEditionConferences);
    labelConferenceName->setObjectName(QString::fromUtf8("labelConferenceName"));
    labelConferenceName->setGeometry(QRect(370, 32, 157, 20));
    labelConferenceName->setText( "Nom :");

        
    lineEditConferenceName = new QLineEdit(groupBoxEditionConferences);
    lineEditConferenceName->setObjectName(QString::fromUtf8("lineEditConferenceName"));
    lineEditConferenceName->setGeometry(QRect(510, 30, 171, 20));
    lineEditConferenceName->setMaxLength(12);
    lineEditConferenceName->setValidator( validator );


	// <<<<<<<<<<< tab conferences









	// <<<<<<<<<<<gestion des conferences













    // ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    // >>>>>>>>>> panneau de construction des groupes ( pannel_groupes ) 

    pushButtonDeleteGroup = new QPushButton(pannel_groupes);
    pushButtonDeleteGroup->setObjectName(QString::fromUtf8("pushButtonDeleteGroup"));
    pushButtonDeleteGroup->setGeometry(QRect(420, 19, 150, 23));

    comboBoxChooseEditGroup = new QComboBox(pannel_groupes);
    comboBoxChooseEditGroup->setObjectName(QString::fromUtf8("comboBoxChooseEditGroup"));
    comboBoxChooseEditGroup->setGeometry(QRect(80,  19, 150, 23));
	//comboBoxChooseEditGroup->addItem(""); 

    pushButtonCreateGroup = new QPushButton(pannel_groupes);
    pushButtonCreateGroup->setObjectName(QString::fromUtf8("pushButtonCreateGroup"));
    pushButtonCreateGroup->setGeometry(QRect(250, 19, 150, 23));

	groupBoxGroupEdition = new QGroupBox(pannel_groupes);
    groupBoxGroupEdition->setObjectName(QString::fromUtf8("groupBoxGroupEdition"));
    groupBoxGroupEdition->setGeometry(QRect(20, 70, 500, 380));

    lineEditGroupName = new QLineEdit(groupBoxGroupEdition);
    lineEditGroupName->setObjectName(QString::fromUtf8("lineEditGroupName"));
    lineEditGroupName->setGeometry(QRect(60, 20, 100, 20));
    lineEditGroupName->setMaxLength(12);
    lineEditGroupName->setValidator( validator );
    labelGroupName  = new QLabel(groupBoxGroupEdition);
    labelGroupName->setGeometry(QRect(10, 20, 70, 20));


    labelEditGroup = new QLabel(pannel_groupes);
    labelEditGroup->setObjectName(QString::fromUtf8("labelEditGroup"));
    labelEditGroup->setGeometry(QRect(30, 23, 70, 18));
    labelEditGroup->setText( "Editer :");


    QLabel *labelGroupMembers  = new QLabel(groupBoxGroupEdition);
    labelGroupMembers->setGeometry(QRect(20, 50, 150, 20));


    QLabel *labelGroupDevices  = new QLabel(groupBoxGroupEdition);
    labelGroupDevices->setGeometry(QRect(280, 50, 150, 20));

    listWidgetGroupMembers = new QListWidget(groupBoxGroupEdition);
	listWidgetGroupMembers->setAlternatingRowColors(true);
    listWidgetGroupMembers->setObjectName(QString::fromUtf8("listWidgetGroupMembers"));
    listWidgetGroupMembers->setGeometry(QRect(20, 70, 171, 301));
    
	QLabel *label_52 = new QLabel(groupBoxGroupEdition);
    label_52->setObjectName(QString::fromUtf8("label_52"));
    label_52->setGeometry(QRect(280, 50, 141, 18));
    listWidgetGroupDevices = new QListWidget(groupBoxGroupEdition);
	listWidgetGroupDevices->setAlternatingRowColors(true);
    listWidgetGroupDevices->setObjectName(QString::fromUtf8("listWidgetGroupDevices"));
    listWidgetGroupDevices->setGeometry(QRect(280, 70, 181, 301));
    
	QPushButton *pushButtonGroupInsertMember = new QPushButton(groupBoxGroupEdition);
    pushButtonGroupInsertMember->setObjectName(QString::fromUtf8("pushButtonGroupInsertMember"));
    pushButtonGroupInsertMember->setGeometry(QRect(200, 140, 75, 28));
    pushButtonGroupInsertMember->setText( "<<");

    QPushButton *pushButtonGroupDeleteMember = new QPushButton(groupBoxGroupEdition);
    pushButtonGroupDeleteMember->setObjectName(QString::fromUtf8("pushButtonGroupDeleteMember"));
    pushButtonGroupDeleteMember->setGeometry(QRect(200, 180, 75, 28));
    pushButtonGroupDeleteMember->setText( ">>");
    
	pushButtonCreateGroup->setText( "Creer");
    pushButtonDeleteGroup->setText( "Supprimer");

    groupBoxGroupEdition->setTitle( "Edition du groupe");
    labelGroupName->setText( "Nom :");
    labelGroupMembers->setText( "Membres du groupe");
    labelGroupDevices->setText( "Postes disponibles");

    // <<<<<<<<<< fin tab groupes





	// >>>>>>>>>gestion des configurations 


	//QLabel *labelCurrentConfigurationNameInUse = new QLabel();
	//labelCurrentConfigurationNameInUse->setObjectName(QString::fromUtf8("labelCurrentConfigurationNameInUse"));
	//labelCurrentConfigurationNameInUse->setGeometry(QRect(600, 610, 400, 30));
	//maingrid->addWidget(labelCurrentConfigurationNameInUse,1,0);
	//labelCurrentConfigurationNameInUse->setText("Configuration courante: ");

    comboBoxChooseEditConfiguration = new QComboBox();//);
    //comboBoxChooseEditConfiguration->setObjectName(QString::fromUtf8("comboBoxChooseEditConfiguration"));
    //comboBoxChooseEditConfiguration->setGeometry(QRect(110, 40, 250, 20));
	maingrid->addWidget(comboBoxChooseEditConfiguration,1,0);

    pushButtonDeleteConfiguration = new QPushButton();//);
    //pushButtonDeleteConfiguration->setObjectName(QString::fromUtf8("pushButtonCreateConference"));
    //pushButtonDeleteConfiguration->setGeometry(QRect(460, 40, 80, 20));
	maingrid->addWidget(pushButtonDeleteConfiguration,1,1);
    pushButtonDeleteConfiguration->setText( "Supprimer config");

    pushButtonPrintConfiguration = new QPushButton();//);
    //pushButtonPrintConfiguration->setObjectName(QString::fromUtf8("pushButtonPrintConfiguration"));
    //pushButtonPrintConfiguration->setGeometry(QRect(550, 40, 90, 20));
	maingrid->addWidget(pushButtonPrintConfiguration,1,2);
    pushButtonPrintConfiguration->setText("Imprimer");

    pushButtonCreateConfiguration = new QPushButton();//);
    //pushButtonCreateConfiguration->setObjectName(QString::fromUtf8("pushButtonCreateConference"));
    //pushButtonCreateConfiguration->setGeometry(QRect(370, 40, 80, 20));
	maingrid->addWidget(pushButtonCreateConfiguration,1,3);
	pushButtonCreateConfiguration->setText( "Nouvelle");

    //QGroupBox *groupBoxCurrentConfiguration = new QGroupBox();//tab_3); ///  pannel_conference  ?????
    //groupBoxCurrentConfiguration->setObjectName(QString::fromUtf8("groupBoxCurrentConfiguration"));
    //groupBoxCurrentConfiguration->setGeometry(QRect(10, 400, 700, 50));
    //groupBoxCurrentConfiguration->setVisible(false);
	//maingrid->addWidget(groupBoxCurrentConfiguration,1,1);

    /// un label box + combo box pour choisir la configuration courante
	//QLabel *labelCurrentConfiguration = new QLabel();//groupBoxCurrentConfiguration);
    //labelCurrentConfiguration->setObjectName(QString::fromUtf8("labelconfigurationcourante"));
    //labelCurrentConfiguration->setGeometry(QRect(10, 20, 150, 20));
	//maingrid->addWidget(labelCurrentConfiguration,1,2);
    //labelCurrentConfiguration->setText( "Configuration courante :");
    
	/// Combo box  qui permet de choisir la configuration courante
    //comboBoxCurrentConfiguration = new QComboBox();//groupBoxCurrentConfiguration);
    //comboBoxCurrentConfiguration->setObjectName(QString::fromUtf8("comboBoxCurrentConfiguration"));
    //comboBoxCurrentConfiguration->setGeometry(QRect(180, 20, 150, 20));
	//maingrid->addWidget(comboBoxCurrentConfiguration,1,3);

    // nom de la configuration
    //labelConferenceConfigurationName = new QLabel();//);
    //labelConferenceConfigurationName->setObjectName(QString::fromUtf8("labelConferenceConfigurationName"));
    //labelConferenceConfigurationName->setGeometry(QRect(650, 40, 65, 20));
    //labelConferenceConfigurationName->setText( "Renommer:");
	//maingrid->addWidget(labelConferenceConfigurationName,1,5);

    lineEditConfigurationName = new QLineEdit();//);
    //lineEditConfigurationName->setObjectName(QString::fromUtf8("lineEditConfigurationName"));
    //lineEditConfigurationName->setGeometry(QRect(720, 40, 220, 20));
	//maingrid->addWidget(lineEditConfigurationName,1,6,1,2);
    lineEditConfigurationName->setValidator( validator );
    
    QLabel* labelEditConfiguration = new QLabel();//);
    //labelEditConfiguration->setObjectName(QString::fromUtf8("label"));
    //labelEditConfiguration->setGeometry(QRect(65 , 40, 70, 20));
	//maingrid->addWidget(labelEditConfiguration,1,7,1,3);
    //labelEditConfiguration->setText( "Editer :");



	// LES CONFIGs 
	comboBoxChooseEditConfiguration->clear();
	comboBoxChooseEditConfiguration->addItem("<aucune>",-1);
	comboBoxChooseEditConfiguration->setCurrentIndex(0);
	//comboBoxChooseEditConfiguration->setItemData(0, Qt::green, Qt::ForegroundRole);
	comboBoxChooseEditConfiguration->setItemData(0, Qt::yellow, Qt::BackgroundRole);
	//comboBoxChooseEditConfiguration->setStyleSheet("QPushButton { background-color: " COLOR_RED " }");
	foreach(VegaConfig* pvcfg, VegaConfig::m_qlist)	{
		comboBoxChooseEditConfiguration->addItem(QIcon("/home/operateur/vega/imgs/flag_red.png"),pvcfg->getName(),pvcfg->getNumber());
		//int index = comboBoxChooseEditConfiguration->count()-1;
		//comboBoxChooseEditConfiguration->setItemData(index, Qt::yellow, Qt::BackgroundRole);
	}


  	//groupBoxEditionConferences->setVisible(false);
	//labelConferenceConfigurationName->setVisible(false);
	//lineEditConfigurationName->setVisible(false);

	// afficher la configuration running sur le pc de controle
	/*if (VegaConfig::pRunning) 
	{
		int idx = 0;
		fprintf(stderr,"Ui_MainWindow::showInitialConfig: VegaConfig::pRunning = %p\n",VegaConfig::pRunning);

		for (int i = 0; i < comboBoxChooseEditConfiguration->count(); i++) 
		{
			QString txt = comboBoxChooseEditConfiguration->itemText(i);
			if (!strcmp(txt.latin1(), VegaConfig::pRunning->getName())) {
				idx  = i;
			}
		}
		comboBoxChooseEditConfiguration->setCurrentIndex(idx);
		tabWidgetGeneral->setVisible(true);
	}else 
	{
		//tabWidgetGeneral->setVisible(true);
	}*/

	////////////////////////// TABBBED PANNELS ( conferences, groupes, touches, ....)

	//QWidget *verticalLayout = new QWidget();//	verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
	//verticalLayout->setGeometry(QRect(10, 80, 1071, 515));
	tabWidgetGeneral = new QTabWidget();//verticalLayout);//	tabWidgetGeneral->setObjectName(QString::fromUtf8("tabWidget"));
	maingrid->addWidget(tabWidgetGeneral,3,0,6,5);
	
	//QVBoxLayout* vboxLayout = new QVBoxLayout(verticalLayout);//	vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    //vboxLayout->addWidget(tabWidgetGeneral);
    tabWidgetGeneral->addTab(pannel_conference, "Conferences");// QString("line:%1").arg(__LINE__));//QString("groupes"));
    tabWidgetGeneral->addTab(pannel_groupes, "Groupes");//QString("line:%1").arg(__LINE__));
	tabWidgetGeneral->addTab(pannel_terminaux,  "Terminaux");//QString("line:%1").arg(__LINE__));//QString("Caracteristiques des Terminaux"));
	tabWidgetGeneral->addTab(pannel_touches, "Touches");//QString("line:%1").arg(__LINE__));
	tabWidgetGeneral->addTab( tabSQL, "Recherche" );
	initAuthenticationPassword(); // pannel de changement des mots de passe
	//tabWidgetGeneral->addTab( tabConf, "Conferences (new)" );
    //tabWidgetGeneral->addTab(new SharedDirTab(fileInfo), "Config en cours");
    



	///////////////////// BOUTONS REDEMARRER, SAUVEGARDER, etc...

    /*QFrame *line = new QFrame();
    line->setObjectName(QString::fromUtf8("line"));
    line->setGeometry(QRect(20, 640, 1050, 16));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);*/

    pushButtonSaveModifications = new QPushButton();
	pushButtonSaveModifications->setText( "Sauvegarder les modifications");
    //pushButtonSaveModifications->setObjectName(QString::fromUtf8("pushButtonSaveModifications"));
    //pushButtonSaveModifications->setGeometry(QRect(30, 610, 200, 28));
	maingrid->addWidget(pushButtonSaveModifications,9,0);

    pushButtonExport = new QPushButton();
    //pushButtonExport->setObjectName(QString::fromUtf8("pushButtonExport"));
    //pushButtonExport->setGeometry(QRect(680, 610, 100, 28));
	maingrid->addWidget(pushButtonExport,9,1);
    pushButtonExport->setText("Exporter");
    
    pushButtonRestartControl = new QPushButton();
    //pushButtonRestartControl->setObjectName(QString::fromUtf8("pushButtonRestartControl"));
    //pushButtonRestartControl->setGeometry(QRect(510, 610, 150, 28));
	maingrid->addWidget(pushButtonRestartControl,9,2);
    pushButtonRestartControl->setText("Redemarrer le control");
    
    pushBtnApplyCurrentAsRunning = new QPushButton();
    //pushBtnApplyCurrentAsRunning->setObjectName(QString::fromUtf8("pushButtonApplyCurrentConfigAsRunningConfig"));
    //pushBtnApplyCurrentAsRunning->setGeometry(QRect(250, 610, 240, 28));
	maingrid->addWidget(pushBtnApplyCurrentAsRunning,9,3);
    pushBtnApplyCurrentAsRunning->setText( "Soumettre");


    /*QFrame *line_20 = new QFrame();
    line_20->setObjectName(QString::fromUtf8("line_20"));
    line_20->setGeometry(QRect(20, 590, 1050, 16));
    line_20->setFrameShape(QFrame::HLine);
    line_20->setFrameShadow(QFrame::Sunken);
    
	QLabel *label_14 = new QLabel();
    label_14->setObjectName(QString::fromUtf8("label_14"));
    label_14->setGeometry(QRect(20, 650, 231, 18));
    label_14->setText( "Alarmes et evenements au fil de l'eau:");*/
    
	///// initialize le composant affichant les evenement au fil de l'eau

	tableLiveEvents = new QTableWidget();//View();
	//tableLiveEvents->setObjectName(QString::fromUtf8("tableLiveEvents"));
	//tableLiveEvents->setGeometry(QRect(17, 670, 1057, 185));
	maingrid->addWidget(tableLiveEvents,10,0,2,5);
	tableLiveEvents->setFont( QFont("Times", 8 , QFont::Bold ) ) ;	
	//tableLiveEvents->setRowHeight(i, 14);

	tableLiveEvents->setColumnCount(5);
	tableLiveEvents->setAlternatingRowColors(true);
	tableLiveEvents->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	tableLiveEvents->setSelectionMode(QAbstractItemView::NoSelection);
	tableLiveEvents->setEditTriggers( QAbstractItemView::NoEditTriggers );
	  QStringList list; list << "Date" << "Heure" << "Type" << "Message" << "Code" ;
	tableLiveEvents->setHorizontalHeaderLabels(list);
	tableLiveEvents->setColumnWidth(3, 510); 

	int rowcount=0;
	/*{
			rowcount = tableLiveEvents->rowCount();
			tableLiveEvents->insertRow(rowcount );
			QTableWidgetItem* item0=new QTableWidgetItem ("123");	item0->setBackgroundColor( Qt::yellow );
			QTableWidgetItem* item1=new QTableWidgetItem ("description");	
			QTableWidgetItem* item2=new QTableWidgetItem ("345");	
			tableLiveEvents->setItem(rowcount,0,item0);
			tableLiveEvents->setItem(rowcount,1,item1);
			tableLiveEvents->setItem(rowcount,2,item2);

			tableLiveEvents->item(rowcount,1)->setText("jklhkljlkjl");
			tableLiveEvents->item(rowcount,2)->setText("opghkpghkopg");
			tableLiveEvents->resizeColumnsToContents();
	}*/

	statusBar = new QStatusBar();
	setStatusBar(statusBar);


    //QMetaObject::connectSlotsByName(MainWindow);
	connect(tabWidgetGeneral,					SIGNAL(currentChanged(int)),this,			SLOT(tabGeneralChanged(int)));
	//QObject::connect((QObject*)this->tabWidgetDevices, SIGNAL(currentChanged(int)), (QObject*)this, SLOT(tabDevicesConfChanged(int)));
	connect(comboBoxChooseEditConfiguration,	SIGNAL(currentIndexChanged(int)), this,		SLOT(slot_comboConfiguration(int)));
	connect(comboBoxChooseEditGroup,			SIGNAL(currentIndexChanged(int)), this,		SLOT(slot_comboGroup(int)));
	connect(comboBoxConferenceNumberChoose,		SIGNAL(currentIndexChanged(int)), this,		SLOT(slot_comboConference(int)));
	connect(comboBoxDeviceName,					SIGNAL(currentIndexChanged(int)), this,		SLOT(slot_deviceKeysConfigurationDeviceChanged(int)));  

	//connect(comboBoxCurrentConfiguration,		SIGNAL(currentIndexChanged(int)), this,		SLOT(configurationChanged(int)));
	//connect(lineEditConfigurationName,			SIGNAL(editingFinished()), this,			SLOT(configurationNameChanged()));
	connect(pushButtonCreateConfiguration,		SIGNAL(clicked()),  this,					SLOT(creationConfiguration()));
	connect(pushButtonDeleteConfiguration,		SIGNAL(clicked()), this,					SLOT(deleteCurrentConfiguration()));	
	connect(checkBoxEnableConference,			SIGNAL(stateChanged(int)), this,			SLOT(checkboxActivateConference(int)));
	connect(lineEditConferenceName,				SIGNAL(editingFinished()), this,			SLOT(conferenceNameChanged()));
	connect(pushButtonInsertDevice,				SIGNAL(clicked()), this,					SLOT(insertDeviceInConferenceClicked()));
	connect(pushButtonDeleteDevice,				SIGNAL(clicked()), this,					SLOT(deleteDeviceInConferenceClicked()));
	connect(comboBoxRadio1,						SIGNAL(currentIndexChanged(int)), this,		SLOT(conferenceRadio1Changed(int)));
	connect(comboBoxRadio2,						SIGNAL(currentIndexChanged(int)), this,		SLOT(conferenceRadio2Changed(int)));
	connect(comboBoxJupiter2,					SIGNAL(currentIndexChanged(int)), this,		SLOT(conferenceJupiter2Changed(int)));
	connect(comboBoxConferenceDirector,			SIGNAL(currentIndexChanged(int)), this,		SLOT(conferenceDirectorChanged(int)));
	connect(pushBtnApplyCurrentAsRunning,		SIGNAL(clicked()), this,					SLOT(applyCurrentConfigAsRunningConfig()));
	connect(pushButtonCreateGroup,				SIGNAL(clicked()), this,					SLOT(createNewGroup()));
	connect(pushButtonSaveModifications,		SIGNAL(clicked()), this,					SLOT(saveModifications()));
	connect(pushButtonGroupInsertMember,		SIGNAL(clicked()), this,					SLOT(groupAddMember()));
	connect(pushButtonGroupDeleteMember,		SIGNAL(clicked()), this,					SLOT(groupRemoveMember()));
	connect(lineEditGroupName,					SIGNAL(editingFinished()), this,			SLOT(groupNameChanged()));
	connect(pushButtonDeleteGroup,				SIGNAL(clicked()), this,					SLOT(deleteGroup()));
	
	connect(pushButtonExport,					SIGNAL(clicked()), this,					SLOT(exportConfigurations()));
	connect(pushButtonRestartControl,			SIGNAL(clicked()), this,					SLOT(restartControl()));
	connect(pushButtonPrintConfiguration,		SIGNAL(clicked()), this,					SLOT(printConfiguration()));

	connect(pushButtonKeyConfigureFinished,		SIGNAL(clicked()), this,					SLOT(deviceKeyConfiguredOK()));  
	connect(comboBoxChooseKeyType ,				SIGNAL(currentIndexChanged(int)), this,		SLOT(keyActionChanged(int)));

} // setupUi

void Ui_MainWindow::init()
{
	//initSystemStatus();

	/*liveEvents = new LiveEvents();  
	if(liveEvents) {
		liveEvents->start();
		connect(liveEvents, SIGNAL(updateEvents(const QList<live_event_t*> *)), this, SLOT(updateLiveEvents(const QList<live_event_t*> *)));
		connect(liveEvents, SIGNAL(finished()), this, SLOT(dbConnectionLost()));
		connect(liveEvents, SIGNAL(alarmReceived(QList<alarm_t*>*)), this , SLOT(newAlarm( QList<alarm_t*>*)));
	}*/


	QTimer *timerStatusBar = new QTimer(this);
    connect(timerStatusBar, SIGNAL(timeout()), this, SLOT(updateStatus()));
    timerStatusBar->start(5000);

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(slot_showTime()));
	timer->start(1000);

	QTimer *timer2 = new QTimer(this);
	connect(timer2, SIGNAL(timeout()), this, SLOT(slot_getRunningConfig()));
	timer2->setSingleShot(true);
	timer2->start(5000);

	initControlConnectionTCP();

	QTimer *timer3 = new QTimer(this);
	connect(&m_liveEvents, SIGNAL(signal_linkControlAlphacomUp()), this, SLOT(slot_linkControlAlphacomUp()));
	connect(&m_liveEvents, SIGNAL(signal_linkControlAlphacomDown()), this, SLOT(slot_linkControlAlphacomDown()));
	connect(&m_liveEvents, SIGNAL(signal_AlimentationUp()), this, SLOT(slot_AlimentationUp()));
	connect(&m_liveEvents, SIGNAL(signal_AlimentationDown()), this, SLOT(slot_AlimentationDown()));
	m_liveEvents.start();
	//pthread_t t_id;pthread_create (&t_id, NULL, connexion_mysql_thread, NULL);
	connect(timer3, SIGNAL(timeout()), this, SLOT(slot_timerJournaldeBord()));
	timer3->start(3000);

}

