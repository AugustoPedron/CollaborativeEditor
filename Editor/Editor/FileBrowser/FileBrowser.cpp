#include "FileBrowser.h"
#include <QMap>

FileBrowser::FileBrowser(QSharedPointer<SocketHandler> socketHandler, QSharedPointer<QPixmap> profileImage, QSharedPointer<QPixmap> profileImageResized, QColor userColor, QString email, QString username,
	int clientID, QWidget* parent)
	: QMainWindow(parent), m_socketHandler(socketHandler), m_profileImage(profileImage), m_userColor(userColor), m_timer(new QTimer(this)),
	m_openAfterUri(false), email(email), username(username), m_profileImageResized(profileImageResized), m_modifyProfile(Q_NULLPTR)
{
	ui.setupUi(this);
	ui.username->setText(username);
	ui.profileImage->setPixmap(*m_profileImageResized);
	ui.renameFile->setEnabled(false);
	ui.deleteFile->setEnabled(false);
	ui.addSharedFileButton->setEnabled(false);

	//this->username = username;
	this->clientID = clientID;
	m_serializeInstance = m_serializeInstance->getInstance();
	connect(m_socketHandler.get(), &SocketHandler::dataReceived, this, &FileBrowser::handleNewMessage);
	connect(this, &FileBrowser::dataToSend, m_socketHandler.get(), &SocketHandler::writeData, Qt::QueuedConnection);
	connect(m_timer, &QTimer::timeout, this, &FileBrowser::showErrorMessage);
	connect(ui.fileList, &QListWidget::itemSelectionChanged, this, &FileBrowser::on_file_clicked);
	connect(ui.uriLineEdit, &QLineEdit::textChanged, this, &FileBrowser::on_URI_set);

	QByteArray message = m_serializeInstance->requestFileList(MessageTypes::SendFiles);
	//m_socketHandler->writeData(message);
	emit dataToSend(message);
}

FileBrowser::~FileBrowser()
{
	//qDebug()<<"closing file browser";
}

void FileBrowser::on_fileList_itemDoubleClicked(QListWidgetItem* item) {
	QString filename = item->text();
	if (filename == "") {
		return;
	}
	auto it = m_textEditors.find(item->data(Qt::UserRole).toInt());
	Editor* editor;
	//controllo per vedere se l'editor del file che sto cercando di aprire esista gi? o no
	if (it == m_textEditors.end()) {
		//caso in cui si fa doppio click sul file di test
		if (filename == "test file") {
			QString path = QDir::currentPath().append("\\PROVA SCRITTURA.txt");
			editor = new Editor(m_socketHandler, m_profileImage, m_userColor, path, username, 0, clientID);
			m_textEditors.insert(std::pair<int, Editor*>(-1, editor));
		}
		//caso in cui si apra un file che ? presente sul server
		else {
			int id = item->data(Qt::UserRole).toInt();
			editor = new Editor(m_socketHandler, m_profileImage, m_userColor, filename, username, id, clientID);
			m_textEditors.insert(std::pair<int, Editor*>(id, editor));
			QByteArray data = m_serializeInstance->openDeleteFileSerialize(id, MessageTypes::Open);
			//this->m_socketHandler->writeData(data);
			emit dataToSend(data);
		}
		connect(editor, &Editor::editorClosed, this, &FileBrowser::editorClosed);
		editor->show();
	}
	//nel caso in cui l'editor del file che ho cercato di aprire sia gi? aperto, questo vien portato in primo piano
	else {
		editor = it->second;
		editor->raise();
	}
}

//creazione di un nuovo file
void FileBrowser::on_newFile_clicked() {
	/*auto model = ui.treeView->model();
	QModelIndex parent = model->index(0, 0);
	model->insertRow(0, parent);
	model->setData(model->index(0, 0, parent), QString("Child Item"));
	ui.treeView->setModel(model);*/
	bool ok;

	//viene aperto un dialog dove immettere il nome del nuovo file
	QString filename = QInputDialog::getText(this, tr("New File"),
		tr("File name:"), QLineEdit::Normal,
		"New Text File", &ok);
	if (ok && !filename.isEmpty()) {
		//send to server
		QByteArray data = m_serializeInstance->newFileSerialize(filename, MessageTypes::NewFile);
		//this->m_socketHandler->writeData(data);
		//emit dataToSend(data);
		addFile(filename);
	}
	else {
		QMessageBox resultDialog(this);
		QString res_text = "File name needed";
		resultDialog.setInformativeText(res_text);
		resultDialog.exec();
	}

}

