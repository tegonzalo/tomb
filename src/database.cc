/*
* database.cc
* Created by T.Gonzalo on 31/03/2016.
* Last modified on 27/07/2016
*/

#include <map>
#include "database.h"


namespace Tomb
{
  
  // Fills the group database with info from the files
  void group_database_fill()
  {
    try
    {	std::stringstream filepath;
      filepath << "./out/";
      
      List<std::string> OutputContents = Files::GetDirectoryContents(filepath.str());
      
      //if(OutputContents.nterms()) std::cout << "Loading database...." << std::endl;

      for(auto it_Contents = OutputContents.begin(); it_Contents != OutputContents.end(); it_Contents++) {
        
        std::stringstream filename;
        filename << filepath.str() << *it_Contents << "/" << *it_Contents << ".out";
        if(Files::FileExists(filename.str())) {
          JSONNode json = libjson::parse(Files::ReadFileString(filename.str()));
          if(it_Contents->find("x") == std::string::npos) {
            if(SimpleGroup::JSONDataBase.find(*it_Contents) != SimpleGroup::JSONDataBase.end())
              SimpleGroup::JSONDataBase.erase(*it_Contents);
            SimpleGroup::JSONDataBase.emplace(*it_Contents,json);
          }
          if(LieGroup::JSONDataBase.find(*it_Contents) != LieGroup::JSONDataBase.end())
            LieGroup::JSONDataBase.erase(*it_Contents);
          LieGroup::JSONDataBase.emplace(*it_Contents,json);
          
          // Parse the reps
          if(LieGroup::JSONDataBase.at(*it_Contents).at("hasReps").as_bool()) {
            JSONNode Reps;
            if(it_Contents->find("x") == std::string::npos) {
              Reps = LieGroup::JSONDataBase.at(*it_Contents).at("Irreps");
            } else {
              Reps = LieGroup::JSONDataBase.at(*it_Contents).at("Reps");
            }
            for(JSONNode::const_iterator it_Reps = Reps.begin(); it_Reps != Reps.end(); it_Reps++) {
              std::stringstream repname;
              repname << filepath.str() << *it_Contents << "/reps/" << it_Reps->as_string() << ".out";
              if(Files::FileExists(repname.str())) {
                JSONNode json = libjson::parse(Files::ReadFileString(repname.str()));
                if(it_Contents->find("x") == std::string::npos) {
                  if(Irrep::JSONDataBase.find(it_Reps->as_string()) != Irrep::JSONDataBase.end())
                    Irrep::JSONDataBase.erase(it_Reps->as_string());
                  Irrep::JSONDataBase.emplace(it_Reps->as_string(), json);
                }
                if(Rrep::JSONDataBase.find(it_Reps->as_string()) != Rrep::JSONDataBase.end())
                  Rrep::JSONDataBase.erase(it_Reps->as_string());
                Rrep::JSONDataBase.emplace(it_Reps->as_string(), json);
              }							
              //std::cout << Rrep::DataBase.at(id).Weights() << std::endl;
            }
          }
          
          // Parse the subgroups
          if(LieGroup::JSONDataBase.at(*it_Contents).at("hasSubgroups").as_bool()) {
            JSONNode Subgroups = LieGroup::JSONDataBase.at(*it_Contents).at("Subgroups");
            for(auto it_Subgroups = Subgroups.begin(); it_Subgroups != Subgroups.end(); it_Subgroups ++) {
              std::stringstream subname;
              subname << filepath.str() << *it_Contents << "/subs/" << it_Subgroups->as_string() << ".out";
              if(Files::FileExists(subname.str())) {
                JSONNode json = libjson::parse(Files::ReadFileString(subname.str()));
                if(SubGroup::JSONDataBase.find(it_Subgroups->as_string()) != SubGroup::JSONDataBase.end())
                  SubGroup::JSONDataBase.erase(it_Subgroups->as_string());
                SubGroup::JSONDataBase.emplace(it_Subgroups->as_string(), json);
              }
            }
          }
        }
      }
      //for(auto it = SimpleGroup::JSONDataBase.begin(); it != SimpleGroup::JSONDataBase.end(); it++)
      //	std::cout << it->first << " -> " << it->second.write_formatted() << std::endl;
      //for(auto it = LieGroup::JSONDataBase.begin(); it != LieGroup::JSONDataBase.end(); it++)
      //	std::cout << it->first << " --> " << it->second.write_formatted() << std::endl;
      //for(auto it = Irrep::JSONDataBase.begin(); it != Irrep::JSONDataBase.end(); it++)
      //	std::cout << it->first << " -> " << it->second.write_formatted() << std::endl;
      //for(auto it = Rrep::JSONDataBase.begin(); it != Rrep::JSONDataBase.end(); it++)
      //	std::cout << it->first << " --> " << it->second.write_formatted() << std::endl;
      //for(auto it = SubGroup::JSONDataBase.begin(); it != SubGroup::JSONDataBase.end(); it++)
      //	std::cout << it->first << " --> " << it->second.write_formatted() << std::endl;
    }
    catch (...)
    {
      throw;
    }
  }
  
