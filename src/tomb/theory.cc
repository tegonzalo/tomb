/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * theory.cc
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 12/01/2016
 */

#include "theory.h"

/****************************/
/* Class Theory definitions */
/****************************/

namespace Tomb
{

  // Member functions

  /* Empty constructor */
  Theory::Theory()
  {
  }
  
  /* Constructor */
  Theory::Theory(LieGroup &Group, Chain &BreakingChain, List<Field> &Fields)
  {
    try {
      //_Group = Group.id();
      _Group = &Group;
      _BreakingChain = BreakingChain;
      _Fields = Fields;
      _Fields.Order();
      _norm = 1;
      _Mixing = RVector<double>();
      _anomalyFree = true;
      _observables.clear();
      
    }
    catch (...) { throw; }
  }
  
  /* Copy constructor */
  Theory::Theory(const Theory &T)
  {   
    try
    {
      //_Group = T.GroupId();
      _Group = T.GroupPtr();
      _BreakingChain = T.BreakingChain();
      _Fields = T.Fields();
      _norm = T.norm();
      _Mixing = T.Mixing();
      _anomalyFree = T.anomalyFree();
      _observables = T.observables();
    }
    catch (...) { throw; }
  }
  
  
  /* Copy constructor with JSON node */
  Theory::Theory(const JSONNode &json)
  {
    try
    {
      ParseJSON(json);
    }
    catch (...) { throw; }
  }

  /* Move constructor */
  Theory::Theory(Theory &&T) :
    _Group(std::move(T._Group)),
    _BreakingChain(std::move(T._BreakingChain)),
    _Fields(std::move(T._Fields)),
    _norm(std::move(T._norm)),
    _Mixing(std::move(T._Mixing)),
    _anomalyFree(std::move(T._anomalyFree)),
    _observables(std::move(T._observables))
  {
    try
    {
      //T._Group = "";
      T._Group = NULL;
      T._BreakingChain.Clear();
      T._Fields.Clear();
      T._norm == 1;
      T._Mixing = RVector<double>();
      T._anomalyFree = false;
      T._observables.clear();
      
    }
    catch (...) { throw; }
  }

  /* Destructor */
  Theory::~Theory()
  {
  }

  /* Assignment operator */
  Theory &Theory::operator=(const Theory &T)
  {   
    try
    {
      if(this == &T) return *this;
      
      //_Group = T.GroupId();
      _Group = T.GroupPtr();
      _BreakingChain = T.BreakingChain();
      _Fields = T.Fields();
      _norm = T.norm();
      _Mixing = T.Mixing();
      _anomalyFree = T.anomalyFree();
      _observables = T.observables();
      
      return *this;
    }
    catch (...) { throw; }
  }

  /* Move assignment operator */
  Theory &Theory::operator=(Theory &&T)
  {
    try
    {
      if(this == &T) return *this;
      
      _Group = std::move(T._Group);
      _BreakingChain = std::move(T._BreakingChain);
      _Fields = std::move(T._Fields);
      _norm = std::move(T._norm);
      _Mixing = std::move(T._Mixing);
      _anomalyFree = std::move(T._anomalyFree);
      _observables = std::move(T._observables);
      
      //T._Group = "";
      T._Group = NULL;
      T._BreakingChain.Clear();
      T._Fields.Clear();
      T._norm = 1;
      T._Mixing = RVector<double>();
      T._anomalyFree = false;
      T._observables.clear();
      
      return *this;
    }
    catch (...) { throw; }
  }
  
  
  /* Group getter */
  LieGroup &Theory::Group() const
  {
    //return LieGroup(_Group);
    return *_Group;
  }
  
  /* Group id getter */
  std::string Theory::GroupId() const
  {
    //return _Group;
    return _Group->id();
  }
  
  /* Group pointer getter */
  LieGroup *Theory::GroupPtr() const
  {
    return _Group;
  }

  /* Breaking chain getter */
  Chain Theory::BreakingChain() const
  {
    return _BreakingChain;
  }
  
