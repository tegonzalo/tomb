/*
 * irrep.cc
 * Created by T.Gonzalo on 13/08/2013.
 * Last modified on 10/11/2015
 */

#include "headers.h"

/**************************************************/
/***************Class Irrep methods****************/
/**************************************************/

namespace Tomb
{
	// Definition of static variables and helper functions
	
	std::map<std::string, Irrep> Irrep::DataBase;
	std::map<std::string, JSONNode> Irrep::JSONDataBase;
	std::map<std::pair<std::string, std::string>, Sum<Rrep> > Irrep::DecomposeDataBase;
	std::map<std::pair<std::string, std::string>, Sum<Irrep> > Irrep::ProductDataBase;
	
	bool Irrep::database_check(std::string id, std::string what) {
		if(DataBase.find(id) != DataBase.end()) {
			if (what == "")
				return true;
			if(what == "Weights" and DataBase.at(id).hasWeights())
				return true;
		}
		if(JSONDataBase.find(id) != JSONDataBase.end()) {
			Irrep R(JSONDataBase.at(id));
			if(what == "Weights") {
				//if(R.Group().rank() < 6) return false;
				R.ParseJSON(JSONDataBase.at(id),"Weights");
			}
			if(DataBase.find(id) != DataBase.end())
				DataBase.erase(id);
			DataBase.emplace(id, R);
			return true;
		}
		return false;
	}
	
	void Irrep::database_emplace(std::string id, Irrep R) {
		if(database_check(id))
			DataBase.erase(id);
		DataBase.emplace(id, R);
	}
	
	// Member functions

	/* Constructor */
	Irrep::Irrep(const SimpleGroup &G, const Weight &HW){

		try {
			_Group = new SimpleGroup(G);
			_HWeight = new Weight(HW);
			init();
		} catch (...) {
			throw;
		}

	}

	/* Constructor 2 */
	Irrep::Irrep(const std::string id) {
		
		try {
			
			_HWeight = new Weight(id);
			//_Group = new SimpleGroup(HWeight().Group().GetObject(0));
			_Group = new SimpleGroup(HWeight().Group());
			
			init();
			
		} catch (...) {
			throw;
		}
	}

	/* Constructor 3, with json nodes */
	Irrep::Irrep(const JSONNode &n) {
		if(n.as_string() != "") {
					
			_HWeight = new Weight(n.as_string());
			//_Group = new SimpleGroup(HWeight().Group().GetObject(0));
			_Group = new SimpleGroup(HWeight().Group());
			
			init();
			
		} else {
			ParseJSON(n);
		}
	}

	/* Copy constructor */
	Irrep::Irrep(const Irrep &Rep) {
		
		try {
			_Group = new SimpleGroup(Rep.Group());
			_dim = Rep.dim();
			_real = Rep.real();
			_conjugate = Rep.conjugate();
			_congruency = Rep.congruency();
			_HWeight = new Weight(Rep.HWeight());
			_label = Rep.label();
			_Casimir = Rep.Casimir();
			_DynkinIndex = Rep.DynkinIndex();
			_hasWeights = Rep.hasWeights();
			if(_hasWeights) _Weights = Rep.WeightsConst();
			if(_hasWeights) _DualWeights = Rep.WeightsConst("Dual");

		} catch (...) {
			throw;
		}
	}

	/* Move constructor */
	Irrep::Irrep(Irrep &&Rep) :
		_Group(std::move(Rep._Group)),
		_dim(std::move(Rep._dim)),
		_real(std::move(Rep._real)),
		_conjugate(std::move(Rep._conjugate)),
		_congruency(std::move(Rep._congruency)),
		_HWeight(std::move(Rep._HWeight)),
		_label(std::move(Rep._label)),
		_Casimir(std::move(Rep._Casimir)),
		_DynkinIndex(std::move(Rep._DynkinIndex)),
		_hasWeights(std::move(Rep._hasWeights)),
		_Weights(std::move(Rep._Weights)),
		_DualWeights(std::move(Rep._DualWeights))
		
	{
		try {
			Rep._Group = nullptr;
			Rep._dim = 0;
			Rep._real = false;
			Rep._conjugate = 0;
			Rep._congruency = RVector<int>();
			Rep._HWeight = nullptr;
			Rep._label = "";
			Rep._Casimir = 0;
			Rep._DynkinIndex = 0;
			Rep._hasWeights = false;
			Rep._Weights.Clear();
			Rep._DualWeights.Clear();

		} catch (...) {
			throw;
		}
	}

