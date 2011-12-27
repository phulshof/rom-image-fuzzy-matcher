
#ifndef FILEIO_H
#define FILEIO_H 

#include <string.h>
#include <string>
#include <vector>

std::string getBaseFileName(std::string f);
int copyFile(std::string from_file, std::string to_file);
std::string getFileExtension(std::string f);

void getFiles(std::string dir_path, std::vector<std::string>& art_files);

enum { RETURN_ERROR, RETURN_SUCCESS, };

#endif /* FILEIO_H */
