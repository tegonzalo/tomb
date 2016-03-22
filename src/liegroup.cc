/*
 * liegroup.cc
 * Created by T.Gonzalo on 15/02/2012.
 * Last modified on 10/11/2015
 */

#include "headers.h"

/**************************************************/
/*************Class LieGroup methods***************/
/**************************************************/

namespace Tomb
{
	// Definition of static variables and helper functions
	
	std::map<std::string, LieGroup> LieGroup::DataBase;
	std::map<std::string, JSONNode> LieGroup::JSONDataBase;
	
	bool LieGroup::database_check(std::string id, std::string what) {
		if(DataBase.empty())
		{
			DataBase.clear();
			return false;
		}
		if(DataBase.find(id) != DataBase.end()) {
			if (what == "")
				return true;
			if(what == "Reps" and DataBase.at(id).hasReps())
				return true;
			if(what == "Subgroups" and DataBase.at(id).hasSubgroups())
				return true;
		}
		if(JSONDataBase.find(id) != JSONDataBase.end()) {
			LieGroup G(JSONDataBase.at(id));
			if(what == "Reps" or (what == "Subgroups" and DataBase.find(id) != DataBase.end() and DataBase.at(id).hasReps()))
				G.ParseJSON(JSONDataBase.at(id),"Reps");
			if(what == "Subgroups" or (what == "Reps" and DataBase.find(id) != DataBase.end() and DataBase.at(id).hasSubgroups()))
				G.ParseJSON(JSONDataBase.at(id),"Subgroups");
			if(DataBase.find(id) != DataBase.end())
				DataBase.erase(id);
			DataBase.emplace(id, G);
			return true;
		}
		return false;
	}
	
	void LieGroup::database_emplace(std::string id, LieGroup G) {
		if(database_check(id))
			DataBase.erase(id);
		DataBase.emplace(id, G);
	}
	
	// Member functions

	/* Constructor */
	LieGroup::LieGroup() : Product<SimpleGroup>() {
		this->_ngroups = 0;
		this->_rank = 0;
		this->_dim = 0;
		this->_nabelians = 0;
		this->_simple = false;
		this->_semisimple = false;
		this->_label = this->Print();
		this->_Casimir.Clear();
		
	}

	/* Constructor 2, with strings */
	LieGroup::LieGroup(const std::string id) : Product<SimpleGroup>() {
		
		try {
			this->_ngroups = 0;
			this->_rank = 0;
			this->_dim = 0;
			this->_nabelians = 0;
			this->_simple = false;
			this->_semisimple = false;
			

			std::string liegroupid(Strings::split_string(id, '[')[0]);
			List<std::string> simplegroupids = Strings::split_string(liegroupid,'x');
			
			SimpleGroup sg(simplegroupids[0]);

			for(int i=0; i<simplegroupids.nterms(); i++)
				AddTerm(SimpleGroup(simplegroupids[i]));
			
			this->Order();
			
			init();
			
		} catch (...) {
			throw;
		}
	}

	/* Constructor 3, with json nodes */
	LieGroup::LieGroup(const JSONNode &n) {
		
		if(n.as_string() != "") {
			LieGroup(n.as_string());
		} else {
			ParseJSON(n);
		}
	}
	
	/* Copy constructor 1 */
	LieGroup::LieGroup(const LieGroup &Group) : Product<SimpleGroup>(Group) {
		
		try {
			
			this->_rank = Group.rank();
			this->_dim = Group.dim();
			this->_simple = Group.simple();
			this->_semisimple = Group.semisimple();
			this->_ngroups = Group.ngroups();
			this->_nabelians = Group.nabelians();
			this->_label = this->Print();
			this->_Casimir = Group.Casimir();
			this->_repsMaxDim = Group.repsMaxDim();
			this->_hasReps = Group.hasReps();
			this->_hasSubgroups = Group.hasSubgroups();
			//std::cout << "hasReps = " << _hasReps << std::endl;
			if(_hasReps) _Reps = Group.RepsConst();
			
			//std::cout << "hasSubgroups = " << _hasSubgroups << std::endl;
			if(_hasSubgroups) _Subgroups = Group.SubgroupsConst();
		} catch (...) {
			throw;
		}
	}

	/* Copy constructor 2 */
	LieGroup::LieGroup(const SimpleGroup &Group) : Product<SimpleGroup>(Group) {
		try {
			this->_ngroups = 1;
			this->_rank = Group.rank();
			this->_dim = Group.dim();
			this->_nabelians = (Group.abelian() ? 1 : 0);
			this->_simple = true;
			this->_semisimple = true;	
			this->_label = this->Print();
			this->_Casimir.AddTerm(Group.Casimir());
			this->_repsMaxDim = Group.repsMaxDim();
			this->_hasReps = Group.hasReps();
			this->_hasSubgroups = Group.hasSubgroups();
			if(_hasReps) _Reps = Irreps2Reps(Group.IrrepsConst());
			if(_hasSubgroups) _Subgroups = Group.SubgroupsConst();

		} catch (...) {
			throw;
		}
	}

	/* Copy constructor 3 */
	LieGroup::LieGroup(const List<SimpleGroup> &Group) : Product<SimpleGroup>() {
		try {
			_ngroups = 0;
			_rank = 0;
			_dim = 0;
			_nabelians = 0;
			_simple = false;
			_semisimple = false;
			_label = this->Print();
			
			for(int i=0; i<Group.nterms(); i++) {
				AddTerm(Group.GetObject(i));
			}
			
			_label = this->Print();
			_hasReps = false;
			_hasSubgroups = false;
			
		} catch(...) {
			throw;
		}
	}

	/* Copy constructor 4 */
	LieGroup::LieGroup(const Product<SimpleGroup> &Group) : Product<SimpleGroup>() {
		try {
			_ngroups = 0;
			_rank = 0;
			_dim = 0;
			_nabelians = 0;
			_simple = false;
			_semisimple = false;
			_label = this->Print();
			
			for(int i=0; i<Group.nterms(); i++) {
				AddTerm(Group.GetObject(i));
				
			_label = this->Print();
			_hasReps = false;
			_hasSubgroups = false;
			
			}
		} catch(...) {
			throw;
		}
	}

	/* Move constructor */
	LieGroup::LieGroup(LieGroup &&Group) : 
		Product<SimpleGroup>(std::move(Group)),
		_rank(std::move(Group._rank)),
		_dim(std::move(Group._dim)),
		_simple(std::move(Group._simple)),
		_semisimple(std::move(Group._semisimple)),
		_ngroups(std::move(Group._ngroups)),
		_nabelians(std::move(Group._nabelians)),
		_label(std::move(Group._label)),
		_Casimir(std::move(Group._Casimir)),
		_repsMaxDim(std::move(Group._repsMaxDim)),
		_hasReps(std::move(Group._hasReps)),
		_hasSubgroups(std::move(Group._hasSubgroups)),
		_Reps(std::move(Group._Reps)),
		_Subgroups(std::move(Group._Subgroups))
	{
		try {
			Group._rank = 0;
			Group._dim = 0;
			Group._nabelians = 0;
			Group._simple = false;
			Group._semisimple = false;
			Group._label = "";
			Group._Casimir.Clear();
			Group._repsMaxDim = 0;
			Group._hasReps = false;
			Group._hasSubgroups = false;
			Group._Reps.Clear();
			Group._Subgroups.Clear();
			
		} catch (...) {
			throw;
		}
	}	

