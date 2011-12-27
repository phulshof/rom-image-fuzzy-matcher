
#ifndef STRINGMANIPULATION_H
#define STRINGMANIPULATION_H 

#include <string>

std::string toLowerCase(std::string s1);
void replaceAll(std::string& filename, const std::string toremove, 
    const std::string replacement);

bool replaceBetween(std::string& filename, const char first, const char second,
    const std::string replacement);

void deleteBetween(std::string& filename, const char first, const char second);
void removeAll(std::string& filename, const std::string toremove);

void testReplaceBetween();
#endif /* STRINGMANIPULATION_H */
