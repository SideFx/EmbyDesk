/////////////////////////////////////////////////////////////////////////////
// Name:       sqlreturntypes.h
// Purpose:    Struct definitions (data + result code/message)
// Author:     Jan Buchholz
// Created:    2026-05-16
// Changed:    2026-05-17
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "metatypes.h"
#include "jbparser.hpp"

struct EmbyCollectionResult {
    std::vector<EmbyCollection> collections;
    int code = -1;
    QString message;
};

struct VectorStringResult {
    std::vector<std::string> stringList;
    int code = -1;
    std::string message;
};

struct ByteArrayResult {
    std::vector<uint8_t> bytes;
    int code = -1;
    std::string message;
};

struct FolderDataResult {
    std::vector<FolderDataInc> tFolderData;
    int code = -1;
    std::string message;
};
