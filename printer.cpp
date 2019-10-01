#include <iostream>
#include <QtGui>
#include "debug.h"
#include "printer.h"

VegaPrinter::VegaPrinter(){  };

VegaPrinter::~VegaPrinter(){};

int VegaPrinter::execDialogBoxPrint(QWidget *pWidget)
{
  QPrintDialog dialog(&m_printer, pWidget);
  dialog.setWindowTitle("Imprimer la configuration");

  m_parentWidget = pWidget;
  
  if (dialog.exec() != QDialog::Accepted)
    return -1;

  return 0;
};

// In typography, a point is the smallest unit of measure, being a
// subdivision of the larger pica. It is commonly abbreviated as
// pt. The traditional printer's point, from the era of hot metal
// typesetting and presswork, varied between 0.18 and 0.4 mm depending
// on various definitions of the foot.
// Today, the traditional point has been supplanted by the desktop
// publishing point (also called the PostScript point), which has been
// rounded to an even 72 points to the inch (1 point = 127⁄360 mm =
// 352.7 µm). In either system, there are 12 points to the pica.


int VegaPrinter::printText(const char *filename)
{
   ;
  const char *line;
  int i = 0;
  int k = 0;
  
  QFont font("Helvetica Normal"); /// point size de 12 par defaut
  QFontMetrics fontMetrics(font);
  QRect rect = m_printer.pageRect();
  
  m_painter.begin(&m_printer);
  m_painter.setFont(font);  



     QFile file(filename);
     if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
		 dbgBox("impossible d'ouvrir le fichier %s",filename);
         return -1;
	 }

	 m_painter.drawText(1, 1, filename);

     QTextStream in(&file);
     while (!in.atEnd()) {
         QString line = in.readLine();
         //process_line(line);
		 //m_painter.drawText(1, 1, text);

      int height = i * (fontMetrics.height() + fontMetrics.leading());
      QString text = line;
      m_painter.drawText(5, height, text);
      line = strtok(NULL, "\n");
      i ++;   k++;
      if (height > rect.height()) 
	  {
		k = 0; i = 0;
	
			if (m_printer.newPage() == false) 
			{
			  m_painter.end();
			  QMessageBox msgBox(m_parentWidget);
			  msgBox.setText(QString::fromUtf8("Impossible d'imprimer !"));
			  msgBox.setStandardButtons(QMessageBox::Ok);
			  msgBox.exec();
			  return -1;
			}
      }
   }


/*
  line  = strtok((char *)txt, "\n");
  if (line == NULL) {
    text = txt;
    m_painter.drawText(1, 1, text);
  }
  else 
  {
    while (line) 
	{
      int height = i * (fontMetrics.height() + fontMetrics.leading());
      text = line;
      m_painter.drawText(5, height, text);
      line = strtok(NULL, "\n");
      i ++;   k++;
      if (height > rect.height()) 
	  {
		k = 0; i = 0;
	
			if (m_printer.newPage() == false) 
			{
			  m_painter.end();
			  QMessageBox msgBox(m_parentWidget);
			  msgBox.setText(QString::fromUtf8("Impossible d'imprimer !"));
			  msgBox.setStandardButtons(QMessageBox::Ok);
			  msgBox.exec();
			  return -1;
			}
      }
    
	
	}
  }
*/
  m_painter.end();
 
  QMessageBox msgBox(m_parentWidget);
  msgBox.setText(QString::fromUtf8("Impression envoyee a l'imprimante!"));
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
  
  return 0;
};
  
