#ifndef   	DEBUG_HH_
# define   	DEBUG_HH_

#define _INFO_ __FILE__,__LINE__,__FUNCTION__


#define _LOG_DEBUG      0
#define _LOG_INFO       1
#define _LOG_ERROR      2
#define _LOG_CRITICAL   3


#define VEGA_LOG_INFO                    _LOG_INFO, _INFO_
#define VEGA_LOG_DEBUG                    _LOG_DEBUG, _INFO_
#define VEGA_LOG_ERROR                    _LOG_ERROR, _INFO_
#define VEGA_LOG_CRITICAL                 _LOG_CRITICAL, _INFO_

#include <stdarg.h>
#include <QString>

extern "C" {
  void vega_log(int loglvl, const char *file, int line, const char *function, const char* fmt, ...);
}

void dbgBox(const char*fmt, ...);
void msgBox(const char*fmt, ...);
void dbgBox(QString);


#endif	    /* !DEBUG_HH_ */