  // Flush the group database into json files
  void group_database_flush()
  {
    try
    {
      std::stringstream filepath;
      filepath << "./out/";
      
      // Flush the SimpleGroups to file
      for(auto it_SimpleGroup = SimpleGroup::DataBase.begin(); it_SimpleGroup != SimpleGroup::DataBase.end(); it_SimpleGroup ++)
      {
        
        // If the group directory is not there yet, create it
        std::stringstream dir;
        dir << filepath.str() << it_SimpleGroup->first;
        if(!Files::IsDirectory(dir.str())) Files::CreateDirectory(dir.str());
        
        
        // If the file is not there create it
        std::stringstream filename;
        filename << dir.str() << "/" << it_SimpleGroup->first << ".out";
        if(!Files::FileExists(filename.str())) 
          Files::WriteFileString(filename.str(), it_SimpleGroup->second.json().write_formatted());
        // Otherwise, if it has reps or subgroups compare it with the written file
        else if (it_SimpleGroup->second.hasReps() or it_SimpleGroup->second.hasSubgroups()) {
          JSONNode json = libjson::parse(Files::ReadFileString(filename.str()));
          bool changed = false;
          if(it_SimpleGroup->second.hasReps()) {
            JSONNode::const_iterator it = json.begin(); 
            while(it != json.end() and it->name() != "hasReps") it++;
            if(!it->as_bool()) {
              json.erase(json.find("hasReps"));
              json.erase(json.find("Irreps"));
              json.push_back(JSONNode("hasReps", true));
              json.push_back(it_SimpleGroup->second.Irreps().json("Irreps"));
              changed = true;
            }
          }
          if(it_SimpleGroup->second.hasSubgroups()) {
            JSONNode::const_iterator it = json.begin(); 
            while(it != json.end() and it->name() != "hasSubgroups") it++;
            if(!it->as_bool()) {
              json.erase(json.find("hasSubgroups"));
              json.erase(json.find("Subgroups"));
              json.push_back(JSONNode("hasSubgroups", true));
              json.push_back(it_SimpleGroup->second.Subgroups().json("Subgroups"));
              changed = true;
            }
          }
          if(changed) Files::ReplaceFileString(filename.str(), json.write_formatted());
        }
      }
      
      // Flush the LieGroups to file(
      for(auto it_LieGroup = LieGroup::DataBase.begin(); it_LieGroup != LieGroup::DataBase.end(); it_LieGroup ++)
      {
        std::stringstream dir;
        dir << filepath.str() << it_LieGroup->first;
        
        // If the directory is not there yet, create it
        if(!Files::IsDirectory(dir.str())) Files::CreateDirectory(dir.str());
        
        std::stringstream filename;
        filename << dir.str() << "/" << it_LieGroup->first << ".out";
        
        // If the file is not there create it
        if(!Files::FileExists(filename.str())) 
          Files::WriteFileString(filename.str(), it_LieGroup->second.json().write_formatted());
        // Otherwise, if it has reps or subgroups compare it with the written file
        else if (it_LieGroup->second.hasReps() or it_LieGroup->second.hasSubgroups()) {
          JSONNode json = libjson::parse(Files::ReadFileString(filename.str()));
          bool changed = false;
          if(it_LieGroup->second.hasReps()) {
            JSONNode::const_iterator it = json.begin();
            while(it != json.end() and it->name() != "hasReps") it++;
            if(!it->as_bool()) {
              json.erase(json.find("hasReps"));
              json.erase(json.find("Reps"));
              json.push_back(JSONNode("hasReps", true));
              json.push_back(it_LieGroup->second.Reps().json("Reps"));
              changed = true;
            }
          }
          if(it_LieGroup->second.hasSubgroups()) {
            JSONNode::const_iterator it = json.begin();
            while(it != json.end() and it->name() != "hasSubgroups") it++;
            if(!it->as_bool()) {
              json.erase(json.find("hasSubgroups"));
              json.erase(json.find("Subgroups"));
              json.push_back(JSONNode("hasSubgroups", true));
              json.push_back(it_LieGroup->second.Subgroups().json("Subgroups"));
              changed = true;
            }
          }
          if(changed) Files::ReplaceFileString(filename.str(), json.write_formatted());
        }
      }
      
      // Flush the Irreps to file
      for(auto it_Irreps = Irrep::DataBase.begin(); it_Irreps != Irrep::DataBase.end(); it_Irreps ++)
      {
        std::stringstream repdir;
        repdir << filepath.str() << it_Irreps->second.Group().id() << "/reps/";
        
        // If the directory is not there yet, create it
        if(!Files::IsDirectory(repdir.str())) Files::CreateDirectory(repdir.str());
        
        std::stringstream repname;
        repname << repdir.str() << it_Irreps->first << ".out";
        if(!Files::FileExists(repname.str()))
          Files::WriteFileString(repname.str(), it_Irreps->second.json().write_formatted());
        // Otherwise, if it has weights
        else if(it_Irreps->second.hasWeights()) {
          JSONNode json = libjson::parse(Files::ReadFileString(repname.str()));
          JSONNode::const_iterator it = json.begin();
          while(it != json.end() and it->name() != "hasWeights") it++;
          if(!it->as_bool())
            Files::ReplaceFileString(repname.str(), it_Irreps->second.json().write_formatted());
        }
      }
      
      
      // Flush the Rreps to file
      for(auto it_Reps = Rrep::DataBase.begin(); it_Reps != Rrep::DataBase.end(); it_Reps ++)
      {
        std::stringstream repdir;
        repdir << filepath.str() << it_Reps->second.Group().id() << "/reps/";
        
        // If the directory is not there yet, create it
        if(!Files::IsDirectory(repdir.str())) Files::CreateDirectory(repdir.str());
        
        std::stringstream repname;
        repname << repdir.str() << it_Reps->first << ".out";
        if(!Files::FileExists(repname.str()))
          Files::WriteFileString(repname.str(), it_Reps->second.json().write_formatted());
        // Otherwise, if it has weights
        else if(it_Reps->second.hasWeights()) {
          JSONNode json = libjson::parse(Files::ReadFileString(repname.str()));
          JSONNode::const_iterator it = json.begin();
          while(it != json.end() and it->name() != "hasWeights") it++;
          if(!it->as_bool())
            Files::ReplaceFileString(repname.str(), it_Reps->second.json().write_formatted());
        }
      }
      
      // Flush the SubGroups to file
      for(auto it_Subgroups = SubGroup::DataBase.begin(); it_Subgroups != SubGroup::DataBase.end(); it_Subgroups ++)
      {
        std::stringstream subdir;
        subdir << filepath.str() << it_Subgroups->second.SuperGroup().id() << "/subs/";
        
        // If the directory is not there yet, create it
        if(!Files::IsDirectory(subdir.str())) Files::CreateDirectory(subdir.str());
        
        std::stringstream subname;
        subname << subdir.str() << it_Subgroups->first << ".out";
        if(!Files::FileExists(subname.str()))
          Files::WriteFileString(subname.str(), it_Subgroups->second.json().write_formatted());
      }
    }
    catch (...)
    {
      throw;
    }
  }