	/* Destructor */
	LieGroup::~LieGroup() {
		this->_ngroups = 0;
		this->_rank = 0;
		this->_dim = 0;
		this->_nabelians = 0;
		this->_simple = false;
		this->_semisimple = false;
		this->_label = "";
		this->_Reps.Clear();
		this->_Subgroups.Clear();
	}

	/* Assignment operator 1 */
	LieGroup &LieGroup::operator=(const LieGroup &Group) {
		try {

			if(this == &Group) return *this;

			Product<SimpleGroup>::operator=(Group);
			
			_rank = Group.rank();
			_dim = Group.dim();
			_simple = Group.simple();
			_semisimple = Group.semisimple();
			_ngroups = Group.ngroups();
			_nabelians = Group.nabelians();
			_label = this->Print();
			_Casimir = Group.Casimir();
			_repsMaxDim = Group.repsMaxDim();
			_hasReps = Group.hasReps();
			_hasSubgroups = Group.hasSubgroups();
			_Reps.Clear();
			_Subgroups.Clear();
			if(_hasReps) _Reps = Group.RepsConst();
			if(_hasSubgroups) _Subgroups = Group.SubgroupsConst();

			return *this;
			
		} catch (...) {
			throw;
		}
	}

	/* Assignment operator 2 */
	LieGroup &LieGroup::operator=(const SimpleGroup &Group) {
		try {			
			Product<SimpleGroup>::operator=(Group);
			
			this->_ngroups =1;
			this->_rank = Group.rank();
			this->_dim = Group.dim();
			this->_nabelians = (Group.abelian() ? 1 : 0);
			this->_simple = true;
			this->_semisimple = true;
			this->_label = this->Print();
			this->_Casimir.AddTerm(Group.Casimir());
			_repsMaxDim = Group.repsMaxDim();
			_hasReps = Group.hasReps();
			_hasSubgroups = Group.hasSubgroups();
			_Reps.Clear();
			_Subgroups.Clear();
			if(_hasReps) _Reps = Irreps2Reps(Group.IrrepsConst());
			if(_hasSubgroups) _Subgroups = Group.SubgroupsConst();
			
			return *this;
		} catch (...) {
			throw;
		}
	}

	/* Assignment operator 3 */
	LieGroup &LieGroup::operator=(const List<SimpleGroup> &Group) {
		try {
			if(this == &Group) return *this;
			
			if(this->ngroups()) this->Clear();
			_Reps.Clear();
			_Subgroups.Clear();
			
			_ngroups = 0;
			_rank = 0;
			_dim = 0;
			_nabelians = 0;
			_simple = false;
			_semisimple = false;
			
			for(int i=0; i<Group.nterms(); i++) {
				AddTerm(Group.GetObject(i));
			}
			
			_label = this->Print();
			_hasReps = false;
			_hasSubgroups = false;
			_Reps.Clear();
			_Subgroups.Clear();
			
			return *this;
		} catch(...) {
			throw;
		}
	}

	/* Assignment operator 4 */
	LieGroup &LieGroup::operator=(const Product<SimpleGroup> &Group) {
		try {
			if(this == &Group) return *this;
			
			if(this->ngroups()) this->Clear();
			_Reps.Clear();
			_Subgroups.Clear();
			
			_ngroups = 0;
			_rank = 0;
			_dim = 0;
			_nabelians = 0;
			_simple = false;
			_semisimple = false;
			
			for(int i=0; i<Group.nterms(); i++) {
				AddTerm(Group.GetObject(i));
			}
			
			_label = this->Print();
			_hasReps = false;
			_hasSubgroups = false;
			_Reps.Clear();
			_Subgroups.Clear();
			
			return *this;
		} catch(...) {
			throw;
		}
	}

	/* Move = operator */
	LieGroup &LieGroup::operator=(LieGroup &&Group) {
		try {
			if(this == &Group) return *this;
			
			Product<SimpleGroup>::operator=(std::move(Group));
			
			_rank = std::move(Group._rank);
			_dim = std::move(Group._dim);
			_simple = std::move(Group._simple);
			_semisimple = std::move(Group._semisimple);
			_ngroups = std::move(Group._ngroups);
			_nabelians = std::move(Group._nabelians);
			_label = std::move(Group._label);
			_Casimir = std::move(Group._Casimir);
			_repsMaxDim = std::move(Group._repsMaxDim);
			_hasReps = std::move(Group._hasReps);
			_hasSubgroups = std::move(Group._hasSubgroups);
			_Reps = std::move(Group._Reps);
			_Subgroups = std::move(Group._Subgroups);
			
			Group._rank = 0;
			Group._dim = 0;
			Group._nabelians = 0;
			Group._simple = false;
			Group._semisimple = false;
			Group._label = "";
			Group._Casimir.Clear();
			Group._repsMaxDim = 0;
			Group._hasReps = false;
			Group._hasSubgroups = false;
			Group._Reps.Clear();
			Group._Subgroups.Clear();

			return *this;
			
		} catch (...) {
			throw;
		}
	}

	/* Initialises some variables */
	void LieGroup::init() {
		
		try {
			if(database_check(id()))
				*this = DataBase.at(id());
			 else		
				database_emplace(id(), *this);
			
		} catch (...) {
			throw;
		}
	}

	/* Identifier of the Lie Group */
	std::string LieGroup::id() const {
		std::stringstream s;
		if(nterms()) {
			for(int i=0; i<nterms()-1; i++) {
				s << GetObject(i).id() << 'x';
			}
			s << GetObject(nterms()-1).id();
		}
		return s.str();
	}

	/* Returns the rank */
	int LieGroup::rank() const {
		return _rank;
	}

	/* Returns the dimension */
	int LieGroup::dim() const {
		return _dim;
	}

	/* Returns if the group is simple */
	bool LieGroup::simple() const {
		try {
			if(this->ngroups() == 1 and !this->nabelians()) {
				return true;
			} else {
				return false;
			}
		} catch (...) {
			throw;
		}
	}

	/* Returns if the group is semisimple */
	bool LieGroup::semisimple() const {
		if(this->ngroups() == 1) {
			return true;
		}
		if(this->ngroups() > 1 and this->nabelians() == 0) {
			return true;
		} else {
			return false;
		}
	}

	/* Returns the number of simple groups / abelian groups in the product */
	int LieGroup::ngroups() const {
	return this->_ngroups;
	}

	/* Returns the number of abelian groups in the product */
	int LieGroup::nabelians() const {

		try {
			int nabelians = 0;
			for(int i=0; i<this->ngroups(); i++) {
				if(this->GetObject(i).abelian()) {
					nabelians++;
				}
			}
			return nabelians;
		} catch (...) {
			throw;
		}
	}

	/* Returns if the Lie Group is abelian */
	bool LieGroup::abelian() const {
		
		try {
			for(int i=0; i<this->ngroups(); i++) {
				if(!this->GetObject(i).abelian()) {
					return false;
				}
			}
			return true;
		} catch (...) {
			throw;
		}
	}

	/* Returns the label of the LieGroup */
	std::string LieGroup::label() const {
		return _label;
	}

	/* Returns the Casimir of the LieGroup */
	List<double> LieGroup::Casimir() const {
		return _Casimir;
	}
	
