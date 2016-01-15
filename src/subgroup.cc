/*
 * subgroup.cc
 * Created by T.Gonzalo on 15/02/2012.
 * Last modified on 30/11/2015
 */

#include "headers.h"

/**************************************************/
/*************Class SubGroup methods***************/
/**************************************************/

namespace Tomb
{
	// Definition of static variables and helper functions
	
	std::map<std::string, SubGroup> SubGroup::DataBase;
	std::map<std::string, JSONNode> SubGroup::JSONDataBase;
	
	bool SubGroup::database_check(std::string id) {
		if(DataBase.find(id) != DataBase.end())
			return true;
		if(JSONDataBase.find(id) != JSONDataBase.end()) {
			SubGroup G(JSONDataBase.at(id));
//			std::cout << G << std::endl;
			DataBase.emplace(id, G);
			return true;
		}
		return false;
	}
	
	void SubGroup::database_emplace(std::string id, SubGroup G) {
		if(database_check(id))
			DataBase.erase(id);
		DataBase.emplace(id, G);
	}

	
	// Member functions

	/* Constructor, empty */
	SubGroup::SubGroup() : LieGroup() {

	}

	/* Constructor 1 */
	SubGroup::SubGroup(const LieGroup &SuperGroup) : LieGroup() {

		try {
			char label = 'A';
			for(int i=0; i<SuperGroup.nterms(); i++) {
				Tree<SimpleGroup> tree(SuperGroup.GetObject(i));
				tree.setLabel(std::string(1,label));
				label++;
				_SuperGroups.AddTerm(tree);
				
			}
			//_Projection = 0;
			
			_maximal = false;
			_regular = false;
			_special = false;
		} catch (...) {
			throw;
		}
	}

	/* Constructor 2 */
	SubGroup::SubGroup(const SimpleGroup &SuperGroup) : LieGroup() {
		try {
			Tree<SimpleGroup> tree(SuperGroup);
			tree.setLabel("");
			_SuperGroups.AddTerm(tree);
			//_Projection = 0;
			_maximal = false;
			_regular = false;
			_special = false;
		} catch (...) {
			throw;
		}
	}

	/* Constructor 3, with strings */
	SubGroup::SubGroup(const std::string id) {

		try {
			init(id);
		} catch (...) {
			throw;
		}
		
	}

	/* Constructor 3.5, with strings and mixings */
	SubGroup::SubGroup(const std::string id, Matrix<double> Mixing) {
		
		try {
			
			// If there is no mixing initialise as usual
			if(Mixing.cols() <= 1) {
				init(id);
				return ;
			}
			
			std::stringstream ss(id);
			std::string SGstr, Gstr;

			getline(ss, Gstr, '[');
			getline(ss, SGstr, ']');
			//std::cout << SGstr << std::endl;
		
			std::stringstream ss2(Gstr);
			std::string Gstr2;
		
			List<std::string> ids;
			List<List<double> > mixings;
			List<double> R;
			List<std::string> mixlabels;
			
			int semisimplerank = 0;
		
			// Extract the list of subgoups to parse
			int count = 0, abelians = 0;
			while(getline(ss2, Gstr2,  'x')) {
				std::stringstream ss3(Gstr2);
				std::string Gstr3;
				getline(ss3, Gstr3, '(');
				if(Gstr3 == "U1") {
					std::string labels;
					getline(ss3, labels, ')');
					for(int i=0; i<ids.nterms(); i++) {
						//std::cout << " i = " << i << std::endl;
						std::stringstream mixss;
						mixss << ids.GetObject(i);
						ids.DeleteTerm(i);
						R = mixings.GetObject(i);
						mixings.DeleteTerm(i);
						std::stringstream ss4(labels);
						std::string label;
						int count = 0;
						while(getline(ss4, label, '+')) {
							std::stringstream mixss2;
							mixss2 << mixss.str();
							mixss2 << "xU1(" << label << ")";
							ids.InsertTerm(i,mixss2.str());
							List<double> R2(R);
							R2.AddTerm(Mixing[abelians][count]);
							mixings.InsertTerm(i,R2);
							i++;
							count++;
						}
						abelians ++ ;
					}
					mixlabels.AddTerm(labels);			
				} else {
					semisimplerank += SimpleGroup(Gstr3).rank();
					std::stringstream ss4;
					ss4 << Gstr3;
					getline(ss3, Gstr3, ')');
					ss4 << '(' << Gstr3 << ')';
					if(ids.nterms() == 0) {
						ids.AddTerm(ss4.str());
						R.AddTerm(1);
						mixings.AddTerm(R);
					} else {
						for(int i=0; i<ids.nterms(); i++) {
							std::stringstream mixss;
							mixss << ids.GetObject(i);
							mixss << "x" << ss4.str();
							ids.DeleteTerm(i);
							ids.InsertTerm(i,mixss.str());
							R = mixings.GetObject(i);
							R.AddTerm(1);
							mixings.DeleteTerm(i);
							mixings.InsertTerm(i,R);
							
						}
					}
					mixlabels.AddTerm(Gstr3);
				}
				//std::cout << ids << std::endl;
				//std::cout << mixings << std::endl;
			}
		
			// Finish the ids by adding the supergroup
			for(int i=0; i<ids.nterms(); i++) {
				std::stringstream idsstream;
				idsstream << ids.GetObject(i) << '[' << SGstr << ']';
				ids.DeleteTerm(i);
				ids.InsertTerm(i, idsstream.str());
			}
			
			//std::cout << ids << std::endl;
			//std::cout << mixings << std::endl;
		
			// If there is no U(1) mixing, initialise as usual
			if(ids.nterms() == 1) {
				init(id);
				return ;
			}
		
			// Check wether the mixing matrix has the proper shape for all mixings
			int size = 0;
			/*for(int i=0; i<Mixing.rows(); i++) {
				for(int j=0; j<Mixing.cols(); j++) {
					if(Mixing[i][j] != 0) size++;
				}
			}*/
			size = Mixing.cols();
			//std::cout << ids.nterms() << std::endl;
			//std::cout << Mixing << std::endl;	
			if(size != ids.nterms()) {
				throw "SubGroup::SubGroup::Mixing vector of the wrong size";
			}
		
			// Create the subgroup by copying the first on the list of labels
			init(ids.GetObject(0));
		
			// Change the labels
			_labels = mixlabels;
			
			// Change the supergroups
			// Don't do it now, cause it's a pain, maybe we won't need it
					
			// Change the projection matrix
			_Projection = 0;
			for(int i=0; i<ids.nterms(); i++) {
				SubGroup Subgroup(ids.GetObject(i));
				//std::cout << "Subgroup Projection " << std::endl << Subgroup.Projection() << std::endl;
				Matrix<double> MixMatrix(_Projection.rows());
				MixMatrix = MixMatrix.Identity(_Projection.rows());
				for(int j=0; j<_Projection.rows()-semisimplerank; j++) {
					MixMatrix[_Projection.rows()-j-1][_Projection.rows()-j-1] = ids.nterms()*mixings.GetObject(i).GetObject(-j-1);
				}
				//std::cout << "MixMatrix" << std::endl << MixMatrix << std::endl;
				_Projection += MixMatrix*Subgroup.Projection()/ids.nterms();
			}
					
			//std::cout << "Projection" << std::endl << Projection() << std::endl;
			
			// If there is an entry in the database delete it and dump this
			database_emplace(id, *this);
		
		} catch (...) {
			throw;
		}
	}

