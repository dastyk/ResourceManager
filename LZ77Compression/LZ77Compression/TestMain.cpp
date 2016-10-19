#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unordered_map>

#define w 2047
#define offsetBits 11
#define lengthBits 5
#define lengthAnd 0x1f

#include "CompressLZ77.h"

struct HuffmanNode
{
	char byte = '\0';
	uint64_t frequency = 0;
	HuffmanNode* zeroChild = nullptr;
	HuffmanNode* oneChild = nullptr;
};

struct HuffmanTableEntry
{
	unsigned int nrOfBits = 0;
	uint32_t bits = '\0';
};

struct pointer
{
	//uint8_t offset = 0;
	//uint8_t length = 0;
	uint16_t offsetAndLength = 0;
	unsigned char byte = NULL;

	void AddHere(unsigned char* p)
	{
		int size = sizeof(uint16_t);
		memcpy(p, ((unsigned char*)&offsetAndLength) + 1, 1); // magical thingies
		memcpy(p + 1, ((unsigned char*)&offsetAndLength), 1);
		//memcpy(p + 1, &length, 1);
		memcpy(p + 2, &byte, 1);

		unsigned char check = p[0];
		unsigned char check2 = p[1];
//		p[0] = p[1];
	//	p[1] = check;
	}

};

std::vector<HuffmanNode> CountBytes(unsigned char* data, int size)
{
	std::unordered_map<unsigned char, int> bytes;
	std::vector<HuffmanNode> nodes;

	for (int i = 0; i < size; i++)
	{
		std::unordered_map<unsigned char, int>::const_iterator got = bytes.find(data[i]);

		if (got == bytes.end())
		{
			bytes[data[i]] = nodes.size();

			HuffmanNode temp;
			temp.byte = data[i];
			temp.frequency = 1;

			nodes.push_back(temp);
		}
		else
		{
			nodes[bytes[data[i]]].frequency += 1;
		}

		
	}

	printf("%d unique bytes found\n", bytes.size());
	

	return nodes;
}

std::string CreateHuffmanTable(HuffmanNode root, std::string prefix = std::string(""))
{
	RawData temp;

	if (root.oneChild == nullptr)
	{
		//We found a leaf node

		return std::string(prefix + " " + root.byte + "\n");
	}
	else
	{
		//Just send it down to the rest, adding on to the prefix as we go

		std::string returnValue;
		returnValue.append(CreateHuffmanTable(*root.oneChild, std::string(prefix + "1")));
		returnValue.append(CreateHuffmanTable(*root.zeroChild, std::string(prefix + "0")));

		return returnValue;
	}
}

pointer FindMatch(unsigned char* window, int windowSize, unsigned char* lookAhead)
{
	pointer returnValue;
	unsigned char startByte = lookAhead[0];
	unsigned char* longestMatch = new unsigned char[windowSize];
	uint16_t offsetToLongestMatch = 0;
	uint8_t longestMatchLength = 0;
	unsigned char temp[w];


	for (int i = 0; i < windowSize; i++)
	{
		int j = 0;

		while (window[i + j] == lookAhead[j] && i+j < windowSize && j < pow(2, lengthBits) - 1)
		{
			temp[j] = window[i + j];
			j++;
		}

		temp[j] = NULL;

		if (j >= longestMatchLength)
		{
			offsetToLongestMatch = windowSize - i;
			longestMatchLength = j;

			for (int k = 0; k < longestMatchLength; k++)
			{
				longestMatch[k] = temp[k];
			}


		}

		i += j - 1 * bool(j);
		
	}

	if (longestMatchLength == 0)
	{
		returnValue.byte = lookAhead[0];
	}
	else
	{
		returnValue.offsetAndLength = offsetToLongestMatch;
		returnValue.offsetAndLength = (returnValue.offsetAndLength << lengthBits);
		returnValue.offsetAndLength |= longestMatchLength;
		//returnValue.length = longestMatchLength;
	}

	delete longestMatch;
	return returnValue;

}

