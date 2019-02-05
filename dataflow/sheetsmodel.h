#ifndef SHEETSMODEL_H
#define SHEETSMODEL_H

#include <QMediaPlayer>
#include <QRandomGenerator64>
#include "api/googlewrapper.h"
#include "server/listener.h"
#include "studentsflow.h"


// THIS STRUCTURE DEFINES DATA FROM THE

struct ClassList{
    QString subject, className, sheetId;
};

class SheetsModel : public QObject
{
    Q_OBJECT


public:
    explicit SheetsModel(QObject *parent = nullptr);

    StudentsFlow studentsflow;

private:

    GoogleWrapper googlewrapper;
    Listener listener;

    QMediaPlayer player; //PLAY SOUNDS
    QBuffer audioStream; //BUFFER TO PLAY
    QByteArray bytesOfSound; //AUDIO DATA

    QRandomGenerator xCase;
    QList<ClassList*> classNames; //LIST OF STRUCTURES CLASSLIST
    QStringList columnIndexes; //LIST OF COLUMN INDEXES
    QStringList classesRepresent; //LIST OF SUBJS&CLASSES FOR INTERFACE
    QStringList students; //LIST OF STUDENTS
    QStringList dates; //LIST OF DATES
    QList<int> mksCount; //LIST OF MARKS COUNT
    QList<int> studDays; //DAYS WITHOUT SKIP
    QList<double> avMark; //AVERAGE MARK
    QList<float> attend; //ATTENDANCE PERCENT
    QList<int> actions; //COUNT OF ACTIONS --> PERMANENTLY 0
    QList<bool> isActive; //STUDENT IS ON SCREEN OR NOT --> PERMANENTLY FALSE
    QList<bool> isOn; //IS STUDENT ATTENDING THE LESSON
    QList<bool> isMain; //IS STUDENT CURRENTLY MAIN
    QList<double> probability; //PROBABILITY OF ASKING STUDENT: SEE MATHEMATICAL MODEL
    QList<double> decisionList; //PROB [ STUD_id ] OF MAX RANGES VALUES IN [0,1]
    QList<double> currentProbability; //PROBABILITY AT CURRENT QUESTION
    QList<double> currentDecisionList; //PROB [ STUD_id ] OF MAX RANGES VALUES IN [0,1]
    QList<int> order; //TIME ORDERING OF ACTIVE STUDENTS
    QStringList names; //LIST OF STUDENT'S NAMES
    QStringList surnames; //LIST OF STUDENT'S SURNAMES
    QStringList sex; //STUDENT'S SEX
    QStringList plumin; //PLUSES AND MINUSES
    QString curClass; //CURRENT CLASS
    QString curCol; //CURRENT COLUMN INDEX
    QList<QStringList*> mksAtd; //LIST OF MARKS AND ATTENDANCE (SHEETS DATA)
    QString sheetId; //ID OF DATA SHEET
    QString pmColumn; // COLUMN OF PLUSES AND MINUSES IN SHEETS

    bool locked = false; // BLOCK BUTTONS
    bool isLessonStarted = false;
    bool grading = false; // TRUE IF TEACHER GRADING MAIN STUDENT
    int gradeId = 99; // ID OF STUDENT TO GET MARK/PM
    // IF THERE IS ONLY ONE MAIN STUDENT GRADE ID = 99

    int dataSize; //DATA LENGTH FOR SINGLE STUDENT SINCE THE BEGINNING OF SEMESTER
    int studentsCount = 0; //FOR INITIALIZATION PURPOSES

    const QStringList months = {"Январь","Февраль","Март","Апрель",
                                "Май","Июнь","Июль","Август",
                                    "Сентябрь","Октябрь","Ноябрь","Декабрь"};
    const QString stMA = "D7"; //ADRESS OF STARTING MONTH
    const QString stDA = "D8"; //ADRESS OF STARTING DATE
    const int startId = 11; //START RAW NUMBER OF DATA

    void colInd();   //SHEET COLUMN INDEXES
    void makeClassesRepresent(); //Prepare list of subjects & classes
    void downloadData(); //Get Arrays of Students Marks & Attendances
    void getNames(); //get students names and surnames
    void googleSay(QString phrase); //MAKE SPEECH
    void prepareGrid();
    void callStudent(); //MAKE A DECISION FROM PROBABIBILITIES LIST
    void getDates(int lessonNumber); //LESSON NUMBER 0 - NOT AT DATE, 1 - 1st LESSON, 2 - 2nd etc
    void addPM(QString plusmin, int id); //ADD PLUS OR MINUS
    void updatePM(int id); //UPDATE STUDENT'S +- IN SHEETS
    void markUpdate(int mark, int id); //MARK TO STUDENT WITH ID
    void absent(); // ABSENT STUDENTS PASS TO SHEET "п" BY DEFAULT
    void clearNotMain(); // SET ALL NOTMAIN STUDENTS TO INACTIVE STATE
    void updateTotalProb(); //UPDATE PROBABILITIES FOR ALL STUDENTS
    void updateCurrentProb(); //UPDATE PROBABILITIES FOR CURRENTLY ACTIVE STUDENTS

    int sinceLastMark(int id, int mark); // RETURN BY ID NUMBER OF LESSONS SINCE LAST MARK
    int sinceLastMark(int id); // RETURN BY ID SINCE ANY LAST MARK
    int coinToss(QList<double>); //RETURN STUDENT'S ID TO ASK FROM DECISION LIST

    const SingleStudent createSingle(int id); //CREATE A SINGLE STUDENT BY ID

    QString getSex(QString name); //get students sex: can be "M" or "F"
    QString setAva(int i);// CREATE AVATAR FOR INDEX I

    int monthNum(QString startMonth);

    QJsonArray readJSONArray(QNetworkReply* reply); //READS JSON ROWS DATA

    // IR REMOTE CONTROL CODES FROM HEX TO DEC
    enum {
        UP = 16736925,
        DOWN = 16754775,
        OK = 16712445,
        LEFT = 16720605,
        RIGHT = 16761405,
        ONE = 16738455,
        TWO = 16750695,
        THREE = 16756815,
        FOUR = 16724175,
        FIVE = 16718055,
        ZERO = 16730805,
        STAR = 16728765
    };



private slots:
    void getclasses(); //PROCEDURE TO GET classNames FIELD
    void btnHandler(int btnId); //HANDLE BUTTON EVENT
    void irHandler(int irCode); //HANDLE IR EVENT
    void playAudio();



signals:
    void error(const QString &errorString);
    void dataGot(QString, QStringList);//EMIT WHEN CLASSES GOT OR STUDENTS COMPLETED!!!
    void gotCount(int studentsCount); // EMIT WHEN GOT COUNT OF STUDENTS
    void gridPrepared();
//    void audioReady();
    void studentSelected(int studentId);


public slots:
    void grant(); //was public method
    void classSelected(int classId, int lessonNum);
};

#endif // SHEETSMODEL_H
