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
			List<Tree<SimpleGroup> > _BreakingChain;
			List<Rrep> _Reps;
			bool _anomalyFree;
			List<std::string> _protonDecay;
			
		public:
			
			Theory(LieGroup &, List<Tree<SimpleGroup> > &, List<Rrep> &);
			Theory(const Theory &);
			Theory(Theory &&);
			~Theory();
			Theory &operator=(const Theory &);
			Theory &operator=(Theory &&);
			
			LieGroup Group() const;
			List<Tree<SimpleGroup> > BreakingChain() const;
			List<Rrep> Reps() const;
			bool anomalyFree() const;
			List<std::string> protonDecay() const;
	};
	
}

#endif /* __THEORY_H */