//cancellazione del file selezionato nella lista di file
void FileBrowser::on_deleteFile_clicked()
{
	QListWidgetItem* current_item = ui.fileList->currentItem();
	if (current_item == nullptr) {
		QMessageBox resultDialog(this);
		QString res_text = "Select a file";
		resultDialog.setInformativeText(res_text);
		resultDialog.exec();
		return;
	}
	QString filename = current_item->text();
	int id = current_item->data(Qt::UserRole).toInt();
	QByteArray data = m_serializeInstance->openDeleteFileSerialize(id, DELETE);
	//this->m_socketHandler->writeData(data);
	emit dataToSend(data);
	//QListWidgetItem* item = ui.fileList->takeItem(ui.fileList->row(current_item));
	if (current_item != nullptr) {

		disconnect(ui.fileList, &QListWidget::itemSelectionChanged, this, &FileBrowser::on_file_clicked);
		delete current_item;
		current_item = nullptr;
		connect(ui.fileList, &QListWidget::itemSelectionChanged, this, &FileBrowser::on_file_clicked);
		ui.renameFile->setEnabled(false);
		ui.deleteFile->setEnabled(false);
	}
}

void FileBrowser::on_renameFile_clicked()
{
	//QList<QListWidgetItem*> item = ui.fileList->findItems("prova", Qt::MatchFixedString);

	//item[0]->setText("prova_");
	//return;

	QListWidgetItem* current_item = ui.fileList->currentItem();
	if (current_item == nullptr) {
		QMessageBox resultDialog(this);
		QString res_text = "Select a file";
		resultDialog.setInformativeText(res_text);
		resultDialog.exec();
		return;
	}
	//vecchio nome
	QString filename = current_item->text();
	int id = current_item->data(Qt::UserRole).toInt();

	//prendo il nuovo nome
	bool ok;
	QString new_filename = QInputDialog::getText(this, tr("New Name"),
		tr("New name:"), QLineEdit::Normal,
		filename, &ok);

	if (ok && !new_filename.isEmpty()) {
		//send to server
		if (new_filename != filename) {
			QByteArray data = m_serializeInstance->renameFileSerialize(id, new_filename, MessageTypes::Rename);
			//this->m_socketHandler->writeData(data);
			emit dataToSend(data);
			current_item->setText(new_filename);
		}

	}
	else {
		QMessageBox resultDialog(this);
		QString res_text = "File name needed";
		resultDialog.setInformativeText(res_text);
		resultDialog.exec();
	}


}

void FileBrowser::closeEvent(QCloseEvent* event) {
	QByteArray message = m_serializeInstance->logoutUserSerialize(MessageTypes::Logout);
	//m_socketHandler->writeData(message);
	emit dataToSend(message);
	qApp->quit();
}

void FileBrowser::removeBlank()
{
	QList<QListWidgetItem*> items = ui.fileList->findItems("", Qt::MatchExactly);
	for (auto it : items) {
		ui.fileList->takeItem(ui.fileList->row(it));
		delete it;
		it = nullptr;
	}

}

void FileBrowser::on_logoutButton_clicked() {
	disconnect(m_socketHandler.get(), &SocketHandler::dataReceived, this, &FileBrowser::handleNewMessage);
	for (auto el : this->m_textEditors) {
		QByteArray data = m_serializeInstance->closeFileSerialize(el.first, el.second->getSiteCounter_(), MessageTypes::Close);
		emit dataToSend(data);
		el.second->deleteLater();
	}
	m_textEditors.clear();
	QByteArray message = m_serializeInstance->logoutUserSerialize(MessageTypes::Logout);
	//m_socketHandler->writeData(message);
	emit dataToSend(message);
	emit showParent();
	this->hide();
}

//viene aperta la finestra per la modifica dei dati dell'utente
void FileBrowser::on_modifyProfile_clicked()
{
	if (m_modifyProfile != Q_NULLPTR)
		m_modifyProfile->deleteLater();
	this->m_modifyProfile = new ModifyProfile(m_socketHandler, this->username, this->email, this->m_profileImage); 
	m_modifyProfile->show();
	connect(m_modifyProfile, &ModifyProfile::showParentUpdated, this, &FileBrowser::childWindowClosedAndUpdate);
	connect(m_modifyProfile, &ModifyProfile::showParent, this, &FileBrowser::childWindowClosed);
	this->hide();
}

