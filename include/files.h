/*
 * files.h
 * Created by T.Gonzalo on 13/09/2013.
 * Last modified on 03/12/2015.
 */

#ifndef __FILES_H
#define __FILES_H

#include "headers.h"

#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

namespace Tomb
{
	namespace Files
	{

		/*************************************************************************************/
		/* File methods headers*/
		/*************************************************************************************/

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

#endif /* __FILES_H */