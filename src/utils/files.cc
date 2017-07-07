/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file 
 *  files.cc 
 *
 * \author 
 * T.Gonzalo (t.e.gonzalo@fys.uio.no) 
 *
 * \date 
 * 15/02/2016 
 */ 

#include "files.h"

/***************************/
/* File method definitions */
/***************************/


namespace Tomb
{
  namespace Files
  {

    /*************************************************************************************/
    /* File methods */
    /*************************************************************************************/

    /* Print a message into the seleted error output */
    int Error(std::string message)
    { 
      /*std::ofstream ErrorFile;
      char ErrorFileName[] = "Error.out";
      ErrorFile.open(ErrorFileName);
      ErrorFile << message << std::endl;
      ErrorFile.close();
      */
      std::cerr << "Error::" << message << std::endl;
      return 0;
    }

    /* Creates a directory */
    int CreateDirectory(std::string path)
    {
      try
      {   
        if(!IsDirectory(path))
          mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

        return 0;
        
      } 
      catch (...) { throw; }
    }

    /* Empty the contents of a directory */
    int EmptyDirectory(std::string path)
    {   
      char currentdir[100];	
      std::string fullpath = getcwd(currentdir,100);
      
      if(path[0] != '.' && path[0] != '/')
      {
        Error("Tomb::Files::Path not found");
        return 1;
      }
      else if(path[0] == '.')
      {
        while(path[0] == '.')
          path.erase(0,1);
        if(path[0] != '/')
          path.insert(0,"/");
        path.append("/");
        fullpath.append(path);
      }
      else
      {
        path.append("/");
        fullpath = path;
      }
      
      DIR *dir = opendir(fullpath.c_str());
      
      if(dir == NULL)
      {
        Error("Tomb::Files::Directory could not be found");
        return 1;
      }

      struct dirent *object = NULL;
      
      while((object = readdir(dir)) != NULL)
      {
        if(strcmp(object->d_name, ".") && strcmp(object->d_name,".."))
        {
          std::string thispath = fullpath;
          thispath.append(object->d_name);
          
          if(IsDirectory(thispath))
          {
            if(DeleteDirectory(thispath))
              return 1;
          }
          else
          {
            if(remove(thispath.c_str()))
            {
              Error("Tomb::Files::File could not be removed");
              return 1;
            }
            //std::cout << "Deleting " << thispath << std::endl;
          }
        }
      }
      
      closedir(dir);
      
      return 0;
    }

    /* Delete a directory and all its content */
    int DeleteDirectory(std::string path)
    {
      if(EmptyDirectory(path)) 
        return 1;
      if(rmdir(path.c_str()))
      {
        Error("Tomb::Files::Directory could not be removed");
        return 1;
      }	
      std::cout << "Deleting " << path << std::endl;
      return 0;
    }

    /* Check whether the object is a directory */
    bool IsDirectory(std::string path)
    {
      try
      {
        struct stat statbuf;

        if(stat(path.c_str(), &statbuf) != 0 )
          return false;
        else if(statbuf.st_mode and S_ISDIR(statbuf.st_mode))
          return true;
        else
          return false;
        
      }
      catch (...) { throw; }
      
    }

    /* Check if a file exists */
    bool FileExists(std::string path)
    {   
      std::fstream file;
      
      file.open(path.c_str());
      
      if(file.is_open())
      {
        file.close();
        return true;
      }
      else
      {
        file.close();
        return false;
      }
    }

    /* Deletes a file if exists */
    int DeleteFile(std::string path) 
    {
      if(FileExists(path))
      {
        if(remove(path.c_str()))
        {
          Error("Tomb::Files::File could not be removed");
          return 1;
        }	
        //std::cout << "Deleting " << path << std::endl;
      }
      return 0;
    }

    /* Check if the file has end */
    bool FileHasEnd(std::string path)
    {
      if(FileExists(path))
      {
        std::fstream file;

        file.open(path.c_str());
        
        if(file.is_open()) 
        {
          file.seekg(-1,std::ios_base::end);
          std::string lastline;
          getline(file,lastline);
          file.close();
          if(!lastline.compare("END"))
            return true;
          else
            return false;
        }
      }
      
      return false;
    }

    /* Reads a file and returns a string */
    std::string ReadFileString(std::string path)
    {
      if(FileExists(path))
      {     
        std::ifstream file;
        std::stringstream filestring;
        
        file.open(path.c_str());
        
        if(file.is_open())
        {
          std::string line;
          while(getline(file,line))
            filestring << line << "\n";
          
          file.close();
          
          return filestring.str();
        }
        else
          throw "Tomb::Files::Could not open file";
        
      }
      else
        throw "Tomb::Files::File does not exist";
    }

    /* Writes a string to a file */
    int WriteFileString(std::string path, std::string content)
    {
      if(!FileExists(path))
      {
        std::ofstream file;
        file.open(path.c_str());
        file << content << std::endl;
        file.close();
      }
      else
        throw "Tomb::Files::File already exists";
      return 0;			
    }

    /* Replace a file with a another one writen as a string */
    int ReplaceFileString(std::string path, std::string content)
    {
      if(FileExists(path))
      {
        if(remove(path.c_str()))
        {
          Error("Tomb::Files::File could not be removed");
          return 1;
        }
        std::ofstream file;
        file.open(path.c_str());
        file << content << std::endl;
        file.close();
      }
      return 0;
    }
    
    /* Gets the contents of a directory */
    std::vector<std::string> GetDirectoryContents(std::string path) 
    {
      std::vector<std::string> contents;
      if(!IsDirectory(path)) throw "Tomb::Files::Path is not a directory";
      DIR *dir;
      struct dirent *ent;
      if ((dir = opendir (path.c_str())) != NULL)
      {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL)
          if(strcmp(ent->d_name, ".") and strcmp(ent->d_name,".."))
            contents.push_back(ent->d_name);
        closedir (dir);
      }
      else
      {
        /* could not open directory */
        Error("Tomb::Files::Could not open directory");
      }
      return contents;
    }
  }
}



