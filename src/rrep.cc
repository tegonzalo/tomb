/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * rrep.cc
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 13/08/2013
 */

#include "rrep.h"
#include "liegroup.h"
#include "subgroup.h"

/**************************/
/* Class Rrep definitions */
/**************************/

namespace Tomb
{
  // Member functions
  
  /* Constructor 0 */
  Rrep::Rrep()
  {

  }

  /* Constructor 1*/
  Rrep::Rrep(LieGroup &Group, Weight &HWeight) : List<Irrep>()
  {
    try
    {
      if(Group.rank() != HWeight.cols())
        throw "Rrep::Rrep::Dimension of the weight doesn\'t match rank of the group";

      _Group = &Group;
      _HWeight = new Weight(HWeight);

      init();
      
    }
    catch (...) { throw; }
  }

  /* Constructor 2*/
  Rrep::Rrep(SubGroup &Group, Weight &HWeight) : List<Irrep>()
  {
    try
    {
      if(Group.rank() != HWeight.cols())
        throw "Rrep::Rrep::Dimension of weight doesn\'t match rank of group";

      _Group = &Group;
      _HWeight = new Weight(HWeight);
      
      init();
      
    }
    catch (...) { throw; }
  }

  /* Constructor 3 */
  Rrep::Rrep(LieGroup &Group, const Irrep &Rep) : List<Irrep>(Rep)
  {
    try
    {
      _Group = &Group;
      _HWeight = new Weight(Rep.HWeight());
      _dim = Rep.dim();
      _real = Rep.real();
      _nirreps = 1;
      _label = Rep.label();
      _Casimir.AddTerm(Rep.Casimir());
      _DynkinIndex.AddTerm(Rep.DynkinIndex());

      // If the rrep is already in the database, do nothing
      if(DB<Rrep>().find(id()) != NULL)
        return ;

      // Store the rrep in the database
      DB<Rrep>().set(id(), this);
      DB<Rrep>().at(id())->_Weights = Rep.Weights();
            
    }
    catch (...) { throw; }
  }

  /* Constructor 4, with strings */
  Rrep::Rrep(const std::string id)
  {   
    try
    {
      _HWeight = new Weight(id);
      _Group = DB<LieGroup>().get(_HWeight->Group().id());
      init();

    }
    catch (...) { throw; }	
  }

