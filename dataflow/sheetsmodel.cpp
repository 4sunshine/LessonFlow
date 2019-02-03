#include "sheetsmodel.h"

SheetsModel::SheetsModel(QObject *parent) : QObject(parent),
  studentsflow(),
  googlewrapper(),
  listener()
{
    colInd();
    grant();
    connect(&listener, &Listener::buttonEvent, this, &SheetsModel::btnHandler);
    connect(&listener, &Listener::irSignal, this, &SheetsModel::irHandler);
//    connect(this,&SheetsModel::audioReady,this,&SheetsModel::playAudio);
}

void SheetsModel::grant()
{
    googlewrapper.grant();
    connect(&googlewrapper, &GoogleWrapper::authenticated, this, &SheetsModel::getclasses);
}

void SheetsModel::getclasses()
{
    auto reply = googlewrapper.getClassesList();

    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() != QNetworkReply::NoError) {
            emit error(reply->errorString());
            return;
        }
        const QJsonArray mName = readJSONArray(reply);

        for (int i = 0 ; i < mName.size(); i++)
            {
                ClassList* pp =
                        new ClassList{mName[i][0].toString(),
                        mName[i][1].toString(), mName[i][2].toString()};//CREATE POINTER OF CLASS LIST
                this->classNames.append(pp);
            }

    makeClassesRepresent();
    emit dataGot("models",classesRepresent); //CLASSES GOT
    });
}

void SheetsModel::colInd()
{
    //COLUMN AA INDEXES CREATION. 26 ALPHABET LETTERS. 65 - START POSITION IN ASCII
    QStringList addr = {};
    for(int i=0; i<=100; i++){
        QString str2 = QByteArray::fromHex(QString::number(65+(i%26),16).toLocal8Bit());
        if (i<26) {
            addr<<str2;
        }
        else {
            QString str1 = QByteArray::fromHex(QString::number(64+(i/26),16).toLocal8Bit());
            addr<<str1+str2;
        }
    }
    columnIndexes = addr;
}

void SheetsModel::makeClassesRepresent()
{
    for (int i=0; i < classNames.size(); i++)
        classesRepresent<<(classNames[i]->subject+' '+classNames[i]->className);
}

void SheetsModel::classSelected(int classId, int lessonNum)
{
    qInfo()<<classId;
    sheetId = classNames[classId]->sheetId;
    curClass = classNames[classId]->className;
    googlewrapper.sheetId = sheetId;
    auto reply = googlewrapper.requestForStudents();
    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() != QNetworkReply::NoError) {
            emit error(reply->errorString());
            return;
        }
        const QJsonArray classes = readJSONArray(reply);
        for (int i = 0 ; i < classes.size(); i++)
            {
                students << classes[i][0].toString();   //APPENDING STUDENTS
                actions << 0;  //APPENDING STATUSES OF ACTIONS
                isActive << false; //
                isOn << false;
                isMain << false;
                names << classes[i][0].toString().section(' ',1,1); //APPENDING NAMES
                surnames << classes[i][0].toString().section(' ',0,0); //APPNG SURNAMES
                sex << getSex(names[i]); //SETTING SEX
            }
        googlewrapper.studCount = students.length();

        emit gotCount(googlewrapper.studCount); //EMIT THIS TO SET OPTIMAL SIZE OF CELLS
/*
        qInfo()<<students.length();
        qInfo()<<students;
        qInfo()<<sex;
        qInfo()<<names;
        qInfo()<<lessonNum;
*/
        getDates(lessonNum);
    });
}

