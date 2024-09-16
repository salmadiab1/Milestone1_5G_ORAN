#include "Header1.h"
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;


/////////////////////////////////////////////////////////////////////////////////////////////////////
string HexToString(long long value) {
    stringstream ss;

    // Convert to hexadecimal and ensure we retain two digits if it's a single-digit hex value
    ss << hex << value;

    string hexStr = ss.str();

    // Check if the result is a single digit (i.e., has length 1) and add a leading zero if needed
    if (hexStr.length() % 2 != 0) {
        hexStr = "0" + hexStr;
    }

    return hexStr;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
// Function to trim leading and trailing whitespace
string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    return (start == string::npos) ? "" : str.substr(start, end - start + 1);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Function to remove the '0x' prefix from hexadecimal values
string removeHexPrefix(const string& hexValue) {
    if (hexValue.find("0x") == 0 || hexValue.find("0X") == 0) {
        return hexValue.substr(2);  // Remove the '0x' prefix
    }
    return hexValue;
}

// Function to read and parse the configuration file
bool readConfigFile(const string& fileName, EtherPacket& config) {
    ifstream configFile(fileName);
    if (!configFile.is_open()) {
        cerr << "Error: Could not open config file" << endl;
        return false;
    }

    string line;
    while (getline(configFile, line)) {
        // Remove comments
        size_t commentPos = line.find("//");
        if (commentPos != string::npos) {
            line = line.substr(0, commentPos);
        }

        // Remove leading and trailing whitespace
        line = trim(line);
        if (line.empty()) continue;  // Skip empty lines

        stringstream ss(line);
        string key;
        string value;

        getline(ss, key, '=');
        getline(ss, value);

        key = trim(key);
        value = trim(value);

        if (key == "Eth.LineRate") {
            config.lineRate = stoi(value);
        }
        else if (key == "Eth.CaptureSizeMs") {
            config.captureSizeMs = stoi(value);
        }
        else if (key == "Eth.MinNumOfIFGsPerPacket") {
            config.minNumOfIFGs = stoi(value);
        }
        else if (key == "Eth.DestAddress") {
            config.destAddress = removeHexPrefix(value);
        }
        else if (key == "Eth.SourceAddress") {
            config.sourceAddress = removeHexPrefix(value);
        }
        else if (key == "Eth.MaxPacketSize") {
            config.maxPacketSize = stoi(value);
        }
        else if (key == "Eth.BurstSize") {
            config.burstSize = stoi(value);
        }
        else if (key == "Eth.BurstPeriodicity_us") {
            config.burstPeriodicity_us = stoi(value);
        }
    }

    configFile.close();
    return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
// Function to calculate CRC-32 of a byte array
unsigned int calculateCRC32(const unsigned char* data, int length) {
    unsigned int crc = 0xFFFFFFFF; // Initial CRC value
    unsigned int polynomial = 0xEDB88320; // CRC-32 polynomial

    for (int i = 0; i < length; ++i) {
        crc ^= data[i];
        for (int j = 0; j < 8; ++j) {
            if (crc & 1) {
                crc = (crc >> 1) ^ polynomial;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc ^ 0xFFFFFFFF; // Final XOR value
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
// Function to convert a string of hex digits to a byte array
void hexStringToByteArray(const string& hexStr, unsigned char* byteArray, int& length) {
    length = hexStr.length() / 2; // 2 hex digits per byte
    for (int i = 0; i < length; ++i) {
        string byteString = hexStr.substr(i * 2, 2);
        byteArray[i] = (unsigned char)strtol(byteString.c_str(), nullptr, 16);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Function to return CRC as a hexadecimal string
string crc32ToHexString(unsigned int crc) {
    stringstream ss;
    ss << hex << setw(8) << setfill('0') << crc;
    return ss.str();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
