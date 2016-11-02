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
      std::string _Group = "";
      int _GroupRank = 0;
      int _dim = 0;
      bool _real = true;
      int _nirreps = 0;
      Weight *_HWeight = NULL;
      std::string _label = "";
      List<double> _Casimir;
      List<double> _DynkinIndex;
      bool _hasWeights = false;
      List<Weight> _Weights;

    public:
      Rrep();
      Rrep(const LieGroup &, const Weight &);
      Rrep(const SubGroup &, const Weight &);
      Rrep(const Irrep &);
      Rrep(const std::string);
      Rrep(const JSONNode &);
      Rrep(const Rrep &);
      Rrep(Rrep &&);
      ~Rrep();
      Rrep &operator=(const Rrep &);
      Rrep &operator=(Rrep &&);
      
      void init();
      std::string id() const;
      LieGroup Group() const;
      std::string GroupId() const;
      int GroupRank() const;
      int dim() const;
      bool real() const;
      int nirreps() const;
      std::string label() const;
      List<double> Casimir() const;
      List<double> DynkinIndex() const;
      bool hasWeights() const;
      Weight HWeight() const;
      void setWeights(const List<Weight> &);
      List<Weight> &Weights();
      List<Weight> WeightsConst() const;
      Rrep AddIrrep(Irrep);
      Rrep AddRrep(Rrep);
      bool isSinglet(int = -1) const;
      List<Weight> Project(SubGroup);
      Sum<Rrep> Decompose(SubGroup &);
      Sum<Rrep> operator*(Rrep);
      static Sum<Rrep> Product(List<Rrep> &);
      bool hasSinglet() const;
      bool operator==(const Rrep &) const;
      bool operator!=(const Rrep &) const;
      bool operator>(const Rrep &) const;
      bool operator<(const Rrep &) const;
      JSONNode json(std::string = "") const;
      void ParseJSON(const JSONNode &n, std::string ="");
    
  };
}

#endif  /* __RREP_H */
