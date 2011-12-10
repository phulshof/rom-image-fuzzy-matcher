/**
 *   Fuzzy screenshot and snapshot renamer.
 *   Given a rom directory and at least one picture dir (snapshot and/ or box art)
 *   Use Levenshtein distance to determine the best match and rename the picture
 *   to match (and create new dir(s) with renamed pictures)
 */
#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <dirent.h>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <assert.h>
#include <vector>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <getopt.h>

#include <string.h>
#include <string>
//#include <boost/filesystem.hpp>
//#include <boost/regex.hpp>
//#include <boost/numeric/ublas/matrix.hpp>

#include "Error.h"
#include "LevenshteinDistance.h"

//using namespace boost::filesystem;
using namespace std;

/// Full path Where we will have our snapshots art
string snapshots_dir;
const char SNAPSHOT_DIR_COMMAND = 's';

/// Path Where we will have our box art
string box_art_dir;
const char BOX_ART_DIR_COMMAND = 'b';

/// Path to where the roms to match against are
string roms_dir;
const char ROMS_DIR_COMMAND = 'r';

/// Where to place the renamed snapshots
string renamed_snapshots_dir;

/// Where to place the renamed box art
string renamed_box_art_dir;

//*****************************************************************************
/**
 *  Check for command usage errors
 */

void checkErrors()
{

  if(roms_dir == "")
  {
     ERROR("No roms dir specified!\n");
  }

  if(renamed_snapshots_dir == "" && renamed_box_art_dir == "")
  {

     ERROR("No renamed snapshots or renamed box art dir specified!\n");
  }

}

//*****************************************************************************
/**
 *   Grab up all files in a snapshot files in a directory and
 *     put them in a vector
 */

void getArtFiles(string dir_path, vector<string>& art_files)
{

  DIR* dp = NULL;
  dp = opendir(dir_path.c_str());
  if(dp == NULL)
  {
     ERROR("opening directory %s\n", dir_path.c_str());
     exit(1);
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

    cout << "Adding: " << file->d_name << endl;
    art_files.push_back(file->d_name);

  }

}

//*****************************************************************************
/**
 *  Parse all command line arguments
 */

void parseArguments(int argc, char* argv[])
{

  int opt = 0;
  string COMMANDS = "" ;
  COMMANDS += SNAPSHOT_DIR_COMMAND;
  COMMANDS += ":";
  COMMANDS += BOX_ART_DIR_COMMAND;
  COMMANDS += ":";
  COMMANDS +=  ROMS_DIR_COMMAND;
  COMMANDS += ":";

  cout << "Parts: " << SNAPSHOT_DIR_COMMAND << 
    BOX_ART_DIR_COMMAND << ROMS_DIR_COMMAND << endl;

  cout << "COMMANDS: " << COMMANDS << endl;

  while((opt = getopt(argc, argv, COMMANDS.c_str())) >= 0)
  {

    switch(opt)
    {

      case SNAPSHOT_DIR_COMMAND:
        if(optarg)
            snapshots_dir = optarg;
        break;
      case BOX_ART_DIR_COMMAND:
        if(optarg)
        {

          box_art_dir = optarg;
          cout << "Box art dir used: " << box_art_dir << endl;
        }
        break;
      case ROMS_DIR_COMMAND:
        if(optarg)
            roms_dir = optarg;
        break;

    }

  }
  checkErrors();

}

//*****************************************************************************
/** 
 *   Case in sensitive compare
 */

int caseInsensitiveLevenshteinDistance(string s1, string s2)
{

  string s1_ = s1;
  string s2_ = s2;

  for(unsigned int i = 0; i < s1_.length(); i++)
  {

    s1_[i] = tolower(s1[i]);
  }

  for(unsigned int i = 0; i < s2_.length(); i++)
  {

    s2_[i] = tolower(s2[i]);
  }

  return levenshteinDistance((const char*)s1_.c_str(), 
      (const char*)s2_.c_str());

}


//*****************************************************************************
int main(int argc, char* argv[])
{

  parseArguments(argc, argv);

  assert(0 == minimum(1, 0, 3));

  printf("Distance %d\n",
      levenshteinDistance("sitting", "kitten"));
  //levenshteinDistance("kitten", "sitting"));
  printf("Distance %d\n",
      levenshteinDistance("KYLE", "kyle"));
  printf("Distance %d\n",
      levenshteinDistance("mario Kart", "Mario_Kart"));
  printf("Case Ins Distance %d\n",
      caseInsensitiveLevenshteinDistance("mario Kart", "Mario_Kart"));

  vector<string> box_art_files;
  getArtFiles(box_art_dir,box_art_files);


  return EXIT_SUCCESS;

}

