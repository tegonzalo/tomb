/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
* \file
* irrep.h
*
* \author
* T. Gonzalo (t.e.gonzalo@fys.uio.no)
*
* \date
* 13/08/2013
*/

#ifndef __IRREP_H
#define __IRREP_H

#include "matrix.h"
#include "linkedlists.h"
#include "tree.h"
#include "libjson.h"
#include "weight.h"

// Forward declarations
namespace Tomb
{
  class SimpleGroup;
  class SubGroup;
  class Rrep;
}

/****************************/
/* Class Irrep declarations */
/****************************/

namespace Tomb
{
  class Irrep {

    private:
      std::string _Group = "";
      int _GroupRank = 0;
      int _nirreps = 1;
      int _dim = 0;
      bool _real = true;
      int _conjugate = 0;
      RVector<int> _congruency;
      Weight *_HWeight = NULL;
      std::string _label = "";
      double _Casimir;
      double _DynkinIndex;
      bool _hasWeights = false;
      List<Weight> _Weights;
      List<Weight> _DualWeights;

    public:
      Irrep(const SimpleGroup &, const Weight &);
      Irrep(const std::string);
      Irrep(const JSONNode &);
      Irrep(const Irrep &);
      Irrep(Irrep &&);
      ~Irrep();
      Irrep &operator=(const Irrep &);
      Irrep &operator=(Irrep &&);
    
      void init();
      std::string id() const;
      //SimpleGroup &Group() const;
      SimpleGroup Group() const;
      std::string GroupId() const;
      int GroupRank() const;
      int dim() const;
      bool real() const;
      int conjugate() const;
      RVector<int> congruency() const;
      std::string label() const;
      void setLabel(std::string);
      Weight HWeight() const;
      double Casimir() const;
      double DynkinIndex() const;
      bool hasWeights() const;
      void setWeights(const List<Weight> &);
      List<Weight> &Weights(std::string = "Dynkin");
      List<Weight> WeightsConst(std::string = "Dynkin") const;
      bool isConjugateOf(Irrep);
      bool isSinglet() const;
      List<Weight> Project(SubGroup);
      Sum<Rrep> Decompose(SubGroup);
      Sum<Irrep> operator*(Irrep);
      static Sum<Irrep> Product(List<Irrep> &);
      bool operator==(const Irrep) const;
      bool operator!=(const Irrep) const;
      bool operator>(const Irrep) const;
      bool operator<(const Irrep) const;
      std::string Print() const;
      std::string Table() const;
      JSONNode json(std::string = "") const;
      void ParseJSON(const JSONNode &n, std::string = "");

  
  };
  
  std::ostream &operator<<(std::ostream &, const Irrep &);
}


#endif /* __IRREP_H */
