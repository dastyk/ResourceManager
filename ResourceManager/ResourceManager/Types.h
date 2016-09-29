#pragma once
#include <stdint.h>
enum ObjectType : uint8_t
{
	Mesh,
	Texture,
	Sound,
	Video
};

enum FileType : uint8_t
{
	obj,
	fbx,
	wav,
	ogg,
	mp3,
	mp4
};