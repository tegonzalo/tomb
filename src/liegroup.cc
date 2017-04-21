/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * liegroup.cc
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 15/02/2012
 */

#include "liegroup.h"
#include "subgroup.h"

/*****************************/
/* Class LieGroup defintions */
/*****************************/

namespace Tomb
{
  // Member functions

  /* Constructor */
  LieGroup::LieGroup() : Product<SimpleGroup>() {
    _ngroups = 0;
    _rank = 0;
    _dim = 0;
    _nabelians = 0;
    _simple = false;
    _semisimple = false;
    _label = this->Print();
    _Casimir.Clear();
    _Reps.Clear();
    _MaxSubgroups.Clear();
    _Subgroups.Clear();
     
  }

  /* Constructor 2, with strings */
  LieGroup::LieGroup(const string id) : Product<SimpleGroup>() {
    
    try {
      _ngroups = 0;
      _rank = 0;
      _dim = 0;
      _nabelians = 0;
      _simple = false;
      _semisimple = false;
      

      string liegroupid(Strings::split_string(id, '[')[0]);
      List<string> simplegroupids = Strings::split_string(liegroupid,'x');
      
      for(int i=0; i<simplegroupids.nterms(); i++)
      {
        AddTerm(*DB<SimpleGroup>().get(simplegroupids[i]));
      }
      
      this->Order();
      
      init();
      
    } catch (...) {
      throw;
    }
  }

  /* Constructor 3, with json nodes */
/*  LieGroup::LieGroup(const JSONNode &n) {
    
    if(n.as_string() != "") {
      LieGroup(n.as_string());
    } else {
      ParseJSON(n);
    }
  }
*/  
  /* Copy constructor 1 */
  LieGroup::LieGroup(const LieGroup &Group) : Product<SimpleGroup>(Group)
  {  
    try
    {
      _rank = Group.rank();
      _dim = Group.dim();
      _simple = Group.simple();
      _semisimple = Group.semisimple();
      _ngroups = Group.ngroups();
      _nabelians = Group.nabelians();
      _label = this->Print();
      _Casimir = Group.Casimir();
      _repsMaxDim = Group.repsMaxDim();
//      _Reps = Group._Reps;
//      _MaxSubgroups = Group._MaxSubgroups;
//      _Subgroups = Group._Subgroups;
    }
    catch (...)
    {
      throw;
    }
  }

  /* Copy constructor 2 */
  LieGroup::LieGroup(const SimpleGroup &Group) : Product<SimpleGroup>(Group)
  {
    try
    {
      _ngroups = 1;
      _rank = Group.rank();
      _dim = Group.dim();
      _nabelians = (Group.abelian() ? 1 : 0);
      _simple = true;
      _semisimple = true;	
      _label = this->Print();
      _Casimir.AddTerm(Group.Casimir());
      _repsMaxDim = Group.repsMaxDim();

      // If the LieGroup is already in the databasei and it has the necessary info, do nothing
      LieGroup *G = DB<LieGroup>().find(id());
      if(G != NULL and G->_Reps.nterms() and G->_MaxSubgroups.nterms() and G->_Subgroups.nterms())
        return ;

      DB<LieGroup>().set(id(),this,true);
      DB<LieGroup>().at(id())->Irreps2Reps(Group.Irreps());
      DB<LieGroup>().at(id())->_MaxSubgroups = Group.MaximalSubgroups();
      DB<LieGroup>().at(id())->_Subgroups = Group.Subgroups();

    }
    catch (...) { throw; }
  }

  /* Copy constructor 3 */
  LieGroup::LieGroup(const List<SimpleGroup> &Group) : Product<SimpleGroup>()
  {
    try
    {
      _ngroups = 0;
      _rank = 0;
      _dim = 0;
      _nabelians = 0;
      _simple = false;
      _semisimple = false;
      _label = this->Print();
      
      for(int i=0; i<Group.nterms(); i++)
        AddTerm(Group.GetObject(i));
      
      _label = this->Print();

      init();
      
    }
    catch (...) { throw; }
  }
  
  /* Copy constructor 4 */
  LieGroup::LieGroup(const List<SimpleGroup *> &Group) : Product<SimpleGroup>()
  {
    try
    {
      _ngroups = 0;
      _rank = 0;
      _dim = 0;
      _nabelians = 0;
      _simple = false;
      _semisimple = false;
      _label = this->Print();
      
      for(int i=0; i<Group.nterms(); i++)
        AddTerm(*Group.GetObject(i));
      
      _label = this->Print();

      init();
      
    }
    catch (...) { throw; }
  }


