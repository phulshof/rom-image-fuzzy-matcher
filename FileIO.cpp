#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include <dirent.h>
#include <assert.h>
#include <vector>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>


#include "Error.h"
#include "FileIO.h"


using namespace std;

extern void printUsageAndExit();

//*****************************************************************************
/**
 *   Get file name without rom end extension
 */

string getBaseFileName(string f)
{

  // Find last location of a period 
  //          (since period could be in other parts of file name)
  size_t pos = 0;
  size_t temp = pos;
  while(temp != string::npos && temp < f.length())
  {

    pos = temp;
    temp = f.find('.', pos+1);
  }

  return f.substr(0, pos);

}

//*****************************************************************************
/**
 *   Get file extension
 */

string getFileExtension(string f)
{

  // Find last location of a period 
  //          (since period could be in other parts of file name)
  size_t pos = 0;
  size_t temp = pos;
  while(temp != string::npos && temp < f.length())
  {

    pos = temp;
    temp = f.find('.', pos+1);
  }

  return f.substr(pos+1);

}

//*****************************************************************************
/**
 *   Copy file to another.
 *
 *   @param from_file copy from
 *   @param to_file copy to
 */

int copyFile(string from_file, string to_file)
{

  if(from_file.length() == 0)
  {

    ERROR("File name to copy from is blank...\n");
    return RETURN_ERROR;
  }

  if(to_file.length() == 0)
  {

    ERROR("File name to copy to is blank...\n");
    return RETURN_ERROR;
  }

  try
  {

    ifstream ifs(from_file.c_str(), ifstream::binary);
    ofstream ofs(to_file.c_str(), ofstream::binary);

    ofs << ifs.rdbuf();

  }
  catch(exception& e)
  {

    cout << "Exception occured " << e.what() << endl;
    return RETURN_ERROR;
  }

  return RETURN_SUCCESS;

}

//*****************************************************************************
/**
 *   Grab up all files in a directory and
 *     put them in a vector
 */

void getFiles(string dir_path, vector<string>& art_files)
{

  DIR* dp = NULL;
  dp = opendir(dir_path.c_str());
  if(dp == NULL)
  {
    ERROR("opening directory %s, %s\n", dir_path.c_str(),
        strerror(errno));
    printUsageAndExit();
  }

  string filepath = "";
  struct dirent* file;
  struct stat filestat;
  while((file = readdir(dp)))
  {

    filepath = dir_path + "/" + file->d_name;

    // Skip invalid files
    if(stat(filepath.c_str(), &filestat))
    {
      continue;
    }
    // Skip dirs
    if(S_ISDIR(filestat.st_mode))
    {

      continue;
    }

    //cout << "Adding: " << file->d_name << endl;
    art_files.push_back(file->d_name);

  }

  if(art_files.size() == 0)
  {

    ERROR("No files found in directory!\n");
    printUsageAndExit();

  }

  cout << "Packed up: " << art_files.size() << " files" << endl;

}

