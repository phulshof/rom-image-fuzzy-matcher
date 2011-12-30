/**
 *   Fuzzy screenshot and snapshot renamer.
 *   Given a rom directory and at least one picture dir (snapshot and/ or box art)
 *   Use Levenshtein distance to determine the best match and rename the picture
 *   to match (and create new dir(s) with renamed pictures)
 */
#define BOOST_FILESYSTEM_NO_DEPRECATED

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

#include <StringManipulation.h>
#include "Error.h"
#include "LevenshteinDistance.h"
#include "FileIO.h"

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
const char RENAMED_ART_DIR_COMMAND = 'd';

/// Where to place the renamed box art
string renamed_box_art_dir;

/// Threshold of where to indicated no good match found
float match_threshold = .50;
const char MATCH_THRESHOLD_COMMAND  = 'm';

/// Perfect match threshhold max match distance to be considered perfect
///  match and stop searching
float perfect_match_threshold = 0;
const char PERFECT_MATCH_THRESHOLD_COMMAND  = 'p';

/// If we should ignore bad matches and FORCE make images anyhow
bool force_bad_matches = false;
const char FORCE_BAD_MATCHES_COMMAND  = 'f';


string toLowerCase(string s1);
FileMatch findBestMatch(string rom, vector<string>& art_files,
    bool use_substrings);
void deleteBetween(string& filename, const char first, const char second);

//*****************************************************************************
/**
 *  Print usage and exit
 */

void printUsageAndExit()
{

  printf("Usage: " TARGET_STRING 
      " [-m match_threshold 0.0 to 1.0] "
      " [-p perfect_match_threshold 0.0 to 1.0] "
      " [-f force bad matches to be made anyhow]"
      "-s art_dir_path "
      "-r rom_dir_path -d dest_renamed_path\n\n");
  printf("perfect_match_threshold is used to cull out all other "
      "art files in the search, within a certain match threshold. "
      "Default is 0 = \"real\" perfect match distance needed to cull the rest of "
      "a search.\n\n"
      "If you don't care about accurate matches, since matching an entire dir of "
      "art is taking too long, you can increase the perfect match threshold to "
      "make matching happen faster, at the expense of "
      "getting less accurate matches.\n\n");
  printf("-f force_bad_matches: will force matches to be created that are below "
      "the threshold.  Use this if you don't care about a few bad matches, but "
      "want an art image for all your ROMS anyhow.\n\n");

  printf("Example: " TARGET_STRING 
      " -m 0.50 -s /home/kyle2/Games/Roms/SNES/SNES_Box_Scans/ "
      "-r /home/kyle2/Games/Roms/SNES/Roms/ "
      "-d /home/kyle2/Games/Roms/SNES/SNES_Corrected_box_scans/\n\n");

  exit(1);
}

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
 *  Create New renamed Art File (from rom name).
 *
 *  @param rom_name the rom name to create matching art file name from
 *  @param old_art_name use the extension from old art file to create new
 *  matching file
 *
 *   @return new art file name to use
 *
 */

string getNewArtFileName(string rom_name, string old_art_name)
{

  if(renamed_snapshots_dir.length() == 0)
  {

    ERROR("No renamed snapshots dir specified...\n");
    printUsageAndExit();
  }

  string art = getBaseFileName(rom_name);
  string art_extension = getFileExtension(old_art_name);

  return art + "." + art_extension;

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
  COMMANDS += RENAMED_ART_DIR_COMMAND;
  COMMANDS += ":";
  COMMANDS += PERFECT_MATCH_THRESHOLD_COMMAND;
  COMMANDS += ":";
  COMMANDS += FORCE_BAD_MATCHES_COMMAND;

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
      case PERFECT_MATCH_THRESHOLD_COMMAND:
        if(optarg)
            perfect_match_threshold = atof(optarg);
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
      case FORCE_BAD_MATCHES_COMMAND:
        force_bad_matches  = true;
        break;
      case RENAMED_ART_DIR_COMMAND:
        renamed_snapshots_dir = optarg;
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

FileMatch findBestMatch(string rom, vector<string>& art_files)
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

FileMatch findBestMatch(string rom, vector<string>& art_files,
    bool use_substrings)
{

  if(art_files.size() == 0)
  {

    ERROR("No art files passed to function...\n");
    printUsageAndExit();
  }

  if(rom.size() == 0)
  {

    ERROR("No rom file passed to function...\n");
    printUsageAndExit();
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
    //    cout << "Matched: " << rom << " <=> " << best_match.filename << endl;
    //    cout << "Distance: " << best_match.match_distance << endl;
  }

  best_match.to_match = rom;
  return best_match;

}

//*****************************************************************************
/**
 *   Perform rough matching
 */

void performMatching()
{

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
    printUsageAndExit();
  }

  int good_matches = 0;
  int bad_matches = 0;
  int art_created = 0;
  for(unsigned int i = 0; i < rom_files.size(); i++)
  {

    FileMatch ma = findBestMatch(rom_files[i], art_files);

    string new_art_file = getNewArtFileName(rom_files[i], ma.filename);

    // Only copy good matches (or if forced to)
    if(force_bad_matches || ma.good_match)
    {

      // Copy the file and check for errors
      if(copyFile(snapshots_dir +"/"+ ma.filename, 
            renamed_snapshots_dir + "/" + new_art_file) == RETURN_ERROR)
      {

        printUsageAndExit();
      }
      else
      {

        art_created++;
      }

    }

    // Keep a tally of good and bad matches (using Levenshtein threshold
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
  cout << "Art created: " << art_created << endl;

}

//*****************************************************************************
int main(int argc, char* argv[])
{

  assert(0 == minimum(1, 0, 3));
  testReplaceBetween();
  testLevenshtein();

  parseArguments(argc, argv);

  performMatching();

  return EXIT_SUCCESS;

}

