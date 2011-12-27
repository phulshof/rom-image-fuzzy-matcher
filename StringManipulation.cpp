#include <stdio.h>
#include <assert.h>
#include <iostream>

#include "FileIO.h"
#include "LevenshteinDistance.h"
#include "StringManipulation.h"

using namespace std;

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

bool replaceBetween(string& filename, const char first, const char second,
    const string replacement)
{

  bool modified = false;

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

    modified = true;
    filename.replace(pos, pos2-pos+1, replacement);

    // Find the next occurance of the two characters
    pos = filename.find(first);
    pos2 = filename.length();
    if(pos < filename.length())
    {
      pos2 = filename.find(second, pos);
    }

  }

  return modified;

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
 *   Delete all text between two characters with @p replacement
 */

void deleteBetween(string& filename, const char first, const char second)
{

  replaceBetween(filename, first, second, "");

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
 *   Test replaceBetween  function
 */

void testReplaceBetween()
{

  string s = "Some crazy[stuff between the brackets] junk[this should be deleted].";
  string s2 = "Some crazy junk.";

  replaceBetween(s, '[', ']', "");
  cout << "s is now: " << s << endl;
  cout << "s2 is now: " << s2 << endl;
  cout << "Distance: " << caseInsensitiveLevenshteinDistance(s, s2) << endl;
  cout << "Compare: " << s.compare( s2) << endl;
  if(s == s2) cout << "The same! " << endl;
       
  assert(s == s2);

  s = "Some crazy[stuff between the brackets] junk[this should be deleted].";
  s2 = "Some crazyiest junkiest.";

  replaceBetween(s, '[', ']', "iest");
  cout << "s is now: " << s << endl;
  cout << "s2 is now: " << s2 << endl;
  cout << "Distance: " << caseInsensitiveLevenshteinDistance(s, s2) << endl;
  cout << "Compare: " << s.compare( s2) << endl;
  if(s == s2) cout << "The same! " << endl;

  assert(s == s2);


  // Test file name

  string f = "come.crazy.file.and.such.jpg";
  string f2 = "come.crazy.file.and.such";
  assert(f2 == getBaseFileName(f));

  string test = "Donkey Kong () Country 2 - Diddy's Kong [dumb crap] Quest (U) (V1.1) [!].zip";
  cout << "New file is: " << test << endl;
  replaceBetween(test, '(', ')', "stuff...blah");
  deleteBetween(test, '[', ']');
  cout << "New file is: " << test << endl;


  printf("Case Ins Distance %d\n",
      caseInsensitiveLevenshteinDistance(
        "Donkey Kong Country 2 - Diddy's Kong Quest (U) (V1.1) [!].zip", 
        "Donkey Kong Country 2 - Diddy's Kong Quest (USA) (En,Fr).png"));

  printf("Case Ins Distance %d\n",
      caseInsensitiveLevenshteinDistance(
        "Diddy's Kong Quest (V1.1) (U).zst",
        "Donkey Kong Country 2 - Diddy's Kong Quest (USA) (En,Fr).png"));


}

