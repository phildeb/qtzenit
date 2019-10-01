#ifndef   	SWITCH_HH_
# define   	SWITCH_HH_


typedef enum {
  BOARD_STATE_ANOMALY = 0,
  BOARD_STATE_OK,
  BOARD_STATE_ABSENT
} board_state_t;


typedef enum {
  BOARD_ASLT = 0,
  BOARD_AGA,
  BOARD_ALPHACOM,
  BOARD_CLOCK
} board_type_t;

#define BOARD_AGA_STR		"AGA"
#define BOARD_ASLT_STR		"ASLT"
#define BOARD_ALPHACOM_STR	"ALPHACOM"
#define BOARD_CLOCK_STR		"CLOCK"


class Board {

public:
  Board(){
	  this->m_place = -1;
	  this->m_present = false;
  }
  Board(int place, bool present, board_type_t type){  this->m_place = place;
	  this->m_present = present;
	  this->m_type = type;
	}

  void setState(board_state_t state){m_state = state;}
  board_state_t getState(){return this->m_state;}
  
  void addConferenceDependence(int conference_number);
  void addDeviceDependence(int device_number);

  bool isDeviceDependant(int device_number);
  bool isConferenceDependant(int conference_number);
  
  int getPlace(){return this->m_place;}
  void setPlace(int place){this->m_place = place;}
  
  static QList<Board*> *loadSwitchConfigFromFile();
  static int saveSwitchConfigToFile(QList<Board*> *boards);

  static const char *typeToStr(board_type_t type);

  board_type_t getType(){return this->m_type;}
  void setType(board_type_t type){this->m_type = type;}

  void setPresent(bool enabled){this->m_present = enabled;}
  const char *dump();

  bool isPresent(){return this->m_present;}

  QList<int> * getConferenceDependencies();
  QList<int> * getDeviceDependencies();

//private:

  board_type_t m_type;
  int m_place;
  bool m_present;
  board_state_t m_state;
  QString m_description;
  QList<int> m_devicesDependencies;
  QList<int> m_conferencesDependencies;
};

extern QList<Board*> *pBoards;

#endif	    /* !SWITCH_HH_ */