void FileBrowser::childWindowClosed() {
	this->show();
}

void FileBrowser::childWindowClosedAndUpdate(QString m_username, QString m_email, QSharedPointer<QPixmap> m_profileImage) {
	this->username = m_username;
	this->email = m_email;
	this->m_profileImage = m_profileImage;
	m_profileImageResized = QSharedPointer<QPixmap>(new QPixmap(m_profileImage->scaled(QSize(60, 60), Qt::KeepAspectRatio)));

	ui.username->setText(m_username);
	ui.profileImage->setPixmap(*m_profileImageResized);
	this->show();
}

void FileBrowser::editorClosed(int fileId, int siteCounter) {
	Editor* editor = m_textEditors.at(fileId);
	editor->deleteLater();
	QByteArray message = m_serializeInstance->removeEditingUserSerialize(this->clientID, fileId, MessageTypes::RemoveEditingUser);
	//m_socketHandler->writeData(message);
	emit dataToSend(message);
	QByteArray data = m_serializeInstance->closeFileSerialize(fileId, siteCounter, MessageTypes::Close);
	//this->m_socketHandler->writeData(data);
	emit dataToSend(data);
	filename_id.remove(fileId);
	m_textEditors.erase(fileId);
	this->raise();
}

void FileBrowser::mousePressEvent(QMouseEvent* event) {

	if (ui.fileList->currentItem() == nullptr) {
		ui.renameFile->setEnabled(false);
		ui.deleteFile->setEnabled(false);
	}

	show();
}

void FileBrowser::requestFiles() {
	//aggiungere messaggio per la richiesta della lista dei file al server
	//m_socketHandler->writeData();
}



void FileBrowser::addFiles(QJsonObject filesList) {
	m_timer->stop();
	QMap<int, QString> map = m_serializeInstance->fileListUnserialize(filesList);

	for (auto id : map.keys()) {
		QListWidgetItem* item = new QListWidgetItem(map.value(id), ui.fileList);
		item->setData(Qt::UserRole, id);
		this->filename_id.insert(id, map.value(id));
	}
	removeBlank();
}

void FileBrowser::addFile(QJsonObject file) {
	QPair<int, QString> pair = m_serializeInstance->newFileUnserialize(file);
	this->filename_id.insert(pair.first, pair.second);
	QListWidgetItem* item = new QListWidgetItem(pair.second, ui.fileList);
	item->setData(Qt::UserRole, pair.first);

	removeBlank();
}

void FileBrowser::addFile(QString file) {
	//QPair<int, QString> pair = m_serializeInstance->newFileUnserialize(file);
	this->filename_id.insert(0, file);
	QListWidgetItem* item = new QListWidgetItem(file, ui.fileList);
	item->setData(Qt::UserRole, 0);

	removeBlank();
}