	/* Constructor 4, with json nodes */
	SubGroup::SubGroup(const JSONNode &n) {
		try {
			if(n.as_string() != "") {
				init(n.as_string());
			} else {
				ParseJSON(n);
			}
		} catch (...) {
			throw;
		}
	}

	/* Copy constructor 0 */
	SubGroup::SubGroup(const SubGroup &Group) : LieGroup(Group) {
		try {
			_SuperGroups = Group.SuperGroups();
			_Projection = Matrix<double>(Group.Projection());
			_labels = Group.labels();
			_maximal = Group.maximal();
			_regular = Group.regular();
			_special = Group.special();
		} catch (...) {
			throw;
		}
	}

	/* Copy constructor 1 */
	SubGroup::SubGroup(const LieGroup &Group, const LieGroup &SuperGroup) : LieGroup(Group) {
		try {
			
			if(Group != SuperGroup) {
				throw "SubGroup::SubGroup::Nothing to do here";
			} else {
				char label = 'A';
				for(int i=0; i<Group.ngroups(); i++) {
					Tree<SimpleGroup> tree(SuperGroup.GetObject(i));
					tree.setLabel(std::string(1,label));
					label++;
					tree.AddBranch(Tree<SimpleGroup>(Group.GetObject(i)));
					_SuperGroups.AddTerm(tree);
				}
			}
			
			_Projection = Matrix<double>(this->rank(), this->SuperGroup().rank());
			_Projection = 1;
			
			/////////////////////
			// Temporary trial //
			std::string label = "A";
			_labels.AddTerm(label);
			for(int i=1; i<Group.nterms(); i++) {
				label[label.length()-1] ++;
				_labels.AddTerm(label);
			}
			/////////////////////
			
			if(rank() == SuperGroup.rank()) {
				_maximal = true;
			} else {
				_maximal = false;
			}
			_regular = true;
			_special = false;
			
			database_emplace(id(),*this);

		} catch (...) {
			throw;
		}
	}

	/* Copy constructor 2 */
	SubGroup::SubGroup(const LieGroup &Group, const SimpleGroup &SuperGroup) : LieGroup(Group) {
		try {
			Tree<SimpleGroup> tree(SuperGroup);
			tree.setLabel("A");
			for(int j=0; j<Group.nterms(); j++) {
				tree.AddBranch(Tree<SimpleGroup>(Group.GetObject(j)));
			}
			_SuperGroups.AddTerm(Tree<SimpleGroup>(SuperGroup));
			
			_Projection = Matrix<double>(this->rank(), this->SuperGroup().rank());
			_Projection = 1;
			
			/////////////////////
			// Temporary trial //
			std::string label = "A";
			_labels.AddTerm(label);
			for(int i=1; i<Group.nterms(); i++) {
				label[label.length()-1] ++;
				_labels.AddTerm(label);
			}
			/////////////////////
			
			if(rank() == SuperGroup.rank()) {
				_maximal = true;
			} else {
				_maximal = false;
			}
			_regular = true;
			_special = false;
			
			database_emplace(id(),*this);
			
		} catch (...) {
			throw;
		}
	}

