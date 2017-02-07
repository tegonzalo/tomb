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
      _HWeight = &HW;
      init();
    }
    catch (...) { throw; }

  }

  /* Constructor 2 */
  Irrep::Irrep(const string id)
  {
    try
    {
      _HWeight = Weight::get(id);
      _Group = SimpleGroup::get(_HWeight->Group().id());
      
      init();
      
    }
    catch (...) { throw; }
  }

  /* Constructor 3, with json nodes */
/*  Irrep::Irrep(const JSONNode &n)
  {
    if(n.as_string() != "")
    {
      _HWeight = new Weight(n.as_string());
      //_Group = new SimpleGroup(HWeight().Group().GetObject(0));
      //_Group = new SimpleGroup(HWeight().Group());
      _Group = HWeight().GroupId();
      
      init();
      
    } 
    else
    {
      ParseJSON(n);
    }
  }
Si*/
  /* Copy constructor */
  Irrep::Irrep(const Irrep &Rep)
  {  
    try
    {
      cout << "ir::cpct" << endl;
      _Group = &Rep.Group();
      _dim = Rep.dim();
      _real = Rep.real();
      _conjugate = Rep.conjugate();
      _congruency = Rep.congruency();
      _HWeight = &Rep.HWeight();
      _label = Rep.label();
      _Casimir = Rep.Casimir();
      _DynkinIndex = Rep.DynkinIndex();
      _Weights = Rep.Weights();
      _DualWeights = Rep.DualWeights();

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
      Rep._HWeight = nullptr;
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
    //cout << "deleting irrep" << endl;
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
      Rep._HWeight = nullptr;
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
      if(_Group == NULL or _HWeight == NULL)
        throw "Irrep::init::Not enough information to initialise the variables";
      
      double aux = 1;    
   
      double a,b;
      int nproots = (int)(0.5*(_Group->dim()-_Group->rank()));
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
              if((*_HWeight)[i]>(*_HWeight)[_Group->rank()-1-i])
                _conjugate = 0;
            break;

          case 'D':
            if((*_HWeight)[_Group->rank()-1]<(*_HWeight)[_Group->rank()-2])
              _conjugate = 0;
            break;

          case 'E':
            for(int i=0; i<(_Group->rank()-1)/2;i++)
              if((*_HWeight)[i]>(*_HWeight)[_Group->rank()-2-i])
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
        
      _Weights = CalculateWeights();
        
      // Store the info in the database
      DB<Irrep>().set(id(),this);  
      
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

  /* Sets the label */
  void Irrep::setLabel(string label)
  {
    try
    {
      _label = label;
      
      //database_emplace(id(), *this);
      
      // Output the info
      /*ostringstream OutputDirectory;
      OutputDirectory << "./out/" << _Group->id() << "/reps";

      mkdir(OutputDirectory.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

      ostringstream OutputFileName;
      OutputFileName << OutputDirectory.str() << "/" << id() << ".out";

      ofstream OutputFile;
      OutputFile.open(OutputFileName.str().c_str());
      OutputFile << json().write_formatted() << endl;
      OutputFile.close();*/

    }
    catch (...) { throw; }
  }

  /* Returns HWeight */
  Weight &Irrep::HWeight() const
  {
    return *_HWeight;
  }

  /* Returns the weight list */
  List<Weight> Irrep::Weights() const
  {
    return _Weights;
  }

  List<Weight> Irrep::DualWeights() const
  {
    return _DualWeights;
  }
  
  /* Sets the weights */
  void Irrep::setWeights(const List<Weight> &Weights)
  {
    _Weights = Weights;
  }
  
  /* Calculates the weights diagram */
  List<Weight> &Irrep::CalculateWeights()
  {
    try
    {  

      int rank = _Group->rank();

      // Look for the weights in the database			
      /*if(database_check(id(), "Weights") and DataBase.at(id()).hasWeights()) {
        _Weights = DataBase.at(id()).Weights();
        if(_Weights.nterms()) _hasWeights = true;
        if(basis == "DUAL") {
          for(List<Weight>::iterator it_Weights = _Weights.begin(); it_Weights != _Weights.end(); it_Weights ++)
            _DualWeights.AddTerm(it_Weights->Dual());
          return _DualWeights;
        }
        return _Weights;
      }*/
    
      //cout << "Calculating weights of " << *this << :endl;
      // Reserve enough space for all the weights
/*      _Weights.reserve(dim());
      _Weights.AddTerm(*_HWeight);
      
      if(_Group->abelian()) {
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
      //cout << _Weights << endl;

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
              //cout << v << endl;
              if((index=_Weights.Index(v)) >=0) {
                //cout << "index = " << index << endl;
                //cout << v*alpha << endl;
                multiplicity += 2.0*_Weights.GetObject(index).multiplicity() * v*alpha;
              }
              k++;
            } while(index >=0);
            //cout << "multiplicity = " << multiplicity << endl;
          }
          double norm = ((*_HWeight)++)*((*_HWeight)++) - (w++)*(w++);
          //cout << "norm = " << norm << endl;
          if(norm > 0) {
            multiplicity /= norm;
          } else {
            multiplicity = 1;
          }
          //cout << "final multiplicity = " << multiplicity << endl;
          _Weights.DeleteTerm(i);
          w.setMultiplicity((int)round(multiplicity));
          for(int j=0; j<w.multiplicity(); j++) {
            _Weights.InsertTerm(i,w);
            //cout << _Weights.GetObject(i) << endl;
            i++;
          }
          it_Weights += w.multiplicity()-1;
          //cout << "multilpicity of " << _Weights.GetObject(i-w.multiplicity()+1) << " is " << _Weights.GetObject(i-w.multiplicity()+1).multiplicity() << endl;
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
      //database_emplace(id(), *this);
      
      // Output the info
      /*ostringstream OutputDirectory;
      OutputDirectory << "./out/" << _Group->type() << _Group->rank() << "/reps";

      mkdir(OutputDirectory.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
      ostringstream OutputFileName;
      OutputFileName << OutputDirectory.str() << "/" << id() << ".out";

      ofstream OutputFile;
      OutputFile.open(OutputFileName.str().c_str());
      OutputFile << json().write_formatted() << endl;
      OutputFile.close();
      */
      
      return _Weights;

    } catch (...) {
      throw;
    }
  }

  /* Direct product of representations */
  Sum<Irrep> Irrep::operator*(Irrep R)
  {
    try
    {
      Sum<Irrep> Reps;
      
      /*if(ProductDataBase.find(pair<string,string>(id(),R.id())) != ProductDataBase.end())
      {
        Reps = ProductDataBase.at(pair<string,string>(id(),R.id()));
        return Reps;
      }*/
      
      int totaldim = this->dim()*R.dim();
      int nreps = 0;

      List<Weight> R1Weights = this->Weights();
      List<Weight> R2Weights = R.Weights();
      List<Weight> W;

      for(int i=0;i<totaldim;i++)
      {
        if(_dim <= R.dim())
        {
          Weight w = R1Weights.GetObject(i/R.dim()) + R2Weights.GetObject(i%R.dim());
          W.AddTerm(w);
        }
        else
        {
          Weight w = R2Weights.GetObject(i/this->dim()) + R1Weights.GetObject(i%this->dim());
          W.AddTerm(w);
        }
      }

      //cout << W << endl;

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
        Irrep Rep(*_Group, HW);

        List<Weight> RepWeights = Rep.Weights();

        for(int i=0; i<Rep.dim(); i++)
        {
          n = W.Index(RepWeights.GetObject(i));
          //cout << " n = " << n << endl;
          //cout << "RepWeights(" << i << ") = " << RepWeights.GetObject(i) << endl;
          //cout << "W(n) = " << W.GetObject(n) << endl;
          if(n >= 0)
            W.DeleteTerm(n);
        }	
        totaldim -= Rep.dim();

        //cout << W << endl;

        //cout << "Dimension = " << Rep.dim() << endl;
        Reps.AddTerm(Rep);
        nreps++;
        //cout << "nreps = " << nreps << endl;
        //cout << "totaldim = " << totaldim << endl;
        
      } 
      while(totaldim>0 and W.nterms() > 0);

      if(!W.nterms() and totaldim)
        throw "Irrep::operator*::Error calculating the product";

      Reps.Order();

      /*if(ProductDataBase.find(pair<string,string>(id(),R.id())) != ProductDataBase.end())
        ProductDataBase.erase(pair<string,string>(id(),R.id()));
      ProductDataBase.emplace(pair<string,string>(id(),R.id()), Reps);
      */
      return Reps;
      
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
            if(this->HWeight()[i] != R.HWeight()[_Group->rank()-i-1])
              return false;
          return true;

        case 'D':
          if(_Group->rank()%2)
            if(this->HWeight()[_Group->rank()-1] == R.HWeight()[_Group->rank()-2])
              return true;
          return false;

        case 'E':
          if(_Group->rank() != 6)
            return false;
         
          for(int i=0; i<_Group->rank()-1; i++)
            if(this->HWeight()[i] != R.HWeight()[_Group->rank()-i-2])
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
      if(_Group->abelian() and _HWeight[0] != 0) return false;

      return true;
    }
    
    return false;
  }

  /* Projects the weights of a irrep into the weights of irreps of a subgroup */
/*  List<Weight> Irrep::Project(SubGroup Subgroup)
  {
    
    try {
      List<Weight> Weights = this->Weights();
      //cout << Weights.nterms() << endl;
      List<Weight> ProjectedWeights;
      //cout << Subgroup.Projection() << endl;
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
*/
  /* Obtains the decomposition into irreps of a subgroup */
/*  Sum<Rrep> Irrep::Decompose(SubGroup Subgroup) {
    
    try {
      
      //cout << "decomposing " << *this << " into " << Subgroup << endl;
      Sum<Rrep> Reps;
      
      /*if(DecomposeDataBase.find(pair<string,string>(id(),Subgroup.id())) != DecomposeDataBase.end())
      {
        Reps = DecomposeDataBase.at(pair<string,string>(id(),Subgroup.id()));
        return Reps;
      }*/
    
/*      if(Subgroup.isSubgroupOf(this->Group())) {
        List<Weight> ProjectedWeights = this->Project(Subgroup);
        //cout << "Projected Weights = "<< endl << ProjectedWeights.Print() << endl;
        
        Subgroup.Order();
        do {
          //cout << "Projected Weights = "<< endl << ProjectedWeights.Print() << endl;
          Weight HWeight = ProjectedWeights.GetObject(0);
          int maximum_sum_of_values = 0;
          for(int i=0; i<ProjectedWeights.nterms(); i++) {
            int sum_of_values = 0;
            int group = 0;
            int acc_rank = Subgroup.GetObject(0).rank();
            Weight weight = ProjectedWeights.GetObject(i);
            //cout << "weight = " << weight << " is positive? " << weight.positive() << endl;
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
          //cout << Subgroup << endl;
          //cout << HWeight << endl;
          Rrep Rep(Subgroup, HWeight);
          //cout << "The rep is " << Rep << ", " << Rep.id() << endl;
          //cout << Rep.json().write_formatted() << endl;
          List<Weight> SubWeights = Rep.Weights();
          //cout << SubWeights << endl;
          for(int i=0; i<SubWeights.nterms(); i++) {
            //cout << "i = " << i << endl;
            int n = ProjectedWeights.Index(SubWeights.GetObject(i));
            //cout << "n = " << n << endl;
            if(n >= 0) {
              ProjectedWeights.DeleteTerm(n);
            }
          }
          Reps.AddTerm(Rep);
        
        } while(ProjectedWeights.nterms()>0);
      
      } else {
        throw "Irrep:Decompose:Not a subgroup";
      }
      //cout << Reps << endl;
      int dim=0;
      for(int i=0; i<Reps.nterms(); i++) {
        dim += Reps.GetObject(i).dim();
        //cout << Reps.GetObject(i) << endl;
        //cout << dim << endl;
      }
      if(dim != this->dim()) {
        throw "Irrep::Decompose::Dimension of the result doesn't match the dimension of the rep";
      }
      
      /*if(DecomposeDataBase.find(pair<string,string>(id(),Subgroup.id())) != DecomposeDataBase.end())
        DecomposeDataBase.erase(pair<string,string>(id(),Subgroup.id()));
      DecomposeDataBase.emplace(pair<string,string>(id(),Subgroup.id()), Reps);
      */
/*      return Reps;

    } catch (...) {
      throw;
    }

  }
*/
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
/*  JSONNode Irrep::json(string name) const {
    
    if(name == "id") {
      return JSONNode("", id());
    }
    
    JSONNode json;
      
    json.push_back(JSONNode("id", id()));
    //json.push_back(JSONNode("Group", _Group->id()));
    json.push_back(JSONNode("Group", _Group));
    json.push_back(JSONNode("Group->rank()", _Group->rank()));
    json.push_back(_HWeight->json("HWeight"));
    json.push_back(JSONNode("nirreps", _nirreps));
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
*/
  /* Parses a json object into the attributes of the class */
/*  void Irrep::ParseJSON(const JSONNode &n, string what) {
    JSONNode::const_iterator i = n.begin();
    while (i != n.end()){
  
      // get the node name and value as a string
      string node_name = i -> name();
      
      // find out where to store the values
      if(node_name == "Group") {
        //_Group = new SimpleGroup(i->as_string());
        _Group = i->as_string();
      } else if(node_name == "Group->rank()") {
        _Group->rank() = i->as_int();
      } else if(node_name =="HWeight") {
        //_HWeight = new Weight(*_Group, _Group->rank());
        _HWeight = new Weight(_Group, _Group->rank());
        _HWeight->ParseJSON(*i);
      } else if(node_name == "nirreps") {
        _nirreps = i->as_int();
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
        //cout << i->write_formatted() << endl;
        _Weights.ParseJSON(*i);
      }
  
      //increment the iterator
      ++i;
    }
  }
*/
  /* Overloaded << operator with irreps on the right */
  ostream &operator<<(ostream &stream, const Irrep &i)
  {
    stream << i.label();
    return stream;
  }

}
