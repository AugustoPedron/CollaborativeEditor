#pragma once
/*----------------------------------------------------------------------------------------------------
file che contiene tutte le define che vengono usate per capire che messaggi arrivano sul client/server
------------------------------------------------------------------------------------------------------*/

enum MessageTypes {
	LoginRequest,
	Logout,
	Register,
	Filename,
	SymbolMessage,
	Text,
	Image,
	NewFile,
	Open,
	Close,
	CursorPosition,
	ServerAnswer,
	Delete,
	Rename,
	Share,
	OpenShare,
	SendFiles,
	ChangePassword,
	ChangeProfile,
	NewEditingUser,
	RemoveEditingUser,
	SiteCounter
};