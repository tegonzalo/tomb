/*
 * model.cc
 * Created by T. Gonzalo on 12/01/16
 * Last modified on 12/01/16
 */


#include "headers.h"

/**************************************************/
/************Class Model methods*************/
/**************************************************/

namespace Tomb
{

	// Member functions

	/* Constructor */
	Model::Model(Theory theory) : List<Theory>(theory) {

		try {
			_nsteps = 1;
			_scales.Clear();
			
		} catch (...) {
			throw;
		}
	}

	/* Copy constructor 1 */
	Model::Model(const Model &M) : List<Theory>(M) {
		
		try {
			_nsteps = M.nsteps();
			_scales = M.scales();
			
		} catch (...) {
			throw;
		}
	}
	
	/* Copy constructor 2 */
	Model::Model(const List<Theory> &M) : List<Theory>(M) {
		
		try {
			_nsteps = M.nterms();
			_scales.Clear();
			
		} catch (...) {
			throw;
		}
	}

	/* Move constructor */
	Model::Model(Model && M) :
		List<Theory>(std::move(M)),
		_nsteps(std::move(M._nsteps)),
		_scales(std::move(M._scales))
	{
		
		try {
			M._nsteps = 0;
			M._scales.Clear();
			
		} catch (...) {
			throw;
		}
	}

	/* Destructor */
	Model::~Model() {
	}

	/* Assignment operator 1 */
	Model &Model::operator=(const Model &M) {
		
		try {
			if(this == &M) return *this;
			   
			List<Theory>::operator=(M);
			
			_nsteps = M.nsteps();
			_scales = M.scales();
			
			return *this;
		} catch (...) {
			throw;
		}
	}

	/* Assignment operator 2 */
	Model &Model::operator=(const List<Theory> &M) {
		
		try {
			if(this == &M) return *this;
			   
			List<Theory>::operator=(M);
			
			_nsteps = M.nterms();
			_scales.Clear();
			
			return *this;
		} catch (...) {
			throw;
		}
	}
	
	/* Move assignment operator */
	Model &Model::operator=(Model &&M) {
		
		try {
			if(this == &M) return *this;
			
			_nsteps = std::move(M._nsteps);
			_scales = std::move(M._scales);
			
			M._nsteps = 0;
			M._scales.Clear();
			
			return *this;
		} catch (...) {
			throw;
		}
	}
	
	
	/* NSteps getter */
	int Model::nsteps() const {
		return _nsteps;
	}
	
	List<double> Model::scales() const {
		return _scales;
	}
}