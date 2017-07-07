/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
* \file
* model.h
*
* \author
* T. Gonzalo (t.e.gonzalo@fys.uio.no)
*
* \date
* 12/01/2016
*/

#ifndef __MODEL_H
#define __MODEL_H

#include <omp.h>
#include "linkedlists.h"
#include "libjson.h"
#include "files.h"
#include "helper_functions.h"
#include "sm.h"
#include "rge.h"
#include "theory.h"

/****************************/
/* Class Model declarations */
/****************************/

namespace Tomb
{
  class Model : public List<Theory>
  {
    protected:
      
      int _nsteps;
      List<double> _scales;
      
    public:
      
      Model();
      Model(Theory);
      Model(const Model &);
      Model(const List<Theory> &);
      Model(std::string);
      Model(const JSONNode &);
      Model(Model &&);
      ~Model();
      Model &operator=(const Model &);
      Model &operator=(const List<Theory> &);
      Model &operator=(Model &&);
      
      int nsteps() const;
      List<double> scales() const;
      
      void DeleteTerm(int);
      void AddTerm(const Theory &);
      
      void generateModels(int = 0, int = 0);
      long int generateModelsRec(int = 0, int = 0);
      
      void calculateObservables();
            
      void ParseJSON(const JSONNode &);
  };
  
  std::ostream &operator<<(std::ostream &, const Model &);
}

#endif /* __MODEL_H */
