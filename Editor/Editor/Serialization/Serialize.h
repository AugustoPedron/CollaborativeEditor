#pragma once

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QString>
#include <QStringList>
#include <QImage>
#include <QPixMap>
#include <string>
#include <vector>
#include <QByteArray>

#include "define.h"
#include "../CRDT/CRDT.h"

class Serialize : public QObject
{
	Q_OBJECT

private:
	static Serialize* instance;

	Serialize(QObject* parent = Q_NULLPTR);

	QByteArray fromObjectToArray(QJsonObject obj);
	QJsonValue jsonValFromPixmap(const QPixmap& p);
	QPixmap pixmapFrom(const QJsonValue& val);

public:
	static Serialize* getInstance();

	//QJsonObject unserialize(QString str); // old
	int actionType(QJsonObject obj);

	QByteArray changeProfileSerialize(QString oldUsername, QString newUsername, QString oldEmail, QString newEmail, QPixmap* newImage, int type);

	QStringList changeProfileUnserialize(QJsonObject obj);


	QByteArray userSerialize(QString user, QString password, QString email, int type, QPixmap* profileImage = Q_NULLPTR);//type usato per discriminare login o register
	QStringList userUnserialize(QJsonObject obj);//in particolare la lista contiene 2 elementi se uso login oppure 3 se uso
	//la register l'immagine viene serializzata a parte per ora

	QByteArray changePasswordSerialize(QString oldPassword, QString newPassword, int type);
	QStringList changePasswordUnserialize(QJsonObject obj);

	QByteArray fileNameSerialize(QString fileName, int type);

	QString fileNameUnserialize(QJsonObject obj);

	QByteArray FileListSerialize(QMap<int, QString> files, int type);
	QMap<int, QString> fileListUnserialize(QJsonObject obj);

	QByteArray newFileSerialize(QString filename, int type);
	QPair<int,QString> newFileUnserialize(QJsonObject obj);

	QByteArray openDeleteFileSerialize(int fileId, int type);
	int openDeleteFileUnserialize(QJsonObject obj);

	QByteArray closeFileSerialize(int fileId, int siteCounter, int type);
	QPair<int, int> closeFileUnserialize(QJsonObject obj);

	QByteArray renameFileSerialize(int fileId, QString newName, int type);
	QStringList renameFileUnserialize(QJsonObject obj);

	QByteArray openSharedFileSerialize(QString URI, int type);
	QString openSharedFileUnserialize(QJsonObject obj);

	QStringList changeProfileResponseUnserialize(QJsonObject obj);

	QByteArray messageSerialize(Message message, int fileId, int type);//qui abbiamo sia il messaggio con all'interno un simbolo
	QPair<int, Message> messageUnserialize(QJsonObject obj);

	QByteArray textMessageSerialize(QString str, int type);
	QString textMessageUnserialize(QJsonObject obj);

	QByteArray imageSerialize(QPixmap img, int type);
	QPixmap imageUnserialize(QJsonObject obj);

	QByteArray responseSerialize(bool res, QString message, int type, QString username = "", QString email = "", int userID = -1, QColor color = Qt::black);
	QStringList responseUnserialize(QJsonObject obj);

	QString URIUnserialize(QJsonObject uri);

	QByteArray ObjectFromString(QString& in);

	QByteArray cursorPostionSerialize(int position, int user, int fileId, int type);
	std::vector<int> cursorPostionUnserialize(QJsonObject obj);

	QByteArray addEditingUserSerialize(int userId, QString username, QColor userColor, int fileId, int type);
	QPair<int, QStringList> addEditingUserUnserialize(QJsonObject obj);

	QByteArray removeEditingUserSerialize(int userId, int fileId, int type);
	QPair<int, int> removeEditingUserUnserialize(QJsonObject obj);

	QByteArray requestFileList(int type);

	QByteArray logoutUserSerialize(int type);

	QPair<int, int> siteCounterUnserialize(QJsonObject obj);

};