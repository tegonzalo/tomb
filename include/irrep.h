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

using namespace std;

/****************************/
/* Class Irrep declarations */
/****************************/

namespace Tomb
{
  class Irrep {

    private:
      SimpleGroup* _Group = NULL;
      int _nirreps = 1;
      int _dim = 0;
      bool _real = true;
      int _conjugate = 0;
      RVector<int> _congruency;
      Weight *_HWeight = NULL;
      string _label = "";
      double _Casimir;
      double _DynkinIndex;

    public:
      List<Weight> _Weights;
      List<Weight> _DualWeights;

      Irrep(SimpleGroup&, Weight&);
      Irrep(const string);
//      Irrep(const JSONNode &);
      Irrep(const Irrep &);
      Irrep(Irrep &&);
      ~Irrep();
      Irrep &operator=(const Irrep &);
      Irrep &operator=(Irrep &&);
    
      void init();
      string id() const;
      SimpleGroup &Group() const;
      int dim() const;
      bool real() const;
      int conjugate() const;
      RVector<int> congruency() const;
      string label() const;
      void setLabel(string);
      Weight &HWeight() const;
      double Casimir() const;
      double DynkinIndex() const;
      List<Weight> Weights() const;
      List<Weight> DualWeights() const;
      void setWeights(const List<Weight> &);
      List<Weight> &CalculateWeights();
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
      string Print() const;
      string Table() const;
      JSONNode json(string = "") const;
//      void ParseJSON(const JSONNode &n, string = "");
      static Irrep* find(string);
      static Irrep* get(string);
  
  };
  
  ostream &operator<<(ostream &, const Irrep &);
}


#endif /* __IRREP_H */