void SheetsModel::getDates(int lessonNum)
{
    auto replyd = googlewrapper.getDateList();

    connect(replyd, &QNetworkReply::finished, [=]() {
        if (replyd->error() != QNetworkReply::NoError) {
            emit error(replyd->errorString());
            return;
        }
        const QJsonArray datMo = readJSONArray(replyd); //

        //CHECK FOR COLUMN WITH PLUSES AND MINUSES:

        if(datMo[datMo.size()-1][0].toString() != "PLUMIN") {
            auto reply2 = googlewrapper.updateSheet("PLUMIN",
                                      columnIndexes[datMo.count()+3],7);//SEE SHEET STRUCTURE
            connect(reply2, &QNetworkReply::finished, [=]() {
                if (reply2->error() != QNetworkReply::NoError) {
                    emit error(reply2->errorString());
                    return;
                }
             });
        }

        //IF PLUMIN IS OK: GET PLUSES AND MINUSES

        else if (datMo[datMo.size()-1][0].toString() == "PLUMIN")
        {
            qInfo()<<"getplumines";

            auto reply3 = googlewrapper.getPM(columnIndexes[datMo.count()+2]); //SEE SHEET STRUCTURE

            pmColumn = columnIndexes[datMo.count()+2]; //INITIALISE +- COLUMN

            connect(reply3, &QNetworkReply::finished, [=]() {
                if (reply3->error() != QNetworkReply::NoError) {
                    emit error(reply3->errorString());
                    qInfo() << "PLUMIN ERROR";
                    return;
                }
                qInfo() << "PLUMIN SUCCESS";
                const QJsonArray dataPM = readJSONArray(reply3);

                for (int i = 0; i < dataPM.size();i++) //READING PLUMINS TO CLASS
                    plumin << dataPM[i][0].toString();

                while (plumin.length() < students.length()) //FULLFILLING PLUMINS ARRAY
                    plumin << "";

                qInfo() << plumin;

            });
        }
        QDateTime curDate;
        int moN = curDate.currentDateTime().date().month();
        int daY = curDate.currentDateTime().date().day();
/*********POSSIBLE BUG IF LESSON EARLIER THAN 1st LESSON OF MONTH. MUST CHECK************/
        int i = 0;

        while(!datMo[i][0].isUndefined()&&(months[moN-1] != datMo[i][0].toString())){
            i++;
        }

        while(!datMo[i][1].isUndefined()&&(daY > datMo[i][1].toString().toInt())){
            i++;
        }
/***********************************************************************************/
        //RECHECK DATASIZE!!!
        dataSize = i + lessonNum; //LENGTH OF DATA WITH MARKS & ATTENDANCE
        qInfo() << "CURRENT COLUMN UNDER";
        curCol = columnIndexes[i+2+lessonNum]; //BECAUSE OF COLUMN INDEXES OF MARKS DATA
        //ALSO IF THERE ARE SEVERAL LESSONS AT THE SAME DATE
        qInfo() << curCol;
        googlewrapper.curCol = curCol;

        downloadData(); //DOWNLOAD MARKS & ATTENDANCE
    });
}

void SheetsModel::downloadData()
{
        auto reply = googlewrapper.downloadData();
        connect(reply, &QNetworkReply::finished, [=]() {
            if (reply->error() != QNetworkReply::NoError) {
                emit error(reply->errorString());
                return;
            }
            const QJsonArray data = readJSONArray(reply);

            for (int i = 0; i < students.length(); i++){
                QStringList locData; //TEMPORARY STRING LIST
                int locnt = 0; //TEMP FOR COUNT OF MARKS
                int sum = 0; //TEMP FOR SUM OF MARKS
                int sdays = 0; //TEMP FOR SDAYS
                int lastsk = -1; //TEMP FOR LAST SKIPPED LESSON
                for (int j = 0; j < dataSize; j++){
                    if (j < data[i].toArray().size()) {
                        locData << data[i][j].toString();
                        if (data[i][j].toString().isLower()){
                            sdays++;
                            lastsk = j;
                        }
                        else if (!data[i][j].toString().isUpper() &&
                                !data[i][j].toString().isEmpty()){
                            sum += data[i][j].toString().toInt();
                            locnt++;
                        }
                    }
                    else {
                        locData << "";
                    }
                }

                mksCount << locnt;
                studDays << (dataSize - lastsk - 1);

                if (locnt!=0){
                    avMark << double(sum)/double(locnt);
                }

                else
                    avMark << double(0);

                attend << float(100)*float(dataSize-sdays)/float(dataSize);

                QStringList* locD = new QStringList(locData);
                mksAtd << locD; //MARKS AND ATTENDANCE

                probability << double(0); //INITIALIZE PROBABILITIES LISTS
                decisionList << double(0);
                currentProbability << double(0);
                currentDecisionList << double(0);

            }

            prepareGrid(); //PREPARE GRID AT START
            listener.isActive = true; //start receive data from server
            emit gridPrepared();
//*******************************************************************
            googleSay("Привет, ребята! Отметьтесь на уроке");

            emit dataGot("popups",students);//STUDENTS COMPLETED SIGNAL

        });
}

