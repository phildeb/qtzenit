#include <QtCore/QVariant>
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>
#include <QSqlError>
#include <QTextBrowser>
#include <QTextTableFormat>
#include <QVector>
#include <QTextTable>
#include <QTextTableCell>
#include <QPrintDialog>

#include "vega_ihm.h"
#include "switch.h"
#include "debug.h"
#include "configuration.h"
#include "printer.h"
#include "liveEvents.h"
#include "pannels.h"

static QMap<int,QString> event_map;
static QMap<int,QString> alarmes_map;

void SharedDirTab::my_slot3()
{
	// pdl 20091019 recuperer par TCP la configuration en cours de fonctionnement sur le pc de controle:
	/*if ( !m_controlLink.isConnected() ) {
		msgBox("Deconnecte du PC de controle. Impossible de recuperer la config en cours! ");
	}else{
		msgBox("Connecte au PC de controle: recuperation de la config en cours! ");
		m_controlLink.getFichierConfigTCP();
	}*/
	model->refresh(model->index(CONFIGURATION_DIR));
	//tree->setRootIndex(model->index(CONFIGURATION_DIR) );
	return ;
}

void SharedDirTab::my_slot1(const QModelIndex & index)
{
	/*QMessageBox::information(NULL, 
		model->fileName(index).toStdString().c_str(), 
		model->filePath(index).toStdString().c_str() ) ;*/

     QFile file(model->filePath(index).toStdString().c_str());
     /*if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         return;
     while (!file.atEnd()) {
         QByteArray line = file.readLine();
         QMessageBox::information(this,"vega",line);
     }*/

     if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         return;

     QTextStream in(&file);
     QApplication::setOverrideCursor(Qt::WaitCursor);
     textEdit->setPlainText(in.readAll());
     QApplication::restoreOverrideCursor();


}

SharedDirTab::SharedDirTab(const QFileInfo &fileInfo, QWidget *parent)
    : QWidget(parent)
{
    //QHBoxLayout *mainLayout = new QHBoxLayout;
    QGridLayout *mainLayout = new QGridLayout();
    //mainLayout->addWidget(permissionsGroup);
    //mainLayout->addWidget(ownerGroup);

	QStringList filters;
	filters.append(QString("*.conf"));
	//QDirModel::QDirModel ( const QStringList & nameFilters, QDir::Filters filters, QDir::SortFlags sort, QObject * parent = 0 )
	model = new QDirModel(filters,
		/*QDir::AllDirs|*/QDir::Files|QDir::AllEntries|QDir::Readable|QDir::NoDotAndDotDot,
		//QDir::DirsFirst,
		QDir::Time,
		this);

	tree = new QTreeView;
	tree->setModel(model);
	tree->setWindowTitle(QObject::tr("Dir View"));
	//tree->setRootIndex(model->index(QDir::currentPath()));
	tree->setRootIndex(model->index(CONFIGURATION_DIR) );
	//tree->setAnimated(false);
	//tree->setIndentation(20);
	//tree->setSortingEnabled(true);
	tree->resizeColumnToContents(0);
	
	/*QDir dir(CONFIGURATION_DIR);
	dir.setNameFilters(filters);
	QStringList list(dir.entryList());*/
	//model->setReadOnly(true);
    //model->setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    //model->setSorting(QDir::Size | QDir::Reversed);
	//tree->resize(640, 480);

    textEdit = new QPlainTextEdit;
	textEdit->setReadOnly(true);

	QPushButton* btn = new QPushButton("Recuperer la configuration du controle");
	QPushButton* btn2 = new QPushButton("Rafraichir");

	mainLayout->addWidget(tree,0,0,1,2);	
	mainLayout->addWidget(textEdit,1,0,1,2);
	mainLayout->addWidget(btn,2,0);
	mainLayout->addWidget(btn2,2,1);
	
    //mainLayout->addStretch(1);

	setLayout(mainLayout);

	//color = #AED0F2
	//setPalette(QPalette(QColor(0xAE, 0xD0, 0xF2)));
	setPalette(QPalette(Qt::green));
	setPalette(QPalette(COLOR_GREEN));
	setAutoFillBackground(true);

	QObject::connect(tree,SIGNAL(clicked(const QModelIndex & )),this, SLOT(my_slot1(const QModelIndex & )));
	//QObject::connect(btn,SIGNAL(clicked()),this, SLOT(my_slot3()));
	QObject::connect(btn2,SIGNAL(clicked()),this, SLOT(my_slot3()));
}	

void ConfDirTab::my_slot2()
{
	//QMessageBox::information(NULL,"hello","slot2");
	//tree->setModel(model);
	model->refresh();
}