  /* Fields getter */
  List<Field> Theory::Fields() const
  {
    return _Fields;
  }
  
  /* Norm getter */
  double Theory::norm() const
  {
    return _norm;
  }
  
  /* Mixing getter */
  RVector<double> Theory::Mixing() const
  {
    return _Mixing;
  }
  
  /* Mixing setter */
  void Theory::setMixing(RVector<double> mixing)
  {
    _Mixing = mixing;
  }

  /* Returns whether the theory is chiral or not */
  bool Theory::chirality() const
  {
    try
    {
      //LieGroup Group(_Group);

      //if(Group.nterms() == 1)
      if(_Group->nterms() == 1)
      {
        //if(!(Group.GetObject(0).type() == 'A' and Group.rank() > 1) and !(Group.GetObject(0).type() == 'D' and Group.rank()%2 != 0))
        if(!(_Group->GetObject(0).type() == 'A' and _Group->rank() > 1) and !(_Group->GetObject(0).type() == 'D' and _Group->rank()%2 != 0))
          return false;
        else
        {
          int nfields = 0;
          for(auto it=_Fields.begin(); it != _Fields.end(); it++)
            if(it->isFermion() and !it->real())
              nfields += it->dim();
          if(nfields >= 45)
            return true;
          return false;
        }
      }
      else
      {
        int nfields = 0;
        for(auto it=_Fields.begin(); it != _Fields.end(); it++)
        {
          bool real = true;
          for(int i = 0; i != it->nterms() and real; i++) {
            //if((Group.GetObject(i).abelian() and it->GetObject(i).HWeight()!=0) or (!Group.GetObject(i).abelian() and !it->GetObject(i).real()))
            if((_Group->GetObject(i).abelian() and it->GetObject(i).HWeight()!=0) or (!_Group->GetObject(i).abelian() and !it->GetObject(i).real()))
              real = false;
          }
          if(it->isFermion() and !real){
            nfields += it->dim();}
        }
        if(nfields >= 45)
          return true;
        return false;
      }
          
    } catch (...)
    {
      throw;
    }
  }

  /* Anomaly getter */
  bool Theory::anomalyFree() const
  {
    return _anomalyFree;
  }
  
