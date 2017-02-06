/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * simplegroup.cc
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 15/02/2012
 */

#include "simplegroup.h"
#include "subgroup.h"

/*********************************/
/* Class SimpleGroup definitions */
/*********************************/

namespace Tomb
{
  // Member functions

  /* Constructor */
  SimpleGroup::SimpleGroup(int rank, char type) {

    try {
      _rank = rank;
      _type = type;
      init();
    } catch (...) {
      throw;
    }

  }

  /* Constructor 2 */
  SimpleGroup::SimpleGroup(int rank, const Matrix<double> &Cartan) {	

    try {
      
      _rank = rank;

      //cout << "Group indentification" << endl;
      if(rank == 1) {
        if(Cartan[0][0] == 1) {
          _type = 'U';
        } else {
          _type = 'A';
        }
      } else if(rank == 2 and Cartan[0][1] == -3) {
        _type = 'G';
      } else if(rank == 4 and Cartan[1][2] == -2) {
        _type = 'F';
      } else if(Cartan != Cartan.Transpose()) {
        int row, col;
        for(int i=0; i<rank; i++) {
          for(int j=0; j<rank; j++) {
            if(Cartan[i][j] == -2) {
              row = i;
              col = j;
            }
          }
        }
        if(rank == 2) {
          if(row < col) {
            _type = 'B';
          } else {
            _type = 'C';
          }
        } else {
          if(((row < col and col == rank-1) or (row > col and col == 0))) {
            _type = 'B';
          } else {
            _type = 'C';
          }
        }
      } else {
        bool typeA = true;
        for(int i=0; i<rank; i++) {
          int nlinks = 0;
          for(int j=0; j<rank; j++) {
            if(Cartan[i][j] == -1) {
              nlinks++;
            }
          }
          if(nlinks > 2 or nlinks == 0) {
            typeA = false;
          }
        }

        if(typeA) {
          //cout << "The group is of type A" << endl;
          _type = 'A';
        } else if (Cartan[rank-3][rank-1] != 0 or Cartan[1][rank-1] != 0) {
          _type = 'D';
        } else {
          _type = 'E';
        }
      
        bool typedore = false;
        int junction;
        for(int i=0; i<rank; i++) {
          int numberoflinks = 0;
          for(int j=0; j<rank; j++) {
            if(Cartan[i][j] == -1) {
              numberoflinks++;
            }
          }
          if(numberoflinks == 3) {
            typedore = true;
            junction = i;
          }
        }
        if(typedore) {
          if((rank != 6 and rank != 7 and rank != 8) or (Cartan[rank-3][rank-1] != 0 or Cartan[1][rank-1] != 0 or Cartan[1][3] != 0)) {
            _type = 'D';
          } else {
            int numberofbrancheswithonlyonedot = 3;
            for(int i=0; i<rank; i++) {
              if(Cartan[junction][i] == -1) {
                bool hasotherconnections = false;
                for(int j=0;j<rank;j++) {
                  if(Cartan[i][j] == -1 and j!=junction) {
                    hasotherconnections = true;
                  }
                }
                numberofbrancheswithonlyonedot--;
              }
            }
            if(numberofbrancheswithonlyonedot == 1) {
              _type = 'E';
            } else {
              _type = 'D';
            }
          }
        } else {
          _type = 'A';
        }

      }
      
      init();
    
    } catch (...) {
      throw;
    }
  }

  /* Constructor 3, with strings */
  SimpleGroup::SimpleGroup(const string id) {
    
    try {
      string label = id;
      label.erase(remove(label.begin(), label.end(), ' '), label.end());
      
      switch(toupper(label[0])) {
        case 'A':
          _type = toupper(label[0]);
          _rank = atoi((label.substr(1,label.length()-1)).c_str());
          break;
      
        case 'B':
          _type = toupper(label[0]);
          _rank = atoi((label.substr(1,label.length()-1)).c_str());
          break;
          
        case 'C':
          _type = toupper(label[0]);
          _rank = atoi((label.substr(1,label.length()-1)).c_str());
          break;
      
        case 'D':
          _type = toupper(label[0]);
          _rank = atoi((label.substr(1,label.length()-1)).c_str());
          break;
        
        case 'E':
          _type = toupper(label[0]);
          _rank = atoi((label.substr(1,label.length()-1)).c_str());
          if(_rank < 6 or _rank > 8) {
            throw "SimpleGroup::SimpleGroup::Group does not exist";
          }
          break;
        
        case 'F':
          _type = toupper(label[0]);
          _rank = atoi((label.substr(1,label.length()-1)).c_str());
          if(_rank != 4) {
            throw "SimpleGroup::SimpleGroup::Group does not exist";
          }
          break;

        case 'G':
          _type = toupper(label[0]);
          _rank = atoi((label.substr(1,label.length()-1)).c_str());
          if(_rank != 2) {
            throw "SimpleGroup::SimpleGroup::Group does not exist";
          }
          break;
        
        case 'S':
          switch(toupper(label[1])) {
            case 'U':
              _type = 'A';
              if(label[2] == '(' or label[2] == '[' or label[2] == '{') {
                _order = atoi((label.substr(3,label.length()-4)).c_str());
              } else {
                _order = atoi((label.substr(2,label.length()-2)).c_str());
              }
              _rank = _order - 1;
              break;
            
            case 'O':
              if(label[2] == '(' or label[2] == '[' or label[2] == '{') {
                _order = atoi((label.substr(3,label.length()-4)).c_str());
              } else {
                _order = atoi((label.substr(2,label.length()-2)).c_str());
              }
              if(_order%2) {
                _type = 'B';
                _rank = (_order-1)/2;
              } else {
                _type = 'D';
                _rank = _order/2;
              }
              break;
            
            case 'P':
              _type = 'C';
              if(label[2] == '(' or label[2] == '[' or label[2] == '{') {
                _order = atoi((label.substr(3,label.length()-4)).c_str());
              } else {
                _order = atoi((label.substr(2,label.length()-2)).c_str());
              }
              if(_order%2) {
                throw "SimpleGroup::SimpleGroup::Group does not exist";
              } else {
                _rank = _order/2;
              }
              break;
              
            default:
              throw "SimpleGroup::SimpleGroup::Group does not exist";
          }
          break;
      
        case 'U':
          _type = toupper(label[0]);
          _rank = atoi((label.substr(1,label.length()-1)).c_str());
          if(_rank != 1) {
            throw "SimpleGroup::SimpleGroup::This is not a Lie Group";
          }
          break;
        
        default:
          throw "SimpleGroup::SimpleGroup::Group does not exist";
      }
      
      init();
    
    } catch (...) {
      throw;
    }
  }

  /* Constructor 4, with json nodes */
/*  SimpleGroup::SimpleGroup(const JSONNode &n) {
    
    if(n.as_string() != "") {
      SimpleGroup(n.as_string());
    } else {
      ParseJSON(n);
    }
  }
*/
  /* Copy constructor */
  SimpleGroup::SimpleGroup(const SimpleGroup &G) 
  {
    cout << "sg::cpct" << endl;  
    try 
    {
      _rank = G.rank();
      _type = G.type();
      _dim = G.dim();
      _order = G.order();
      _label = G.label();
      _Cartan = Matrix<double>(G.Cartan());
      _G = Matrix<double>(G.G());
      _abelian = G.abelian();
      _Casimir = G.Casimir();
      _repsMaxDim = G.repsMaxDim();
      _hasReps = G.hasReps();
      _hasSubgroups = G.hasSubgroups();
      if(_hasReps) _Irreps = G.Irreps();
//      if(_hasSubgroups) _Subgroups = G.SubgroupsConst();
    }
    catch (...) 
    {
      throw;
    }
  }