void ConfDirTab::my_slot1(const QModelIndex & index)
{//
#if 0
	const QPixmap pm(PIXMAP_DIR"/V.xpm");
	QMessageBox msgBox;
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setIconPixmap ( pm );
	msgBox.setText (QString("Supprimer ")+model->fileName(index).toStdString().c_str()+QString(" ?"));
	msgBox.setDefaultButton(QMessageBox::No);
	//msgBox.setInformativeText (model->filePath(index).toStdString().c_str());

	switch (msgBox.exec()) {
	case QMessageBox::Yes :
		{
			bool ret = false;
			//(model->filePath(index));
			//ret = dir.rmpath(model->filePath(index));
			if ( model->isDir(index) ) 
			{
				QDir dir = QDir(model->filePath(index));
				// s'il y a d'autres fichiers:
				QStringList list(dir.entryList());
				QString fic;
				foreach( fic, list ) {
					//QMessageBox::information( this,"vega",fic);
					dir.remove(fic);
				}
				/*QStringListIterator it=list.begin();
				while ( it != list.end() ){
					//ret = dir.remove(model->filePath(index));
					QMessageBox::information( this,model->filePath(index).toStdString().c_str(),(ret==true)? "OK": "NOK" );

				}*/
				
				ret = dir.rmdir(model->filePath(index));
				QMessageBox::information( this,model->filePath(index).toStdString().c_str(),(ret==true)? "suppression OK": "suppression NOK" );
				
				//ret = dir.rmpath(model->filePath(index));
				//QMessageBox::information( this,model->filePath(index).toStdString().c_str(),(ret==true)? "OK": "NOK" );
			}else{
				QDir dir = QDir::tempPath();
				ret = dir.remove(model->filePath(index));
				QMessageBox::information( this,model->filePath(index).toStdString().c_str(),(ret==true)? "suppression OK": "suppression NOK" );
			}

			model->refresh();
		}
		break;
	case QMessageBox::No :
		// no was clicked
		break;
	default:
		// should never be reached
		break;
	}
#endif

	/*int ret = QMessageBox::question(NULL, 
		model->fileName(index).toStdString().c_str(), 
		model->filePath(index).toStdString().c_str(),
		QMessageBox::Yes | QMessageBox::No );// | QMessageBox::Cancel, QMessageBox::Save );//| QMessageBox::Question);
	if ( QMessageBox::Yes == ret ) */


	{
     QFile file(model->filePath(index).toStdString().c_str());
     /*if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         return;
     while (!file.atEnd()) {
         QByteArray line = file.readLine();
         QMessageBox::information(this,"vega",line);
     }*/

     if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         return;

     QTextStream in(&file);
     QApplication::setOverrideCursor(Qt::WaitCursor);
     textEdit->setPlainText(in.readAll());
     QApplication::restoreOverrideCursor();
	}
}


void ParametersTab::my_slot1()
{
	QMessageBox::information(NULL,"vega",administrateurPassword->text());
}


ParametersTab::ParametersTab(const QFileInfo &fileInfo, QWidget *parent)
    : QWidget(parent)
{
    QGroupBox *ownerGroup = new QGroupBox(tr("Mot de passe operateur"));
    QLabel *ownerLabel = new QLabel(tr("mot de passe"));

	administrateurPassword = new QLineEdit(this);
	administrateurPassword->setEchoMode(QLineEdit::Password);

    //QLabel *ownerValueLabel = new QLabel(fileInfo.owner());
    //ownerValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    QLabel *groupLabel = new QLabel(tr("confirmer"));
    //QLabel *groupValueLabel = new QLabel(fileInfo.group());
    //groupValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	operateurPassword = new QLineEdit(this);
	operateurPassword->setEchoMode(QLineEdit::Password);
    
	btn = new QPushButton("Modifier");	

	QLabel* lbl = new QLabel(); lbl->setPixmap(QPixmap(PIXMAP_DIR"/V.xpm") ) ;
	QLabel* lbl2 = new QLabel(); lbl2->setPixmap(QPixmap(PIXMAP_DIR"/crm4.xpm") ) ;
	//QLabel* lbl3 = new QLabel(); lbl3->setPixmap(QPixmap("PIXMAP_DIR/ip-desktop-stations.xpm") ) ; //STENTOFON-on-a-page.xpm
	QLabel* lbl3 = new QLabel(); lbl3->setPixmap(QPixmap(PIXMAP_DIR"/STENTOFON-on-a-page.xpm") ) ; //



	QGridLayout *ownerLayout = new QGridLayout;
	ownerLayout->addWidget(ownerLabel,0,0);
    ownerLayout->addWidget(operateurPassword,0,1);
    ownerLayout->addWidget(groupLabel,0,2);
    ownerLayout->addWidget(administrateurPassword,0,3);
    ownerLayout->addWidget(btn,0,4);
    //ownerLayout->addWidget(lbl,1,0);
    //ownerLayout->addWidget(lbl2,2,0);
    ownerLayout->addWidget(lbl3,0,5);
    ownerGroup->setLayout(ownerLayout);
	
	QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(ownerGroup);
    mainLayout->addStretch(1);

    setLayout(mainLayout);

	connect( btn,SIGNAL(clicked()),this, SLOT(my_slot1() ) );

}

ConfDirTab::ConfDirTab(const QFileInfo &fileInfo, QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
	QPushButton* refresh_btn=new QPushButton("rafraichir");
    //mainLayout->addWidget(refresh_btn);
    
	QStringList filters;
	filters.append(QString("*.conf"));
	//QDirModel::QDirModel ( const QStringList & nameFilters, QDir::Filters filters, QDir::SortFlags sort, QObject * parent = 0 )
	model = new QDirModel(filters,
		QDir::AllDirs|QDir::Files|QDir::AllEntries|QDir::Readable|QDir::NoDotAndDotDot,
		QDir::DirsFirst,//QDir::Time,
		this);


	tree = new QTreeView;
	tree->setModel(model);
	tree->setWindowTitle(QObject::tr("Dir View"));
	//tree->setRootIndex(model->index(QDir::currentPath()));
	tree->setRootIndex(model->index(CONFIGURATION_DIR) );
	//tree->setRootIndex(model->index( QDir::tempPath() ) );
	//tree->setAnimated(false);
	//tree->setIndentation(20);
	//tree->setSortingEnabled(true);
	tree->resizeColumnToContents(0);
	tree->setAlternatingRowColors(true);	

	/*QDir dir(CONFIGURATION_DIR);
	dir.setNameFilters(filters);
	QStringList list(dir.entryList());*/
	//model->setReadOnly(true);
    //model->setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    //model->setSorting(QDir::Size | QDir::Reversed);
	//tree->resize(640, 480);
	mainLayout->addWidget(tree);	

    textEdit = new QPlainTextEdit;
	textEdit->setReadOnly(true);

	mainLayout->addWidget(textEdit);	
	setLayout(mainLayout);

	QObject::connect(tree,SIGNAL(clicked(const QModelIndex & )),this, SLOT(my_slot1(const QModelIndex & )));
	QObject::connect(refresh_btn,SIGNAL(clicked()),this, SLOT(my_slot2()));
}	