	/* Destructor */
	Irrep::~Irrep() {
		//std::cout << "deleting irrep" << std::endl;
		if(_Group != NULL) delete _Group;
		if(_HWeight != NULL) delete _HWeight;
	}

	/* Overloaded = operator */
	Irrep &Irrep::operator=(const Irrep &Rep) {
		try {
			if(this == &Rep) return *this;
			this->~Irrep();
			_Group = new SimpleGroup(Rep.Group());
			_dim = Rep.dim();
			_real = Rep.real();
			_conjugate = Rep.conjugate();
			_congruency = Rep.congruency();
			_HWeight = new Weight(Rep.HWeight());
			_label = Rep.label();
			_Casimir = Rep.Casimir();
			_DynkinIndex = Rep.DynkinIndex();
			_hasWeights = Rep.hasWeights();
			if(_hasWeights) _Weights = Rep.WeightsConst();
			if(_hasWeights) _DualWeights = Rep.WeightsConst("Dual");
			return *this;
		} catch (...) {
			throw;
		}
	}

	/* Move = operator */
	Irrep &Irrep::operator=(Irrep &&Rep) {
		try {
			if(this == &Rep) return *this;
			this->~Irrep();
			_Group = std::move(Rep._Group);
			_dim = std::move(Rep._dim);
			_real = std::move(Rep._real);
			_conjugate = std::move(Rep._conjugate);
			_congruency = std::move(Rep._congruency);
			_HWeight = std::move(Rep._HWeight);
			_label = std::move(Rep._label);
			_Casimir = std::move(Rep._Casimir);
			_DynkinIndex = std::move(Rep._DynkinIndex);
			_hasWeights = std::move(Rep._hasWeights);
			_Weights = std::move(Rep._Weights);
			_DualWeights = std::move(Rep._DualWeights);
			
			Rep._Group = nullptr;
			Rep._dim = 0;
			Rep._real = false;
			Rep._conjugate = 0;
			Rep._congruency = RVector<int>();
			Rep._HWeight = nullptr;
			Rep._label = "";
			Rep._Casimir = 0;
			Rep._DynkinIndex = 0;
			Rep._hasWeights = false;
			Rep._Weights.Clear();
			Rep._DualWeights.Clear();
			
			return *this;
		} catch (...) {
			throw;
		}
	}