QString SheetsModel::getSex(QString name)
{
    const QStringList maleNames = {"Александр","Данила","Даниил","Сергей","Евгений",
                               "Матвей","Дмитрий","Илья","Захар","Фёдор",
                               "Артём","Роман","Пётр","Алексей","Иван",
                               "Вадим","Владимир","Андрей","Денис","Максим",
                                  "Леонид","Артемий","Владислав","Григорий",
                                  "Томас","Лев","Марк","Игорь","Егор","Кирилл",
                                  "Матвей"};
    int i=0;
    while(maleNames[i]!=name){
        i++;
        if (i==maleNames.length())
            break;
         }
    if (i==maleNames.length()){
        return "F";
    }
    else
        return "M";
}

void SheetsModel::btnHandler(int btnId)
{
    if (btnId > students.length()) //CHECK IF ID <= LENGTH OF STUDENTS
        return;

    /*STARTING CHECK OF STUDENTS STATUS*/

    if ( !isOn[btnId-1] ){

        isOn[btnId-1] = !isOn[btnId-1];

        if ( !isLessonStarted ){
            studentsflow.setIsOn(btnId-1);
       }
        else {
            updateTotalProb(); //WHEN A NEW STUDENT CAME DURING LESSON
       }

    }

    /*BUTTON EVENTS DURING LESSON*/
    if (isLessonStarted && isOn[btnId-1] && !grading) {
        isActive[btnId-1] = !isActive[btnId-1];
        if (isActive[btnId-1])
        {
            studentsflow.addStudent(createSingle(btnId-1));// IS MAIN FALSE PERMANENTLY
            order << btnId-1;
            updateCurrentProb();
        }
        else {
            order.removeAt(order.indexOf(btnId-1));
            updateCurrentProb();

            if(!isMain[btnId-1]){
                studentsflow.removeStudent(btnId-1);
            }
            else {
                grading = true;
                gradeId = btnId-1;
                // SOUND OR NOTIFICATION OF +- OR MARK NECESSITY
            }

        }

    }

    //DISCUSSIVE POINT!!! ACTIVITY ++ EVERY BTN EVENT

}

void SheetsModel::irHandler(int irCode)
{
    int mcount = isMain.count(true); //COUNT OF MAIN STUDENTS

    int mId; //ID OF CURRENTLY GRADING STUDENT

    if (gradeId == 99){
        mId = isMain.indexOf(true);
    }
    else {
        mId = gradeId;
    }

    switch (irCode) {
        case UP:
            if (isLessonStarted)
                callStudent();

            break;
        case DOWN: // END OF GRADING
            if (mcount > 0){
                updatePM(mId);
                studentsflow.removeStudent(mId);
                isMain[mId] = false;
                gradeId = 99;
                grading = false;
            }

            break;
        case RIGHT:
            if (isLessonStarted && (mcount > 0))
                addPM("+", mId);

            break;
        case LEFT:
            if (isLessonStarted && (mcount > 0))
                addPM("-", mId);

            break;
        case OK:
            if (!isLessonStarted){
                studentsflow.removeAll();
                updateTotalProb();
                isLessonStarted = true;
                order.clear();
                qInfo() << probability;
                qInfo() << decisionList;
            }
            else if ( mcount > 0 ) {
                qInfo() << "LESSON IS STARTED";
            }

            break;
        case TWO:
            if (isLessonStarted && (mcount > 0))
                markUpdate(2, mId);

            break;
        case THREE:
            if (isLessonStarted && (mcount > 0))
                markUpdate(3, mId);

            break;
        case FOUR:
            if (isLessonStarted && (mcount > 0))
                markUpdate(4, mId);

            break;
        case FIVE:
            if (isLessonStarted && (mcount > 0))
                markUpdate(5, mId);

            break;

        default:
            break;
    }


}

QJsonArray SheetsModel::readJSONArray(QNetworkReply *reply) //MODULE FOR READ JSON
{
        const auto json = reply->readAll();
        const auto document = QJsonDocument::fromJson(json);
        QString strJson(document.toJson(QJsonDocument::Compact));
        const auto rootObject = document.object();
        const auto dataValue = rootObject.value("range");
        QJsonArray mName = rootObject.value("values").toArray();
        return mName;
}

void SheetsModel::prepareGrid()
{
    int i = 0;
    while(i < students.length())
    {
        studentsflow.addStudent(createSingle(i));
        i++;
    }
}

QString SheetsModel::setAva(int i)
{
    QFile realAva;
    realAva.setFileName(QCoreApplication::applicationDirPath()+"/avatar/"+
                        curClass+"/"+QString::number(i+1)+".png");
    if (!realAva.exists()){
        int rNum = rand() % 23;
        realAva.setFileName(QCoreApplication::applicationDirPath()+"/avatar/"+
                            sex[i]+"/"+QString::number(rNum)+".png");
    }
    if (realAva.exists())
        return QString("file:/")+realAva.fileName();//file WAS INSTEAD OF IMAGE
    else {
        return QString("qrc:/ui/graphics/path868white.png");
    }
}

