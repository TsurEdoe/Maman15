#include <iostream>
#include <boost/filesystem.hpp>
#include <fstream>
#include "FileUtils.h"

using namespace boost::filesystem;

/*
   Open a file for read/write. Create folders in filepath if do not exist. Returns true if opened successfully. false otherwise.
 */
bool FileUtils::fileRequestOpen(const string& filePathStr, fstream& fs, bool write)
{
	try
	{
		if (filePathStr.empty())
		{
			return false;
		}

		path filePath(filePathStr);
		path parentFolderPath = filePath.parent_path();

		if (!exists(parentFolderPath)) 
		{
			// create directories within the path if they are do not exist.
			(void)create_directories(parentFolderPath);
		}
		
		const auto flags = write ? (fstream::binary | fstream::out) : (fstream::binary | fstream::in);
		fs.open(filePathStr, flags);
		return fs.is_open();
	}
	catch (exception&)
	{
		return false;
	}
}

/*
   Closes file stream. Return true if closed successfully. false otherwise.
 */
bool FileUtils::closeFile(fstream& fs)
{
	try
	{
		fs.close();
		return true;
	}
	catch (exception&)
	{
		return false;
	}
}

/*
   Writes bytes from fs to file. Returns true upon successful write. false otherwise.
 */
bool FileUtils::writeToFile(fstream& fs, const uint8_t* const file, const uint32_t byteAmount)
{
	try
	{
		if (file == NULL || byteAmount == 0)
		{
			return false;
		}

		fs.write(reinterpret_cast<const char*>(file), byteAmount);
		return true;
	}
	catch (exception&)
	{
		return false;
	}
}

/*
   Reads bytes from fs to file. Returns true if read successfully. false, otherwise.
 */
bool FileUtils::readFromFile(fstream& fs, uint8_t* const file, uint32_t byteAmount)
{
	try
	{
		if (file == NULL || byteAmount == 0)
		{
			return false;
		}

		fs.read(reinterpret_cast<char*>(file), byteAmount);
		return true;
	}
	catch (exception&)
	{
		return false;
	}
}

/*
   Calculates file size which is opened by fs. Return file's size. 0 if failed.
 */
uint32_t FileUtils::calculateFileSize(fstream& fs)
{
	try
	{
		const auto cur = fs.tellg();
		fs.seekg(0, fstream::end);
		const auto size = fs.tellg();
		if ((size <= 0) || (size > UINT32_MAX))
		{
			return 0;
		}

		fs.seekg(cur);
		return static_cast<uint32_t>(size);
	}
	catch (exception&)
	{
		return 0;
	}
}

/*
   Checks if file exists given a file path. Return true if file exists.
 */
bool FileUtils::doesFileExist(const string& filePath)
{
	if (filePath.empty())
	{
		return false;
	}

	try
	{
		const ifstream fs(filePath);
		return (!fs.fail());
	}
	catch (exception&)
	{
		return false;
	}
}

/*
   Removes a file given a file path. Return true if successfully removed the file. False, otherwise. If file not exists, return false.
 */
bool FileUtils::deleteFile(const string& filePath)
{
	try
	{
		return (0 == remove(filePath.c_str()));
	}
	catch (exception&)
	{
		return false;
	}
}