/*
 * theory.cc
 * Created by T. Gonzalo on 12/01/16
 * Last modified on 12/01/16
 */


#include "headers.h"

/**************************************************/
/************Class Theory methods*************/
/**************************************************/

namespace Tomb
{

	// Member functions

	/* Constructor */
	Theory::Theory(LieGroup &Group, List<Tree<SimpleGroup> > &BreakingChain, List<Rrep> &Reps) {

		try {
			_Group = Group;
			_BreakingChain = BreakingChain;
			_Reps = Reps;
			_anomalyFree = true;
			_protonDecay.Clear();
			
		} catch (...) {
			throw;
		}
	}

	/* Copy constructor */
	Theory::Theory(const Theory &T) {
		
		try {
			_Group = T.Group();
			_BreakingChain = T.BreakingChain();
			_Reps = T.Reps();
			_anomalyFree = true;
			_protonDecay.Clear();
		} catch (...) {
			throw;
		}
	}

	/* Move constructor */
	Theory::Theory(Theory &&T) :
		_Group(std::move(T._Group)),
		_BreakingChain(std::move(T._BreakingChain)),
		_Reps(std::move(T._Reps))
	{
		
		try {
			T._Group = LieGroup();
			T._BreakingChain.Clear();
			T._Reps.Clear();
			
		} catch (...) {
			throw;
		}
	}

	/* Destructor */
	Theory::~Theory() {
	}

	/* Assignment operator */
	Theory &Theory::operator=(const Theory &T) {
		
		try {
			if(this == &T) return *this;
			
			_Group = T.Group();
			_BreakingChain = T.BreakingChain();
			_Reps = T.Reps();
			
			return *this;
		} catch (...) {
			throw;
		}
	}

	/* Move assignment operator */
	Theory &Theory::operator=(Theory &&T) {
		
		try {
			if(this == &T) return *this;
			
			_Group = std::move(T._Group);
			_BreakingChain = std::move(T._BreakingChain);
			_Reps = std::move(T._Reps);
			
			T._Group = LieGroup();
			T._BreakingChain.Clear();
			T._Reps.Clear();
			
			return *this;
		} catch (...) {
			throw;
		}
	}
	
	
	/* Group getter */
	LieGroup Theory::Group() const {
		return _Group;
	}
	
	/* Breaking chain getter */
	List<Tree<SimpleGroup> > Theory::BreakingChain() const {
		return _BreakingChain;
	}
	
	/* Reps getter */
	List<Rrep> Theory::Reps() const {
		return _Reps;
	}

	/* AnomalyFre getter */
	bool Theory::anomalyFree() const {
		return _anomalyFree;
	}
	
	/* ProtonDecay getter */
	List<std::string> Theory::protonDecay() const {
		return _protonDecay;
	}
}