  // Fills the model database with info from the files
  void model_database_fill()
  {
    try
    {	std::stringstream filepath;
      filepath << "./models/";
      
      //List<std::string> OutputContents = Files::GetDirectoryContents(filepath.str());
      
      //if(OutputContents.nterms()) std::cout << "Loading database...." << std::endl;

    /*	for(auto it_Contents = OutputContents.begin(); it_Contents != OutputContents.end(); it_Contents++) {
        
        std::stringstream filename;
        filename << filepath.str() << *it_Contents << "/" << *it_Contents << ".out";
        if(Files::FileExists(filename.str())) {
          JSONNode json = libjson::parse(Files::ReadFileString(filename.str()));
          if(it_Contents->find("x") == std::string::npos) {
            if(SimpleGroup::JSONDataBase.find(*it_Contents) != SimpleGroup::JSONDataBase.end())
              SimpleGroup::JSONDataBase.erase(*it_Contents);
            SimpleGroup::JSONDataBase.emplace(*it_Contents,json);
          }
          if(LieGroup::JSONDataBase.find(*it_Contents) != LieGroup::JSONDataBase.end())
            LieGroup::JSONDataBase.erase(*it_Contents);
          LieGroup::JSONDataBase.emplace(*it_Contents,json);
          
          // Parse the reps
          if(LieGroup::JSONDataBase.at(*it_Contents).at("hasReps").as_bool()) {
            JSONNode Reps;
            if(it_Contents->find("x") == std::string::npos) {
              Reps = LieGroup::JSONDataBase.at(*it_Contents).at("Irreps");
            } else {
              Reps = LieGroup::JSONDataBase.at(*it_Contents).at("Reps");
            }
            for(JSONNode::const_iterator it_Reps = Reps.begin(); it_Reps != Reps.end(); it_Reps++) {
              std::stringstream repname;
              repname << filepath.str() << *it_Contents << "/reps/" << it_Reps->as_string() << ".out";
              if(Files::FileExists(repname.str())) {
                JSONNode json = libjson::parse(Files::ReadFileString(repname.str()));
                if(it_Contents->find("x") == std::string::npos) {
                  if(Irrep::JSONDataBase.find(it_Reps->as_string()) != Irrep::JSONDataBase.end())
                    Irrep::JSONDataBase.erase(it_Reps->as_string());
                  Irrep::JSONDataBase.emplace(it_Reps->as_string(), json);
                }
                if(Rrep::JSONDataBase.find(it_Reps->as_string()) != Rrep::JSONDataBase.end())
                  Rrep::JSONDataBase.erase(it_Reps->as_string());
                Rrep::JSONDataBase.emplace(it_Reps->as_string(), json);
              }							
              //std::cout << Rrep::DataBase.at(id).Weights() << std::endl;
            }
          }
          
          // Parse the subgroups
          if(LieGroup::JSONDataBase.at(*it_Contents).at("hasSubgroups").as_bool()) {
            JSONNode Subgroups = LieGroup::JSONDataBase.at(*it_Contents).at("Subgroups");
            for(auto it_Subgroups = Subgroups.begin(); it_Subgroups != Subgroups.end(); it_Subgroups ++) {
              std::stringstream subname;
              subname << filepath.str() << *it_Contents << "/subs/" << it_Subgroups->as_string() << ".out";
              if(Files::FileExists(subname.str())) {
                JSONNode json = libjson::parse(Files::ReadFileString(subname.str()));
                if(SubGroup::JSONDataBase.find(it_Subgroups->as_string()) != SubGroup::JSONDataBase.end())
                  SubGroup::JSONDataBase.erase(it_Subgroups->as_string());
                SubGroup::JSONDataBase.emplace(it_Subgroups->as_string(), json);
              }
            }
          }
        }
      }
      //for(auto it = SimpleGroup::JSONDataBase.begin(); it != SimpleGroup::JSONDataBase.end(); it++)
      //	std::cout << it->first << " -> " << it->second.write_formatted() << std::endl;
      //for(auto it = LieGroup::JSONDataBase.begin(); it != LieGroup::JSONDataBase.end(); it++)
      //	std::cout << it->first << " --> " << it->second.write_formatted() << std::endl;
      //for(auto it = Irrep::JSONDataBase.begin(); it != Irrep::JSONDataBase.end(); it++)
      //	std::cout << it->first << " -> " << it->second.write_formatted() << std::endl;
      //for(auto it = Rrep::JSONDataBase.begin(); it != Rrep::JSONDataBase.end(); it++)
      //	std::cout << it->first << " --> " << it->second.write_formatted() << std::endl;
      //for(auto it = SubGroup::JSONDataBase.begin(); it != SubGroup::JSONDataBase.end(); it++)
      //	std::cout << it->first << " --> " << it->second.write_formatted() << std::endl;*/
    }
    catch (...)
    {
      throw;
    }
  }
  
