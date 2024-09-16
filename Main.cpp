#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include<cmath>
#include "Header1.h"
using namespace std;


int main() {
    string configFileName = "first_milestone.txt"; // Configuration file name
    EtherPacket config;

    // Read and parse the configuration file
    if (!readConfigFile(configFileName, config)) {
        cerr << "Configuration loading failed." << endl;
        return 1;
    }

    // Print the loaded configuration
    cout << "Line Rate: " << config.lineRate << endl;
    cout << "Capture Size (ms): " << config.captureSizeMs << endl;
    cout << "Min Number of IFGs: " << config.minNumOfIFGs << endl;
    cout << "Dest Address: " << config.destAddress << endl;
    cout << "Source Address: " << config.sourceAddress << endl;
    cout << "Max Packet Size: " << config.maxPacketSize << endl;
    cout << "Burst Size: " << config.burstSize << endl;
    cout << "Burst Periodicity (us): " << config.burstPeriodicity_us << endl;


    
////////////////////////////////////////////////////////////////////////////////////////////////////    
//Calculations on data based on config file
long long Tot_Data_Bytes = (( config.lineRate* 1e9 ) / 8) * (config.captureSizeMs* 1e-3);
cout<<"Total number of data bytes is "<<Tot_Data_Bytes<<" bytes"<<endl;
long long  IFG_Bytes_no = (( config.lineRate* 1e9 ) / 8) * config.burstPeriodicity_us /1000000 ;
cout<<"Total number of IFG bytes is "<<IFG_Bytes_no<<" bytes"<<endl;
int No_Bursts = floor(Tot_Data_Bytes/(IFG_Bytes_no + (config.burstSize*(config.maxPacketSize + config.minNumOfIFGs ))));
cout<<"Total number of Bursts is "<<No_Bursts<<endl;
int IFG_extra_bytes = Tot_Data_Bytes - (No_Bursts * ((config.burstSize* (config.maxPacketSize + config.minNumOfIFGs)) + IFG_Bytes_no));
cout << "Total number of IFG_extra_bytes is " << IFG_extra_bytes << endl;

////////////////////////////////////////////////////////////////////////////////////////////////////    
    //Converting hex to string 
    string preamble_str = HexToString(preamble); 
    string SFD_str = HexToString(SFD);  
    string ethertype_str = HexToString(ethertype); 
    string payload_str = HexToString(payload); 
    string IFG_str = HexToString(IFG); 


////////////////////////////////////////////////////////////////////////////////////////////////////    
    //calculating payload size
    int paylaod_size = config.maxPacketSize - 26; 
    string Payload_new ;
    for (int i =0 ; i< paylaod_size ;i++){
        Payload_new += payload_str ;
    }


////////////////////////////////////////////////////////////////////////////////////////////////////    
    string Packet_No_CRC = preamble_str + SFD_str + config.destAddress + config.sourceAddress + 
                           ethertype_str + Payload_new; 
    
////////////////////////////////////////////////////////////////////////////////////////////////////    
    //Calculating CRC field 
    // Allocate memory dynamically based on input size
    int length ;
    unsigned char* byteArray = new unsigned char[config.maxPacketSize ];

    // Convert hex string to byte array
    hexStringToByteArray(Packet_No_CRC, byteArray, length);

    // Calculate CRC-32
    unsigned int crc = calculateCRC32(byteArray, length);

    // Convert CRC to a hex string and display
    string CRC_str = crc32ToHexString(crc);
    cout << "CRC: 0x" << CRC_str << endl<<endl;

    // Free dynamically allocated memory
    delete[] byteArray;

//////////////////////////////////////////////////////////////////////////////////////////////////// 
    string IFG_APPENDED; 

    //calculate IFGs after each packet based on config file
    for (int z = 0; z < config.minNumOfIFGs ; z++) {
        IFG_APPENDED += IFG_str ;
    }

    //add paddding IFGs if the min IFGs are not 4 byte alligned
    if (config.minNumOfIFGs % 4 != 0) {
        int add_ifg = config.minNumOfIFGs;
        // Continue adding IFG_str until add_ifg is a multiple of 4
        while (add_ifg % 4 != 0) {
            IFG_APPENDED += IFG_str;  
            add_ifg++;
        }
    }

    string Packet_CRC = Packet_No_CRC + CRC_str + IFG_APPENDED ;
   

//////////////////////////////////////////////////////////////////////////////////////////////////
    // Open the output file
    ofstream outFile("output.txt");
    if (!outFile) {
        cerr << "Error opening file for writing." << endl;
        return 1;
    }

    //each char is a byte so we have 1500 char
    char* array = new char[Packet_CRC.size()];     //initialize dynamic array with max packet size
    int count = 1;
    while (count <= No_Bursts) {                                    // Loop on number of bursts
        for (int j = 0; j < config.burstSize; j++) {        // Loop on packets
            int charCount = 0;
            for (int i = 0; i < Packet_CRC.length(); i++) {  // Loop on packet size
                array[i] = Packet_CRC[i];
                outFile << array[i];
                charCount++;

                // Check if we need to start a new line every 8 bytes
                if (charCount % 8 == 0) {
                    outFile << endl;
                    charCount = 0;
                }
            }

            // If there are leftover bytes (less than 8), add a newline to maintain alignment
            if (charCount > 0) {
                outFile << endl;
            }

        }
        count++;

        // IFGs that are not burst IFGs
        int charCount1 = 0;
        for (int k = 0 ; k <= IFG_extra_bytes/ No_Bursts; k++) {  // Loop for IFGs
            outFile << IFG_str[0] << IFG_str[1];
            charCount1 += 2;

            // Check if we need to start a new line every 8 bytes
            if (charCount1 % 8 == 0) {
                outFile << endl;
                charCount1 = 0;
            }
        }

        // If there are leftover bytes (less than 8), add a newline to maintain alignment
        if (charCount1 > 0) {
            outFile << endl;
        }


        // IFGs
        int charCount = 0;
        for (int k = 0; k < IFG_Bytes_no; k++) {  // Loop for IFGs
            outFile << IFG_str[0] << IFG_str[1];
            charCount += 2;

            // Check if we need to start a new line every 8 bytes
            if (charCount % 8 == 0) {
                outFile << endl;
                charCount = 0;
            }
        }
        

        // If there are leftover bytes (less than 8), add a newline to maintain alignment
        if (charCount > 0) {
            outFile << endl;
        }
 
    }

    // Close the file
    outFile.close();

    // Free the dynamically allocated memory
    delete[] array;

    return 0;
}  
