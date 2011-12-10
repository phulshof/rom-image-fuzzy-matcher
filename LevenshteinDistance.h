
#ifndef LEVENSHTEINDISTANCE_H
#define LEVENSHTEINDISTANCE_H 

enum
{ 
  LEVENSTEIN_ERROR = -1,
  LEVENSTEIN_SUCCESS = 0,
};


int levenshteinDistance(const char* s1, const char* s2);
int minimum(int a, int b, int c);

#endif /* LEVENSHTEINDISTANCE_H */
