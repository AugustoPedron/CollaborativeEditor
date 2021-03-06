#pragma once

#include <Qt>
#include <QMainWindow>
#include <QDesktopWidget>
#include <QApplication>
#include <QProcess>
#include <QFileSystemModel>
#include <QTreeView>
#include <QCloseEvent>
#include <QLineEdit>

#include "../Editor/Editor.h"
#include "../SocketHandler/SocketHandler.h"
#include "../Serialization/Serialize.h"
#include "../Modify Profile/ModifyProfile.h"
#include "../Dialog/Dialog.h"
#include "ui_FileBrowser.h"

class FileBrowser : public QMainWindow
{
	Q_OBJECT

public:
	FileBrowser(QSharedPointer<SocketHandler> socketHandler, QSharedPointer<QPixmap> profileImage, QSharedPointer<QPixmap> profileImageResized, QColor userColor,
		QString email, QString username = "", int clientID = 0, QWidget* parent = Q_NULLPTR);
	~FileBrowser();


private:
	QSharedPointer<SocketHandler> m_socketHandler;
	std::map<int, Editor*> m_textEditors;
	QFileSystemModel model;
	QString username;
	QString email;
	int clientID;
	ModifyProfile* m_modifyProfile;
	//ModifyPassword* m_modifyPassword;
	Ui::FileBrowser ui;
	QLineEdit* m_newFileLabel;
	QSharedPointer<QPixmap> m_profileImage;
	QSharedPointer<QPixmap> m_profileImageResized;
	QMap<int, QString> filename_id;
	QColor m_userColor;
	QTimer* m_timer;
	bool m_openAfterUri;
	Serialize* m_serializeInstance;

	void closeEvent(QCloseEvent* event);
	void removeBlank();
	void requestFiles();
	void addFile(QString file);
	
	/*void showURI(QJsonObject msg);*/

private slots:
	void on_fileList_itemDoubleClicked(QListWidgetItem* item);
	void on_logoutButton_clicked();
	void on_modifyProfile_clicked();
	void on_newFile_clicked();
	void on_deleteFile_clicked();
	void editorClosed(int, int);
	void on_renameFile_clicked();
	void on_addSharedFileButton_clicked();
	void childWindowClosed();
	void childWindowClosedAndUpdate(QString m_username, QString m_email, QSharedPointer<QPixmap> m_profileImage);
	void addFiles(QJsonObject message);
	void addFile(QJsonObject message);
	void handleNewMessage(QJsonObject message);
	void processEditorMessage(QJsonObject message);
	void showErrorMessage();
	void showURI(QJsonObject msg);
	void on_file_clicked(); //ilio (quando clicco su un file appaiono i bottoni delete e rename)
	void on_URI_set();//ilio (quando inserisco la URI appare il bottone share)
	/*void copia();*/
	//void on_modifyPassword_clicked();

signals:
	void showParent();
	void fileClicked();
	void dataToSend(QByteArray);

protected:
	void mousePressEvent(QMouseEvent* event);
};