	/* Returns the maximum dimension of the calculated reps */
	int LieGroup::repsMaxDim() const {
		return _repsMaxDim;
	}
	
	/* Returns whether the LieGroup has calculated its reps */
	bool LieGroup::hasReps() const {
		return _hasReps;
	}
	
	/* Returns whether the LieGroup has calculated its subgroups */
	bool LieGroup::hasSubgroups() const {
		return _hasSubgroups;
	}

	/* Delete a subgroup of the LieGroup */
	void LieGroup::DeleteTerm(int i) {
		
		try {
			if(GetObject(i).abelian()) {
				_nabelians--;
			}
			_rank -= GetObject(i).rank();
			_dim -= GetObject(i).dim();
			_ngroups--;
			_simple = this->simple();
			_semisimple = this->semisimple();
			_Casimir.DeleteTerm(i);
			
			Product<SimpleGroup>::DeleteTerm(i);
			
			_label = this->Print();
			
		} catch (...) {
			throw;
		}
	}

	/* Adds a simple group to the Lie group */
	void LieGroup::AddTerm(const SimpleGroup &Group) {

		try {
			Product<SimpleGroup>::AddTerm(Group);
			if(_ngroups == 0) _dim = 0;
			_rank += Group.rank();
			_dim += Group.dim();
			if(Group.abelian()) {
				_semisimple = false;
				_nabelians++;
			}
			_ngroups++;
			_simple = true;
			if(_ngroups > 1) _simple = false;
			_label = this->Print();
			_Casimir.AddTerm(Group.Casimir());

		} catch (...) {
			throw;
		}
	}

	/* Adds a lie group to the lie group */
	void LieGroup::AddTerm(const LieGroup &Group) {
		try {
			for(int i=0; i<Group.ngroups(); i++) {
				AddTerm(Group.GetObject(i));
			}

		} catch(...) {
			throw;
		}
	}

	/* Calculates the maximal subgroups of a Lie Group */
	List<SubGroup> &LieGroup::MaximalSubgroups() {
		
		try {
			
			// If maximal subgroups have been calculated already return them
			if(_MaxSubgroups.nterms()) {
				return _MaxSubgroups;
			}
			
			// If the group consists only on abelian groups return the empty variable
			if(ngroups() == nabelians()) {
				return _MaxSubgroups;
			}
			
			// If there is only one group return the maximal subgroups of that simple group
			if(this->ngroups() == 1) {
				return this->GetObject(0).MaximalSubgroups();
			}
			
			List<List<SubGroup> > ListofSubgroups;
			
			for(LieGroup::iterator it = this->begin(); it != this->end(); it++) {
				List<SubGroup> Subgroups(SubGroup(*it,*it));
				
				Subgroups.AppendList(it->MaximalSubgroups());
				ListofSubgroups.AddTerm(Subgroups);
			}
			
			int nterms = 1;
			for(int i=0; i<ListofSubgroups.nterms(); i++) {
				nterms *= ListofSubgroups.GetObject(i).nterms();
			}

			//std::cout << ListofSubgroups << std::endl;

			//time1 = clock();
			for(int i=0; i<nterms; i++) {
				///std::cout << "i = " << i << std::endl;
				SubGroup Subgroup(*this);
				
				int terms = 1;
				for(int j=0; j<ngroups(); j++) {
					//std::cout << "j = " << j << std::endl;
					int thisterms = ListofSubgroups.GetObject(j).nterms();
					terms *= thisterms;
					int index = (i*terms/nterms)%thisterms;
					Subgroup.AddTerm(ListofSubgroups.GetObject(j).GetObject(index),j);
				}
				//std::cout << Subgroup.json().write_formatted() << std::endl;
				//getchar();
				Subgroup.Order();
				//std::cout << "------" << std::endl << Subgroup.json().write_formatted() << std::endl << "--------" << std::endl;
				//getchar();
				_MaxSubgroups.AddTerm(Subgroup);
			}
			//std::cout << _MaxSubgroups << std::endl;
			_MaxSubgroups.DeleteTerm(0);
			_MaxSubgroups.Order();
			_MaxSubgroups.EliminateRepeated();
			//std::cout << _MaxSubgroups << std::endl;
			//std::cout << "max2 = " << double(clock()-time1)/CLOCKS_PER_SEC << std::endl;


			//for(int i=0; i<_MaxSubgroups.nterms(); i++) {
			//	std::cout << _MaxSubgroups.GetObject(i).json().write_formatted() << std::endl;
			//}
			
			return _MaxSubgroups;
			
		} catch (...) {
			throw;
		}
	}

	/* Calculates the special subgroups of a Lie Group */
	List<SubGroup> LieGroup::SpecialSubgroups() {
		
		try {
			
			List<SubGroup> SpecialSubgroups;
			
			// If the group consists only on abelian groups return the empty variable
			if(ngroups() == nabelians()) {
				return SpecialSubgroups;
			}
			
			// If there is only one group return the maximal subgroups of that simple group
			if(this->ngroups() == 1) {
				return this->GetObject(0).SpecialSubgroups();
			}
			
			List<List<SubGroup> > ListofSubgroups;

			for(LieGroup::iterator it = this->begin(); it != this->end(); it++) {
				List<SubGroup> Subgroups(SubGroup(*it,*it));
				Subgroups.AppendList(it->SpecialSubgroups());
				ListofSubgroups.AddTerm(Subgroups);
			}
			
			int nterms = 1;
			for(int i=0; i<ListofSubgroups.nterms(); i++) {
				nterms *= ListofSubgroups.GetObject(i).nterms();
			}

			for(int i=0; i<nterms; i++) {
				SubGroup Subgroup(*this);
				
				int terms = 1;
				for(int j=0; j<ngroups(); j++) {
					int thisterms = ListofSubgroups.GetObject(j).nterms();
					terms *= thisterms;
					int index = (i*terms/nterms)%thisterms;
					Subgroup.AddTerm(ListofSubgroups.GetObject(j).GetObject(index),j);
				}
				Subgroup.Order();
				SpecialSubgroups.AddTerm(Subgroup);
			}
			
			SpecialSubgroups.DeleteTerm(0);
			SpecialSubgroups.Order();
			SpecialSubgroups.EliminateRepeated();

			return SpecialSubgroups;
			
		} catch (...) {
			throw;
		}
	}