#if 0

#include "/usr/include/mysql/mysql.h"

static MYSQL*				mysql					= 0;
static string				server					= HOST_CONTROL_IP;//"169.254.1.2";
static string				database				= "zenitel";
static string				user					= "zenitel";
static string				password				= "zenitel";
bool mysql_init_done=false;
void init_mysql()
{
	fprintf(stderr,"init_mysql...");
	//sleep(2);
	/*QMessageBox::information(NULL, "mysql connect", server.c_str()) ;
	QMessageBox::information(NULL, "mysql connect", database.c_str()) ;
	QMessageBox::information(NULL, "mysql connect", user.c_str()) ;
	QMessageBox::information(NULL, "mysql connect", password.c_str()) ;*/

	if ( true==mysql_init_done ) return;
	
	if (!(mysql = mysql_init((MYSQL*) 0))){
		fprintf(stderr,"mysql_init %s",mysql_error (mysql));
		mysql=0;
		return;
	}else{
		fprintf(stderr,"ok mysql_init \n"); // apparait  bien dans la consle linux
	}
	
	if (mysql_real_connect( mysql, server.c_str(), user.c_str(), password.c_str(), NULL, MYSQL_PORT, NULL, 0 ))
	{
		if ( mysql_select_db( mysql, database.c_str() ) >= 0 ) 
		{
			QString str;
			str.sprintf("%s Ouverture de la base OK\n",database.c_str()) ;
			//QMessageBox::information(NULL, "mysql connect", str) ;
			mysql_init_done = true;
	
			string reqsql("select id,name from event_type;");
			if ( !mysql_query( mysql, reqsql.c_str()) ) // execution de la requete proprement dite!
			{
				MYSQL_RES *result = mysql_store_result( mysql );
				if ( result != NULL )
				while(1)
				{
					MYSQL_ROW sqlrow = mysql_fetch_row (result);	// essayer de lire le premier enregistrement
					if (0 != sqlrow){
						int id = atoi(sqlrow[0]);
						QString str = sqlrow[1];
						event_map[id]=str;
						//QMessageBox::information(NULL, "mysql connect", qPrintable(event_map[id] ) ) ;
						//s.sprintf ("%s %s %s evt_id:%d count:%d", sqlrow[0], sqlrow[1] );		
					}else
						break;
				}		
			}
			reqsql=("select id,name from alarm_type;");
			if ( !mysql_query( mysql, reqsql.c_str()) ) // execution de la requete proprement dite!
			{
				MYSQL_RES *result = mysql_store_result( mysql );
				if ( result != NULL )
				while(1)
				{
					MYSQL_ROW sqlrow = mysql_fetch_row (result);	// essayer de lire le premier enregistrement
					if (0 != sqlrow){
						int id = atoi(sqlrow[0]);
						QString str = sqlrow[1];
						alarmes_map[id]=str;
						//QMessageBox::information(NULL, "mysql connect", qPrintable(event_map[id] ) ) ;
						//s.sprintf ("%s %s %s evt_id:%d count:%d", sqlrow[0], sqlrow[1] );		
					}else
						break;
				}		
			}
		}else{
			fprintf(stderr,"mysql erreur: %s\n",mysql_error (mysql));
			QMessageBox::critical(NULL, "mysql connect",mysql_error (mysql));
		}
	}else{
		fprintf(stderr,"mysql erreur: %s\n",mysql_error (mysql));
		QMessageBox::critical(NULL, "mysql connect",mysql_error (mysql));
	}
}
#endif

