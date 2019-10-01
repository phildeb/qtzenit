#include <iostream>
#include <pthread.h>
#include "vega_ihm.h"
#include "switch.h"
#include "debug.h"
#include "configuration.h"
#include "printer.h"

void Ui_MainWindow::slot_linkControlAlphacomUp()
{
	pBtnLiaisonAlphacomControl->setStyleSheet("QPushButton { background-color: " COLOR_GREEN" }");
}
void Ui_MainWindow::slot_linkControlAlphacomDown()
{
	pBtnLiaisonAlphacomControl->setStyleSheet("QPushButton { background-color: " COLOR_RED" }");
}
void Ui_MainWindow::slot_AlimentationUp()
{
	pBtnAlimentationStatus->setStyleSheet("QPushButton { background-color: " COLOR_GREEN" }");
}
void Ui_MainWindow::slot_AlimentationDown()
{
	pBtnAlimentationStatus->setStyleSheet("QPushButton { background-color: " COLOR_RED" }");
}

void LiveEvents::run()
{
	QString strQuery;
	QSqlQuery sqlQuery;    
	fprintf(stderr,"connexion_mysql_thread %s %d\n",__FILE__,__LINE__);

	m_dbConnection = QSqlDatabase::addDatabase("QMYSQL");//, QString::number((int)this));
	m_dbConnection.setHostName(HOST_CONTROL_IP);
	m_dbConnection.setDatabaseName(DB_NAME);
	m_dbConnection.setUserName(DB_USER);
	m_dbConnection.setPassword(DB_PASSWORD);

	while(1)
	{
		sleep(1);
		if (m_dbConnection.isOpen() == false) 
		{
			fprintf(stderr,"TRY ouverture de la base de donnees mysql %s %d\n",__FILE__,__LINE__);
			if (m_dbConnection.open() == false){ // BLOQUANT !!!
				
				fprintf(stderr,"erreur ouverture de la base de donnees mysql %s %d\n",__FILE__,__LINE__);
				sleep(3);
			}
			else 
			  fprintf(stderr,"OK ouverture de la base de donnees mysql %s %d\n",__FILE__,__LINE__);
		}else{
			// AU DEBUT, chercher les dates du dernier evenement et de la derniere alarme
			if ( m_lastEventTs == 0 )
			{
				{
					strQuery =  "SELECT ts FROM alarm order by ts desc limit 2;";  
					sqlQuery =  m_dbConnection.exec(strQuery);
					if (sqlQuery.isActive() == false || sqlQuery.next() == false){ continue; }
					if ( sqlQuery.next() == false){continue; }
					m_lastAlarmTs = sqlQuery.value(0).toInt();
					sqlQuery.clear();


					strQuery =  "SELECT ts FROM  event order by ts desc limit 2;";  
					sqlQuery =  m_dbConnection.exec(strQuery);
					if ( sqlQuery.isActive() == false || sqlQuery.next() == false){continue; }
					if ( sqlQuery.next() == false){continue; }
					m_lastEventTs = sqlQuery.value(0).toInt()  ;
					sqlQuery.clear();
				}

				if (m_lastEventTs == 0)	m_lastEventTs = time(NULL);
				if (m_lastAlarmTs == 0)	m_lastAlarmTs = time(NULL);

				//if ( m_lastAlarmTs < m_lastEventTs ) m_lastEventTs = m_lastAlarmTs; // affichage de tous les evenement depuis la derniere alarme
			}


			///////////////////////////////////////////////////////////////////
			{      
			  strQuery =  "SELECT ts, value, alt_id, device1 FROM alarm WHERE ts > " + QString::number(m_lastAlarmTs) + " ; ";
 
			  fprintf(stderr,"%s\n",qPrintable(strQuery) );
      
			  strQuery = strQuery; 
			  sqlQuery =  m_dbConnection.exec(strQuery);
 
			  if (sqlQuery.isActive() == false){
				fprintf(stderr,"requete echec. Ici possible uniquement si on perdu la connexion\n");
				continue;
			  }

			  while (sqlQuery.next()) 
			  {
				  QStringList* li = new QStringList;

					fprintf(stderr,"event->description:[1]:%s [3]:%d\n", sqlQuery.value(1).toString().latin1(), sqlQuery.value(3).toInt());

					time_t t = m_lastAlarmTs = sqlQuery.value(0).toInt();//time(NULL);
					char horodate[128];
					strftime(horodate, sizeof(horodate), "%d / %m /%y", localtime(&t));
					*li<< horodate;
					strftime(horodate, sizeof(horodate), "%H:%M:%S", localtime(&t));
					*li<< horodate;
					*li<< "ALARME";
					*li << sqlQuery.value(1).toString().latin1();
					 *li << sqlQuery.value(2).toString().latin1();
					

					m_liveEvents.qlist_journalDeBord_mutex.lock();
					{
						m_liveEvents.qlist_journalDeBord.append(li);
					}
					m_liveEvents.qlist_journalDeBord_mutex.unlock();

			  }// while next
			  sqlQuery.clear();
			}

			{      
				strQuery =  "SELECT ts, value,evt_id FROM event WHERE ts > " + QString::number(m_lastEventTs) + " ; ";
 
			  fprintf(stderr,"%s\n",qPrintable(strQuery) );
      
			  strQuery = strQuery; 
			  sqlQuery =  m_dbConnection.exec(strQuery);
 
			  if (sqlQuery.isActive() == false){
				fprintf(stderr,"requete echec. Ici possible uniquement si on perdu la connexion\n");
				continue;
			  }

			  while (sqlQuery.next()) 
			  {
				  QStringList* li = new QStringList;

					fprintf(stderr,"event->description:[1]:%s [3]:%d\n", sqlQuery.value(1).toString().latin1(), sqlQuery.value(3).toInt());

					time_t t = m_lastEventTs = sqlQuery.value(0).toInt();//time(NULL);
					char horodate[128];
					strftime(horodate, sizeof(horodate), "%d / %m /%y", localtime(&t));
					*li<< horodate;
					strftime(horodate, sizeof(horodate), "%H:%M:%S", localtime(&t));
					*li<< horodate;
					*li << "EVENEMENT";
					*li << sqlQuery.value(1).toString().latin1();
					 *li << sqlQuery.value(2).toString().latin1();
					

					m_liveEvents.qlist_journalDeBord_mutex.lock();
					{
						m_liveEvents.qlist_journalDeBord.append(li);
					}
					m_liveEvents.qlist_journalDeBord_mutex.unlock();

			  }// while next
			  sqlQuery.clear();
			}

			// bouton connexion Alphacom <-> Control
			{

					strQuery.sprintf("SELECT alt_id,ts FROM alarm where (alt_id=%d or alt_id=%d) order by ts desc limit 1;",
						EV_ALARM_CONTROL_NETWORK_DOWN, EV_ALARM_CONTROL_NETWORK_UP ) ;  

					fprintf(stderr,"REQ:%s\n",qPrintable(strQuery) );
					
					  strQuery = strQuery; 
					  //pthread_create (&t_id, NULL, async_exec_mysql_thread, &sql);usleep(100 * 1000);
					sqlQuery =  m_dbConnection.exec(strQuery);
					if (sqlQuery.isActive() == false || sqlQuery.next() == false){continue ;}
					{

						if ( EV_ALARM_CONTROL_NETWORK_UP == sqlQuery.value(0).toInt() )
							emit signal_linkControlAlphacomUp();
							//pBtnLiaisonAlphacomControl->setStyleSheet("QPushButton { background-color: " COLOR_GREEN" }");
						else
							emit signal_linkControlAlphacomDown();
							//pBtnLiaisonAlphacomControl->setStyleSheet("QPushButton { background-color: " COLOR_RED" }");
					}
					sqlQuery.clear();

			}

			// bouton alimentation Up / Down
			{
					strQuery.sprintf("SELECT alt_id,ts FROM alarm where (alt_id=%d or alt_id=%d) order by ts desc limit 1;",
						EV_ALARM_POWER_DOWN, EV_ALARM_POWER_UP ) ;  

					fprintf(stderr,"REQ:%s\n",qPrintable(strQuery) );

					  strQuery = strQuery; 
					  //pthread_create (&t_id, NULL, async_exec_mysql_thread, &sql);usleep(100 * 1000);
					sqlQuery =  m_dbConnection.exec(strQuery);
					if (sqlQuery.isActive() == false || sqlQuery.next() == false){continue ;}
					{
						if ( EV_ALARM_POWER_UP == sqlQuery.value(0).toInt() )
							emit signal_AlimentationUp();
							//pBtnAlimentationStatus->setStyleSheet("QPushButton { background-color: " COLOR_GREEN" }");
						else
							emit signal_AlimentationDown();
							//pBtnAlimentationStatus->setStyleSheet("QPushButton { background-color: " COLOR_RED" }");
					}
					sqlQuery.clear();
			}
		}
	}
}