	/* Calculates all the subgroups of a Lie group */
	List<SubGroup> &LieGroup::Subgroups() {
		
		try {
			
			// If Subgroups are known, return them
			if(_Subgroups.nterms()) {
				return _Subgroups;
			}
			
			// If info is already on the database, pull it
			if(database_check(id(), "Subgroups") and DataBase.at(id()).hasSubgroups()) {
				_Subgroups = DataBase.at(id()).Subgroups();
				if(_Subgroups.nterms()) _hasSubgroups = true;
				return _Subgroups;
			}

			// If not, calculate it
			std::cout << "Calculating subgroups of " << *this << std::endl;
		
			// Then calculate the maximal subgroups
			List<SubGroup> MaximalSubgroups = this->MaximalSubgroups();
			_Subgroups.AppendList(MaximalSubgroups);
			//std::cout << "maximal sugroups " << _Subgroups << std::endl;

			//time1 = clock();
			// Now, iterate to find the subgroups of the subroups
			int nsubgroups = _Subgroups.nterms();
			int group = 0;
			//time1 = clock();
			//std::cout << nsubgroups << std::endl;
			for(List<SubGroup>::iterator it_Subgroups = MaximalSubgroups.begin(); it_Subgroups != MaximalSubgroups.end() and group < nsubgroups; it_Subgroups++) {
				//std::cout << group << std::endl;
				SubGroup subgroup = *it_Subgroups;
				//std::cout << subgroup << std::endl;
				//std::cout << "suuuubgroups of " << subgroup << " = {  " << subgroup.Subgroups() << "  }" << std::endl;
				List<SubGroup> AuxSubgroups = subgroup.Subgroups();
				//std::cout << "Subgroups of " << subgroup << " are " << AuxSubgroups << std::endl;

				for(List<SubGroup>::iterator it_AuxSubgroups = AuxSubgroups.begin(); it_AuxSubgroups != AuxSubgroups.end(); it_AuxSubgroups++) {
					//std::cout << it_AuxSubgroups->json().write_formatted() << std::endl;
					SubGroup Subgroup(*it_AuxSubgroups, subgroup);
					//if(Subgroup.ngroups()==Subgroup.nabelians()) std::cout << Subgroup.json().write_formatted() << std::endl;
					Subgroup.SetProjection(it_AuxSubgroups->Projection()*it_Subgroups->Projection());
					//std::cout << Subgroup.Projection() << std::endl;
					Subgroup.Order();
					//std::cout << Subgroup.json().write_formatted() << std::endl;
					//for(int j=0; j<Subgroup.SuperGroups().nterms(); j++) {
					//	std::cout << Subgroup.SuperGroups().GetObject(j).nleaves() << std::endl;
					//	}
					//std::cout << Subgroup.id() << std::endl;
					if(_Subgroups.Index(Subgroup) == -1) {
						_Subgroups.AddTerm(Subgroup);
					}
				}
				//std::cout << _Subgroups << std::endl;
				
				group++;
			}	

			//std::cout << ngroups() << std::endl;
			// Calculate subgroups by truncating the group
			if(ngroups()>1) _Subgroups.AppendList(SplitToSubGroups());
			
			//std::cout << _Subgroups << std::endl;

			// Last operations on the groups
			_Subgroups.Order();
			_Subgroups.EliminateRepeated();
			
			//std::cout << Subgroups << std::endl;
			
			// Set the hasSubgroups flag to true
			if(_Subgroups.nterms()) _hasSubgroups = true;
			
			// If there is an entry in the database delete it and dump this
			database_emplace(id(), *this);
			
			//std::cout << "Subgroups of " << *this << " calculated" << std::endl;
			
			return _Subgroups;
		
		} catch (...) {
			throw;
		}
	}

	/* Returns the variable Subgroups */
	List<SubGroup> LieGroup::SubgroupsConst() const {
		return _Subgroups;
	}
	
	/* Calculates the subgroups with a given rank of a Lie group */
	List<SubGroup> LieGroup::Subgroups(int rank) {
		
		try {
			
			// If Subgroups are known, return them
			if(_Subgroups.nterms()) {
				List<SubGroup> Subgroups;
				for(List<SubGroup>::iterator it_Subgroups = _Subgroups.begin(); it_Subgroups != _Subgroups.end(); it_Subgroups++)
					if(it_Subgroups->rank() == rank)
						Subgroups.AddTerm(*it_Subgroups);
				return Subgroups;
			}
			
			// If info is already on the database, pull it			
			if(database_check(id(), "Subgroups") and DataBase.at(id()).hasSubgroups()) {
				_Subgroups = DataBase.at(id()).Subgroups();
				_hasSubgroups = true;
				List<SubGroup> Subgroups;
				for(List<SubGroup>::iterator it_Subgroups = _Subgroups.begin(); it_Subgroups != _Subgroups.end(); it_Subgroups++)
					if(it_Subgroups->rank() == rank)
						Subgroups.AddTerm(*it_Subgroups);
				return Subgroups;
			}

			// If not, calculate it
			//std::cout << "Calculating subgroups of " << *this << std::endl;
			// Calculate the maximal subgroups
			List<SubGroup> MaximalSubgroups = this->MaximalSubgroups();
			List<SubGroup> Subgroups;
			Subgroups.AppendList(MaximalSubgroups);


			// Now, iterate to find the subgroups of the subroups
			int nsubgroups = Subgroups.nterms();
			int group = 0;
			for(List<SubGroup>::iterator it_Subgroups = MaximalSubgroups.begin(); it_Subgroups != MaximalSubgroups.end() and group < nsubgroups; it_Subgroups++) {
				SubGroup subgroup = *it_Subgroups;
				List<SubGroup> AuxSubgroups = subgroup.Subgroups(rank);

				for(List<SubGroup>::iterator it_AuxSubgroups = AuxSubgroups.begin(); it_AuxSubgroups != AuxSubgroups.end(); it_AuxSubgroups++) {
					SubGroup Subgroup(*it_AuxSubgroups, subgroup);
					Subgroup.SetProjection(it_AuxSubgroups->Projection()*it_Subgroups->Projection());
					Subgroup.Order();
					if(Subgroups.Index(Subgroup) == -1 and Subgroup.rank() == rank) {
						Subgroups.AddTerm(Subgroup);
					}
				}

				//std::cout << Subgroups << std::endl;
				group++;
			}
			
			if(rank != _rank)
				for(int i=0; i<MaximalSubgroups.nterms(); i++) 
					Subgroups.DeleteTerm(0);
			
			
			// Calculate subgroups by truncating the group
			Subgroups.AppendList(SplitToSubGroups(rank));
			
			Subgroups.EliminateRepeated();
			
			return Subgroups;
		
		} catch (...) {
			throw;
		}
	}

	/* Calculates the subgroups with ranks between rank1 and rank2 */
	List<SubGroup> LieGroup::Subgroups(int r1, int r2) {
		try {
			int rank1 = r1, rank2 = r2;
			if(r1 > r2) {
				rank1 = r2;
				rank2 = r1;
			}

			// If Subgroups are known, return them
			if(_Subgroups.nterms()) {
				List<SubGroup> Subgroups;
				for(List<SubGroup>::iterator it_Subgroups = _Subgroups.begin(); it_Subgroups != _Subgroups.end(); it_Subgroups++)
					if(it_Subgroups->rank() >= rank1 and it_Subgroups->rank() <= rank2)
						Subgroups.AddTerm(*it_Subgroups);
				return Subgroups;
			}
			
			// If info is in the database pull it
			if(database_check(id(), "Subgroups") and DataBase.at(id()).hasSubgroups()) {
				_Subgroups = DataBase.at(id()).Subgroups();
				_hasSubgroups = true;
				List<SubGroup> Subgroups;
				for(List<SubGroup>::iterator it_Subgroups = _Subgroups.begin(); it_Subgroups != _Subgroups.end(); it_Subgroups++)
					if(it_Subgroups->rank() >= rank1 and it_Subgroups->rank() <= rank2)
						Subgroups.AddTerm(*it_Subgroups);
				return Subgroups;
			}

			// If not, calculate it
			// Calculate the maximal subgroups
			List<SubGroup> MaximalSubgroups = this->MaximalSubgroups();
			List<SubGroup> Subgroups;
			Subgroups.AppendList(MaximalSubgroups);

			// Now, iterate to find the subgroups of the subroups
			int nsubgroups = Subgroups.nterms();
			int group = 0;

			for(List<SubGroup>::iterator it_Subgroups = MaximalSubgroups.begin(); it_Subgroups != MaximalSubgroups.end() and group < nsubgroups; it_Subgroups++) {	
				SubGroup subgroup = *it_Subgroups;
				List<SubGroup> AuxSubgroups = subgroup.Subgroups(rank1, rank2);
				for(List<SubGroup>::iterator it_AuxSubgroups = AuxSubgroups.begin(); it_AuxSubgroups != AuxSubgroups.end(); it_AuxSubgroups++) {	
					SubGroup Subgroup(*it_AuxSubgroups, subgroup);
					Subgroup.SetProjection(it_AuxSubgroups->Projection()*it_Subgroups->Projection());
					Subgroup.Order();
					if(Subgroups.Index(Subgroup) == -1 and Subgroup.rank() >= rank1 and Subgroup.rank() <= rank2) {
						Subgroups.AddTerm(Subgroup);
					}
				}

				//std::cout << Subgroups << std::endl;
				group++;
			}

			// Calculate subgroups by truncating the group
			Subgroups.AppendList(SplitToSubGroups(rank1, rank2));

			Subgroups.EliminateRepeated();

			return Subgroups;

		} catch (...) {
			throw;
		}
			
	}

