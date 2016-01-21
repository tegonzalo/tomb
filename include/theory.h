/*
 * theory.h
 * Created by T. Gonzalo on 12/01/16
 * Last modified on 12/01/16
 */

#ifndef __THEORY_H
#define __THEORY_H

#include "headers.h"

namespace Tomb
{
	class Theory
	{
		
		protected:
			
			LieGroup _Group;
			Chain _BreakingChain;
			List<Field> _Fields;
			RVector<double>  _Mixing;
			bool _anomalyFree;
			List<std::string> _protonDecay;
			
		public:
			
			Theory(LieGroup &, Chain &, List<Field> &);
			Theory(const Theory &);
			Theory(const JSONNode &);
			Theory(Theory &&);
			~Theory();
			Theory &operator=(const Theory &);
			Theory &operator=(Theory &&);
			
			LieGroup Group() const;
			Chain BreakingChain() const;
			List<Field> Fields() const;
			
			RVector<double> Mixing() const;
			void setMixing(RVector<double>);
			
			bool chirality() const;
			bool anomalyFree() const;
			void calculateAnomaly();
			List<std::string> protonDecay() const;
			
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
			
			JSONNode json() const;
			void ParseJSON(const JSONNode &);
	};
	
	
	std::ostream &operator<<(std::ostream &, const Theory &);
	
}

#endif /* __THEORY_H */