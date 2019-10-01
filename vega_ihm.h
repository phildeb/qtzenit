#ifndef   	VEGA_IHM_HH_
#define   	VEGA_IHM_HH_

#include <QtGui>
#include <qabstractsocket.h>
#include <qtcpsocket.h>
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>

#include "switch.h"
#include "liveEvents.h"
#include "parameters.h"
#include "pannels.h"

#define HOST_CONTROL_IP		"169.254.1.1"
#define HOST_CONTROL_PORT       50000
//#define HOST_CONTROL_IP		"127.0.0.1"
//#define HOST_CONTROL_IP		"192.168.0.252"
//#define DB_HOST		"169.254.1.1" //"localhost"
//#define MYSQL_IP_ADDRESS DB_HOST 
#define DB_PASSWORD	"zenitel"
#define DB_USER		"zenitel"
#define DB_NAME		"zenitel"

typedef enum{

	EV_LOG_CONFERENCE_ACTIVATED		= 100,
	EV_LOG_CONFERENCE_DEACTIVATED	= 101,
		
	EV_LOG_DEVICE_START_SPEAKING		= 200,
	EV_LOG_DEVICE_STOP_SPEAKING			= 201,
	EV_LOG_DEVICE_START_GENERAL_CALL	= 202,
	EV_LOG_DEVICE_STOP_GENERAL_CALL		= 203,
	EV_LOG_DEVICE_ADDED					= 204,
	EV_LOG_DEVICE_REMOVED				= 205,
	EV_LOG_DEVICE_PLUGGED				= 206, //=> alarme en rouge sur IHM
	EV_LOG_DEVICE_UNPLUGGED				= 207, //=> alarme en rouge sur IHM
	EV_LOG_DEVICE_EXCLUDED				= 208,
	EV_LOG_DEVICE_ASK_REINCLUSION		= 209,
	EV_LOG_DEVICE_SWITCH_CONFERENCE		= 210,
	EV_LOG_DEVICE_START_GROUP_CALL		= 211,
	EV_LOG_DEVICE_STOP_GROUP_CALL		= 212,
	EV_LOG_DEVICE_INCLUDED				= 213,

	//EV_LOG_VEGA_CONTROL_STARTED		= 300, => alarme
	//EV_LOG_VEGA_CONTROL_RESTARTED	= 301, => alarme
	//EV_LOG_VEGA_CONTROL_NETWORK_DOWN= 302, => alarme
	//EV_LOG_VEGA_CONTROL_NETWORK_UP	= 303, => alarme
	EV_LOG_CONFIGURATION_RELOADED	= 304, // seul evt qui est a la fois une alarme et un evenement !
	EV_LOG_VEGA_GUI_STARTED			= 305,
	EV_LOG_VEGA_GUI_STOPPED			= 306,


	
	EV_ALARM_AGA_BOARD_PLUGGED		= 400,
	EV_ALARM_AGA_BOARD_UNPLUGGED	= 401,
	EV_ALARM_ASLT_BOARD_PLUGGED		= 402,
	EV_ALARM_ASLT_BOARD_UNPLUGGED	= 403,
	EV_ALARM_BOARD_ANOMALY			= 404,
	EV_ALARM_END_BOARD_ANOMALY		= 405,
	EV_ALARM_DEVICE_ANOMALY			= 406,
	EV_ALARM_END_DEVICE_ANOMALY		= 407,
	EV_ALARM_RESTARTED_BY_GUI		= 408,
	EV_ALARM_CONTROL_NETWORK_DOWN	= 409,
	EV_ALARM_CONTROL_NETWORK_UP		= 410,
	EV_ALARM_POWER_DOWN				= 411,
	EV_ALARM_POWER_UP				= 412,
	EV_ALARM_CONFIGURATION_RELOADED = 413

}EV_LOG_T;