	/* Splits the group into subgroups */
	List<SubGroup> LieGroup::SplitToSubGroups(int rank) {
		try {
			
			List<SubGroup> Subgroups;
			
			if(!nabelians()) return Subgroups;
						
			for(int j=0; j<nabelians(); j++) {
				//std::cout << "j = " << j << std::endl;
				SubGroup Subgroup(SubGroup(*this,*this));
				//std::cout << Subgroup << std::endl;
				Subgroup.DeleteTerm(ngroups()-j-1);
				//std::cout << Subgroup << std::endl;
				if(Subgroup.nterms() and Subgroups.Index(Subgroup) == -1) {
					Subgroups.AddTerm(Subgroup);
					List<SubGroup> AuxSubgroups = Subgroup.SplitToSubGroups();
					for(List<SubGroup>::iterator it_AuxSubgroups = AuxSubgroups.begin(); it_AuxSubgroups != AuxSubgroups.end(); it_AuxSubgroups++) {	
						SubGroup subgroup(*it_AuxSubgroups, Subgroup);
						subgroup.SetProjection(it_AuxSubgroups->Projection()*Subgroup.Projection());
						subgroup.Order();
						if(Subgroups.Index(subgroup) == -1 and (!rank or subgroup.rank() == rank)) {
							Subgroups.AddTerm(subgroup);
						}
					}
				}
			}
			//std::cout << Subgroups << std::endl;
			return Subgroups;
			
		} catch (...) {
			throw;
		}
	}

	/* Splits the group into subgroups between two ranks */
	List<SubGroup> LieGroup::SplitToSubGroups(int r1, int r2) {
		try {
			
			List<SubGroup> Subgroups;
			
			int rank1 = r1, rank2 = r2;
			if(r1 > r2) {
				rank1 = r2;
				rank2 = r1;
			}
			
			for(int j=0; j<ngroups(); j++) {
				SubGroup Subgroup(SubGroup(*this,*this));
				Subgroup.DeleteTerm(j);
				if(Subgroup.nterms() and Subgroups.Index(Subgroup) == -1) {
					Subgroups.AddTerm(Subgroup);
					List<SubGroup> AuxSubgroups = Subgroup.SplitToSubGroups();
					for(List<SubGroup>::iterator it_AuxSubgroups = AuxSubgroups.begin(); it_AuxSubgroups != AuxSubgroups.end(); it_AuxSubgroups++) {	
						SubGroup subgroup(*it_AuxSubgroups, Subgroup);
						subgroup.SetProjection(it_AuxSubgroups->Projection()*Subgroup.Projection());
						subgroup.Order();
						if(Subgroups.Index(subgroup) == -1 and subgroup.rank() >= rank1 and subgroup.rank() <= rank2) {
							Subgroups.AddTerm(subgroup);
						}
					}
				}
			}
			
			return Subgroups;
			
		} catch (...) {
			throw;
		}
	}

	/* Returns the breaking chain from a liegroup to a simple group */
	List<List<Tree<SimpleGroup> > > LieGroup::BreakingChains(const SimpleGroup &Subgroup) {
		try {
			return BreakingChains(LieGroup(Subgroup));	
		} catch (...) {
			throw;
		}
	}

