/*
* weight.cc
* Created by T.Gonzalo on 13/08/2013.
* Last modified on 10/11/2015
*/

#include "headers.h"


/**************************************************/
/**************Class Weight methods****************/
/**************************************************/

namespace Tomb
{
  /* Constructor, empty */
  Weight::Weight() : RVector<double>() {
    _positive = false;
    _multiplicity = 0;
    _level = 0;
  }

  /* Constructor 1 */
  Weight::Weight(const LieGroup &G, int dim) : RVector<double>(dim) {
    try {
      //_Group = new LieGroup(G);
      _Group = G.id();
      _positive = true;
      _multiplicity = 1;
      _level = 0;
    } catch (...) {
      throw;
    }
  }
  
  /* Constructor 2 */
  Weight::Weight(const LieGroup &G, const RVector<double> &V) : RVector<double>(V) {
    try {
      //_Group = new LieGroup(G);
      _Group = G.id();
      _positive = true;
      for(int i=0; i<V.cols(); i++) {
        if(V[i] < 0) {
          _positive = false;
        }
      }
      _multiplicity = 1;
      _level = 0;
    } catch (...) {
      throw;
    }
  }
  
  /* Constructor 3 */
  Weight::Weight(const SimpleGroup &G, int dim) : RVector<double>(dim) {
    try {
      //_Group = new LieGroup(G);
      _Group = G.id();
      _positive = true;
      _multiplicity = 1;
      _level = 0;
    } catch (...) {
      throw;
    }
  }
  
  /* Constructor 4 */
  Weight::Weight(const SimpleGroup &G, const RVector<double> &V) : RVector<double>(V) {
    try {
      //_Group = new LieGroup(G);
      _Group = G.id();
      _positive = true;
      for(int i=0; i<V.cols(); i++) {
        if(V[i] < 0) {
          _positive = false;
        }
      }
      _multiplicity = 1;
      _level = 0;
    } catch (...) {
      throw;
    }
  }
  
  /* Constructor 5 */
  Weight::Weight(const std::string &G, int dim) : RVector<double>(dim) {
    try {
      _Group = G;
      _positive = true;
      _multiplicity = 1;
      _level = 0;
    } catch (...) {
      throw;
    }
  }
  
  /* Constructor 6 */
  Weight::Weight(const std::string &G, const RVector<double> &V) : RVector<double>(V) {
    try {
      _Group = G;
      _positive = true;
      for(int i=0; i<V.cols(); i++) {
        if(V[i] < 0) {
          _positive = false;
        }
      }
      _multiplicity = 1;
      _level = 0;
    } catch (...) {
      throw;
    }
  }

  /* Constructor 5, with strings */
  Weight::Weight(const std::string id) : RVector<double>() {
    try {
      std::stringstream ss(id);

      std::string W;
      getline(ss, W, '(');
      getline(ss, W, ')');
      std::string G;
      getline(ss, G, '\0');
            
      //_Group = new LieGroup(G);
      _Group = G;

      //*this = Weight(Group(),Group().rank());
      
      W.insert(0,"[[");
      W.append("]]");
            
      RVector<double>::ParseJSON(libjson::parse(W));

      /*std::stringstream Ws(W);
      std::string s;
      for(int i=0; i<this->cols()-1; i++) {
        getline(Ws,s,',');
        (*this)[i] = atof(s.c_str());
      }
      getline(Ws,s,')');
      (*this)[this->cols()-1] = atof(s.c_str());*/
      
      _positive = true;
      _multiplicity = 1;
      _level = 0;
      

    } catch (...) {
      throw;
    }
  }

  /* Constructor 6, with json nodes */
  Weight::Weight(const JSONNode &n) {
    
    if(n.as_string() != "") {
      Weight(n.as_string());
    } else {
      ParseJSON(n);
    }
  }

  /* Constructor 7, with doubles */
  Weight::Weight(const SimpleGroup &G, const double value) : RVector<double>(1) {
    try {
      //_Group = new LieGroup(G);
      _Group = G.id();
      (*this)[0] = value;
      _positive = true;
      _multiplicity = 1;
      _level = 0;
    } catch (...) {
      throw;
    }
  }

