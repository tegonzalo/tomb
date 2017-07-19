/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
* \file
* irrep.cc
*
* \author
* T. Gonzalo (t.e.gonzalo@fys.uio.no)
*
* \date
* 13/08/2013
*/

#include "irrep.h"
#include "simplegroup.h"
#include "subgroup.h"
#include "rrep.h"

/***************************/
/* Class Irrep definitions */
/***************************/

namespace Tomb
{
  
  // Member functions

  /* Constructor */
  Irrep::Irrep(SimpleGroup &G, Weight &HW)
  {
    try
    {
      _Group = &G;
      _HWeight = new Weight(HW);
      init();
    }
    catch (...) { throw; }

  }

  /* Constructor 2 */
  Irrep::Irrep(const string id)
  {
    try
    {
      _HWeight = new Weight(id);
      _Group = DB<SimpleGroup>().get(_HWeight->GroupId());
      
      init();
      
    }
    catch (...) { throw; }
  }

  /* Constructor 3, with json nodes */
  Irrep::Irrep(const JSONNode &n)
  {
    if(n.as_string() != "")
      Irrep(n.as_string());
    else
      ParseJSON(n);
  }

  /* Copy constructor */
  Irrep::Irrep(const Irrep &Rep)
  {  
    try
    {
      _Group = &Rep.Group();
      _dim = Rep.dim();
      _real = Rep.real();
      _conjugate = Rep.conjugate();
      _congruency = Rep.congruency();
      _HWeight = new Weight(Rep.HWeight());
      _label = Rep.label();
      _Casimir = Rep.Casimir();
      _DynkinIndex = Rep.DynkinIndex();
      //_Weights = Rep.Weights();
      //_DualWeights = Rep.DualWeights();

    }
    catch (...) { throw; }
  }

  /* Move constructor */
  Irrep::Irrep(Irrep &&Rep) :
    _Group(move(Rep._Group)),
    _dim(move(Rep._dim)),
    _real(move(Rep._real)),
    _conjugate(move(Rep._conjugate)),
    _congruency(move(Rep._congruency)),
    _HWeight(move(Rep._HWeight)),
    _label(move(Rep._label)),
    _Casimir(move(Rep._Casimir)),
    _DynkinIndex(move(Rep._DynkinIndex)),
    _Weights(move(Rep._Weights)),
    _DualWeights(move(Rep._DualWeights))
    
  {
    try
    {
      Rep._Group = NULL;
      Rep._dim = 0;
      Rep._real = false;
      Rep._conjugate = 0;
      Rep._congruency = RVector<int>();
      Rep._HWeight = NULL;
      Rep._label = "";
      Rep._Casimir = 0;
      Rep._DynkinIndex = 0;
      Rep._Weights.Clear();
      Rep._DualWeights.Clear();

    }
    catch (...) { throw; }
  }

  /* Destructor */
  Irrep::~Irrep()
  {
    delete _HWeight;
  }

  /* Overloaded = operator */
  Irrep &Irrep::operator=(const Irrep &Rep)
  {
    try
    {
      if(this == &Rep) return *this;
      _Group = &Rep.Group();
      _dim = Rep.dim();
      _real = Rep.real();
      _conjugate = Rep.conjugate();
      _congruency = Rep.congruency();
      _HWeight = new Weight(Rep.HWeight());
      _label = Rep.label();
      _Casimir = Rep.Casimir();
      _DynkinIndex = Rep.DynkinIndex();
      _Weights = Rep.Weights();
      _DualWeights = Rep.DualWeights();
      return *this;
    }
    catch (...) { throw; }
  }

  /* Move = operator */
  Irrep &Irrep::operator=(Irrep &&Rep)
  {
    try
    {
      if(this == &Rep) return *this;
      this->~Irrep();
      _Group = move(Rep._Group);
      _dim = move(Rep._dim);
      _real = move(Rep._real);
      _conjugate = move(Rep._conjugate);
      _congruency = move(Rep._congruency);
      _HWeight = move(Rep._HWeight);
      _label = move(Rep._label);
      _Casimir = move(Rep._Casimir);
      _DynkinIndex = move(Rep._DynkinIndex);
      _Weights = move(Rep._Weights);
      _DualWeights = move(Rep._DualWeights);
      
      Rep._Group = NULL;
      Rep._dim = 0;
      Rep._real = false;
      Rep._conjugate = 0;
      Rep._congruency = RVector<int>();
      Rep._HWeight = NULL;
      Rep._label = "";
      Rep._Casimir = 0;
      Rep._DynkinIndex = 0;
      Rep._Weights.Clear();
      Rep._DualWeights.Clear();
      
      return *this;
    }
    catch (...) { throw; }
  }

