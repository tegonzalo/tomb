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
#include "files.h"
//#include "libjson.h"

#define DB_NOT_FOUND 0
#define DB_FOUND 1
#define DB_FOUND_JSON 2
#define DB_FOUND_CONTENT 3

/*******************************/
/* Class DataBase declarations */
/*******************************/

namespace Tomb
{
  template <class TYPE> class DataBase
  {

    protected:
      std::stringstream  _outdir ;
      std::map<std::string, int> _flags;
      std::map<std::string, std::string> _json;
      std::map<std::string, TYPE*> _content;
      std::map<std::string, std::string> _files;

    public:
      DataBase();
      ~DataBase();

      int check(std::string);
   //   JSONNode json(std::string);
      TYPE* at(std::string);
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
    _outdir << "./out/" << typeid(TYPE).name() << "/";
    //fill();
  }

  /* Destructor */
  template <class TYPE> DataBase<TYPE>::~DataBase()
  {
    flush();
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
  /* Gets the object corresponding to the key */
  template <class TYPE> TYPE *DataBase<TYPE>::at(std::string key)
  {
    if(int flag = check(key))
    {
      if(flag == DB_FOUND_CONTENT)
        return _content.at(key);
  /*    if(flag == DB_FOUND_JSON)
      {
        _content.emplace(key,TYPE(libjson::parse(_json.at(key))));
        _flags[key] = DB_FOUND_CONTENT;
        return _content.at(key);
      }
      std::string imp = import(key);
      _json.emplace(key,imp);
      _content.emplace(key,TYPE(libjson::parse(imp)));
      _flags[key] = DB_FOUND_CONTENT;
      return _content.at(key);
 */   }
    else
      throw "DataBase::Could not find key on the database";
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
        if(flag == DB_FOUND_CONTENT and replace)
          _content[key] = new TYPE(*Object);
/*        if(flag == DB_FOUND_JSON)
        {
          if(replace)
            _content.emplace(key, Object);
          else
            _content.emplace(key, TYPE(libjson::parse(_json.at(key))));
        }
        if(flag == DB_FOUND)
        {
          if(replace)
            _content.emplace(key, Object);
          else
            _content.emplace(key, TYPE(libjson::parse(import(key))));
        }
*/      }
      else
        _content.emplace(key, new TYPE(*Object));
      _flags[key] = DB_FOUND_CONTENT;
    }
    catch (...)
    {
      throw;
    }
  }

  /* Fills the information about the databases */
  template <class TYPE> void DataBase<TYPE>::fill()
  {
    std::vector<std::string> content = Files::GetDirectoryContents(_outdir.str());

    for(auto it = content.begin(); it != content.end(); it++)
    {
      std::stringstream file;
      file << _outdir.str() << *it;
      _files.emplace(*it,file.str());
      _flags.emplace(*it, DB_FOUND);
    }
  }

  /* Flush the database to file */
  template <class TYPE> void DataBase<TYPE>::flush()
  {
    if(!Files::IsDirectory(_outdir.str()))
      Files::CreateDirectory(_outdir.str());

    for(auto it = _content.begin(); it != _content.end(); it++)
    {
      std::stringstream file;
      file << _outdir.str() << "/" << it->first;
//      Files::WriteFileString(file.str(), it->second.json().write_formatted());
    }
  }
}


/******************************************/
/* Database helper functions declarations */
/******************************************/

namespace Tomb
{

  // Singleton Database
  template <typename TYPE> DataBase<TYPE>& DB_Singleton();

  // Fills the database of type TYPE with info from files
  template <typename TYPE> void database_fill();

  // Fills the databases of variadic types with info from the files
  template <typename TYPE, typename ...Args> void database_fill();
	
  void group_database_fill();

  void group_database_flush();

  // Fills the model database with info from the files
  void model_database_fill();
	
  // Flush the model database into json files
  void model_database_flush();

  // Filter the database with the observables
 // void model_database_filter(List<std::string> &);
}

/*****************************************/
/* Database helper functions definitions */
/*****************************************/

namespace Tomb
{

  // Singleton DataBase
  template <typename TYPE> DataBase<TYPE>& DB()
  {
    static DataBase<TYPE> database;
    return database;
  }

  // Fills the database of type TYPE with info from files
  template <typename TYPE> void database_fill()
  {
    try
    {
      DB<TYPE>().fill();
    }
    catch (...)
    {
      throw;
    }
  }

  // Fills the databases of variadic types with info from the files
  template <typename TYPE, typename ...Args> void database_fill()
  {
    try
    {
      database_fill<TYPE>();
      database_fill<Args...>();
    }
    catch (...)
    {
      throw;
    }
  }
}

#endif /* __DATABASE_H */