  /* Copy constructor 5 */
  LieGroup::LieGroup(const Product<SimpleGroup> &Group) : Product<SimpleGroup>()
  {
    try
    {
      _ngroups = 0;
      _rank = 0;
      _dim = 0;
      _nabelians = 0;
      _simple = false;
      _semisimple = false;
      _label = this->Print();
      
      for(int i=0; i<Group.nterms(); i++)
        AddTerm(Group.GetObject(i));
        
      _label = this->Print();
      
      init();
    }
    catch (...) { throw; }
  }

  /* Move constructor */
  LieGroup::LieGroup(LieGroup &&Group) : 
    Product<SimpleGroup>(move(Group)),
    _rank(move(Group._rank)),
    _dim(move(Group._dim)),
    _simple(move(Group._simple)),
    _semisimple(move(Group._semisimple)),
    _ngroups(move(Group._ngroups)),
    _nabelians(move(Group._nabelians)),
    _label(move(Group._label)),
    _Casimir(move(Group._Casimir)),
    _repsMaxDim(move(Group._repsMaxDim)),
    _Reps(move(Group._Reps)),
    _MaxSubgroups(move(Group._MaxSubgroups)),
    _Subgroups(move(Group._Subgroups))
  {
    try
    {
      Group._rank = 0;
      Group._dim = 0;
      Group._nabelians = 0;
      Group._simple = false;
      Group._semisimple = false;
      Group._label = "";
      Group._Casimir.Clear();
      Group._repsMaxDim = 0;
      Group._Reps.Clear();
      Group._MaxSubgroups.Clear();
      Group._Subgroups.Clear();
      
    }
    catch (...) { throw; }
  }	

  /* Destructor */
  LieGroup::~LieGroup()
  {
    _ngroups = 0;
    _rank = 0;
    _dim = 0;
    _nabelians = 0;
    _simple = false;
    _semisimple = false;
    _label = "";
    _Reps.Clear();
    _MaxSubgroups.Clear();
    _Subgroups.Clear();
  }

  /* Assignment operator 1 */
  LieGroup &LieGroup::operator=(const LieGroup &Group)
  {
    try
    {

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
//      _Reps = Group._Reps;
//      _MaxSubgroups = Group._MaxSubgroups;
//      _Subgroups = Group._Subgroups;

      return *this;
      
    }
    catch (...) { throw; }
  }

  /* Assignment operator 2 */
  LieGroup &LieGroup::operator=(const SimpleGroup &Group)
  {
    try
    {
      Product<SimpleGroup>::operator=(Group);
      
      _ngroups =1;
      _rank = Group.rank();
      _dim = Group.dim();
      _nabelians = (Group.abelian() ? 1 : 0);
      _simple = true;
      _semisimple = true;
      _label = this->Print();
      _Casimir.AddTerm(Group.Casimir());
      _repsMaxDim = Group.repsMaxDim();

      // If the liegroup is already in the database and has the necessary info, do noting
      LieGroup *G = DB<LieGroup>().find(id());
      if(G != NULL and G->_Reps.nterms() and G->_MaxSubgroups.nterms() and G->_Subgroups.nterms())
        return *this;

      DB<LieGroup>().set(id(),this,true);
      DB<LieGroup>().at(id())->Irreps2Reps(Group.Irreps());
      DB<LieGroup>().at(id())->_MaxSubgroups = Group.MaximalSubgroups();
      DB<LieGroup>().at(id())->_Subgroups = Group.Subgroups();

      return *this;
    }
    catch (...) { throw; }
  }

  /* Assignment operator 3 */
  LieGroup &LieGroup::operator=(const List<SimpleGroup> &Group)
  {
    try
    {
      if(this == &Group) return *this;
      
      if(this->ngroups()) this->Clear();
      _Reps.Clear();
      _MaxSubgroups.Clear();
      _Subgroups.Clear();
      
      _ngroups = 0;
      _rank = 0;
      _dim = 0;
      _nabelians = 0;
      _simple = false;
      _semisimple = false;
      
      for(int i=0; i<Group.nterms(); i++)
        AddTerm(Group.GetObject(i));
      
      _label = this->Print();

      init();
      
      return *this;
    }
    catch(...)
    {
      throw;
    }
  }

  /* Assignment operator 4 */
  LieGroup &LieGroup::operator=(const List<SimpleGroup*> &Group)
  {
    try
    {
      if(this->ngroups()) this->Clear();
      _Reps.Clear();

      _ngroups = 0;
      _rank = 0;
      _dim = 0;
      _nabelians = 0;
      _simple = false;
      _semisimple = false;
      
      for(int i=0; i<Group.nterms(); i++)
        AddTerm(*Group.GetObject(i));
      
      _label = this->Print();

      init();
      
      return *this;
    }
    catch(...)
    {
      throw;
    }
  }

     

