#include "googlewrapper.h"

GoogleWrapper::GoogleWrapper(QObject *parent) : QObject(parent)
{
    /*GOOGLE APP VALUES*/
    /*READ GOOGLE JSON*/
    QString val; //google secret data
    QFile file(QCoreApplication::applicationDirPath()+"/api/secret_google.json"); //JSON file with google secret
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();
    /*SETTINGS FOR GRANT*/
    QJsonDocument document = QJsonDocument::fromJson(val.toUtf8());
    const auto object = document.object();
    const auto settingsObject = object["web"].toObject();
    const QUrl authUri(settingsObject["auth_uri"].toString());
    const auto clientId = settingsObject["client_id"].toString();
    const QUrl tokenUri(settingsObject["token_uri"].toString());
    const auto clientSecret(settingsObject["client_secret"].toString());
    const auto redirectUris = settingsObject["redirect_uris"].toArray();
    const QUrl redirectUri(redirectUris[0].toString()); // Get the first URI
    const auto port = static_cast<quint16>(redirectUri.port()); // Get the port
    /*OAUTH2 SETTINGS*/
    oauth2.setScope("https://www.googleapis.com/auth/spreadsheets https://www.googleapis.com/auth/cloud-platform");
    oauth2.setAuthorizationUrl(authUri);
    oauth2.setClientIdentifier(clientId);
    oauth2.setAccessTokenUrl(tokenUri);
    oauth2.setClientIdentifierSharedKey(clientSecret);

//    QNetworkAccessManager* netManager = new QNetworkAccessManager;
//    oauth2.setNetworkAccessManager(netManager);

    auto replyHandler = new QOAuthHttpServerReplyHandler(port, this);
    oauth2.setReplyHandler(replyHandler);

    connect(&oauth2, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
        &QDesktopServices::openUrl);

    connect(&oauth2,&QOAuth2AuthorizationCodeFlow::statusChanged,
            [=](QAbstractOAuth::Status status) {
        if (status == QAbstractOAuth::Status::Granted)
            {
                emit authenticated();
                qInfo() << "google authenticated";
            }
        });

}

void GoogleWrapper::grant(){
    oauth2.grant();
}



QNetworkReply *GoogleWrapper::requestForStudents()
{
    QString reqUrl = gHeader+sheetId+"/values/B11%3AB42?majorDimension=ROWS";
    qDebug() << "Getting students info..."+reqUrl;
    return oauth2.get(reqUrl);
}

QNetworkReply *GoogleWrapper::getDateList()
{
    QString reqUrl = gHeader+sheetId+"/values/D7%3ACZ8?majorDimension=COLUMNS";
    qDebug() << "Getting dates info..."+reqUrl;
    return oauth2.get(reqUrl);
}

QNetworkReply *GoogleWrapper::getClassesList()
{
    QString dataSheetId;
    QFile file(QCoreApplication::applicationDirPath()+"/api/subandclas");
    if(!file.open(QIODevice::ReadOnly)) {
        qInfo()<<"error reading file with subject structure";
    }

    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine();
        if (line == "dataSheetId:"){
            dataSheetId = in.readLine();
        }
    }
    file.close();
    QString reqUrl = gHeader+dataSheetId+"/values/A2%3AC30?majorDimension=ROWS";
//    qDebug() << dataSheetId;
    return oauth2.get(reqUrl);
}

QNetworkReply *GoogleWrapper::downloadData()
{
    QString lastRow = QString::number(studCount+10);
    QString reqUrl = gHeader+sheetId+"/values/D11%3A"+curCol+lastRow+"?majorDimension=ROWS";
    qDebug() << "Getting marks info..."+reqUrl;
    return oauth2.get(reqUrl);
}

QNetworkReply *GoogleWrapper::getPM(QString col)
{
    QString reqUrl = gHeader+sheetId+values+col+"11%3A"+col+"40?majorDimension=ROWS";
    qDebug() << "Getting +-"+reqUrl;
    return oauth2.get(reqUrl);
}

QNetworkReply *GoogleWrapper::updateSheet(QString data, QString col, int row)
{
    QString toWrite = "{\"values\":[[\""+data+"\"]]}"; //JSON FORMATTED DATA TO WRITE

    QNetworkRequest reqq; //SETTING REQUEST PARAMETERS
    reqq.setUrl(QUrl(gHeader+sheetId+values+col+
                     QString::number(row)+"%3A"+col+QString::number(row)+"?valueInputOption=RAW"));

    reqq.setRawHeader(QString("Authorization").toLocal8Bit(),
                      ("Bearer "+oauth2.token()).toLocal8Bit());

    qInfo()<<"PUT REQUEST";
    qInfo()<<reqq.url();

    return oauth2.networkAccessManager()->put(reqq,toWrite.toLocal8Bit());
}

QNetworkReply *GoogleWrapper::updateSheet(QStringList data, QString col)
{
    int dataSize = data.length();
    QNetworkRequest reqq;
    reqq.setUrl(QUrl(gHeader+sheetId+values+col+
                     QString::number(initRaw)+"%3A"+col+
                     QString::number(initRaw+dataSize-1)+"?valueInputOption=RAW"));
    reqq.setRawHeader(QString("Authorization").toLocal8Bit(),
                      ("Bearer "+oauth2.token()).toLocal8Bit());
    reqq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject subjson;

    QString range = col+QString::number(initRaw)+":"+
            col+QString::number(initRaw+dataSize-1);

    QJsonArray jsonData = QJsonArray::fromStringList(data);

    QJsonObject json
    {
        {"majorDimension", "ROWS"},
        {"range", range},
        {"values", jsonData}
    };

    return oauth2.networkAccessManager()->put(reqq, QJsonDocument(json).toJson());

}

QNetworkReply *GoogleWrapper::speechGet(QString text)
{
    QNetworkRequest reqq;
    reqq.setUrl(QUrl(cloudHeader));
    reqq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject json;
    QJsonObject subjson;

    subjson.insert("text",text);
    json.insert("input",subjson);
    subjson.remove("text");

    QJsonArray pAudio;
    pAudio.prepend("OGG_OPUS");
    subjson.insert("audioEncoding","MP3");
    json.insert("audioConfig",subjson);
    subjson.remove("audioEncoding");


    subjson.insert("languageCode","ru-RU");
//    subjson.insert("name",)
    json.insert("voice",subjson);
    subjson.remove("languageCode");




    qInfo()<<"POST CLOUD";
    qInfo()<<json;
    reqq.setRawHeader(QString("Authorization").toLocal8Bit(),
                      ("Bearer "+oauth2.token()).toLocal8Bit());
    return  oauth2.networkAccessManager()->post(reqq,QJsonDocument(json).toJson());


}
