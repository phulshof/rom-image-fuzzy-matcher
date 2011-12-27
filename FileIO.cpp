#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include "Error.h"
#include "FileIO.h"


using namespace std;

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
 *   Copy file to another
 */

void copyFile(string f1, string f2)
{

  if(f1.length() == 0)
  {

    ERROR("File name to copy from is blank...\n");
    exit(1);
  }

  if(f2.length() == 0)
  {

    ERROR("File name to copy to is blank...\n");
    exit(1);
  }

  try
  {

    ifstream ifs(f1.c_str(), ifstream::binary);
    ofstream ofs(f2.c_str(), ofstream::binary);

    ofs << ifs.rdbuf();

  }
  catch(exception& e)
  {

    cout << "Exception occured " << e.what() << endl;
    exit(1);
  }

}

