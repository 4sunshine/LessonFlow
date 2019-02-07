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
    const QJsonObject object = document.object();
    const QJsonObject settingsObject = object["web"].toObject();
    const QUrl authUri(settingsObject["auth_uri"].toString());
    const QString clientId = settingsObject["client_id"].toString();
    const QUrl tokenUri(settingsObject["token_uri"].toString());
    const QString clientSecret(settingsObject["client_secret"].toString());
    const QJsonArray redirectUris = settingsObject["redirect_uris"].toArray();
    const QUrl redirectUri(redirectUris[0].toString()); // Get the first URI
    const u_short port = static_cast<quint16>(redirectUri.port()); // Get the port
    /*OAUTH2 SETTINGS*/
    oauth2.setScope("https://www.googleapis.com/auth/spreadsheets https://www.googleapis.com/auth/cloud-platform");
    oauth2.setAuthorizationUrl(authUri);
    oauth2.setClientIdentifier(clientId);
    oauth2.setAccessTokenUrl(tokenUri);
    oauth2.setClientIdentifierSharedKey(clientSecret);
    QOAuthHttpServerReplyHandler *replyHandler = new QOAuthHttpServerReplyHandler(port, this);
    oauth2.setReplyHandler(replyHandler);
    connect(&oauth2, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
        &QDesktopServices::openUrl);

    connect(&oauth2,&QOAuth2AuthorizationCodeFlow::statusChanged,
            [=](QAbstractOAuth::Status status) {
        if (status == QAbstractOAuth::Status::Granted)
            {
                emit authenticated();
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
    QString val; //datasheet id data
    QFile file(QCoreApplication::applicationDirPath()+"/api/subandclas.json"); //JSON file with google secret
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();
    QJsonDocument document = QJsonDocument::fromJson(val.toUtf8());
    const QJsonObject object = document.object();
    const QString dataSheetId = object["dataSheetId"].toString();

    QString reqUrl = gHeader+dataSheetId+"/values/A2%3AC30?majorDimension=ROWS";
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

    QString range = col+QString::number(initRaw)+":"+
            col+QString::number(initRaw+dataSize-1);

    QJsonArray jsonData;
    int i = 0;
    while ( i < dataSize) {
        QJsonArray element = {data[i]};
        jsonData << element;
        i++;
    }

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
    reqq.setRawHeader(QString("Authorization").toLocal8Bit(),
                      ("Bearer "+oauth2.token()).toLocal8Bit());

    QJsonObject audioConfig
    {
        {"audioEncoding", "MP3"}
    };

    QJsonObject input
    {
        {"text", text}
    };

    QJsonObject voice
    {
        {"languageCode", "ru-RU"}
    };

    QJsonObject json
    {
        {"audioConfig", audioConfig},
        {"input", input},
        {"voice", voice}
    };

    return  oauth2.networkAccessManager()->post(reqq,QJsonDocument(json).toJson());

}