unsigned char reverse(unsigned char b, uint8_t nrOfBits) // Needed to simplify reading when decompressing
{
	uint8_t top = nrOfBits - 1;
	uint8_t bottom = 0;
	uint8_t topBit = 0;
	uint8_t bottomBit = 0;

	if (nrOfBits >= 2)
	{
		while (top > bottom)
		{
			topBit = (b >> top) & 1;
			bottomBit = (b >> bottom) & 1;

			b ^= (-bottomBit ^ b) & (1 << top);
			b ^= (-topBit ^ b) & (1 << bottom);

			top--;
			bottom++;
		}
	}
	return b;
}

CompressedData HuffmanEncode(std::string table, RawData rData)
{
	//unsigned char check = reverse(3, 3);

	std::unordered_map<unsigned char, HuffmanTableEntry> translatedTable;
	HuffmanTableEntry temp;
	uint32_t numberTranslated = 0;
	unsigned int nrOfBitsFound = 0;

	for (int i = 0; i < table.size(); i++)
	{
		if (table[i] == ' ')
		{
			unsigned char byte = table[i + 1];
			temp.bits = numberTranslated;
			temp.nrOfBits = nrOfBitsFound;

			translatedTable[byte] = temp;
			numberTranslated = 0;
			nrOfBitsFound = 0;
			i += 2;
		}
		else
		{
			nrOfBitsFound++;
			numberTranslated *= 2;
			numberTranslated += table[i] - '0';
		}
	}

	//uint8_t posInByte = 0;
	uint8_t remainingBitsToWrite = 0;
	uint8_t remainingSpaceInByte = 8;
	std::vector<unsigned char> cData;
	char activeByte = '\0';
	int totalBitsWritten = 0;
	int totalBytesAllocated = 1;

	for (int i = 0; i < rData.size; i++)
	{
		HuffmanTableEntry entry = translatedTable[rData.data[i]];
		remainingBitsToWrite = entry.nrOfBits;
		//remainingSpaceInByte = 8 - posInByte;
		

		if(remainingSpaceInByte >= remainingBitsToWrite)
		{
			unsigned char bits = reverse(entry.bits, entry.nrOfBits);
			bits = bits << (8 - remainingSpaceInByte);
			activeByte |= bits;
			remainingSpaceInByte -= remainingBitsToWrite;
			totalBitsWritten += remainingBitsToWrite;
		}
		else
		{
			unsigned char bits = reverse(entry.bits, entry.nrOfBits);
			unsigned int bitsWritten = 0;

			while (remainingBitsToWrite > 0)
			{
				activeByte |= (bits << (8 - remainingSpaceInByte));
				bits = bits >> remainingSpaceInByte;

				bitsWritten = __min(remainingBitsToWrite, remainingSpaceInByte);
				remainingSpaceInByte -= bitsWritten;

				if (remainingSpaceInByte == 0)
				{
					remainingSpaceInByte = 8;
					cData.push_back(activeByte);
					activeByte = '\0';
					totalBytesAllocated++;
				} 

				totalBitsWritten += bitsWritten;

				remainingBitsToWrite -= bitsWritten;
			}

			
		}


	}

	if(activeByte != NULL)
		cData.push_back(activeByte);

	//Add on the table to the compressed data and then add the compressed data itself

	CompressedData returnData;
	returnData.sizeCompressed = cData.size() + sizeof(uint8_t) + table.size();
	returnData.sizeUncompressed = rData.size;
	returnData.data = new unsigned char[returnData.sizeCompressed];
	uint8_t nrOfEntries = (uint8_t)translatedTable.size();

	int bit = cData[13];

	memcpy(returnData.data, &nrOfEntries, sizeof(uint8_t));

	for (int i = sizeof(uint8_t); i < table.size() + sizeof(uint8_t); i++)
	{
		returnData.data[i] = table[i - sizeof(uint8_t)];
	}

	for (int i = table.size() + sizeof(uint8_t); i < cData.size() + table.size() + sizeof(uint8_t); i++)
	{
		returnData.data[i] = cData[i - (table.size() + sizeof(uint8_t))];
	}

	return returnData;
}