class LiveEvents : public QThread 
{
  Q_OBJECT

public:
	QSqlDatabase m_dbConnection;  
	int m_lastEventTs ; //=time(NULL);
	int m_lastAlarmTs ;//= time(NULL);
	QLinkedList<QStringList *>	qlist_journalDeBord;
	QMutex						qlist_journalDeBord_mutex;
	void run();

signals:
	void signal_linkControlAlphacomUp();
	void signal_linkControlAlphacomDown();
	void signal_AlimentationUp();
	void signal_AlimentationDown();

};
class controlLink : public QThread 
{
	Q_OBJECT
public:
	controlLink();
	//bool sendMessage(const char *);
	bool sendFichierConfigTCP(const char *nomConfig, const char* nomFichier );
	bool sendRestartRequestTCP();
	bool getFichierConfigTCP();
	bool isConnected();
	void run();
	//void activateKeepAlive(QTcpSocket *);

private:
	bool m_stop;
	bool m_connected;
	QTcpSocket *m_socket;
	bool m_restartRequestPending;
	QByteArray m_bufferEmission;
	QByteArray m_bufferReception;
	QList<QString*> m_msgsToSend;

public slots:

	void eventConnected();
	void eventDisconnected();

signals:
  
	//void eventReceivedACK();
	//void eventReceivedNACK();
	//void eventRestartResponseOK();
	//void eventRestartResponseNOK();
	//void eventConnectionEstablished();

	void signal_linkIHMControlDown();
	void signal_linkIHMControlUp();
	void signal_got_running_config(); // fichier vegactrl.conf recu
};
class VegaConfig;

class FenSQL : public QWidget
{
	Q_OBJECT;
public slots:
	void mettre_a_jour(int result_ping);
	void exec_sql(QString& req);

private:
	QComboBox* combo_conferences;
	QComboBox* combo_alarmes;
	QComboBox* combo_evenements;
	QComboBox* combo_poste;

	QLabel* lbl_alphacom_link;

	QCalendarWidget* cal_deb;
	QCalendarWidget* cal_fin;

	QPushButton* btn_evenements;
	QPushButton* btn_alarmes;

	QListWidget* m_list;
	QTableWidget* m_table;
	
	QDateTimeEdit *dateEditDeb;
	QDateTimeEdit *dateEditFin;

	  VegaConfig *lastConfig;	

public:	
	FenSQL(QWidget* parent);
	QString m_reqsql;
	QString m_reqsql_alarmes;
	  QString poste_filter;

public slots:	
	void my_slot_recherche_evenement();	
	void my_slot_recherche_alarme();	
	void my_slot_calendar();	
	void my_slot_remplir(int);	
	void my_slot_imprimer();	
	  void my_slot_remplir_postes(int i);
	  
};

class ConfDirTab : public QWidget
{
    Q_OBJECT

public:
    ConfDirTab(const QFileInfo &fileInfo, QWidget *parent = 0);
private:
	QDirModel* model;
	QTreeView* tree;
	QPlainTextEdit *textEdit;
public slots:	
	void my_slot1(const QModelIndex & index);	
	void my_slot2();	
signals:
  void my_signal1();
};

class SharedDirTab : public QWidget
{
    Q_OBJECT
public:
    SharedDirTab(const QFileInfo &fileInfo, QWidget *parent = 0);

private:
	QDirModel* model;
	QTreeView* tree;
	QPlainTextEdit *textEdit;

public slots:	
	void my_slot1(const QModelIndex & index);	
	void my_slot3();	

signals:
  void my_signal1();
};

class ParametersTab : public QWidget
{
    Q_OBJECT

public:
    ParametersTab(const QFileInfo &fileInfo, QWidget *parent = 0);
	VegaParameters param;
private:
	QDirModel* model;
	QTreeView* tree;
	QPlainTextEdit *textEdit;
	QLineEdit* operateurPassword;
	QLineEdit* administrateurPassword;
	QPushButton *btn;
public slots:	
	void my_slot1();	
signals:
  void my_signal1();
};

//class Ui_MainWindow : QObject
class Ui_MainWindow : public QMainWindow
{
  Q_OBJECT
protected:
	void closeEvent(QCloseEvent* event);
	bool event(QEvent* event);
//	void keyPressedEvent(QKeyEvent* event);
public:
	void setupUi();//QMainWindow *MainWindow);
	Ui_MainWindow(void)	{
		m_authenticationReady = false;
		m_readOnly = false;
		m_controlConnectionLostUserAlerted = false;
		m_nbCurrentAlarms = -1;
	}
	~Ui_MainWindow(){}

