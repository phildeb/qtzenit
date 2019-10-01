#include <iostream>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "vega_ihm.h"
#include "controlLink.h"
#include "configuration.h"
#include "debug.h"
#include <rude/config.h>

#define CONNECT_POLLING_TIME	2

extern controlLink			m_controlLink;
struct timeval				timeval_last_PONG_received;

extern double time_between_button(struct timeval* ptv);

void Ui_MainWindow::initControlConnectionTCP()
{    
	m_controlLink.start();
	connect(&m_controlLink, SIGNAL(signal_linkIHMControlDown()), this, SLOT(slot_linkIHMControlDown()));
	connect(&m_controlLink, SIGNAL(signal_linkIHMControlUp()), this, SLOT(slot_linkIHMControlUp()));	
	connect(&m_controlLink, SIGNAL(signal_got_running_config()), this, SLOT(slot_updateVegaConfigCombo()));
}

void Ui_MainWindow::controlConnectionRestartSucceed()
{
  msgBox("La demande de redemarrage a ete acceptee !");
}

void Ui_MainWindow::controlConnectionRestartFailed()
{
  msgBox("La demande de redemarrage a ete refusee !");
}

void Ui_MainWindow::controlConnectionLost()
{ // connexion perdue avec le control
  //msgBox("Ui_MainWindow::controlConnectionLost");
  printf("Ui_MainWindow::controlConnectionLost\n");
  if (m_controlConnectionLostUserAlerted == false) 
  {
    m_controlConnectionLostUserAlerted = true;
  }
  goOfflineMode();
  QPushButton *pButtonLink = NULL;
  if ( (pButtonLink = qFindChild<QPushButton*>(this,"buttonLinkState")) != NULL ) 
  {
  	  pButtonLink->setStyleSheet("QPushButton { background-color: " COLOR_RED " }");
  }
}

void Ui_MainWindow::controlConnectionEstablished()
{
  //msgBox("Ui_MainWindow::controlConnectionEstablished");
  fprintf(stderr,"Ui_MainWindow::controlConnectionEstablished\n");

  m_controlConnectionLostUserAlerted = false;

  goOnlineMode();

  QPushButton *pButtonLink = NULL;
  if ( (pButtonLink = qFindChild<QPushButton*>(this,"buttonLinkState")) != NULL ) 
  {
  	  pButtonLink->setStyleSheet("QPushButton { background-color: " COLOR_GREEN " }");
  }

  return;
}

bool controlLink::isConnected()
{
  if (m_socket == NULL)
    return false;
  if ( m_socket->state() == QAbstractSocket::ConnectedState)
    return true;
  else 
    return false;
}

controlLink::controlLink()
{//#define BUFFER_SIZE  1024
  m_bufferEmission.resize(10*1024);
  m_bufferReception.resize(10*1024);
  m_connected = false;
  m_stop = false;
  m_socket = NULL;
};

/*void controlLink::activateKeepAlive(QTcpSocket *socket)
{
  int sock = socket->socketDescriptor();
  int optval = 1;
  int optlen = sizeof(optval);
  setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);
}*/
  
bool controlLink::sendRestartRequestTCP()
{
	fprintf(stderr,"sendRestartRequestTCP\n");
	send(m_socket->socket(), "<RST>",strlen("<RST>"), 0) ; 
	return true;//sendMessage("<RST>");
}

bool controlLink::getFichierConfigTCP()
{
	fprintf(stderr,"getFichierConfigTCP\n");
	send(m_socket->socket(), "<GET>",strlen("<GET>"), 0) ; 
	return true;//sendMessage("<RST>");
}

bool controlLink::sendFichierConfigTCP(const char *nomConfig, const char* nomFichier )
{	
	fprintf(stderr,"sendFichierConfig %s %s\n", nomConfig, nomFichier);
  
	if (m_connected == false)  {
		return false;
	}

	FILE *f = fopen(nomFichier,"r");
	if ( f ) 
	{		
		fseek( f, 0L, SEEK_END );
		long endPos = ftell( f );
		fseek( f, 0L, SEEK_SET );
		fprintf(stderr,"The file size of %s is %ld bytes\n", nomFichier, endPos);

		dbgBox("Envoi au Controle du fichier %s de taille %d octets",nomFichier, endPos);
	   
		send(m_socket->socket(), "<PUT|",strlen("<PUT|"), 0) ; 
		while (!feof(f) )
		{
			char buffer[10*1024]={"<PUT|"}; // 5 octets
			int nbread = fread(buffer+5, 1, sizeof(buffer)-5, f);
			buffer[nbread+5] = '>'; // 1 octet
			//buffer[nbread+6] = 0;

			int nbsent = send(m_socket->socket(), buffer, nbread + 5 + 1 , 0) ; 
			fprintf(stderr,"nbread %d/nbsend %d\n", nbread,nbsent);
			//dbgBox("nbread %d/nbsend %d\n", nbread,nbsent);
		}
		fclose(f);
	}

  //if (send(sock, str,len, 0) != len)    return false;
  return true;
}