CompressedData CompressHuffman(RawData rData)
{
	//https://distributedalgorithm.wordpress.com/2016/01/02/huffman-coding-the-optimal-prefix-code/
	printf("Started Huffman Compression\n");
	std::vector<HuffmanNode> nodes = CountBytes(rData.data, rData.size);
	HuffmanNode* temp1;
	HuffmanNode* temp2;
	long long int position;

	//while (nodes.size())
	//{
	//	temp.frequency = UINT64_MAX;
	//	position = -1;

	//	for (int i = 0; i < nodes.size(); i++)
	//	{
	//		if (nodes[i].frequency < temp.frequency)
	//		{
	//			temp = nodes[i];
	//			position = i;
	//		}
	//	}
	//	
	//	sortedList.push_back(temp); // KRASCHAR PÅ nodes size = 10
	//	nodes.erase(nodes.begin() + position);
	//}

	//---------------------------------------------------------------------------------------------------------------
	//create the tree

	while (nodes.size() > 1)
	{
		temp1 = new HuffmanNode;
		temp1->frequency = UINT64_MAX;
		position = -1;

		for (int i = 0; i < nodes.size(); i++)
		{
			if (nodes[i].frequency < temp1->frequency)
			{
				*temp1 = nodes[i];
				position = i;
			}
		}

		nodes.erase(nodes.begin() + position);

		temp2 = new HuffmanNode;
		temp2->frequency = UINT64_MAX;
		position = -1;

		for (int i = 0; i < nodes.size(); i++)
		{
			if (nodes[i].frequency < temp2->frequency)
			{
				*temp2 = nodes[i];
				position = i;
			}
		}
		
		nodes.erase(nodes.begin() + position);


		HuffmanNode temp3;
		temp3.oneChild = temp1;
		temp3.zeroChild = temp2;
		temp3.frequency = temp1->frequency + temp2->frequency;

		nodes.push_back(temp3);
	}

	//--------------------------------------------------------------------------------------------------------------
	//Create the table

	std::string table = CreateHuffmanTable(nodes[0]);

	//--------------------------------------------------------------------------------------------------------------
	
	CompressedData cData = HuffmanEncode(table, rData);
	printf("Huffman Compression complete\n");
	return cData;
}

RawData UncompressHuffman(CompressedData cData)
{
	//Get the information stored in the start of the data
	printf("Started Huffman Uncompression\n");
	uint8_t nrOfEntries = cData.data[0];
	uint8_t foundEndLines = 0;
	std::string table;
	unsigned int counter = 1;
	char readByte;

	while (foundEndLines < nrOfEntries)
	{
		readByte = (char)cData.data[counter];
		table += readByte;

		if (readByte == '\n')
			foundEndLines++;

		counter++;
	}

	std::unordered_map<unsigned char, HuffmanTableEntry> translatedTable;
	std::unordered_map<std::string, unsigned char> translationTable;
	HuffmanTableEntry temp;
	unsigned int numberTranslated = 0;
	std::string identifier("");
	unsigned int nrOfBitsFound = 0;

	for (int i = 0; i < table.size(); i++)
	{
		if (table[i] == ' ')
		{

			translationTable[identifier] = table[i + 1];
			identifier.clear();
			i += 2;
		}
		else
		{
			identifier += table[i];
		}
	}

	uint8_t bit = 0;
	uint8_t posInByte = 0;
	std::string foundCode("");

	RawData rData;
	rData.data = new unsigned char[cData.sizeUncompressed];
	rData.size = cData.sizeUncompressed;
	unsigned int uncompressedCounter = 0;
	
	while (counter < cData.sizeCompressed)
	{
		bit = (cData.data[counter] >> posInByte) & 1;
		foundCode += char('0' + bit);

		std::unordered_map<std::string, unsigned char>::const_iterator got = translationTable.find(foundCode);

		if (got == translationTable.end())
		{
			//not found, so keep looking
		}
		else
		{
			rData.data[uncompressedCounter] = got->second;
			uncompressedCounter++;
			foundCode.clear();
		}

		posInByte++;

		if (posInByte == 8)
		{
			posInByte = 0;
			counter++;
		}
	}

	printf("Huffman Uncompression complete\n");
	return rData;

}