FenSQL::FenSQL(QWidget* parent)
{		
	//init_mysql();

	combo_conferences = new QComboBox ;
	combo_alarmes = new QComboBox ;
	combo_evenements = new QComboBox ;
	combo_poste  = new QComboBox ;
	combo_conferences->addItem("<toutes>");
	combo_poste->addItem("<tous>", -1);
	lastConfig = NULL;
	combo_conferences->addItem("1");
	combo_conferences->addItem("2");
	combo_conferences->addItem("3");
	combo_conferences->addItem("4");
	combo_conferences->addItem("5");
	combo_conferences->addItem("6");
	combo_conferences->addItem("7");
	combo_conferences->addItem("8");
	//combo_conferences->addItem("9");

	//QMap<int,QString> event_map;
	combo_evenements->addItem("<toutes>");
	QMap<int,QString>::const_iterator i = event_map.constBegin();
	while (i != event_map.constEnd()) {
		//cout << i.key() << ": " << i.value() << endl;
		QString str(i.value());
		combo_evenements->addItem(qPrintable(str) ,i.key());
		++i;
	}

	combo_alarmes->addItem("<toutes>");
	i = alarmes_map.constBegin();
	while (i != alarmes_map.constEnd()) {
		//cout << i.key() << ": " << i.value() << endl;
		QString str(i.value());
		combo_alarmes->addItem(qPrintable(str) ,i.key());
		++i;
	}	
	

	//QDate adate(2009,8,31);
		
	cal_deb = new QCalendarWidget;
	cal_deb->setGridVisible(true);
	cal_deb->setFixedHeight(150);
	
	cal_fin = new QCalendarWidget;
	cal_fin->setGridVisible(true);
	cal_fin->setFixedHeight(150);
		
	/*dateEditDeb = new QDateTimeEdit(QDate::currentDate());
	dateEditDeb->setMinimumDate(QDate::currentDate().addDays(-365));
	dateEditDeb->setMaximumDate(QDate::currentDate().addDays(365));
	dateEditDeb->setDisplayFormat("yyyy.MM.dd");

	//lbl_alphacom_link = new QLabel("alphacom link");

	dateEditFin = new QDateTimeEdit(QDate::currentDate());
	dateEditFin->setMinimumDate(QDate::currentDate().addDays(-365));
	dateEditFin->setMaximumDate(QDate::currentDate().addDays(365));
	dateEditFin->setDisplayFormat("yyyy.MM.dd");*/

	QPushButton* btn_print=new QPushButton("Imprimer");

	btn_evenements=new QPushButton("RECHERCHE EVENEMENT");
	btn_alarmes=new QPushButton("RECHERCHE ALARME");
	//btn_alarmes->setStyleSheet("QPushButton { background-color: " COLOR_RED " }");
		
	m_list=new QListWidget;
	//m_list->setFont( QFont("Times", 12 , QFont::Bold ) );

	m_table=new QTableWidget();
	//m_table->setFont( QFont("Times", 12 , QFont::Bold ) );
	m_table->setAlternatingRowColors(true);
	//m_table->setFixedHeight(350);
	m_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	//m_table->set
	m_table->setSelectionMode(QAbstractItemView::NoSelection);
	m_table->setEditTriggers( QAbstractItemView::NoEditTriggers );


#if 1	
	QGridLayout* gr= new QGridLayout;
	gr->addWidget(cal_deb,0,0,1,2);
	gr->addWidget(cal_fin,0,2,1,2);
	//gr->addWidget(btn_print,0,4);
	//gr->addWidget(btn_evenements,1,4,1,1);
	//gr->addWidget(btn_alarmes,2,4,1,1);
	{
		QGroupBox *gbox = new QGroupBox(tr("action:"));
		QVBoxLayout *vbox = new QVBoxLayout;
		vbox->addWidget(btn_evenements);
		vbox->addWidget(btn_alarmes);
		vbox->addWidget(btn_print);
		gbox->setLayout(vbox);
		gr->addWidget(gbox,0,4,1,1);
	}
	{
		QGroupBox *gbox = new QGroupBox(tr("Numero de conference:"));
		QVBoxLayout *vbox = new QVBoxLayout;
		vbox->addWidget(combo_conferences);
		gbox->setLayout(vbox);
		gr->addWidget(gbox,1,4);
	}
	{
		QGroupBox *gbox = new QGroupBox(tr("Alarme:"));
		QVBoxLayout *vbox = new QVBoxLayout;
		vbox->addWidget(combo_alarmes);
		gbox->setLayout(vbox);
		gr->addWidget(gbox,2,4);
	}
	{
		QGroupBox *gbox = new QGroupBox(tr("Evenement:"));
		QVBoxLayout *vbox = new QVBoxLayout;
		vbox->addWidget(combo_evenements);
		gbox->setLayout(vbox);
		gr->addWidget(gbox,3,4);
	}
	{
		QGroupBox *gbox = new QGroupBox(tr("Poste:"));
		QVBoxLayout *vbox = new QVBoxLayout;
		vbox->addWidget(combo_poste);
		gbox->setLayout(vbox);
		gr->addWidget(gbox,4,4);
	}

	gr->addWidget(m_table,1,0,4,4);
	setLayout(gr);
#else
#endif	

	connect(btn_evenements,SIGNAL(clicked()),this,SLOT(my_slot_recherche_evenement()) );	
	connect(btn_alarmes,SIGNAL(clicked()),this,SLOT(my_slot_recherche_alarme()) );	

	connect(cal_deb,SIGNAL(selectionChanged()),this,SLOT(my_slot_calendar()) );
	connect(cal_fin,SIGNAL(selectionChanged()),this,SLOT(my_slot_calendar()) );

	connect(combo_conferences,SIGNAL(activated(int)),this,SLOT(my_slot_calendar()) );
	connect(combo_evenements,SIGNAL(activated(int)),this,SLOT(my_slot_calendar()) );
	connect(combo_alarmes,SIGNAL(activated(int)),this,SLOT(my_slot_calendar()) );
	connect(combo_poste,SIGNAL(activated(int)),this,SLOT(my_slot_calendar()) );

	connect(combo_alarmes,SIGNAL(highlighted (int)),this,SLOT(my_slot_remplir(int)) );
	connect(combo_evenements,SIGNAL(highlighted (int)),this,SLOT(my_slot_remplir(int)) );
	connect(combo_poste,SIGNAL(highlighted (int)),this,SLOT(my_slot_remplir_postes(int)) );


	connect(btn_print,SIGNAL(clicked ( )),this,SLOT(my_slot_imprimer()) );

	my_slot_calendar();
}