void controlLink::eventConnected()
{
	fprintf(stderr,"controlLink::eventConnected\n");
	m_connected = true;
	//emit eventConnectionEstablished();
	emit signal_linkIHMControlUp();
}


void controlLink::eventDisconnected()
{
	fprintf(stderr,"controlLink::eventDisconnected\n");
	m_connected = false;
	emit signal_linkIHMControlDown();
}

void Ui_MainWindow::slot_linkIHMControlDown()
{
	pBtnLiaisonIHMControl->setStyleSheet("QPushButton { background-color: " COLOR_RED" }");
}

void Ui_MainWindow::slot_linkIHMControlUp()
{
	pBtnLiaisonIHMControl->setStyleSheet("QPushButton { background-color: " COLOR_GREEN" }");
}

void controlLink::run()
{
	m_socket = new QTcpSocket();
	//qint64 ret;
	bool beginFound = false;    
	m_connected = false;
	//qRegisterMetaType<QIODevice::OpenMode>("OpenMode");
	//activateKeepAlive(m_socket); // ne fonctionne pas
	//connect(m_socket, SIGNAL(disconnected()), this, SLOT(eventDisconnected()));
	fprintf(stderr,"controlLink::run...\n");
	while (1) 
	{
		if (m_stop == true)
			break;
		fprintf(stderr, "socket state = %d\n",m_socket->state());
		if ( m_socket->state()==QAbstractSocket::ConnectedState && m_connected == true) 
		{
			char c;
			fprintf(stderr,"attente sur reception TCP ...\n");
			// attente bloquante sur des donnees (ou erreur) sur la m_socket
			if (m_socket->waitForReadyRead(5000) == false) 
			{
				// no data available
				int nbsent = send(m_socket->socket(), "<PING>",strlen("<PING>"), 0) ;
				//m_controlLink.sendMessage("<PING>");

				int tdiff = time_between_button(&timeval_last_PONG_received );

				fprintf(stderr,"envoi PING au controle: %d octets ( tdiff=%d)\n",nbsent, tdiff);


				if ( tdiff > 10 ){
					m_socket->disconnectFromHost();
					emit signal_linkIHMControlDown();
					//pBtnLiaisonIHMControl->setStyleSheet("QPushButton { background-color: " COLOR_RED" }");
				}else{
					emit signal_linkIHMControlUp();
					//pBtnLiaisonIHMControl->setStyleSheet("QPushButton { background-color: " COLOR_RED" }");
				}
				continue;
			}
     
			while (m_socket->bytesAvailable() > 0 && m_socket->state()==QAbstractSocket::ConnectedState) 
			{
			  int ret = m_socket->read(&c, 1);	
			  if (ret <= 0) 
			    {
				vega_log(VEGA_LOG_ERROR, "connection error with control");
				m_connected = false;
				continue;
			    }
		
			  if (beginFound == false && c == '<') 
			    {
			      beginFound = true;
			      m_bufferReception.clear();
			      strcat(m_bufferReception.data(), "<");
			      fprintf(stderr,"============= DEBUT PACKET TCP:%c len=%d================\n",c,m_bufferReception.size());
			      
			    }      
			  else if (beginFound == true) 
			    {
			      if (c == '>') 
				{
				  emit signal_linkIHMControlUp();
				  fprintf(stderr,"FIN PACKET TCP:%c len=%d\n",c,m_bufferReception.size());
				  //strcat(m_bufferReception.data(), ">");
				  {
				    char temp[40]={0};
				    strncpy(temp,m_bufferReception.data(), sizeof(temp)-1);
				    fprintf(stderr,"FIN PACKET TCP{%s}\n",temp);
				    
				  }
				  
				  /*if (!strncmp(m_bufferReception.data(), MSG_ACK, strlen(MSG_ACK))) 
				    {
				    vega_log(VEGA_LOG_DEBUG, "received ACK");
				    if (m_restartRequestPending == true) {
				    emit eventRestartResponseOK();
				    m_restartRequestPending = false;
				    }
				    m_bufferReception.clear();
				    }
				    else*/
				  if (!strncmp(m_bufferReception.data(), "PONG", strlen("PONG"))) 
				    {
				      vega_log(VEGA_LOG_DEBUG, "received PONG");
				      fprintf(stderr,"received PONG\n");
				      //emit eventConnectionEstablished();
				      
				      
				      gettimeofday(&timeval_last_PONG_received, NULL); // memoriser 
				      m_bufferReception.clear();
				      
				      
				      /*if (m_restartRequestPending == true) {
					emit eventRestartResponseNOK();
					m_restartRequestPending = false;
					}*/
				    }
				  else if (!strncmp(m_bufferReception.data(), "GET", strlen("GET") ) ) 
				  {
					  fprintf(stderr,"TCP Message: %s...\n","GET");
					  char pathrunning[1024]={0};
					  sprintf(pathrunning, "%s/%s", CONFIGURATION_DIR, "vegactrl.conf");
					  FILE* fic = fopen(pathrunning,"w");
					  if ( fic ) 
					  {
						  int idx = m_bufferReception.indexOf("["); 
						  if ( idx >= 0 ){ 
							  int nbwritten=0;
								//char *data = m_bufferReception.data();
								//while (*data) 
							  for (int i = idx; i < m_bufferReception.size(); ++i) 
							  {
									char carcou = m_bufferReception.at(i);//*data;
									nbwritten += fwrite(&carcou,1,1,fic);
									//++data;
									//if ( *data=='>') break;
								}
							  fclose(fic);
							  fprintf(stderr,"nbwritten:%d in %s\n",nbwritten,pathrunning);

							  if ( nbwritten > 0 ) 
							  {
								  rude::Config config;
								if (config.load(pathrunning) == false) 
								{
									dbgBox("cannot load %s file", pathrunning);
									return;
								}
								else
								{
									emit signal_got_running_config();
								}
							  }
						  }
					  }
					  /// reinitialization
					  beginFound = false;
					  //bzero(m_bufferReception.data(), BUFFER_SIZE);
					  m_bufferReception.clear();
				
				  }//if (!strncmp(m_bufferReception.data(), MSG_ACK, strlen(MSG_ACK))) 
				  else 
				  {
					  char temp[20]={0};
					  strncpy(temp,m_bufferReception.data(), sizeof(temp)-1);
					  fprintf(stderr,"Unhandled TCP Message[%s]\n",temp);
				  }
			
				  m_bufferReception.clear();

				}

			  else //if ( isprint(c) || c=='\n' || c=='\r')
			  {
				  if (  c=='\n'){
					fprintf(stderr,"1er octet: fin de ligne LF %02X\n",c);
					if (m_bufferReception.size() == 0 ) continue;;
				
				  }
				  
				  if (  c=='\r'){
					fprintf(stderr,"1er octet: fin de ligne CR %02X\n",c);
					if (m_bufferReception.size() == 0 ) continue;
				  }
				
				  if ( c!='<' && c!= '>' ) 
				  {
					m_bufferReception.append((char)c);
					fprintf(stderr,"%c",c);
				
				  }
			  }
			    }//else if (beginFound == true)
		  	}// while
		}
		else // if (m_connected == false)  
		{
			

			fprintf(stderr,"try connectToHost %s %d\n",HOST_CONTROL_IP,HOST_CONTROL_PORT);
			m_socket->connectToHost(HOST_CONTROL_IP, HOST_CONTROL_PORT);
			if (m_socket->waitForConnected(3000) == false) 
			{
				fprintf(stderr,"NOK connectToHost %s %d\n",HOST_CONTROL_IP,HOST_CONTROL_PORT);
				emit signal_linkIHMControlDown();
				sleep (CONNECT_POLLING_TIME);
				m_connected = false;
			}
			else 
			{
				emit signal_linkIHMControlUp();
				fprintf(stderr,"OK connectToHost %s %d\n",HOST_CONTROL_IP,HOST_CONTROL_PORT);
				m_connected = true;
				gettimeofday(&timeval_last_PONG_received, NULL); // memoriser 
				m_bufferReception.clear();	
			}
			//	sleep(3);
		} // if (m_connected == true) 
	}// while(1)
	m_socket->close();
}