LiveEvents						m_liveEvents;

void  Ui_MainWindow::slot_timerJournaldeBord()
{ 	
	fprintf(stderr,"slot_timerJournaldeBord\n");
	int nb=0;

	m_liveEvents.qlist_journalDeBord_mutex.lock();
	{
		while (m_liveEvents.qlist_journalDeBord.size()>0)
		{
			QStringList* li = m_liveEvents.qlist_journalDeBord.takeFirst();
			if ( li ) {

					int rowcount = tableLiveEvents->rowCount();
					tableLiveEvents->insertRow(rowcount );
					tableLiveEvents->setRowHeight(rowcount, 15);

					QTableWidgetItem* item0=new QTableWidgetItem (li->at(0));
					QTableWidgetItem* item1=new QTableWidgetItem (li->at(1));
					QTableWidgetItem* item2=new QTableWidgetItem (li->at(2));
					QTableWidgetItem* item3=new QTableWidgetItem (li->at(3));
					QTableWidgetItem* item4=new QTableWidgetItem (li->at(4));
					
					if ( li->at(2)=="ALARME") item0->setBackgroundColor( Qt::red );
					else item0->setBackgroundColor( Qt::yellow );

					tableLiveEvents->setItem(rowcount,0,item0);
					tableLiveEvents->setItem(rowcount,1,item1);
					tableLiveEvents->setItem(rowcount,2,item2);
					tableLiveEvents->setItem(rowcount,3,item3);
					tableLiveEvents->setItem(rowcount,4,item4);

					if ( rowcount > 200 ) {
						tableLiveEvents->removeRow(0);
					}
					tableLiveEvents->scrollToItem(item0);
					delete li;

			}
		}
	}
	m_liveEvents.qlist_journalDeBord_mutex.unlock();

}