void SheetsModel::playAudio()
{
    audioStream.close();
    audioStream.setData(bytesOfSound);
    audioStream.open(QIODevice::ReadOnly);
    player.setMedia(QMediaContent(),&audioStream);
    player.setVolume(99);
    player.play();
//CONNECTION BTW PLAYER.STOPPED AND AUDIOSTREAM.CLOSE MUST BE IMPLEMENTED
    qInfo() << "PLAY SOUND";
}

void SheetsModel::googleSay(QString phrase)
{
    bytesOfSound.clear();
    auto replygo = googlewrapper.speechGet(QString::fromUtf8(phrase.toUtf8()));
    connect(replygo, &QNetworkReply::finished, [=]() {
        if (replygo->error() != QNetworkReply::NoError) {
            emit error(replygo->errorString());
//            qInfo()<<replygo->errorString();
//            qInfo()<<replygo->readAll();
            return;
        }
        const auto json = replygo->readAll();
        const auto rootObject = QJsonDocument::fromJson(json).object();
        QByteArray zzz = rootObject.value("audioContent").toString().toUtf8();
        bytesOfSound = QByteArray::fromBase64(zzz);
        playAudio();
    });
}

void SheetsModel::updateTotalProb()//ALPHA!!! DISCUSSIVE POINT!!! ADD 0.5 INCREASE OF MARK!!!
{
    listener.isActive = false;
    int onlineCount = 0;
    int sumCountOfMarks = 0; //SUM OF COUNT OF MARKS OF EACH STUDENT
    int sumStudDays = 0; //SUM OF DAYS WITHOUT SKIP
    int maxStud = 0; //MAX OF DAYS WITHOUT SKIP
    int maxCount = 0; //MAX OF MARKS COUNT
    QList<double> countProb; //LIST OF MARKS COUNT PROBABILITIES
    QList<double> studDaysProb; //LIST OF STUD DAYS PROBABILITIES

    for (int i = 0; i < mksCount.count(); i++) {
        countProb << double(0);
        studDaysProb << double(0);

        if ( isOn[i] )
        {
            onlineCount ++;
            probability[i] = 0;
            decisionList[i] = 0;

            if (studDays[i] > maxStud)
                maxStud = studDays[i];

            if (mksCount[i] > maxCount)
                maxCount = mksCount[i];
        }
    }

    for (int i = 0; i < mksCount.count(); i++) {
        if ( isOn[i] )
        {
            sumStudDays += (maxStud - studDays[i] + 1);
            sumCountOfMarks += (maxCount - mksCount[i] + 1);
        }
    }

    for (int i = 0; i < mksCount.count(); i++) {
        if ( isOn[i] )
        {
            if ( maxStud > 0 )
                studDaysProb[i] = double(maxStud - studDays[i] + 1) / double(sumStudDays);
            else {
                studDaysProb[i] = double(1)/ double(onlineCount);
            }

            if ( maxCount > 0 )
                countProb[i] = double(maxCount - mksCount[i] + 1) / double(sumCountOfMarks);
            else {
                countProb[i] = double(1)/ double(onlineCount);
            }

            //***********MATHEMATICAL MODEL OF MAKING DECISIONS**********************//
            // ALPHA VERSION!!!: [P(COUNT) + P(STUDDAYS)] / 2

            probability[i] = .5*(countProb[i] + studDaysProb[i]);

            if ( i > 0 ) //MAKE DECISION LIST: [0.05 , 0.09, 0.10 etc..]
                decisionList[i] = probability[i] + decisionList[i-1];
            else {
                decisionList[i] = probability[i];
            }
        }
        // IF STUDENT IS NOT ATTENDING THE LESSON
        else {
            if ( i > 0)
                decisionList[i] = decisionList[i-1];
            else {
                decisionList[i] = double(0);
            }
        }

    }

    listener.isActive = true;

}

