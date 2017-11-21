/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * tomb.cc
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 12/01/2016
 */

#include <omp.h>
#include "files.h"
#include "database.h"
#include "liegroup.h"
#include "theory.h"
#include "model.h"

/**************************/
/* Main function for Tomb */
/**************************/

using namespace Tomb;

void die()
{
  std::cerr << "TOMB::Wrong sintax or file doesn't exists" << std::endl;
}

bool parsearguments(int argc, char *argv[], std::string &filename, std::string &mode) 
{
  try {
    switch(argc)
    {
      case 2:
        if(!strcmp(argv[1],"--clean"))
        {
          mode = "remove";
          return true;
        }
        return false;
      case 3:
        filename = argv[2];
    
        if(!strcmp(argv[1],"-f") and Files::FileExists(filename))
        {
          mode = "file";
          return true;
        }
        else if(!strcmp(argv[1],"-o") and Files::FileExists(filename))
        {
          mode = "observables";
          return true;
        }
    
        return false;
      default:
        return false;
    }
    
    return false;
    
  } catch (...) {
    throw;
  }
}

int main(int argc, char *argv[])
{
  try {
    
    std::string filename, mode;
    
    if(parsearguments(argc, argv, filename, mode))
    {
      if(mode == "remove")
      {
        char cont;
        std::cout << "You are about to delete all the models, do you want to continue (Y/N)" << std::endl;
        do
        {
          cont = getchar();
        }
        while (cont != 'y' and cont != 'Y' and cont != 'n' and cont != 'N');

        if(cont == 'y' or cont == 'Y')
        {
          Files::EmptyDirectory("./database/Model");
          Files::EmptyDirectory("./database/RGE");
        }
      }
      else if(mode == "file")
      {
        JSONNode json = libjson::parse(Files::ReadFileString(filename));
     
        JSONNode::iterator i = json.begin();
        while(i != json.end() and i->name() != "Data") i++;
      
       // Pull all the available info from the database first of all
        database_fill<Model, RGE, SimpleGroup, LieGroup, SubGroup, Irrep, Rrep>();

        // Empty the temp directory in case there was leftovers from a previous run
        if(Files::IsDirectory("./temp")) Files::EmptyDirectory("./temp");

        Theory theory(*i);
        Model model(theory);
      
        // Get the number of reps
        i = json.begin();
        while(i != json.end() and i->name() != "NReps") i++;
        int nreps = i->as_int();
        
        double time1 = omp_get_wtime();
        long int success = model.generateModels(nreps);
        std::cout << "Number of successful models: " << success << std::endl;
        std::cout << "Time used to generate models = " << omp_get_wtime() - time1 << std::endl;

        // Flush all the databases to files
        database_flush<Model, RGE, SimpleGroup, LieGroup, SubGroup, Irrep, Rrep>();
        
/*        // Filter with the observables
        i = json.begin();
        while(i != json.end() and i->name() != "Observables") i++;
        
        if(i != json.end())
        {
          List<std::string> observables(*i);
          //Tomb::model_database_filter(observables);
        }
        
        std::cout << "END" << std::endl;
      }
      else if(mode == "observables")
      {
        JSONNode json = libjson::parse(Files::ReadFileString(filename));
      
        JSONNode::iterator i = json.begin();
        while(i != json.end() and i->name() != "Observables") i++;
        
        List<std::string> observables(*i);
        //Tomb::model_database_filter(observables);
  */      
      }
    }
    else
      die();

  }
  catch (std::exception &e)
  {
    std::cerr << "Exception caught: " << e.what() << std::endl;
  }
  catch (const std::string e)
  {
    std::cerr << "Exception caught: " << e << std::endl;
  }
  catch (const char *e)
  {
    std::cerr << "Exception caught: " << e << std::endl;
  }
  catch (...)
  {
    std::cerr << "Exception caught: Unspecified exception" << std::endl;
  }
}