  /* Calculate the anomaly contribution */
  void Theory::calculateAnomaly()
  {
    try
    {
      //List<List<SimpleGroup> > tdgroups = Combinatorics::permutations<SimpleGroup>(LieGroup(_Group), 3, true, true);
      List<List<SimpleGroup> > tdgroups = Combinatorics::permutations<SimpleGroup>(*_Group, 3, true, true);
      
      //std::cout << tdgroups << std::endl;
  
      // Create an abelian group to compare with
      SimpleGroup abelian("U1");
      
      // Default anomaly
      _anomalyFree = true;
      
      //std::cout << _Fields.json().write_formatted() << std::endl;
    
      for(List<List<SimpleGroup> >::iterator it = tdgroups.begin(); it != tdgroups.end(); it++)
      {
        //std::cout << *it << std::endl;
        double anomaly = 0;
        switch(std::count(it->begin(), it->end(), abelian))
        {
          case 0:
            if(it->GetObject(0) == it->GetObject(1) and it->GetObject(0) == it->GetObject(2))
              if((it->GetObject(0).type() == 'A' and it->GetObject(0).rank() > 1) or (it->GetObject(0).type() == 'D' and it->GetObject(0).rank() == 3))
              {
                //Calculate the anomaly for a general gauge group, MISSING 
                for(List<Field>::iterator it_fields = _Fields.begin(); it_fields != _Fields.end(); it_fields++)
                {
                  Irrep rep = it_fields->GetObject(it_fields->Group().Index(it->GetObject(0)));
                  if(rep.real())
                    switch(it->GetObject(0).rank())
                    {
                      case 2:
                        // SU(3)
                        if(rep.dim() == 3)
                          anomaly += (rep.conjugate() ? -1 : 1)*it_fields->dim()/rep.dim();
                        break;
                      case 3:
                        // SU(4)
                        if(rep.dim() == 4)
                          anomaly += (rep.conjugate() ? -1 : 1)*it_fields->dim()/rep.dim();
                        break;
                      case 5:
                        // SU(5)
                        if(rep.dim() == 5)
                          anomaly += (rep.conjugate() ? -1 : 1)*it_fields->dim()/rep.dim();
                        else if(rep.dim() == 10)
                          anomaly += (rep.conjugate() ? -1 : 1)*it_fields->dim()/rep.dim();
                        break;
                    }
                }
              }
            break;
          case 1: 
            // Only works if there is one abelian component and it's at the end
            if(it->GetObject(0) == it->GetObject(1))
              for(List<Field>::iterator it_fields = _Fields.begin(); it_fields != _Fields.end(); it_fields++)
                if(!it_fields->isSinglet(it_fields->Group().Index(it->GetObject(0))))
                  anomaly += it_fields->dim()*it_fields->HWeight()[-1];
            break;
          case 2:
            anomaly = 0;
            break;
          case 3:
            // Only works if there is one abelian component and it's at the end
            for(List<Field>::iterator it_fields = _Fields.begin(); it_fields != _Fields.end(); it_fields++)
            {
              //std::cout << *it_fields <<  std::endl;
              //std::cout <<  (it_fields->dim()*pow(it_fields->HWeight()[-1],3)) << std::endl;
              anomaly += it_fields->dim()*pow(it_fields->HWeight()[-1],3);
              //std::cout << anomaly << std::endl;
            }
            break;
        }
        
        //std::cout << "anomaly = " << anomaly << std::endl;
        if(fabs(anomaly) > 0.001)
          _anomalyFree = false;
      }
      // Maybe add gravitational and Witten anomalies
    }
    catch (...) { throw; }
  }
  
  /* Proton decay getter */
  std::map<std::string, bool> Theory::observables() const
  {
    return _observables;
  }
  
  /* Calculate the observable contribution */
  void Theory::calculateObservables(List<Field> &fields) 
  {
    try
    {
      // Proton Decay
      _observables.emplace("ProtonDecay", Observables::ProtonDecay(fields));
      
      // N-N oscillations
      _observables.emplace("NNOscillations", Observables::NNOscillations(fields));
      
        
    }
    catch (...) { throw; }
  }
  
  /* Obtain the scalars */
  List<Field> Theory::getScalars() const
  {
    try
    {
      List<Field> scalars;
      for(List<Field>::const_iterator it_Fields = _Fields.begin(); it_Fields != _Fields.end(); it_Fields++)
        if(it_Fields->isScalar())
          scalars.AddTerm(*it_Fields);
      return scalars;
    }
    catch (...) { throw; }
  }
  
  /* Obtain the fermions */
  List<Field> Theory::getFermions() const
  {
    try
    {
      List<Field> fermions;
      for(List<Field>::const_iterator it_Fields = _Fields.begin(); it_Fields != _Fields.end(); it_Fields++)
        if(it_Fields->isFermion())
          fermions.AddTerm(*it_Fields);
      return fermions;
    }
    catch (...) { throw; }
  }
  
  /* Overloaded == operator */
  bool Theory::operator==(const Theory &theory) const 
  {
    if(Group() == theory.Group() and 
      BreakingChain() == theory.BreakingChain() and 
      Fields() == theory.Fields() and
      Mixing() == theory.Mixing() and
      anomalyFree() == theory.anomalyFree() and
      observables() == theory.observables())
      return true;
    return false;
  }

  /* Overloaded != operator */
  bool Theory::operator!=(const Theory &theory) const
  {
    if(!(Group() == theory.Group() and BreakingChain() == theory.BreakingChain() and Fields() == theory.Fields()))
      return true;
    return false;
  }