	/* Initialises some internal variables */
	void Irrep::init() {

		try {
			
			if(_Group == NULL or _HWeight == NULL) {
				throw "Irrep::init::Not enough information to initialise the variables";
			}
			
			//std::cout << "Group = " << *_Group << " and HWeight = " <<  *_HWeight << std::endl;
			
			if (database_check(id())) {
				*this = DataBase.at(id());
			} else {
				
				double aux = 1;
				int rank = _Group->rank();
				
				double a,b;
				int nproots = (int)(0.5*(_Group->dim()-rank));
				List<Root> PRoots(_Group->PRoots("Dual"));
		
				for(int i=0; i<nproots; i++) {
					a = 0;
					b = 0;
					for(int j=0; j<rank; j++) {
						a += PRoots.GetObject(i)[j]*((*_HWeight)[j]+1);
						b += PRoots.GetObject(i)[j];
					}
					aux *= a*1./b;
				}
			
				_dim = (int) aux;
				if((aux - _dim) > 0.5) {
					_dim++;
				}
			
				if(_dim < 0) {
					throw "Irrep::init::Dimension cannot be negative";
				}
				
				switch(_Group->type()) {
			
					case 'A':
						_real = true;
						for(int i=0; i<_Group->rank(); i++) {
							if((*_HWeight)[i] != (*_HWeight)[_Group->rank()-i-1]) {
								_real = false;
							}
						}
						break;

					case 'B':
						_real = true;
						break;

					case 'C':
						_real = true;
						break;

					case 'D':
						if(_Group->rank()%2) {
							if((*_HWeight)[_Group->rank()-1] == (*_HWeight)[_Group->rank()-2]) {
								_real = true;
							} else {
								_real = false;
							}
						} else {
							_real = true;
						}
						break;

					case 'E':
						if(_Group->rank() != 6) {
							_real = true;
						} else {
							_real = true;
							for(int i=0; i<_Group->rank()-1; i++) {
								if((*_HWeight)[i] != (*_HWeight)[_Group->rank()-i-2]) {
									_real = false;
								}
							}
						}
						break;

					case 'F':
						_real = true;
						break;

					case 'G':
						_real = true;
						break;
				
				}
				
				_conjugate = 0;

				char label[10];
				if(_Group->abelian()) {
					sprintf(label, "%.3f", (*_HWeight)[0]);
				} else {
					sprintf(label, "%d", _dim);
				}
				_label = label;

				if(_real == false and rank > 1 and _dim > 0) {
					_conjugate = 1;	
					switch(_Group->type()) {
						case 'A':
							for(int i=0; i<rank/2;i++) {
								if((*_HWeight)[i]>(*_HWeight)[rank-1-i]){
									_conjugate = 0;
								}
							}
							break;
						case 'D':
							if((*_HWeight)[rank-1]<(*_HWeight)[rank-2]) {
								_conjugate = 0;
							}
							break;
						case 'E':
							for(int i=0; i<(rank-1)/2;i++) {
								if((*_HWeight)[i]>(*_HWeight)[rank-2-i]){
									_conjugate = 0;
								}
							}
							break;
					}
				}


				_congruency = RVector<int>(1);
				switch(_Group->type()) {
					case 'A':
						//_congruency = new RVector<int>(1);
						_congruency[0] = 0;
						for(int i=0; i<rank; i++) {
							_congruency[0] += (*_HWeight)[i];
						}
						_congruency[0] %= rank + 1;
						break;
					case 'B':
						//_congruency = new RVector<int>(1);
						_congruency[0] = (int) (*_HWeight)[rank-1] % 2;
						break;
					case 'C':
						//_congruency = new RVector<int>(1);
						_congruency[0] = 0;
						for(int i=0; i<rank; i+=2) {
							_congruency[0] += (*_HWeight)[i];
						}
						_congruency[0] %= 2;
						break;
					case 'D':
						//_congruency = new RVector<int>(2);
						_congruency = RVector<int>(2);
						_congruency[0] = (int)((*_HWeight)[rank-1] + (*_HWeight)[rank-2]) % 2;
						_congruency[1] = 0;
						for(int i=0; i<rank-2; i++) {
							_congruency[1] += 2*(*_HWeight)[i];
						}
						_congruency[1] += (rank-2)*(*_HWeight)[rank-2] + rank*(*_HWeight)[rank-1];
						_congruency[1] %= 4;
						break;
					case 'E':
						//_congruency = new RVector<int>(1);
						if(rank == 6) {
							_congruency[0] = (int)((*_HWeight)[0] - (*_HWeight)[1] + (*_HWeight)[3] - (*_HWeight)[4]) % 3;
						} else if(Group().rank() == 7) {
							_congruency[0] = (int)((*_HWeight)[3] + (*_HWeight)[5] + (*_HWeight)[6]) % 2;
						} else {
							_congruency[0] = 0;
						}
						break;
					case 'F':
						//_congruency = new RVector<int>(1);
						_congruency[0] = 0;
						break;
					case 'G':
						//_congruency = new RVector<int>(1);
						_congruency[0] = 0;
						break;
					case 'U':
						//_congruency = new RVector<int>(1);
						_congruency[0] = 0;
						break;
				}
			
				if(_conjugate) {
					_label.append("*");
				}
				
				if(_Group->abelian()) {
					_Casimir = (*_HWeight)[0]*(*_HWeight)[0];
					_DynkinIndex = _Casimir;
				} else {
					_Casimir = 0;
					for(int i=0; i<rank; i++) {
						for(int j=0; j<rank; j++) {
							_Casimir += 0.5*(*_HWeight)[i]*_Group->G()[i][j]*((*_HWeight)[j]+2);
						}
					}
					_DynkinIndex = _Casimir*_dim/_Group->dim();
				}
				
				//std::cout << "Rep = " << *this << ", Casimir = " << _Casimir << ", Dynkin = " << _DynkinIndex << std::endl;
				
				// Store the info in the database
				database_emplace(id(),*this);
			}
			
		} catch (...) {
			throw;
		}
	}

	/* Identifier of the irrep */
	std::string Irrep::id() const {
		std::stringstream s;
		s << _HWeight->id();
		return s.str();
	}