void FenSQL::my_slot_remplir_postes(int i)
{

  /* msgBox("lastConfig = %p pCurrent == %p", lastConfig,VegaConfig::pCurrent);*/
  if (lastConfig != VegaConfig::pCurrent && VegaConfig::pCurrent != NULL ) {
 
    combo_poste->clear();

    combo_poste->addItem("<Tous>", -1);

    if (VegaConfig::pCurrent == NULL) 
      return;
  
    foreach (Device *d, VegaConfig::pCurrent->m_devices) {
      combo_poste->addItem(d->getName(), d->getNumber());
    }
    lastConfig = VegaConfig::pCurrent;

  }

}

void FenSQL::my_slot_remplir(int i)
{	
	if ( combo_alarmes->count() <= 1 ) {
		QSqlDatabase mysql_db;
		if (mysql_db.isValid() == false) {
			mysql_db = QSqlDatabase::addDatabase("QMYSQL", "searchdb");
			mysql_db.setHostName( HOST_CONTROL_IP	);
			//msgBox("ip address database :%s",HOST_CONTROL_IP);
			mysql_db.setDatabaseName("zenitel");
			mysql_db.setUserName("zenitel");
			mysql_db.setPassword("zenitel");
		}else{
			msgBox("invalide database :%s",HOST_CONTROL_IP);
		}
		bool ok = mysql_db.open();
		QSqlQuery query(mysql_db);
		QString req("select id,name from alarm_type;");
		if (query.exec(req) == false)
		{
		  qDebug() << query.lastError();
		  return;
		}
		query.first();
		do
		{
			combo_alarmes->addItem(qPrintable(query.value(1).toString()) ,query.value(0).toInt());
			//for (int col=0;col<rec.count();col++)
		}while(query.next());
		
		req=("select id,name from event_type;");
		if (query.exec(req) == false)
		{
		  qDebug() << query.lastError();
		  return;
		}
		query.first();
		do
		{
			combo_evenements->addItem(qPrintable(query.value(1).toString()) ,query.value(0).toInt());
			//for (int col=0;col<rec.count();col++)
		}while(query.next());

		mysql_db.close();
		//QMessageBox::information(this,"my_slot_remplir",QString::number(combo_evenements->count()) );
	}
}