  // Flush the model database into json files
  void model_database_flush()
  {
    try
    {
      std::stringstream dirpath;
      dirpath << "./models/";
      
      // Create directory if it doenst exist
      if(!Files::IsDirectory(dirpath.str()))
          Files::CreateDirectory(dirpath.str());
      
      // Iterate over the RGE database
      int i_RGE = 0;
      List<List<Model> >::iterator it_Models = Model::DataBase.begin();
      for(auto it_RGE = RGE::DataBase.begin(); it_RGE != RGE::DataBase.end(); it_RGE ++)
      {
        std::stringstream rgename;
        rgename << dirpath.str() << "RGEs_" << std::setfill('0') << std::setw(4) << i_RGE << ".out";
        
        // Flush the RGEs to file
        if(Files::FileExists(rgename.str()))
          Files::DeleteFile(rgename.str());
        Files::WriteFileString(rgename.str(), it_RGE->json().write_formatted());
        
        // Create the model directory if it doesn't exist
        std::stringstream dirmodelname;
        dirmodelname << dirpath.str() << "Model_" << std::setfill('0') << std::setw(4) << i_RGE << "/";
        if(!Files::IsDirectory(dirmodelname.str()))
          Files::CreateDirectory(dirmodelname.str());
        
        // Iterate over the models
        int i_Models = 0;
        for(auto it_Models2 = it_Models->begin(); it_Models2 != it_Models->end(); it_Models2 ++)
        {
          std::stringstream modelname;
          modelname << dirmodelname.str() << "Model_" << std::setfill('0') << std::setw(4) << i_RGE << "_" << std::setfill('0') << std::setw(4) << i_Models << ".out";
          
          // Flush the model to file
          if(Files::FileExists(modelname.str()))
            Files::DeleteFile(modelname.str());
          Files::WriteFileString(modelname.str(), it_Models2->json().write_formatted());
          
          // Increment counters
          i_Models++;
        }

        // Increment counters
        i_RGE++;
        it_Models++;
      }
      
      RGE::DataBase.Clear();
      Model::DataBase.Clear();
    }
    catch (...)
    {
      throw;
    }
  }

