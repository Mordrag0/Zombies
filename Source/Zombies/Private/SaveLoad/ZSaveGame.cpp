// Copyright 2026 Luka Markuš. All rights reserved.


#include "SaveLoad/ZSaveGame.h"

const FGuid FZSaveWorldVersion::GUID(0x12345678, 0x90AB, 0xCDEF, 0x11223344);

FCustomVersionRegistration GRegisterZSaveWorldVersion(
	FZSaveWorldVersion::GUID,
	FZSaveWorldVersion::LatestVersion,
	TEXT("ZSaveWorldVersion"));
