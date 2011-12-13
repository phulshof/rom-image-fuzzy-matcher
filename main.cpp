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

/// Threshold of where to indicated no good match found
float match_threshold = .50;
const char MATCH_THRESHOLD_COMMAND  = 'm';

/// Perfect match threshhold max match distance to be considered perfect
///  match and stop searching
float perfect_match_threshold = 0.30;

struct FileMatch
{
  string filename;
  int match_distance;
  bool good_match;

};


string toLowerCase(string s1);
FileMatch findBestMatch(string rom, vector<string> art_files,
    bool use_substrings);

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

void getFiles(string dir_path, vector<string>& art_files)
{

  DIR* dp = NULL;
  dp = opendir(dir_path.c_str());
  if(dp == NULL)
  {
     ERROR("opening directory %s, %s\n", dir_path.c_str(),
         strerror(errno));
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

    //cout << "Adding: " << file->d_name << endl;
    art_files.push_back(file->d_name);

  }

  if(art_files.size() == 0)
  {

    ERROR("No files found in directory!\n");
    exit(1);

  }

  cout << "Packed up: " << art_files.size() << " files" << endl;

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
  COMMANDS += MATCH_THRESHOLD_COMMAND;
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
      case MATCH_THRESHOLD_COMMAND:
        if(optarg)
            match_threshold = atof(optarg);
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

  s1_ = toLowerCase(s1_);
  s2_ = toLowerCase(s2_);

  return levenshteinDistance((const char*)s1_.c_str(), 
      (const char*)s2_.c_str());

}

//*****************************************************************************
/**
 *   Elimate all occurances of a particular string
 */

void replaceAll(string& filename, const string toremove, 
    const string replacement)
{

  unsigned int pos = filename.find(toremove);
  while(pos != string::npos && pos < filename.length())
  {

    filename.replace(pos,toremove.length(), replacement);
    pos = filename.find(toremove);
  }

}

//*****************************************************************************
/**
 *   Substitute all text between two characters with @p replacement
 */

void replaceBetween(string& filename, const char first, const char second,
    const string replacement)
{

  unsigned int pos = filename.find(first);
  unsigned int pos2 = filename.length();
  if(pos < filename.length())
  {
    pos2 = filename.find(second, pos);
  }

  while(pos != string::npos && pos < filename.length() &&
      pos2 != string::npos && pos2 < filename.length() &&
      pos < pos2)
  {

    filename.replace(pos, pos2-pos+1, replacement);

    // Find the next occurance of the two characters
    pos = filename.find(first, pos2);
    pos2 = filename.length();
    if(pos < filename.length())
    {
      pos2 = filename.find(second, pos);
    }

  }

}

//*****************************************************************************
/**
 *   Delete all text between two characters with @p replacement
 */

void deleteBetween(string& filename, const char first, const char second)
{

  replaceBetween(filename, first, second, " ");

}


//*****************************************************************************
/**
 *   Elimate all occurances of a particular string
 */

void removeAll(string& filename, const string toremove)
{

  replaceAll(filename, toremove, "");

}


//*****************************************************************************
/**
 *  String to lower case
 */

string toLowerCase(string s1)
{

  string s1_ = s1;

  for(unsigned int i = 0; i < s1_.length(); i++)
  {

    s1_[i] = tolower(s1[i]);
  }

  return s1_;

}

//*****************************************************************************
/**
 *  Sanitize filename, get rid of _ spaces, (USA) (J), ! and other dumb 
 *  things in rom filenames (assume all converted to lower case)
 */

string sanitizeFileName(string filename)
{

  string s = filename;
  s = toLowerCase(s);

  // remove common adjectives etc
  removeAll(s, " the ");
  replaceAll(s, " & ", " and ");
  deleteBetween(s, '[', ']');
  deleteBetween(s, '(', ')');
  removeAll(s, " a ");
  //removeAll(s, " of ");
  removeAll(s, "-");
  removeAll(s, "'");
  removeAll(s, "\"");
  removeAll(s, ".png");
  removeAll(s, ".bmp");
  removeAll(s, ".rom");
  removeAll(s, ".zip");
  removeAll(s, ".jpg");
  removeAll(s, "!");
  removeAll(s, "(");
  removeAll(s, ")");
  removeAll(s, ".");
  removeAll(s, ",");
  replaceAll(s, "  ", " ");
  replaceAll(s, " ", "_");
  removeAll(s, "_");
  removeAll(s, "[");
  removeAll(s, "]");

  return s;

}

//*****************************************************************************
/**
 *  Is one a substring of the other?
 */

bool substringOfOther(string s1, string s2)
{

  unsigned int found = s1.find(s2);
  if(found != string::npos && found < s1.length())
  {

    return true;
  }


  found = s2.find(s1);
  if(found != string::npos && found < s2.length())
  {

    return true;
  }

  return false;

}

//*****************************************************************************
/**
 *  Compare all snapshots to a filename and return best snapshot
 */

FileMatch findBestMatch(string rom, vector<string> art_files)
{

  FileMatch m = findBestMatch(rom, art_files, false);
  if(m.good_match == false)
  {

    m = findBestMatch(rom, art_files, true);
  }
  
  return m;

}


//*****************************************************************************
/**
 *  Compare all snapshots to a filename and return best snapshot
 */