  // Filter the database with the observables
  void model_database_filter(List<std::string> &observables)
  {
    try
    {
      std::cout << "Filtering models" << std::endl;
      std::cout << observables << std::endl;
    
      std::stringstream filepath;
      filepath << "./models/";
      
      List<std::string> OutputContents = Files::GetDirectoryContents(filepath.str());
      
      std::cout << OutputContents.nterms() << std::endl;
      int count = 0;
      
      for(auto it_Contents = OutputContents.begin(); it_Contents != OutputContents.end(); it_Contents++)
      {
        std::cout << count << std::endl;
        count++;
        std::stringstream dirname;
        dirname << filepath.str() << *it_Contents;
        if(Files::IsDirectory(dirname.str()))
        {
          List<std::string> DirContents = Files::GetDirectoryContents(dirname.str());

          //std::cout << DirContents.nterms() << std::endl;
          for(auto it_files = DirContents.begin(); it_files != DirContents.end(); it_files++)
          {
            std::stringstream filename;
            filename << dirname.str() << "/" << *it_files;

            if(Files::FileExists(filename.str())) {
              JSONNode json = libjson::parse(Files::ReadFileString(filename.str()));
              Model model(json);
              //std::cout << filename.str() << std::endl;
            }
          }
        }
      }
      
    }
    catch (...)
    {
      throw;
    }
  }
}