  /* Move constructor */
  SimpleGroup::SimpleGroup(SimpleGroup &&G) :
    _rank(move(G._rank)),
    _type(move(G._type)),
    _dim(move(G._dim)),
    _order(move(G._order)),
    _label(move(G._label)),
    _Cartan(move(G._Cartan)),
    _G(move(G._G)),
    _abelian(move(G._abelian)),
    _Casimir(move(G._Casimir)),
    _repsMaxDim(move(G._repsMaxDim)),
    _hasReps(move(G._hasReps)),
    _hasSubgroups(move(G._hasSubgroups)),
    _Irreps(move(G._Irreps))//,
 //   _Subgroups(move(G._Subgroups))
  {
    cout << "sg::mvct" << endl;  
    try
    {
      G._rank = 0;
      G._type = '\0';
      G._dim = 0;
      G._order = 0;
      G._label = "";
      G._Cartan = Matrix<double>();
      G._G = Matrix<double>();
      G._abelian = false;
      G._Casimir = 0;
      G._repsMaxDim = 0;
      G._hasReps = false;
      G._hasSubgroups = false;
      G._Irreps.Clear();
//      G._Subgroups.Clear();
      
    } catch (...) {
      throw;
    }
  }

  /* Destructor */
  SimpleGroup::~SimpleGroup()
  {
    //cout << "deleting group " << endl;
  }

  /* Assignment operator */
  SimpleGroup &SimpleGroup::operator=(const SimpleGroup &G)
  {
    cout << "sg::asop" << endl;  
    try {
      if(this == &G) return *this;
      
      _rank = G.rank();
      _type = G.type();
      _dim = G.dim();
      _order = G.order();
      _label = G.label();
      _Cartan = Matrix<double>(G.Cartan());
      _G = Matrix<double>(G.G());
      _abelian = G.abelian();
      _Casimir = G.Casimir();
      _repsMaxDim = G.repsMaxDim();
      _hasReps = G.hasReps();
      _hasSubgroups = G.hasSubgroups();
      if(_hasReps) _Irreps = G.Irreps();
//      if(_hasSubgroups) _Subgroups = G.SubgroupsConst();
      
      return *this;
    } catch (...) {
      throw;
    }
  }

  /* Move assignment operator */
  SimpleGroup &SimpleGroup::operator=(SimpleGroup &&G)
  {
  
    cout << "sg::mvasop" << endl;  
    try {
      if(this == &G) return *this;
      
      _rank = move(G._rank);
      _type = move(G._type);
      _dim = move(G._dim);
      _order = move(G._order);
      _label = move(G._label);
      _Cartan = move(G._Cartan);
      _G = move(G._G);
      _abelian = move(G._abelian);
      _Casimir = move(G._Casimir);
      _repsMaxDim = move(G._repsMaxDim);
      _hasReps = move(G._hasReps);
      _hasSubgroups = move(G._hasSubgroups);
      _Irreps = move(G._Irreps);
//      _Subgroups = move(G._Subgroups);
      
      G._rank = 0;
      G._type = '\0';
      G._dim = 0;
      G._order = 0;
      G._label = "";
      G._Cartan = Matrix<double>();
      G._G = Matrix<double>();
      G._abelian = false;
      G._Casimir = 0;
      G._repsMaxDim = 0;
      G._hasReps = false;
      G._hasSubgroups = false;
      G._Irreps.Clear();
//      G._Subgroups.Clear();
      
      return *this;
    } catch (...) {
      throw;
    }
  }

  /* Initialise variables */
  void SimpleGroup::init() {

    try {

      if(_rank == 0 or _type == 0) {
        throw "SimpleGroup::init::Not enough variables no initialise";
      }
            
/*      if(DB<SimpleGroup>().check(id()))
      {
        *this = *DB<SimpleGroup>().at(id());
        return ;
      }
*/       
      _abelian = false;

      char label[10];
      _Cartan = Matrix<double>(rank());
        
      switch(type()) {
        case 'A':
          _Cartan[0][0] = 2;
          for(int i=1; i<rank(); i++) {
            _Cartan[i][i] = 2;
            _Cartan[i-1][i] = -1;
            _Cartan[i][i-1] = -1;
          }
          sprintf(label,"SU(%d)", rank()+1);
          _label = label;
          _dim = rank()*(rank() + 2);
          _order = rank() + 1;
          break;
        
        case 'B':
          _Cartan[0][0] = 2;
          for(int i=1; i<rank(); i++) {
            _Cartan[i][i] = 2;
            _Cartan[i-1][i] = -1;
            _Cartan[i][i-1] = -1;
          }
          if(rank()>1) {
            _Cartan[rank()-2][rank()-1] = -2;
          }
          sprintf(label,"SO(%d)", 2*rank()+1);
          _label = label;
          if(rank() == 1) {
            _type = 'A';
            _label = "SU(2)";	
          //} else if(rank() == 2) {
          //	_type = 'C';
          //	_Cartan = _Cartan.Transpose();
          }
          _dim = rank()*(2*rank() + 1);
          _order = 2*rank() + 1;
          break;
      
        case 'C':
          _Cartan[0][0] = 2;
          for(int i=1; i<rank(); i++) {
            _Cartan[i][i] = 2;
            _Cartan[i-1][i] = -1;
            _Cartan[i][i-1] = -1;
          }
          if(rank()>1) {
            _Cartan[rank()-1][rank()-2] = -2;
          }
          sprintf(label,"Sp(%d)", 2*rank());
          _label = label;
          if(rank() == 1) {
            _type = 'A';
            _label = "SU(2)";
          }
          _dim = rank()*(2*rank() + 1);
          _order = 2*rank() + 1;
          break;
      
        case 'D':
          _Cartan[0][0] = 2;
          for(int i=1; i<rank()-1; i++) {
            _Cartan[i][i] = 2;
            _Cartan[i-1][i] = -1;
            _Cartan[i][i-1] = -1;
          }
          if(rank()>2) {
            _Cartan[rank()-3][rank()-1] = -1;
            _Cartan[rank()-1][rank()-3] = -1;
          } 
          if(rank()>1) {
            _Cartan[rank()-1][rank()-1] = 2;
          }
          sprintf(label,"SO(%d)", 2*rank());
          _label = label;
          if(rank() == 1) {
            _Cartan[0][0] = 1;
            _abelian = true;
            _type = 'U';
            _label = "U(1)";
          } else if(rank() == 3) {
            _type = 'A';
            _Cartan.ChangeBasis(0,1);
            _label = "SU(4)";
          }
          _dim = rank()*(2*rank() - 1);
          _order = 2*rank();
          break;
          
        case 'E':
          if(rank()>=6 && rank()<=8) {
            for(int i=0;i<rank()-2;i++) {
              _Cartan[i][i] = 2;
              _Cartan[i][i+1] = -1;
              _Cartan[i+1][i] = -1;
            }	
            _Cartan[rank()-2][rank()-2] = 2;
            _Cartan[rank()-1][rank()-1] = 2;
            _Cartan[2][rank()-1] = -1;
            _Cartan[rank()-1][2] = -1;
            sprintf(label,"E%d", rank());
            _label = label;
          } else {
            throw "SimpleGroup::init::Group does not exist";
          }
          if(rank()==6) {
            _dim = 78;
            _order = 27;
          } else if(rank()==7) {
            _dim = 133;
            _order = 55;
          } else {	
            _order = 3875;
            _dim = 248;
          }
          break;
      
        case 'F':
          if(rank() == 4) {
            for(int i=0;i<rank()-1;i++) {
              _Cartan[i][i] = 2;
              _Cartan[i][i+1] = -1;
              _Cartan[i+1][i] = -1;
            }
            _Cartan[rank()-1][rank()-1] = 2;
            _Cartan[1][2] = -2;
            _label = "F4";
          } else {
            throw "SimpleGroup::init::Group does not exist";
          }
          _dim = 52;
          _order = 52;
          break;
           
        case 'G':
          if(rank() == 2) {
            _Cartan[0][0] = 2;
            _Cartan[0][1] = -3;
            _Cartan[1][0] = -1;
            _Cartan[1][1] = 2;
            _label = "G2";
          } else {
            throw "SimpleGroup::init::Group does not exist";
          }
          _dim = 14;
          _order = 14;
          break;
            
        case 'U':
          if(rank() == 1) {
            _abelian = true;
            _Cartan[0][0] = 1;
            _label = "U(1)";
          } else {
            throw "SimpleGroup::init::Group does not exist";
          }
          _dim = 1;
          _order = 1;
          break;
    
        default:
          throw "SimpleGroup::init::Group does not exist";
      }		
    
      _G = Matrix<double>(rank());
      _G = _Cartan.Inverse();

      if(rank() != 1 and _Cartan != _Cartan.Transpose()) 
        {
        for(int i=0; i<rank(); i++) 
        {
          for(int j=0; j<rank(); j++) 
          {
            Root alpha = this->SRoot(j);
            _G[i][j] *= 0.5*alpha.length();
          }	
        }
      }

      _PRoots = this->PRoots();
        
      if(abelian()) 
      {
        _Casimir = 0;
      }
      else 
      {
        RVector<double> w(PRoots().GetObject(0));
        _Casimir = 0;
        for(int i=0; i<_rank; i++) 
          for(int j=0; j<_rank; j++) 
            _Casimir += 0.5*w[i]*_G[i][j]*(w[j]+2);
        //Irrep Adjoint = Irrep(*this, Weight(*this, PRoots().GetObject(0)));
        //_Casimir = Adjoint.DynkinIndex();
      }

      _Irreps = this->CalculateIrreps(_repsMaxDim);      

      DB<SimpleGroup>().set(id(), this);
       
      // Output the info
      /*ostringstream OutputDirectory;
      OutputDirectory << "./out/" << id();

      mkdir(OutputDirectory.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

      ostringstream OutputFileName;
      OutputFileName << OutputDirectory.str() << "/" << id() << ".out";

      ofstream OutputFile;
      OutputFile.open(OutputFileName.str().c_str());
      OutputFile << json().write_formatted() << endl;
      OutputFile.close();*/
    
    } catch (...) {
      throw;
    }
  }

