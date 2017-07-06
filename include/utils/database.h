/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * database.h
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 10/11/2015
 */

#ifndef __DATABASE_H
#define __DATABASE_H

#include <map>
#include <typeinfo>
#include "cmake_variables.h"
#include "files.h"
#include "libjson.h"

#define DB_NOT_FOUND 0
#define DB_FOUND 1
#define DB_FOUND_JSON 2
#define DB_FOUND_FILE 3

/*******************************/
/* Class DataBase declarations */
/*******************************/

namespace Tomb
{
  template <class TYPE> class DataBase
  {

    protected:
      std::string _basedir;
      std::string _outdir;
      std::map<std::string, int> _flags;
      std::map<std::string, TYPE*> _content;
      std::map<std::string, std::string> _files;

    public:
      DataBase();
      ~DataBase();

      int check(std::string);
      typename std::map<std::string,TYPE*>::iterator begin();
      typename std::map<std::string,TYPE*>::iterator end();
      TYPE* at(std::string);
      TYPE* find(std::string);
      TYPE* get(std::string);
      void set(std::string, TYPE*, bool = false);
      std::string import(std::string);
      void fill();
      void flush();
  
  };
}

/******************************/
/* Class DataBase definitions */
/******************************/

namespace Tomb
{

  /* Constructor */
  template <class TYPE> DataBase<TYPE>::DataBase()
  {
    std::string name = typeid(TYPE).name();
    name.pop_back();
    name.erase(0,6);
    std::size_t found = name.find_first_not_of("12345678");
    if(found != std::string::npos) name.erase(0,found);

    std::stringstream dir;
    dir  << TOMB_DIR << "/database/";
    _basedir = dir.str();
    dir << name << "/";
    _outdir = dir.str();
  }

  /* Destructor */
  template <class TYPE> DataBase<TYPE>::~DataBase()
  {
  }

  /* Check whether the key is in the database */
  template <class TYPE> int DataBase<TYPE>::check(std::string key)
  {
     if(_flags.find(key) != _flags.end())
       return _flags.at(key);
     return DB_NOT_FOUND;
  }

  /* Gets the json node corresponding to the key */
/*  template <class TYPE> JSONNode DataBase<TYPE>::json(std::string key)
  {
    if(int flag = check(key))
    {
      if(flag == DB_FOUND_JSON)
        return libjson::parse(_json.at(key));
      if(flag == DB_FOUND_CONTENT)
      {
        JSONNode json(_content.at(key).json());
        _json.emplace(key, json.write_formatted());
        return json;
      }
      std::string imp = import(key);
      _json.emplace(key,imp);
      _flags[key] = DB_FOUND_JSON;
      return libjson::parse(imp);
    }
    else
      throw "DataBase::Could not find key on the database";
  }
*/

  /* Returns an iterator to the map */
   template <class TYPE> typename std::map<std::string, TYPE*>::iterator DataBase<TYPE>::begin()
  {
    return _content.begin();
  }

  /* Returns the end of the map */
  template <class TYPE> typename std::map<std::string, TYPE*>::iterator DataBase<TYPE>::end()
  {
    return _content.end();
  }

  /* Gets the object corresponding to the key */
  template <class TYPE> TYPE *DataBase<TYPE>::at(std::string key)
  {
    if(int flag = check(key))
    {
      // If it is in the database as an object return it
      if(flag == DB_FOUND)
        return _content.at(key);
      // If it is only in the file database, import it
      // TODO: Uncomment this and fix every class' ParseJSON
/*      std::string imp = import(key);
      _content.emplace(key,TYPE(libjson::parse(imp)));
      _flags[key] = DB_FOUND;
      return _content.at(key);*/
    }
    else
      throw "DataBase::Could not find key on the database";
  }

  /* Finds the object corresponding to the key or return null */
  template <class TYPE> TYPE *DataBase<TYPE>::find(std::string key)
  {
    try
    {
      if(check(key) == DB_FOUND)
        return at(key);
      else
        return NULL;
    }
    catch (...) { throw; }
  }

  /* Gets the object corresponding to the key or creates it otherwise */
  template <class TYPE> TYPE *DataBase<TYPE>::get(std::string key)
  {
    try
    {
      if(!check(key))
        set(key, new TYPE(key));
      return at(key);
    }
    catch (...) { throw; }
  }

  /* Imports the file corresponding to the key */
  template <class TYPE> std::string DataBase<TYPE>::import(std::string key)
  {

    if(check(key) and _files.find(key) != _files.end())
      return Files::ReadFileString(_files.at(key));
    else
      throw "DataBase::Could not find key on the database";
  }