	bool m_readOnly;

	QPixmap *imageVegaVert;
	QPixmap *imageVegaLogo;
	QPixmap *imageVegaRouge;
	QPixmap *imageInterphone;
	QLabel *labelImageVega;
	QLabel *labelImageAlarme;
	QLabel *labelImageLogo;
	QLabel *labelImageInterphone;
	QPixmap *imageAlarme;


	//QWidget *centralwidget;
	QTabWidget *tabWidgetGeneral;



	// >>>>>>>>>>>> panneau des terminaux
	QComboBox *comboBoxDeviceName;
	QWidget *pannel_terminaux;
	QTableWidget *tableDevices;

	// >>>>>>>>>>>> panneau des touches des postes
	QWidget *pannel_touches;
	QGroupBox *groupBoxKeyConfiguration;
	QComboBox *comboBoxChooseKeyType;
	QComboBox *comboBoxKeyGroupName;
	QGroupBox *groupBoxMapCRM4;
	QSignalMapper *signalMapperDuplex;
	QSignalMapper *signalMapperContactSec;
	QSignalMapper *signalMapperGains;
	QSignalMapper *signalMapperDeviceKeys;


	// >>>>>>>>> configuration

	//QLabel *labelConferenceConfigurationName;
	QPushButton *pushButtonPrintConfiguration;
	QPushButton *pushButtonExport;
	QPushButton *pushButtonRestartControl; 

	QComboBox *comboBoxCurrentConfiguration;

	QLineEdit *lineEditConfigurationName;
	;
	QComboBox *comboBoxChooseEditConfiguration;
	QPushButton *pushButtonCreateConfiguration;
    QPushButton *pushButtonDeleteConfiguration;


	// >>>  conferences 

    QWidget *pannel_conference;
    
	QCheckBox *checkBoxEnableConference;
	QGroupBox *groupBoxEditionConferences;
	QLineEdit *lineEditConferenceName;

	QPushButton *pushButtonInsertDevice;
	QPushButton *pushButtonDeleteDevice;

	QComboBox *comboBoxConferenceNumberChoose;
	QComboBox *comboBoxConferenceDirector;
	QComboBox *comboBoxRadio2;
	QComboBox *comboBoxRadio1;
	QComboBox *comboBoxJupiter2;


	QListWidget *listDevices;
	QListWidget *listConferenceMembers;

	
	QLabel *labelConferenceDevices;
	QLabel *labelDevices;
	QLabel *labelConferenceDirector;
	QLabel *label_63;
	QLabel *labelConferenceRadio2;
	QLabel *labelConferenceName;
	QLabel *label_65;
	QLabel *labelConferenceNumberChoose;




    // >>>>>>>>>>> pannel groupes
	QWidget *pannel_groupes;
	QLabel *labelGroupName;
	QLineEdit *lineEditGroupName;
	QPushButton *pushButtonDeleteGroup;
	QGroupBox *groupBoxGroupEdition;	
    QPushButton *pushButtonCreateGroup;
    QComboBox *comboBoxChooseEditGroup;
    QLabel *labelEditGroup;
    QListWidget *listWidgetGroupMembers;
    QListWidget *listWidgetGroupDevices;





	/// >>>>>>>> evenements au fil de l'eau
	//LiveEvents *liveEvents;
	QTableWidget *tableLiveEvents;
	//QStandardItemModel *liveEventsModel;

	//  >>>    boutons et heure


	QPushButton* pBtnLiaisonAlphacomControl;
	QPushButton* pBtnLiaisonIHMControl;
	QPushButton* pBtnAlimentationStatus;
	QLCDNumber *lcdDate;
	QPushButton *pushButtonSaveModifications;
	QPushButton * pushBtnApplyCurrentAsRunning;


	void setupUi(QMainWindow *MainWindow);

	void init();			 
	void msgBox(const char*fmt, ...);
	int confirmBox(const char*fmt, ...);

	void setConferenceEditable();
	void setConferenceUneditable();
	void showDeviceKeysMapConfiguration();
	void goOfflineMode();
	void goOnlineMode();
	void updateUIElement(const char*name);
				       
