#pragma once

#include "ArfData.h"
int ParseObj(const char* file, ArfData::Data& data, ArfData::DataPointers& pointers);
int ParseObj(const void* rawData, uint32_t size, ArfData::Data& data, ArfData::DataPointers& pointers);