	/* Returns the Lie Group */
	SimpleGroup &Irrep::Group() const {
	return *_Group;
	}

	/* Returns dim */
	int Irrep::dim() const {
	return _dim;
	}

	/* Returns real */
	bool Irrep::real() const {
		return _real;
	}

	/* Returns conjugate */
	int Irrep::conjugate() const {
	return _conjugate;
	}

	/* Returns the congruency number */
	RVector<int> Irrep::congruency() const {
		return _congruency;
	}

	/* Returns label */
	std::string Irrep::label() const {
	return _label;
	}

	/* Sets the label */
	void Irrep::setLabel(std::string label) {
		try {
			_label = label;
			
			database_emplace(id(), *this);
			
			// Output the info
			/*std::ostringstream OutputDirectory;
			OutputDirectory << "./out/" << _Group->id() << "/reps";

			mkdir(OutputDirectory.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

			std::ostringstream OutputFileName;
			OutputFileName << OutputDirectory.str() << "/" << id() << ".out";

			std::ofstream OutputFile;
			OutputFile.open(OutputFileName.str().c_str());
			OutputFile << json().write_formatted() << std::endl;
			OutputFile.close();*/

		} catch (...) {
			throw;
		}
	}

	/* Returns HWeight */
	Weight Irrep::HWeight() const {
		return *_HWeight;
	}

	/* Returns whether the Irrep has calculated its weights */
	bool Irrep::hasWeights() const {
		return _hasWeights;
	}
	
	/* Sets the weights */
	void Irrep::setWeights(const List<Weight> &Weights) {
		if(Weights.nterms()) _hasWeights = true;
		_Weights = Weights;
	}
	