  /* Identifier of the group */
  string SimpleGroup::id() const
  {
    stringstream s;
    s << _type << _rank;
    return s.str();
  }

  /* Returns rank */
  int SimpleGroup::rank() const
  {
    return _rank;
  }

  /* Returns type */
  char SimpleGroup::type() const
  {
    return _type;
  }

  /* Returns dimension */
  int SimpleGroup::dim() const
  {
    return _dim;
  }

  /* Returns order */
  int SimpleGroup::order() const
  {
    return _order;
  }

  /* Returns the label */
  string SimpleGroup::label() const
  {
    return _label;
  }

  /* Returns whether the group is abelian */
  bool SimpleGroup::abelian() const
  {
    return _abelian;
  }

  /* Returns Cartan matrix */
  Matrix<double> SimpleGroup::Cartan() const
  {
    return _Cartan;
  }

  /* Returns the matrix G, inverse of the Cartan matrix */
  Matrix<double> SimpleGroup::G() const
  {
    return _G;
  }

  /* Returns the Casimir of the group */
  double SimpleGroup::Casimir() const
  {
    return _Casimir;
  }

  /* Returns the maximum dimension of the calculated reps */
  int SimpleGroup::repsMaxDim() const
  {
    return _repsMaxDim;
  }

  /* Returns whether the SimpleGroup has calculated its reps */
  bool SimpleGroup::hasReps() const
  {
    return _hasReps;
  }
  
  /* Returns whether the SimpleGroup has calculated its subgroups */
  bool SimpleGroup::hasSubgroups() const
  {
    return _hasSubgroups;
  }

  List<Irrep> SimpleGroup::Irreps() const
  {
    return _Irreps;
  }
  
  /* Returns Simple Root i */
  Root SimpleGroup::SRoot(int i){
    
    try {
      if(i>=_rank) {
        throw "SimpleGroup::SRoot::Element doesn\'t exist";
      }
      RVector<double> Vector(Cartan().Row(i));
      Root alpha(*this, Vector);
      return alpha;
    
    } catch (...) {
      throw;
    }
  }

  /* Calculates all positive roots */
  List<Root> SimpleGroup::PRoots(string _basis) {

    try {

      string basis = _basis;
      
      string::iterator init = basis.begin();
      string::iterator end = basis.end();

      while (init != end) {
        *init = toupper((unsigned char)*init);
        ++init;
      }
      
      if(basis.compare("DYNKIN") and basis.compare("DUAL")) {
        throw "SimpleGroup::PRoots::Basis has to be either Dynkin or Dual";
      }
  
      if(!_PRoots.Empty()) {
        if(basis=="DUAL") {
          List<Root> DualRoots;
          for(int i=0; i<_PRoots.nterms(); i++) {
            DualRoots.AddTerm(_PRoots.GetObject(i).Dual());
          }
          return DualRoots;
        } else {
          return _PRoots;
        }
      }
      int nproots = (int)(0.5*(this->dim()-this->rank()));
      List<Root> Roots;
      for(int i=0; i<this->rank(); i++)
      {
        Roots.AddTerm(this->SRoot(i));
      }

      double n = 0;

      for (int i=0; i<nproots; i++) {
        //cout << "i = " << i << endl;
        for(int j=0; j<_rank; j++) {
          //cout << "-j = "<< j << endl;
          if(i!=j) {
            n = -Roots.GetObject(i)[j];
            //cout << "n = " << n << endl;
            if(n>0){
              for(int l=1; l<=n; l++) {
                //cout << "l = " << l << endl;
                //cout << this->SRoot(j) << endl;
                Root alpha = Roots.GetObject(i) + this->SRoot(j)*(double)l;
                //cout << alpha << endl;
                if(Roots.Index(alpha) < 0) {
                  Roots.AddTerm(alpha);
            }						
            }
          }
        }
      }
      }
    
      //cout << "Roots = " << Roots << endl;
      Roots.Reverse();
      //cout << "Roots = " << Roots << endl;
      
      if(basis=="DUAL") {
        List<Root> DualRoots;
        for(int i=0; i<Roots.nterms(); i++) {
          DualRoots.AddTerm(Roots.GetObject(i).Dual());
        }
        return DualRoots;
      }

      //cout << Roots << endl;
      return Roots;
    
    } catch (...) {
      throw;
    }
  }

