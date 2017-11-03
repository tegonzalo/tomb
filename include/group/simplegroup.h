/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * simplegroup.h
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 15/02/2012
 */

#ifndef __SIMPLEGROUP_H
#define __SIMPLEGROUP_H

#include "matrix.h"
#include "linkedlists.h"
#include "libjson.h"
#include "root.h"
#include "irrep.h"

// Forward declarations
namespace Tomb
{
  class LieGroup;
  class SubGroup;
}

using namespace std;

/**********************************/
/* Class SimpleGroup declarations */
/**********************************/

namespace Tomb
{

  class SimpleGroup
  {
    protected:
      int _rank = 0;
      char _type = '\0';
      int _dim = 0;
      int _order = 0;
      string _label = "";
      bool _abelian = false;
      Matrix<double> _Cartan;
      Matrix<double> _G;
      double _Casimir = 0;
      int _repsMaxDim = 50;
	
    public:
      List<Root> _PRoots;
      List<Root> _PRootsDual;
      List<Irrep> _Irreps;
      List<SubGroup> _MaxSubgroups;
      List<SubGroup> _Subgroups;

      SimpleGroup();
      SimpleGroup(int,char);
      SimpleGroup(int, const Matrix<double> &);
      SimpleGroup(const string);
      SimpleGroup(const JSONNode &);
      SimpleGroup(const SimpleGroup &);
      SimpleGroup(SimpleGroup &&);
      ~SimpleGroup();
      SimpleGroup &operator=(const SimpleGroup &);
      SimpleGroup &operator=(SimpleGroup &&);
			
      void init();
      string id() const;
      int rank() const;
      char type() const;
      int dim() const;
      int order() const;
      string label() const;
      bool abelian() const;
      Matrix<double> Cartan() const;
      Matrix<double> G() const;
      double Casimir() const;
      int repsMaxDim() const;
      List<Irrep> Irreps() const;
      List<Irrep> &Irreps(int);
      Root SRoot(int) const;
      List<Root> PRoots(string = "Dynkin");
      List<Root> Roots();
      List<Irrep> SimpleReps();
      Irrep GeneratingRep();
      Irrep AdjointRep();
      Irrep SingletRep();
      List<Irrep> &CalculateIrreps(int=50);
      Matrix<double> ExtendedCartan();
      List<SubGroup> MaximalSubgroups() const;
      List<SubGroup> &CalculateMaximalSubgroups();
//      List<SubGroup> SpecialSubgroups()
      List<SubGroup> Subgroups() const;
      List<SubGroup> &CalculateSubgroups();
      List<SubGroup> Subgroups(int);
      List<SubGroup> Subgroups(int, int);
      List<List<Tree<SimpleGroup> > > BreakingChains(const SimpleGroup &);
      List<List<Tree<SimpleGroup> > > BreakingChains(const LieGroup &);
//      List<Product<Irrep> > Invariants(const List<Irrep> &, const int = 4);
      bool isSubgroupOf(LieGroup) const;
      bool isSubgroupOf(SimpleGroup) const;
      bool operator==(const SimpleGroup &) const;
      bool operator!=(const SimpleGroup &) const;
      bool operator>(const SimpleGroup &) const;
      bool operator<(const SimpleGroup &) const;
      string Print() const;
      string Table() const;
      JSONNode json(string = "") const;
      void ParseJSON(const JSONNode &n);
      static bool GroupExists(int, char);
      static char GroupIdentify(int, const Matrix<double> &);
      static void RestructureCartan(Matrix<double> &);

    };	

    ostream &operator<<(ostream &, const SimpleGroup &);
}


#endif /* __SIMPLEGROUP_H */