void FenSQL::my_slot_calendar()
{	
	QDate d = cal_deb->selectedDate();// () const
	QDate d_fin = cal_fin->selectedDate();// () const
	
	//QMessageBox::information(this,"my_slot_calendar",d.toString());
	//QMessageBox::information(this,"my_slot_calendar",d_fin.toString());
	//lineEdit->setText(d.toString());
	
	time_t now = time(NULL);	
	//struct tm *tm_now = localtime(&now);
	
	struct tm *debut = localtime(&now);
	debut->tm_hour = 0;//d.hour();
	debut->tm_min = 0;//d.minute();
	debut->tm_sec = 0;//d.hour();
	debut->tm_mday = d.day();
	debut->tm_mon = d.month()-1;
	debut->tm_year = d.year()-1900;

	//time_t ts_debut = mktime(debut);

	struct tm *fin = localtime(&now);
	fin->tm_hour = 0;//d.hour();
	fin->tm_min = 0;//d.minute();
	fin->tm_sec = 0;//d.hour();
	fin->tm_mday = d_fin.day()+1;
	fin->tm_mon = d_fin.month()-1;
	fin->tm_year = d_fin.year()-1900;

	//time_t ts_fin = mktime(fin);
	
	QString str,str2;
	//str.sprintf("now=%ld ts=%ld  ( diff %ld heures )", now , ts, (ts-now) / 3600);
	//str.sprintf("select * from event where ts>=%ld and ts<=%ld ", ts_debut, ts_fin );

	QString conference_filter="1";
	QString evenements_filter="1";
	QString alarmes_filter="1";

	if ( "<toutes>"!=combo_conferences->currentText())
	{
		//QMessageBox::information(this,"my_slot_calendar",combo_conferences->currentText());
		
		conference_filter.sprintf("conf_number=%d", combo_conferences->currentText().toInt() );

	}
	if ( "<toutes>"!=combo_alarmes->currentText())
	{
		int index = combo_alarmes->currentIndex();
		if ( index != -1 ) {
			QVariant val = combo_alarmes->itemData(index);
			if ( QVariant::Invalid != val) {
				alarmes_filter.sprintf("alt_id=%d", val.toInt() );
				//QMessageBox::information(this,qPrintable(alarmes_filter),combo_alarmes->currentText());
			}
		}
		//QMessageBox::information(this,"my_slot_calendar",combo_alarmes->currentText());
		//alarmes_filter.sprintf("alt_id=%d", combo_alarmes->currentText().toLong() );
	}
	if ( "<toutes>"!=combo_evenements->currentText())
	{
		int index = combo_evenements->currentIndex();
		if ( index != -1 ) {
			QVariant val = combo_evenements->itemData(index);
			if ( QVariant::Invalid != val) {
				evenements_filter.sprintf("evt_id=%d", val.toInt() );
				//QMessageBox::information(this,qPrintable(evenements_filter),combo_evenements->currentText());
			}
		}
	}

	if (combo_poste->itemData(combo_poste->currentIndex()).toInt() == -1)
	  poste_filter = "1";
	else
	  poste_filter.sprintf(" device1 = %d or device2 = %d ",  
			       combo_poste->itemData(combo_poste->currentIndex()).toInt(),
			       combo_poste->itemData(combo_poste->currentIndex()).toInt());
	

	if ( d==d_fin ) {

	  	str.sprintf("select "
			"event.evt_id,event.device1,event.device2,event.conf_number,event.ts,event.value,event_type.name "
			"from event,event_type where event.evt_id=event_type.id and (%s) and (%s) and DATE('%d-%d-%d')=DATE(FROM_UNIXTIME(ts)) and (%s) "
			"order by event.ts desc ;",
			conference_filter.toStdString().c_str(), 
			evenements_filter.toStdString().c_str(), 
			    d.year(),d.month(),d.day(), poste_filter.toStdString().c_str() ) ;
	  
		str2.sprintf("select "
			"alarm.alt_id,alarm.device1,alarm.device2,alarm.conf_number,alarm.ts,alarm.value,alarm_type.name "
			"from alarm,alarm_type where alarm.alt_id=alarm_type.id and (%s) and (%s) and DATE('%d-%d-%d')=DATE(FROM_UNIXTIME(ts)) and (%s) "
			"order by alarm.ts desc ;",

			conference_filter.toStdString().c_str(), 
			alarmes_filter.toStdString().c_str(), 
			     d.year(),d.month(),d.day(), poste_filter.toStdString().c_str()) ;
	
		//btn->setStyleSheet("QPushButton { background-color: " COLOR_GREEN "  }"); 
		//QImage img("./vega_v.xpm");
		//lbl_alphacom_link->setPixmap(QPixmap::fromImage(img));
	
	}
	else{

	  	str.sprintf("select "
			"event.evt_id,event.device1,event.device2,event.conf_number,event.ts,event.value,event_type.name "
			"from event,event_type where event.evt_id=event_type.id and (%s)  and (%s) and ( DATE('%d-%d-%d')<=DATE(FROM_UNIXTIME(ts)) ) "
			"AND ( DATE('%d-%d-%d')>=DATE(FROM_UNIXTIME(ts)) ) and (%s) order by event.ts desc;", 
			conference_filter.toStdString().c_str(), 
			evenements_filter.toStdString().c_str(), 
			d.year(),d.month(),d.day() ,
			    d_fin.year(),d_fin.month(),d_fin.day(), poste_filter.toStdString().c_str()
			) ;
	  
		str2.sprintf("select "
			"alarm.alt_id,alarm.device1,alarm.device2,alarm.conf_number,alarm.ts,alarm.value,alarm_type.name "
			"from alarm,alarm_type where alarm.alt_id=alarm_type.id and (%s) and (%s) and DATE('%d-%d-%d')<=DATE(FROM_UNIXTIME(ts)) "
			"and ( DATE('%d-%d-%d')>=DATE(FROM_UNIXTIME(ts)) ) and (%s) order by alarm.ts desc ;",
			conference_filter.toStdString().c_str(), 
			alarmes_filter.toStdString().c_str(), 
			d.year(),d.month(),d.day() ,
			     d_fin.year(),d_fin.month(),d_fin.day() ,poste_filter.toStdString().c_str()
			) ;
			
			
		//btn->setStyleSheet("QPushButton { background-color: " COLOR_RED "  }"); 
		//QImage img("./vega_r.xpm");
		//lbl_alphacom_link->setPixmap(QPixmap::fromImage(img));
	}
	
	m_reqsql = str;
	
	m_reqsql_alarmes = str2;
	//	msgBox("requete : %s", m_reqsql.toStdString().c_str());
}


void FenSQL::my_slot_recherche_evenement()
{	
	exec_sql(m_reqsql); 
}

