#if 0

#include <malloc.h>
#include <iostream>
#include "liveEvents.h"
#include "debug.h"
#include "vega_ihm.h"
#define NB_DEVICES	46
#define NB_BOARDS	26


LiveEvents::LiveEvents()
{
  m_lastEventTs = 0;
  m_lastAlarmTs = 0;
  m_stop = false;
};

LiveEvents::~LiveEvents()
{
  m_dbConnection.close();
};
    
void LiveEvents::getLastEventDate()
{
#if 0
  QString strQuery;
  QSqlQuery query;    
  int i;

  m_lastAlarmTs = m_lastEventTs = 0;
  QString n[2] = { "alarm", "event" };
  
  for (i = 0 ;  i < 1 ; i ++) 
  {

    strQuery =  "SELECT ts FROM " + n[i] +  " order by ts desc limit 1;";  
    
    query =  m_dbConnection.exec(strQuery);
    if (query.isActive() == false || query.next() == false)
      continue;
   
    m_lastEventTs = query.value(0).toInt();
  }

  for (i = 1 ;  i < 2 ; i ++) 
  {

    strQuery =  "SELECT ts FROM " + n[i] +  " order by ts desc limit 1;";  
    
    query =  m_dbConnection.exec(strQuery);
    if (query.isActive() == false || query.next() == false)
      continue;
   
    m_lastAlarmTs = query.value(0).toInt();
  }
  
  if (m_lastEventTs == 0)
    m_lastEventTs = time(NULL);
  if (m_lastAlarmTs == 0)
    m_lastAlarmTs = time(NULL);
  
#endif

  return;
  
}

void LiveEvents::run()
{
#if 0

	QString strQuery;
	live_event_t *event;
	bool bIsNewEvent;
	int i ;
	alarm_t alarm;


    m_dbConnection = QSqlDatabase::addDatabase("QMYSQL", QString::number((int)this));
    m_dbConnection.setHostName(HOST_CONTROL_IP);
    m_dbConnection.setDatabaseName(DB_NAME);
    m_dbConnection.setUserName(DB_USER);
    m_dbConnection.setPassword(DB_PASSWORD);

	
	// recupere les alarmes en cours
	//getCurrentAlarmsStatusFromDB() ;  
	//emit alarmReceived(&m_currentAlarms);
	//getLastEventDate();

	fprintf(stderr,"execution : %s line %d\n", __FILE__ , __LINE__ );


  while (1) 
  {

	if (m_dbConnection.isOpen() == false) 
	{
	  if (m_dbConnection.open() == false)
	  {
		fprintf(stderr,"erreur ouverture de la base de donnees mysql %s %d\n",__FILE__,__LINE__);
		usleep(CHECK_EVENT_POLLING_PERIOD * 1000);
		continue;
	  }
	
	}else{
		fprintf(stderr,"OK ouverture de la base de donnees mysql %s %d\n",__FILE__,__LINE__);
		getCurrentAlarmsStatusFromDB() ;  
		emit alarmReceived(&m_currentAlarms);
		getLastEventDate();
	}


    
    usleep(CHECK_EVENT_POLLING_PERIOD * 1000);

    QSqlQuery query;    
    bIsNewEvent = false;

    for (i = 0; i < 2; i ++) 
	{
      
      if (i == 0)
		strQuery =  "SELECT ts, value FROM event WHERE ts > " + QString::number(m_lastEventTs) + " ; ";
      else
		strQuery =  "SELECT ts, value, alt_id, device1 FROM alarm WHERE ts > " + QString::number(m_lastAlarmTs) + " ; ";
 


	
	  fprintf(stderr,"%s\n",qPrintable(strQuery) );
      
      query =  m_dbConnection.exec(strQuery);

      
	  if (query.isActive() == false)
	  {
		/// requete a echouée. Ici possible uniquement si on perdu la connexion
		
		return;
      }



      while (query.next()) 
	  {
	

		  fprintf(stderr,"query.next\n");
		
		  if (i == 0) {

			  event = (live_event_t*)malloc(sizeof(live_event_t));
			  event->type = CONFERENCE_EVENT;
			  event->date = query.value(0).toInt();

			  sprintf(event->description , "%s", query.value(1).toString().latin1());
			  m_lastEventTs = event->date;
			  enqueueEvent(event);
			}
			else {
			  
			  //// ici evenement de type alarme ... (pas coherent avec la notion de alarmes et evenements dans le controle...)
			  /// Utile pour afficher dans le fil de l'eau

			  event = (live_event_t*)malloc(sizeof(live_event_t));
			  event->type =  ALARM_EVENT;
			  event->date = query.value(0).toInt();

			  sprintf(event->description , "%s", query.value(1).toString().latin1());
			  
			  fprintf(stderr,"event->description:[1]:%s [3]:%d\n", query.value(1).toString().latin1(), query.value(3).toInt());
			  enqueueEvent(event);

			  //// On cree une nouvelle alarme
			  alarm.type = query.value(2).toInt();
			  alarm.date = event->date;

			  switch (alarm.type) {
			  case EV_ALARM_BOARD_ANOMALY:
			  case EV_ALARM_END_BOARD_ANOMALY:
				alarm.board_number = query.value(3).toInt();
				break;
			  case EV_ALARM_DEVICE_ANOMALY:
			  case EV_ALARM_END_DEVICE_ANOMALY:
				alarm.device_number = query.value(3).toInt();
				break;
			  default:
				break;
			  }

			  sprintf(alarm.description , "%s", query.value(1).toString().latin1());
			  m_lastEventTs = alarm.date;
			}

			bIsNewEvent = true;
      }// while next
 
      query.clear();
 
    }// for 1 a 2

  
    if (bIsNewEvent == true) 
	{
      getCurrentAlarmsStatusFromDB() ;
      emit alarmReceived(&m_currentAlarms);
      emit updateEvents((const QList<live_event_t*> *)&m_lastEvents);
      query.clear();
    }
    
    if (m_stop == true)
      break;  
  }
  m_stop = false;

#endif
  
  return;
}