	QStatusBar *statusBar;

	/////////////////////////// PARTIE AUTHENTIFICATION


	void initAuthenticationPassword();

	QLabel *labelAuthenticateResponse;

	QLineEdit *lineEditPasswordManager;
	QLineEdit *lineEditPasswordManagerVerify;
	QLineEdit *lineEditPasswordOperator;
	QLineEdit *lineEditPasswordOperatorVerify;
	
	QLineEdit *lineEditAuthenticatePassword;  
	QComboBox *comboBoxAuthenticateChooseUser;
	
	QPushButton *pushButtonModifyPasswordOperator;
	QPushButton *pushButtonModifyPasswordManager;
	
	bool authenticateWindow();				    
	bool m_authenticationReady;
	void loadParameters();
	void saveParameters();
	
	//QDialog *dialogAuthenticate;
	//VegaParameters m_vegaParameters;

	QPushButton *pushButtonAuthenticateOK;
	QPushButton *pushButtonAuthenticateQuit;
	

	/////////////////////////// pannel SYSTEM STATUS
	QWidget *pannelStatus;

	void initSystemStatus();
	void setElementNewStatus(const char *name, int number, board_state_t newState);
	void setReadOnlyMode(bool);
	void setReadOnlyModeUI();

	//pingThread* mypingthread;
	void initControlConnectionTCP();
	bool m_controlConnectionLostUserAlerted;			      

	int m_nbCurrentAlarms;


public slots:

	void tabDevicesChanged(int tabIndex);
	void tabGeneralChanged(int tabIndex);


	// actions terminaux
	void devicesNameChanged(int, int);
	void devicesGainChanged(int index);
	void devicesDuplexChanged(int index);
	void devicesContactSecChanged(int index);
	

	// actions configurations
	void slot_updateVegaConfigCombo();
	void slot_comboConfiguration(int index);
	void configurationNameChanged();
	void MAJ_comboConfiguration();
	void deleteCurrentConfiguration();
	void creationConfiguration();
	void checkboxActivateConference(int state);
	void configurationChanged(int index);
	void printConfiguration();
	void applyCurrentConfigAsRunningConfig();
	void exportConfigurations();

	
	// actions conferences
	void slot_comboConference(int index);
	void conferenceNameChanged();
	void insertDeviceInConferenceClicked();
	void deleteDeviceInConferenceClicked();
	void conferenceRadio1Changed(int index);
	void conferenceRadio2Changed(int index);
  	void conferenceJupiter2Changed(int index);
	void conferenceDirectorChanged(int index);

	// actions groupes
	void slot_comboGroup(int index);
	void createNewGroup();
	void groupAddMember();
	void groupRemoveMember();
	void groupNameChanged();
	void deleteGroup();


	// actions sur touches des postes CRM4
	void slot_deviceKeysConfigurationDeviceChanged(int);
	void slot_crm4KeyConfigure(int); // on clique sur une touche de 39 a 52
	void deviceKeyConfiguredOK(); // on clique sur OK apres avoir selectionne une touche sur CRM4
	void keyActionChanged(int index);

	
	void updateStatus();
	void controlConnectionRestartSucceed();
	void controlConnectionRestartFailed();
	void controlConnectionLost();
	void controlConnectionEstablished();
			   
	// affichage des alarmes 
	void slot_linkIHMControlDown();
	void slot_linkIHMControlUp();
	void slot_linkControlAlphacomUp();
	void slot_linkControlAlphacomDown();
	void slot_AlimentationUp();
	void slot_AlimentationDown();

	// affichage des resultats des requetes SQL pour journal de bord
	void slot_timerJournaldeBord();

	// affichage de l'heure
	void slot_showTime(); // toutes les secondes
	
	void slot_getRunningConfig();// au demarrage, une seule demande de recuperation au controle
	void saveModifications();
	void restartControl();

	void passwordManagerChanged();
	void passwordOperatorChanged();
	
	void propagateBoardStatus();
  
}; // class Ui_MainWindow

extern controlLink						m_controlLink;
extern LiveEvents						m_liveEvents;

#endif	    /* !VEGA_IHM_HH_ */