  /* Find the breaking reps from this theory to subgroup with optional reps */
  List<Field> Theory::findBreakingReps(SubGroup &subgroup, const List<Field> &reps)
  {
    try {
      List<Field> breakingreps;
      List<Field> scalars;
      
      if(reps.Empty())
        scalars = getScalars();
      else
        for(List<Field>::const_iterator it_reps = reps.begin(); it_reps != reps.end(); it_reps++)
          if(it_reps->isScalar())
            scalars.AddTerm(*it_reps);
                    
      for(List<Field>::iterator it_scalars = scalars.begin(); it_scalars != scalars.end(); it_scalars++) 
      {
        Field scalar = *it_scalars;
        // Check if the rep can break to the next step
        if(scalar.canBreak(_BreakingChain))
        {
          // Decompose every scalar into fields in the subgroup
          List<Field> subreps = scalar.Decompose(subgroup);
          
          // Check whether the decomposition has singlets
          bool singlet = false;
          for(List<Field>::iterator it_subreps = subreps.begin(); it_subreps != subreps.end(); it_subreps++)
          {
            if(it_subreps->isSinglet()) 
              singlet = true;
          }
          // If it has a singlet check that the rep does not decompose into intermediate groups (NOT COMPLETE)
          if(singlet and !(_Group->rank() > subgroup.rank() and scalar.dim() == _Group->dim()))
            breakingreps.AddTerm(scalar);
        
        }
        else
        {
          //std::cout << "cant break" << std::endl;
        }
      }

      return breakingreps;
    }
    catch (...) { throw; }
  }
  
  /* Calculates the breaking */
  List<Sum<Field> > Theory::calculateBreaking(List<RVector<double> > &mixings)
  {
    try
    {   
      SubGroup subgroup = BreakingChain().extractSubgroups().GetObject(1);

      List<Field> fermions = getFermions();
      List<Field> scalars = getScalars();
      
      List<Field> breakingreps;

      // If there is a + character in the subgroup, there is U(1) mixing
      if(subgroup.id().find("+") != std::string::npos)
      {
              
        SubGroup auxsubgroup = subgroup;
        auxsubgroup.DeleteTerm(-1);
        SubGroup semisimplesubgroup(auxsubgroup.id());
        
        //std::cout << scalars << std::endl;
        breakingreps = findBreakingReps(semisimplesubgroup, scalars);
        
        //std::cout << semisimplesubgroup << std::endl;
        //std::cout << breakingreps << std::endl;
  
        // Calculate the Mixing of the U(1)s
        List<std::string> labels = Strings::split_string(subgroup.labels().GetObject(-1), '+');
        for(auto j=breakingreps.begin(); j!=breakingreps.end(); j++)
        {
          List<double> charges;
          RVector<double> mixing(2);
          for(int i=0; i<labels.nterms(); i++) 
          {
            std::stringstream groupstream;
            groupstream << Strings::split_string(semisimplesubgroup.id(), '[').GetObject(0) << "xU1(" << labels.GetObject(i) << ")[" << _Group->id() << "]";
            SubGroup subgroup(groupstream.str());
            Sum<Field> subreps = j->Decompose(subgroup);
            Sum<Field>::iterator k;
            for(k=subreps.begin(); k!=subreps.end() and k->dim() != 1 and k->HWeight()[-1] == 0; k++);
            charges.AddTerm(k->HWeight()[-1]);
          }
          // In case of two U(1) Mixing 
          if(charges.GetObject(1) == 0)
            mixing[1] = 1;
          else
          {
            mixing[0] = 1;
            mixing[1] = -charges.GetObject(0)/charges.GetObject(1);
          }
          mixings.AddTerm(mixing);
          
        }

        // Create the subgroup with the mixing and calculate the subreps
        List<Sum<Field> > subreps;
        for(int j=0; j<breakingreps.nterms(); j++)
        {
          SubGroup newsubgroup(subgroup.id(), mixings.GetObject(j));
          List<Field> fields = _Fields;
          fields.DeleteObject(breakingreps.GetObject(j),1);
          
          Sum<Field> subrep;
          for(List<Field>::iterator it_fields = fields.begin(); it_fields != fields.end(); it_fields++)
          {
            Sum<Field> auxsubrep = it_fields->Decompose(newsubgroup);
            subrep.AppendList(auxsubrep);
          }
          subreps.AddTerm(subrep);
        }
        
        return subreps;
      }
  
      // There is no U1 mixing here
      breakingreps = findBreakingReps(subgroup,scalars);
      
      List<Sum<Field> > subreps;
      for(int i=0; i<breakingreps.nterms(); i++)
      {
        List<Field> fields = _Fields;
        fields.DeleteObject(breakingreps.GetObject(i),1);
        
        Sum<Field> subrep;
        for(List<Field>::iterator it_fields = fields.begin(); it_fields != fields.end(); it_fields++)
        {
          Sum<Field> auxsubrep = it_fields->Decompose(subgroup);
          subrep.AppendList(auxsubrep);
        }
        subreps.AddTerm(subrep);
      }

      
      return subreps;
      
    }
    catch (...) { throw; }
  }
  