void CompressLz77(RawData rData, CompressedData* cdata)
{
	printf("Compressing\n");
	unsigned char* inputStream = rData.data;
	int sizeOfInput = rData.size;

	uint64_t codingPosition = 0;
	unsigned char* lookAheadBuffer = inputStream;
	uint64_t wa = w;
	unsigned char* window = new unsigned char[w];
	int currentWorldSize = 0;
	std::vector<pointer> pointers;

	int percentDone = 1;

	while (codingPosition < sizeOfInput)
	{
		pointers.push_back(FindMatch(window, currentWorldSize, lookAheadBuffer + codingPosition));

		uint8_t length = (pointers[pointers.size() - 1].offsetAndLength & lengthAnd);

		if ((100.0f * codingPosition) / sizeOfInput > percentDone)
		{
			printf("%d percent complete\n", percentDone);
			percentDone++;
		}

		if (length == 0)
		{
			if (currentWorldSize + 1 >= w)
			{
				currentWorldSize = 0;

				for (int i = 0; i < w; i++)
				{
					window[i] = NULL;
				}
			}

			window[currentWorldSize] = lookAheadBuffer[codingPosition];
			codingPosition++;
			currentWorldSize++;
		}
		else
		{

			if ((currentWorldSize + length) >= w)
			{
				currentWorldSize = 0;

				for (int i = 0; i < w; i++)
				{
					window[i] = NULL;
				}
			}

			for (int i = 0; i < length; i++)
			{
				window[currentWorldSize] = lookAheadBuffer[codingPosition];
				codingPosition++;
				currentWorldSize++;
			}

		}

	}
	printf("Compression Complete\n");
	unsigned char* compressed = new unsigned char[pointers.size() * 3];

	for (int i = 0; i < pointers.size(); i++)
	{
		pointers[i].AddHere(compressed + i * 3);

		//uint8_t a = compressed[i*3];
		//uint8_t b = compressed[i*3 + 1];
		//char c = compressed[i*3 + 2];

		//printf("%d,", a);
		//printf("%d,", b);
		//printf("%c\n", c);
	}

	cdata->data = compressed;
	cdata->sizeCompressed = pointers.size() * 3;
	cdata->sizeUncompressed = sizeOfInput;

}

