#ifndef   	PARAMETERS_HH_
# define   	PARAMETERS_HH_

#include <QtGui>

class VegaParameters 
{
public:
  VegaParameters();
  ~VegaParameters();
  
  void setReadPassword(const char *password);
  void setReadWritePassword(const  char* password);
  bool compareToReadPassword(const  char *input);
  bool compareToReadWritePassword(const  char *input);
  char *getReadPassword();
  char *getReadWritePassword();
  bool saveParametersToFile(const char*fileName);
  bool loadParametersFromFile(const char*fileName);
  bool isValid();
private:
  bool comparePasswords(unsigned char *input, unsigned  char *input2);
  void strToMD5Sum(const char *str, unsigned char *md5sum);
  void md5sum(const unsigned char *input, unsigned char *output);
  unsigned char m_readPassword[16];
  unsigned char m_readWritePassword[16];
  bool m_isValid;
};

class KeyPressEater : public QObject
{
	Q_OBJECT
protected:
	bool eventFilter(QObject *obj, QEvent *event);
};
 

//class Calculator : public QWidget
class Calculator : public QDialog
{
Q_OBJECT
public:
	Calculator(QWidget *parent = 0);
	bool m_readOnly;  
protected:
	void closeEvent(QCloseEvent* event);
	bool event(QEvent* event);
	//bool eventFilter(QObject *target, QEvent *event);
	
	QLineEdit* lineEditAuthenticatePassword;
	QComboBox* comboUser;
	QLabel* lablResponse;
	bool m_passwordIsCorrect;

public slots:
	bool authenticateUser();

};

#endif	    /* !PARAMETERS_HH_ */
