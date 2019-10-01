#ifndef   	PRINTER_HH_
# define   	PRINTER_HH_

#include <QPrinter>
#include <QPainter>
#include <QPrintDialog>

class VegaPrinter {
  
public:
  VegaPrinter();
  ~VegaPrinter();
  int execDialogBoxPrint(QWidget* );
  
  int printText(const char *txt);
  
private:
  QPainter m_painter;
  QPrinter m_printer;
  QWidget *m_parentWidget;
};

#endif	    /* !PRINTER_HH_ */
