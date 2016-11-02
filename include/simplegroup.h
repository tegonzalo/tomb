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

/**********************************/
/* Class SimpleGroup declarations */
/**********************************/

namespace Tomb
{

  class SimpleGroup {

    protected:
      int _rank = 0;
      char _type = '\0';
      int _dim = 0;
      int _order = 0;
      std::string _label = "";
      bool _abelian = false;
      Matrix<double> _Cartan;
      Matrix<double> _G;
      double _Casimir = 0;
      int _repsMaxDim = 50;
      bool _hasReps = false;
      bool _hasSubgroups = false;
      List<Root> _PRoots;
      List<Irrep> _Irreps;
      List<SubGroup> _MaxSubgroups;
      List<SubGroup> _Subgroups;
		
    public:
      //static std::map<std::string, SimpleGroup> DataBase;
      //static std::map<std::string, JSONNode> JSONDataBase;
			
      //bool database_check(std::string, std::string = "");
      //void database_emplace(std::string, SimpleGroup);
			
      SimpleGroup(int,char);
      SimpleGroup(int, const Matrix<double> &);
      SimpleGroup(const std::string);
      SimpleGroup(const JSONNode &);
      SimpleGroup(const SimpleGroup &);
      SimpleGroup(SimpleGroup &&);
      ~SimpleGroup();
      SimpleGroup &operator=(const SimpleGroup &);
      SimpleGroup &operator=(SimpleGroup &&);
			
      void init();
      std::string id() const;
      int rank() const;
      char type() const;
      int dim() const;
      int order() const;
      std::string label() const;
      bool abelian() const;
      Matrix<double> Cartan() const;
      Matrix<double> G() const;
      double Casimir() const;
      int repsMaxDim() const;
      bool hasReps() const;
      bool hasSubgroups() const;
      Root SRoot(int);
      List<Root> PRoots(std::string = "Dynkin");
      List<Root> Roots();
      List<Irrep> SimpleReps();
      Irrep GeneratingRep();
      Irrep AdjointRep();
      Irrep SingletRep();
      List<Irrep> &Irreps(int=50);
      List<Irrep> IrrepsConst() const;
      Matrix<double> ExtendedCartan();
      List<SubGroup> &MaximalSubgroups();
      List<SubGroup> SpecialSubgroups();
      List<SubGroup> &Subgroups();
      List<SubGroup> SubgroupsConst() const;
      List<SubGroup> Subgroups(int);
      List<SubGroup> Subgroups(int, int);
      List<List<Tree<SimpleGroup> > > BreakingChains(const SimpleGroup &);
      List<List<Tree<SimpleGroup> > > BreakingChains(const LieGroup &);
      List<Product<Irrep> > Invariants(const List<Irrep> &, const int = 4);
      bool isSubgroupOf(LieGroup) const;
      bool isSubgroupOf(SimpleGroup) const;
      bool operator==(const SimpleGroup &) const;
      bool operator!=(const SimpleGroup &) const;
      bool operator>(const SimpleGroup &) const;
      bool operator<(const SimpleGroup &) const;
      std::string Print() const;
      std::string Table() const;
      JSONNode json(std::string = "") const;
      void ParseJSON(const JSONNode &n, std::string = "");
      static bool GroupExists(int, char);
    };	

    std::ostream &operator<<(std::ostream &, const SimpleGroup &);

}

#endif /* __SIMPLEGROUP_H */