	/* Calculates the weights diagram */
	List<Weight> &Irrep::Weights(std::string _basis) {

		try {
			
			std::string basis = _basis;
			
			std::string::iterator init = basis.begin();
			std::string::iterator end = basis.end();

			while (init != end) {
				*init = std::toupper((unsigned char)*init);
				++init;
			}
			if(basis.compare("DYNKIN") and basis.compare("DUAL")) {
				throw "Irrep::Weights::Basis must be Dynkin or Dual";
			}

			int rank = this->Group().rank();

			// If the weights have been calculated already take them
			if(_Weights.nterms()) {
				if(basis == "DUAL") {
					for(List<Weight>::iterator it_Weights = _Weights.begin(); it_Weights != _Weights.end(); it_Weights ++)
						_DualWeights.AddTerm(it_Weights->Dual());
					return _DualWeights;
				}
				return _Weights;
			}

			// Look for the weights in the database			
			if(database_check(id(), "Weights") and DataBase.at(id()).hasWeights()) {
				_Weights = DataBase.at(id()).Weights();
				if(_Weights.nterms()) _hasWeights = true;
				if(basis == "DUAL") {
					for(List<Weight>::iterator it_Weights = _Weights.begin(); it_Weights != _Weights.end(); it_Weights ++)
						_DualWeights.AddTerm(it_Weights->Dual());
					return _DualWeights;
				}
				return _Weights;
			}
		
			//std::cout << "Calculating weights of " << *this << std::endl;
			// Reserve enough space for all the weights
			_Weights.reserve(dim());
			_Weights.AddTerm(*_HWeight);
			
			if(this->Group().abelian()) {
				return _Weights;
			}
			
			int n = 0;
			
			for(List<Weight>::iterator it_Weights = _Weights.begin(); it_Weights != _Weights.end(); ++ it_Weights) {
				for(int j=0; j<rank; j++) {
					n = round((*it_Weights)[j]);
					if(n>0){
						for(int l=1; l<=n; l++) {
							Weight w = *it_Weights - _Group->SRoot(j)*(double)l;
							w.setLevel(it_Weights->level()+l);
							if(_Weights.Index(w) < 0) {
								_Weights.AddTerm(w);
							}
						}
					}
				}
			}
			
			_Weights.Order("ASC");
			//std::cout << _Weights << std::endl;

			// If there are less terms than the dimension of the rep calculate multiplicities
			if(_Weights.nterms() < _dim) {
				List<Root> PRoots = _Group->PRoots();
				int i = 0;
				for(List<Weight>::iterator it_Weights = _Weights.begin(); it_Weights != _Weights.end() and _Weights.nterms() < _dim; it_Weights++) {
					Weight w = *it_Weights;
					double multiplicity = 0;
					for(List<Root>::iterator it_PRoots = PRoots.begin(); it_PRoots != PRoots.end(); it_PRoots++) {
						Root alpha = *it_PRoots;
						int k = 1;
						int index = 0;
						do {
							Weight v = w + alpha*k;
							//std::cout << v << std::endl;
							if((index=_Weights.Index(v)) >=0) {
								//std::cout << "index = " << index << std::endl;
								//std::cout << v*alpha << std::endl;
								multiplicity += 2.0*_Weights.GetObject(index).multiplicity() * v*alpha;
							}
							k++;
						} while(index >=0);
						//std::cout << "multiplicity = " << multiplicity << std::endl;
					}
					double norm = ((*_HWeight)++)*((*_HWeight)++) - (w++)*(w++);
					//std::cout << "norm = " << norm << std::endl;
					if(norm > 0) {
						multiplicity /= norm;
					} else {
						multiplicity = 1;
					}
					//std::cout << "final multiplicity = " << multiplicity << std::endl;
					_Weights.DeleteTerm(i);
					w.setMultiplicity((int)round(multiplicity));
					for(int j=0; j<w.multiplicity(); j++) {
						_Weights.InsertTerm(i,w);
						//std::cout << _Weights.GetObject(i) << std::endl;
						i++;
					}
					it_Weights += w.multiplicity()-1;
					//std::cout << "multilpicity of " << _Weights.GetObject(i-w.multiplicity()+1) << " is " << _Weights.GetObject(i-w.multiplicity()+1).multiplicity() << std::endl;
				}
			}

			if(_Weights.nterms() < _dim) {
				throw "Irrep::Weights::Error in the weight calculation";
			}
			
			// Calculate also the dual weights
			for(List<Weight>::iterator it_Weights = _Weights.begin(); it_Weights != _Weights.end(); it_Weights++)
				_DualWeights.AddTerm(it_Weights->Dual());
			
			// Set the hasWeights flag to true
			if(_Weights.nterms()) _hasWeights = true;
			
			// If there is an entry in the database delete it and dump this
			database_emplace(id(), *this);
			
			// Output the info
			/*std::ostringstream OutputDirectory;
			OutputDirectory << "./out/" << Group().type() << Group().rank() << "/reps";

			mkdir(OutputDirectory.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			std::ostringstream OutputFileName;
			OutputFileName << OutputDirectory.str() << "/" << id() << ".out";

			std::ofstream OutputFile;
			OutputFile.open(OutputFileName.str().c_str());
			OutputFile << json().write_formatted() << std::endl;
			OutputFile.close();
			*/
			
			// If the basis demanded was the dual return DualWeights, otherwise return Weights
			if(basis == "DUAL") {
				return _DualWeights;
			}
			
			return _Weights;

		} catch (...) {
			throw;
		}
	}

	/* Returns the weights variable */
	List<Weight> Irrep::WeightsConst(std::string _basis) const {
		
		
		std::string basis = _basis;
		
		std::string::iterator init = basis.begin();
		std::string::iterator end = basis.end();

		while (init != end) {
			*init = std::toupper((unsigned char)*init);
			++init;
		}
		if(basis.compare("DYNKIN") and basis.compare("DUAL")) {
			throw "Irrep::WeightsConst::Basis must be Dynkin or Dual";
		}
		
		if(basis == "DYNKIN") return _Weights;
		else return _DualWeights;
	}
		