	/* Copy constructor 4 */
	SubGroup::SubGroup(const SimpleGroup &Group, const SimpleGroup &SuperGroup) : LieGroup(Group) {
		try {
			
			Tree<SimpleGroup> tree(SuperGroup);
			tree.setLabel("A");
			if(Group != SuperGroup) {
				tree.AddBranch(Tree<SimpleGroup>(Group));
			}
			_SuperGroups.AddTerm(tree);
			
			_Projection = Matrix<double>(this->rank(), this->SuperGroup().rank());
			_Projection = 1;
			
			/////////////////////
			// Temporary trial //
			_labels.AddTerm("A");
			/////////////////////
					
			if(rank() == SuperGroup.rank()) {
				_maximal = true;
			} else {
				_maximal = false;
			}
			_regular = true;
			_special = false;
			
			database_emplace(id(),*this);
			
		} catch (...) {
			throw;
		}
	}

	/* Copy constructor 6 */
	SubGroup::SubGroup(const SubGroup &Group, const SimpleGroup &SuperGroup) : LieGroup(Group) {
		try {
			if(Group.SuperGroups().nterms() != 1 or Group.SuperGroup() != SuperGroup) {
				Tree<SimpleGroup> tree(SuperGroup);
				tree.setLabel("A");
				for(int i=0; i<Group.SuperGroups().nterms(); i++) {
					tree.AddBranch(Group.SuperGroups().GetObject(i));
				}
				_SuperGroups.AddTerm(tree);
			} else {
				_SuperGroups = Group.SuperGroups();
			}
			
			_Projection = Matrix<double>(Group.Projection());
			
			
			/////////////////////
			// Temporary trial //
			_labels = Group.labels();
			/////////////////////
			
			
			/*if(!SuperGroups().nterms() or Group.SuperGroup() < SuperGroup) {
				_SuperGroups.AddTerm(LieGroup(SuperGroup));
				if(SuperGroup.rank() > Group.SuperGroup().rank()) {
					_Projection.Append(Matrix<double>(rank(),SuperGroup.rank()-Group.SuperGroup().rank()),"RIGHT");
				}
			}*/
			
			int depth = 0;
			for(int i=0; i<SuperGroups().nterms(); i++) {
				if(depth < SuperGroups().GetObject(i).depth()) {
					depth = SuperGroups().GetObject(i).depth();
				}
			}
			if(rank() == SuperGroup.rank() and depth == 2) {
				_maximal = true;
			} else {
				_maximal = false;
			}
			_regular = Group.regular();
			_special = Group.special();
			
			database_emplace(id(),*this);
			
		} catch (...) {
			throw;
		}
	}

	/* Copy constructor 7 */
	SubGroup::SubGroup(const SubGroup &Group, const SubGroup &SuperGroup) : LieGroup(Group) {
		try {
			
			//std::cout << "===================" << std::endl;
			//std::cout << Group << std::endl;
			
			_SuperGroups = SuperGroup.SuperGroups();
			//std::cout << _SuperGroups.json().write_formatted() << std::endl;
			//std::cout << Group.SuperGroups().json().write_formatted() << std::endl;
			
			bool replace = true;
			for(int i=0; i<Group.SuperGroups().nterms(); i++) {
				if(Group.SuperGroups().GetObject(i).nbranches()) {
					if(Group.SuperGroups().GetObject(i).Level(0) != Group.SuperGroups().GetObject(i).Level(1)) {
						replace = false;
					}
				}
			}
			
			int count = 0;
			for(int i=0; i<_SuperGroups.nterms(); i++) {
				Tree<SimpleGroup> tree = _SuperGroups.GetObject(i);
				for(int j=0; j<tree.nleaves(); j++) {
					if(tree.Leaf(j).level() > 0) {
						if(Group.nterms() != SuperGroup.nterms() and replace) {
							if(Group.SuperGroups().GetObject(count).nbranches()) {
								//std::cout << "replacing business" << std::endl;
								//std::cout << Group.SuperGroups().GetObject(count).Branch(0).label() << std::endl;
								tree.ReplaceLeaf(j, Group.SuperGroups().GetObject(count).Branches());
								//std::cout << tree.Leaf(j).label() << std::endl;
								j += Group.SuperGroups().GetObject(count).nleaves() - 1;
							} else {
								//std::cout << "deleting business" << std::endl;
								int nleaves = tree.nleaves();
								do {
									tree.DeleteLeaf(j);
									if(nleaves > tree.nleaves()) j--;
								} while (tree.nleaves() == nleaves and tree.Leaf(j).level() > 0);
							}
						} else {
							if(Group.SuperGroups().GetObject(count).nbranches()) {
								//std::cout << "attaching business" << std::endl;
								tree.AttachToLeaf(j, Group.SuperGroups().GetObject(count).Branches());
								j += Group.SuperGroups().GetObject(count).nleaves() - 1;
								//std::cout << j << std::endl;
							} else {
								//std::cout << "deleting business" << std::endl;
								int nleaves = tree.nleaves();
								do {
									tree.DeleteLeaf(j);
									if(nleaves > tree.nleaves()) j--;
								} while (tree.nleaves() == nleaves and tree.Leaf(j).level() > 0);
							}
						}	
						count ++;
					}
					//std::cout << tree.json().write_formatted() << std::endl;
				}
				if(tree != _SuperGroups.GetObject(i)) {
					_SuperGroups.DeleteTerm(i);
					_SuperGroups.InsertTerm(i,tree);
				}
			}	
			
			_Projection = Matrix<double>(Group.Projection()*SuperGroup.Projection());

			int depth = 0;
			for(int i=0; i<_SuperGroups.nterms(); i++) {
				if(depth < _SuperGroups.GetObject(i).depth()) {
					depth = _SuperGroups.GetObject(i).depth();
				}
			}
			if(depth == 2) {
				_maximal = true;
			} else {
				_maximal = false;
			}
			_regular = Group.regular();
			_special = Group.special();
			
			/////////////////////
			// Temporary trial //
			if(Group.SuperGroups().nterms() > 1) {
				for(int i=0; i<Group.nterms(); i++) {
					char where = Group.labels().GetObject(i)[0];
					where -= 'A';
					std::string label = SuperGroup.labels().GetObject(int(where));
					label.append(Group.labels().GetObject(i).substr(1));
					_labels.AddTerm(label);
				}
			} else {
			
				_labels = Group.labels();
			}
			/////////////////////
			
			database_emplace(id(),*this);
				
		} catch (...) {
			throw;
		}
	}

