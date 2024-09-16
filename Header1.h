#pragma once
#ifndef FUNCTIONS
#define FUNCTIONS

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

//Global Variables 
const long long preamble = 0xFB555555555555;
const int SFD = 0xD5;
const int ethertype = 0xAEFE;
const int payload = 0x00;
const int IFG = 0x07;



class EtherPacket{
public:
    int lineRate;              // gigabit/sec 
    int captureSizeMs ;
    int minNumOfIFGs ;
    string destAddress ;
    string sourceAddress ;
    int maxPacketSize ;
    int burstSize ;
    int burstPeriodicity_us ;   //microsec
    
};

// Function prototypes
bool readConfigFile(const string& fileName, EtherPacket& config);
string trim(const string& str);
string removeHexPrefix(const string& hexValue);
string HexToString(long long value);
string crc32ToHexString(unsigned int crc);
void hexStringToByteArray(const string& hexStr, unsigned char* byteArray, int& length);
unsigned int calculateCRC32(const unsigned char* data, int length);

#endif