  /* Assignment operator 5 */
  LieGroup &LieGroup::operator=(const Product<SimpleGroup> &Group)
  {
    try
    {
      if(this == &Group) return *this;
      
      if(this->ngroups()) this->Clear();
      
      _ngroups = 0;
      _rank = 0;
      _dim = 0;
      _nabelians = 0;
      _simple = false;
      _semisimple = false;
      
      for(int i=0; i<Group.nterms(); i++)
        AddTerm(Group.GetObject(i));
      
      _label = this->Print();

      init();
      
      return *this;
    }
    catch(...) { throw; }
  }

  /* Move = operator */
  LieGroup &LieGroup::operator=(LieGroup &&Group)
  {
    try
    {
      if(this == &Group) return *this;
      
      Product<SimpleGroup>::operator=(move(Group));
      
      _rank = move(Group._rank);
      _dim = move(Group._dim);
      _simple = move(Group._simple);
      _semisimple = move(Group._semisimple);
      _ngroups = move(Group._ngroups);
      _nabelians = move(Group._nabelians);
      _label = move(Group._label);
      _Casimir = move(Group._Casimir);
      _repsMaxDim = move(Group._repsMaxDim);
      _Reps = move(Group._Reps);
      _MaxSubgroups = move(Group._MaxSubgroups);
      _Subgroups = move(Group._Subgroups);
      
      Group._rank = 0;
      Group._dim = 0;
      Group._nabelians = 0;
      Group._simple = false;
      Group._semisimple = false;
      Group._label = "";
      Group._Casimir.Clear();
      Group._repsMaxDim = 0;
      Group._Reps.Clear();
      Group._MaxSubgroups.Clear();
      Group._Subgroups.Clear();

      return *this;
      
    }
    catch (...) { throw; }
  }

  /* Initialises some variables */
  void LieGroup::init()
  {
    try
    {
 
      LieGroup *G = DB<LieGroup>().find(id());
      if(G != NULL and G->_Reps.nterms() and G->_MaxSubgroups.nterms() and G->_Subgroups.nterms())
      {
        if(G->_repsMaxDim < _repsMaxDim)
          G->CalculateReps(_repsMaxDim);
        return ;
      }

      if(nterms() == 1)
      {
        SimpleGroup *G = DB<SimpleGroup>().get(id());
        DB<LieGroup>().set(id(), this); 
        DB<LieGroup>().at(id())->Irreps2Reps(G->_Irreps);
        DB<LieGroup>().at(id())->_MaxSubgroups = G->_MaxSubgroups;
        DB<LieGroup>().at(id())->_Subgroups = G->_Subgroups;
        return ;
      }

      DB<LieGroup>().set(id(), this);
      DB<LieGroup>().at(id())->CalculateReps(_repsMaxDim);
      DB<LieGroup>().at(id())->CalculateMaximalSubgroups();
//      DB<LieGroup>().at(id())->CalculateSubgroups();
    }
    catch (...) { throw; }
  }

  /* Identifier of the Lie Group */
  string LieGroup::id() const
  {
    stringstream s;
    if(nterms())
    {
      for(int i=0; i<nterms()-1; i++)
        s << GetObject(i).id() << 'x';
      s << GetObject(nterms()-1).id();
    }
    return s.str();
  }

  /* Returns the rank */
  int LieGroup::rank() const
  {
    return _rank;
  }

  /* Returns the dimension */
  int LieGroup::dim() const
  {
    return _dim;
  }

  /* Returns if the group is simple */
  bool LieGroup::simple() const
  {
    try
    {
      if(this->ngroups() == 1 and !this->nabelians())
        return true;
      else
        return false;
    }
    catch (...) { throw; }
  }

  /* Returns if the group is semisimple */
  bool LieGroup::semisimple() const
  {
    if(this->ngroups() == 1)
      return true;
    
    if(this->ngroups() > 1 and this->nabelians() == 0) 
      return true;
    else
      return false;
  }

  /* Returns the number of simple groups / abelian groups in the product */
  int LieGroup::ngroups() const
  {
    return this->_ngroups;
  }

  /* Returns the number of abelian groups in the product */
  int LieGroup::nabelians() const
  {
    try
    {
      int nabelians = 0;
      for(int i=0; i<this->ngroups(); i++)
        if(this->GetObject(i).abelian())
          nabelians++;
      return nabelians;
    } 
    catch (...) { throw; }
  }

