#include "zipper.h"
#include "unzipper.h"
#include <iostream>

int main()
{
	ziputils::zipper zip;

	zip.open("data", true);

	if(zip.isOpen())
	{
		zip.addEntry(main.cc);
	}
	zip.close();


	ziputils::unzipper uzip;
	uzip.open("data");

	if(uzip.isOpen())
	{
		const std::vector<std::string>& files = uzip.getFilenames();
		for(auto& it : files)
		{
			std::cout << it << std::endl;
		}
	}
	uzip.close();
	return 0;
}