	/* Direct product of representations */
	Sum<Irrep> Irrep::operator*(Irrep R) {

		try {
			Sum<Irrep> Reps;
			
			if(ProductDataBase.find(std::pair<std::string,std::string>(id(),R.id())) != ProductDataBase.end())
			{
				Reps = ProductDataBase.at(std::pair<std::string,std::string>(id(),R.id()));
				return Reps;
			}
			
			int totaldim = this->dim()*R.dim();
			int nreps = 0;

			List<Weight> R1Weights = this->Weights();
			List<Weight> R2Weights = R.Weights();
			List<Weight> W;

			for(int i=0;i<totaldim;i++) {
		
				if(_dim <= R.dim()) {
					Weight w = R1Weights.GetObject(i/R.dim()) + R2Weights.GetObject(i%R.dim());
					W.AddTerm(w);
				} else {
					Weight w = R2Weights.GetObject(i/this->dim()) + R1Weights.GetObject(i%this->dim());
					W.AddTerm(w);
				}
			}

			//std::cout << W << std::endl;

			int n;

			do {
				Weight HW(this->Group(),this->Group().rank());
				int count = 0;
				do {
					HW = W.GetObject(count);
					count++;
				} while(!HW.positive() and HW != 0 and count < W.nterms());

				HW.setMultiplicity(1);
				Irrep Rep(this->Group(), HW);

				List<Weight> RepWeights = Rep.Weights();

				for(int i=0; i<Rep.dim(); i++) {
					n = W.Index(RepWeights.GetObject(i));
					//std::cout << " n = " << n << std::endl;
					//std::cout << "RepWeights(" << i << ") = " << RepWeights.GetObject(i) << std::endl;
					//std::cout << "W(n) = " << W.GetObject(n) << std::endl;
					if(n >= 0) {
						W.DeleteTerm(n);
					}
				}	
				totaldim -= Rep.dim();

				//std::cout << W << std::endl;

				//std::cout << "Dimension = " << Rep.dim() << std::endl;
				Reps.AddTerm(Rep);
				nreps++;
				//std::cout << "nreps = " << nreps << std::endl;
				//std::cout << "totaldim = " << totaldim << std::endl;
				
			} while(totaldim>0 and W.nterms() > 0);

			if(!W.nterms() and totaldim) {
				throw "Irrep::operator*::Error calculating the product";
			}

			Reps.Order();

			if(ProductDataBase.find(std::pair<std::string,std::string>(id(),R.id())) != ProductDataBase.end())
				ProductDataBase.erase(std::pair<std::string,std::string>(id(),R.id()));
			ProductDataBase.emplace(std::pair<std::string,std::string>(id(),R.id()), Reps);
			
			return Reps;
			
		} catch (...) {
			throw;
		}

	}

	/* Direct product of a list of representations */
	Sum<Irrep> Irrep::Product(List<Irrep> &ListofReps) {
		
		try {

			Sum<Irrep> Reps;
			Sum<Irrep> Reps2;
			
			if(ListofReps.nterms() < 2) return Sum<Irrep>(ListofReps.GetObject(0));
			if(ListofReps.nterms() == 2) return ListofReps.GetObject(0)*ListofReps.GetObject(1);
		
			List<Irrep> ListofReps2(ListofReps);
			ListofReps2.DeleteTerm(0);
			Irrep Rep = ListofReps.GetObject(0);
			
			Reps2 = Irrep::Product(ListofReps2);
			for(Sum<Irrep>::iterator it_Reps2 = Reps2.begin(); it_Reps2 != Reps2.end(); it_Reps2++)
				Reps.AppendList(Rep*(*it_Reps2));
			
			Reps.Order();
			
			return Reps;

		} catch (...) {
			throw;
		}
	}	

	/* Returns whether the irrep is the conjugate of a given irrep */
	bool Irrep::isConjugateOf(Irrep R) {
		
		try {
			if(_real or _dim != R.dim()) {
				return false;
			} else {
		
				switch(this->Group().type()) {

					case 'A':
						for(int i=0; i<this->Group().rank(); i++) {
							if(this->HWeight()[i] != R.HWeight()[this->Group().rank()-i-1]) {
								return false;
							}
						}
						return true;
						break;

					case 'D':
						if(this->Group().rank()%2) {
							if(this->HWeight()[this->Group().rank()-1] == R.HWeight()[this->Group().rank()-2]) {
								return true;
							}
						}
						return false;
						break;

					case 'E':
						if(this->Group().rank() != 6) {
							return false;
						} else {;
							for(int i=0; i<this->Group().rank()-1; i++) {
								if(this->HWeight()[i] != R.HWeight()[this->Group().rank()-i-2]) {
									return false;
								}
							}
							return true;
						}
						break;
				}
			}
			return false;
		
		} catch (...) {
			throw;
		}
	}

	/* Returns the first Casimir of the representation */
	double Irrep::Casimir() const {
		return _Casimir;
	}

	/* Returns the Dynkin index of a representation */
	double Irrep::DynkinIndex() const {
		return _DynkinIndex;
	}

	/* Check whether the rep is a singlet or not */
	bool Irrep::isSinglet() const {

		if(dim() == 1) {
			if(Group().abelian() and _HWeight[0] != 0) return false;

			return true;
		}
		
		return false;
	}