  /* Returns if the Lie Group is abelian */
  bool LieGroup::abelian() const 
  {  
    try
    {
      for(int i=0; i<this->ngroups(); i++)
        if(!this->GetObject(i).abelian())
          return false;
      return true;
    } 
    catch (...) { throw; }
  }

  /* Returns the label of the LieGroup */
  string LieGroup::label() const 
  {
    return _label;
  }

  /* Returns the Casimir of the LieGroup */
  List<double> LieGroup::Casimir() const 
  {
    return _Casimir;
  }
  
  /* Returns the maximum dimension of the calculated reps */
  int LieGroup::repsMaxDim() const 
  {
    return _repsMaxDim;
  }
  
  /* Returns the Reps of the group */
  List<Rrep> LieGroup::Reps() const
  {
    try
    {
      LieGroup *G = DB<LieGroup>().find(id());
      if(G != NULL and G->_Reps.nterms())
      {
        if(G->repsMaxDim() < _repsMaxDim)
          G->CalculateReps(_repsMaxDim);
        return G->_Reps;
      }
      return _Reps;
    }
    catch (...) { throw; }
  }
 
  /* Returns the reps or calculate them if the dimension is larger */
  List<Rrep> &LieGroup::Reps(int maxdim)
  {
    try
    {
      LieGroup *G = DB<LieGroup>().find(id());
      if(G != NULL and G->_Reps.nterms())
      {
        if(G->repsMaxDim() < maxdim)
          G->CalculateReps(maxdim);
        return G->_Reps;
      }
      return _Reps;
    }
    catch (...) { throw; }
  }

  /* Returns the maximal subgroups of a lie group */
  List<SubGroup> LieGroup::MaximalSubgroups() const
  {
    try
    {
      LieGroup *G = DB<LieGroup>().find(id());
      if(G != NULL and G->_MaxSubgroups.nterms())
        return G->_MaxSubgroups;
      return _MaxSubgroups;
    }
    catch (...) { throw; }
  }

  /* Returns the subgroups of a lie group */
  List<SubGroup> LieGroup::Subgroups() const
  {
    try
    {
      LieGroup *G = DB<LieGroup>().find(id());
      if(G != NULL and G->_Subgroups.nterms())
        return G->_Subgroups;
      return G->_Subgroups;
    }
    catch (...) { throw; }
  }

  /* Returns whether the Lie group is a subgroup of another Lie Group */
/*  bool LieGroup::isSubgroupOf(LieGroup Group) const {

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
*/
  /* Returns whether the Lie group is a subgroup of another Simple Group */
/*  bool LieGroup::isSubgroupOf(SimpleGroup Group) const {

    try {
      return isSubgroupOf(LieGroup(Group));
    } catch (...) {
      throw;
    }
  }
*/