void FileBrowser::handleNewMessage(QJsonObject message)
{
	int type = message.value("type").toInt();

	switch (type)
	{
	case MessageTypes::SymbolMessage:
		processEditorMessage(message);
		break;
	case MessageTypes::SendFiles:
		addFiles(message);
		break;
	case MessageTypes::NewFile:
		addFile(message);
		break;

	case MessageTypes::NewEditingUser: {
		QPair<int, QStringList> fileUserInfo = m_serializeInstance->addEditingUserUnserialize(message);
		auto it = m_textEditors.find(fileUserInfo.first);
		if (it != m_textEditors.end()) {
			it->second->addEditingUser(fileUserInfo.second);
		}
		break;
	}
	case MessageTypes::RemoveEditingUser: {
		QPair<int, int> userToRemove = m_serializeInstance->removeEditingUserUnserialize(message);
		auto it = m_textEditors.find(userToRemove.first);
		if (it != m_textEditors.end()) {
			it->second->removeEditingUser(userToRemove.second);
		}
		break;
	}
	case MessageTypes::ServerAnswer: {
		if (m_openAfterUri) {
			m_timer->stop();
			m_openAfterUri = false;
			QStringList serverMessage = m_serializeInstance->responseUnserialize(message);
			QMessageBox resultDialog(this);
			resultDialog.setInformativeText(serverMessage[1]);
			resultDialog.exec();
		}
		break;
	}
	case MessageTypes::Share: {
		//funzione per mostrare uri
		showURI(message);
		/*QStringList serverMessage = m_serializeInstance->responseUnserialize(message);
		QMessageBox resultDialog(this);
		resultDialog.setInformativeText(serverMessage[1]);
		resultDialog.exec();*/
		break;

	}
	case MessageTypes::SiteCounter: {
		QPair<int, int> fileId_siteCounter = m_serializeInstance->siteCounterUnserialize(message);
		auto it = m_textEditors.find(fileId_siteCounter.first);
		if (it != m_textEditors.end()) {
			it->second->setSiteCounter(fileId_siteCounter.second);
		}
		break;
	}
	case MessageTypes::Rename: {
		QStringList list = m_serializeInstance->renameFileUnserialize(message);

		QString oldName = list.at(0);
		QString newName = list.at(1);


		QList<QListWidgetItem*> items = ui.fileList->findItems(oldName, Qt::MatchFixedString);
		if (items.size() == 1) {
			items.at(0)->setText(newName);
		}

		break;
	}


	default:
		break;
	}
}

void FileBrowser::processEditorMessage(QJsonObject message)
{
	QPair<int, Message> m = m_serializeInstance->messageUnserialize(message);

	auto it = m_textEditors.find(m.first);
	if (it != m_textEditors.end()) {
		it->second->remoteAction(m.second);
	}
}

void FileBrowser::showURI(QJsonObject msg) {
	QString serverMessage = m_serializeInstance->URIUnserialize(msg);
	/*QInputDialog resultDialog(this);
	resultDialog.setLabelText("File Link");
	resultDialog.setTextValue(serverMessage);*/

	/*QDialog* dial = new QDialog(this);
	QLabel label(serverMessage);
	QPushButton button("copia", dial);
	connect(&button, &QPushButton::clicked, this, &FileBrowser::copia);*/


	/*dial->exec();*/

	//resultDialog.exec();
	

	/*QClipBoard *clipboard = QApplication::clipboard();*/


	Dialog* linkDialog = new Dialog(serverMessage, this);
	if (linkDialog->exec() == QDialog::Accepted) {

	}
	else {

	}
}

//void FileBrowser::on_modifyPassword_clicked(){
//	this->m_modifyPassword = new ModifyPassword(m_socketHandler); 
//	m_modifyPassword->show();
//	//connect(m_modifyProfile, &ModifyProfile::showParent, this, &FileBrowser::childWindowClosed);
//	//this->hide();
//}

void FileBrowser::on_file_clicked(){
	bool current_item = false;
	current_item = ui.fileList->currentItem()->isSelected();
	if (current_item) {
		ui.renameFile->setEnabled(true);
		ui.deleteFile->setEnabled(true);
		//connect(ui.centralWidget, &QListWidget::item, this, &FileBrowser::on_file_clicked);
	}
	else {
		ui.renameFile->setEnabled(false);
		ui.deleteFile->setEnabled(false);
	}
}

void FileBrowser::on_URI_set() {
	if (ui.uriLineEdit->text() != "") {
		ui.addSharedFileButton->setEnabled(true);
	}
	else {
		ui.addSharedFileButton->setEnabled(false);
	}
}

void FileBrowser::on_addSharedFileButton_clicked() {
	QString uri = ui.uriLineEdit->text();
	if (uri != "") {
		QByteArray message = m_serializeInstance->openSharedFileSerialize(uri, MessageTypes::OpenShare);
		//bool result = m_socketHandler->writeData(message);
		emit dataToSend(message);
		//if (result) {
		//	m_timer->setInterval(2000);
		//	m_timer->setSingleShot(true);
		//	m_timer->start();
		//}
		//else {
		//	qDebug() << m_socketHandler->getSocketState();
		//}
	}
}


//dialog per mostrare un errore di connessione se la risposta dal server non arriva in tempo
void FileBrowser::showErrorMessage() {
	QMessageBox errorDialog(this);
	errorDialog.setInformativeText("errore di connessione");
	errorDialog.exec();
	qDebug() << "messaggio di errore per il login";
}


