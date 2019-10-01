#include <iostream>
#include "vega_ihm.h"
#include "switch.h"
#include "debug.h"
#include "configuration.h"
#include "printer.h"
#include "liveEvents.h"
#include <iostream>
#include <rude/config.h>
#include <string.h>
extern "C"  {	
#include "md5.h"
}
#include "parameters.h"
#include "debug.h"

static VegaParameters m_vegaParameters;

VegaParameters::VegaParameters()
{
  bzero(m_readPassword, 16);
  bzero(m_readWritePassword, 16);
  m_isValid = false;
}

VegaParameters::~VegaParameters()
{
}
 
void VegaParameters::md5sum(const unsigned char *input, unsigned char *output)
{
  struct cvs_MD5Context context;
  
  cvs_MD5Init (&context);
  cvs_MD5Update (&context, input, (unsigned)(strlen ((char *) input)));
  cvs_MD5Final (output, &context);
}

void VegaParameters::setReadPassword(const char *password)
{
  unsigned char out[16];
  
  md5sum((const unsigned char*)password, out);
  memcpy(m_readPassword, out, 16);
}

void VegaParameters::setReadWritePassword(const  char* password)
{
  unsigned char out[16];
  
  md5sum((const unsigned char *)password, out);
  memcpy(m_readWritePassword, out, 16);
}

bool VegaParameters::comparePasswords(unsigned char *input,  unsigned char *input2)
{
  int i;
  
  for (i = 0; i < 16; i++) {
    if (input[i] != input2[i])
      return false;
  }

  return true;  
}


bool VegaParameters::compareToReadPassword(const char *input)
{
  unsigned char out[16];
  
  md5sum((const unsigned char *)input, out);
  if (m_readPassword[0])
	return comparePasswords(out, m_readPassword);
  else
  {
	  //dbgBox("mot de passe vide");
    return true;
  }
}

bool VegaParameters::compareToReadWritePassword(const  char *input)
{
  unsigned char out[16];
  
  md5sum((const unsigned char *)input, out);
     
  if ( m_readWritePassword[0] ) 
	return comparePasswords(out, m_readWritePassword);
  else
  {
	  //dbgBox("mot de passe vide");
	  return true;
  }
}



bool VegaParameters::saveParametersToFile(const char *fileName)
{
  rude::Config config;
  char md5sum[33];
  int i;

  config.setSection("authentication");
  bzero(md5sum, 33);
  for (i = 0; i < 16; i++) 
    sprintf (md5sum + (i*2), "%02x", ((unsigned int) (m_readPassword[i])) & 0xff);
  config.setStringValue("operator_password", md5sum);
  for (i = 0; i < 16; i++) 
    sprintf (md5sum + (i*2), "%02x", ((unsigned int) (m_readWritePassword[i])) & 0xff);
  config.setStringValue("manager_password", md5sum);
  config.save(fileName);
  
  return true;
}


void VegaParameters::strToMD5Sum(const char *str, unsigned char *md5sum)
{
  int i, k;
  char buf[2];
  int val;
  char hex[] = "0123456789abcdef";
  
  for (i = 0; i < 16; i++) {
    memcpy(buf, str + (i*2), 2);
    val = 0;
    for (k = 0; k < 16; k++)  
      if (hex[k] == buf[0]) 
	val = k << 4;		// multiple of 16
    for (k = 0; k < 16; k++)  
      if (hex[k] == buf[1]) 
	val = val |  k;

    md5sum[i] = (unsigned char)(val & 0xff);
  }
}

bool VegaParameters::isValid()
{
  return m_isValid;
}

bool VegaParameters::loadParametersFromFile(const char*fileName)
{
  m_readWritePassword[0] = m_readPassword[0] = 0;

  bool ret = true;

  rude::Config config;
  if (config.load(fileName) == false) {
    dbgBox("cannot load %s file", fileName);
    return NULL;
  }


  config.setSection("authentication");    
  const char *passwd = config.getStringValue("manager_password");
  //dbgBox("read encrypted password %s in %s", passwd,fileName);
  if (passwd) 
    strToMD5Sum(passwd, m_readWritePassword);
  else 
    ret =  false;


  passwd = config.getStringValue("operator_password");
  if (passwd)
    strToMD5Sum(passwd, m_readPassword);
  else 
    ret = false;

  m_isValid = true;

  return ret;

}

bool KeyPressEater::eventFilter(QObject *obj, QEvent *event)
{
	
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		qDebug("Ate key press %d", keyEvent->key());
		dbgBox("Ate key press %d", keyEvent->key() );
		return true;
	} else {
		dbgBox("standard event processing");
		return QObject::eventFilter(obj, event);
	}
}