  /* Calculates all roots */
/*  List<Root> SimpleGroup::Roots() {
    
    try {
*/      /*if(json.find_nocase("Roots")) {
        
      }*/
      //cout << "Calculating the roots..." << endl;
    
/*      List<Root> Roots = this->PRoots();
      int nproots = Roots.nterms();
      for(int i=0; i<this->rank(); i++) {
        RVector<double> V(this->rank());
        Roots.AddTerm(Root(*this, V));
      }

      for(int i=0; i<nproots; i++) {
        Root beta = Roots.GetObject(nproots-i-1);
        beta *= -1;
        Roots.AddTerm(beta);
      }
        
      return Roots;
    
    } catch (...) {
      throw;
    }
  }
*/
  /* Returns the simple representations of a group */
/*  List<Irrep> SimpleGroup::SimpleReps() {
    
    try {
      Weight HWeight(*this, this->rank());

      List<Irrep> SimpleReps;
      string label;

      switch(_type) {
    
        case 'A':
          HWeight[0] = 1;
          SimpleReps.AddTerm(Irrep(*this,HWeight));
          HWeight[0] = 0;
          HWeight[_rank-1] = 1;
          SimpleReps.AddTerm(Irrep(*this,HWeight));
          break;

        case 'B':
          HWeight[0] = 1;
          SimpleReps.AddTerm(Irrep(*this,HWeight));
          HWeight[0] = 0;
          HWeight[_rank-1] = 1;
          SimpleReps.AddTerm(Irrep(*this,HWeight));
          break;

        case 'C':
          HWeight[0] = 1;
          SimpleReps.AddTerm(Irrep(*this,HWeight));
          break;

        case 'D':
          HWeight[0] = 1;
          SimpleReps.AddTerm(Irrep(*this,HWeight));
          HWeight[0] = 0;
          HWeight[_rank-1] = 1;
          SimpleReps.AddTerm(Irrep(*this,HWeight));
          HWeight[_rank-1] = 0;
          HWeight[_rank-2] = 1;
          SimpleReps.AddTerm(Irrep(*this,HWeight));
          break;

        case 'E':
          if(_rank==6) {
            HWeight[0] = 1;
            SimpleReps.AddTerm(Irrep(*this,HWeight));
            HWeight[0] = 0;
            HWeight[_rank-2] = 1;
            SimpleReps.AddTerm(Irrep(*this,HWeight));
          } else if(_rank==7) {
            HWeight[_rank-2] = 1;
            SimpleReps.AddTerm(Irrep(*this,HWeight));
          } else if(_rank==8) {
            HWeight[_rank-2] = 1;
            SimpleReps.AddTerm(Irrep(*this,HWeight));
          }
          break;

        case 'F':
          HWeight[_rank-1] = 1;
          SimpleReps.AddTerm(Irrep(*this,HWeight));
          break;

        case 'G':
          HWeight[1] = 1;
          SimpleReps.AddTerm(Irrep(*this,HWeight));
          break;

        case 'U':
          HWeight[0] = 1;
          SimpleReps.AddTerm(Irrep(*this,HWeight));
      }
      
      return SimpleReps;
      
    } catch (...) {
      throw;
    }

  }
*/
  /* Returns the generating rep of the group */
/*  Irrep SimpleGroup::GeneratingRep() {

    try {
      //clock_t  time1 = clock();
      Weight HWeight(*this, this->rank());
        
      switch(_type) {

        case 'A':
          HWeight[0] = 1;
          break;
        case 'B':
          HWeight[0] = 1;
          break;
        case 'C':
          HWeight[0] = 1;
          break;
        case 'D':
          HWeight[_rank-2] = 1;
          break;
        case 'E':
          if(_rank==6) {
            HWeight[0] = 1;
          } else if(_rank==7) {
            HWeight[_rank-2] = 1;
          } else if(_rank==8) {
            HWeight[_rank-2] = 1;
          }
          break;
        case 'F':
          HWeight[_rank-1] = 1;
          break;
        case 'G':
          HWeight[1] = 1;
          break;
        case 'U':
          HWeight[0] = 1;
          break;
        default:
          throw "SimpleGroup::GeneratingRep::Group does not exist";
      }
      //cout << "Time in generating rep = " << double(clock()-time1)/CLOCKS_PER_SEC << endl; 

      return Irrep(*this,HWeight);
    
    } catch (...) {
      throw;
    }
  }
*/
  /* Returns the adjoint rep of te group */
/*  Irrep SimpleGroup::AdjointRep()
  {
    try
    {
      List<Irrep> irreps = Irreps(dim());
      if(irreps[-1].dim() == dim())
        return irreps[-1];
      else
        throw "SimpleGroup::Irrep::Could not get the adjoint irrep";
      
    } catch (...)
    {
      throw;
    }	
  }
*/  
  /* Returns a singlet rep */
/*  Irrep SimpleGroup::SingletRep()
  {
    try
    {
      return Irrep(*this, Weight(*this,RVector<double>(_rank)));
      
    } catch (...)
    {
      throw;
    }	
  }
*/  
  /* Calculates all the representations of the group up to dimension maxdim */
  List<Irrep> &SimpleGroup::CalculateIrreps(int maxdim) {

    try 
    {
      // If the reps are calculated already
      if(_Irreps.nterms() and _repsMaxDim >= maxdim) {
        int i = _Irreps.nterms()-1;
        while(_Irreps.GetObject(i).dim() > maxdim) {
          _Irreps.DeleteTerm(i);
          i--;
        }
        _repsMaxDim = maxdim;
        return _Irreps;
      }
      
      /*if(database_check(id(), "Reps") and DataBase.at(id()).hasReps() and DataBase.at(id()).repsMaxDim() >= maxdim) {
        _Irreps = DataBase.at(id()).Irreps(maxdim);
        if(_Irreps.nterms()) _hasReps = true;
        _repsMaxDim = maxdim;
        return _Irreps;
      }*/
      
      // If the group is abelian just return {0.0}
      if(abelian()) {
        _Irreps =  List<Irrep>(Irrep(*this,Weight(*this,1)));
        if(_Irreps.nterms()) _hasReps = true;
        _repsMaxDim = maxdim;
        return _Irreps;
      }
      
      
      // If not, calculate it
      Weight HW(*this, this->rank());
      List<Weight> HWs(HW);
      Irrep Rep(*this, HW);
      //cout << Rep << endl;
      _Irreps.AddTerm(Rep);
      int dim = 1;

      // List of labels so avoid repetition
      List<string> labels("1");
      for(int i = 0; i<HWs.nterms(); i++) {
        HW = HWs.GetObject(i);
        
        for(int k=0; k<_rank; k++) {
          HW[k] ++;
          //cout << HW << endl;
          if(HWs.Index(HW) < 0) {
            Irrep Rep(*this, HW);
            //cout << Rep << endl;
            dim = Rep.dim();
            if(dim<=maxdim && dim>0) {
              string label = to_string(Rep.dim());
              do {
                int index = labels.Index(label);
                if(index >= 0) {
                  if(Rep.isConjugateOf(_Irreps.GetObject(index))) {
                    label.append("*");
                  } else {
                    label.append("'");
                  }
                }
              } while(labels.Index(label) >= 0);
              Rep.setLabel(label);
              _Irreps.AddTerm(Rep);
              HWs.AddTerm(HW);
              labels.AddTerm(label);
            }	
            //cout << Reps << endl;
            //cout << HWs << endl;
          }
          HW[k] --;
        }
      }
      //cout << Reps << endl;

      _Irreps.Order("ASC");
      
      //cout << _Irreps << endl;
      
      // Set th hasReps flag to true and repsMaxDim to the current maxdim
      if(_Irreps.nterms()) _hasReps = true;
      _repsMaxDim = maxdim;
      
      // If there is an entry in the database delete it and dump this
//      database_emplace(id(), *this);

      //for(List<Irrep>::iterator it_Irreps = _Irreps.begin(); it_Irreps != _Irreps.end(); it_Irreps++)
      //	cout << it_Irreps->json().write_formatted() << endl;
      
      //cout << "Irreps = " << _Irreps << endl;
      
      return _Irreps;
      
    } 
    catch (...) { throw; }
  }

