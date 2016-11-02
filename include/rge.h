/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
* \file
* rge.h
*
* \author
* T. Gonzalo (t.e.gonzalo@fys.uio.no)
*
* \date
* 22/01/2016
*/

#ifndef __RGE_H
#define __RGE_H

#include "matrix.h"
#include "linkedlists.h"
#include "libjson.h"
#include "field.h"
#include "theory.h"

// Forward declarations
namespace Tomb
{
  class Model;
}

/**************************/
/* Class RGE declarations */
/**************************/

namespace Tomb
{

  class RGE
  {
    
    protected:
      
      int _ncouplings;
      Matrix<double> _bSUSY;
      Matrix<double> _bSM;
      Matrix<double> _Mixing;
      
    public:
      static List<RGE> DataBase;
      
      RGE(const Model &);
      RGE(const Theory &);
      RGE(const RGE &);
      RGE(const JSONNode &);
      RGE(std::string);
      RGE(RGE &&);
      ~RGE();
      RGE &operator=(const RGE &);
      RGE &operator=(RGE &&);
      
      int ncouplings() const;
      Matrix<double> bSUSY() const;
      Matrix<double> bSM() const;
      Matrix<double> Mixing() const;
      
      static List<double> SUSYslopes(const List<Field> &);
      static List<double> SMslopes(const List<Field> &);
      static Matrix<double> slopesMatrix(const Model &, List<double> (*)(const List<Field> &));
      static Matrix<double> mixingMatrix(const Model &);
      
      bool operator==(const RGE &) const;
      bool operator!=(const RGE &) const;
      
      JSONNode json(std::string = "id") const;
      void ParseJSON(const JSONNode &);
  };
  
  
  std::ostream &operator<<(std::ostream &, const RGE &);


}

#endif /* __RGE_H */
