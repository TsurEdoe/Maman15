#pragma once
#include <set>
#include <string>
#include <fstream>

using namespace std;

/*
    Utils class to handle client file IO operations
*/
class FileUtils
{
public:
    static bool fileRequestOpen(const string& filepath, fstream& fs, bool write = false);
    static bool closeFile(fstream& fs);
    static bool writeToFile(fstream& fs, const uint8_t* const file, const uint32_t bytes);
    static bool readFromFile(fstream& fs, uint8_t* const file, uint32_t bytes);
    static uint32_t calculateFileSize(fstream& fs);
    static bool doesFileExist(const string& filepath);
};