  /* Returns the variable Irreps */
/*  List<Irrep> SimpleGroup::IrrepsConst() const {
    return _Irreps;
  }
*/  
  /* Returns the extended Cartan matrix */
/*  Matrix<double> SimpleGroup::ExtendedCartan(){

    try {
      Matrix<double> ExtCartan(this->rank()+1);
      for(int i=0; i<this->rank(); i++) {
        for(int j=0; j<this->rank(); j++) {
          ExtCartan[i][j] = this->Cartan()[i][j];
        }
      }
      ExtCartan[this->rank()][this->rank()] = 2;
      switch(this->type()) {
        case 'A':
          ExtCartan[0][this->rank()] = -1;
          ExtCartan[this->rank()][0] = -1;
          ExtCartan[this->rank()][this->rank()-1] = -1;
          ExtCartan[this->rank()-1][this->rank()] = -1;
          break;
        case 'B':
          if(this->rank()>2) {
            ExtCartan[this->rank()][1] = -1;
            ExtCartan[1][this->rank()] = -1;
          } else if(this->rank() == 2) {
            ExtCartan[this->rank()][1] = -2;
            ExtCartan[1][this->rank()] = -1;
          } else {
            ExtCartan[1][0] = -1;
            ExtCartan[0][1] = -1;
          }
          break;
        case 'C':
          if(this->rank()>1) {
            ExtCartan[this->rank()][0] = -2;
            ExtCartan[0][this->rank()] = -1;
          } else {
            ExtCartan[1][0] = -1;
            ExtCartan[0][1] = -1;
          }
          break;
        case 'D':
          if(this->rank()>3) {
            ExtCartan[this->rank()][1] = -1;
            ExtCartan[1][this->rank()] = -1;
            //ExtCartan.MoveRow(this->rank(),0);
            //ExtCartan.MoveColumn(this->rank(),0);
          } else if(this->rank() == 3 or this->rank() == 2) {
            ExtCartan[this->rank()-2][this->rank()] = -1;
            ExtCartan[this->rank()][this->rank()-2] = -1;
            ExtCartan[this->rank()][this->rank()-1] = -1;
            ExtCartan[this->rank()-1][this->rank()] = -1;			
          } else {
            ExtCartan[1][0] = -1;
            ExtCartan[0][1] = -1;
          }
          break;
        case 'G':
          ExtCartan[0][2] = -1;
          ExtCartan[2][0] = -1;
          break;
        case 'F':
          ExtCartan[0][4] = -1;
          ExtCartan[4][0] = -1;
          break;
        case 'E':
          if(this->rank() == 6) {
            ExtCartan[6][5] = -1;
            ExtCartan[5][6] = -1;
          } else if(this->rank() == 7) {
            ExtCartan[7][0] = -1;
            ExtCartan[0][7] = -1;
          } else if(this->rank() == 8) {
            ExtCartan[8][6] = -1;
            ExtCartan[6][8] = -1;
          }
          break;
        case 'U':
          ExtCartan[0][1] = -1;
          ExtCartan[1][0] = -1;
          break;
      }
      ExtCartan.MoveRow(this->rank(),0);
      ExtCartan.MoveColumn(this->rank(),0);
      return ExtCartan;
        
    } catch (...) {
      throw;
    }
  }
*/
  /* Calculates the maximal subgroups of simple group */
/*  List<SubGroup> &SimpleGroup::MaximalSubgroups() {

    try {
      // If the maximal subgroups have been calculated already return them
      if(_MaxSubgroups.nterms()) return _MaxSubgroups;
      
      // If the group is abelian return the empty list
      if(abelian()) return _MaxSubgroups;
      
      // First, the semisimple groups obtained by removing a dot from the extended Dynkin diagram
      //clock_t time1 = clock();
      if(this->type() != 'A' and this->type() != 'U' and this->rank() >= 1) {
        
        // Create the extended root
        List<Root> PRoots = this->PRoots();
        Root ExtendedRoot(PRoots.GetObject(0)*-1);
        //cout << "ExtendedRoot = " << ExtendedRoot << endl;
        
        // Calculate the projection matrix for this subgroup, first get the weights of the generating representation of the group
        List<Weight> Weights = this->GeneratingRep().Weights();
        Matrix<double> Superweights(this->rank(), Weights.nterms());
        Matrix<double> Subweights(this->rank(), Weights.nterms());
        int col = 0;
        //for(int col=0; col<Superweights.cols(); col++) {
        for(List<Weight>::iterator it_Weights = Weights.begin(); it_Weights != Weights.end(); it_Weights++) {
          Superweights.SetColumn(col,it_Weights->Transpose());
          col++;
        }
        //cout << Superweights << endl;

        // Calculate the weight coefficient for the new root
        RVector<double> ExtendedWeight(Subweights.cols());
        for(int col=0; col<Subweights.cols(); col++) {
          ExtendedWeight[col] = Weights.GetObject(col)*ExtendedRoot; 
        }
        //cout << "ExtendedWeight = " << ExtendedWeight << endl;

        //clock_t time1 = clock();
        for(int i=0; i<this->rank()+1; i++) {
          //clock_t time2 = clock();
          // Create the SubCartan matrix by eliminating the row and column of the eliminated root from the extended Cartan matrix
          Matrix<double> SubCartan(this->ExtendedCartan().Adjoint(i,i));
          //cout << ExtendedCartan() << endl;
          SubGroup Subgroup(*this);
          //cout << SubCartan << endl;
          //cout << "in 1 = " << double(clock()-time2)/CLOCKS_PER_SEC << endl;
          //time2 = clock();
          Subweights = Superweights;
          // Insert the extended weight and delete the row of the subweights corresponding with the deleted root
          //cout << Subweights << endl;
          //cout << " adsf" << endl;
          Subweights.InsertRow(0, ExtendedWeight);
          //cout << Subweights << endl;
          Subweights.DeleteRow(i);
          //cout << Subweights << endl;
          //if(i) {
          //	Subweights.MoveRow(0,i-1);
          //}
          //cout << Subweights << endl;
          
          // Identify within the SubCartan matrix the Cartan matrices of Simple Groups
          int from = 0;
          for(int j=0; j<SubCartan.rows(); j++) {
            //cout << "j = " << j << endl;
            Matrix<double> Piece1(SubCartan.ExtractMatrix(from,j,0,from-1));
            Matrix<double> Piece2(SubCartan.ExtractMatrix(from,j,j+1,SubCartan.rows()-1));
            if(Piece1 == 0 && Piece2 == 0) {
              Matrix<double> BlockMatrix(SubCartan.ExtractMatrix(from,j));
              //cout << BlockMatrix <<endl;
              SimpleGroup AuxGroup(j-from+1, BlockMatrix);
              Matrix<double> Auxweights = Subweights;
              //cout << "======" << endl;
              //cout << BlockMatrix << endl;
              //cout << "------" << endl;
              //cout << AuxGroup.Cartan() << endl;
              //cout << "======" << endl;
              int k=0;
              while(AuxGroup.Cartan() != BlockMatrix and k<BlockMatrix.rows()) {
                BlockMatrix.MoveRow(0,BlockMatrix.rows()-1);
                BlockMatrix.MoveColumn(0,BlockMatrix.rows()-1);
                Subweights.MoveRow(from,j);
                //cout << BlockMatrix << endl;
                //cout << "------" << endl;
                //cout << Subweights << endl;
                //cout << "------" << endl;
                k++;
              }
              if(AuxGroup.Cartan() != BlockMatrix) {
                BlockMatrix = SubCartan.ExtractMatrix(from,j);
                Subweights = Auxweights;
                for(int k=0; k<BlockMatrix.rows()/2; k++) {
                  BlockMatrix.ChangeBasis(k,BlockMatrix.rows()-k-1);
                  Subweights.SwapRows(from+k,from+BlockMatrix.rows()-1-k);
                }
              }
              //cout << BlockMatrix << endl;
              //cout << "======" << endl;
              //cout << AuxGroup << endl;
              Subgroup.AddTerm(AuxGroup);
              from = j+1;
            }
          }
          
          //cout << Subweights << endl;
          //cout << Subgroup << endl;

          
          // And the projection matrix
          Subgroup.SetProjection(Subweights*Superweights.PseudoInverse());
          //cout << Subgroup.Projection() << endl;
          
          // Final changes
          Subgroup.setMaximal(true);
          Subgroup.setRegular(true);
          
          // Order the projection matrix columns with respect to the rank of the groups
          Subgroup.Order();
          //cout << Subgroup << endl;
          //cout << Subgroup.Projection() << endl;
          
          
          // Add to the list
          if(_MaxSubgroups.Index(Subgroup) == -1) {
            _MaxSubgroups.AddTerm(Subgroup);
          }
          
          //cout << Subgroup << endl;
        
        }
        //cout << "time in loop = " << double(clock()-time1)/CLOCKS_PER_SEC << endl;
        

      }
      
      //cout << _MaxSubgroups << endl;
    
      // Second, the non-semisimple groups obtained by removing a dot from the Dynkin diagram
      //clock_t time2 = clock();
      if(this->type() != 'U' and this->rank() >= 1) {
        
        // To calculate the projection matrix for a subgroup, first get the weights of the generating representation of the group
        List<Weight> Weights = this->GeneratingRep().Weights();
        //cout << Weights << endl;
        List<Weight> WeightsDual = this->GeneratingRep().Weights("Dual");
        //cout << WeightsDual << endl;

        Matrix<double> Superweights(this->rank(), Weights.nterms());
        Matrix<double> Subweights(this->rank(), Weights.nterms());
        for(int col=0; col<Superweights.cols(); col++) {
          Superweights.SetColumn(col, Weights.GetObject(col).Transpose());
        }	
        //cout << Superweights << endl;
        
        for(int i=0; i<this->rank(); i++) {
          //cout << "i = " << i << endl;
          // Create the SubCartan matrix by eliminating the row and column of the eliminated root from the Cartan matrix
          Matrix<double> SubCartan = Cartan().Adjoint(i,i);
          SubGroup Subgroup(*this);
          //cout << "SubCartan" << endl << SubCartan << endl;

          // Construct the subweights by moving the deleted node row to the end and substituting it with its dual values
          Subweights = Superweights;
          for(int col=0; col<Superweights.cols(); col++) {
            Subweights[i][col] = WeightsDual.GetObject(col)[i];
          }
          //cout << Subweights << endl;
          //for(int j=i+1; j<Subweights.rows(); j++) {
          //	Subweights.MoveRow(Subweights.rows()-1,0);
          //}
          Subweights.MoveRow(i, Subweights.rows()-1);
          if(!((Subweights.rows()-i)%2)) {
            Subweights *= -1;
          }
          //for(int j=i; j<Subweights.rows(); j++) {
          //	Subweights.SetRow(j,Subweights.Row(j)*(-1));
          //}
          
          //cout << Subweights << endl;
        
          // Identify within the SubCartan matrix the Cartan matrices of Simple Groups
          int from = 0;
          for(int j=0; j<SubCartan.rows(); j++) {
            //cout << "j = " << j << endl;
            Matrix<double> Piece1 = SubCartan.ExtractMatrix(from,j,0,from-1);
            Matrix<double> Piece2 = SubCartan.ExtractMatrix(from,j,j+1,SubCartan.rows()-1);
            if(Piece1 == 0 && Piece2 == 0) {
              Matrix<double> BlockMatrix = SubCartan.ExtractMatrix(from,j);
              SimpleGroup AuxGroup(j-from+1, BlockMatrix);
              Matrix<double> Auxweights = Subweights;
              int k=0;
              while(AuxGroup.Cartan() != BlockMatrix and k<BlockMatrix.rows()) {
                BlockMatrix.MoveRow(0,BlockMatrix.rows()-1);
                BlockMatrix.MoveColumn(0,BlockMatrix.rows()-1);
                Subweights.MoveRow(from,j);
                //cout << BlockMatrix << endl;
                //cout << "------" << endl;
                //cout << Subweights << endl;
                //cout << "------" << endl;
                k++;
              }
              if(AuxGroup.Cartan() != BlockMatrix) {
                BlockMatrix = SubCartan.ExtractMatrix(from,j);
                Subweights = Auxweights;
                for(int k=0; k<BlockMatrix.rows()/2; k++) {
                  BlockMatrix.ChangeBasis(k,BlockMatrix.rows()-k-1);
                  Subweights.SwapRows(k,BlockMatrix.rows()-1-k);
                }
              }
              
              Subgroup.AddTerm(AuxGroup);
              from = j+1;
            }
          }	
          
          SimpleGroup AuxGroup(1,'U');
          Subgroup.AddTerm(AuxGroup);

          //cout << Subgroup << endl;
          //cout << Subweights << endl;
          
          // And the projection matrix
          //Subgroup.SetProjection(Subweights*Superweights.PseudoInverse());
          Matrix<double> Projection = Subweights*Superweights.PseudoInverse();
          //Projection.SwapRows(i, Projection.rows()-1);
          //Projection.SwapColumns(i, Projection.cols()-1);
          Subgroup.SetProjection(Projection);
          //cout << Subgroup.Projection() << endl;
          
          // Final changes
          Subgroup.setMaximal(true);
          Subgroup.setRegular(true);
          Subgroup.setSpecial(false);
          //cout << Subgroup << endl;
          
          //cout << Subgroup << endl;
          //cout << Subgroup.Projection() << endl;
          // Order the projection matrix columns with respect to the rank of the groups
          Subgroup.Order();
          //cout << Subgroup << endl;
          //cout << Subgroup.Projection() << endl;
          
          //cout << Projection*Superweights << endl;
          
          //cout << Subgroup.json().write_formatted() << endl;
          
          // Add to the list
          if(_MaxSubgroups.Index(Subgroup) == -1) {
            _MaxSubgroups.AddTerm(Subgroup);
          }
          
          //time3 = clock();
        
        }
      }

      //cout << "---> " << double(clock()-time2)/CLOCKS_PER_SEC << endl;

      // Eliminate the original group as a subgroup, repetitions and subgroups that are subgroups of others
      /*_MaxSubgroups.DeleteObject(SubGroup(*this,*this));
      if(_MaxSubgroups.GetObject(0).rank() == rank() && _MaxSubgroups.GetObject(0).dim() == dim()) {
        _MaxSubgroups.DeleteObject(_MaxSubgroups.GetObject(0));
      }
      for(int i=0; i<_MaxSubgroups.nterms(); i++) {
        List<SubGroup> AuxMax = _MaxSubgroups.GetObject(i).MaximalSubgroups();
        for(int j=i; j<_MaxSubgroups.nterms(); j++) {
          if(AuxMax.Index(_MaxSubgroups.GetObject(j)) != -1) {
            _MaxSubgroups.DeleteTerm(j);
            j--;
          }
        }
      }*/
    
