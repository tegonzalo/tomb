/*
 * rrep.h
 * Created by T.Gonzalo on 13/08/2013.
 * Last modified on 21/10/2015
 */

#ifndef __RREP_H
#define __RREP_H

#include "headers.h"

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
			bool _hasWeights = false;
			List<Weight> _Weights;
		public:
			
			static std::map<std::string, Rrep> DataBase;
			static std::map<std::string, JSONNode> JSONDataBase;
			static std::map<std::pair<std::string, std::string>, Sum<Rrep> > DecomposeDataBase;
			static std::map<std::pair<std::string, std::string>, Sum<Rrep> > ProductDataBase;
			
			bool database_check(std::string, std::string = "");
			void database_emplace(std::string, Rrep);
			
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
			LieGroup &Group() const;
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