void FenSQL::my_slot_imprimer()
{	
	QTextBrowser * editor = new QTextBrowser;
	QTextCursor cursor = editor->textCursor();
	cursor.beginEditBlock();

	QTextTableFormat tableFormat;
	tableFormat.setAlignment(Qt::AlignHCenter);
	tableFormat.setBackground(QColor("#e0e0e0"));
	tableFormat.setCellPadding(2);
	tableFormat.setCellSpacing(4);

	QVector<QTextLength> constraints;
	constraints << QTextLength(QTextLength::PercentageLength, 5)
			 << QTextLength(QTextLength::PercentageLength, 5)
			 << QTextLength(QTextLength::PercentageLength, 5)
			 << QTextLength(QTextLength::PercentageLength, 5)
			 << QTextLength(QTextLength::PercentageLength, 20)
			 << QTextLength(QTextLength::PercentageLength, 30)
			 << QTextLength(QTextLength::PercentageLength, 30);
	tableFormat.setColumnWidthConstraints(constraints);

	QTextTable *tableau = cursor.insertTable(1, 7, tableFormat);

	
	
	QTextFrame *frame = cursor.currentFrame();
	QTextFrameFormat frameFormat = frame->frameFormat();
	frameFormat.setBorder(1);
	frame->setFrameFormat(frameFormat);

	
	QTextCharFormat format = cursor.charFormat();
	format.setFontPointSize(10);	
	QTextCharFormat boldFormat = format;
	boldFormat.setFontWeight(QFont::Bold);
	QTextCharFormat highlightedFormat = boldFormat;

	highlightedFormat.setBackground(Qt::yellow);

	QTextTableCell cell0 = tableau->cellAt(0,0);
	QTextTableCell cell1 = tableau->cellAt(0,1);
	QTextTableCell cell2 = tableau->cellAt(0,2);
	QTextTableCell cell3 = tableau->cellAt(0,3);
	QTextTableCell cell4 = tableau->cellAt(0,4);
	QTextTableCell cell5 = tableau->cellAt(0,5);
	QTextTableCell cell6 = tableau->cellAt(0,6);


	QTextCursor cellCursor = cell0.firstCursorPosition();
	
	cellCursor.insertText(QString("code"),boldFormat);
	cellCursor = cell1.firstCursorPosition();
	cellCursor.insertText(QString("D1"),boldFormat);
	cellCursor = cell2.firstCursorPosition();
	cellCursor.insertText(QString("D2"),boldFormat);
	cellCursor = cell3.firstCursorPosition();
	cellCursor.insertText(QString("Conf"),boldFormat);
	cellCursor = cell4.firstCursorPosition();
	cellCursor.insertText(QString("Horodate"),boldFormat);
	cellCursor = cell5.firstCursorPosition();
	cellCursor.insertText(QString("Commentaire"),boldFormat);
	cellCursor = cell6.firstCursorPosition();
	cellCursor.insertText(QString("nom du code alarme"),boldFormat);

	QTextTableCell cell;
	QTextCursor cellCursor1;

	tableau->insertRows(tableau->rows(),m_table->rowCount());
	for (int row = 1; row <= tableau->rows()-1; row ++) 
	{
	 for (int col = 0; col < 7; col ++) 
	 {
		 cell = tableau->cellAt(row, col);
		 cellCursor1 = cell.firstCursorPosition();
		 cellCursor1.insertText(tr("%1").arg(m_table->item(row-1,col)->text()));
	 }     
	}
	QDate date;
	QTime time;
	date = date.currentDate();
	time = time.currentTime();
	QString modif ="\nSysteme de conferences VEGA (copyrigth Zenitel Stentofon 2009)\nFait le :\n" + date.toString("dddd dd MMMM yyyy") + " a " + time.toString();
	editor->append(modif);
	cursor.endEditBlock();

	QPrinter printer;
	//printer.setOutputFormat(QPrinter::PdfFormat);
	//printer.setOutputFormat(QPrinter::NativeFormat);
	//printer.setOutputFileName("/home/pdl/qtest1.pdf");

	/*QPainter painter;
	if (! painter.begin(&printer)) { // failed to open file
		qWarning("failed to open file, is it writable?");
		return ;
	}
	painter.drawText(10, 10, "Test");
	if (! printer.newPage()) {
		qWarning("failed in flushing page to disk, disk full?");
		return ;
	}
	painter.drawText(10, 10, "Test 2");
	painter.end();*/
	QPrintDialog printDialog(&printer,this);
	if (printDialog.exec() == QDialog::Accepted) {
		 // print ...
		editor->print(printDialog.printer());
	}
	
	
}

void FenSQL::my_slot_recherche_alarme()
{	
		exec_sql(m_reqsql_alarmes);
}

void FenSQL::exec_sql(QString& req)
{
  //QMessageBox::information(this, "requete SQL", req.toStdString().c_str() );
  QSqlDatabase mysql_db;
  if (mysql_db.isValid() == false) {
    mysql_db = QSqlDatabase::addDatabase("QMYSQL", "searchdb");
    mysql_db.setHostName( HOST_CONTROL_IP	);
    //msgBox("ip address database :%s",HOST_CONTROL_IP);
    mysql_db.setDatabaseName("zenitel");
    mysql_db.setUserName("zenitel");
    mysql_db.setPassword("zenitel");
  }
  else{
    msgBox("invalide database :%s",HOST_CONTROL_IP);
    return;
  }
  bool ok = mysql_db.open();
  if (!ok) return;
  QSqlQuery query(mysql_db);
  if (query.exec(req) == false)
    {
      qDebug() << query.lastError();
      return;
    }
  QMessageBox msgBox;
  QString s; s.sprintf ("======= %d resultats ========", query.size() );
  msgBox.setText(s);
  msgBox.setInformativeText("Voulez vous afficher le resultat ?");
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);
  int ret = msgBox.exec();
  if ( ret == QMessageBox::No || query.size()==0 ) {
    return;
  }
	
  m_table->clearContents();
  while( m_table->rowCount() ) m_table->removeRow(0);
  QStringList headers;
  QSqlRecord rec = query.record();
  m_table->setColumnCount(rec.count());
  for (int j=0; j<rec.count() ; j++){
    headers << rec.fieldName(j);
  }
  m_table->setHorizontalHeaderLabels( headers ) ;
  //    int i;
  fprintf(stderr,"execution : %s line %d\n", __FILE__ , __LINE__ );
  if ( query.first() ) do
    {
        QList<QStandardItem *> eventList;
		//QDateTime date = QDateTime::fromTime_t(query.value(0).toInt());
        //eventList << new QStandardItem(date.toString("d/m/yyyy.h:mm:ss"));
		int rowcount = m_table->rowCount();
		m_table->insertRow(rowcount);
		for (int col=0;col<m_table->columnCount() /*rec.count()*/;col++)
		{
#if 1
			QTableWidgetItem* item0=new QTableWidgetItem (query.value(col).toString());	

			//m_table->setCurrentItem(item0);
			
			m_table->setItem(rowcount,col,item0);
			
			if ( rowcount%2 ) 
				item0->setBackgroundColor( Qt::lightGray );
			else
				item0->setBackgroundColor( Qt::yellow );

			item0->setTextColor ( Qt::darkBlue );

			if (headers[col]=="ts"){
				time_t ts = query.value(col).toInt() ;
				char horodate[128];
				strftime(horodate, sizeof(horodate), "%Y-%m-%d %H:%M:%S", localtime(&ts));	
				//stmp.sprintf("[%d] %s (%s)", j, horodate,  headers[j].toStdString().c_str());
				m_table->item(rowcount,col)->setText(QString(horodate));
			}/*else {
				m_table->item(rowcount,col)->setText(query.value(col).toString());
			}*/

#endif
		}
	}while ( query.next() );
	m_table->resizeColumnsToContents();
	mysql_db.close();

	fprintf(stderr,"execution : %s line %d\n", __FILE__ , __LINE__ );

	return;