  /* Copy constructor */
  Weight::Weight(const Weight &w) : RVector<double>(w) {
    try {
      //_Group = new LieGroup(w.Group());
      _Group = w.GroupId();
      _positive = w.positive();
      _multiplicity = w.multiplicity();
      _level = w.level();
    } catch (...) {
      throw;
    }
  }

  /* Move constructor */
  Weight::Weight(Weight &&w) : 
    RVector<double>(std::move(w)),
    _Group(std::move(w._Group)),
    _positive(std::move(w._positive)),
    _multiplicity(std::move(w._multiplicity)),
    _level(std::move(w._level))
  {
    try {
      //w._Group = nullptr;
      w._Group = "";
      w._positive = false;
      w._multiplicity = 0;
      w._level = 0;
    } catch (...) {
      throw;
    }
  }

  /* Destructor */
  Weight::~Weight() {
    //if(_Group != nullptr) delete _Group;
  }

  /* Overloaded = operator */
  Weight &Weight::operator=(const Weight &w) {
    
    try {
      if(this == &w) return *this;
      //if(_Group != nullptr) delete _Group;
      RVector<double>::operator=(w);
      //_Group = new LieGroup(w.Group());
      _Group = w.GroupId();
      _positive = w.positive();
      _multiplicity = w.multiplicity();
      _level = w.level();
      return *this;
    } catch (...) {
      throw;
    }
    
  }

  /* Move = operator */
  Weight &Weight::operator=(Weight &&w) {
    
    try {
      if(this == &w) return *this;
      //if(_Group != nullptr) delete _Group;
      RVector<double>::operator=(std::move(w));
      _Group = std::move(w._Group);
      _positive = std::move(w._positive);
      _multiplicity = std::move(w._multiplicity);
      _level = std::move(w._level);
      
      //w._Group = nullptr;
      w._Group = "";
      w._positive = false;
      w._multiplicity = 0;
      w._level = 0;
      return *this;
    } catch (...) {
      throw;
    }
    
  }

  /* Indentifier of the weight */
  std::string Weight::id() const {
    std::stringstream s;
    s << '(';
    for(int i=0; i<cols()-1; i++) {
      if(fabs((*this)[i]) < pow(10,-5)) 
        s << 0;
      else
        s << std::setprecision(3) << (*this)[i];
      s << ",";
    }
    s << (*this)[cols()-1] << ')';
    //s << _Group->id();
    s << _Group;
    return s.str();
  }

  /* Returns the Lie Group */
  LieGroup Weight::Group() const 
  {
    return LieGroup(_Group);
  }
  
  /* Returns the Group id */
  std::string Weight::GroupId() const {
    //return *_Group;
    return _Group;
  }

  /* Returns the positiveness of the weight */
  bool Weight::positive() const {
    return _positive;
  }

  /* Returns the multiplicity of the weight */
  int Weight::multiplicity() const {
    return _multiplicity;
  }

  /* Returns the level of the weight */
  int Weight::level() const {
    return _level;
  }

  /* Sets if the weight if positive */
  void Weight::setPositive(bool pos) {
    _positive = pos;
  }

  /* Set the multiplicity of the weight */
  void Weight::setMultiplicity(int mult) {
    _multiplicity = mult;
  }

  /* Set the level of the weight */
  void Weight::setLevel(int lev) {
    _level = lev;
  }

