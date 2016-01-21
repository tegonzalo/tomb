/*
 * rrep.cc
 * Created by T.Gonzalo on 13/08/2013.
 * Last modified on 10/11/2015
 */

#include "headers.h"

 /**************************************************/
/***************Class Rrep methods *****************/
/**************************************************/

namespace Tomb
{
	// Definition of static variables and helper functions
	
	std::map<std::string, Rrep> Rrep::DataBase;
	std::map<std::string, JSONNode> Rrep::JSONDataBase;
	std::map<std::pair<std::string, std::string>, Sum<Rrep> > Rrep::DecomposeDataBase;
	std::map<std::pair<std::string, std::string>, Sum<Rrep> > Rrep::ProductDataBase;
	
	bool Rrep::database_check(std::string id, std::string what) {
		if(DataBase.find(id) != DataBase.end()) {
			if (what == "")
				return true;
			if(what == "Weights" and DataBase.at(id).hasWeights())
				return true;
		}
		if(JSONDataBase.find(id) != JSONDataBase.end()) {
			Rrep R(JSONDataBase.at(id));
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
	
	void Rrep::database_emplace(std::string id, Rrep R) {
		if(database_check(id))
			DataBase.erase(id);
		DataBase.emplace(id, R);
	}
	
	// Member functions
	
	/* Constructor 0 */
	Rrep::Rrep() {
	}

	/* Constructor 1*/
	Rrep::Rrep(const LieGroup &Group, const Weight &HWeight) : List<Irrep>() {

		try {
			if(Group.rank() != HWeight.cols()) {
				throw "Rrep::Rrep::Dimension of the weight doesn\'t match rank of the group";
			}

			_Group = new LieGroup(Group);
			_HWeight = new Weight(HWeight);
			
			init();
			
		} catch (...) {
			throw;
		}

	}

	/* Constructor 2*/
	Rrep::Rrep(const SubGroup &Group, const Weight &HWeight) : List<Irrep>() {

		try {
			if(Group.rank() != HWeight.cols()) {
				throw "Rrep::Rrep::Dimension of weight doesn\'t match rank of group";
			}

			_Group = new LieGroup(Group);
			_HWeight = new Weight(HWeight);
			
			init();
			
		} catch (...) {
			throw;
		}

	}

	/* Constructor 3 */
	Rrep::Rrep(const Irrep &Rep) : List<Irrep>(Rep) {
		try {
			_Group = new LieGroup(Rep.Group());
			_HWeight = new Weight(Rep.HWeight());
			_dim = Rep.dim();
			_real = Rep.real();
			_nirreps = 1;
			_label = Rep.label();
			_Casimir.AddTerm(Rep.Casimir());
			_DynkinIndex.AddTerm(Rep.DynkinIndex());
			_hasWeights = Rep.hasWeights();
			if(_hasWeights) _Weights = Rep.WeightsConst();
						
		} catch (...) {
			throw;
		}
	}

	/* Constructor 4, with strings */
	Rrep::Rrep(const std::string id) {
		
		try {
			
			_HWeight = new Weight(id);
			_Group = new LieGroup(HWeight().Group());

			init();

		} catch (...) {
			throw;
		}	
	}

	/* Constructor 5, with JSON Nodes */
	Rrep::Rrep(const JSONNode &n) {
		if(n.as_string() != "") {
			_HWeight = new Weight(n.as_string());
			_Group = new LieGroup(HWeight().Group());
			init();
		} else {
			ParseJSON(n);
		}
	}

	/* Copy constructor */
	Rrep::Rrep(const Rrep &Rep) : List<Irrep>(Rep) {
		try {
			_Group = new LieGroup(Rep.Group());
			_HWeight = new Weight(Rep.HWeight());
			_dim = Rep.dim();
			_real = Rep.real();
			_nirreps = Rep.nirreps();
			_label = Rep.label();
			_Casimir = Rep.Casimir();
			_DynkinIndex = Rep.DynkinIndex();
			_hasWeights = Rep.hasWeights();
			if(_hasWeights) _Weights = Rep.WeightsConst();
		} catch (...) {
			throw;
		}
	}

	/* Move constructor */
	Rrep::Rrep(Rrep &&Rep) : 
		List<Irrep>(std::move(Rep)),
		_Group(std::move(Rep._Group)),
		_HWeight(std::move(Rep._HWeight)),
		_dim(std::move(Rep._dim)),
		_real(std::move(Rep._real)),
		_nirreps(std::move(Rep._nirreps)),
		_label(std::move(Rep._label)),
		_Casimir(std::move(Rep._Casimir)),
		_DynkinIndex(std::move(Rep._DynkinIndex)),
		_hasWeights(std::move(Rep._hasWeights)),
		_Weights(std::move(Rep._Weights))
	{
		try {		
			Rep._Group = nullptr;
			Rep._HWeight = nullptr;
			Rep._dim = 0;
			Rep._real = false;
			Rep._nirreps = 0;
			Rep._label = "";
			Rep._Casimir.Clear();
			Rep._DynkinIndex.Clear();
			Rep._hasWeights = false;
			Rep._Weights.Clear();
			
		} catch (...) {
			throw;
		}
	}

	/* Destructor */
	Rrep::~Rrep() {
		if(_Group != NULL) delete _Group;
		if(_HWeight != NULL) delete _HWeight;
	}

	/* Overloaded = operator */
	Rrep &Rrep::operator=(const Rrep &Rep) {
		try {
			if(this == &Rep) return *this;
			List<Irrep>::operator=(Rep);
			_Group = new LieGroup(Rep.Group());
			_HWeight = new Weight(Rep.HWeight());
			_dim = Rep.dim();
			_real = Rep.real();
			_nirreps = Rep.nirreps();
			_label = Rep.label();
			_Casimir = Rep.Casimir();
			_DynkinIndex = Rep.DynkinIndex();
			_hasWeights = Rep.hasWeights();
			if(_hasWeights) _Weights = Rep.WeightsConst();
			return *this;
		} catch (...) {
			throw;
		}
	}

	/* Move = operator */
	Rrep &Rrep::operator=(Rrep &&Rep) {
		try {
			if(this == &Rep) return *this;
			List<Irrep>::operator=(std::move(Rep));
			_Group = std::move(Rep._Group);
			_HWeight = std::move(Rep._HWeight);
			_dim = std::move(Rep._dim);
			_real = std::move(Rep._real);
			_nirreps = std::move(Rep._nirreps);
			_label = std::move(Rep._label);
			_Casimir = std::move(Rep._Casimir);
			_DynkinIndex = std::move(Rep._DynkinIndex);
			_hasWeights = std::move(Rep._hasWeights);
			_Weights = std::move(Rep._Weights);
			Rep._Group = nullptr;
			Rep._HWeight = nullptr;
			Rep._dim = 0;
			Rep._real = false;
			Rep._nirreps = 0;
			Rep._label = "";
			Rep._Casimir.Clear();
			Rep._DynkinIndex.Clear();
			Rep._hasWeights = false;
			Rep._Weights.Clear();
			return *this;
		} catch (...) {
			throw;
		}
	}

	/* Initialises some internal variables */
	void Rrep::init() {
		
		try {
			
			if(_Group == NULL or _HWeight == NULL) {
				throw "Rrep::init::Not enough information to initialise the variables";
			}
			
			if(database_check(id())) {
				//std::cout << "in the db " << DataBase.at(id()).nterms() << std::endl;
				*this = DataBase.at(id());
			} else {
				
				_dim = 1;
				_nirreps = 0;
				int accumulated_rank = 0;
				for(int i=0; i<Group().ngroups(); i++) {
					RVector<double> R = HWeight().ExtractMatrix(0,0,accumulated_rank,accumulated_rank + Group().GetObject(i).rank()-1).Row(0);
					Weight HW(Group().GetObject(i), R);
					Irrep aRep(Group().GetObject(i),HW); 
					this->AddTerm(aRep);
					_dim *= aRep.dim();
					_nirreps++;
					accumulated_rank += Group().GetObject(i).rank();
				}
				//std::cout << "dim = " << _dim << std::endl;
				if(nirreps() > 1) {
					std::stringstream ss;
					ss << *this;
					_label = ss.str();
				} else {
					_label = GetObject(0).label();
				}
				
				_real = true;
				for(int i=0; i<nirreps(); i++) {
					if(!GetObject(i).real()) _real = false;
					
					_Casimir.AddTerm(GetObject(i).Casimir());
					//std::cout << _Casimir << std::endl;
					_DynkinIndex.AddTerm(GetObject(i).DynkinIndex()*dim()/GetObject(i).dim());
					//std::cout << _DynkinIndex << std::endl;
				}
				
				// Store the info in the database
				database_emplace(id(),*this);

			}
		
		} catch(...) {
			throw;
		}	
	}

	/* Identifier of the Rrep */
	std::string Rrep::id() const {
		std::stringstream s;
		s << HWeight().id();
		return s.str();
	}

	/* Returns the group */
	LieGroup &Rrep::Group() const {
	return *_Group;
	}

	/* Returns the dimension */
	int Rrep::dim() const {
	return _dim;
	}

	/* Returns if the rep is real */
	bool Rrep::real() const {
		return _real;
	}

	/* Returns the number of irreps */
	int Rrep::nirreps() const {
	return _nirreps;
	}

	/* Returns the label */
	std::string Rrep::label() const {
		return _label;
	}

	/* Returns the Casimir of the representation */
	List<double> Rrep::Casimir() const {
		return _Casimir;
	}
	
	/* Returns whether the Rrep has calculated its weights */
	bool Rrep::hasWeights() const {
		return _hasWeights;
	}

	/* Returns the Dynkin Index of the representation */
	List<double> Rrep::DynkinIndex() const {
		return _DynkinIndex;
	}

	/* Returns the Height Weight */
	Weight Rrep::HWeight() const {
		return *_HWeight;
	}

	/* Sets the weights */
	void Rrep::setWeights(const List<Weight> &Weights) {
		if(Weights.nterms()) _hasWeights = true;
		_Weights = Weights;
	}
	
	/* Calculates the weights of the representation */
	List<Weight> &Rrep::Weights() {
		
		try {
			
			// If the weights have been calculated already take them
			if(_Weights.nterms()) 
				return _Weights;
			
			if(database_check(id(), "Weights") and DataBase.at(id()).hasWeights())
			{
				_Weights = DataBase.at(id()).Weights();
				if(_Weights.nterms()) _hasWeights = true;
				return _Weights;
			}
			
			//std::cout << "Calculating weights of " << *this << std::endl;
			List< List<Weight> > ListofWeights;
			//std::cout << _nirreps << std::endl;
			for(int i=0; i<this->nirreps(); i++) {
				ListofWeights.AddTerm(this->GetObject(i).Weights());
			}
			//std::cout << ListofWeights << std::endl;
			
			int totalterms = 1;
			for(int i=0; i<ListofWeights.nterms(); i++) {
				totalterms *= ListofWeights.GetObject(i).nterms();
			}
			
			// Reserve enough space for the weights
			_Weights.reserve(dim());
			
			for(int i=0; i<totalterms; i++) {
				Weight weight;
				//std::cout << "i = " << i << std::endl;
				int terms = 1;
				for(int j=0; j<ListofWeights.nterms(); j++) {
					//std::cout << "j = " << j << std::endl;
					int thisterms = ListofWeights.GetObject(j).nterms();
					terms *= thisterms;
					int index = (i*terms/totalterms)%thisterms;
					if(j==0) {
						weight = Weight(this->Group().GetObject(0),ListofWeights.GetObject(0).GetObject(index));
					} else {	
						//std::cout << weight << std::endl;
						weight.Append(ListofWeights.GetObject(j).GetObject(index));
						//std::cout << weight << std::endl;
					}
				}
				_Weights.AddTerm(weight);
				//std::cout << Weights << std::endl;
			}
			
			//std::cout << _Weights << std::endl;
			// Set the hasWeights flag to true
			if(_Weights.nterms()) _hasWeights = true;
			
			// If there is an entry in the database delete it and dump this
			database_emplace(id(), *this);
			
			return _Weights;

		} catch (...) {
			throw;
		}
	}

	/* Returns the variable Weights */
	List<Weight> Rrep::WeightsConst() const {
		return _Weights;
	}
	
	/* Adds a irrep of another group to the rep */
	Rrep Rrep::AddIrrep(Irrep Rep) {
		
		try {
			this->AddTerm(Rep);
			_Group->AddTerm(Rep.Group());
		
			Weight w = *_HWeight;
			_HWeight = new Weight(*_Group, w.Append(Rep.HWeight()));
		
			_dim *= Rep.dim();
			if(!Rep.real()) _real = false;
			_nirreps++;
			
			std::stringstream ss;
			ss << *this;
			_label = ss.str();
			
			_Casimir.AddTerm(Rep.Casimir());
			_DynkinIndex.AddTerm(Rep.DynkinIndex()*dim()/Rep.dim());
			
			return *this;
		} catch (...) {
			throw;
		}
	}

	/* Adds a rep of another lie group to the rep */
	Rrep Rrep::AddRrep(Rrep Rep) {
		
		try {
			this->AppendList(Rep);
			_Group->AppendList(Rep.Group());
		
			Weight w = *_HWeight;
			_HWeight = new Weight(*_Group, w.Append(Rep.HWeight()));
		
			_dim *= Rep.dim();
			if(!Rep.real()) _real = false;
			_nirreps += Rep.nirreps();
			
			std::stringstream ss;
			ss << *this;
			_label = ss.str();
			
			_Casimir.AppendList(Rep.Casimir());
			_DynkinIndex.AppendList(Rep.DynkinIndex());
			
			return *this;
		} catch (...) {
			throw;
		}
	}

	/* Checks whether the rep is a singlet */
	bool Rrep::isSinglet(int which) const {
		if(which == -1)
		{
			if(dim() == 1)
			{
				for(int i=0; i<Group().nabelians(); i++)
					if((*_HWeight)[-i-1] != 0)
						return false;
				return true;
			}
		} else
			return GetObject(which).isSinglet();
		return false;
	}

	/* Projects the weights of a rrep into the weights of rreps of a subgroup */
	List<Weight> Rrep::Project(SubGroup Subgroup)
	{
		
		try {
			List<Weight> Weights = this->Weights();
			//std::cout << Weights << std::endl;
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

	/* Obtains the decomposition into rreps of a subgroup */
	Sum<Rrep> Rrep::Decompose(SubGroup &Subgroup)
	{

		try {
			
			//std::cout << "decomposing " << *this << " into " << Subgroup << std::endl;
			//std::cout << "Weights = " << Weights()  << std::endl;
			//std::cout << "Projection matrix" << std::endl << Subgroup.Projection() << std::endl;
			Sum<Rrep> Reps;
			
			if(DecomposeDataBase.find(std::pair<std::string,std::string>(id(),Subgroup.id())) != DecomposeDataBase.end())
			{
				Reps = DecomposeDataBase.at(std::pair<std::string,std::string>(id(),Subgroup.id()));
				return Reps;
			}
					
			if(Subgroup.isSubgroupOf(this->Group())) {
				List<Weight> ProjectedWeights = this->Project(Subgroup);
				Subgroup.Order();
				do {
					//std::cout << "Projected Weights = "<< std::endl << ProjectedWeights.Print() << std::endl;
					
					Weight HWeight = ProjectedWeights.GetObject(0);
					//int maximum_sum_of_values = 0;
					int highest_dim = 0;
					for(List<Weight>::iterator it_ProjectedWeights = ProjectedWeights.begin(); it_ProjectedWeights != ProjectedWeights.end(); it_ProjectedWeights++) {
						//int sum_of_values = 0;
						int group = 0;
						int acc_rank = Subgroup.GetObject(0).rank();
						Weight weight = *it_ProjectedWeights;
						for(int j=0; j<Subgroup.rank(); j++) {
							if(j >= acc_rank) {
								group++;
								acc_rank += Subgroup.GetObject(group).rank();
							}
						}
						if(weight.positive()) {
							Rrep Rep(Subgroup, weight);
							if(Rep.dim() > highest_dim) {
								highest_dim = Rep.dim();
								HWeight = weight;
							}
						}
					}
					//std::cout << Subgroup << std::endl;
					//std::cout << HWeight << std::endl;
					Rrep Rep(Subgroup, HWeight);
					//std::cout << "Rep = " << Rep << std::endl;
					List<Weight> SubWeights = Rep.Weights();
					//std::cout << SubWeights << std::endl;
					for(List<Weight>::iterator it_SubWeights = SubWeights.begin(); it_SubWeights != SubWeights.end(); it_SubWeights++) {
						//std::cout << *it_SubWeights << std::endl;	
						int n = ProjectedWeights.Index(*it_SubWeights);
						if(n >= 0)
							ProjectedWeights.DeleteTerm(n);
					}
					Reps.AddTerm(Rep);
				
					//std::cout << "terms = " << ProjectedWeights.nterms() << std::endl;
					
				} while(ProjectedWeights.nterms()>0);
			
			} else {
				throw "Rrep::Decompose::Not a subgroup";
			}
			
			if(DecomposeDataBase.find(std::pair<std::string,std::string>(id(),Subgroup.id())) != DecomposeDataBase.end())
				DecomposeDataBase.erase(std::pair<std::string,std::string>(id(),Subgroup.id()));
			DecomposeDataBase.emplace(std::pair<std::string,std::string>(id(),Subgroup.id()), Reps);
			
			return Reps;

		} catch (...) {
			throw;
		}
	}

	/* Overloaded * operator with Reps */
	Sum<Rrep> Rrep::operator*(Rrep Rep) {
		
		try {
			if(nirreps() != Rep.nirreps()) {
				throw "Rrep::operator*::Reps should belong to the same group";
			}

			Sum<Rrep> ListofReps;
			Sum<Rrep> ListofReps2;
			
			if(ProductDataBase.find(std::pair<std::string,std::string>(id(),Rep.id())) != ProductDataBase.end())
			{
				ListofReps = ProductDataBase.at(std::pair<std::string,std::string>(id(),Rep.id()));
				return ListofReps;
			}

			for(int i=0; i<nirreps(); i++) {
				Sum<Irrep> Prod = GetObject(i)*Rep.GetObject(i);
				if(ListofReps.nterms()) {
					for(Sum<Rrep>::iterator it_ListofReps = ListofReps.begin(); it_ListofReps != ListofReps.end(); it_ListofReps++) {
						for(Sum<Irrep>::iterator it_Prod = Prod.begin(); it_Prod != Prod.end(); it_Prod++) {
							Rrep Rep(*it_ListofReps);
							Rep.AddIrrep(*it_Prod);
							ListofReps2.AddTerm(Rep);
						}
					}
					ListofReps = ListofReps2;
					ListofReps2.Clear();
							
				} else {
					for(Sum<Irrep>::iterator it_Prod = Prod.begin(); it_Prod != Prod.end(); it_Prod++)
						ListofReps.AddTerm(Rrep(*it_Prod));
				}
			}
			
			if(ProductDataBase.find(std::pair<std::string,std::string>(id(),Rep.id())) != ProductDataBase.end())
				ProductDataBase.erase(std::pair<std::string,std::string>(id(),Rep.id()));
			ProductDataBase.emplace(std::pair<std::string,std::string>(id(),Rep.id()), ListofReps);
			
			return ListofReps;

		} catch(...) {
			throw;
		}
	}

	/* Direct product of a list of representations */
	Sum<Rrep> Rrep::Product(List<Rrep> &ListofReps) {
		
		try {

			Sum<Rrep> Reps;
			Sum<Rrep> Reps2;
			
			if(ListofReps.nterms() < 2) return Sum<Rrep>(ListofReps.GetObject(0));
			
			if(ListofReps.nterms() == 2) return ListofReps.GetObject(0)*ListofReps.GetObject(1);
		
			List<Rrep> ListofReps2(ListofReps);
			ListofReps2.DeleteTerm(0);
			Rrep Rep = ListofReps.GetObject(0);
			
			Reps2 = Rrep::Product(ListofReps2);
			for(Sum<Rrep>::iterator it_Reps2 = Reps2.begin(); it_Reps2 != Reps2.end(); it_Reps2++)
				Reps.AppendList(Rep*(*it_Reps2));
			
			Reps.Order();
			
			return Reps;

		} catch (...) {
			throw;
		}
	}

	/* Checks whether the rep includes a singlet */
	bool Rrep::hasSinglet() const {
		try {
			for(Rrep::const_iterator it = this->begin(); it != this->end(); it++)
				if(it->dim() == 1)
					return true;
			return false;
		} catch (...) {
			throw;
		}
	}

	/* Overloaded == operator */
	bool Rrep::operator==(const Rrep &R) const
	{
		return List<Irrep>::operator==(R);
	}
	
	/* Overloaded != operator */
	bool Rrep::operator!=(const Rrep &R) const
	{
		return !(*this == R);
	}
	
	/* Overloaded > operator */
	bool Rrep::operator>(const Rrep &R) const {
		
		try {
			if(this->dim() > R.dim())
			{
				return true;
			} else if(this->dim() == R.dim())
			{
				for(int i=0; i<nterms(); i++)
				{
					if(GetObject(i) > R.GetObject(i))
						return true;
					else if(GetObject(i) < R.GetObject(i))
						return false;
				}
			}
			return false;
		} catch (...) {
			throw;
		}
	}

	/* Overloaded < operator */
	bool Rrep::operator<(const Rrep &R) const {
		
		try {
			if((*this)>R)
				return false;
			return true;
		} catch (...) {
			throw;
		}
	}

	/* Returns the json string */
	JSONNode Rrep::json(std::string name) const {
		
		if(name == "id") {
			return JSONNode("", id());
		}
		
		if(nirreps() == 1) return GetObject(0).json();
		
		JSONNode json;
			
		json.push_back(JSONNode("id", id()));
		json.push_back(JSONNode("Group", Group().id()));
		json.push_back(HWeight().json("HWeight"));
		json.push_back(JSONNode("dim", dim()));
		json.push_back(JSONNode("real",real()));
		json.push_back(JSONNode("label",label()));
		json.push_back(JSONNode("nirreps", nirreps()));
		
		JSONNode Casimir(JSON_ARRAY);
		Casimir.set_name("Casimir");
		for(int i=0; i<_Casimir.nterms(); i++) {
			Casimir.push_back(JSONNode("",_Casimir.GetObject(i)));
		}
		
		JSONNode DynkinIndex(JSON_ARRAY);
		DynkinIndex.set_name("DynkinIndex");
		for(int i=0; i<_DynkinIndex.nterms(); i++) {
			DynkinIndex.push_back(JSONNode("",_DynkinIndex.GetObject(i)));
		}
		
		json.push_back(Casimir);
		json.push_back(DynkinIndex);
		json.push_back(JSONNode("hasWeights", hasWeights()));
		json.push_back(_Weights.json("Weights"));
		
		return json;
	}

	/* Parses a json object into the attributes of the class */
	void Rrep::ParseJSON(const JSONNode & n, std::string what) {
		JSONNode::const_iterator i = n.begin();
		while (i != n.end()){
	
			// get the node name and value as a string
			std::string node_name = i -> name();

			// find out where to store the values
			if (node_name == "id") {
					std::string id = i->as_string();
				} else if(node_name == "Group") {
					_Group = new LieGroup(i->as_string());
				} else if(node_name =="HWeight") {
					_HWeight = new Weight(*_Group, _Group->rank());
					_HWeight->ParseJSON(*i);
				} else if(node_name == "dim") {
					_dim = i->as_int();
				} else if(node_name == "real") {
					_real = i->as_bool();
				} else if(node_name == "nirreps") {
					_nirreps = i->as_int();
				} else if(node_name == "label") {
					_label = i->as_string();
				} else if(node_name == "Casimir") {
					JSONNode::const_iterator it = i->begin();
					_Casimir.Clear();
					while(it != i->end()) {
						_Casimir.AddTerm(it->as_float());
						++it;
					}
				} else if(node_name == "DynkinIndex") {
					JSONNode::const_iterator it = i->begin();
					_DynkinIndex.Clear();
					while(it != i->end()) {
						_DynkinIndex.AddTerm(it->as_float());
						++it;
					}
				} else if(node_name == "hasWeights" and what == "Weights") {
					_hasWeights = i->as_bool();
				} else if(node_name == "Weights" and what == "Weights") {
					_Weights.ParseJSON(*i);
				}
		
			//increment the iterator
			++i;
		}
		
		if(!nterms())
		{
			int accumulated_rank = 0;
			_nirreps = 0;
			for(int i=0; i<Group().ngroups(); i++)
			{
				RVector<double> R = HWeight().ExtractMatrix(0,0,accumulated_rank,accumulated_rank + Group().GetObject(i).rank()-1).Row(0);
				Weight HW(Group().GetObject(i), R);
				Irrep aRep(Group().GetObject(i), HW);
				this->AddTerm(aRep);
				_nirreps ++;
				accumulated_rank += Group().GetObject(i).rank();
			}
		}
		

	}

}