bool LiveEvents::getCurrentAlarmsStatusFromDB()
{
#if 0
  // recuperer les alarmes sur cartes
  QString req1;
  QString req2;
  QSqlQuery query;    
  int dateStart;
  int dateEnd;
  int i;
  
  QList<alarm_t*>::iterator ia;

  // efface toutes les alarmes en memoire
  for (ia = m_currentAlarms.begin(); ia != m_currentAlarms.end(); ia++) 
    free (*ia);
 
  m_currentAlarms.clear();

  for (i = 1; i < NB_BOARDS + 1; i++) 
  {	// 26 : nombre de cartes
    dateStart = 0;
    dateEnd = 0;
    
    // recherche un debut d'alarme pour la carte i 
    req1 = "SELECT ts, value FROM alarm WHERE device1='" + QString::number(i) + "' ";
    req1 = req1  + " AND alt_id=" + QString::number(EV_ALARM_BOARD_ANOMALY) +  " ";					  
    req1 = req1 + " order by ts desc limit 1;";
    query =  m_dbConnection.exec(req1);
    
    if (query.next() == true) 
      dateStart = query.value(0).toInt();
    query.clear();

    // recherche une fin d'alarme pour la carte i
    req2 = "SELECT ts, value FROM alarm WHERE device1='" + QString::number(i) + "' ";
    req2 = req2  + " AND alt_id=" + QString::number(EV_ALARM_END_BOARD_ANOMALY) +  " ";					  
    req2 = req2 + " order by ts desc limit 1;";
 
    query =  m_dbConnection.exec(req2);
    if (query.next() == true) 
      dateEnd = query.value(0).toInt();
    query.clear();    
    
    if (dateEnd == 0 && dateEnd == dateStart) // pas d'alarme
      continue;
    if (dateEnd == 0 && dateStart != 0 || dateEnd < dateStart) {
      /* 
	 2 case: 
	 - Un debut d'alarme mais pas de fin, donc une alarme en cours
	 - la fin d'alarme est plus ancienne qu'une nouvelle
      */
      alarm_t *pAlarm;
      pAlarm = (alarm_t*) malloc(sizeof(alarm_t));
      pAlarm->board_number = i;
      pAlarm->date = dateStart;
      sprintf(pAlarm->description, "Alarme sur carte %d", i);
      pAlarm->type = EV_ALARM_BOARD_ANOMALY;
      m_currentAlarms.append(pAlarm);
      printf("nouvelle alarme sur carte %d\n", i);
    }
    if (dateStart != 0 && dateEnd == 0) {
      /// Une fin mais pas de bebut  => tres bizzare !!
      printf("recuperation date alarmes pour carte %d : date de debut  == 0 et date de fin =%d\n", i, dateEnd);
    }
  } 

  for (i = 1; i < NB_DEVICES + 1; i++) {
    dateStart = 0;
    dateEnd = 0;

    req1 = "SELECT ts, value FROM alarm WHERE device1='" + QString::number(i) + "' ";
    req1 = req1  + " AND alt_id=" + QString::number(EV_ALARM_DEVICE_ANOMALY) +  " ";					  
    req1 = req1 + " order by ts desc limit 1;";
    query =  m_dbConnection.exec(req1);
    if (query.next() == true) 
      dateStart = query.value(0).toInt();
    query.clear();

    req2 = "SELECT ts, value FROM alarm WHERE device1='" + QString::number(i) + "' ";
    req2 = req2  + " AND alt_id=" + QString::number(EV_ALARM_END_DEVICE_ANOMALY) +  " ";					  
    req2 = req2 + " order by ts desc limit 1;";
    query =  m_dbConnection.exec(req2);
    if (query.next() == true) 
      dateEnd = query.value(0).toInt();
    query.clear();    
    
    if (dateEnd == 0 && dateEnd == dateStart)
      continue;
    if (dateEnd == 0 && dateStart != 0 || dateEnd < dateStart) {
      /* 
	 Un debut d'alarme mais pas de fin, donc une alarme en cours
	 ou bien une la fin d'alarme est plus ancienne qu'une nouvelle
      */
      alarm_t *pAlarm;
      pAlarm = (alarm_t*) malloc(sizeof(alarm_t));
      pAlarm->device_number = i;
      pAlarm->date = dateStart;
      sprintf(pAlarm->description, "Alarme sur poste %d", i);
      pAlarm->type = EV_ALARM_DEVICE_ANOMALY;
      m_currentAlarms.append(pAlarm);
    }
//     if (dateStart == 0 && dateEnd != 0) {
//       /// Une fin mais pas de bebut  => tres bizzare !!
//       printf("recuperation date alarmes pour poste %d : date de debut  == 0 et date de fin =%d\n", i, dateEnd);
//     }
  } 

  // alarme sur lien TCP entre l'alphacom et le controle
  req1 = "SELECT ts, value FROM alarm WHERE alt_id=" + QString::number(EV_ALARM_CONTROL_NETWORK_DOWN) +  " ";					  
  req1 = req1 + " order by ts desc limit 1;";
  query =  m_dbConnection.exec(req1);
  if (query.next() == true) 
    dateStart = query.value(0).toInt();
  query.clear();
  
  req2 = "SELECT ts, value FROM alarm WHERE alt_id=" + QString::number(EV_ALARM_CONTROL_NETWORK_UP) +  " ";					  
  req2 = req2 + " order by ts desc limit 1;";
  query =  m_dbConnection.exec(req2);
  if (query.next() == true) 
    dateEnd = query.value(0).toInt();
  query.clear();    
  
  if (dateEnd == 0 && dateStart != 0 || dateEnd < dateStart) {
    /* 
       Un debut d'alarme mais pas de fin, donc une alarme en cours
       ou bien une la fin d'alarme est plus ancienne qu'une nouvelle
    */
    alarm_t *pAlarm;
    pAlarm = (alarm_t*) malloc(sizeof(alarm_t));
    pAlarm->device_number = i;
    pAlarm->date = dateStart;
    sprintf(pAlarm->description, "Alarme sur lien TCP commutateur - control");
    pAlarm->type = EV_ALARM_CONTROL_NETWORK_DOWN;
    m_currentAlarms.append(pAlarm);
  }


  // alarme sur ALIMENTATION pdl 20090907 puis 20091004
  req1 = "SELECT ts, value FROM alarm WHERE alt_id=" + QString::number(EV_ALARM_POWER_DOWN) +  " ";					  
  req1 = req1 + " order by ts desc limit 1;";
  query =  m_dbConnection.exec(req1);
  if (query.next() == true) 
    dateStart = query.value(0).toInt();
  query.clear();
  
  req2 = "SELECT ts, value FROM alarm WHERE alt_id=" + QString::number(EV_ALARM_POWER_UP) +  " ";					  
  req2 = req2 + " order by ts desc limit 1;";
  query =  m_dbConnection.exec(req2);
  if (query.next() == true) 
    dateEnd = query.value(0).toInt();
  query.clear();    
  
  if (dateEnd == 0 && dateStart != 0 || dateEnd < dateStart)
   {
    /* 
       Un debut d'alarme mais pas de fin, donc une alarme en cours
       ou bien une la fin d'alarme est plus ancienne qu'une nouvelle
    */
    alarm_t *pAlarm;
    pAlarm = (alarm_t*) malloc(sizeof(alarm_t));
    pAlarm->device_number = i;
    pAlarm->date = dateStart;
    sprintf(pAlarm->description, "Alarme sur ALIMENTATION");
    pAlarm->type = EV_ALARM_POWER_DOWN;
    m_currentAlarms.append(pAlarm);
  }
#endif

  return true;
}

void LiveEvents::enqueueEvent(live_event_t *pEvent)
{
#if 0
  live_event_t *event;
    
  if (m_lastEvents.count() == NB_MAX_LAST_LIVE_EVENTS)  {
    event = m_lastEvents.at(NB_MAX_LAST_LIVE_EVENTS-1);
    if (event) free (event);
    m_lastEvents.removeAt(NB_MAX_LAST_LIVE_EVENTS - 1);
  }
   
  m_lastEvents.insert(0,  pEvent); 
#endif
  return;
}

#endif