  /* Delete a subgroup of the LieGroup */
  void LieGroup::DeleteTerm(int i)
  {   
    try
    {
      if(GetObject(i).abelian())
      {
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
      
    }
    catch (...) { throw; }
  }

  /* Adds a simple group to the Lie group */
  void LieGroup::AddTerm(const SimpleGroup &Group)
  {
    try
    {
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

    }
    catch (...) { throw; }
  }

  /* Adds a lie group to the lie group */
  void LieGroup::AddTerm(const LieGroup &Group)
  {
    try
    {
      for(int i=0; i<Group.ngroups(); i++)
      {
        AddTerm(Group.GetObject(i));
      }

    }
    catch(...) { throw; }
  }

  /* Returns the simple representations of the Lie group */
  List<Rrep> LieGroup::SimpleReps()
  {
    try
    {
      List<Rrep> Reps;
    
      List< List<Irrep> > ListofSimpleReps;
    
      int nirreps = 1;
    
      for(int i=0; i<this->ngroups(); i++)
      {
        ListofSimpleReps.AddTerm(List<Irrep>(this->GetObject(i).SimpleReps()));
        nirreps *= ListofSimpleReps.GetObject(i).nterms();
      }
      
      for(int i=0; i<nirreps; i++)
      {
        Weight w(*this,this->rank());
        int reps = 1;
        for(int j=0; j<this->ngroups(); j++)
        {
          int thisreps = ListofSimpleReps.GetObject(j).nterms();
          reps *= thisreps;
          int index = (i*reps/nirreps)%thisreps;
          if(j==0)
            w = ListofSimpleReps.GetObject(0).GetObject(index).HWeight();
          else
            w.Append(ListofSimpleReps.GetObject(j).GetObject(index).HWeight());
        }
        Reps.AddTerm(Rrep(*this,w));

      }
    
      return Reps;
      
    }
    catch (...) { throw; }
  }

  /* Returns the generating rep of the Lie group */
  Rrep LieGroup::GeneratingRep()
  {
    try
    {
      Rrep Rep(*this, GetObject(0).GeneratingRep());
      for(int i=1; i<ngroups(); i++)
        Rep.AddIrrep(GetObject(i).GeneratingRep());
      Rep.FinishRrep();
      return Rep;
    }
    catch (...) { throw; }
  }
  
  /* Returns the adjoint reps for the Lie group */
  List<Rrep> LieGroup::AdjointReps() 
  {
    try
    {
      List<Rrep> Reps;
      for(int i=0; i<ngroups(); i++)
      {
        Rrep Rep(*this,GetObject(0).AdjointRep());
        for(int j=1; j<ngroups(); j++)
        {
          if(j==i)
            Rep.AddIrrep(GetObject(j).AdjointRep());
          else
            Rep.AddIrrep(GetObject(j).SingletRep());
        }
        Rep.FinishRrep();
        Reps.AddTerm(Rep);
      }
      return Reps;
    }
    catch (...) { throw; }
  }

  /* Returns the singlet rep for the Lie group */
  Rrep LieGroup::SingletRep() 
  {
    try
    {
      return Reps().GetObject(0);
      
    }
    catch (...) { throw; }
  }

  /* Transform a list of irreps into a list of reps (if the LieGroup is simple) */
  List<Rrep> &LieGroup::Irreps2Reps(List<Irrep> Irreps)
  {
    try 
    {
      if(ngroups() != 1) throw "LieGroup::Irreps2Reps::Operation only permitted on simple groups";
      
      _Reps.Clear();
      for(auto it = Irreps.begin(); it != Irreps.end(); it++) _Reps.AddTerm(Rrep(*this,*it));
      
      return _Reps;
    }
    catch (...) { throw; }
  }

  /* Calculates all the representations of the group up to dimension maxdim */
  List<Rrep> &LieGroup::CalculateReps(int maxdim)
  {  
    try
    {
      // If the reps are calculated already
      if(_Reps.nterms() and _repsMaxDim >= maxdim)
      {
        int i = _Reps.nterms()-1;
        while(_Reps.GetObject(i).dim() > maxdim)
        {
          _Reps.DeleteTerm(i);
          i--;
        }
        return _Reps;
      }
      
      
      // If it is a simple group, calculate irreps
      if(ngroups() == 1)
      {
        List<Irrep> Irreps = GetObject(0).CalculateIrreps(maxdim);
        _Reps = Irreps2Reps(Irreps);
        _repsMaxDim = maxdim;
      
        return _Reps;
      }

      // If not, calculate it

      List<List<Rrep> > ListofReps;
    
      LieGroup Group = *this;
      Group.DeleteTerm(Group.nterms()-1);
      ListofReps.AddTerm(Group.Reps());

      Group = this->GetObject(this->nterms()-1);
      ListofReps.AddTerm(Group.Reps());
        
      int nirreps = ListofReps.GetObject(0).nterms()*ListofReps.GetObject(1).nterms();
    

      for(int i=0; i<nirreps; i++)
      {
        Weight w;
        int reps = 1;
        int dim = 1;
        for(int j=0; j<2; j++)
        {
          int thisreps = ListofReps.GetObject(j).nterms();
          reps *= thisreps;
          int index = (i*reps/nirreps)%thisreps;
          if(j==0)
            w = ListofReps.GetObject(0).GetObject(index).HWeight();
          else
            w.Append(ListofReps.GetObject(j).GetObject(index).HWeight());
          dim *= ListofReps.GetObject(j).GetObject(index).dim();
        }
        
        if(dim <= maxdim)
        {
          Rrep Rep(*this,w);
          _Reps.AddTerm(Rep);
        }
      }
      
      _repsMaxDim = maxdim;
      
      return _Reps;
      
    }
    catch (...) { throw; }
    
  }

  /* Calculates the maximal subgroups of a Lie Group */
  List<SubGroup> &LieGroup::CalculateMaximalSubgroups()
  { 
    try
    {   
      // If maximal subgroups have been calculated already return them
      if(_MaxSubgroups.nterms())
        return _MaxSubgroups;
      
      // If the group consists only on abelian groups return the empty variable
      if(ngroups() == nabelians())
        return _MaxSubgroups;
      
      // If there is only one group return the maximal subgroups of that simple group
      if(this->ngroups() == 1)
      {
        _MaxSubgroups = GetObject(0)._MaxSubgroups;
        return _MaxSubgroups;
      }
      
      List<List<SubGroup> > ListofSubgroups;
      
      for(LieGroup::iterator it = this->begin(); it != this->end(); it++)
      {
        List<SubGroup> Subgroups(SubGroup(*it,*it));
        
        Subgroups.AppendList(it->MaximalSubgroups());
        ListofSubgroups.AddTerm(Subgroups);
      }
      
      int nterms = 1;
      for(int i=0; i<ListofSubgroups.nterms(); i++)
        nterms *= ListofSubgroups.GetObject(i).nterms();

      for(int i=0; i<nterms; i++)
      {
        SubGroup Subgroup(*this);
        
        int terms = 1;
        for(int j=0; j<ngroups(); j++)
        {
          int thisterms = ListofSubgroups.GetObject(j).nterms();
          terms *= thisterms;
          int index = (i*terms/nterms)%thisterms;
          Subgroup.AddTerm(ListofSubgroups.GetObject(j).GetObject(index),j);
        }
        Subgroup.Order();
        _MaxSubgroups.AddTerm(Subgroup);
      }
      _MaxSubgroups.DeleteTerm(0);
      _MaxSubgroups.Order();
      _MaxSubgroups.EliminateRepeated();


      return _MaxSubgroups;
      
    }
    catch (...) { throw; }
  }

  /* Calculates the special subgroups of a Lie Group */
/*  List<SubGroup> LieGroup::SpecialSubgroups() {
    
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
*/
  /* Calculates all the subgroups of a Lie group */
  List<SubGroup> &LieGroup::CalculateSubgroups() 
  {
    try 
    {
      // If Subgroups are known, return them
      if(_Subgroups.nterms())
        return _Subgroups;

      _Subgroups.AppendList(_MaxSubgroups);

      // Now, iterate to find the subgroups of the subroups
      int nsubgroups = _Subgroups.nterms();
      int group = 0;
      for(List<SubGroup>::iterator it_Subgroups = _MaxSubgroups.begin(); it_Subgroups != _MaxSubgroups.end() and group < nsubgroups; it_Subgroups++)
      {
        SubGroup subgroup = *it_Subgroups;
        List<SubGroup> AuxSubgroups = subgroup.Subgroups();

        for(List<SubGroup>::iterator it_AuxSubgroups = AuxSubgroups.begin(); it_AuxSubgroups != AuxSubgroups.end(); it_AuxSubgroups++)
        {
          SubGroup Subgroup(*it_AuxSubgroups, subgroup);
          Subgroup.SetProjection(it_AuxSubgroups->Projection()*it_Subgroups->Projection());
          Subgroup.Order();
          if(_Subgroups.Index(Subgroup) == -1)
            _Subgroups.AddTerm(Subgroup);
        }
        
        group++;
      }	

      // Calculate subgroups by truncating the group
//      if(ngroups()>1) _Subgroups.AppendList(SplitToSubGroups());
      

      // Last operations on the groups
//      _Subgroups.Order();
//      _Subgroups.EliminateRepeated();

cout << _Subgroups << endl;
      
      return _Subgroups;
    
    } catch (...) {
      throw;
    }
  }

  /* Calculates the subgroups with a given rank of a Lie group */
/*  List<SubGroup> LieGroup::Subgroups(int rank) {
    
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
      /*if(database_check(id(), "Subgroups") and DataBase.at(id()).hasSubgroups()) {
        _Subgroups = DataBase.at(id()).Subgroups();
        _hasSubgroups = true;
        List<SubGroup> Subgroups;
        for(List<SubGroup>::iterator it_Subgroups = _Subgroups.begin(); it_Subgroups != _Subgroups.end(); it_Subgroups++)
          if(it_Subgroups->rank() == rank)
            Subgroups.AddTerm(*it_Subgroups);
        return Subgroups;
      }*/

      // If not, calculate it
      // Calculate the maximal subgroups
/*      List<SubGroup> MaximalSubgroups = this->MaximalSubgroups();
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
*/
  /* Calculates the subgroups with ranks between rank1 and rank2 */
/*  List<SubGroup> LieGroup::Subgroups(int r1, int r2) {
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
      /*if(database_check(id(), "Subgroups") and DataBase.at(id()).hasSubgroups()) {
        _Subgroups = DataBase.at(id()).Subgroups();
        _hasSubgroups = true;
        List<SubGroup> Subgroups;
        for(List<SubGroup>::iterator it_Subgroups = _Subgroups.begin(); it_Subgroups != _Subgroups.end(); it_Subgroups++)
          if(it_Subgroups->rank() >= rank1 and it_Subgroups->rank() <= rank2)
            Subgroups.AddTerm(*it_Subgroups);
        return Subgroups;
      }*/

      // If not, calculate it
      // Calculate the maximal subgroups
/*      List<SubGroup> MaximalSubgroups = this->MaximalSubgroups();
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
*/
  /* Splits the group into subgroups */
/*  List<SubGroup> LieGroup::SplitToSubGroups(int rank) {
    try {
      
      List<SubGroup> Subgroups;
      
      if(!nabelians()) return Subgroups;
            
      for(int j=0; j<nabelians(); j++) {
        SubGroup Subgroup(SubGroup(*this,*this));
        Subgroup.DeleteTerm(ngroups()-j-1);
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
      return Subgroups;
      
    } catch (...) {
      throw;
    }
  }
*/
  /* Splits the group into subgroups between two ranks */
/*  List<SubGroup> LieGroup::SplitToSubGroups(int r1, int r2) {
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
*/
  /* Returns the breaking chain from a liegroup to a simple group */
/*  List<List<Tree<SimpleGroup> > > LieGroup::BreakingChains(const SimpleGroup &Subgroup) {
    try {
      return BreakingChains(LieGroup(Subgroup));	
    } catch (...) {
      throw;
    }
  }
*/
  /* Returns the breaking chain from a liegroup to a subgroup */
/*  List<List<Tree<SimpleGroup> > > LieGroup::BreakingChains(const LieGroup &Subgroup) {
    try {
      
      List<List<Tree<SimpleGroup> > > BrChains;
      
      if(!Subgroup.isSubgroupOf(*this)) {
        return BrChains;
      }
      
      List<SubGroup> Subgroups = this->Subgroups(Subgroup.rank(), rank());		
      
    
      for(List<SubGroup>::iterator it_Subgroups = Subgroups.begin(); it_Subgroups != Subgroups.end(); it_Subgroups++) {
        if(LieGroup(*it_Subgroups) == Subgroup) {
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
            tree.setLabel(string(1,label));
            Chain.AddTerm(tree);	
          }
          BrChains.AddTerm(Chain);
        } else if(Subgroup.isSubgroupOf(*it_Subgroups)) {
          List<List<Tree<SimpleGroup> > > BrChains2 = it_Subgroups->BreakingChains(Subgroup);
          for(List<List<Tree<SimpleGroup> > >::iterator it_BrChains2 = BrChains2.begin(); it_BrChains2 != BrChains2.end(); it_BrChains2++) {
            List<Tree<SimpleGroup> > Chain;
            for(int i=0; i<nterms(); i++) {
              Tree<SimpleGroup> tree(GetObject(i));
              char label = 'A' + i;
              for(int j=0; j<it_BrChains2->nterms(); j++) {
                if(nterms() == 1 or (nterms() > 1 and it_Subgroups->labels().GetObject(j)[0] == label)) {
                  Tree<SimpleGroup> branch(it_BrChains2->GetObject(j));
                  //branch.setLabel(it_BrChains2->GetObject(j).label());
                  branch.setLabel(it_Subgroups->labels().GetObject(j));
                  tree.AddBranch(branch);
                }
              }
              tree.setLabel(string(1,label));
              Chain.AddTerm(tree);	
            }
            
            BrChains.AddTerm(Chain);
            }
            
        }
      }
      
      /// U(1) breaking
      // Iterate over the chains to obtain the label
      List<string> Labels;
      List<List<Tree<SimpleGroup> > > SemisimpleParts;
      
      for(List<List<Tree<SimpleGroup> > >::iterator it_BrChains = BrChains.begin(); it_BrChains != BrChains.end(); it_BrChains++) {
        List<Tree<SimpleGroup> > Chain = *it_BrChains;
          
        LieGroup Supergroup = Chain.GetObject(0).Level(0);
        LieGroup Group = Chain.GetObject(0).Level(1);
        for(int i=1; i<Chain.nterms(); i++) {
          Supergroup.AddTerm(Chain.GetObject(i).Level(0));
          if(Chain.GetObject(i).depth() > 1) {
            Group.AddTerm(Chain.GetObject(i).Level(1));
          }
        }
            
        
        // Check whether there is rank reduction and, if so get the labels of the abelians
        if(Supergroup.rank() > Group.rank() and Group.nabelians() > 0) {
          
          string label;
          
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
            if(Labels.GetObject(n).find(label) == string::npos) {
              label.append(Labels.GetObject(n));
              Labels.DeleteTerm(n);
              Labels.InsertTerm(n, label);
            }
          }
        }			
      }
      
      
      
      // Iterate again over the chains, now to change the label of the abelians
      int count = 0;
      for(List<List<Tree<SimpleGroup> > >::iterator it_BrChains = BrChains.begin(); it_BrChains != BrChains.end(); it_BrChains++) {
        List<Tree<SimpleGroup> > Chain = *it_BrChains;
          
        LieGroup Supergroup = Chain.GetObject(0).Level(0);
        LieGroup Group = Chain.GetObject(0).Level(1);
        for(int i=1; i<Chain.nterms(); i++) {
          Supergroup.AddTerm(Chain.GetObject(i).Level(0));
          if(Chain.GetObject(i).depth() > 1) {
            Group.AddTerm(Chain.GetObject(i).Level(1));
          }
        }
            
        
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
          
          string label = Labels.GetObject(SemisimpleParts.Index(SemisimpleChain));
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
        
              
        // If there was any change, replace the chain
        if(change) {
          BrChains.DeleteTerm(count);
          BrChains.InsertTerm(count, Chain);
        }
        count ++;
        
      }
      BrChains.EliminateRepeated();
      
      
      return BrChains;
      
    } catch (...) {
      throw;
    }
  }
*/
  /* Returns the list of possible invariant products of the representations of the group at up dimension operator dim */
/*  List<Product<Rrep> > LieGroup::Invariants(const List<Rrep> &reps, const int dim) {
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
      

      for(List<Product<Rrep> >::iterator it_Products = Products.begin(); it_Products != Products.end() and it_Products->nterms() < dim; it_Products++) {
        for(List<Rrep>::iterator it_Reps = Reps.begin(); it_Reps != Reps.end() and it_Reps->dim() >= it_Products->GetObject(-1).dim(); it_Reps++) {
          Product<Rrep> Prod(*it_Products);
          Prod.AddTerm(*it_Reps);
          if(Products.Index(Prod) == -1) {
            Products.AddTerm(Prod);
          }
        }
      }
      
      //Products.Reverse();

      List<Product<Rrep> > Invariants;
      
      for(List<Product<Rrep> >::iterator it_Products = Products.begin(); it_Products != Products.end(); it_Products++) {
        Sum<Rrep> Prod(Rrep::Product(*it_Products));
        Prod.Order("ASC");
        if(Prod.GetObject(0).isSinglet()) {
          Invariants.AddTerm(*it_Products);
        }
      }
      
      return Invariants;
      
    } catch (...) {
      throw;
    }
  }
*/
    
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

      //if(Product<SimpleGroup>(*this) == Group) {
      //  return true;
      //}

      LieGroup::iterator it_1 = this->begin();
      for(LieGroup::const_iterator it_2 = Group.begin(); it_1 != this->end() and it_2 != Group.end(); it_2++)
      {
        if(*it_1 != *it_2)
          return false;
        *it_1++;
      }
      /*for(int i=0; i<this->ngroups(); i++) {
        if(this->GetObject(i) != Group.GetObject(i)) {
          return false;
        }
      }*/
    
      //return false;
      return true;
      
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
        if(next(it) != this->end())
          _label.append("x");
      }
    } catch (...) {
      throw;
    }
  }

  /* Returns the json string */
/*  JSONNode LieGroup::json(string name) const {
    
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
*/
  /* Parses a json object into the attributes of the class */
/*  void LieGroup::ParseJSON(const JSONNode & n, string what) {
    JSONNode::const_iterator i = n.begin();

    while (i != n.end()){
      // get the node name and value as a string
      string node_name = i -> name();
      
      // find out where to store the values
      if (node_name == "id") {
        string id = i->as_string();
        stringstream ss(id);
        if(!this->nterms()) {
          while(getline(ss, id, 'x')) {
            AddTerm(SimpleGroup(id));
          }
          if(nterms() == 1) {
            SimpleGroup G(id);
            G.ParseJSON(n,what);
            //this->~LieGroup();
            DeleteTerm(0);
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
*/

  /* Static function to get a liegroup from the Database or create it otherwise */
  LieGroup* LieGroup::get(const SimpleGroup& Group)
  {
    LieGroup *sg = DB<LieGroup>().find(Group.id());
    if(sg == NULL)
    {
      sg = new LieGroup(Group);
      DB<LieGroup>().set(Group.id(),sg);
    }
    return sg;
  }

}