#if 0
	//string reqsql = req.toStdString() ;
	
	//QMessageBox::information(this, "requete SQL", req.toStdString().c_str() );
		init_mysql();
		
		m_list->clear();
		m_table->clearContents();
		while( m_table->rowCount() ) m_table->removeRow(0);
		
		if ( mysql ) 
		if ( !mysql_query( mysql, reqsql.c_str()) ) // execution de la requete proprement dite!
		{
			m_list->addItem("SQL OK");
			fprintf(stderr,"==>OKOK|%s|",reqsql.c_str());
			//ret=true;
			
			MYSQL_RES *result = mysql_store_result( mysql );
			if ( result != NULL )
			{
				int rows = mysql_num_rows( result );
				//printf( "Retrieved %u rows\n", rows );
				QString s; s.sprintf ("======= Retrieved %u rows ========", rows );
						
				 QMessageBox msgBox;
				 msgBox.setText(s);
				 msgBox.setInformativeText("Voulez vous afficher le resultat ?");
				 msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
				 msgBox.setDefaultButton(QMessageBox::No);
				 int ret = msgBox.exec();

				if ( ret == QMessageBox::No) {
				   return;
				}
				 
				//table_map.clear();
				fprintf(stderr,"======= champs de la table:=========");
				QStringList headers;
				int k,NC = mysql_num_fields(result);							
				for (k=0;k<NC;k++)
				{
					MYSQL_FIELD*  fd = mysql_fetch_field( result );
					QString s; s.sprintf ("field[%d]: %s",k,fd->name);
					fprintf (stderr,"=== field[%d]: %s ===\n",k,fd->name);
					m_list->addItem(s);

					headers << fd->name ;//QString::number(k);//fd->name);
					//table_map.insert( TStrIndexPair(string(fd->name),k));

					

				}			

				map<std::string,int>::iterator it;
				/*for ( it=table_map.begin() ; it != table_map.end(); it++ )
				{
					header << (*it).first.c_str();
				}*/
				m_table->setHorizontalHeaderLabels( headers ) ;//QStringList() << "conference" << "device" ) ;
				
				
				while(1)
				{
					MYSQL_ROW sqlrow = mysql_fetch_row (result);	// essayer de lire le premier enregistrement
					if (0 != sqlrow) {
						QString s; 
						//s.sprintf ("%s %s %s evt_id:%d count:%d", sqlrow[0], sqlrow[1], sqlrow[2], table_map[string("evt_id")], table_map.size() );
						//s.sprintf ("", table_map[string("evt_id")] );


						

						int rowcount = m_table->rowCount();
						m_table->insertRow(rowcount);

						int col = 0, j=0;
						//for ( it=table_map.begin() ; it != table_map.end(); it++ )
						for ( j=0;j<NC; j++)
						{
							fprintf(stderr,"column:%d\n",col);

							QString stmp;

							//fprintf(stderr,"%s[%d] = %s   ",(*it).first.c_str(), (*it).second, sqlrow[(*it).second] );
							//stmp.sprintf("%s   ",sqlrow[(*it).second] );
							//stmp.sprintf("%s[%d] = %s   ",(*it).first.c_str(), (*it).second, sqlrow[(*it).second] );
							
							fprintf(stderr,"[%d]%s",j,sqlrow[j]);
							if (headers[j]=="ts"){
								time_t ts = atol(sqlrow[j]);//toLong() ;
								char horodate[128];
								strftime(horodate, sizeof(horodate), "%Y-%m-%d %H:%M:%S", localtime(&ts));	

								//stmp.sprintf("[%d] %s (%s)", j, horodate,  headers[j].toStdString().c_str());
								stmp = horodate;
							}else {
								//stmp.sprintf("[%d] %s (%s)", j, sqlrow[j],  headers[j].toStdString().c_str());
								stmp = sqlrow[j];
							}
							//s+=stmp;


							QTableWidgetItem* item0=new(QTableWidgetItem);m_table->setItem(rowcount,col,item0);
							m_table->item(rowcount,col)->setText(stmp);
							//QTableWidgetItem *ti = new QTableWidgetItem ("test", 0 );
							item0->setBackgroundColor( Qt::lightGray );
							item0->setTextColor ( Qt::darkBlue );
							col++;
						}
						//m_list->addItem(s);
					}else break;
				}
				m_table->resizeColumnsToContents();
				mysql_free_result (result);
			}	
		}else{
			string err = mysql_error (mysql);
			fprintf(stderr,"==>KOKO|%s|%s|\n",err.c_str() ,reqsql.c_str());
			/*sleep(2);			
			if ( err==ERR_MYSQL_GONE_AWAY  ) {
				fprintf(stderr,"ERROR %s %s",ERR_MYSQL_GONE_AWAY, reqsql.c_str());
				init_mysql();
			}
			ret = false;*/
		}
	
#endif

}

void FenSQL::mettre_a_jour(int /*result_ping*/)
{
}

