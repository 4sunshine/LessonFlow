#ifndef GOOGLEWRAPPER_H
#define GOOGLEWRAPPER_H

#include <QObject>
#include <QNetworkReply>
#include <QOAuth2AuthorizationCodeFlow>
#include <QJsonDocument>
#include <QtCore>
#include <QDesktopServices>
#include <QtNetwork>
#include <QtNetworkAuth>
#include <QJsonObject>

class GoogleWrapper : public QObject
{
    Q_OBJECT
public:
    explicit GoogleWrapper(QObject *parent = nullptr);

    QNetworkReply *requestForStudents();
    QNetworkReply *getClassesList();
    QNetworkReply *getDateList();
    QNetworkReply *downloadData();
    QNetworkReply *updateSheet(QString data, QString col, int row);//UPDATE SHEETS WITH DATA
    QNetworkReply *getPM(QString col);//GET PLUSES AND MINUSES
    QNetworkReply *speechGet(QString text);

    QString sheetId;
    QString curCol;
    int studCount; //COUNT OF STUDENTS


signals:
    void authenticated();
    void subscribed(const QUrl &url);

public slots:
    void grant();   //SLOT FOR GRANTING PROCEDURE
//    void firstRequest(QOAuth2AuthorizationCodeFlow *parent = nullptr);

private:
    QOAuth2AuthorizationCodeFlow oauth2;
    const QString gHeader = "https://sheets.googleapis.com/v4/spreadsheets/"; //COMMON HEADER OF SHEET REQUESTS
    const QString cloudHeader = "https://texttospeech.googleapis.com/v1beta1/text:synthesize"; //GOOGLE CLOUD HEADER
};

#endif // GOOGLEWRAPPER_H
