/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
* \file
* theory.h
*
* \author
* T. Gonzalo (t.e.gonzalo@fys.uio.no)
*
* \date
* 12/01/2016
*/

#ifndef __THEORY_H
#define __THEORY_H

#include <map>
#include "matrix.h"
#include "linkedlists.h"
#include "libjson.h"
#include "liegroup.h"
#include "chain.h"
#include "field.h"
#include "sm.h"
#include "observables.h"

/*****************************/
/* Class Theory declarations */
/*****************************/

namespace Tomb
{
  class Theory
  {
    
    protected:
      
      //std::string _Group;
      LieGroup *_Group;
      Chain _BreakingChain;
      List<Field> _Fields;
      double _norm = 1;
      RVector<double>  _Mixing;
      bool _anomalyFree;
      std::map<std::string, bool> _observables;
      
    public:
      
      Theory();
      Theory(LieGroup &, Chain &, List<Field> &);
      Theory(const Theory &);
      Theory(const JSONNode &);
      Theory(Theory &&);
      ~Theory();
      Theory &operator=(const Theory &);
      Theory &operator=(Theory &&);
      
      LieGroup &Group() const;
      std::string GroupId() const;
                        LieGroup *GroupPtr() const;
      Chain BreakingChain() const;
      List<Field> Fields() const;
      
      double norm() const;
      RVector<double> Mixing() const;
      void setMixing(RVector<double>);
      
      bool chirality() const;
      bool anomalyFree() const;
      void calculateAnomaly();
      std::map<std::string, bool> observables() const;
      void calculateObservables(List<Field> &);
      
      List<Field> getScalars() const;
      List<Field> getFermions() const;
      
      bool operator==(const Theory &) const;
      bool operator!=(const Theory &) const;
      
      List<Field> findBreakingReps(SubGroup &, const List<Field> & = List<Field>());
      List<Sum<Field> > calculateBreaking(List<RVector<double> > &);
      bool containsSM() const;
      double normaliseToSM();
      double normaliseMixing(double);
      double normaliseReps(double);
      static List<Field> normaliseReps(List<Field>, double);
      
      List<List<Field> > generatePossibleReps(int);
      
      JSONNode json(std::string = "") const;
      void ParseJSON(const JSONNode &);
  };
  
  
  std::ostream &operator<<(std::ostream &, const Theory &);
  
}

#endif /* __THEORY_H */