	/* Move constructor */
	SubGroup::SubGroup(SubGroup &&Group) : 
		LieGroup(std::move(Group)),
		_SuperGroups(std::move(Group._SuperGroups)),
		_Projection(std::move(Group._Projection)),
		_labels(std::move(Group._labels)),
		_maximal(std::move(Group._maximal)),
		_regular(std::move(Group._regular)),
		_special(std::move(Group._special))
	{
		try {
			Group._SuperGroups.Clear();
			Group._Projection = Matrix<double>();
			Group._labels.Clear();
			Group._maximal = false;
			Group._regular = false;;
			Group._special = false;
			
		} catch (...) {
			throw;
		}
	}

	/* Destructor */
	SubGroup::~SubGroup() {
	}

	/* Assignment operator */
	SubGroup &SubGroup::operator=(const SubGroup &Group) {

		try {
			if(this == &Group) return *this;
			
			LieGroup::operator=(Group);
			
			_SuperGroups = Group.SuperGroups();
			_Projection = Matrix<double>(Group.Projection());
			_labels = Group.labels();
			_maximal = Group.maximal();
			_regular = Group.regular();
			_special = Group.special();
			
			return *this;
			
		} catch (...) {
			throw;
		}
		
	}

	/* Move assignment operator */
	SubGroup &SubGroup::operator=(SubGroup &&Group) {

		try {
			if(this == &Group) return *this;
			
			LieGroup::operator=(std::move(Group));
			
			_SuperGroups = std::move(Group._SuperGroups);
			_Projection = std::move(Group._Projection);
			_labels = std::move(Group._labels);
			_maximal = std::move(Group._maximal);
			_regular = std::move(Group._regular);
			_special = std::move(Group._special);
			
			
			Group._SuperGroups.Clear();
			Group._Projection = Matrix<double>();
			Group._labels.Clear();
			Group._maximal = false;
			Group._regular = false;;
			Group._special = false;
			
			return *this;
			
		} catch (...) {
			throw;
		}
		
	}

	/* Initialises some variables */
	void SubGroup::init(const std::string Id) {
		
		try {
			
			std::stringstream ss(Id);
			
			std::string SGstr, Gstr;
			
			getline(ss, Gstr, '[');
			getline(ss, SGstr, ']');
			
			if(Strings::split_string(Id,'+').nterms() > 1)
			{
				ss.str("");
				ss << Strings::split_string(Id,'+').GetObject(0) << ")[" << SGstr << "]";
			}
			
			getline(ss, Gstr, '[');
			getline(ss, SGstr, ']');
			
			std::string id = ss.str();
					
			if(database_check(id))
			{
				*this = DataBase.at(id);
			} else
			{
				LieGroup Supergroup(SGstr);
				Supergroup.Order();
				Supergroup.Subgroups();
				//if(FileExists(filepath.str())) {
				//	JSONNode json = libjson::parse(ReadFileString(filepath.str()));
				//	ParseJSON(json);
				if(database_check(id)) {
					*this = DataBase.at(id);
				} else {
					
					std::stringstream ss2(Gstr);
					std::string str,label;
					
					SubGroup Subgroup(Supergroup);
					List<std::string> labels;
					
					while(getline(ss2, str, '(')) {
						Subgroup.AddTerm(SimpleGroup(str));
						getline(ss2, label, ')');
						labels.AddTerm(label);
						getline(ss2, str, 'x');
					}
					Subgroup.setLabels(labels);
					Subgroup.Order();
				
					//std::stringstream filepath;
					//filepath << "./out/" << Supergroup.id() << "/sub/" << Subgroup.id() << ".out";
					//std::cout << filepath.str() << std::endl;

					//if(FileExists(filepath.str())) {
					//	JSONNode json = libjson::parse(ReadFileString(filepath.str()));
					//	ParseJSON(json);
					if(database_check(id)) {
						*this = DataBase.at(Subgroup.id());
					} else {
						throw "SubGroup::init::Not a subgroup";
					}
				}
			}
			if(Strings::split_string(Id,'+').nterms() > 1)
			{
				std::string label = Strings::split_string(Strings::split_string(Id,'(').GetObject(-1),')').GetObject(0);
				
				List<std::string> oldlabels = labels();
				oldlabels.DeleteTerm(-1);
				oldlabels.AddTerm(label);
				setLabels(oldlabels);
			}
			
		} catch (...) {
			throw;
		}
	}

