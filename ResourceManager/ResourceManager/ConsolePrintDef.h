#pragma once

#ifndef PrintDebugString
#ifdef _DEBUG
#define PrintDebugString(x, ...) printf(x, __VA_ARGS__)
#else
#define PrintDebugString
#endif
#endif