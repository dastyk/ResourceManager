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
struct CompressedData
{
	uint64_t sizeCompressed;
	uint64_t sizeUncompressed;
	unsigned char* data;
};

struct RawData
{
	RawData()
	{
		size = 0;
		data = nullptr;
	}

	RawData(uint64_t s, unsigned char* d)
	{
		size = s;
		data = d;
	}

	uint64_t size;
	unsigned char* data;
};
struct CompressedData32
{
	uint64_t sizeCompressed;
	uint64_t sizeUncompressed;
	uint32_t* data;
};

struct HuffmanNode
{
	char byte = '\0';
	uint64_t frequency = 0;
	HuffmanNode* zeroChild = nullptr;
	HuffmanNode* oneChild = nullptr;
};

struct HuffmanTableEntry
{
	uint32_t nrOfBits = 0;
	uint32_t bits = '\0';

	std::size_t operator()() const
	{
		uint64_t rValue = nrOfBits;
		rValue << 16;
		rValue += bits;
		return rValue;
	}

	bool operator==(const HuffmanTableEntry& other) const
	{
		return (this->nrOfBits == other.nrOfBits && this->bits == other.bits);
	}
};

namespace std
{
	template<> struct hash<HuffmanTableEntry>
	{
		typedef HuffmanTableEntry argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const
		{
			uint64_t rValue = s.nrOfBits;
			rValue << 16;
			rValue += s.bits;
			return rValue;
		}
	};
}

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

void CompressLz77(void* rdata, uint64_t size, uint64_t* csize, void** cdata)
{
	printf("Compressing\n");
	unsigned char* inputStream = (unsigned char*)rdata;
	int sizeOfInput = size;

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
	}

	*cdata = compressed;
	*csize = pointers.size() * 3;

}

void UncompressLz77(void* rdata, uint64_t csize, void* cdata)
{
	printf("Uncompressing\n");
	int currentPos = 0;
	unsigned char* endData = (unsigned char*)rdata;
	unsigned char* data = (unsigned char*)cdata;
	for (int i = 0; i < csize; i += 3)
	{

		int a = 0;
		a++;

		if ((data[i + 1] & lengthAnd) == 0)
		{
			endData[currentPos] = data[i + 2];
			currentPos++;
		}
		else
		{
			uint8_t length = (((uint8_t)data[i + 1]) & lengthAnd);
			uint16_t offset = (uint8_t)data[i];
			offset = (offset << 3); // 3 because up 8 and then down 5 = 3 up (to align to 11 bit)
			offset |= (data[i + 1] >> (16 - 11));

			memcpy(endData + currentPos, endData + currentPos - offset, length);
			currentPos += length;
		}
	}
	printf("Uncompression Complete\n");

}

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

uint32_t reverse(uint32_t b, uint8_t nrOfBits) // Needed to simplify reading when decompressing
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

void HuffmanEncode(std::string table, void* rdata, uint64_t size, uint64_t* csize, void** cdata)
{

	std::unordered_map<unsigned char, HuffmanTableEntry> translatedTable;
	HuffmanTableEntry temp;
	uint32_t numberTranslated = 0;
	unsigned int nrOfBitsFound = 0;
	unsigned char* rData = (unsigned char*)rdata;
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

	uint8_t remainingBitsToWrite = 0;
	uint8_t remainingSpaceInByte = sizeof(uint32_t) * 8;
	std::vector<uint32_t> cData;
	uint32_t activeByte = '\0';
	int totalBitsWritten = 0;
	int totalBytesAllocated = 1;

	for (int i = 0; i < size; i++)
	{
		HuffmanTableEntry entry = translatedTable[rData[i]];
		remainingBitsToWrite = entry.nrOfBits;


		if (remainingSpaceInByte >= remainingBitsToWrite)
		{
			uint32_t bits = reverse(entry.bits, entry.nrOfBits);
			bits = bits << (sizeof(uint32_t) * 8 - remainingSpaceInByte);
			activeByte |= bits;
			remainingSpaceInByte -= remainingBitsToWrite;
			totalBitsWritten += remainingBitsToWrite;
		}
		else
		{
			uint32_t bits = reverse(entry.bits, entry.nrOfBits);
			unsigned int bitsWritten = 0;

			while (remainingBitsToWrite > 0)
			{
				if (remainingSpaceInByte == 0)
				{
					remainingSpaceInByte = sizeof(uint32_t) * 8;
					cData.push_back(activeByte);
					activeByte = '\0';
					totalBytesAllocated++;
				}

				activeByte |= (bits << (sizeof(uint32_t) * 8 - remainingSpaceInByte));
				bits = bits >> remainingSpaceInByte;

				bitsWritten = __min(remainingBitsToWrite, remainingSpaceInByte);
				remainingSpaceInByte -= bitsWritten;

				totalBitsWritten += bitsWritten;

				remainingBitsToWrite -= bitsWritten;
			}


		}


	}

	if (activeByte != NULL)
		cData.push_back(activeByte);

	//Add on the table to the compressed data and then add the compressed data itself


	*csize = cData.size() * sizeof(uint32_t) + sizeof(uint16_t) + table.size() * sizeof(uint8_t);

	*cdata = new uint32_t[*csize];
	uint16_t nrOfEntries = (uint16_t)translatedTable.size();

	memcpy(*cdata, &nrOfEntries, sizeof(uint16_t));

	unsigned int counter = 0;
	while (counter < table.size())
	{
		memcpy((unsigned char*)*cdata + sizeof(uint16_t) + counter, &table[counter], sizeof(char));
		counter++;
	}

	unsigned int counter2 = 0;
	while (counter2 < cData.size())
	{
		memcpy(((unsigned char*)*cdata) + sizeof(uint16_t) + counter + counter2 * sizeof(uint32_t), &cData[counter2], sizeof(uint32_t));
		counter2++;
	}
}