	/* Returns the breaking chain from a liegroup to a subgroup */
	List<List<Tree<SimpleGroup> > > LieGroup::BreakingChains(const LieGroup &Subgroup) {
		try {
			
			//std::cout << "Chains of " << *this << " to " << Subgroup << std::endl;
			List<List<Tree<SimpleGroup> > > BrChains;
			
			if(!Subgroup.isSubgroupOf(*this)) {
				return BrChains;
			}
			
			List<SubGroup> Subgroups = this->Subgroups(Subgroup.rank(), rank());		
			
			//std::cout << Subgroups.json().write_formatted() << std::endl;
		
			for(List<SubGroup>::iterator it_Subgroups = Subgroups.begin(); it_Subgroups != Subgroups.end(); it_Subgroups++) {
			//	std::cout << *it_Subgroups << std::endl;
				if(LieGroup(*it_Subgroups) == Subgroup) {
					//std::cout << "-----" << std::endl;
					List<Tree<SimpleGroup> > Chain;
					for(int i=0; i<nterms(); i++) {
						Tree<SimpleGroup> tree(GetObject(i));
						char label = 'A' + i;
						for(int j=0; j<it_Subgroups->nterms(); j++) {
							if(nterms() == 1 or (nterms() > 1 and it_Subgroups->labels().GetObject(j)[0] == label)) {
								Tree<SimpleGroup> branch(it_Subgroups->GetObject(j));
								branch.setLabel(it_Subgroups->labels().GetObject(j));
								tree.AddBranch(branch);
							}
						}
						//std::cout << tree << std::endl;
						tree.setLabel(std::string(1,label));
						Chain.AddTerm(tree);	
					}
					//std::cout << Chain << std::endl;
					BrChains.AddTerm(Chain);
				} else if(Subgroup.isSubgroupOf(*it_Subgroups)) {
					List<List<Tree<SimpleGroup> > > BrChains2 = it_Subgroups->BreakingChains(Subgroup);
				//	std::cout << "BrChains" << std::endl;
				//	std::cout << BrChains2 << std::endl;
					for(List<List<Tree<SimpleGroup> > >::iterator it_BrChains2 = BrChains2.begin(); it_BrChains2 != BrChains2.end(); it_BrChains2++) {
						List<Tree<SimpleGroup> > Chain;
						for(int i=0; i<nterms(); i++) {
							Tree<SimpleGroup> tree(GetObject(i));
							char label = 'A' + i;
							//std::cout << label << std::endl;
							for(int j=0; j<it_BrChains2->nterms(); j++) {
								//std::cout << j << std::endl;
								if(nterms() == 1 or (nterms() > 1 and it_Subgroups->labels().GetObject(j)[0] == label)) {
									Tree<SimpleGroup> branch(it_BrChains2->GetObject(j));
									//branch.setLabel(it_BrChains2->GetObject(j).label());
									branch.setLabel(it_Subgroups->labels().GetObject(j));
									//std::cout << "------------" << std::endl;
									//std::cout << branch << std::endl;
									//std::cout << "------------" << std::endl;
									tree.AddBranch(branch);
								}
							}
							//std::cout << "=====================" << std::endl;
							//std::cout << tree << std::endl;
							tree.setLabel(std::string(1,label));
							//std::cout << "=====================" << std::endl;
							//std::cout << tree << std::endl;
							//std::cout << "=====================" << std::endl;
							Chain.AddTerm(tree);	
						}
						
						BrChains.AddTerm(Chain);
						}
						
				}
				//std::cout << BrChains << std::endl;
			}
			
			/// U(1) breaking
			// Iterate over the chains to obtain the label
			List<std::string> Labels;
			List<List<Tree<SimpleGroup> > > SemisimpleParts;
			
			for(List<List<Tree<SimpleGroup> > >::iterator it_BrChains = BrChains.begin(); it_BrChains != BrChains.end(); it_BrChains++) {
				List<Tree<SimpleGroup> > Chain = *it_BrChains;
			//	std::cout << Chain << std::endl;
					
				LieGroup Supergroup = Chain.GetObject(0).Level(0);
				LieGroup Group = Chain.GetObject(0).Level(1);
				for(int i=1; i<Chain.nterms(); i++) {
					Supergroup.AddTerm(Chain.GetObject(i).Level(0));
					if(Chain.GetObject(i).depth() > 1) {
						Group.AddTerm(Chain.GetObject(i).Level(1));
					}
				}
						
				//std::cout << Supergroup << std::endl;
				//std::cout << Group << std::endl;
				
				// Check whether there is rank reduction and, if so get the labels of the abelians
				if(Supergroup.rank() > Group.rank() and Group.nabelians() > 0) {
					
					std::string label;
					
					for(int i=0; i<Chain.nterms(); i++) {
						Tree<SimpleGroup> SubChain = Chain.GetObject(i);
						bool changed = false;
						for(int j=0; j<SubChain.nbranches(); j++) {
							if(SubChain.Branch(j).Object().abelian()) {
								label.append(SubChain.Branch(j).label());
								label.push_back('+');
								// delete it so that we can compare the semisimple bits
								SubChain.DeleteBranch(j);
								j--;
								changed = true;
							}
						}
						if(changed)
						{
							Chain.DeleteTerm(i);
							Chain.InsertTerm(i,SubChain);
						}
					}
					
					
					// Compare the semisimple bits and store the labels where they belong
					int n;
					if((n = SemisimpleParts.Index(Chain)) == -1) {
						SemisimpleParts.AddTerm(Chain);
						Labels.AddTerm(label);
					} else {
						if(Labels.GetObject(n).find(label) == std::string::npos) {
							label.append(Labels.GetObject(n));
							Labels.DeleteTerm(n);
							Labels.InsertTerm(n, label);
						}
					}
				}			
			}
			
			
		//	std::cout << SemisimpleParts << std::endl;
		//	std::cout << Labels << std::endl;
			
			
			// Iterate again over the chains, now to change the label of the abelians
			int count = 0;
			for(List<List<Tree<SimpleGroup> > >::iterator it_BrChains = BrChains.begin(); it_BrChains != BrChains.end(); it_BrChains++) {
				//std::cout << "count = " << count << std::endl;
				List<Tree<SimpleGroup> > Chain = *it_BrChains;
					
				LieGroup Supergroup = Chain.GetObject(0).Level(0);
				LieGroup Group = Chain.GetObject(0).Level(1);
				for(int i=1; i<Chain.nterms(); i++) {
					Supergroup.AddTerm(Chain.GetObject(i).Level(0));
					if(Chain.GetObject(i).depth() > 1) {
						Group.AddTerm(Chain.GetObject(i).Level(1));
					}
				}
						
				//std::cout << Supergroup << std::endl;
				//std::cout << Group << std::endl;
				
				// Check whether there is rank reduction and, if so, mix the abelians
				bool change = false;
				if(Supergroup.rank() > Group.rank() and Group.nabelians() > 0) {
					
					// Get the semisimple reference first
					List<Tree<SimpleGroup> > SemisimpleChain = Chain;
					for(int i=0; i<SemisimpleChain.nterms(); i++) {
						Tree<SimpleGroup> SubChain = SemisimpleChain.GetObject(i);
						for(int j=0; j<SubChain.nbranches(); j++) {
							if(SubChain.Branch(j).Object().abelian()) {
								SubChain.DeleteBranch(j);
							}
						}
						SemisimpleChain.DeleteTerm(i);
						SemisimpleChain.InsertTerm(i,SubChain);
					}
					
					std::string label = Labels.GetObject(SemisimpleParts.Index(SemisimpleChain));
					label.pop_back();
					
					for(int i=0; i<Chain.nterms(); i++) {
						Tree<SimpleGroup> SubChain = Chain.GetObject(i);
						// If there is an abelian, change it's label
						for(int j=0; j<SubChain.nbranches(); j++) {
							if(SubChain.Branch(j).Object().abelian()) {
								Tree<SimpleGroup> AbelianTree(SubChain.Branch(j));
								AbelianTree.setLabel(label);
								SubChain.DeleteBranch(j);
								SubChain.InsertBranch(j,AbelianTree);
							}
						}
						// If there is no branches or there is a branch missing, add it
						if(SubChain.nbranches() == 0 or SubChain.Object().rank() > LieGroup(SubChain.Level(1)).rank()) {
							Tree<SimpleGroup> AbelianTree(SimpleGroup("U1"));
							AbelianTree.setLabel(label);
							SubChain.AddBranch(AbelianTree);
						}
						
						Chain.DeleteTerm(i);
						Chain.InsertTerm(i,SubChain);
					}
					
					change = true;
				}
				
				//std::cout << Chain << std::endl;
							
				// If there was any change, replace the chain
				if(change) {
					BrChains.DeleteTerm(count);
					BrChains.InsertTerm(count, Chain);
				}
				count ++;
				
			}
			BrChains.EliminateRepeated();
			
			//std::cout << BrChains << std::endl;
			
			return BrChains;
			
		} catch (...) {
			throw;
		}
	}

	/* Returns the list of possible invariant products of the representations of the group at up dimension operator dim */
	List<Product<Rrep> > LieGroup::Invariants(const List<Rrep> &reps, const int dim) {
		try {
			if(dim < 1) throw "LieGroup::Invariants::Wrong dimension";
			
			List<Product<Rrep> > Products;
			List<Rrep> Reps(reps);
			
			int factorial = 1;
			for(List<Rrep>::iterator it_Reps = Reps.begin(); it_Reps != Reps.end(); it_Reps++) {
				Products.AddTerm(Product<Rrep>(*it_Reps));
				factorial *= it_Reps->dim();
			}
			
			// Reserve enough space for the Products
			Products.reserve(factorial);
			
			//std::cout << Products << std::endl;

			for(List<Product<Rrep> >::iterator it_Products = Products.begin(); it_Products != Products.end() and it_Products->nterms() < dim; it_Products++) {
				//std::cout << it_Products->GetObject(-1) << std::endl;
				for(List<Rrep>::iterator it_Reps = Reps.begin(); it_Reps != Reps.end() and it_Reps->dim() >= it_Products->GetObject(-1).dim(); it_Reps++) {
					Product<Rrep> Prod(*it_Products);
					Prod.AddTerm(*it_Reps);
					//std::cout << Prod << std::endl;
					if(Products.Index(Prod) == -1) {
						Products.AddTerm(Prod);
					}
				}
			}
			
			//Products.Reverse();
			//std::cout << Products << std::endl;

			List<Product<Rrep> > Invariants;
			
			for(List<Product<Rrep> >::iterator it_Products = Products.begin(); it_Products != Products.end(); it_Products++) {
				//std::cout << *it_Products << std::endl;
				Sum<Rrep> Prod(Rrep::Product(*it_Products));
				Prod.Order("ASC");
				//std::cout << Prod << std::endl;
				if(Prod.GetObject(0).isSinglet()) {
					Invariants.AddTerm(*it_Products);
				}
			}
			
			return Invariants;
			
		} catch (...) {
			throw;
		}
	}