  /* Constructor 5, with JSON Nodes */
/*  Rrep::Rrep(const JSONNode &n)
    {
    if(n.as_string() != "")
    {
      _HWeight = new Weight(n.as_string());
      //_Group = new LieGroup(HWeight().Group());
      _Group = HWeight().GroupId();
      
      init();
    }
    else
    {
      ParseJSON(n);
    }
  }
*/
  /* Copy constructor */
  Rrep::Rrep(const Rrep &Rep) : List<Irrep>(Rep)
  {
    try
    {
      _Group = &Rep.Group();
      _HWeight = new Weight(Rep.HWeight());
      _dim = Rep.dim();
      _real = Rep.real();
      _nirreps = Rep.nirreps();
      _label = Rep.label();
      _Casimir = Rep.Casimir();
      _DynkinIndex = Rep.DynkinIndex();
      //_Weights = Rep.Weights();
    }
    catch (...) { throw; }
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
    _Weights(std::move(Rep._Weights))
  {
    try
    {		
      Rep._Group = NULL;
      Rep._HWeight = NULL;
      Rep._dim = 0;
      Rep._real = false;
      Rep._nirreps = 0;
      Rep._label = "";
      Rep._Casimir.Clear();
      Rep._DynkinIndex.Clear();
      Rep._Weights.Clear();
      
    }
    catch (...) { throw; }
  }

  /* Destructor */
  Rrep::~Rrep()
  {
    delete _HWeight;
  }

  /* Overloaded = operator */
  Rrep &Rrep::operator=(const Rrep &Rep)
  {
    try
    {
      if(this == &Rep) return *this;
      List<Irrep>::operator=(Rep);
      _Group = &Rep.Group();
      _HWeight = new Weight(Rep.HWeight());
      _dim = Rep.dim();
      _real = Rep.real();
      _nirreps = Rep.nirreps();
      _label = Rep.label();
      _Casimir = Rep.Casimir();
      _DynkinIndex = Rep.DynkinIndex();
      _Weights = Rep.Weights();
      return *this;
    }
    catch (...) { throw; }
  }

  /* Move = operator */
  Rrep &Rrep::operator=(Rrep &&Rep)
  {
    try
    {
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
      _Weights = std::move(Rep._Weights);
      
      Rep._Group = NULL;
      Rep._HWeight = NULL;
      Rep._dim = 0;
      Rep._real = false;
      Rep._nirreps = 0;
      Rep._label = "";
      Rep._Casimir.Clear();
      Rep._DynkinIndex.Clear();
      Rep._Weights.Clear();
      return *this;
    }
    catch (...) { throw; }
  }

  /* Initialises some internal variables */
  void Rrep::init()
  {   
    try
    {
      if(_Group == NULL or _HWeight == NULL)
        throw "Rrep::init::Not enough information to initialise the variables";

      _dim = 1;
      _nirreps = 0;
      int accumulated_rank = 0;
      for(int i=0; i<_Group->ngroups(); i++)
      {
        RVector<double> R = _HWeight->ExtractMatrix(0,0,accumulated_rank,accumulated_rank + _Group->GetObject(i).rank()-1).Row(0);
        Weight HW(_Group->GetObject(i), R);
        if(DB<Irrep>().check(HW.id()))
        {
          AddTerm(*DB<Irrep>().at(HW.id()));
          _dim *= DB<Irrep>().at(HW.id())->dim();
        }
        else
       {
          Irrep aRep = Irrep(_Group->GetObject(i),HW);
          AddTerm(aRep);
          _dim *= aRep.dim();
        }
        _nirreps++;
        accumulated_rank += _Group->GetObject(i).rank();
      }
      if(nirreps() > 1) {
        std::stringstream ss;
        ss << *this;
        _label = ss.str();
      } else {
        _label = GetObject(0).label();
      }
      _real = true;
      for(int i=0; i<nirreps(); i++)
      {
        if(!GetObject(i).real()) _real = false;
        _Casimir.AddTerm(GetObject(i).Casimir());
        _DynkinIndex.AddTerm(GetObject(i).DynkinIndex()*dim()/GetObject(i).dim());
      }

      // If the rrep is already in the database, do nothing
      if(DB<Rrep>().find(id()) != NULL)
        return ;
      // Store the info in the database
      DB<Rrep>().set(id(),this);

      // Perhaps do not calculate reps for very large reps     
      if(_dim < 1000)
        DB<Rrep>().at(id())->CalculateWeights();
    
    }
    catch(...) { throw; }	
  }

  /* Identifier of the Rrep */
  std::string Rrep::id() const
  {
    std::stringstream s;
    s << _HWeight->id();
    return s.str();
  }

  /* Returns the group */
  LieGroup &Rrep::Group() const 
  {
    return *_Group;
  }
  
  /* Returns the dimension */
  int Rrep::dim() const
  {
    return _dim;
  }

  /* Returns if the rep is real */
  bool Rrep::real() const 
  {
    return _real;
  }

  /* Returns the number of irreps */
  int Rrep::nirreps() const 
  {
    return _nirreps;
  }

  /* Returns the label */
  std::string Rrep::label() const
  {
    return _label;
  }

  /* Returns the Casimir of the representation */
  List<double> Rrep::Casimir() const
  {
    return _Casimir;
  }
  
  /* Returns the Dynkin Index of the representation */
  List<double> Rrep::DynkinIndex() const
  {
    return _DynkinIndex;
  }

  /* Returns the Height Weight */
  Weight &Rrep::HWeight() const
  {
    return *_HWeight;
  }

  /* Returns the weight list */
  List<Weight> Rrep::Weights() const
  {
    Rrep *R = DB<Rrep>().find(id());
    if(R != NULL)
      return R->_Weights;
    return _Weights;
  }

  /* Sets the weights */
  void Rrep::setWeights(const List<Weight> &Weights)
  {
    _Weights = Weights;
 
    if(DB<Rrep>().find(id()) != NULL)
      DB<Rrep>().at(id())->_Weights = Weights;
   
  }
  
  /* Calculates the weights of the representation */
  List<Weight> &Rrep::CalculateWeights()
  {   
    try
    {
      // If the weights have been calculated already take them
      if(_Weights.nterms()) return _Weights;
      _Weights.Clear();
      
      List< List<Weight> > ListofWeights;
      for(int i=0; i<this->nirreps(); i++)
        ListofWeights.AddTerm(DB<Irrep>().at(GetObject(i).id())->_Weights);
      
      int totalterms = 1;
      for(int i=0; i<ListofWeights.nterms(); i++)
        totalterms *= ListofWeights.GetObject(i).nterms();
      
      // Reserve enough space for the weights
      _Weights.reserve(_dim);
      
      for(int i=0; i<totalterms; i++)
      {
        Weight weight;
        int terms = 1;
        for(int j=0; j<ListofWeights.nterms(); j++)
        {
          int thisterms = ListofWeights.GetObject(j).nterms();
          terms *= thisterms;
          int index = (i*terms/totalterms)%thisterms;
          if(j==0)
            weight = Weight(_Group->GetObject(0),ListofWeights.GetObject(0).GetObject(index));
          else 
            weight.Append(ListofWeights.GetObject(j).GetObject(index));
        }
        _Weights.AddTerm(weight);
      }
      
      
      return _Weights;

    }
    catch (...) { throw; }
  }

  /* Adds a irrep of another group to the rep */
  Rrep Rrep::AddIrrep(Irrep Rep)
  {
    try
    {
      this->AddTerm(Rep);
      
      string groupid;
      groupid.push_back('x');
      groupid.append(Rep.Group().id());
      _Group = DB<LieGroup>().get(groupid);
      
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
    }
    catch (...) { throw; }
  }

  /* Adds a rep of another lie group to the rep */
  Rrep Rrep::AddRrep(Rrep Rep)
  {   
    try
    {
      this->AppendList(Rep);

      string groupid;
      groupid.push_back('x');
      groupid.append(Rep.Group().id());
    
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
    }
    catch (...) { throw; }
  }

  /* Finish the rrep */
  Rrep Rrep::FinishRrep()
  {
    try
    {
      // If the rrep is already in the database, do nothing
      if(DB<Rrep>().find(id()) != NULL)
        return *this;

      // Store the rep in the database
      DB<Rrep>().set(id(), this);
      DB<Rrep>().at(id())->CalculateWeights();

      return *this;

    }
    catch (...) { throw; }
  }

  /* Checks whether the rep is a singlet */
  bool Rrep::isSinglet(int which) const
  {
    if(which == -1)
    {
      if(dim() == 1)
      {
        for(int i=0; i<Group().nabelians(); i++)
          if((*_HWeight)[-i-1] != 0)
            return false;
        return true;
      }
    }
    else
      return GetObject(which).isSinglet();
    return false;
  }

  /* Projects the weights of a rrep into the weights of rreps of a subgroup */
  List<Weight> Rrep::Project(SubGroup &Subgroup)
  { 
    try
    {
      List<Weight> ProjectedWeights;
      Rrep *R = DB<Rrep>().find(id());
      if(R == NULL)
        throw "Rrep::Project::Rrep not in the database";

      for(auto it = R->_Weights.begin(); it != R->_Weights.end(); it++)
      {
        Weight weight = Weight(Subgroup, Subgroup.Projection()*(*it));
        weight.setPositive(true);
        for(int j=0; j<weight.cols()-Subgroup.nabelians(); j++)
        {
          if(weight[j] < 0)
            weight.setPositive(false);
        }
        ProjectedWeights.AddTerm(weight);
      }
    
      return ProjectedWeights;
    }
    catch (...) { throw; }
  }

  /* Obtains the decomposition into rreps of a subgroup */
  Sum<Rrep> Rrep::Decompose(SubGroup &Subgroup)
  {
    try
    {
      Rrep *R = DB<Rrep>().find(id());
      if(R == NULL)
        throw "Rrep::Decompose::Rrep not in the database";

      if(R != NULL and R->_Subreps.find(Subgroup.id()) != R->_Subreps.end())
        return R->_Subreps.at(Subgroup.id());

      if(!Subgroup.isSubgroupOf(*_Group))
        throw "Rrep::Decompose::Not a subgroup";

      Sum<Rrep> Reps;
      
      List<Weight> ProjectedWeights = Project(Subgroup);

     do
      {
        Weight HWeight = ProjectedWeights.GetObject(0);

        int highest_dim = 0;
        for(auto it = ProjectedWeights.begin(); it != ProjectedWeights.end(); it++)
        {
          int group = 0;
          int acc_rank = Subgroup.GetObject(0).rank();
            
          for(int j=0; j<Subgroup.rank(); j++)
          {
            if(j >= acc_rank)
            {
              group++;
              acc_rank += Subgroup.GetObject(group).rank();
            }
          }
          if(it->positive())
          {
            Rrep *Rep = DB<Rrep>().get(it->id());
            if(Rep->dim() > highest_dim)
            {
              highest_dim = Rep->dim();
              HWeight = *it;
            }
          }
        }
        Rrep *Rep = DB<Rrep>().find(HWeight.id());
        if(Rep == NULL)
        {
          Rep = new Rrep(Subgroup, HWeight);
          DB<Rrep>().set(Rep->id(), Rep);
        }
        int deleted =0;
        for(auto it = Rep->_Weights.begin(); it != Rep->_Weights.end(); it++)
        {
          int n = ProjectedWeights.Index(*it);
          if(n >= 0)
          {
            ProjectedWeights.DeleteTerm(n);
            deleted++;
          }
        }
        if(deleted != Rep->dim())
          throw "Rrep::Decompose::Error deleting terms from the projected weights";
        Reps.AddTerm(*Rep);
      }
      while(ProjectedWeights.nterms()>0);

      int repsdim = 0;
      for(auto it = Reps.begin(); it != Reps.end(); it++)
        repsdim += it->dim();
      if(repsdim != dim())
        throw "Rrep::Decompose::Dimension of the result doesn't match the dimension of the rep";

      R->_Subreps.emplace(Subgroup.id(), Reps);

      return R->_Subreps.at(Subgroup.id());

    }
    catch (...) { throw; }
  }

  /* Overloaded * operator with Reps */
  Sum<Rrep> &Rrep::operator*(const Rrep &Rep)
  {   
    try
    {
      if(nirreps() != Rep.nirreps())
        throw "Rrep::operator*::Reps should belong to the same group";

      Rrep *R = DB<Rrep>().find(id());
      if(R != NULL and !R->_Products.empty() and R->_Products.find(Rep.id()) != R->_Products.end())
        return R->_Products.at(Rep.id());
      if(R == NULL)
        throw "Rrep::Unknown rrep, cannot continue";

      Sum<Rrep> ListofReps;
      Sum<Rrep> ListofReps2;
      
      for(int i=0; i<nirreps(); i++)
      {
        Sum<Irrep> Prod = GetObject(i)*Rep.GetObject(i);
        if(ListofReps.nterms())
        {
          for(auto it = ListofReps.begin(); it != ListofReps.end(); it++)
          {
            for(auto it_Prod = Prod.begin(); it_Prod != Prod.end(); it_Prod++)
            {
              Rrep Rep(*it);
              Rep.AddIrrep(*it_Prod);
              Rep.FinishRrep();
              ListofReps2.AddTerm(Rep);
            }
          }
          ListofReps = ListofReps2;
          ListofReps2.Clear();
              
        }
        else
        {
          for(auto it_Prod = Prod.begin(); it_Prod != Prod.end(); it_Prod++)
            ListofReps.AddTerm(Rrep(*_Group,*it_Prod));
        }
      }
      
      R->_Products.emplace(Rep.id(), ListofReps);
      Rrep *R2 = DB<Rrep>().find(Rep.id());
      if(R2 != NULL)
        R2->_Products.emplace(id(), ListofReps);

      return R->_Products.at(Rep.id());;

    }
    catch(...) { throw; }
  }

  /* Direct product of a list of representations */
  Sum<Rrep> Rrep::Product(List<Rrep> &ListofReps)
  {   
    try
    {

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

    }
    catch (...) { throw; }
  }

  /* Checks whether the rep includes a singlet */
  bool Rrep::hasSinglet() const
  {
    try
    {
      for(Rrep::const_iterator it = this->begin(); it != this->end(); it++)
        if(it->dim() == 1)
          return true;
      return false;
    }
    catch (...) { throw; }
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
  bool Rrep::operator>(const Rrep &R) const
  {   
    try
    {
      if(this->dim() > R.dim())
        return true;
      else if(this->dim() == R.dim())
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
    }
    catch (...) { throw; }
  }

  /* Overloaded < operator */
  bool Rrep::operator<(const Rrep &R) const
  {   
    try
    {
      if((*this)>R)
        return false;
      return true;
    }
    catch (...) { throw; }
  }

  /* Returns the json string */
/*  JSONNode Rrep::json(std::string name) const
  {   
    if(name == "id")
    {
      return JSONNode("", id());
    }
    
    if(nirreps() == 1) return GetObject(0).json();
    
    JSONNode json;
      
    json.push_back(JSONNode("id", id()));
    json.push_back(JSONNode("Group", _Group->id()));
    json.push_back(HWeight().json("HWeight"));
    json.push_back(JSONNode("dim", _dim));
    json.push_back(JSONNode("real",_real));
    json.push_back(JSONNode("label",_label));
    json.push_back(JSONNode("nirreps", _nirreps));
    
    JSONNode Casimir(JSON_ARRAY);
    Casimir.set_name("Casimir");
    for(int i=0; i<_Casimir.nterms(); i++)
      Casimir.push_back(JSONNode("",_Casimir.GetObject(i)));
    
    JSONNode DynkinIndex(JSON_ARRAY);
    DynkinIndex.set_name("DynkinIndex");
    for(int i=0; i<_DynkinIndex.nterms(); i++)
      DynkinIndex.push_back(JSONNode("",_DynkinIndex.GetObject(i)));
    
    json.push_back(Casimir);
    json.push_back(DynkinIndex);
    json.push_back(_Weights.json("Weights"));
    
    return json;
  }
*/
  /* Parses a json object into the attributes of the class */
/*  void Rrep::ParseJSON(const JSONNode & n, std::string what)
  {
    JSONNode::const_iterator i = n.begin();
    while (i != n.end()){
  
      // get the node name and value as a string
      std::string node_name = i -> name();

      // find out where to store the values
      if (node_name == "id") {
          std::string id = i->as_string();
        } else if(node_name == "Group") {
          //_Group = new LieGroup(i->as_string());
          _Group = i->as_string();
        } else if(node_name == "GroupRank") {
          _GroupRank = i->as_int();
        } else if(node_name =="HWeight") {
          //_HWeight = new Weight(*_Group, _Group->rank());
          _HWeight = new Weight(_Group, _GroupRank);
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
          if(_nirreps == 1)
            _Casimir = i->as_float();
          else
          {
            JSONNode::const_iterator it = i->begin();
            _Casimir.Clear();
            while(it != i->end())
            {
              _Casimir.AddTerm(it->as_float());
              ++it;
            }
          }
        } else if(node_name == "DynkinIndex") {
          if(_nirreps == 1)
            _DynkinIndex = i->as_float();
          else
          {
            JSONNode::const_iterator it = i->begin();
            _DynkinIndex.Clear();
            while(it != i->end())
            {
              _DynkinIndex.AddTerm(it->as_float());
              ++it;
            }
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
      LieGroup Group(_Group);
      int accumulated_rank = 0;
      _nirreps = 0;
      for(int i=0; i<Group.ngroups(); i++)
      {
        RVector<double> R = HWeight().ExtractMatrix(0,0,accumulated_rank,accumulated_rank + Group.GetObject(i).rank()-1).Row(0);
        Weight HW(Group.GetObject(i), R);
        Irrep aRep(Group.GetObject(i), HW);
        this->AddTerm(aRep);
        _nirreps ++;
        accumulated_rank += Group.GetObject(i).rank();
      }
    }
    

  }
*/


}