  /* Returns the dual of a weight */
  Weight Weight::Dual() {
    try {
      LieGroup G(_Group);
      if(G.nterms() == 1)
      //if(this->Group().nterms() == 1)
      {
        //RVector<double> R = (*this)*this->Group()->GetObject(0)->G().Transpose();
        Weight w = (*this)*G.GetObject(0).G().Transpose();
        //Weight w(this->Group(),R);
        return w;
      } 
      return *this;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded + operator */
  Weight Weight::operator+(Weight w) {
    try {
      Weight v = *this;
      v += w;
      v.setPositive(true);
      for(int i=0; i<v.cols(); i++) {
        if(int(v[i]) < 0) {
          v.setPositive(false);
        }
      }
      return v;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded - operator */
  Weight Weight::operator-(Weight w) {
    try {
      Weight v = *this;
      v -= w;
      v.setPositive(true);
      for(int i=0; i<v.cols(); i++) {
        if(int(v[i]) < 0) {
          v.setPositive(false);
        }
      }
      return v;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded + operator with roots */
  Weight Weight::operator+(Root alpha) {
    try {
      Weight w = *this;
      w += alpha;
      w.setPositive(true);
      for(int i=0; i<w.cols(); i++) {
        if(int(w[i]) < 0) {
          w.setPositive(false);
        }
      }
      return w;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded - operator with roots */
  Weight Weight::operator-(Root alpha) {
    try {
      Weight w = *this;
      w -= alpha;
      w.setPositive(true);
      for(int i=0; i<w.cols(); i++) {
        if(int(w[i]) < 0) {
          w.setPositive(false);
        }
      }
      return w;
    } catch (...) {
      throw;
    }
  }

  /*Overloaded ++ operator */
  Weight Weight::operator++(int n) {
    try {
  /*
      Weight *Delta = new Weight(this->Group(),this->cols());
      for(int i=0; i<this->cols(); i++) {
        (*Delta)[i] = 1;
      }
      return *Delta + *this;
      Weight Delta = *this;
      for(int i=0; i<this->cols(); i++) {
        Delta[i] = 1;
      }
      return Delta + *this;
  */
      Weight w = *this;
      for(int i=0; i<this->cols(); i++) {
        w[i] ++;
      }
      return w;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded * operator for scalar products of weights */
  double Weight::operator*(Weight w) {
    try {
      CVector<double> C = w.Transpose();
      double product = this->Dual()*C;
      return product;
    } catch (...) {
      throw;
    }
  }

  /*Overloaded * operator for scalar products with a root */
  double Weight::operator*(Root alpha) {
    try {
      CVector<double> C = alpha.Transpose();
      double product = this->Dual()*C;
      return product;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded * operator with column vectors */
  double Weight::operator*(CVector<double> C) {
    try {
      double result = RVector<double>(*this)*C;
      return result;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded * operator with matrices */
  Weight Weight::operator*(Matrix<double> M) {
    try {
      Weight w(_Group, RVector<double>(*this)*M);
      //Weight w(this->Group(), RVector<double>(*this)*M);
      return w;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded > operator */
  bool Weight::operator>(const Weight w) const {
    if(this->level() > w.level()) {
      return true;
    }
    return false;
  }

  /* Overloaded < operator */
  bool Weight::operator<(const Weight w) const {
    if(this->level() < w.level()) {
      return true;
    }
    return false;
  }

  /* Append a weight to the right */
  Weight Weight::Append(Weight w) {
    try {
      RVector<double>::Append(w);
      /*for(int i=0; i<w.Group().ngroups(); i++) {
        _Group->AddTerm(w.Group().GetObject(i));
      }*/
      _Group.push_back('x');
      _Group.append(w.GroupId());
      
      if(positive() and w.positive()) {
        setPositive(true);
      } else {
        setPositive(false);
      }
    } catch (...) {
      throw;
    }
    return *this;
  }

  /* Print to json format */
  JSONNode Weight::json(std::string name) const {

    try {
      if(name == "id") {
        return JSONNode("", id());
      } else {
        return RVector<double>::json(name);
      }
    } catch (...) {
      throw;
    }
  }

  /* Overloaded * operator with scalars on the right */
  Weight operator*(Weight w, double n) {
    try {
      //Weight v(w.Group(),RVector<double>(w)*n);
      Weight v(w.GroupId(), RVector<double>(w)*n);
      return v;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded * operator with scalars on the left */
  Weight operator*(double n, Weight w) {
    try {
      //Weight v(w.Group(),RVector<double>(w)*n);
      Weight v(w.GroupId(), RVector<double>(w)*n);
      return v;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded * operator with matrices on the left */
  Weight operator*(Matrix<double> M, Weight w){
    try {
      return w*(M.Transpose());
    } catch (...) {
      throw;
    }
  }

}