      //cout << "Regular subgroups " << *this << "->" << _MaxSubgroups << endl;

      //cout << "First step = " << double(time2 - time1)/CLOCKS_PER_SEC << endl;
      //cout << "Second step = " << double(clock() - time2)/CLOCKS_PER_SEC << endl;

      // Finally, calculate singular subgroups
      //List<SubGroup> SpecialSubgroups = this->SpecialSubgroups();
      //_MaxSubgroups.AppendList(SpecialSubgroups);
      
      // Eliminate the original group as a subgroup, repetitions and subgroups that are subgroups of others
/*      if(_MaxSubgroups.nterms() and LieGroup(_MaxSubgroups.GetObject(0)) == LieGroup(*this)) _MaxSubgroups.DeleteTerm(0);
      if(_MaxSubgroups.GetObject(0).rank() == rank() && _MaxSubgroups.GetObject(0).dim() == dim()) {
        _MaxSubgroups.DeleteObject(_MaxSubgroups.GetObject(0));
      }
      //for(int i=0; i<_MaxSubgroups.nterms(); i++) {
        //List<SubGroup> AuxMax = _MaxSubgroups.GetObject(i).MaximalSubgroups();
        /*for(int j=i; j<_MaxSubgroups.nterms(); j++) {
          if(AuxMax.Index(_MaxSubgroups.GetObject(j)) != -1) {
            _MaxSubgroups.DeleteTerm(j);
            j--;
          }
        }*/
      //}
      