	/* Returns whether the Lie group is a subgroup of another Lie Group */
	bool LieGroup::isSubgroupOf(LieGroup Group) const {

		try {
			if(this->rank() > Group.rank() or this->dim() > Group.dim()) {
				return false;
			}
			
			List<SubGroup> Subgroups = Group.Subgroups(rank(), Group.rank());
			List<LieGroup> Subgroups2;

			for(List<SubGroup>::iterator it_Subgroups = Subgroups.begin(); it_Subgroups != Subgroups.end(); it_Subgroups++)
				Subgroups2.AddTerm(*it_Subgroups);
			
			if(Subgroups2.Index(*this) == -1) {
				return false;
			} else {
				return true;
			}
		
			return false;
		} catch (...) {
			throw;
		}
	}

	/* Returns whether the Lie group is a subgroup of another Simple Group */
	bool LieGroup::isSubgroupOf(SimpleGroup Group) const {

		try {
			return isSubgroupOf(LieGroup(Group));
		} catch (...) {
			throw;
		}
	}

	/* Returns the simple representations of the Lie group */
	List<Rrep> LieGroup::SimpleReps() {

		try {
			List<Rrep> Reps;
		
			List< List<Irrep> > ListofSimpleReps;
		
			int nirreps = 1;
		
			for(int i=0; i<this->ngroups(); i++) {
				ListofSimpleReps.AddTerm(List<Irrep>(this->GetObject(i).SimpleReps()));
				nirreps *= ListofSimpleReps.GetObject(i).nterms();
			}
			
			for(int i=0; i<nirreps; i++) {
				Weight w(*this,this->rank());
				int reps = 1;
				for(int j=0; j<this->ngroups(); j++) {
					int thisreps = ListofSimpleReps.GetObject(j).nterms();
					reps *= thisreps;
					int index = (i*reps/nirreps)%thisreps;
					if(j==0){
						w = ListofSimpleReps.GetObject(0).GetObject(index).HWeight();
					} else {
						w.Append(ListofSimpleReps.GetObject(j).GetObject(index).HWeight());
					}
				}
				//Rrep Rep(this,w);
				Reps.AddTerm(Rrep(*this,w));

			}
		
			return Reps;
			
		} catch (...) {
			throw;
		}
	}

	/* Returns the generating rep of the Lie group */
	Rrep LieGroup::GeneratingRep() {
		try {
			Rrep Rep(Irrep(this->GetObject(0).GeneratingRep()));
			for(int i=1; i<this->ngroups(); i++) {
				Rep.AddIrrep(Irrep(this->GetObject(i).GeneratingRep()));
			}
			return Rep;
		} catch (...) {
			throw;
		}
	}

	/* Returns all the representations of the group up to dimension maxdim */
	List<Rrep> &LieGroup::Reps(int maxdim) {
		
		try {
			// If the reps are calculated already
			if(_Reps.nterms() and _repsMaxDim >= maxdim) {
				int i = _Reps.nterms()-1;
				while(_Reps.GetObject(i).dim() > maxdim) {
					_Reps.DeleteTerm(i);
					i--;
				}
				return _Reps;
			}
			
			List<Rrep> Reps;
			
			// If info is already on the db, pull it
			if(database_check(id(), "Reps") and DataBase.at(id()).hasReps() and DataBase.at(id()).repsMaxDim() >= maxdim) {
				_Reps = DataBase.at(id()).Reps(maxdim);
				if(_Reps.nterms()) _hasReps = true;
				_repsMaxDim = maxdim;
				return _Reps;
			}
			
			
			// If it is a simple group, calculate irreps
			if(ngroups() == 1) {
				List<Irrep> Irreps = GetObject(0).Irreps(maxdim);
				for(List<Irrep>::iterator it_Irreps = Irreps.begin(); it_Irreps != Irreps.end(); it_Irreps++)
					Reps.AddTerm(Rrep(*it_Irreps)); 
				_Reps = Reps;
				
				// Set th hasReps flag to true and repsMaxDim to the current maxdim
				if(_Reps.nterms()) _hasReps = true;
				_repsMaxDim = maxdim;
			
				// If there is an entry in the database delete it and dump this
				database_emplace(id(), *this);
				
				return _Reps;
			}

			//std::cout << "Calculating Reps of " << *this << std::endl;
			// If not, calculate it


			List<List<Rrep> > ListofReps;
		
			LieGroup Group = *this;
			Group.DeleteTerm(Group.nterms()-1);
			ListofReps.AddTerm(Group.Reps(maxdim));

			Group = this->GetObject(this->nterms()-1);
			ListofReps.AddTerm(Group.Reps(maxdim));
				
			int nirreps = ListofReps.GetObject(0).nterms()*ListofReps.GetObject(1).nterms();
		
			//std::cout << ListofReps << std::endl;

			for(int i=0; i<nirreps; i++) {
				//std::cout << "i = " << i << std::endl;
				Weight w;
				int reps = 1;
				int dim = 1;
				//for(int j=0; j<this->ngroups(); j++) {
				for(int j=0; j<2; j++) {
					//std::cout << "j = " << j << std::endl;
					int thisreps = ListofReps.GetObject(j).nterms();
					reps *= thisreps;
					int index = (i*reps/nirreps)%thisreps;
					if(j==0){
						w = ListofReps.GetObject(0).GetObject(index).HWeight();
					} else {
						w.Append(ListofReps.GetObject(j).GetObject(index).HWeight());
					}
					dim *= ListofReps.GetObject(j).GetObject(index).dim();
				}
				
				if(dim <= maxdim) {
					Rrep Rep(*this,w);
					//std::cout << Rep << std::endl;
					Reps.AddTerm(Rep);
				}
			}
			_Reps = Reps;
			//std::cout << "Reps = " << Reps << std::endl;
			
			// Set th hasReps flag to true and repsMaxDim to the current maxdim
			if(_Reps.nterms()) _hasReps = true;
			_repsMaxDim = maxdim;
			
			// If there is an entry in the database delete it and dump this
			database_emplace(id(), *this);
		
			return _Reps;
			
		} catch (...) {
			throw;
		}
		
	}

	/* Returns the variable Reps */
	List<Rrep> LieGroup::RepsConst() const {
		return _Reps;
	}
	
	/* Transform a list of irreps into a list of reps (if the LieGroup is simple) */
	List<Rrep> LieGroup::Irreps2Reps(List<Irrep> Irreps) {
		try 
		{
			if(ngroups() != 1) throw "LieGroup::Irreps2Reps::Operation only permitted on simple groups";
			
			List<Rrep> Reps;
			for(auto it = Irreps.begin(); it != Irreps.end(); it++) Reps.AddTerm(Rrep(*it));
			
			return Reps;
		}
		catch (...)
		{
			throw;
		}
	}
		