  /* Check if the field content reproduces the SM at least */
  bool Theory::containsSM() const
  {
    try
    {
      // Recast the hypercharge normalisation of the SMreps
/*
      List<Field>::const_iterator pos = _Fields.begin();
      while(pos != _Fields.end() and (pos->dim() != 1 or pos->HWeight()[-1] == 0 or !pos->isFermion())) pos++;
      double norm = pos->HWeight()[-1]/StandardModel::e.HWeight()[-1];

      for(List<Field>::const_iterator it = StandardModel::Reps.begin(); it != StandardModel::Reps.end(); it++)
      {
        Weight HW = it->HWeight();
        HW[-1]*= norm;
        Field SMRep(Rrep(StandardModel::Group, HW), it->LorentzRep()); 
        if(_Fields.Index(SMRep) == -1)
          return false;
      }
*/      
      return true;
      
    }
    catch (...) { throw; }
  }

  /* Normalise the field content to SM normalisation */
  double Theory::normaliseToSM() 
  {
    try
    {
      // If the group is not the SM or the field content does not contain the SM fields, return 0
      if(Group() != StandardModel::Group or !containsSM())
        return 0;
      
      
      // Recast the hypercharge normalisation of the SMreps
      List<Field>::const_iterator pos = _Fields.begin();
      while(pos != _Fields.end() and (pos->dim() != 1 or pos->HWeight()[-1] == 0 or !pos->isFermion())) pos++;
      double norm = sqrt(3./5)*StandardModel::e.HWeight()[-1]/pos->HWeight()[-1];

      List<Field> fields;
      for(List<Field>::const_iterator it = _Fields.begin(); it != _Fields.end(); it++)
      {
        Weight w = it->HWeight();
        w[-1] *= norm; 
        fields.AddTerm(Field(Rrep(it->Group(), w), it->LorentzRep()));
        

      }
      _Fields.Clear();
      _Fields = fields;
      
      _norm = norm;
      
      return norm;
    }
    catch (...) { throw; }
  }
  
  /* Normalises the mixing */
  double Theory::normaliseMixing(double norm)
  {
    try
    {
      if(_Mixing == 0)
        return 0;
      
      _Mixing *= norm;
      
      // If there is a U1, find which of the mixing entries corresponds to it and renormalise its mixing to force a^2 + b^2 = 1
      if(Group().nabelians())
      {
        List<std::string> labels = Strings::split_string(_BreakingChain.GetObject(-1).Branch(-1).label(),'+');
        List<int> indices;
        double norm2 = 1;
        for(auto it_BC = _BreakingChain.begin(); it_BC != _BreakingChain.end() and norm2 == 1; it_BC++)
        {
          int index = labels.Index(it_BC->label());
          if(index != -1 and it_BC->Object().abelian())
          {
            if(_Mixing[index] != 0)
            {
              norm2 = _Mixing[index];
              _Mixing[index] = sqrt(1.-pow(_Mixing[-index-1],2));
              norm2 /= _Mixing[index];
            }
          }
        }
        _norm = norm2;
        
        return norm2;
      }
      
      return 0;

    }
    catch (...) { throw; }
  }
  
