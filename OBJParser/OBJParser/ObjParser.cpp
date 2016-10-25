#include <fstream>
#include <iostream>
#include "ObjParser.h"
#include "parser.tab.h"


extern Object o;
extern FILE * yyin;
std::vector<std::string> errors;

void yy::parser::error(std::string const&err)
{
	//std::cout << "It's one of the bad ones... " << err << std::endl;
	errors.push_back(err);
}

int ParseObj(const char* file, ArfData::Data& data, ArfData::DataPointers& pointers)
{
	o.Clear();
	o.New();
	fopen_s(&yyin, file, "r");
	if (yyin == NULL)
	{
		o.Clear();
		return -2;
	}
	errors.clear();
	
	yy::parser parser;
	if (parser.parse())
	{
		for (auto s : errors)
			std::cout << s << std::endl;
		o.Clear();
		return -1;
	}

	data = o._data;
	pointers = o._datap;

	fclose(yyin);

	return 1;
}

int ParseObj(const void* rawData, uint32_t size, ArfData::Data& data, ArfData::DataPointers& pointers)
{
	o.Clear();
	o.New();
	if (rawData == nullptr)
	{
		o.Clear();
		return -2;
	}
	errors.clear();
	
	fopen_s(&yyin, "temp", "w");
	fclose(yyin);
	fopen_s(&yyin, "temp", "r");

	setvbuf(yyin, (char*)rawData, _IONBF, size);

	yy::parser parser;
	if (parser.parse())
	{
		for (auto s : errors)
			std::cout << s << std::endl;
		o.Clear();
		return -1;
	}

	data = o._data;
	pointers = o._datap;

	fclose(yyin);
	remove("temp");


	return 1;
}