void Ui_MainWindow::initAuthenticationPassword()
{
  QWidget *pannelPassword = new QWidget();
  pannelPassword->setObjectName(QString::fromUtf8("pannelPassword"));    
  tabWidgetGeneral->addTab(pannelPassword, QString());

  tabWidgetGeneral->setTabText(tabWidgetGeneral->indexOf(pannelPassword), QString::fromUtf8("Parametres"));

  QLabel *labelPasswordOperator = new QLabel(pannelPassword);
  labelPasswordOperator->setObjectName(QString::fromUtf8("labelPasswordOperator"));
  labelPasswordOperator->setGeometry(QRect(20, 40, 160, 40));
  labelPasswordOperator->setText("Mot de passe operateur :");

  lineEditPasswordOperator = new QLineEdit(pannelPassword);
  lineEditPasswordOperator->setObjectName(QString::fromUtf8("lineEditPasswordOperator"));
  lineEditPasswordOperator->setGeometry(QRect(170, 50, 130, 20));

  QLabel *labelPasswordOperatorVerify = new QLabel(pannelPassword);
  labelPasswordOperatorVerify->setObjectName(QString::fromUtf8("labelPasswordOperator"));
  labelPasswordOperatorVerify->setGeometry(QRect(310, 40, 80, 40));
  labelPasswordOperatorVerify->setText("Confirmation :");

  lineEditPasswordOperatorVerify = new QLineEdit(pannelPassword);
  lineEditPasswordOperatorVerify->setObjectName(QString::fromUtf8("lineEditPasswordOperator"));
  lineEditPasswordOperatorVerify->setGeometry(QRect(400, 50, 130, 20));



  QLabel *labelPasswordManager = new QLabel(pannelPassword);
  labelPasswordManager->setObjectName(QString::fromUtf8("labelPasswordManager"));
  labelPasswordManager->setGeometry(QRect(20, 90, 160, 40));
  labelPasswordManager->setText("Mot de passe manager :");

  QLabel *labelPasswordManagerVerify = new QLabel(pannelPassword);
  labelPasswordManagerVerify->setObjectName(QString::fromUtf8("labelPasswordManager"));
  labelPasswordManagerVerify->setGeometry(QRect(310, 90, 160, 40));
  labelPasswordManagerVerify->setText("Confirmation :");

  lineEditPasswordManager = new QLineEdit(pannelPassword);
  lineEditPasswordManager->setObjectName(QString::fromUtf8("lineEditPasswordManager"));
  lineEditPasswordManager->setGeometry(QRect(170, 100, 130, 20));

  lineEditPasswordManagerVerify = new QLineEdit(pannelPassword);
  lineEditPasswordManagerVerify->setObjectName(QString::fromUtf8("lineEditPasswordManager"));
  lineEditPasswordManagerVerify->setGeometry(QRect(400, 100, 130, 20));

  pushButtonModifyPasswordOperator = new QPushButton(pannelPassword);
  pushButtonModifyPasswordOperator->setGeometry((QRect(620, 50, 80, 20)));
  pushButtonModifyPasswordOperator->setText("Modifier");
 
  pushButtonModifyPasswordManager = new QPushButton(pannelPassword);
  pushButtonModifyPasswordManager->setGeometry((QRect(620, 100, 80, 20)));
  pushButtonModifyPasswordManager->setText("Modifier");

  connect(pushButtonModifyPasswordOperator, SIGNAL(clicked()), this, SLOT(passwordOperatorChanged()));
  connect(pushButtonModifyPasswordManager, SIGNAL(clicked()), this, SLOT(passwordManagerChanged()));

  lineEditPasswordManager->setEchoMode(QLineEdit::Password);
  lineEditPasswordOperator->setEchoMode(QLineEdit::Password);

  lineEditPasswordManagerVerify->setEchoMode(QLineEdit::Password);
  lineEditPasswordOperatorVerify->setEchoMode(QLineEdit::Password);
 
#if 0
  /////////////// boite de dialogue d'authentication
   
  dialogAuthenticate = new QDialog();
  dialogAuthenticate->setFixedSize(400, 120);
  dialogAuthenticate->setModal(true);
  dialogAuthenticate->setWindowTitle("VEGA Authentification");
  dialogAuthenticate->move(480, 410);
    Qt::WindowFlags flags = 0;
        flags = Qt::Dialog;
        //flags |= Qt::MSWindowsFixedSizeDialogHint;
        //flags |= Qt::MSWindowsFixedSizeDialogHint;
        //flags |= Qt::X11BypassWindowManagerHint;
        flags |= Qt::FramelessWindowHint;
        //flags &= ~Qt::WindowTitleHint;
		//flags &= ~Qt::WindowSystemMenuHint;
        //flags &= ~Qt::WindowMinimizeButtonHint;
        //flags |= Qt::WindowMaximizeButtonHint;
        //flags |= Qt::WindowContextHelpButtonHint;
        //flags |= Qt::WindowShadeButtonHint;
        //flags |= Qt::WindowStaysOnTopHint;
  dialogAuthenticate->setWindowFlags(flags);



  QLabel *labelAuthenticateUser = new QLabel(dialogAuthenticate);
  labelAuthenticateUser->setGeometry(QRect(100, 10, 100, 20));
  labelAuthenticateUser->setText("Utilisateur");


    QLabel *labelAuthenticateUse ;

  QLabel *labelAuthenticatePassword = new QLabel(dialogAuthenticate);
  labelAuthenticatePassword->setGeometry(QRect(100, 40, 100, 20));
  labelAuthenticatePassword->setText("Mot de passe");

  lineEditAuthenticatePassword = new QLineEdit(dialogAuthenticate);

  lineEditAuthenticatePassword->setGeometry(QRect(200, 40, 100, 20));
  lineEditAuthenticatePassword->setEchoMode(QLineEdit::Password);

  comboBoxAuthenticateChooseUser = new QComboBox(dialogAuthenticate);
  comboBoxAuthenticateChooseUser->insertItem(0,"manager");
  comboBoxAuthenticateChooseUser->insertItem(1,"operateur");
  comboBoxAuthenticateChooseUser->setGeometry(QRect(200, 10, 100, 20));
  comboBoxAuthenticateChooseUser->setCurrentIndex(0);
  
  labelAuthenticateResponse = new QLabel(dialogAuthenticate);
  labelAuthenticateResponse->setGeometry(QRect(100, 65, 200, 15));

  pushButtonAuthenticateOK = new QPushButton(dialogAuthenticate);
  pushButtonAuthenticateOK->setText("OK");
  pushButtonAuthenticateOK->setGeometry(QRect(135, 85, 60, 20));

  pushButtonAuthenticateQuit = new QPushButton(dialogAuthenticate);
  pushButtonAuthenticateQuit->setText("Quitter");
  pushButtonAuthenticateQuit->setGeometry(QRect(205, 85, 60, 20));

#endif

}