void SheetsModel::updateCurrentProb()
{
    listener.isActive = false;

    double sumCurP = 0; //SUM OF CURRENT PROBABIBILITIES
    QList<double> orderProb; //TIME ORDERING PROBABILITY
    int orderSum = 0; //SUM FOR ORDER PROBABILIBTY

    for (int i = 0; i < mksCount.count(); i++) {
        currentProbability[i] = double(0);
        currentDecisionList[i] = double(0);
        orderProb << double(0);

        if ( isActive[i] )
        {
            orderSum += double(order.count() - order.indexOf(i));
            sumCurP += probability[i];
        }
    }

    for (int i = 0; i < mksCount.count(); i++) {
        if ( isActive[i] ) {
            currentProbability[i] = .5*(double(order.count()-order.indexOf(i))/orderSum
                                        +probability[i]/sumCurP);
            if ( i > 0 )
                currentDecisionList[i] = currentDecisionList[i-1]+currentProbability[i];
            else {
                currentDecisionList[i] = currentProbability[i];
            }

        }
        else {
            if ( i > 0 )
                currentDecisionList[i] = currentDecisionList[i-1];
        }
    }

    listener.isActive = true;

}

void SheetsModel::callStudent()
{
    listener.isActive = false;

    //TO PREVENT 1 STUDENT CALL EVERYTIME THIS FUNCTION MUST BE REVISED
    //IF THERE ARE NO STUDENTS TO ANSWER CASE 1, ELSE CASE 2

    if (order.isEmpty()){
        int id = coinToss(decisionList);
        googleSay("Отвечает "+names[id]+" "+surnames[id]);
        isMain[id] = !isMain[id];
        SingleStudent bounded = createSingle(id);
        studentsflow.addStudent(bounded);
        studentsflow.setMain();
        emit studentSelected(id); //REVISE IS IT NEEDED?
    }
    else {
        int id = coinToss(currentDecisionList);
        googleSay("Отвечает "+names[id]+" "+surnames[id]);
        isMain[id] = !isMain[id];
        order.removeAt(order.indexOf(id));
        studentsflow.activeStudent(id);
        emit studentSelected(id); //REVISE IS IT NEEDED?
    }

    listener.isActive = true;
}

int SheetsModel::coinToss(QList<double> decList)
{
    double toss = double(1) - xCase.generateDouble(); //BECAUSE OF [0,1) RANGE OF GENERATED NUMBER
    qInfo() << toss;
    qInfo() << "toss result of:";
    int i = 0;
    while ((toss > decList[i])&&( i < decList.count())){
        i++;
    }
    qInfo() << i;
    return i;
}

const SingleStudent SheetsModel::createSingle(int id)
{
    QString averageString;
    double avmCache = avMark[id];

    return SingleStudent(id,names[id],surnames[id],plumin[id],
                  averageString.sprintf("%6.2f", avmCache),
                  setAva(id),isOn[id],isMain[id]);
}

void SheetsModel::addPM(QString plusmin, int id)
{
    plumin[id] += plusmin;
    if ( plumin[id].count() == 3 ) {
        int pcount = plumin[id].count("+"); //COUNT OF PLUSES IN STRING
        switch (pcount) {
        case 3:
            markUpdate(5, id);
            plumin[id]="";
            break;
        case 2:
            markUpdate(4, id);
            plumin[id]="";
            break;
        case 1:
            markUpdate(3, id);
            plumin[id]="";
            break;
        case 0:
            markUpdate(2, id);
            plumin[id]="";
            break;
        default:
            break;
        }
    }
}

void SheetsModel::markUpdate(int mark, int id)
{
    avMark[id] = (avMark[id]*mksCount[id]+double(mark))/double(mksCount[id] + 1);
    mksCount[id]++;
    googleSay(" "+names[id]+" получает "+QString::number(mark));
    int lastfree = mksAtd[id]->lastIndexOf(""); //LAST INDEX WITH FREE VALUE
    (*mksAtd[id])[lastfree] = QString::number(mark);

    auto updateReply = googlewrapper.updateSheet(QString::number(mark),
                                                 columnIndexes[lastfree+3], startId+id);
    connect(updateReply, &QNetworkReply::finished, [=]() {
        if (updateReply->error() != QNetworkReply::NoError) {
            emit error(updateReply->errorString());
            qInfo()<<updateReply->errorString();
            return;
        }
    });
}

void SheetsModel::updatePM(int id)
{
    auto updateReply = googlewrapper.updateSheet(plumin[id], pmColumn, id+startId);
    connect(updateReply, &QNetworkReply::finished, [=]() {
        if (updateReply->error() != QNetworkReply::NoError) {
            emit error(updateReply->errorString());
            qInfo()<<updateReply->errorString();
            return;
        }
    });
}
