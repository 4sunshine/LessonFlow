#include "sheetsmodel.h"

SheetsModel::SheetsModel(QObject *parent) : QObject(parent),
  studentsflow(),
  googlewrapper(),
  listener(),
  m_plus(this),
  m_minus(this)
{
    /*SOUND EFFECTS BLOCK*/
    m_plus.setSource(QUrl("qrc:/sounds/plus0.wav"));
    m_plus.setVolume(0.9);
    m_minus.setSource(QUrl("qrc:/sounds/minus0.wav"));
    m_minus.setVolume(0.9);
    /*SOUND EFFECTS BLOCK*/

    colInd();
    grant();
    connect(&listener, &Listener::buttonEvent, this, &SheetsModel::btnHandler);
    connect(&listener, &Listener::irSignal, this, &SheetsModel::irHandler);
//    connect(this,&SheetsModel::audioReady,this,&SheetsModel::playAudio);
    xCase.seed(uint32_t(QDateTime::currentMSecsSinceEpoch()));
}

void SheetsModel::grant()
{
    googlewrapper.grant();
    connect(&googlewrapper, &GoogleWrapper::authenticated, this, &SheetsModel::getclasses);
}

void SheetsModel::getclasses()
{
    QNetworkReply * reply = googlewrapper.getClassesList();

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
    QNetworkReply * reply = googlewrapper.requestForStudents();
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

        studentsCount = students.length();

        emit gotCount(studentsCount); //EMIT THIS TO SET OPTIMAL SIZE OF CELLS

        getDates(lessonNum);
    });
}

