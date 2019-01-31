#ifndef SHEETSMODEL_H
#define SHEETSMODEL_H

#include <QMediaPlayer>
#include "googlewrapper.h"
#include "listener.h"
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
    QList<QStringList*> mksAtd; //LIST OF MARKS
    QString sheetId;
    bool isLessonStarted = false;

    int dataSize;

    const QStringList months = {"Январь","Февраль","Март","Апрель",
                                "Май","Июнь","Июль","Август",
                                    "Сентябрь","Октябрь","Ноябрь","Декабрь"};
    const QString stMA = "D7"; //ADRESS OF STARTING MONTH
    const QString stDA = "D8"; //ADRESS OF STARTING DATE

    void colInd();   //SHEET COLUMN INDEXES
    void makeClassesRepresent(); //Prepare list of subjects & classes
    void downloadData(); //Get Arrays of Students Marks & Attendances
    void getNames(); //get students names and surnames
    void googleSay(QString phrase); //MAKE SPEECH
    void prepareGrid();
    void getDates();



    QString getSex(QString name); //get students sex: can be "M" or "F"
    QString setAva(int i);// CREATE AVATAR FOR INDEX I

    int monthNum(QString startMonth);

    QJsonArray readJSONArray(QNetworkReply* reply); //READS JSON ROWS DATA

    enum {
        UP = 16736925,
        DOWN = 16754775,
        OK = 16712445,
        LEFT = 16720605,
        RIGHT = 16761405,
        TWO = 16750695,
        THREE = 16756815,
        FOUR = 16724175,
        FIVE = 16718055
    };



private slots:
    void getclasses(); //PROCEDURE TO GET classNames FIELD
    void btnHandler(int btnId); //HANDLE BUTTON EVENT
    void irHandler(int irCode); //HANDLE IR EVENT
    void playAudio();
//    void callStudent(); //MAKE A DECISION FROM PROBABIBILITIES LIST
    void updateTotalProb(); //UPDATE PROBABILITIES FOR ALL STUDENTS
    void updateCurrentProb(); //UPDATE PROBABILITIES FOR CURRENTLY ACTIVE STUDENTS


signals:
    void error(const QString &errorString);
    void dataGot(QString, QStringList);//EMIT WHEN CLASSES GOT OR STUDENTS COMPLETED!!!
    void gridPrepared();
    void audioReady();


public slots:
    void grant(); //was public method
    void classSelected(int classId);
};

#endif // SHEETSMODEL_H
