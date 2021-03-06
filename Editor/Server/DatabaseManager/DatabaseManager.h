#pragma once

#include <cstdlib>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QMap>
#include <QSet>
#include <QString>
#include <QTcpSocket>
#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QRegularExpression>

#include "../Serialize/Serialize.h"
#include "../Serialize/MessageTypes.h"
#include "../File/file.h"
#include "../ClientManager/clientmanager.h"

class Serialize;
class File;

class DatabaseManager
{
public:
    DatabaseManager();
    static DatabaseManager*startDBConnection();

    static QString generateRandomString(int len);
   // static void sendMessage(QTcpSocket *socket, QByteArray obj);
    static void sendError(ClientManager* user);
    static void sendSuccess(ClientManager* user);
    static bool is_email_valid(QString email);
    static bool is_username_unique(QString username);
    static QString computeHashPassword(QString password);
    static bool checkPassword(QString password, ClientManager* client);

    void registration(QString username, QString email, QString password, QString profileImage, ClientManager* incomingClient);
    void login(QString username, QString password,  ClientManager* incomingClient);
    void logout(ClientManager* user);

    void createFile(QString filename, ClientManager* client);
    void sendFileList(ClientManager* client);
    void openFile(int fileId, ClientManager* client);
    void closeFile(int fileId, int siteCounter, ClientManager* client);
    void deleteFile(int fileId, ClientManager* client);
    QString changeFileName(QString oldPath, QString newName, int fileId, ClientManager* client);
    void renameFile(int fileId, QString newname, ClientManager* client);
    void getURIToShare(int fileid, ClientManager* client);
    void SharedFileAcquisition(QString URI, ClientManager* client);

    void changePassword(QString oldPassword, QString newPassword, ClientManager* client);
    /*
    void changeUsername(QString newUsername, ClientManager* client);
    void changeEmail(QString newEmail, ClientManager* client);
    void changeProfilePic(QString profileImage, ClientManager* client);
    */

    void changeProfile(QString oldUsername, QString newUsername, QString oldEmail, QString newEmail, QString newImage, ClientManager* client);


    void forwardMessage(ClientManager* user, QJsonObject obj, QByteArray data);

    File* getFile(int fileid);
    bool isUserLogged(ClientManager* client);


    /*void funzionedaeliminare() {
        File* primo_file = new File(-1, "C:/Users/Mattia Proietto/Desktop/PROVAEBASTA.txt");
        instance->files.insert(-1, primo_file);
    }*/

    inline QColor getUserColor(int userId){return m_colorPerUser.value(userId);}

private:
    static DatabaseManager* instance;
    QSqlDatabase db;

    QVector<ClientManager*> activeusers;
    QMap<int, File*> files; // mappa fileId - File contenente tutti i file attivi al momentento
    QMap<int, QColor> m_colorPerUser; //lista dei colori usati

    QByteArray intToArray(qint64 source);
    QColor generateRandomColor(int);
    bool colorPresent(QColor);
};