	/* Returns the list of Supergroups */
	List<Tree<SimpleGroup> > SubGroup::SuperGroups() const {
		return _SuperGroups;
	}

	/* Returns the hightest Supergroup */
	LieGroup SubGroup::SuperGroup() const {
		if(SuperGroups().nterms()) {
			LieGroup Supergroup;
			for(int i=0; i<SuperGroups().nterms(); i++) {
				Supergroup.AddTerm(SuperGroups().GetObject(i).Object());
			}
			Supergroup.Order();
			return Supergroup;
		} else {
			throw "SubGroup::SuperGroup::No supergroups";
		}
	}

	/* Returns the Supergroup at level i */
	LieGroup SubGroup::SuperGroup(int i) const {
		try {
			LieGroup Supergroup;
			for(int j=0; j<SuperGroups().nterms(); j++) {
				if(SuperGroups().GetObject(j).nbranches()) {
					Supergroup.AddTerm(LieGroup(SuperGroups().GetObject(j).Level(i)));
				}
			}
			Supergroup.Order();
			return Supergroup;		
		} catch (...) {
			throw;
		}
	}

	/* Returns the Projection matrix */
	Matrix<double> SubGroup::Projection() const {	
		return _Projection;
	}

	/* Sets the projection matrix of a subgroup */
	Matrix<double> SubGroup::SetProjection(Matrix<double> Proj) {
		_Projection = Proj;
		return _Projection;
	}

	/* Returns the labels */
	List<std::string> SubGroup::labels() const {
		return _labels;
	}

	/* Set the labels */
	void SubGroup::setLabels(const List<std::string> labels) {
		_labels = labels;
	}

	/* Adds a simple group to the Subgroup */
	void SubGroup::AddTerm(const SimpleGroup &Group, int where) {

		try {
			// Delete the entry from te DB
			std::string oldid = id();
			if(oldid != "" and database_check(oldid))
				DataBase.erase(oldid);

			LieGroup::AddTerm(Group);

			int superrank = this->SuperGroup().rank();
			if(Group.rank() == superrank) {
				_maximal = true;
			}
			if(where < _SuperGroups.nterms()) {
				Tree<SimpleGroup> Object = _SuperGroups.GetObject(0);
				//if(Object.Object() != Group) {
					Object.AddBranch(Tree<SimpleGroup>(Group));
				//}
				_SuperGroups.DeleteTerm(0);
				_SuperGroups.InsertTerm(0,Object);
			} else {
				throw "SubGroup::AddTerm::Element doesn't exist";
			}
			
			if(!_Projection.rows() and !_Projection.cols()) {
				_Projection = Matrix<double>(Group.rank(),superrank);
			} else {
				_Projection.Append(Matrix<double>(Group.rank(), superrank));
			}
			
			/////////////////////
			// Temporary trial //
			std::string label = "";
			if(_labels.nterms() > 1) {
				label.append(_labels.GetObject(-1));
				label[label.length()-1]++;
			} else if(_labels.nterms() == 1) {
				_labels.DeleteTerm(0);
				label.append("A");
				_labels.AddTerm(label);
				label[label.length()-1]++;
			}	
			_labels.AddTerm(label);
			/////////////////////
			
			database_emplace(id(),*this);
			
		} catch (...) {
			throw;
		}
	}

	/* Adds a liegroup to the Subgroup */
	void SubGroup::AddTerm(const LieGroup &Group, int where) {

		try {
			// Delete the entry from te DB
			std::string oldid = id();
			if(oldid != "" and database_check(oldid))
				DataBase.erase(oldid);
			
			int superrank = this->SuperGroup().rank();
				
			for(int i=0; i<Group.ngroups(); i++) {
				LieGroup::AddTerm(Group.GetObject(i));
			}
			
			if(where < _SuperGroups.nterms()) {
				Tree<SimpleGroup> Object = _SuperGroups.GetObject(where);
				for(int j=0; j<Group.ngroups(); j++) {
					//if(Object.Object() != Group.GetObject(j)) {
						Object.AddBranch(Group.GetObject(j));
					//}
				}
				_SuperGroups.DeleteTerm(where);
				_SuperGroups.InsertTerm(where,Object);
			} else {
				throw "SubGroup::AddTerm::Element doesn't exist";
			}
			
			if(!_Projection.rows() and !_Projection.cols()) {
				_Projection = Matrix<double>(rank(),superrank);
			}
			_Projection.Append(Matrix<double>(rank(), superrank));
			
			database_emplace(id(),*this);
			
			/////////////////////
			// Temporary trial // Unnecessary for now
			/*std::string label = "";
			if(SuperGroup().nterms() > 1) {
				label.append("A" + where);
			}
			if(_labels.nterms() > 1) {
				label.append(_labels.GetObject(-1));
				for(int i=0; i<Group.nterms(); i++) {
					label[label.length()-1]++;
					_labels.AddTerm(label);
				}
			} else if(_labels.nterms() == 1 and Group.nterms()) {
				_labels.DeleteTerm(0);
				label.append("A");
				_labels.AddTerm(label);
				for(int i=0; i<Group.nterms(); i++) {
					label[label.length()-1]++;
					_labels.AddTerm(label);
				}
			} else if(Group.nterms() > 1) {
				label.append("A");	
				_labels.AddTerm(label);
				for(int i=0; i<Group.nterms(); i++) {
					label[label.length()-1]++;
					_labels.AddTerm(label);
				}
			}*/
			/////////////////////
			
		} catch (...) {
			throw;
		}
	}

