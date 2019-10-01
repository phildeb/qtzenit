#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <QtGui>
#include <QMessageLogger>
#include <QApplication>
//#include <QtGui/QMessageBox>
#include "debug.h"

int verbose=1;
int loglvl = _LOG_DEBUG;

void dbgBox(const char*fmt, ...)
{
  va_list ap;
  char str[512];

  va_start(ap, fmt);
  vsprintf(str, fmt, ap);
  va_end(ap);
  
  QMessageBox msgBox;
  msgBox.setText(QString::fromUtf8(str));
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
}

void msgBox(const char*fmt, ...)
{
  va_list ap;
  char str[1024];

  va_start(ap, fmt);
  vsprintf(str, fmt, ap);
  va_end(ap);
  
  QMessageBox msgBox;
  msgBox.setText(QString::fromUtf8(str));
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
}

void dbgBox(QString str)
{
  QMessageBox msgBox;
  msgBox.setText(QString::fromUtf8(str));
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
}

void vega_log_set_loglvl(int nlvl)
{
  loglvl = nlvl;
}

void vega_log(int lvl, const char *file, int line, const char *function, const char* fmt, ...)
{

  //if (lvl < loglvl)    return;
  
  if ( ! fmt ) return;
  
  struct timeval tv;
  struct tm* ptm;
  char time_string[40];
  long milliseconds;
  FILE* fout;
  int nb=0;
  char filename[128];
  char horodate[128];
  char temp[1024]={0};
  time_t t = time(NULL);
  va_list args;
  
  if ( 0==fmt ) return;
  if ( 0==fmt[0] ) return;
  if ( '\n'==fmt[0] ) return;
  
  gettimeofday (&tv, NULL);
  ptm = localtime (&tv.tv_sec);
  strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
  milliseconds = tv.tv_usec / 1000;
  
  va_start(args, fmt);
  //nb = vsnprintf(temp, sizeof(temp) , fmt, args);
  nb = vsprintf(temp, fmt, args);
  va_end (args);
  

  while ( temp[0] && temp[strlen(temp)-1] == '\n' )
    temp[strlen(temp)-1] = 0;
  
  strftime(horodate, sizeof(horodate), "%y%m%d", localtime(&t));
  sprintf(filename,"./ihm-%s.log",horodate);
  
  fout=fopen(filename,"a+t") ;
  if ( fout ) {
    //strftime(horodate, sizeof(horodate), "%Y-%m-%d %H:%M:%S", localtime(&t));
    strftime(horodate, sizeof(horodate), "%H:%M:%S", localtime(&t));
    fprintf(fout,"%s.%03ld %s(%d) %s\n",horodate, milliseconds,file,line, temp);
    
    if ( verbose ) printf("%s.%03ld - %s - f:%s - %s\n",horodate,milliseconds,file, function, temp);
    
    fclose(fout);
  }
}

double time_between_button(struct timeval* ptv)
{
	  struct timeval _now;	      
	  gettimeofday(&_now, NULL);		
	  double t1 =  (double)ptv->tv_sec;
	  double t2 =  (double)_now.tv_sec;
	  t2 += (double)_now.tv_usec/(1000*1000);
	  t1 += (double)ptv->tv_usec/(1000*1000);
	  //vega_log(INFO,"BUTTON within %f seconds\n", t2-t1);
	  /*if ( t2 - t1 < 1.0 ){				
		  vega_log(INFO,"IGNORED EVT_DEVICE_KEY_SELECT_CONFERENCE within %f seconds\n",t2-t1);
		  break;		
	  }*/
	  //gettimeofday(&d->timeval_last_NACK_received, NULL); // memoriser la date a laquelle on prend en compte l'evt
	  return t2 - t1;
}