  /* Normalise the reps with given normalisation */
  double Theory::normaliseReps(double norm)
  {
    try
    {
      List<Field> fields;
      for(List<Field>::const_iterator it = _Fields.begin(); it != _Fields.end(); it++)
      {
        Weight w = it->HWeight();
        w[-1] *= norm; 
        fields.AddTerm(Field(Rrep(it->Group(), w), it->LorentzRep()));
        

      }
      _Fields.Clear();
      _Fields = fields;
      
      _norm = norm;
      
      return norm;
      
    }
    catch (...) { throw; }
  }
  
  /* Static version of normaliseReps */
  List<Field> Theory::normaliseReps(List<Field> Fields, double norm)
  {
    try
    {
      List<Field> fields;
      for(List<Field>::const_iterator it = Fields.begin(); it != Fields.end(); it++)
      {
        Weight w = it->HWeight();
        w[-1] *= norm; 
        fields.AddTerm(Field(Rrep(it->Group(), w), it->LorentzRep()));
      }
      
      return fields;
    }
    catch (...) { throw; }
  }

  /* Generate all possible combinations of reps up to certain number of reps */
  List<List<Field> > Theory::generatePossibleReps(int nReps)
  {
    try
    {
      // If there are less than 10 reps, don't worry about numbers
      int nreps = 0;
      if(getScalars().nterms() > 10)
        nreps = nReps;
    
      List<List<Field> > possibleReps, possibleScalars;
      possibleScalars = getScalars().Subsets(nreps);
      
      for(auto it = possibleScalars.begin(); it != possibleScalars.end(); it++)
      {
        List<Field> fields(getFermions());
        fields.AppendList(*it);
        possibleReps.AddTerm(fields);
      }
    
      return possibleReps;
      
    }
    catch (...) { throw; }
  }
  
  /* Returns the json string */
  JSONNode Theory::json(std::string id) const
  {   
    JSONNode json;
    
    json.push_back(JSONNode("Group", _Group->id()));
    json.push_back(_BreakingChain.json("BreakingChain"));
    json.push_back(_Fields.json("Fields"));
    if(_norm != 1)
      json.push_back(JSONNode("norm",_norm));
    if(_Mixing != 0)
      json.push_back(_Mixing.json("Mixing"));
    json.push_back(JSONNode("AnomalyFree", _anomalyFree));
    JSONNode observables;
    if(_observables.size())
    {
      observables.set_name("Observables");
      for(auto it = _observables.begin(); it != _observables.end(); it++)
        observables.push_back(JSONNode(it->first, it->second));
      json.push_back(observables);
    }
    
    return json;
  }
  
  /* Parses a json object into the attributes of the class */
  void Theory::ParseJSON(const JSONNode & json) 
  {
    for(JSONNode::const_iterator i = json.begin(); i != json.end(); i++)
    {   
      // get the node name and value as a string
      std::string node_name = i->name();

      // find out where to store the values
      if(node_name == "Group")
      {
        //_Group = i->as_string();
        _Group = new LieGroup(i->as_string());
      }
      else if(node_name =="BreakingChain")
      {
        _BreakingChain.ParseJSON(*i);
      }
      else if(node_name == "Fields")
      {
        _Fields.ParseJSON(*i);
      }
      else if(node_name == "Mixing")
      {
        _Mixing.ParseJSON(*i);
      }
      else if(node_name == "AnomalyFree")
      {
        _anomalyFree = i->as_bool();
      }
    }
  }
  
  /* Overloaded << operator with theories on the right */
  std::ostream &operator<<(std::ostream &stream, const Theory &t)
  {
    stream << t.json().write_formatted();
    return stream;
  }
}