	/* Adds a subgroup to the Subgroup */
	void SubGroup::AddTerm(const SubGroup &Group, int where) {

		try {
			// Delete the entry from te DB
			std::string oldid = id();
			if(oldid != "" and DataBase.find(oldid) != DataBase.end())
				DataBase.erase(oldid);
			
			for(int i=0; i<Group.ngroups(); i++) {
				LieGroup::AddTerm(Group.GetObject(i));
			}
					
			if(where < _SuperGroups.nterms()) {
				Tree<SimpleGroup> Object = _SuperGroups.GetObject(where);
				for(int j=0; j<Group.ngroups(); j++) {
					//if(Object.Object() != Group.GetObject(j)) {
						Object.AddBranch(Group.GetObject(j));
					//}
				}
				_SuperGroups.DeleteTerm(where);
				_SuperGroups.InsertTerm(where,Object);
			} else {
				throw "SubGroup::AddTerm::Element doesn't exist";
			}
			
			if(!_Projection.rows() and !_Projection.cols()) {
				_Projection = Group.Projection();
			} else {
				Matrix<double> AuxProjection = Group.Projection();
				if(this->SuperGroup() > Group.SuperGroup()) {
					AuxProjection.Append(Matrix<double>(Group.rank(), _Projection.cols()),"LEFT");
				}
				_Projection.Append(Matrix<double>(_Projection.rows(), Group.SuperGroup().rank()),"RIGHT");
				_Projection.Append(AuxProjection);
			}
			
			/////////////////////
			// Temporary trial //
			std::string label = "";
			if(_labels.nterms()) {
				label = _labels.GetObject(_labels.nterms()-1)[0];
				label[0] ++;
			} else {
				label = "A";
			}
			
			if(Group.nterms() > 1) {
				label.append("A");	
				_labels.AddTerm(label);
				for(int i=1; i<Group.nterms(); i++) {
					label[label.length()-1]++;
					_labels.AddTerm(label);
				}
			} else {
				_labels.AddTerm(label);
			}
			/////////////////////
			
			if(DataBase.find(id()) != DataBase.end())
				DataBase.erase(id());
			DataBase.emplace(id(),*this);
			
		} catch (...) {
			throw;
		}
	}

	/* Deletes a subgroup of the Subgroup */
	void SubGroup::DeleteTerm(int del) {

		try {
			int i = del;
			if(del < 0)
				i = nterms() + i;
			int rank = 0;
			for(int j=0; j<i; j++){
				rank += GetObject(j).rank();
			}

			for(int j=0; j<GetObject(i).rank(); j++) {
				_Projection.DeleteRow(rank);
			}

			int count = 0;
			for(int j=0; j<_SuperGroups.nterms(); j++) {
				if(_SuperGroups.GetObject(j).nbranches() <=  i - count) {
					count += _SuperGroups.GetObject(j).nbranches();
				} else {
					Tree<SimpleGroup> tree = _SuperGroups.GetObject(j);
					tree.DeleteLeaf(i - count);
					_SuperGroups.DeleteTerm(j);
					_SuperGroups.InsertTerm(j,tree);
					count++;
				}
			}
			
			/////////////////////
			// Temporary trial //
			_labels.DeleteTerm(i);
			/////////////////////
			
			_maximal = false;
			
			LieGroup::DeleteTerm(i);
			
			if(DataBase.find(id()) != DataBase.end())
				DataBase.erase(id());
			DataBase.emplace(id(),*this);
			
		} catch(...) {
			throw;
		}
	}