void Ui_MainWindow::passwordManagerChanged()
{
  if (m_vegaParameters.isValid() == false) 
    msgBox("Impossible de modifier les mots de passe !");
  

  if (lineEditPasswordManager->text() != lineEditPasswordManagerVerify->text()) {
    msgBox("Le mot de passe de verification est erroné !");
    return;
  }

  if (lineEditPasswordManager->text().length() < 6) {
    msgBox("Le mot de passe doit etre d'au moins 6 caractères !");
    return;
  }
  

  m_vegaParameters.setReadWritePassword(lineEditPasswordManager->text().latin1());
  m_vegaParameters.saveParametersToFile(SETTINGS_FILE);
  lineEditPasswordManager->setText("");
  lineEditPasswordManagerVerify->setText("");
  msgBox("Mot de passe modifié !");

  return;
}

void Ui_MainWindow::passwordOperatorChanged()
{
  if (lineEditPasswordOperator->text() != lineEditPasswordOperatorVerify->text()) {
    msgBox("Le mot de passe de verification est erroné !");
    return;
  }

  if (lineEditPasswordOperator->text().length() < 6) {
    msgBox("Le mot de passe doit etre d'au moins 6 caractères !");
    return;
  }

  m_vegaParameters.setReadPassword(lineEditPasswordOperator->text().latin1());
  m_vegaParameters.saveParametersToFile(SETTINGS_FILE);
  msgBox("Mot de passe modifié !");
  lineEditPasswordOperator->setText("");
  lineEditPasswordOperatorVerify->setText("");
  return;
}

bool Calculator::authenticateUser()
{
	//dbgBox("Calculator::authenticateUser");
  bool ret;  
  int index = comboUser->currentIndex();
  QString pass = lineEditAuthenticatePassword->text();

  //if (m_vegaParameters.isValid() == false)    return false;
    
  if (index == 0) 
    ret = m_vegaParameters.compareToReadPassword(pass.latin1());
  else 
    ret = m_vegaParameters.compareToReadWritePassword(pass.latin1());
  
  if (ret == true ) 
  {
	  //dbgBox("ret == true");
    if (index == 0) {
		m_readOnly = true;
    }else
		m_readOnly = false;

    lablResponse->setText("<html><body><font color=red>mot de passe correct!</font></html>");
	m_passwordIsCorrect = true;
    done(QDialog::Accepted);
    return true;
  }
  else {
	
    lablResponse->setText("<html><body><font color=red>Mauvais mot de passe !</font></html>");
    lineEditAuthenticatePassword->setText("");
    return false;
  }
  return false;
}

