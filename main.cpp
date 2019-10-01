//smbmount //169.254.1.1/vegashare /usr/vega/share -o guest
#include <iostream>
#include <QtGui>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <rude/config.h>

#include "vega_ihm.h"
#include "switch.h"
#include "debug.h"
#include "parameters.h"
#include "liveEvents.h"
#include "configuration.h"
 
QVector<VegaConfig*>		VegaConfig::m_qlist;

void VegaConfig::loadDirectory(const char *baseDir)
{

  struct dirent *dirEntry = NULL;
  DIR *dd = NULL;
  struct stat sb;
      
  if ((dd = opendir (baseDir)) == NULL) {
    vega_log(VEGA_LOG_ERROR, "cannot open directory %s", baseDir);
    return ;
  }

  
  while ((dirEntry = readdir(dd))) 
  {
    
    if (!strcasecmp(dirEntry->d_name, ".")) 
      continue;
    if (!strcasecmp(dirEntry->d_name, "..")) 
      continue;
    if (strcasecmp(dirEntry->d_name + strlen(dirEntry->d_name) - 5, ".conf")) {
		//dbgBox("doit terminer par .conf");
      continue;
	}
    if (!strcasecmp(dirEntry->d_name, "device-keys.conf")) 
      continue;
    if (!strcasecmp(dirEntry->d_name, "vegactrl.conf")) // configuration en cours sur le controle
      continue;
    if (!strcasecmp(dirEntry->d_name, "settings.conf")) 
      continue;
    if (!strcasecmp(dirEntry->d_name, "switch.conf")) 
      continue;
    if (!strcasecmp(dirEntry->d_name, "template.conf")) 
      continue;
    if (S_ISDIR(sb.st_mode) ) // les configurations sont des fichiers et plus des repertoires !!!
      continue;

	// todo: tester si c'est bien un fichier de conf vega !!!
	char fpath[1024]={0};
	sprintf(fpath, "%s/%s", baseDir, dirEntry->d_name);
	/*if (stat(dirName, &sb) == -1) {
      vega_log(VEGA_LOG_ERROR, "cannot access %s", dirName);
      dbgBox("cannot access %s", dirName);
	  continue;
    }*/
    
	//dbgBox(dirName);

	rude::Config config;
	
	if (config.load(fpath) == false) {
		dbgBox("cannot load %s file", fpath);
		return;
	}else{

		config.setSection("general");
		const char* cfgname = config.getStringValue("name");
		if ( strlen(cfgname) <= 0 ) 
			config.setStringValue("name","nouvelleconfig100");
		config.save();
		cfgname = config.getStringValue("name");

		fprintf(stderr,"%s trouvee dans %s\n",cfgname, fpath);
		//dbgBox("trouvee:%s dans %s",cfgname, fpath);

		VegaConfig *pNewConfiguration = new VegaConfig(fpath,cfgname);
		if (pNewConfiguration->loadConfig() == true) {
			m_qlist.append(pNewConfiguration);
			//VegaConfig::pCurrent = pNewConfiguration;
			//dbgBox( QString(fpath) + QString("\n  C")+QString::number(pNewConfiguration->getNumber()) + QString(" ") +QString(pNewConfiguration->getName()) );
		}
	}    
  }
  closedir(dd);
  //return configs;
  return;
}
int main(int argc, char **argv)
{
	QApplication app(argc, argv); 
	QString homedir = QDir::homePath();
	//dbgBox(homedir);
	QString qCmd = QString("cd ") + homedir;
	int retcmd = system( qPrintable( qCmd ) );

	///////// charge les fichiers de configurations ds le repertoire courant

	//QMainWindow *MainWindow  = new QMainWindow();
	
	Ui_MainWindow *pUIMainWindow  = new Ui_MainWindow();


	pBoards = Board::loadSwitchConfigFromFile();  

	fprintf(stderr,"execution : %s line %d\n", __FILE__ , __LINE__ );


	// lire les configurations presentes dans le repertoire de travail
	VegaConfig::loadDirectory(CONFIGURATION_DIR);



	QDesktopWidget *d = QApplication::desktop();
	int w = d->width();     // returns desktop width
	int h = d->height();    // returns desktop height
	//QString str;str.sprintf("NEW desktop width: %d, desktop height: %d",w,h);	//lbl->setText(str);
	pUIMainWindow->resize(w,h);
	//MainWindow->move(10,10);

	//MainWindow->resize(1100, 800);
	//MainWindow->setFixedSize(1100, 800);

	Calculator* pcalc = new Calculator;
	//pcalc->setWindowModality(Qt::WindowModal);
	pcalc->setWindowModality(Qt::ApplicationModal);
	pcalc->setWindowFlags(Qt::WindowStaysOnTopHint);
	pcalc->move(w/3,h/3);
	if ( QDialog::Accepted == pcalc->exec() )//show();
	{




		if (pcalc->m_readOnly == true)
		{
			dbgBox("mode lecture seule");
			pUIMainWindow->setReadOnlyMode(true);
			  pUIMainWindow->setupUi();//MainWindow);
			  pUIMainWindow->setReadOnlyModeUI();
		  
		}else{
		  pUIMainWindow->setReadOnlyMode(false);
		  	  pUIMainWindow->setupUi();//MainWindow);
			  dbgBox("mode lecture ecriture");
		 
		}
		pUIMainWindow->init();	
		pUIMainWindow->show();
		return app.exec(); 
	}

	// lancement timer d'affichage de tout ce qui est dynamique ( status, event, ....)

	/*if ( !m_controlLink.isConnected() ) {
		msgBox("Deconnecte du PC de controle. Impossible de recuperer la config en cours! ");
	}else{
		msgBox("Connecte au PC de controle: recuperation de la config en cours! ");
		m_controlLink.getFichierConfigTCP();
	}*/


	
}

