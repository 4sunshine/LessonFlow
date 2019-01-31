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
    connect(this,&SheetsModel::audioReady,this,&SheetsModel::playAudio);
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
    //COLUMN AA INDEXES CREATION
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
    this->columnIndexes = addr;
}

void SheetsModel::makeClassesRepresent()
{
    for (int i=0; i < classNames.size(); i++)
        classesRepresent<<(classNames[i]->subject+' '+classNames[i]->className);
}

void SheetsModel::classSelected(int classId)
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
                names << classes[i][0].toString().section(' ',1,1); //APPENDING NAMES
                surnames << classes[i][0].toString().section(' ',0,0); //APPNG SURNAMES
                sex << getSex(names[i]); //SETTING SEX
            }
        googlewrapper.studCount = students.length();
        qInfo()<<students.length();
        qInfo()<<students;
        qInfo()<<sex;
        qInfo()<<names;
        getDates();
    });
}

void SheetsModel::getDates()
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

            connect(reply3, &QNetworkReply::finished, [=]() {
                if (reply3->error() != QNetworkReply::NoError) {
                    emit error(reply3->errorString());
                    return;
                }

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

        int i = 0;

        while((months[moN-1] != datMo[i][0].toString())){
            i++;
        }

        while(!datMo[i][1].isUndefined()&&(daY >= datMo[i][1].toString().toInt())){
            i++;
        }

        dataSize = i; //LENGTH OF DATA WITH MARKS & ATTENDANCE
        curCol = columnIndexes[i+2]; //BECAUSE OF COLUMN INDEXES OF MARKS DATA
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
                mksAtd << locD;

                probability << double(0); //INITIALIZE PROBABILITIES LISTS
                decisionList << double(0);
                currentProbability << double(0);
                currentDecisionList << double(0);

            }
            qInfo()<<attend;
            qInfo()<<avMark;
            qInfo()<<studDays;
            qInfo()<<mksCount;


            prepareGrid(); //PREPARE GRID AT START
            listener.isActive = true; //start recieve data from server
            qInfo()<< "LISTENER STATUS:";
            qInfo()<< listener.isActive;
            emit gridPrepared();
//*******************************************************************
//            googleSay("Привет, ребята! Отметьтесь на уроке");

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

    if (isLessonStarted && isOn[btnId-1]) {
        isActive[btnId-1] = !isActive[btnId-1];
        if (isActive[btnId-1])
        {
            qInfo() << "CREATE NEW STUDENTS";
            QString averageString;
            double avmCache = avMark[btnId-1];
            studentsflow.addStudent(SingleStudent(btnId-1,names[btnId-1],surnames[btnId-1],
                plumin[btnId-1],
                      averageString.sprintf("%6.2f", avmCache),
                      setAva(btnId-1),isOn[btnId-1]));
            order << btnId-1;
            updateCurrentProb();
            qInfo() << currentProbability;
            qInfo() << currentDecisionList;
            qInfo() << order;
        }
        else {
            order.removeAt(order.indexOf(btnId-1));
            studentsflow.removeStudent(btnId-1);
            updateCurrentProb();
            qInfo() << currentProbability;
            qInfo() << currentDecisionList;
            qInfo() << order;
        }

    }

    //DISCUSSIVE POINT!!! ACTIVITY ++ EVERY BTN EVENT

}

void SheetsModel::irHandler(int irCode)
{
    switch (irCode) {
        case UP:

            break;
        case DOWN:

            break;
        case RIGHT:

            break;
        case LEFT:

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
            break;
        case TWO:

            break;
        case THREE:

            break;
        case FOUR:

            break;
        case FIVE:

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
        QString averageString;
        double avmCache = avMark[i];

        studentsflow.addStudent(SingleStudent(i,names[i],surnames[i],plumin[i],
                      averageString.sprintf("%6.2f", avmCache),
                      setAva(i),isOn[i]));
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
        return QString("path868white.png");
    }
}

void SheetsModel::playAudio()
{
    audioStream.setData(bytesOfSound);
    audioStream.open(QIODevice::ReadOnly);
    player.setMedia(QMediaContent(),&audioStream);
    player.setVolume(99);
    player.play();
    qInfo() << "PLAY SOUND";
}

void SheetsModel::googleSay(QString phrase)
{
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
        audioReady();
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