	/* Projects the weights of a irrep into the weights of irreps of a subgroup */
	List<Weight> Irrep::Project(SubGroup Subgroup) {
		
		try {
			List<Weight> Weights = this->Weights();
			//std::cout << Weights.nterms() << std::endl;
			List<Weight> ProjectedWeights;
			//std::cout << Subgroup.Projection() << std::endl;
			for(int i=0; i<Weights.nterms(); i++) {
				Weight weight = Weight(Subgroup, Subgroup.Projection()*Weights.GetObject(i));
				weight.setPositive(true);
				for(int j=0; j<weight.cols()-Subgroup.nabelians(); j++) {
					if(weight[j] < 0) {
						weight.setPositive(false);
					}
				}
				ProjectedWeights.AddTerm(weight);
			}
		
			return ProjectedWeights;
		} catch (...) {
			throw;
		}
	}

	/* Obtains the decomposition into irreps of a subgroup */
	Sum<Rrep> Irrep::Decompose(SubGroup Subgroup) {
		
		try {
			
			//std::cout << "decomposing " << *this << " into " << Subgroup << std::endl;
			Sum<Rrep> Reps;
			
			if(DecomposeDataBase.find(std::pair<std::string,std::string>(id(),Subgroup.id())) != DecomposeDataBase.end())
			{
				Reps = DecomposeDataBase.at(std::pair<std::string,std::string>(id(),Subgroup.id()));
				return Reps;
			}
		
			if(Subgroup.isSubgroupOf(this->Group())) {
				List<Weight> ProjectedWeights = this->Project(Subgroup);
				//std::cout << "Projected Weights = "<< std::endl << ProjectedWeights.Print() << std::endl;
				
				Subgroup.Order();
				do {
					//std::cout << "Projected Weights = "<< std::endl << ProjectedWeights.Print() << std::endl;
					Weight HWeight = ProjectedWeights.GetObject(0);
					int maximum_sum_of_values = 0;
					for(int i=0; i<ProjectedWeights.nterms(); i++) {
						int sum_of_values = 0;
						int group = 0;
						int acc_rank = Subgroup.GetObject(0).rank();
						Weight weight = ProjectedWeights.GetObject(i);
						//std::cout << "weight = " << weight << " is positive? " << weight.positive() << std::endl;
						for(int j=0; j<Subgroup.rank(); j++) {
							if(j >= acc_rank) {
								group++;
								acc_rank += Subgroup.GetObject(group).rank();
							}
							if(!Subgroup.GetObject(group).abelian()) {
								if(weight[j] >= 0) {
									sum_of_values += (int)weight[j];
								}
							}
						}
						if(weight.positive()) {
							if(sum_of_values > maximum_sum_of_values) {
								maximum_sum_of_values = sum_of_values;
								HWeight = weight;
							}
						}
					}
					//std::cout << Subgroup << std::endl;
					//std::cout << HWeight << std::endl;
					Rrep Rep(Subgroup, HWeight);
					//std::cout << "The rep is " << Rep << ", " << Rep.id() << std::endl;
					//std::cout << Rep.json().write_formatted() << std::endl;
					List<Weight> SubWeights = Rep.Weights();
					//std::cout << SubWeights << std::endl;
					for(int i=0; i<SubWeights.nterms(); i++) {
						//std::cout << "i = " << i << std::endl;
						int n = ProjectedWeights.Index(SubWeights.GetObject(i));
						//std::cout << "n = " << n << std::endl;
						if(n >= 0) {
							ProjectedWeights.DeleteTerm(n);
						}
					}
					Reps.AddTerm(Rep);
				
				} while(ProjectedWeights.nterms()>0);
			
			} else {
				throw "Irrep:Decompose:Not a subgroup";
			}
			//std::cout << Reps << std::endl;
			int dim=0;
			for(int i=0; i<Reps.nterms(); i++) {
				dim += Reps.GetObject(i).dim();
				//std::cout << Reps.GetObject(i) << std::endl;
				//std::cout << dim << std::endl;
			}
			if(dim != this->dim()) {
				throw "Irrep::Decompose::Dimension of the result doesn't match the dimension of the rep";
			}
			
			if(DecomposeDataBase.find(std::pair<std::string,std::string>(id(),Subgroup.id())) != DecomposeDataBase.end())
				DecomposeDataBase.erase(std::pair<std::string,std::string>(id(),Subgroup.id()));
			DecomposeDataBase.emplace(std::pair<std::string,std::string>(id(),Subgroup.id()), Reps);
			
			return Reps;

		} catch (...) {
			throw;
		}

	}