	/* Order the groups in the subgroup and the projection matrix */
	void SubGroup::Order() {

		try {
		
			//std::cout << *this << std::endl;
			if(nterms() <= 1) {
				return ;
			}
			
			// Delete the entry from te DB
			std::string oldid = id();
			if(oldid != "" and DataBase.find(oldid) != DataBase.end())
				DataBase.erase(oldid);
		
			int row1 = 0;
			for(int i=0; i<nterms(); i++) {
				int rank1 = GetObject(i).rank();
				int toswap = i;
				int row2,auxrow = row1 + rank1;
				for(int j=i+1; j<nterms(); j++) {
					// Compare first the elements themselves and if they are equal compare their labels
					if(GetObject(toswap) < GetObject(j)) {
						toswap = j;
						row2 = auxrow;
					} else if (GetObject(toswap) == GetObject(j)) {
						std::string label1 = _labels.GetObject(toswap);
						std::string label2 = _labels.GetObject(j);
						int k=0;
						while(k < label1.length() and k < label2.length() and label1[k] == label2[k]) k++; 
						if(label1[k] > label2[k]) {
							toswap = j;
							row2 = auxrow;
						}
					}
					auxrow += GetObject(j).rank();
				}
				
				if(toswap != i) {
					int rank2 = GetObject(toswap).rank();
					SwapTerms(i,toswap);
					//std::cout << "row1 = " << row1 << ", row2 = " << row2 << std::endl;
					//std::cout << "rank1 = " << rank1 << ", rank2 = " << rank2 << std::endl;
					//std::cout << "Projection" << std::endl << _Projection << std::endl;
					_Projection.MoveRows(row2, row2+rank2-1, row1);
					//std::cout << "Projection" << std::endl << _Projection << std::endl;
					_Projection.MoveRows(row1+rank2, row1+rank2+rank1-1, row2+rank2-rank1);
					//std::cout << "Projection" << std::endl << _Projection << std::endl;
					_Casimir.SwapTerms(i, toswap);
					/////////////////////
					// Temporary trial //
					if(!_maximal or SuperGroup().nterms() > 1) {
						_labels.SwapTerms(i, toswap);
					}
					/////////////////////
					
					row1 += rank2;
				} else {
					row1 += rank1;				
				}
			}
			
			_label = this->Print();	
			
			if(DataBase.find(id()) != DataBase.end())
				DataBase.erase(id());
			DataBase.emplace(id(),*this);
			
		} catch (...) {
			throw;
		}
	}

	/* Returns whether the subgroup is a subgroup of another Lie Group */
	bool SubGroup::isSubgroupOf(LieGroup Group) const {

		try {
			if(this->abelian()) {
				return true;
			}
			if(this->rank() > Group.rank() or this->dim() > Group.dim()) {
				return false;
			} else {	

				if(SuperGroup() == Group && this->Projection() != 0) {
					return true;
				}
				
				if(SuperGroup().isSubgroupOf(Group)) {
					return true;
				}
				
				//This makes calculating subgroups recursive
				List<SubGroup> Subgroups = Group.Subgroups(rank(), Group.rank());
				if(Subgroups.Index(*this) == -1) {
					return false;
				} else {
					return true;
				}
			}
			return false;
		} catch (...) {
			throw;
		}
	}

	/* Returns whether the subgroup is a subgroup of another Lie Group */
	bool SubGroup::isSubgroupOf(SimpleGroup Group) const {

		try {
			return isSubgroupOf(LieGroup(Group));
		} catch (...) {
			throw;
		}
	}

	/* Returns whether the subgroup is maximal or not */
	bool SubGroup::maximal() const {
	return _maximal;
	}

	/* Sets the subgroup to be maximal */
	bool SubGroup::setMaximal(bool maximal) {
	_maximal = maximal;
	return _maximal;
	}

	/* Returns whether the subgroup is regular or not */
	bool SubGroup::regular() const {
	return _regular;
	}

	/* Sets the subgroup to be regular */
	bool SubGroup::setRegular(bool regular) {
		_regular = regular;
		return _regular;
	}

	/* Returns whether the subgroup is singular or not */
	bool SubGroup::special() const {
	return _special;
	}

	/* Sets the subgroup to be special */
	bool SubGroup::setSpecial(bool special) {
		_special = special;
		return _special;
	}

	/* Identifier of the Subgroup */
	std::string SubGroup::id() const {
		
		if(!nterms())
			return "";
		
		std::stringstream s;

		/*int depth = 0;
		for(int i=0; i<SuperGroups().nterms(); i++)
			if(SuperGroups().GetObject(i).depth() > depth)
				depth = SuperGroups().GetObject(i).depth();
		
		for(int i=0; i<SuperGroups().nterms(); i++) {
			//std::cout << SuperGroups().GetObject(i).json().write_formatted() << std::endl;
			//std::cout << SuperGroups().GetObject(i).nleaves() << std::endl;
			for(int j=0; j<SuperGroups().GetObject(i).nleaves(); j++) {
				if(SuperGroups().GetObject(i).Leaf(j).level() != 0) {
					s << SuperGroups().GetObject(i).Leaf(j).Object().id() << "(" << SuperGroups().GetObject(i).Leaf(j).label() << ")" << 'x';
				}
			}
		}
		
		std::string str = s.str();
		
		if(str[str.length()-1] == 'x') {
			str.pop_back();
		}*/
		
			
		/////////////////////
		// Temporary trial //
		for(int i=0; i<nterms()-1; i++) {
			s << GetObject(i).id() << "(" << _labels.GetObject(i) << ")" << "x";
		}
		s << GetObject(nterms()-1).id() << "(" << _labels.GetObject(nterms()-1) << ")";
		/////////////////////
		
		//std::cout << str << std::endl;
		//std::cout << SuperGroups() << std::endl;
		//std::cout << depth << std::endl;
		
		/*str.append("[");
		for(int i=depth-2; i>0; i--) {
			str.append(SuperGroup(i).id());
			str.append(",");
		}
		str.append(SuperGroup().id());
		str.append("]");*/
		
		/////////////////////
		// Temporary trial //
		s << "[" << SuperGroup().id() << "]";
		std::string str = s.str();
		/////////////////////
		
		
		//std::cout << str << std::endl;
		
		return str;
	}

