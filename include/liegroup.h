/*
 * liegroup.h
 * Created by T.Gonzalo on 15/02/2012.
 * Last modified on 25/04/2014..
 */

#ifndef __LIEGROUP_H
#define __LIEGROUP_H

#include "headers.h"

namespace Tomb 
{
	class LieGroup: public Product<SimpleGroup>{

		protected:
			int _rank = 0;
			int _dim = 0;
			bool _simple = false;
			bool _semisimple = false;
			int _ngroups = 0;
			int _nabelians = 0;
			std::string _label = "";
			List<double> _Casimir;
			int _repsMaxDim = 50;
			bool _hasReps = false;
			bool _hasSubgroups = false;
			List<Rrep> _Reps;
			List<SubGroup> _MaxSubgroups;
			List<SubGroup> _Subgroups;
		public:
			
			static std::map<std::string, LieGroup> DataBase;
			static std::map<std::string, JSONNode> JSONDataBase;
			
			bool database_check(std::string, std::string = "");
			void database_emplace(std::string, LieGroup);
			
			LieGroup();
			LieGroup(const std::string);
			LieGroup(const JSONNode &);
			LieGroup(const LieGroup &);
			LieGroup(const SimpleGroup &);
			LieGroup(const List<SimpleGroup> &);
			LieGroup(const Product<SimpleGroup> &);
			LieGroup(LieGroup &&);
			~LieGroup();
			LieGroup &operator=(const LieGroup &);
			LieGroup &operator=(const SimpleGroup &);
			LieGroup &operator=(const List<SimpleGroup> &);
			LieGroup &operator=(const Product<SimpleGroup> &);
			LieGroup &operator=(LieGroup &&);
		
			void init();
			std::string id() const;
			int rank() const;
			int dim() const;
			bool simple() const;
			bool semisimple() const;
			int ngroups() const;
			int nabelians() const;
			bool abelian() const;
			std::string label() const;
			List<double> Casimir() const;
			int repsMaxDim() const;
			bool hasReps() const;
			bool hasSubgroups() const;
			bool isSubgroupOf(LieGroup) const;
			bool isSubgroupOf(SimpleGroup) const;
			void DeleteTerm(int);
			void AddTerm(const SimpleGroup &);
			void AddTerm(const LieGroup &);
			List<SubGroup> &MaximalSubgroups();
			List<SubGroup> SpecialSubgroups();
			List<SubGroup> &Subgroups();
			List<SubGroup> SubgroupsConst() const;
			List<SubGroup> Subgroups(int);
			List<SubGroup> Subgroups(int, int);
			List<SubGroup> SplitToSubGroups(int = 0);
			List<SubGroup> SplitToSubGroups(int, int);
			List<List<Tree<SimpleGroup> > > BreakingChains(const SimpleGroup &);
			List<List<Tree<SimpleGroup> > > BreakingChains(const LieGroup &);
			List<Product<Rrep> > Invariants(const List<Rrep> &, const int = 4);
			List<Rrep> SimpleReps();
			Rrep GeneratingRep();
			List<Rrep> AdjointReps();
			Rrep SingletRep();
			List<Rrep> &Reps(int=50);
			List<Rrep> RepsConst() const;
			List<Rrep> Irreps2Reps(List<Irrep>);
			bool operator>(const LieGroup &);
			bool operator<(const LieGroup &);
			bool operator>(const SimpleGroup &);
			bool operator<(const SimpleGroup &);
			void Order();
			bool operator==(const LieGroup &);
			bool operator==(const SimpleGroup &);
			bool operator!=(const LieGroup &);
			bool operator!=(const SimpleGroup &);
			JSONNode json(std::string = "") const;
			void ParseJSON(const JSONNode &n, std::string = "");

	};
}


#endif /* __LIEGROUP_H */