 /*     _MaxSubgroups.EliminateRepeated();	
      //cout << "total time = " << double(clock()-time1)/CLOCKS_PER_SEC << endl;
      
      //for(int i=0; i<_MaxSubgroups.nterms(); i++) {
      //	cout << _MaxSubgroups.GetObject(i).json().write_formatted() << endl;
      //}
      
      return _MaxSubgroups;
    } catch (...) {
      throw;
    }
  }
*/
  /* Calculates the special subroups of a simple group */
/*  List<SubGroup> SimpleGroup::SpecialSubgroups() {

    try {
      
      List<SubGroup> SpecialSubgroups;
      
      // Exit if rank = 1
      if(rank() == 1) return SpecialSubgroups;
      
      // Get the generating rep for this grou
      Irrep GenRep = GeneratingRep();
      cout << "Generating Rep " << GenRep << endl;
      Matrix<double> Superweights(rank(), GenRep.dim());
      for(int col=0; col<Superweights.cols(); col++) {
        Superweights.SetColumn(col, GenRep.Weights().GetObject(col).Transpose());
      }
      
      // Create candidate special subgroups
      List<LieGroup> CandidateSpecialGroups;
      List<SimpleGroup> SimpleGroups;
      for(int j=rank()-1; j>0; j--) {
        for(int k='A'; k<='G'; k++) {
          if(GroupExists(j,k)) {
            SimpleGroup Simplegroup(j,k);
            if(!Simplegroup.abelian() and Simplegroup.dim() < dim() and SimpleGroups.Index(Simplegroup) == -1) {
              SimpleGroups.AddTerm(Simplegroup);
              CandidateSpecialGroups.AddTerm(LieGroup(Simplegroup));
            }
          }
        }
      }
      SimpleGroups.EliminateRepeated();
      CandidateSpecialGroups.EliminateRepeated();
      //cout << "rank = " << rank() << ", dim = " << dim() << endl;
      
      for(List<LieGroup>::iterator it_CSG = CandidateSpecialGroups.begin(); it_CSG != CandidateSpecialGroups.end(); it_CSG++) {
        if(it_CSG->rank() < rank()) {
          LieGroup SpecialSubgroup(*it_CSG);
          for(List<SimpleGroup>::iterator it_SimpleGroups = SimpleGroups.begin(); it_SimpleGroups != SimpleGroups.end(); it_SimpleGroups++) {
            if(SpecialSubgroup.rank() + it_SimpleGroups->rank() < rank() and SpecialSubgroup.dim() + it_SimpleGroups->dim() < dim()) {
              LieGroup AuxSubgroup(SpecialSubgroup);
              AuxSubgroup.AddTerm(*it_SimpleGroups);
              AuxSubgroup.Order();
              CandidateSpecialGroups.AddTerm(AuxSubgroup);
            }
          }
        }
      }
      
      cout << CandidateSpecialGroups << endl;

      // Check the representation dimensions and calculate the subgroup properties
      //cout << Superweights << endl;
      for(List<LieGroup>::iterator it_CSG = CandidateSpecialGroups.begin(); it_CSG != CandidateSpecialGroups.end(); it_CSG++) {
        List<Rrep> ListofReps = it_CSG->Reps(GenRep.dim());
        //cout << ListofReps << endl;
        for(List<Rrep>::iterator it_ListofReps = ListofReps.begin(); it_ListofReps != ListofReps.end(); it_ListofReps++) {
          if(it_ListofReps->dim() == GenRep.dim() and !it_ListofReps->hasSinglet() and (!GenRep.real() or (GenRep.real() and it_ListofReps->real()))) {
            SubGroup Subgroup(*it_CSG,*this);
            //cout << Subgroup << endl;
            // Add projection matrix and other properties
            Matrix<double> Subweights(Subgroup.rank(), it_ListofReps->dim());
            for(int col=0; col<Subweights.cols(); col++) {
              Subweights.SetColumn(col, it_ListofReps->Weights().GetObject(col).Transpose());
            }
            //cout << Subweights << endl;
            Matrix<double> Projection = Subweights*Superweights.PseudoInverse();
            //cout << Projection << endl;
            Subgroup.SetProjection(Projection);
            Subgroup.setMaximal(true);
            Subgroup.setRegular(false);
            Subgroup.setSpecial(true);
            
            // Check if it's already a special subgroup and if so don't add it
            
            bool flag = false;
            for(List<SubGroup>::iterator it_SS = SpecialSubgroups.begin(); it_SS != SpecialSubgroups.end(); it_SS++) {
              //cout << "---- " << *it_SS << endl;
              //cout << "---- " << it_SS->SpecialSubgroups() << endl;
              if(it_SS->SpecialSubgroups().Index(Subgroup) != -1) {
                flag = true;
              }
            }
            if(SpecialSubgroups.Index(Subgroup) == -1 and !flag) {
              SpecialSubgroups.AddTerm(Subgroup);
            }
          }
        }
      }
      
      cout << "Special Subgroups = " << *this << "->" << SpecialSubgroups << endl;
      
      return SpecialSubgroups;
      
    } catch (...) {
      throw;
    }
  }
*/
  /* Calculates all the subgroups of a simple group */
/*  List<SubGroup> &SimpleGroup::Subgroups() {
    
    try {

      // If the group is abelian, return empty
      if(abelian()) return _Subgroups;
      
      if(_Subgroups.nterms()) return _Subgroups;
*/      
      /*if(database_check(id(), "Subgroups") and DataBase.at(id()).hasSubgroups()) {
        _Subgroups = DataBase.at(id()).Subgroups();
        if(_Subgroups.nterms()) _hasSubgroups = true;
        return _Subgroups;
      }*/
      
