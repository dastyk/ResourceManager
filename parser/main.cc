#include <iostream>
#include <fstream>

#include <map>

using namespace std;
#include "binary.tab.hh"
extern unsigned int total;
extern Object o;
extern FILE * yyin;
void yy::parser::error(std::string const&err)
{
  //std::cout << "It's one of the bad ones... " << err << std::endl;
	std::cout << err << std::endl;
}

int main(int argc, char **argv)
{
	if(argc > 1)
	{
		yyin = nullptr;
		cout << "Args > 1 File: " << argv[1] << endl;
		if(argc > 2)
			cout << "Args > 2 Output: " << argv[2] << endl;
		else
			cout << "Output: out.arf" << endl;
		yyin = fopen(argv[1], "r");
		if(!yyin)
		{
			cout << "Failed to open file." << endl;
			return -1;
		}

		cout << "Compiling..." << endl << endl;
		yy::parser parser;


		if(!parser.parse())  
		{
			cout << "Compiled" << endl;
			o.Print();
			ofstream file;
			if(argc > 2)
				file.open(argv[2], ios::binary | ios::trunc);
			else
				file.open("out.arf", ios::binary | ios::trunc);
			o.GenerateArf(file);
			file.close();
		}
	}

	return 0;
}