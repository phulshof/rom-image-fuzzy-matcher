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
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/numeric/ublas/matrix.hpp>

using namespace boost::filesystem;
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
 *  Parse all command line arguments
 */

void parseArguments(int argc, char* argv[])
{

  int opt = 0;
  const string COMMANDS = "" + SNAPSHOT_DIR_COMMAND + BOX_ART_DIR_COMMAND + 
    ROMS_DIR_COMMAND;

  while((opt = getopt(argc, argv, COMMANDS.c_str())) >= 0)
  {

    switch(opt)
    {

      case SNAPSHOT_DIR_COMMAND:
        snapshots_dir = optarg;
        break;
      case BOX_ART_DIR_COMMAND:
        box_art_dir = optarg;
        break;
      case ROMS_DIR_COMMAND:
        roms_dir = optarg;
        break;

    }

  }

}

//*****************************************************************************
int main(int argc, char* argv[])
{


  parseArguments(argc, argv);

  return EXIT_SUCCESS;

}

