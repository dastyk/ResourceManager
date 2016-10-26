#pragma once


#include "ArfData.h"

int __declspec(dllimport) ParseObj(const char* filename, ArfData::Data* data, ArfData::DataPointers* dataPointers);
int __declspec(dllimport) ParseObj(const void* rawData, size_t size, ArfData::Data* data, ArfData::DataPointers* dataPointers);