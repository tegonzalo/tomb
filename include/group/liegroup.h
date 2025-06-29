/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * liegroup.h
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 15/02/2012
 */

#ifndef __LIEGROUP_H
#define __LIEGROUP_H

#include "omp.h"
#include "linkedlists.h"
#include "tree.h"
#include "libjson.h"
#include "helper_functions.h"
#include "database.h"
#include "simplegroup.h"
#include "rrep.h"

// Forward declarations
namespace Tomb
{
  class SubGroup;
}

using namespace std;

/*******************************/
/* Class LieGroup declarations */
/*******************************/

namespace Tomb 
{
  class LieGroup: public Product<SimpleGroup>
  {
    protected:
      int _rank = 0;
      int _dim = 0;
      bool _simple = false;
      bool _semisimple = false;
      int _ngroups = 0;
      int _nabelians = 0;
      string _label = "";
      List<double> _Casimir;
      int _repsMaxDim = 50;

   public:
      List<Rrep> _Reps;
      List<SubGroup> _MaxSubgroups;
      List<SubGroup> _Subgroups;
      List<List<Tree<SimpleGroup> > > _BrChains;
      
      LieGroup();
      LieGroup(const string);
      LieGroup(const JSONNode &);
      LieGroup(const LieGroup &);
      LieGroup(const SimpleGroup &);
      LieGroup(const List<SimpleGroup> &);
      LieGroup(const List<SimpleGroup *> &);
      LieGroup(const Product<SimpleGroup> &);
      LieGroup(LieGroup &&);
      ~LieGroup();
      LieGroup &operator=(const LieGroup &);
      LieGroup &operator=(const SimpleGroup &);
      LieGroup &operator=(const List<SimpleGroup> &);
      LieGroup &operator=(const List<SimpleGroup *> &);
      LieGroup &operator=(const Product<SimpleGroup> &);
      LieGroup &operator=(LieGroup &&);
    
      void init();
      string id() const;
      int rank() const;
      int dim() const;
      bool simple() const;
      bool semisimple() const;
      int ngroups() const;
      int nabelians() const;
      bool abelian() const;
      string label() const;
      List<double> Casimir() const;
      int repsMaxDim() const;
      List<Rrep> Reps() const;
      List<Rrep> &Reps(int);
      List<SubGroup> MaximalSubgroups() const;
//      List<SubGroup> SpecialSubgroups();
      List<SubGroup> Subgroups() const;
      bool isSubgroupOf(LieGroup) const;
      bool isSubgroupOf(SimpleGroup) const;

      void DeleteTerm(int);
      void AddTerm(const SimpleGroup &);
      void AddTerm(const LieGroup &);
 
      List<Rrep> SimpleReps();
      Rrep GeneratingRep();
      List<Rrep> AdjointReps();
      Rrep SingletRep();

      List<Rrep> &Irreps2Reps(List<Irrep>);
      List<Rrep> &CalculateReps(int=50);
      List<SubGroup> &CalculateMaximalSubgroups();
      List<SubGroup> &CalculateSubgroups();
      List<SubGroup> Subgroups(int);
      List<SubGroup> Subgroups(int, int);
      List<SubGroup> SplitToSubGroups(int = 0);
      List<SubGroup> SplitToSubGroups(int, int);
      List<List<Tree<SimpleGroup> > > BreakingChains(const SimpleGroup &);
      List<List<Tree<SimpleGroup> > > BreakingChains(const LieGroup &);
//      List<Product<Rrep> > Invariants(const List<Rrep> &, const int = 4);
 
      bool operator>(const LieGroup &);
      bool operator<(const LieGroup &);
      bool operator>(const SimpleGroup &);
      bool operator<(const SimpleGroup &);
      void Order();
      bool operator==(const LieGroup &);
      bool operator==(const SimpleGroup &);
      bool operator!=(const LieGroup &);
      bool operator!=(const SimpleGroup &);
      JSONNode json(string = "") const;
      void ParseJSON(const JSONNode &n);
      static LieGroup* get(const SimpleGroup&);

  };
}


#endif /* __LIEGROUP_H */
