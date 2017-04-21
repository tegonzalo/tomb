/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * rrep.h
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 13/08/2013
 */

#ifndef __RREP_H
#define __RREP_H

#include "linkedlists.h"
#include "libjson.h"
#include "weight.h"
#include "irrep.h"

// Forward declarations 
namespace Tomb
{
  class LieGroup;
  class SubGroup;
}

/***************************/
/* Class Rrep declarations */
/***************************/

namespace Tomb
{
  class Rrep: public List<Irrep> {

    private:
      LieGroup *_Group = NULL;
      int _dim = 0;
      bool _real = true;
      int _nirreps = 0;
      Weight *_HWeight = NULL;
      std::string _label = "";
      List<double> _Casimir;
      List<double> _DynkinIndex;

    public:
      List<Weight> _Weights;

      Rrep();
      Rrep(LieGroup &, Weight &);
//      Rrep(SubGroup &, Weight &);
      Rrep(LieGroup &, const Irrep &);
      Rrep(const std::string);
//      Rrep(const JSONNode &);
      Rrep(const Rrep &);
      Rrep(Rrep &&);
      ~Rrep();
      Rrep &operator=(const Rrep &);
      Rrep &operator=(Rrep &&);
      
      void init();
      std::string id() const;
      LieGroup &Group() const;
      std::string GroupId() const;
      int dim() const;
      bool real() const;
      int nirreps() const;
      std::string label() const;
      List<double> Casimir() const;
      List<double> DynkinIndex() const;
      Weight &HWeight() const;
      List<Weight> Weights() const;
      void setWeights(const List<Weight> &);
      List<Weight> &CalculateWeights();
      Rrep AddIrrep(Irrep);
      Rrep AddRrep(Rrep);
      Rrep FinishRrep();
      bool isSinglet(int = -1) const;
//      List<Weight> Project(SubGroup);
//      Sum<Rrep> Decompose(SubGroup &);
      Sum<Rrep> operator*(Rrep);
      static Sum<Rrep> Product(List<Rrep> &);
      bool hasSinglet() const;
      bool operator==(const Rrep &) const;
      bool operator!=(const Rrep &) const;
      bool operator>(const Rrep &) const;
      bool operator<(const Rrep &) const;
      JSONNode json(std::string = "") const;
//      void ParseJSON(const JSONNode &n, std::string ="");
    
  };
}

#endif  /* __RREP_H */