void UncompressLz77(CompressedData cData, RawData* data)
{
	printf("Uncompressing\n");
	int currentPos = 0;
	unsigned char* endData = new unsigned char[cData.sizeUncompressed];

	for (int i = 0; i < cData.sizeCompressed; i += 3)
	{

		int a = 0;
		a++;

		if ((cData.data[i + 1] & lengthAnd) == 0)
		{
			endData[currentPos] = cData.data[i + 2];
			currentPos++;
		}
		else
		{
			uint8_t length = (((uint8_t)cData.data[i + 1]) & lengthAnd);
			//uint16_t offset = ((uint16_t)((cData.data[i] << 8) | cData.data[i]) >> (16 - 11));
			uint16_t offset = (uint8_t)cData.data[i];
			//offset = offset >> 5;
			offset = (offset << 3); // 3 because up 8 and then down 5 = 3 up (to align to 11 bit)
			offset |= (cData.data[i + 1] >> (16 - 11));

			memcpy(endData + currentPos, endData + currentPos - offset, length);
			currentPos += length;
		}
	}
	printf("Uncompression Complete\n");


	data->data = endData;
	data->size = cData.sizeUncompressed;



}
//
//int main()
//{
//	//https://msdn.microsoft.com/en-us/library/ee916854.aspx
//
//	RawData rData;
//	std::ifstream t1;
//	t1.open("kran.obj", std::ios::binary);      // open input file
//	t1.seekg(0, std::ios::end);    // go to the end
//	rData.size = t1.tellg();           // report location (this is the length)
//	t1.seekg(0, std::ios::beg);    // go back to the beginning
//	rData.data = new unsigned char[rData.size];    // allocate memory for a buffer of appropriate dimension
//	t1.read((char*)rData.data, rData.size);       // read the whole file into the buffer
//	t1.close();                    // close file handle
//
//	//Huffman(rData);
//
//	RawData testData;
//	testData.size = 100;
//	testData.data = new unsigned char[100];
//	for (int i = 0; i < 20; i++)
//	{
//		testData.data[i] = 'a';
//	}
//	for (int i = 0; i < 15; i++)
//	{
//		testData.data[i + 20] = 'b';
//	}
//	for (int i = 0; i < 5; i++)
//	{
//		testData.data[i + 35] = 'c';
//	}
//	for (int i = 0; i < 15; i++)
//	{
//		testData.data[i + 40] = 'd';
//	}
//	for (int i = 0; i < 45; i++)
//	{
//		testData.data[i + 55] = 'e';
//	}
//
//	//testData = UncompressHuffman(CompressHuffman(rData));
//
//	//-----------------------------------------------------------------------------------------------------------------------------
//
//	CompressedData compressedData = CompressLz77(rData);
//	float sizeDiff1 = compressedData.sizeCompressed / (compressedData.sizeUncompressed * 1.0f);
//	printf("The compressed file should be %f percent of the original size\n", sizeDiff1 * 100);
//
//	std::ofstream compressedFile("compressed.jsc", std::ios::binary);
//	if (compressedFile.is_open())
//	{
//		compressedFile.write((char*)&compressedData.sizeCompressed, sizeof(uint64_t));
//		compressedFile.write((char*)&compressedData.sizeUncompressed, sizeof(uint64_t));
//		compressedFile.write((char*)compressedData.data, compressedData.sizeCompressed);
//		compressedFile.close();
//	}
//
//	RawData compressedFileData;
//	std::ifstream t2;
//	t2.open("compressed.jsc", std::ios::binary);      // open input file
//	t2.seekg(0, std::ios::end);    // go to the end
//	compressedFileData.size = t2.tellg();           // report location (this is the length)
//	t2.seekg(0, std::ios::beg);    // go back to the beginning
//	compressedFileData.data = new unsigned char[compressedFileData.size];    // allocate memory for a buffer of appropriate dimension
//	t2.read((char*)compressedFileData.data, compressedFileData.size);
//	t2.close();
//
//	CompressedData testFileToUncompress;
//	memcpy(&testFileToUncompress.sizeCompressed, compressedFileData.data, sizeof(uint64_t));
//	memcpy((char*)&testFileToUncompress.sizeUncompressed, compressedFileData.data + sizeof(uint64_t), sizeof(uint64_t));
//
//	testFileToUncompress.data = new unsigned char[testFileToUncompress.sizeCompressed];
//	memcpy(testFileToUncompress.data, compressedFileData.data + sizeof(uint64_t) * 2, testFileToUncompress.sizeCompressed);// KRASCHAR HÄR!!!
//
//	//------------------------------------------------------------------------------------------------------------------------------
//
//
//	RawData uncompressedData = UncompressLz77(testFileToUncompress);
//
//	bool foundError = false;
//	for (int i = 0; i < rData.size; i++)
//	{
//		if (rData.data[i] == uncompressedData.data[i])
//		{
//			int apa = 1;
//		}
//		else
//		{
//			printf("Non matching bytes found!\n");
//			foundError = true;
//		}
//	}
//
//	if (!foundError)
//		printf("No errors found!\n");
//
//	std::ofstream myfile1("testfile.obj", std::ios::binary);
//	if (myfile1.is_open())
//	{
//		myfile1.write((char*)uncompressedData.data, uncompressedData.size);
//		myfile1.close();
//	}
//	else std::cout << "Unable to open file";
//
//	return 0;
//
//}