  /* Initialises some internal variables */
  void Irrep::init()
  {
    try
    {
      if(_Group == NULL or !_HWeight->cols())
        throw "Irrep::init::Not enough information to initialise the variables";
      
      double aux = 1;    

      double a,b;
      int nproots = (int)(0.5*(_Group->dim()-_Group->rank()));
      if(nproots)
      {
        List<Root> PRoots(_Group->PRoots("Dual"));
        for(int i=0; i<nproots; i++)
        {
          a = 0;
          b = 0;
          for(int j=0; j<_Group->rank(); j++)
          {
            a += PRoots.GetObject(i)[j]*((*_HWeight)[j]+1);
            b += PRoots.GetObject(i)[j];
          }
          aux *= a*1./b;
        }
      }
      _dim = (int) aux;
      if((aux - _dim) > 0.5)
        _dim++;
      
      if(_dim < 0)
        throw "Irrep::init::Dimension cannot be negative";
        
      switch(_Group->type())
      {
        case 'A':
          _real = true;
          for(int i=0; i<_Group->rank(); i++)
            if((*_HWeight)[i] != (*_HWeight)[_Group->rank()-i-1])
              _real = false;
          break;

        case 'B':
          _real = true;
          break;

        case 'C':
          _real = true;
          break;

        case 'D':
          if(_Group->rank()%2)
          {
            if((*_HWeight)[_Group->rank()-1] == (*_HWeight)[_Group->rank()-2]) 
              _real = true;
            else
              _real = false;
          }
          else
            _real = true;
          break;

        case 'E':
          if(_Group->rank() != 6)
            _real = true;
          else
          {
            _real = true;
            for(int i=0; i<_Group->rank()-1; i++)
                if((*_HWeight)[i] != (*_HWeight)[_Group->rank()-i-2])
                  _real = false;
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
      if(_Group->abelian())
        sprintf(label, "%.3f", (*_HWeight)[0]);
      else
        sprintf(label, "%d", _dim);

      _label = label;

      if(_real == false and _Group->rank() > 1 and _dim > 0)
      {
        _conjugate = 1;	
        switch(_Group->type())
        {
          case 'A':
            for(int i=0; i<_Group->rank()/2;i++)
              if((*_HWeight)[i] > (*_HWeight)[_Group->rank()-1-i])
                _conjugate = 0;
            break;

          case 'D':
            if((*_HWeight)[_Group->rank()-1] < (*_HWeight)[_Group->rank()-2])
              _conjugate = 0;
            break;

          case 'E':
            for(int i=0; i<(_Group->rank()-1)/2;i++)
              if((*_HWeight)[i] > (*_HWeight)[_Group->rank()-2-i])
                _conjugate = 0;
            break;
        }
      }

      _congruency = RVector<int>(1);
      switch(_Group->type())
      {
        case 'A':
          _congruency[0] = 0;
          for(int i=0; i<_Group->rank(); i++)
            _congruency[0] += (*_HWeight)[i];
          _congruency[0] %= _Group->rank() + 1;
          break;

        case 'B':
          _congruency[0] = (int) (*_HWeight)[_Group->rank()-1] % 2;
          break;

        case 'C':
          _congruency[0] = 0;
          for(int i=0; i<_Group->rank(); i+=2)
            _congruency[0] += (*_HWeight)[i];
          _congruency[0] %= 2;
          break;

        case 'D':
          _congruency = RVector<int>(2);
          _congruency[0] = (int)((*_HWeight)[_Group->rank()-1] + (*_HWeight)[_Group->rank()-2]) % 2;
          _congruency[1] = 0;
          for(int i=0; i<_Group->rank()-2; i++)
            _congruency[1] += 2*(*_HWeight)[i];
          _congruency[1] += (_Group->rank()-2)*(*_HWeight)[_Group->rank()-2] + _Group->rank()*(*_HWeight)[_Group->rank()-1];
          _congruency[1] %= 4;
          break;

        case 'E':
          if(_Group->rank() == 6)
            _congruency[0] = (int)((*_HWeight)[0] - (*_HWeight)[1] + (*_HWeight)[3] - (*_HWeight)[4]) % 3;
          else if(_Group->rank() == 7)
            _congruency[0] = (int)((*_HWeight)[3] + (*_HWeight)[5] + (*_HWeight)[6]) % 2;
          else
            _congruency[0] = 0;
          break;

        case 'F':
          _congruency[0] = 0;
          break;

        case 'G':
          _congruency[0] = 0;
          break;

        case 'U':
          _congruency[0] = 0;
          break;
      }
      
      if(_conjugate)
        _label.append("*");
        
      if(_Group->abelian())
      {
        _Casimir = (*_HWeight)[0]*(*_HWeight)[0];
        _DynkinIndex = _Casimir;
      }
      else
      {
        _Casimir = 0;
        for(int i=0; i<_Group->rank(); i++)
          for(int j=0; j<_Group->rank(); j++)
            _Casimir += 0.5*(*_HWeight)[i]*_Group->G()[i][j]*((*_HWeight)[j]+2);
        _DynkinIndex = _Casimir*_dim/_Group->dim();
      }

      // If the irrep is already in the database, do nothing
      if(DB<Irrep>().find(id()) != NULL)
        return ;
      // Store the info in the database
      DB<Irrep>().set(id(),this);  

      // Perhaps do not calculate weights for very large reps
      if(_dim < 5000)
        DB<Irrep>().at(id())->CalculateWeights();

    } 
    catch (...) { throw; }
  }

  /* Identifier of the irrep */
  string Irrep::id() const 
  {
    stringstream s;
    s << _HWeight->id();
    return s.str();
  }

  /* Returns the simple Group */
  SimpleGroup &Irrep::Group() const 
  {
    return *_Group;
  }
  
  /* Returns dim */
  int Irrep::dim() const
  {
    return _dim;
  }

  /* Returns real */
  bool Irrep::real() const
  {
    return _real;
  }

  /* Returns conjugate */
  int Irrep::conjugate() const
  {
    return _conjugate;
  }

  /* Returns the congruency number */
  RVector<int> Irrep::congruency() const
  {
    return _congruency;
  }

  /* Returns label */
  string Irrep::label() const
  {
    return _label;
  }

  /* Returns HWeight */
  Weight& Irrep::HWeight() const
  {
    return *_HWeight;
  }

  /* Returns the weight list */
  List<Weight> Irrep::Weights() const
  {
    Irrep *R = DB<Irrep>().find(id());
    if(R != NULL)
      return R->_Weights;
    return _Weights;
  }

  List<Weight> Irrep::DualWeights() const
  {
    Irrep *R = DB<Irrep>().find(id());
    if(R != NULL)
      return R->_DualWeights;
    return _DualWeights;
  }
  
  /* Sets the weights */
  void Irrep::setWeights(const List<Weight> &Weights)
  {
    _Weights = Weights;
    
    if(DB<Irrep>().find(id()) != NULL)
      DB<Irrep>().at(id())->setWeights(Weights);
  }

   /* Sets the label */
  void Irrep::setLabel(string label)
  {
    try
    {
      _label = label;

      // Replace the DB irrep if label is different
      if(DB<Irrep>().find(id()) != NULL and DB<Irrep>().at(id())->label() != label)
        DB<Irrep>().at(id())->setLabel(label);      

    }
    catch (...) { throw; }
  }

  /* Calculates the weights diagram */
  List<Weight> &Irrep::CalculateWeights()
  {
    try
    {  
      if(_Weights.nterms()) return _Weights;
      _Weights.Clear();

      int rank = _Group->rank();

      // Reserve enough space for all the weights
      _Weights.reserve(_dim);
      _DualWeights.reserve(_dim);
      _Weights.AddTerm(*_HWeight);
      
      if(_Group->abelian() or _dim == 1)
      {
        for(auto it_Weights = _Weights.begin(); it_Weights != _Weights.end(); it_Weights++)
          _DualWeights.AddTerm(it_Weights->Dual());
        return _Weights;
      }

      int n = 0;
      
      for(List<Weight>::iterator it_Weights = _Weights.begin(); it_Weights != _Weights.end(); ++ it_Weights) {
        for(int j=0; j<rank; j++)
        {
          n = round((*it_Weights)[j]);
          if(n>0){
            for(int l=1; l<=n; l++) {
              Weight w = *it_Weights - _Group->SRoot(j)*(double)l;
              w.setLevel(it_Weights->level()+l);
              if(_Weights.Index(w) < 0) {
                _Weights.AddTermOrdered(w, "ASC");
              }
            }
          }
        }
      }
      // Already ordered
      //_Weights.Order("ASC");

      // If there are less terms than the dimension of the rep calculate multiplicities
      if(_Weights.nterms() < _dim)
      {
        List<Root> PRoots = _Group->PRoots();
        int i = 0;
        for(List<Weight>::iterator it_Weights = _Weights.begin(); it_Weights != _Weights.end() and _Weights.nterms() < _dim; it_Weights++)
        {
          Weight w = *it_Weights;
          double multiplicity = 0;
          for(List<Root>::iterator it_PRoots = PRoots.begin(); it_PRoots != PRoots.end(); it_PRoots++)
          {
            Root alpha = *it_PRoots;
            int k = 1;
            int index = 0;
            do {
              Weight v = w + alpha*k;
              if((index=_Weights.Index(v)) >=0) {
                multiplicity += 2.0*_Weights.GetObject(index).multiplicity() * v*alpha;
              }
              k++;
            } while(index >=0);
          }
          double norm = ((*_HWeight)++)*((*_HWeight)++) - (w++)*(w++);
          if(norm > 0)
            multiplicity /= norm;
          else
            multiplicity = 1;
         
          _Weights.DeleteTerm(i);
          w.setMultiplicity((int)round(multiplicity));
          for(int j=0; j<w.multiplicity(); j++)
          {
            _Weights.InsertTerm(i,w);
            i++;
          }
          it_Weights += w.multiplicity()-1;
        }
      }

      if(_Weights.nterms() < _dim)
        throw "Irrep::Weights::Error in the weight calculation";
      
      // Calculate also the dual weights
      for(auto it_Weights = _Weights.begin(); it_Weights != _Weights.end(); it_Weights++)
        _DualWeights.AddTerm(it_Weights->Dual());

      return _Weights;

    } catch (...) {
      throw;
    }
  }

  /* Direct product of representations */
  Sum<Irrep> &Irrep::operator*(const Irrep &Rep)
  {
    try
    {
      Irrep *R = DB<Irrep>().find(id());
      if(R != NULL and !R->_Products.empty() and R->_Products.find(Rep.id()) != R->_Products.end())
        return R->_Products.at(Rep.id());
      if(R == NULL)
        throw "Irrep::Unknown irrep, cannot continue";

      Sum<Irrep> Reps;
      
      int totaldim = this->dim()*Rep.dim();
      int nreps = 0;

      List<Weight> W;
      for(int i=0; i<totaldim; i++)
      {
        if(_dim <= Rep.dim())
        {
          Weight w = R->_Weights.GetObject(i/Rep.dim()) + Rep.Weights().GetObject(i%Rep.dim());
          W.AddTerm(w);
        }
        else
        {
          Weight w = Rep.Weights().GetObject(i/_dim) + R->_Weights.GetObject(i%_dim);
          W.AddTerm(w);
        }
      }

      int n;

      do
      {
        Weight HW(*_Group,_Group->rank());
        int count = 0;
        do
        {
          HW = W.GetObject(count);
          count++;
        }
        while(!HW.positive() and HW != 0 and count < W.nterms());

        HW.setMultiplicity(1);
        Irrep *rep = DB<Irrep>().get(HW.id());
        for(int i=0; i<rep->dim(); i++)
        {
          n = W.Index(rep->_Weights.GetObject(i));
          if(n >= 0)
            W.DeleteTerm(n);
        }	
        totaldim -= rep->dim();

        Reps.AddTerm(*rep);
        nreps++;
        
      } 
      while(totaldim>0 and W.nterms() > 0);

      if(!W.nterms() and totaldim)
        throw "Irrep::operator*::Error calculating the product";

      Reps.Order();

      R->_Products.emplace(Rep.id(), Reps);
      Irrep *R2 = DB<Irrep>().find(Rep.id());
      if(R2 != NULL)
        R2->_Products.emplace(id(), Reps);

      return R->_Products.at(Rep.id());
      
    }
    catch (...) { throw; }

  }

  /* Direct product of a list of representations */
  Sum<Irrep> Irrep::Product(List<Irrep> &ListofReps)
  {   
    try
    {
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

    }
    catch (...) { throw; }
  }	

  /* Returns whether the irrep is the conjugate of a given irrep */
  bool Irrep::isConjugateOf(Irrep R)
  {   
    try
    {  
      if(_real or _dim != R.dim())
        return false;
    
      switch(_Group->type())
      {
        case 'A':
          for(int i=0; i<_Group->rank(); i++)
            if((*_HWeight)[i] != R.HWeight()[_Group->rank()-i-1])
              return false;
          return true;

        case 'D':
          if(_Group->rank()%2)
            if((*_HWeight)[_Group->rank()-1] == R.HWeight()[_Group->rank()-2])
              return true;
          return false;

        case 'E':
          if(_Group->rank() != 6)
            return false;
         
          for(int i=0; i<_Group->rank()-1; i++)
            if((*_HWeight)[i] != R.HWeight()[_Group->rank()-i-2])
              return false;
          return true;
      }
      return false;
    
    }
    catch (...) { throw; }
  }

  /* Returns the first Casimir of the representation */
  double Irrep::Casimir() const
  {
    return _Casimir;
  }

  /* Returns the Dynkin index of a representation */
  double Irrep::DynkinIndex() const
  {
    return _DynkinIndex;
  }

  /* Check whether the rep is a singlet or not */
  bool Irrep::isSinglet() const
  {
    if(dim() == 1)
    {
      if(_Group->abelian() and (*_HWeight)[0] != 0) return false;

      return true;
    }
    
    return false;
  }

  /* Projects the weights of a irrep into the weights of irreps of a subgroup */
  List<Weight> Irrep::Project(SubGroup &Subgroup)
  {    
    try
    {
      List<Weight> ProjectedWeights;
      for(auto it = Weights().begin(); it != Weights().end(); it++)
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

  /* Obtains the decomposition into irreps of a subgroup */
  Sum<Rrep> Irrep::Decompose(SubGroup &Subgroup)
  {   
    try
    {   
      Irrep *R = DB<Irrep>().find(id());
      if(R == NULL)
        throw "Irrep::Decompose::Irrep not in database";

      if(R != NULL and !R->_Subreps.empty() and R->_Subreps.find(Subgroup.id()) != R->_Subreps.end())
        return R->_Subreps.at(Subgroup.id());

    
      if(!Subgroup.isSubgroupOf(Group()))
        throw "Irrep:Decompose:Not a subgroup";
   
      List<Weight> ProjectedWeights = Project(Subgroup);
    
      Sum<Rrep> Reps;
        
      do
      {
        Weight HWeight = ProjectedWeights.GetObject(0);
        int maximum_sum_of_values = 0;
        for(int i=0; i<ProjectedWeights.nterms(); i++)
        {
          int sum_of_values = 0;
          int group = 0;
          int acc_rank = Subgroup.GetObject(0).rank();
          Weight weight = ProjectedWeights.GetObject(i);
          for(int j=0; j<Subgroup.rank(); j++)
          {
            if(j >= acc_rank)
            {
              group++;
              acc_rank += Subgroup.GetObject(group).rank();
            }
            if(!Subgroup.GetObject(group).abelian())
              if(weight[j] >= 0)
                sum_of_values += (int)weight[j];
          }
          if(weight.positive())
          {
            if(sum_of_values > maximum_sum_of_values)
            {
              maximum_sum_of_values = sum_of_values;
              HWeight = weight;
            }
          }
        }
        Rrep *Rep = DB<Rrep>().find(HWeight.id());
        if(Rep == NULL)
        {
          Rep = new Rrep(Subgroup, HWeight);
          DB<Rrep>().set(Rep->id(), Rep);
        }
        for(int i=0; i<Rep->_Weights.nterms(); i++)
        {
          int n = ProjectedWeights.Index(Rep->_Weights.GetObject(i));
          if(n >= 0)
              ProjectedWeights.DeleteTerm(n);
        }
        Reps.AddTerm(*Rep);
        }
        while(ProjectedWeights.nterms()>0);
     
 
      int repsdim=0;
      for(auto it = Reps.begin(); it != Reps.end(); it++)
        repsdim += it->dim();
      if(repsdim != dim())
        throw "Irrep::Decompose::Dimension of the result doesn't match the dimension of the rep";
      
      R->_Subreps.emplace(Subgroup.id(), Reps);

      return R->_Subreps.at(Subgroup.id());

    }
    catch (...) { throw; }
  }

  /* Overloaded == operator */
  bool Irrep::operator==(const Irrep R) const
  {
    if(*_Group == R.Group() && *_HWeight == R.HWeight())
      return true;
    else 
      return false;
  }

  /* Overloaded != operator */
  bool Irrep::operator!=(const Irrep R) const
  {
    if(*this == R)
      return false;
    else
      return true;
  }

  /* Overloaded > operator */
  bool Irrep::operator>(const Irrep R) const
  {
    try
    {
      if(this->dim() > R.dim())
        return true;
      else if(this->dim() == R.dim() && this->DynkinIndex() > R.DynkinIndex())
        return true;
      return false;
    }
    catch (...) { throw; }
  }

  /* Overloaded < operator */
  bool Irrep::operator<(const Irrep R) const
  {
    try
    {
      if(this->dim() < R.dim())
        return true;
      else if(this->dim() == R.dim() && this->DynkinIndex() < R.DynkinIndex())
        return true;
      return false;
    }
    catch (...) { throw; }
  }

  /* Prints the representation */
  string Irrep::Print() const
  {
    return this->label();
  }

  /* Prints the representation for table manipulation*/
  string Irrep::Table() const {
    if(this->label()[this->label().length()-1] == '*' ||
       this->label()[this->label().length()-1] == '\'')
    {
      string label = this->label();
      label.insert(0,"ToString[\"");
      label.append("\", StandardForm]");
      return label;
    }
    
    return this->label();
  }

  /* Returns the json string */
  JSONNode Irrep::json(string name) const
  {
    if(name == "id") {
      return JSONNode("", id());
    }
    
    JSONNode json;
      
    json.push_back(JSONNode("id", id()));
    json.push_back(JSONNode("Group", _Group->id()));
    json.push_back(_HWeight->json("HWeight"));
    json.push_back(JSONNode("nirreps", _nirreps));
    json.push_back(JSONNode("dim", dim()));
    json.push_back(JSONNode("real", real()));
    json.push_back(JSONNode("label", label()));
    json.push_back(JSONNode("conjugate", conjugate()));
    json.push_back(JSONNode("Casimir", Casimir()));
    json.push_back(JSONNode("DynkinIndex",DynkinIndex()));
    json.push_back(_congruency.json("congruency"));
    json.push_back(_Weights.json("Weights"));
    json.push_back(_DualWeights.json("DualWeights"));

    if(!_Subreps.empty())
    {
      JSONNode Subreps;
      Subreps.set_name("Subreps");
      for(auto it = _Subreps.begin(); it != _Subreps.end(); it++)
        Subreps.push_back(it->second.json(it->first));
      json.push_back(Subreps);
    }

    if(!_Products.empty())
    {
      JSONNode Products;
      Products.set_name("Products");
      for(auto it = _Products.begin(); it != _Products.end(); it++)
        Products.push_back(it->second.json(it->first));
      json.push_back(Products);
    }
      
    return json;
  }

  /* Parses a json object into the attributes of the class */
  void Irrep::ParseJSON(const JSONNode &n)
  {
    JSONNode::const_iterator i = n.begin();
    while (i != n.end())
    {
      // get the node name and value as a string
      string node_name = i -> name();
      
      // find out where to store the values
      if(node_name == "Group")
        _Group = DB<SimpleGroup>().at(i->as_string());
      else if(node_name =="HWeight")
      {
        _HWeight = new Weight(*_Group, _Group->rank());
        _HWeight->ParseJSON(*i);
      }
      else if(node_name == "nirreps")
        _nirreps = i->as_int();
      else if(node_name == "dim")
        _dim = i->as_int();
      else if(node_name == "real")
        _real = i->as_bool();
      else if(node_name == "label")
        _label = i->as_string();
      else if(node_name == "Casimir")
        _Casimir = i->as_float();
      else if(node_name == "DynkinIndex")
        _DynkinIndex = i->as_float();
      else if(node_name == "conjugate")
        _conjugate = i->as_bool();
      else if(node_name == "congruency")
        _congruency.ParseJSON(*i);
      else if(node_name == "Weights")
      {
        Irrep *R = DB<Irrep>().set(id(), this);
        R->_Weights.ParseJSON(*i);
      }
      else if(node_name == "Subreps")
      {
        Irrep *R = DB<Irrep>().set(id(), this);
        for(auto j = i->begin(); j != i->end(); j++)
          R->_Subreps[j->name()].ParseJSON(*j);
      }
      else if(node_name == "Products")
      {
        Irrep *R = DB<Irrep>().set(id(), this);
        for(auto j = i->begin(); j != i->end(); j++)
          R->_Products[j->name()].ParseJSON(*j);
      }
  
      //increment the iterator
      ++i;
    }
  }


  /* Overloaded << operator with irreps on the right */
  ostream &operator<<(ostream &stream, const Irrep &i)
  {
    stream << i.label();
    return stream;
  }

}
