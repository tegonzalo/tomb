/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file 
 *  files.h 
 *
 * \author 
 * T. Gonzalo (t.e.gonzalo@fys.uio.no) 
 *
 * \date 
 * 15/02/2016 
 */

#ifndef __FILES_H__
#define __FILES_H__

#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

/****************************/
/* File methods declarations */
/****************************/

namespace Tomb
{
  namespace Files
  {

    inline int Error(std::string);
    int CreateDirectory(std::string);
    int EmptyDirectory(std::string);
    int DeleteDirectory(std::string);
    bool IsDirectory(std::string);
    bool FileExists(std::string);
    int DeleteFile(std::string);
    bool FileHasEnd(std::string);
    std::string ReadFileString(std::string);
    int WriteFileString(std::string, std::string);
    int ReplaceFileString(std::string, std::string);
    std::vector<std::string> GetDirectoryContents(std::string);

  }
}

#endif /* __FILES_H__ */