	/* Overloaded == operator */
	bool SubGroup::operator==(const SubGroup &Group) const {
		
		try {
			//std::cout << id() << " == " << Group.id() << "?" << std::endl;
			//return false;
			//std::cout << 'a' << std::endl;
			if(this->ngroups() != Group.ngroups() or this->rank() != Group.rank()) {
				return false;
			}
			
			if(id() != Group.id()) return false;
			else return true;
			
			if(maximal() != Group.maximal() or regular() != Group.regular() or special() != Group.special()) {
				return false;
			}
			
		
			//std::cout << 'c' << std::endl;
			// Assume that groups are ordered 
			//this->Order();
			//Group.Order();
			
			//std::cout << 'd' << std::endl;
			if(LieGroup(*this) != LieGroup(Group)) {
				return false;
			}
		
			//std::cout << 'f' << std::endl;
			if(this->Projection() == Group.Projection()) {
				return true;
			}
			
			if(labels() == Group.labels()) {
				return true;
			}
			
			return false;
			
			//std::cout << 'g' << std::endl;
			if(SuperGroups() == Group.SuperGroups()) {
				return true;
			}
			
			// Speed things up a bit
			//std::cout << 'h' << std::endl;
			return false;
					
			
		} catch (...) {
			throw;
		}
	}

	/* Overloaded != operator */
	bool SubGroup::operator!=(const SubGroup &Group) const {
		try {
			return !((*this) == Group);
		} catch (...) {
			throw;
		}
	}
	
	/* Returns the json string */
	JSONNode SubGroup::json(std::string name) const {
		
		try {
			if(name == "id") {
				return JSONNode("", id());
			}
		
			JSONNode json = LieGroup::json();
			json.pop_back("id");
			json.push_back(JSONNode("id",id()));
			//std::cout << json.write_formatted() << std::endl;
			json.push_back(_SuperGroups.json("SuperGroups"));
			//std::cout << json.write_formatted() << std::endl;
			json.push_back(_Projection.json("Projection"));
			
			/////////////////////
			// Temporary trial //
			JSONNode labels(JSON_ARRAY);
			labels.set_name("labels");
			for(int i=0; i<_labels.nterms(); i++) {
				labels.push_back(JSONNode("",_labels.GetObject(i)));
			}
			json.push_back(labels);
			/////////////////////
			
			json.push_back(JSONNode("maximal",_maximal));
			json.push_back(JSONNode("regular",_regular));
			json.push_back(JSONNode("special",_special));
		
			return json;
		
		} catch (...) {
			throw;
		}
	}

	/* Parses a json object into the attributes of the class */
	void SubGroup::ParseJSON(const JSONNode & n, std::string what) {

		try {
			
			JSONNode json = n;
			//std::cout << json.write_formatted() << std::endl;
			std::string id = json.at("id").as_string();
			//std::cout << json.write_formatted() << std::endl;
			
			std::stringstream ss(id), ss2;
			
			while(getline(ss, id, '(')) {
				ss2 << id;
				getline(ss, id, ')');
			}
			getline(ss2,id,'[');
			//std::cout << id << std::endl;
			json.insert(json.begin(),JSONNode("id",id));
			
			
			//std::cout << json.write_formatted() << std::endl;
			//std::cout << "about to parse" << std::endl;
			LieGroup::ParseJSON(json,what);
			//std::cout << "lie group parsed" << std::endl;
			JSONNode::const_iterator i = json.begin();
			while (i != json.end()){
		
				// get the node name and value as a string
				std::string node_name = i -> name();

				// find out where to store the values
				if(node_name == "SuperGroups") {
						//std::cout << "parsing supergroups" << std::endl;
						//std::cout << i->write_formatted() << std::endl;
						_SuperGroups.ParseJSON(*i);
						//std::cout << "supergroups parsed" << std::endl;
						//std::cout << _SuperGroups << std::endl;
					} else if(node_name == "Projection") {
						_Projection.ParseJSON(*i);
					/////////////////////
					// Temporary trial //
					} else if(node_name == "labels" and !_labels.nterms()) {
						JSONNode::const_iterator it = i->begin();
						_labels.Clear();
						while(it != i->end()) {
							_labels.AddTerm(it->as_string());
							++it;
						}
					/////////////////////
					} else if(node_name == "maximal") {
						_maximal = i->as_bool();
					} else if(node_name == "regular") {
						_regular = i->as_bool();
					} else if(node_name == "special") {
						_special = i->as_bool();	
					}
		
				//increment the iterator
				++i;
			}

			if(DataBase.find(id) != DataBase.end())
				DataBase.erase(id);
			DataBase.emplace(id, *this);
			//std::cout << json().write_formatted() << std::endl;

			
		} catch (...) {
			throw;
		}

	}

}