void SheetsModel::getDates(int lessonNum)
{
    QNetworkReply * replyd = googlewrapper.getDateList();

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

                while (plumin.length() < studentsCount) //FULLFILLING PLUMINS ARRAY
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
        QNetworkReply * reply = googlewrapper.downloadData();
        connect(reply, &QNetworkReply::finished, [=]() {
            if (reply->error() != QNetworkReply::NoError) {
                emit error(reply->errorString());
                return;
            }
            const QJsonArray data = readJSONArray(reply);

            for (int i = 0; i < studentsCount; i++){
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
    listener.isActive = false;
    if ((btnId > studentsCount) || (btnId <= 0) ) { //CHECK IF ID <= LENGTH OF STUDENTS
        listener.isActive = true;
        return;
    }

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
    if (isLessonStarted && isOn[btnId-1] && !grading && !locked) {

        if (!isActive[btnId-1]){
            isActive[btnId-1] = true;
            studentsflow.addStudent(createSingle(btnId-1));// IS MAIN FALSE PERMANENTLY
            order << btnId-1;
            updateCurrentProb();
        }
        else {
            if (!isMain[btnId-1]){
                isActive[btnId-1] = false;
                studentsflow.removeStudent(btnId-1);
                order.removeAt(order.indexOf(btnId-1));
                updateCurrentProb();
            }
            else {
                googleSay( names[btnId-1]+" хочет оценку" );
                grading = true;
                gradeId = btnId-1;
                isActive[btnId-1] = false;
                // SOUND OR NOTIFICATION OF +- OR MARK NECESSITY
            }
        }
    }

    //DISCUSSIVE POINT!!! ACTIVITY ++ EVERY BTN EVENT
    listener.isActive = true;
}

void SheetsModel::irHandler(int irCode)
{
    listener.isActive = false;

    int mcount = isMain.count(true); //COUNT OF MAIN STUDENTS

    int mId; //ID OF CURRENTLY GRADING STUDENT

    if (gradeId == 99){

        mId = studentsflow.getLast();

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
            if (mId >= 0){
                updatePM(mId);
                studentsflow.removeStudent(mId);
                isActive[mId] = false;
                isMain[mId] = false;
                gradeId = 99;
                grading = false;
            }

            break;
        case RIGHT:
            if (isLessonStarted && (mId >= 0)){
                m_plus.play();
                addPM("+", mId);
            }

            break;
        case LEFT:
            if (isLessonStarted && (mId >= 0)){
                m_minus.play();
                addPM("-", mId);
            }

            break;
        case OK:
            if (!isLessonStarted) {
                studentsflow.removeAll();
                updateTotalProb();
                isLessonStarted = true;
            }
            else {
                studentsflow.removeAll();
                clearNotMain();
            }

            break;

        case ONE:
            if (isLessonStarted)
                locked = !locked;

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

        case ZERO:
            if (isLessonStarted)
                absent();

            break;

        case STAR:
            if (isLessonStarted){
                clearNotMain();
                studentsflow.removeAll();
                callStudent();
            }

            break;

        default:
            break;
    }

    listener.isActive = true;
}

QJsonArray SheetsModel::readJSONArray(QNetworkReply *reply) //MODULE FOR READ JSON
{
        const QByteArray json = reply->readAll();
        const QJsonDocument document = QJsonDocument::fromJson(json);
        QString strJson(document.toJson(QJsonDocument::Compact));
        const QJsonObject rootObject = document.object();
        const QJsonValue dataValue = rootObject.value("range");
        QJsonArray mName = rootObject.value("values").toArray();
        return mName;
}

void SheetsModel::prepareGrid()
{
    int i = 0;
    while(i < studentsCount)
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
        int rNum = xCase.bounded(23);
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
    QNetworkReply * replygo = googlewrapper.speechGet(QString::fromUtf8(phrase.toUtf8()));
    connect(replygo, &QNetworkReply::finished, [=]() {
        if (replygo->error() != QNetworkReply::NoError) {
            emit error(replygo->errorString());
//            qInfo()<<replygo->errorString();
//            qInfo()<<replygo->readAll();
            return;
        }
        const QByteArray json = replygo->readAll();
        const QJsonObject rootObject = QJsonDocument::fromJson(json).object();
        QByteArray zzz = rootObject.value("audioContent").toString().toUtf8();
        bytesOfSound = QByteArray::fromBase64(zzz);
        playAudio();
    });
}

void SheetsModel::updateTotalProb()//ALPHA!!! DISCUSSIVE POINT!!! ADD 0.5 INCREASE OF MARK!!!
{
    int onlineCount = isMain.count(true);
    int sumCountOfMarks = 0; //SUM OF COUNT OF MARKS OF EACH STUDENT
    int sumStudDays = 0; //SUM OF DAYS WITHOUT SKIP
    int sumSinceLast = 0; //SUM OF DAYS SINCE LAST MARK
    int sumTwo = 0; //SUM OF DAYS SINCE 2
    int maxStud = 0; //MAX OF DAYS WITHOUT SKIP
    int maxCount = 0; //MAX OF MARKS COUNT
    int minSinceLast = dataSize; //MAX DAYS SINCE LAST MARK
    int maxTwo = 0; //MIN DAYS SINCE TWO
    QList<int> cacheTwo; //CACHE SINCE TWO
    QList<int> cacheSince; //CACHE SINCE ANY
    QList<double> countProb; //LIST OF MARKS COUNT PROBABILITIES
    QList<double> studDaysProb; //LIST OF STUD DAYS PROBABILITIES
    QList<double> sinceProb; //LIST OF LAST MARK DAYS PROBABILITIES
    QList<double> twoProb; //LIST OF LAST TWO PROBABILITIES

    for (int i = 0; i < studentsCount; i++) {
        countProb << double(0);
        studDaysProb << double(0);
        sinceProb << double(0);
        twoProb << double(0);
        cacheTwo << 0;
        cacheSince << 0;

        if ( isOn[i] )
        {
            // TWO PROBABILITY MUST BE DISCUSSED!!!!!!
            // CHECK THIS PART OF CODE FOR REAL TIME EXECUTION

            cacheTwo[i] = sinceLastMark(i, 2);

            if (cacheTwo[i] > maxTwo) //LAST TWO
                maxTwo = cacheTwo[i];

            // FIND MINIMAL COUNT OF DAYS SINCE ANY LAST MARK
            cacheSince[i] = sinceLastMark(i);

            if (cacheSince[i] < minSinceLast) //LAST MARK
                minSinceLast = cacheSince[i];

            if (studDays[i] > maxStud) //STUD DAYS
                maxStud = studDays[i];

            if (mksCount[i] > maxCount) //MARKS COUNT
                maxCount = mksCount[i];
        }
    }

    for (int i = 0; i < studentsCount; i++) {
        if ( isOn[i] )
        {
            sumStudDays += (maxStud - studDays[i] + 1);
            sumCountOfMarks += (maxCount - mksCount[i]);
            sumSinceLast += (cacheSince[i] - minSinceLast);
            sumTwo += (maxTwo - cacheTwo[i]);
        }
    }

    //COUNT OF MARKS PROBABILITY REDUCED FOR CHILDREN WITH MAXIMAL COUNT OF MARKS
    //THEY SHOULD BE ACTIVE IN LIVE ORDER

    for (int i = 0; i < studentsCount; i++) {
        if ( isOn[i] )
        {
            if ( sumStudDays > 0 )
                studDaysProb[i] = double(maxStud - studDays[i] + 1) / double(sumStudDays);
            else {
                studDaysProb[i] = double(1)/ double(onlineCount);
            }

            if ( sumCountOfMarks > 0 )
                countProb[i] = double(maxCount - mksCount[i]) / double(sumCountOfMarks);
            else {
                countProb[i] = double(1)/ double(onlineCount);
            }

            if ( sumSinceLast > 0 )
                sinceProb[i] = double(cacheSince[i] - minSinceLast) / double(sumSinceLast);
            else {
                sinceProb[i] = double(1)/ double(onlineCount);
            }

            if ( sumTwo > 0 )
                twoProb[i] = double(maxTwo - cacheTwo[i]) / double(sumTwo);
            else {
                twoProb[i] = double(1)/ double(onlineCount);
            }

            //***********MATHEMATICAL MODEL OF MAKING DECISIONS**********************//
            // BETA VERSION!!!: [4 P(COUNT) + P(STUDDAYS) + 2 P(TWO) + 5 P(LAST)] / 12

            probability[i] = (4.*countProb[i] + studDaysProb[i]
                                  + 2.*twoProb[i] + 5.*sinceProb[i]) / double(12);
/*
            qInfo() <<"::::::MKS COUNT" << mksCount[i];
            qInfo() << "PROBABILITY" << probability[i];
            qInfo() << "COUNT PROB" <<countProb[i];
            qInfo() << "STUDDAYSPROB"  <<studDaysProb[i];
            qInfo() << "TWO" <<twoProb[i];
            qInfo() << "SINCE"<<sinceProb[i];
            */
        }
    }

    // UPDATE DECISION LIST
    decisionList[0] = probability[0];
    for(int i = 1; i < studentsCount; i++){
        decisionList[i] = decisionList[i-1] + probability[i];
    }

    qInfo() << decisionList;

}

int SheetsModel::sinceLastMark(int id)
{
    int minSinceAny = sinceLastMark(id, 2);
    // FIND MINIMAL COUNT OF DAYS SINCE ANY LAST MARK
    for (int j = 3; j < 6; j++) {
        int temp = sinceLastMark(id, j);
        if (minSinceAny > temp)
            minSinceAny = temp;
    }

    return minSinceAny; //minSinceAny SHOULD BE POSITIVE BY REGULARIZATION

}

int SheetsModel::sinceLastMark(int id, int mark)
{
    int result = dataSize-(*mksAtd[id]).lastIndexOf(QString::number(mark)) - 1;
    if (( result > 0 )&&( result <= dataSize ))
        return result;
    else {
        return dataSize; //FOR REGULARIZATION
    }
}

void SheetsModel::updateCurrentProb()
{

    double sumCurP = 0; //SUM OF CURRENT PROBABIBILITIES
    QList<double> orderProb; //TIME ORDERING PROBABILITY
    int orderSum = 0; //SUM FOR ORDER PROBABILIBTY

    for (int i = 0; i < studentsCount; i++) {
        currentProbability[i] = double(0);
        orderProb << double(0);

        if ( isActive[i] )
        {
            orderSum += double(order.count() - order.indexOf(i));
            sumCurP += probability[i];
        }
    }

    for (int i = 0; i < studentsCount; i++) {
        if ( isActive[i] ) {
            currentProbability[i] = .5*(double(order.count()-order.indexOf(i))/orderSum
                                        +probability[i]/sumCurP);
        }
    }

    // UPDATE CURRENT DECISION LIST
    currentDecisionList[0] = currentProbability[0];
    for(int i = 1; i < studentsCount; i++){
        currentDecisionList[i] = currentDecisionList[i-1] + currentProbability[i];
    }

}

void SheetsModel::callStudent()
{

    //TO PREVENT 1 STUDENT CALL EVERYTIME THIS FUNCTION MUST BE REVISED
    //IF THERE ARE NO STUDENTS TO ANSWER CASE 1, ELSE CASE 2

    if (order.isEmpty()){

        int id = coinToss(decisionList);
        while (isMain[id])
            id = coinToss(decisionList);

        googleSay("Отвечает "+names[id]+" "+surnames[id]);
        isMain[id] = true;
        isActive[id] = true; //BECAUSE LIST IS EMPTY
        SingleStudent bounded = createSingle(id);
        studentsflow.addStudent(bounded);
        emit studentSelected(id); //REVISE IS IT NEEDED?
    }
    else {
        int id = coinToss(currentDecisionList);
        while (isMain[id])
            id = coinToss(currentDecisionList);

        googleSay("Отвечает "+names[id]+" "+surnames[id]);
        isMain[id] = true;
        clearNotMain();
//        order.removeAt(order.indexOf(id));
        studentsflow.activeStudent(id);
        emit studentSelected(id); //REVISE IS IT NEEDED?
    }
}

int SheetsModel::coinToss(QList<double> decList)
{
    double toss = double(1) - xCase.generateDouble(); //BECAUSE OF [0,1) RANGE OF GENERATED NUMBER
    qInfo() << toss;
    qInfo() << "toss result of:";
    int i = 0;
    while ((toss > decList[i])&&( i < studentsCount)){
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
    studentsflow.setPM(plumin[id], id);
}

void SheetsModel::markUpdate(int mark, int id)
{
    avMark[id] = (avMark[id]*mksCount[id]+double(mark))/double(mksCount[id] + 1);
    mksCount[id]++;

    QString averageString;
    double avmCache = avMark[id];
    studentsflow.setAverage(averageString.sprintf("%6.2f", avmCache), id);

    googleSay(" "+names[id]+" получает "+QString::number(mark));
    int lastfree = mksAtd[id]->lastIndexOf(""); //LAST INDEX WITH FREE VALUE
    (*mksAtd[id])[lastfree] = QString::number(mark);

    updateTotalProb();

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
    QNetworkReply * updateReply = googlewrapper.updateSheet(plumin[id], pmColumn, id+startId);
    connect(updateReply, &QNetworkReply::finished, [=]() {
        if (updateReply->error() != QNetworkReply::NoError) {
            emit error(updateReply->errorString());
            return;
        }
    });
}

void SheetsModel::absent()
{
    QStringList offStudents; // ABSENT STUDENTS LIST
    for (int i = 0; i < studentsCount; i++) {
        if (!isOn[i])
            (*mksAtd[i]).last() = QString(u8"'п"); // BY DEFAULT "п"; ' - TO AVOID AUTOSUBSTITUTION

        offStudents << (*mksAtd[i]).last();
    }

    auto updateReply = googlewrapper.updateSheet(offStudents, curCol);
    connect(updateReply, &QNetworkReply::finished, [=]() {
        if (updateReply->error() != QNetworkReply::NoError) {
            emit error(updateReply->errorString());
            return;
        }
    });
}

void SheetsModel::clearNotMain()
{
    order.clear();
    int i = 0;
    while ( i < studentsCount )
    {
        if ( !isMain[i] ){
            isActive[i] = false;
        }
        i++;
    }
}