  /* Stores the object in the database */
  template <class TYPE> void DataBase<TYPE>::set(std::string key, TYPE *Object, bool replace)
  {
    try
    {
      if(int flag = check(key))
      {
        if(flag == DB_FOUND and replace)
          _content[key] = new TYPE(*Object);
      }
      else
        _content.emplace(key, new TYPE(*Object));
      _flags[key] = DB_FOUND;
    }
    catch (...)
    {
      throw;
    }
  }

  /* Fills the information about the databases */
  template <class TYPE> void DataBase<TYPE>::fill()
  {
    std::cout << "Filling database from " << _outdir << std::endl;

   std::vector<std::string> content = Files::GetDirectoryContents(_outdir);

    for(auto it = content.begin(); it != content.end(); it++)
    {
      std::stringstream file;
      file << _outdir << *it;
      _files.emplace(*it,file.str());
      _flags.emplace(*it, DB_FOUND_FILE);
    }
  }

  /* Flush the database to file */
  template <class TYPE> void DataBase<TYPE>::flush()
  {
    std::cout << "Flushing database to " << _outdir << std::endl;

    if(!Files::IsDirectory(_basedir))
      Files::CreateDirectory(_basedir);
    if(!Files::IsDirectory(_outdir))
      Files::CreateDirectory(_outdir);

    for(auto it = _content.begin(); it != _content.end(); it++)
    {
      std::stringstream file;
      file << _outdir << "/" << it->first;
      if(Files::FileExists(file.str()))
        Files::ReplaceFileString(file.str(), it->second->json().write_formatted());
      else  
        Files::WriteFileString(file.str(), it->second->json().write_formatted());
    
   }
  }
}


/******************************/
/* Database helper functions  */
/******************************/

namespace Tomb
{

  // Singleton Database
  template <typename TYPE> DataBase<TYPE>& DB()
  {
    static DataBase<TYPE> database;
    return database;
  }

  // Dummy type list to make void function variadic	
  template<typename...> struct dummy_type_list{};

  // Fills the database from files, helper empty function
  template <typename...> void database_fill(dummy_type_list<>) {};

  // Fills the database from files, helper variadic function
  template <typename TYPE, typename ...Args> void database_fill(dummy_type_list<TYPE, Args...> var)
  {
    DB<TYPE>().fill();
    database_fill(dummy_type_list<Args...>());
  }

  // Fills the database from files
  template <typename ...Args> void database_fill()
  {
    database_fill(dummy_type_list<Args...>());
  }

  // Flushes the database to files, helper empty function
  template<typename...> void database_flush(dummy_type_list<>) {};

  // Flushes the database to files, helper variadic function
  template <typename TYPE, typename ...Args> void database_flush(dummy_type_list<TYPE, Args...> var)
  {
    DB<TYPE>().flush();
    database_flush(dummy_type_list<Args...>());
  }

  // Flushes the database to files
  template <typename ...Args> void database_flush()
  {
    database_flush(dummy_type_list<Args...>());
  }

}

/*****************************************/
/* Database helper functions definitions */
/*****************************************/
/*
namespace Tomb
{

  // Singleton DataBase
  template <typename TYPE> DataBase<TYPE>& DB()
  {
    static DataBase<TYPE> database;
    static std::vector<void *> DataBases;
    DataBases.push_back(&database);
    return database;
  }

  // Fills the database of type TYPE with info from files
  template <typename TYPE> void database_fill()
  {
    try
    {
      DB<TYPE>().fill();
    }
    catch (...) { throw; }
  }

  // Fills the databases of variadic types with info from the files
  template <typename TYPE, typename ...Args> void database_fill()
  {
    try
    {
      database_fill<TYPE>();
      database_fill<Args...>();
    }
    catch (...) { throw; }
  }

  // Flushes the database of type TYPE with info to files
  void database_flush(dummy_type_list<>)
  {

  }

  // Flushes the databases of variadic types with info to files
  template <typename TYPE, typename ...Args> void database_flush(dummy_type_list<TYPE, Args..>)
  {
    DB<TYPE>().flush();
    database_flush(dummy_type_list<Args...>());
  }

  // Flushes the databases of variadic types with info to files
  template <typename ...Args> void database_flush()
  {
    database_flush(dummy_type_list<Args...>());
  }

}
*/
#endif /* __DATABASE_H */