Calculator::Calculator(QWidget *parent)
: QDialog(parent)
{
	m_readOnly = true;

	QGridLayout* grid = new QGridLayout;

	setWindowTitle("Identification VEGA");

	grid->addWidget(new QLabel("Utilisateur"),0,0);
	grid->addWidget(new QLabel("Mot de passe"),1,0);

	lineEditAuthenticatePassword = new QLineEdit();
	lineEditAuthenticatePassword->setEchoMode(QLineEdit::Password);
	grid->addWidget(lineEditAuthenticatePassword,1,1);


	comboUser = new QComboBox();
	comboUser->addItem("operateur");
	comboUser->addItem("manager");
	comboUser->setCurrentIndex(0);
	grid->addWidget(comboUser,0,1);


	QPushButton* btnOK = new QPushButton();
	btnOK->setText("OK");
	btnOK->setDefault(true);
	grid->addWidget(btnOK,0,2);

	QPushButton* btnQuit = new QPushButton();
	btnQuit->setText("Quitter");
	grid->addWidget(btnQuit,1,2);

	
	
	lablResponse = new QLabel("entrez le mot de passe...");
	grid->addWidget(lablResponse,3,1);

	
	
	QTextEdit* txt = new QTextEdit("<h1>Bienvenue</h1> dans le logiciel Interface Homme Machine <font color=blue>VEGA</font><br>"
		"Le systeme de conference <font color=green>ALPHACOM</font><br>\n"
		"<font color=red>Copyrigth Zenitel Stentophon 2009</red>");
	txt->setReadOnly(true);
	grid->addWidget(txt,5,0,3,2);

	QDate dnow = QDate::currentDate();
	if ( dnow.year() >= 2011 ) {
		QPixmap* pixDS = new QPixmap(PIXMAP_DIR"/ds.xpm");
		QLabel* lblDS = new QLabel();
		lblDS->setPixmap(*pixDS);
		grid->addWidget(lblDS,8,0,3,3);
	}

	connect(btnOK, SIGNAL(clicked()), this, SLOT(authenticateUser()));
	connect(btnQuit, SIGNAL(clicked()), this, SLOT(close()));

	setLayout(grid);

	m_vegaParameters.loadParametersFromFile(SETTINGS_FILE);

	m_passwordIsCorrect = false;

	setWindowIcon(QIcon("/home/operateur/vega/imgs/V.png"));
}

void Calculator::closeEvent(QCloseEvent* event)
{
	//dbgBox("Calculator::closeEvent\n");
	if ( true == m_passwordIsCorrect)	
	    done(QDialog::Accepted);
	else
	    done(QDialog::Rejected);
	//exit(0);
}

bool  Calculator::event(QEvent* event)
{
	fprintf(stderr,"Calculator::event type: %d\n",event->type());
	if ( QEvent::KeyPress == event->type() ){
		QKeyEvent* key = static_cast<QKeyEvent*>(event);
		if ( Qt::Key_Escape == key->key() ){
			//dbgBox("Calculator Qt::Key_Escape");
			done(QDialog::Rejected);
			return false;
		}
	}
	QWidget::event(event);
}
/*
bool Calculator::eventFilter(QObject *target, QEvent *event)
{
	qDebug("Ate key press %d", event->type() );
	dbgBox("Ate key press %d", event->type() );
	
        if (target == lineEditAuthenticatePassword)
	{
            if (event->type() == QEvent::MouseButtonPress
                    || event->type() == QEvent::MouseButtonDblClick
                    || event->type() == QEvent::MouseButtonRelease
                    || event->type() == QEvent::ContextMenu) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
                if (mouseEvent->buttons() & Qt::LeftButton) {
                    QPalette newPalette = palette();
                    newPalette.setColor(QPalette::Base,
                                        display->palette().color(QPalette::Text));
                    newPalette.setColor(QPalette::Text,
                                        display->palette().color(QPalette::Base));
                    display->setPalette(newPalette);
                } else {
                    display->setPalette(palette());
                }
                return true;
            }
        }
        return QDialog::eventFilter(target, event);
}*/



bool Ui_MainWindow::authenticateWindow()
{
	//connect(pushButtonAuthenticateQuit, SIGNAL(clicked()), this, SLOT(appQuit()));
	//connect(pushButtonAuthenticateOK, SIGNAL(clicked()), this, SLOT(authenticateUser()));
		
	//this->dialogAuthenticate->exec();


  return true;
}