void CompressHuffman(void* rdata, uint64_t size, uint64_t* csize, void** cdata)
{
	//https://distributedalgorithm.wordpress.com/2016/01/02/huffman-coding-the-optimal-prefix-code/
	printf("Started Huffman Compression\n");
	std::vector<HuffmanNode> nodes = CountBytes((unsigned char*)rdata, size);
	HuffmanNode* temp1;
	HuffmanNode* temp2;
	long long int position;

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

	HuffmanEncode(table, (unsigned char*)rdata, size, csize, cdata);
	printf("Huffman Compression complete\n");
}

void UncompressHuffman(void* rdata, uint64_t size, void* cdata)
{
	//Get the information stored in the start of the data
	printf("Started Huffman Uncompression\n");
	uint16_t foundEndLines = 0;
	std::string table;
	unsigned int counter = 2;
	char readByte;
	uint32_t* cData = (uint32_t*)cdata;
	uint16_t nrOfEntries = 0;
	memcpy(&nrOfEntries, &cData[0], sizeof(uint16_t));

	while (foundEndLines < nrOfEntries)
	{
		memcpy(&readByte, (unsigned char*)cData + sizeof(unsigned char) * counter, sizeof(unsigned char));

		table += readByte;

		if (readByte == '\n')
		{
			memcpy(&readByte, (unsigned char*)cData + sizeof(unsigned char) * counter + 1, sizeof(unsigned char));

			if (readByte != '\n')
				foundEndLines++;
		}

		counter++;
	}

	std::unordered_map<unsigned char, HuffmanTableEntry> translatedTable;
	std::unordered_map<std::string, unsigned char> translationTable;
	std::unordered_map<HuffmanTableEntry, unsigned char> translationTable2; // might be reversed, again?
	HuffmanTableEntry temp;
	uint32_t numberTranslated = 0;
	std::string identifier("");
	HuffmanTableEntry identifier2;
	uint32_t nrOfFoundBits = 0;
	uint32_t nrOfBitsFound = 0;

	for (int i = 0; i < table.size(); i++)
	{
		if (table[i] == ' ')
		{

			translationTable[identifier] = table[i + 1];
			identifier.clear();

			if (table[i + 1] == '#')
			{
				int a = 0;
				a++;
			}

			translationTable2[identifier2] = table[i + 1];
			identifier2.bits = 0;
			identifier2.nrOfBits = 0;
			nrOfFoundBits = 0;

			i += 2;
		}
		else
		{
			identifier += table[i];

			identifier2.bits += (table[i] - '0') * pow(2, nrOfFoundBits);
			identifier2.nrOfBits++;
			nrOfFoundBits++;
		}
	}

	uint32_t bit = 0;
	uint8_t posInByte = 0;
	std::string foundCode("");

	unsigned int uncompressedCounter = 0;

	//--------------------------------------------------------------------------------------------------------------

	unsigned int maxBits = sizeof(uint32_t) * 8;
	unsigned int totalNumber = 0;
	unsigned int translatedBytes = 0;
	unsigned int counter2 = 0;
	uint8_t lastFoundPos = 0;
	uint32_t mask = 0;
	uint32_t data = NULL;
	HuffmanTableEntry toCheck;
	uint64_t readBits = 0;
	unsigned char* rData = (unsigned char*)rdata;
	while (translatedBytes < size)
	{
		memcpy(&data, (unsigned char*)cData + counter + counter2 * sizeof(uint32_t), sizeof(uint32_t));

		bit = (data >> posInByte) & 1;
		toCheck.bits |= bit << toCheck.nrOfBits;
		toCheck.nrOfBits++;

		std::unordered_map<HuffmanTableEntry, unsigned char>::const_iterator got = translationTable2.find(toCheck);

		if (got == translationTable2.end())
		{
			//not found, so keep looking
		}
		else
		{
			rData[uncompressedCounter] = got->second;
			mask = 0;
			toCheck.bits = 0;
			toCheck.nrOfBits = 0;
			uncompressedCounter++;
			translatedBytes++;
		}

		posInByte++;
		readBits++;

		if (posInByte == sizeof(uint32_t) * 8)
		{
			posInByte = 0;
			counter2++;
		}

	}


	printf("Huffman Uncompression complete\n");

}

//int main()
//{
//	std::ifstream t1;
//	t1.open("Sphere0.arf", std::ios::binary);      // open input file
//	t1.seekg(0, std::ios::end);    // go to the end
//	uint64_t rSize = t1.tellg();           // report location (this is the length)
//	t1.seekg(0, std::ios::beg);    // go back to the beginning
//	unsigned char* rData = new unsigned char[rSize];    // allocate memory for a buffer of appropriate dimension
//	t1.read((char*)rData, rSize);       // read the whole file into the buffer
//	t1.close();
//
//	uint64_t lz77sizeC = 0;
//	void* lz77Data;
//	CompressLz77(rData, rSize, &lz77sizeC, &lz77Data);
//
//	uint64_t huffsizeC = 0;
//	void* huffData;
//	CompressHuffman(lz77Data, lz77sizeC, &huffsizeC, &huffData);
//
//	UncompressHuffman(lz77Data, lz77sizeC, huffData);
//
//}


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
//	CompressedData compressedData;
//	CompressLz77(rData.data, rData.size, &compressedData.sizeCompressed, &compressedData.sizeUncompressed, (void**)&compressedData.data);
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
//	RawData uncompressedData;
//	UncompressLz77((void**)&uncompressedData.data, &uncompressedData.size, testFileToUncompress.sizeCompressed, testFileToUncompress.sizeUncompressed, testFileToUncompress.data);
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