FileMatch findBestMatch(string rom, vector<string> art_files,
    bool use_substrings)
{

  if(art_files.size() == 0)
  {

    ERROR("No art files passed to function...\n");
    exit(1);
  }

  if(rom.size() == 0)
  {

    ERROR("No rom file passed to function...\n");
    exit(1);
  }


  // Sanitized rom name
  string rom_san = sanitizeFileName(rom);
  string art_san = sanitizeFileName(art_files[0]);

  float perfect_match_dist = perfect_match_threshold*rom_san.length();

  FileMatch best_match;
  FileMatch temp;

  best_match.filename = art_files[0];


  // Consider substrings a perfect match
  if(use_substrings && substringOfOther(art_san, rom_san))
  {

    best_match.match_distance = 0;

  }
  else
  {

    best_match.match_distance = 
      caseInsensitiveLevenshteinDistance(rom_san, art_files[0]);
  }

  for(unsigned int i = 1; i < art_files.size() &&
      best_match.match_distance > perfect_match_dist;
      i++)
  {

    temp.match_distance = 0;
    temp.filename = "";

    art_san = sanitizeFileName(art_files[i]);
    temp.filename = art_files[i];

    // Consider substrings a perfect match
    if(use_substrings && substringOfOther(art_san, rom_san))
    {

      temp.match_distance = 0;

    }
    else
    {

      temp.match_distance =
        caseInsensitiveLevenshteinDistance(rom_san, 
            art_san);

    }

    if(temp.match_distance < best_match.match_distance)
    {

      //      cout << "Last distance: " << best_match.match_distance 
      //        << " New Distance: " << temp.match_distance << endl;
      //
      //      cout << "Last file: " << best_match.filename 
      //        << " New file: " << temp.filename << endl;


      best_match = temp;
      best_match.match_distance = temp.match_distance;

      //      cout << rom_san << ": " << art_files[i] << 
      //        ": Match distance: " << best_match.match_distance << 
      //        endl;

    }

  }

  if(best_match.match_distance >= match_threshold*rom_san.length())
  {

    ERROR("Possibly No proper match found for rom: \n"
        "%s <=/>  %s match distance: %d\n",
        rom.c_str(),
        best_match.filename.c_str(),
        best_match.match_distance
        );
    best_match.good_match = false;

  }
  else
  {

    best_match.good_match = true;
    //cout << "Matched: " << rom << " <=> " << best_match.filename << endl;
  }

  return best_match;

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

  vector<string> art_files;
  if(box_art_dir.length())
  {

    getFiles(box_art_dir,art_files);
  }
  else if(snapshots_dir.length())
  {

    getFiles(snapshots_dir,art_files);

  }

  vector<string> rom_files;
  if(roms_dir.length())
  {

    getFiles(roms_dir, rom_files);
  }
  else
  {

    ERROR("No Rom files location :(\n");
    exit(1);
  }

  int good_matches = 0;
  int bad_matches = 0;
  for(unsigned int i = 0; i < rom_files.size(); i++)
  {


    FileMatch ma = findBestMatch(rom_files[i], art_files);
    if(ma.good_match)
    {
      good_matches++;
    }
    else
    {
      bad_matches++;
    }

  }
  cout << "Good matches: " << good_matches << endl;
  cout << "Bad matches: " << bad_matches << endl;

  string test = "Donkey Kong () Country 2 - Diddy's Kong [eat it booger] Quest (U) (V1.1) [!].zip";
  cout << "New file is: " << test << endl;
  replaceBetween(test, '(', ')', "fart");
  deleteBetween(test, '[', ']');
  cout << "New file is: " << test << endl;
         

  //  FileMatch m;
  //
  //  m = findBestMatch(
  //      "Super Mario World",
  //      art_files);
  //  printf("Matched: %s: Match distance %d\n", m.filename.c_str(), 
  //      m.match_distance);
  //
  //
  //  m = findBestMatch(
  //      "Donkey Kong Country 2 - Diddy's Kong Quest (U) (V1.1) [!].zip", 
  //      art_files);
  //  printf("Matched: %s: Match distance %d\n", m.filename.c_str(), 
  //      m.match_distance);
  //
  //  m = findBestMatch(
  //      "Joe and Mac - Caveman Ninja (U).zip",
  //      art_files);
  //  printf("Matched: %s: Match distance %d\n", m.filename.c_str(), 
  //      m.match_distance);
  //
  //  m = findBestMatch(
  //      "Mickey Mania (U).zip",
  //      art_files);
  //  printf("Matched: %s: Match distance %d\n", m.filename.c_str(), 
  //      m.match_distance);
  //
  //  vector<string> test_art;
  //  test_art.push_back("Mickey Mania - The Timeless Adventures of Mickey Mouse (USA).png");
  //  test_art.push_back("Mighty Max (USA).png");
  //
  //  m = findBestMatch(
  //      "Mickey Mania (U).zip",
  //      test_art);
  //  printf("Matched: %s: Match distance %d\n", m.filename.c_str(), 
  //      m.match_distance);
  //
  //
  //  printf("Case Ins Distance Joe mac %d\n",
  //      caseInsensitiveLevenshteinDistance(
  //        "Joe and Mac - Caveman Ninja (U).zip",
  //        "Joe & Mac (USA).png"));


  printf("Case Ins Distance %d\n",
      caseInsensitiveLevenshteinDistance(
        "Donkey Kong Country 2 - Diddy's Kong Quest (U) (V1.1) [!].zip", 
        "Donkey Kong Country 2 - Diddy's Kong Quest (USA) (En,Fr).png"));

  printf("Case Ins Distance %d\n",
      caseInsensitiveLevenshteinDistance(
        "Diddy's Kong Quest (V1.1) (U).zst",
        "Donkey Kong Country 2 - Diddy's Kong Quest (USA) (En,Fr).png"));


  return EXIT_SUCCESS;

}