	/* Overloaded > operator with LieGroups */
	bool LieGroup::operator>(const LieGroup &Group) {
		if(this->rank() > Group.rank()) {
			return true;
		} else if(this->rank() == Group.rank()) {
			if(this->dim() > Group.dim()) {
				return true;
			}
		}
		return false;
	}

	/* Overloaded > operator with SimpleGroups */
	bool LieGroup::operator>(const SimpleGroup &Group) {
		if(this->rank() > Group.rank()) {
			return true;
		} else if(this->rank() == Group.rank()) {
			if(this->dim() > Group.dim()) {
				return true;
			}
		}
		return false;
	}

	/* Overloaded < operator with LieGroups */
	bool LieGroup::operator<(const LieGroup &Group) {
		if(this->rank() < Group.rank()) {
			return true;
		} else if(this->rank() == Group.rank()) {
			if(this->dim() < Group.dim()) {
				return true;
			}
		}
		return false;
	}

	/* Overloaded < operator with SimpleGroups */
	bool LieGroup::operator<(const SimpleGroup &Group) {
		if(this->rank() < Group.rank()) {
			return true;
		} else if(this->rank() == Group.rank()) {
			if(this->dim() < Group.dim()) {
				return true;
			}
		}
		return false;
	}

	/* Overloaded == operator with LieGroups*/
	bool LieGroup::operator==(const LieGroup &Group) {
		try {

			if(this->ngroups() != Group.ngroups()) {
				return false;
			}
			this->Order();
			// Assume Group is ordered
			//Group.Order();

			if(Product<SimpleGroup>(*this) == Group) {
				return true;
			}
		
			/*for(int i=0; i<this->ngroups(); i++) {
				if(this->GetObject(i) != Group.GetObject(i)) {
					return false;
				}
			}*/
		
			return false;
			
		} catch (...) {
			throw;
		}
	}

	/* Overloaded == operator with SimpleGroups */
	bool LieGroup::operator==(const SimpleGroup &Group) {
		try {
			if(this->ngroups() != 1) {
				return false;
			}
		
			if(this->GetObject(0) != Group) {
				return false;
			}
		
			return true;
		} catch (...) {
			throw;
		}
	}

	/* Overloaded != operator with Liegroups */
	bool LieGroup::operator!=(const LieGroup &Group) {
		try {
			return !((*this)==Group);
		} catch (...) {
			throw;
		}
	}

	/* Overloaded != operator with SimpleGroup */
	bool LieGroup::operator!=(const SimpleGroup &Group) {
		try {
			return !((*this)==Group);
		} catch (...) {
			throw;
		}
	}

	/* Order the elements of the group by rank */
	void LieGroup::Order() {
		
		try {
			Product<SimpleGroup>::Order();
			
			_Casimir.Clear();
			_label = "";

			for(LieGroup::iterator it = this->begin(); it != this->end(); it++) {
				_Casimir.AddTerm(it->Casimir());
				_label.append(it->label());
				if(std::next(it) != this->end())
					_label.append("x");
			}
		} catch (...) {
			throw;
		}
	}

	/* Returns the json string */
	JSONNode LieGroup::json(std::string name) const {
		
		if(name == "id") {
			return JSONNode("", id());
		}
		
		
		JSONNode json;
		
		if(ngroups() == 1) {
			json = GetObject(0).json(name);
			json.pop_back("hasReps");
			json.pop_back("Irreps");
			json.pop_back("hasSubgroups");
			json.pop_back("Subgroups");
			
		} else {
		
			json.push_back(JSONNode("id", id()));
			json.push_back(JSONNode("rank", _rank));
			json.push_back(JSONNode("dim", _dim));
			json.push_back(JSONNode("label", _label));
			json.push_back(JSONNode("simple",_simple));
			json.push_back(JSONNode("semisimple",_semisimple));
			json.push_back(JSONNode("ngroups",_ngroups));
			json.push_back(JSONNode("nabelians",_nabelians));
		
			JSONNode Casimir(JSON_ARRAY);
			Casimir.set_name("Casimir");
			for(int i=0; i<_Casimir.nterms(); i++) {
				Casimir.push_back(JSONNode("",_Casimir.GetObject(i)));
			}
			json.push_back(Casimir);
			json.push_back(JSONNode("repsMaxDim", _repsMaxDim));
		
		}
		
		json.push_back(JSONNode("hasReps", _hasReps));
		json.push_back(JSONNode("hasSubgroups", _hasSubgroups));
		json.push_back(_Reps.json("Reps"));
		json.push_back(_Subgroups.json("Subgroups"));
		
		return json;
	}

	/* Parses a json object into the attributes of the class */
	void LieGroup::ParseJSON(const JSONNode & n, std::string what) {
		JSONNode::const_iterator i = n.begin();

		while (i != n.end()){
			// get the node name and value as a string
			std::string node_name = i -> name();
			
			// find out where to store the values
			if (node_name == "id") {
				std::string id = i->as_string();
				std::stringstream ss(id);
				//std::cout << "id = " << id << std::endl;
				if(!this->nterms()) {
					while(getline(ss, id, 'x')) {
						AddTerm(SimpleGroup(id));
					}
					if(nterms() == 1) {
						//std::cout << "problem?" << std::endl;
						SimpleGroup G(id);
						//std::cout << "problem here?" << std::endl;
						G.ParseJSON(n,what);
						//std::cout << "or here?" << std::endl;
						//this->~LieGroup();
						DeleteTerm(0);
						//std::cout << "probably here" << std::endl;
						AddTerm(G);

						//return ;
					}
				}
			} else if(node_name == "rank" and !_rank) {
				_rank = i->as_int();
			} else if(node_name == "dim" and !_dim) {
				_dim = i->as_int();
			} else if(node_name == "label" and _label == "") {
				_label = i->as_string();
			} else if(node_name == "simple") {
				_simple = i->as_bool();
			} else if(node_name == "semisimple") {
				_semisimple = i->as_bool();
			} else if(node_name == "ngroups") {
				_ngroups = i->as_int();
			} else if(node_name == "nabelians") {
				_nabelians = i->as_int();
			} else if(node_name == "Casimir" and !_Casimir.nterms()) {	
				_Casimir.Clear();
				JSONNode::const_iterator it = i->begin();
				while(it != i->end()) {
					_Casimir.AddTerm(it->as_float());
					++it;
				}
			} else if(node_name == "repsMaxDim") {
				_repsMaxDim = i->as_int();
			} else if(node_name == "hasReps" and what == "Reps") {
				_hasReps = i->as_bool();
			} else if(node_name == "hasSubgroups" and what == "Subgroups") {
				_hasSubgroups = i->as_bool();
			} else if(node_name == "Reps" and (what == "Reps")) {
				_Reps.ParseJSON(*i);
			} else if(node_name == "Irreps" and (what == "Reps")) {
				List<Irrep> Irreps;
				Irreps.ParseJSON(*i);
				_Reps = Irreps2Reps(Irreps);
			} else if(node_name == "Subgroups" and (what == "Subgroups")) {
				_Subgroups.ParseJSON(*i);
			}
		
			//increment the iterator
			++i;
		}
		

		return ;

	}

}