	/* Overloaded == operator */
	bool Irrep::operator==(const Irrep R) const {
		if(this->Group() == R.Group() && this->HWeight() == R.HWeight()) {
			return true;
		} else {
			return false;
		}
	}

	/* Overloaded != operator */
	bool Irrep::operator!=(const Irrep R) const {
		if(*this == R) {
			return false;
		} else {
			return true;
		}
	}

	/* Overloaded > operator */
	bool Irrep::operator>(const Irrep R) const {
		try {
			if(this->dim() > R.dim()) {
				return true;
			} else if(this->dim() == R.dim() && this->DynkinIndex() > R.DynkinIndex()) {
				return true;
			}
			return false;
		} catch (...) {
			throw;
		}
	}

	/* Overloaded < operator */
	bool Irrep::operator<(const Irrep R) const {
		try {
			if(this->dim() < R.dim()) {
				return true;
			} else if(this->dim() == R.dim() && this->DynkinIndex() < R.DynkinIndex()) {
				return true;
			}
			return false;
		} catch (...) {
			throw;
		}
	}

	/* Prints the representation */
	std::string Irrep::Print() const {
	return this->label();
	}

	/* Prints the representation for table manipulation*/
	std::string Irrep::Table() const {
		if(this->label()[this->label().length()-1] == '*' || this->label()[this->label().length()-1] == '\'') {
			//std::string label = this->label().erase(this->label().length()-1,1);
			//label.insert(0,"Superscript[\"");
			//label.append("\",\"*\"]");
			std::string label = this->label();
			label.insert(0,"ToString[\"");
			label.append("\", StandardForm]");
			return label;
		} else {
			return this->label();
		}
	}

	/* Returns the json string */
	JSONNode Irrep::json(std::string name) const {
		
		if(name == "id") {
			return JSONNode("", id());
		}
		
		JSONNode json;
			
		json.push_back(JSONNode("id", id()));
		json.push_back(JSONNode("Group", _Group->id()));
		json.push_back(_HWeight->json("HWeight"));
		json.push_back(JSONNode("dim", dim()));
		json.push_back(JSONNode("real", real()));
		json.push_back(JSONNode("label", label()));
		json.push_back(JSONNode("Casimir", Casimir()));
		json.push_back(JSONNode("DynkinIndex",DynkinIndex()));
		json.push_back(JSONNode("conjugate", conjugate()));
		json.push_back(_congruency.json("congruency"));
		json.push_back(JSONNode("hasWeights", hasWeights()));
		json.push_back(_Weights.json("Weights"));
		
		return json;
	}

	/* Parses a json object into the attributes of the class */
	void Irrep::ParseJSON(const JSONNode &n, std::string what) {
		JSONNode::const_iterator i = n.begin();
		while (i != n.end()){
	
			// get the node name and value as a string
			std::string node_name = i -> name();
			
			// find out where to store the values
			if(node_name == "Group") {
				_Group = new SimpleGroup(i->as_string());
			} else if(node_name =="HWeight") {
				_HWeight = new Weight(*_Group, _Group->rank());
				_HWeight->ParseJSON(*i);
			} else if(node_name == "dim") {
				_dim = i->as_int();
			} else if(node_name == "real") {
				_real = i->as_bool();
			} else if(node_name == "label") {
				_label = i->as_string();
			} else if(node_name == "Casimir") {
				_Casimir = i->as_float();
			} else if(node_name == "DynkinIndex") {
				_DynkinIndex = i->as_float();
			} else if(node_name == "conjugate") {
				_conjugate = i->as_bool();
			} else if(node_name == "congruency") {
				_congruency.ParseJSON(*i);
			} else if(node_name == "hasWeights" and what == "Weights") {
				_hasWeights = i->as_bool();
			} else if(node_name == "Weights" and what == "Weights") {
				//std::cout << i->write_formatted() << std::endl;
				_Weights.ParseJSON(*i);
			}
	
			//increment the iterator
			++i;
		}
	}

	/* Overloaded << operator with irreps on the right */
	std::ostream &operator<<(std::ostream &stream, const Irrep &i) {
		stream << i.label();
		return stream;
	}

}