      // Use LieGroup function
/*      LieGroup G(*this);
      _Subgroups = G.Subgroups();
      
      // Set the hasSubgroups flag to true
      if(_Subgroups.nterms()) _hasSubgroups = true;
      
      // If there is an entry in the database delete it and dump this
      //database_emplace(id(), *this);
      
      return _Subgroups;
      
    } catch (...) {
      throw;
    }
  }
*/
  /* Returns the variable Subgroups */
/*  List<SubGroup> SimpleGroup::SubgroupsConst() const {
    return _Subgroups;
  }
*/  
  /* Calculates the subgroups with rank greater that specified */
/*  List<SubGroup> SimpleGroup::Subgroups(int rank) {
    
    try {

      // If the group is abelian, return empty
      if(abelian()) return List<SubGroup>();
      
      // Use LieGroup function
      LieGroup G(*this);
      return G.Subgroups(rank);
      
    } catch (...) {
      throw;
    }
  }
*/
  /* Calculates the subgroups with ranks between rank1 and rank2 */
/*  List<SubGroup> SimpleGroup::Subgroups(int rank1, int rank2) {
    try {
      
      // If the group is abelian, return empty
      if(abelian()) return List<SubGroup>();
      
      // Use LieGroup function
      LieGroup G(*this);
      return G.Subgroups(rank1, rank2);
      
    } catch (...) {
      throw;
    }
  }
*/
  /* Returns the breaking chain from a simple group to a simple group */
/*  List<List<Tree<SimpleGroup> > > SimpleGroup::BreakingChains(const SimpleGroup &Subgroup) {
    try {
      return BreakingChains(LieGroup(Subgroup));	
    } catch (...) {
      throw;
    }
  }
*/
  /* Returns the breaking chain from a simple group to a lie group */
/*  List<List<Tree<SimpleGroup> > > SimpleGroup::BreakingChains(const LieGroup &Subgroup) {
    try {
      LieGroup Group(*this);
      return Group.BreakingChains(Subgroup);
    } catch (...) {
      throw;
    }
  }
*/
  /* Returns the list of possible invariant products of the representations of the group at up dimension operator dim */
/*  List<Product<Irrep> > SimpleGroup::Invariants(const List<Irrep> &irreps, int dim) {
    
    try {
      LieGroup Group(*this);
      
      List<Irrep> Irreps(irreps);
      List<Rrep> Reps;
      
      for(List<Irrep>::iterator it_Irreps = Irreps.begin(); it_Irreps != Irreps.end(); it_Irreps++)
        Reps.AddTerm(Rrep(*it_Irreps));
      
      List<Product<Rrep> > Invariants = Group.Invariants(Reps, dim);
      List<Product<Irrep> > Invariants2;
      
      for(List<Product<Rrep> >::iterator it_Invariants = Invariants.begin(); it_Invariants != Invariants.end(); it_Invariants++) {
        Product<Rrep> Invariant = *it_Invariants;
        Product<Irrep> Invariant2;
        for(Product<Rrep>::iterator it_Invariant = Invariant.begin(); it_Invariant != Invariant.end(); it_Invariant++)
          Invariant2.AddTerm(Irrep(it_Invariant->GetObject(0)));
        Invariants2.AddTerm(Invariant2);
      }
      
      return Invariants2;
      
    } catch (...) {
      throw;
    }
  }
*/
  /* Returns whether the simple group is a subgroup of a Lie Group */
/*  bool SimpleGroup::isSubgroupOf(LieGroup Group) const {

    try {
      LieGroup G(*this);	
      return G.isSubgroupOf(Group);
      
    } catch (...) {
      throw;
    }
  }
*/
  /* Returns whether the Lie group is a subgroup of another simple Group */
/*  bool SimpleGroup::isSubgroupOf(SimpleGroup Group) const {

    try {
      LieGroup G(*this);
      return G.isSubgroupOf(Group);
    } catch (...) {
      throw;
    }
  }
*/
  /* Overloaded == operator */
  bool SimpleGroup::operator==(const SimpleGroup &OtherSimpleGroup) const {

    if(this->rank() == OtherSimpleGroup.rank() and this->type() == OtherSimpleGroup.type()) return true;

    if(rank() == 2 and OtherSimpleGroup.rank() == 2 and (type() == 'B' or type() == 'C') and (OtherSimpleGroup.type() == 'B' or OtherSimpleGroup.type() == 'C')) {
      return true;
    }
    
    return false;
  }

  /* Overloaded != operator */
  bool SimpleGroup::operator!=(const SimpleGroup &OtherSimpleGroup) const {
    return !((*this) == OtherSimpleGroup);

  }

  /* Overloaded > operator */
  bool SimpleGroup::operator>(const SimpleGroup &OtherSimpleGroup) const {
    if(this->rank() > OtherSimpleGroup.rank()) {
      return true;
    } else if(this->rank() == OtherSimpleGroup.rank()) {
      if(this->dim() > OtherSimpleGroup.dim()) {
        return true;
      }
    }
    return false;
  }

  /* Overloaded < operator */
  bool SimpleGroup::operator<(const SimpleGroup &OtherSimpleGroup) const {
    if(this->rank() < OtherSimpleGroup.rank()) {
      return true;
    } else if(this->rank() == OtherSimpleGroup.rank()) {
      if(this->dim() < OtherSimpleGroup.dim()) {
        return true;
      }
    }
    return false;
  }

  /* Returns the json string */
/*  JSONNode SimpleGroup::json(string name) const {
    
    if(name == "id") {
      return JSONNode("", id());
    }
    
    JSONNode json;
    
    json.push_back(JSONNode("id", id()));
    json.push_back(JSONNode("rank", _rank));
    json.push_back(JSONNode("type", string(1,_type)));
    json.push_back(JSONNode("dim", _dim));
    json.push_back(JSONNode("order", _order));
    json.push_back(JSONNode("label", _label));
    json.push_back(JSONNode("abelian", _abelian));
    json.push_back(_Cartan.json("Cartan"));
    json.push_back(_G.json("G"));
    json.push_back(JSONNode("Casimir", _Casimir));
    json.push_back(JSONNode("repsMaxDim", _repsMaxDim));
    json.push_back(JSONNode("hasReps", _hasReps));
    json.push_back(JSONNode("hasSubgroups", _hasSubgroups));
    json.push_back(_Irreps.json("Irreps"));
    json.push_back(_Subgroups.json("Subgroups"));
    
    return json;
  }
*/
  /* Parses a json object into the attributes of the class */
/*  void SimpleGroup::ParseJSON(const JSONNode & n, string what) {
    
    JSONNode::const_iterator i = n.begin();
      
    while (i != n.end()){
    
      // get the node name and value as a string
      string node_name = i -> name();

      // find out where to store the values
      if(node_name == "rank") {
        _rank = i->as_int();
      } else if(node_name =="type") {
        _type = (i->as_string())[0];
      } else if(node_name == "dim") {
        _dim = i->as_int();
      } else if(node_name == "order") {
        _order = i->as_int();
      } else if(node_name == "label") {
        _label = i->as_string();
      } else if(node_name == "abelian") {
        _abelian = i->as_bool();
      } else if(node_name == "Cartan") {
        _Cartan.ParseJSON(*i);
      } else if(node_name == "G") {
        _G.ParseJSON(*i);
      } else if(node_name == "Casimir") {
        _Casimir = i->as_float();
      } else if(node_name == "repsMaxDim") {
        _repsMaxDim = i->as_int();
      } else if(node_name == "hasReps" and (what == "Reps")) {
        _hasReps = i->as_bool();
      } else if(node_name == "hasSubgroups" and what == "Subgroups") {
        _hasSubgroups = i->as_bool();
      } else if(node_name == "Irreps" and (what == "Reps")) {
        _Irreps.ParseJSON(*i);
        if(_Irreps.nterms())
          _hasReps = true;
        else
          _hasReps = false;
      } else if(node_name == "Subgroups" and (what == "Subgroups")) {
        _Subgroups.ParseJSON(*i);
        if(_Subgroups.nterms())
          _hasSubgroups = true;
        else
          _hasSubgroups = false;
      }
        
      //increment the iterator
      ++i;
    }
  }
*/
  /* Checks whether the group exists or not */
  bool SimpleGroup::GroupExists(int rank, char type) {
    try {
    
      if(type < 'A' or type > 'G') return false;
      
      if(rank <= 0) return false;
      
      if(type == 'D' and rank == 2) return false;
      
      if(type == 'E' and (rank < 6 or rank > 8)) return false;
      
      if(type == 'F' and rank != 4) return false;
      
      if(type == 'G' and rank != 2) return false;
      
      return true;
      
    } catch (...) {
      throw;
    }
  }

  /* Static function to find a simplegroup in the Database */
  SimpleGroup* SimpleGroup::find(const string id)
  {
    if(DB<SimpleGroup>().check(id))
      return DB<SimpleGroup>().at(id);
    else
      return NULL;
  }

  /* Static function to get a simplegroup from the Database or create it otherwise */
  SimpleGroup* SimpleGroup::get(const string id)
  {
    SimpleGroup *sg = SimpleGroup::find(id);
    if(sg != NULL)
      return sg;
    else
    {
      sg = new SimpleGroup(id);
      DB<SimpleGroup>().set(id, sg);
      return sg;
    }
  }

  /* Overloaded << operator with simple groups on the right */
  ostream &operator<<(ostream &stream, const SimpleGroup &g) {
    stream << g.label();
    return stream;
  }

  
}
