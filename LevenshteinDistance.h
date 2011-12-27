
#ifndef LEVENSHTEINDISTANCE_H
#define LEVENSHTEINDISTANCE_H 

#include <string>

enum
{ 
  LEVENSTEIN_ERROR = -1,
  LEVENSTEIN_SUCCESS = 0,
};


int levenshteinDistance(const char* s1, const char* s2);
int minimum(int a, int b, int c);

void testLevenshtein();
int caseInsensitiveLevenshteinDistance(std::string s1, std::string s2);

#endif /* LEVENSHTEINDISTANCE_H */
