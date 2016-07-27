/*
 * database.h
 * Created by T.Gonzalo on 10/11/2015.
 * Last modified on 27/07/2016
 */

#ifndef __DATABASE_H
#define __DATABASE_H

#include <map>
#include "headers.h"
#include "files.h"
#include "libjson.h"


// Class DataBase declarations
namespace Tomb
{
  template <class TYPE> class DataBase
  {

    protected:
      std::stringstream  _outdir ;
      static std::map<std::string, bool> _flags;
      static std::map<std::string, std::string> _jsons;
      static std::map<std::string, TYPE> _contents;
      static std::vector<std::string> _files;

    public:
      DataBase();
      ~DataBase();

      bool check(std::string);
      JSONNode json(std::string);
      TYPE at(std::string);
      void set(std::string, TYPE);

      std::string import(key);
      void fill();
      void flush();
  
  };
}


/*************************************************************/
/***************Class DataBase methods ***********************/
/*************************************************************/

namespace Tomb
{

  /* Constructor */
  template <class TYPE> DataBase::DataBase(std::string name)
  {
    _outdir << "./out/" << name;
    fill();
  }

  /* Destructor */
  template <class TYPE> DataBase::~DataBase()
  {
    flush();
  }

  /* Check whether the key is in the database */
  template <class TYPE> DataBase::check(std::string key)
  {
     if(_flags.find(key) != _flags.end())
       return true;
     return false;
  }

  /* Gets the json node corresponding to the key */
  template <class TYPE> JSONNode DataBase::json(std::string key)
  {
    if(check(key))
    {
      if(_jsons.find(key) != _jsons.end())
        return JSONNode(_jsons.at(key));
      if(_contents.find(key) != _contents.end())
        return JSONNode(_contents.at(key));
      std::string imp = import(key);
      _jsons.emplace(key,imp)
      return JSONNode(imp);
    }
    else
      throw "DataBase::Could not find key on the database";
  }

  /* Gets the object corresponding to the key */
  template <class TYPE> TYPE &DataBase::at(std::string key)
  {
    if(check(key))
    {
      if(_contents.find(key) != _contents.end())
        return _contents.at(key);
      if(_json.find(key) != _json.end())
      {
        TYPE obj(_json.at(key));
        _contents.emplace(key,obj);
        return _contents.at(key);
      }
      std::string imp = import(key);
      _jsons.emplace(key,imp);
      _contents.emplace(key,TYPE(JSNONode(imp)));
      return _contents.at(key);
    }
    else
      throw "DataBase::Could not find key on the database";
  }

  /* Imports the file corresponding to the key */
  template <class TYPE> std::string DataBase::import(std::string key)
  {

    if(check(key) and _files.find(key) != _files.end())
      return Files::ReadFileString(_files.at(key));
    else
      throw "DataBase::Could not find key on the database";
  }

  /* Stores the object in the database */
  template <class TYPE> void DataBase::set(std::string key, TYPE Object)
  {
    if(!check(key))
      _contents.emplace(key, Object);
      _flags.emplace(key, true);
    else
      throw "DataBase::Key already in the database";
  }

  /* Fills the information about the databases */
  template <class TYPE> void fill()
  {
    std::vector<std::string> contents = Files::GetDirectoryContents(_outdir.str());

    for(auto it = contents.begin(); it != contents.end(); it++)
    {
      std::stringstream file;
      file << _outdir.str() << *it;
      _files.push_back(file.str());
      _flags.emplace(*it, true);
    }

   /* Flush the database to file */
   template <class TYPE> void flush()
   {
     if(!Files::DirectoryExists(_outdir.str())
       Files::CreateDirectory(_outdir.str());

     for(auto it = _contents.begin(); it != _contents.end(); it++)
     {
       std::stringstream file;
       file << _outdir.str() << "/" << it->first;
     } 
   }
}


// Database helper functions
namespace Tomb
{
	
  // Fills the group database with info from the files
  void group_database_fill();
	
  // Flush the group database into json files
  void group_database_flush();

  // Fills the model database with info from the files
  void model_database_fill();
	
  // Flush the model database into json files
  void model_database_flush();

  // Filter the database with the observables
  void model_database_filter(List<std::string> &);
}

#endif /* __DATABASE_H */
