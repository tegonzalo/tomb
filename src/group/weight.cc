/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * weight.cc
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 13/08/2013
 */

#include "weight.h"
#include "liegroup.h"

/****************************/
/* Class Weight definitions */
/****************************/

namespace Tomb
{
  /* Constructor, empty */
  Weight::Weight() : RVector<double>()
  {
    _positive = false;
    _multiplicity = 0;
    _level = 0;
  }

  /* Constructor 1 */
  Weight::Weight(LieGroup &G, int dim) : RVector<double>(dim)
  {
    try
    {
//      _Group = &G;
      _Group = G.id();
      _positive = true;
      _multiplicity = 1;
      _level = 0;
    }
    catch (...) { throw; }
  }
  
  /* Constructor 2 */
  Weight::Weight(LieGroup &G, const RVector<double> &V) : RVector<double>(V)
  {
    try
    {
//      _Group = &G;
      _Group = G.id();
      _positive = true;
      for(int i=0; i<V.cols(); i++)
      {
        if(V[i] < 0)  _positive = false;
      }
      _multiplicity = 1;
      _level = 0;
    }
    catch (...) { throw; }
  }
  
  /* Constructor 3 */
  Weight::Weight(const SimpleGroup &G, int dim) : RVector<double>(dim)
  {
    try
    {
//      _Group = LieGroup::get(G);
      _Group = G.id();
      _positive = true;
      _multiplicity = 1;
      _level = 0;
    }
    catch (...) { throw; }
  }
  
  /* Constructor 4 */
  Weight::Weight(const SimpleGroup &G, const RVector<double> &V) : RVector<double>(V)
  {
    try
    {
//      _Group = LieGroup::get(G); 
      _Group = G.id();
      _positive = true;
      for(int i=0; i<V.cols(); i++) 
      {
        if(V[i] < 0)  _positive = false;
      }
      _multiplicity = 1;
      _level = 0;
    }
    catch (...) { throw; }
  }
  
  /* Constructor 5 */
  Weight::Weight(const string &G, int dim) : RVector<double>(dim)
  {
    try
    {
//      _Group = DB<LieGroup>().get(G);
      _Group = G;
      _positive = true;
      _multiplicity = 1;
      _level = 0;
    }
    catch (...) { throw; }
  }
  
  /* Constructor 6 */
  Weight::Weight(const string &G, const RVector<double> &V) : RVector<double>(V)
  {
    try
    {
//      _Group = DB<LieGroup>().get(G);
      _Group = G;
      _positive = true;
      for(int i=0; i<V.cols(); i++)
      {
        if(V[i] < 0) _positive = false;
      }
      _multiplicity = 1;
      _level = 0;
    }
    catch (...) { throw; }
  }

  /* Constructor 5, with strings */
  Weight::Weight(const string id) : RVector<double>()
  {
    try
    {
      stringstream ss(id);

      string W;
      getline(ss, W, '(');
      getline(ss, W, ')');
      string G;
      getline(ss, G, '\0');

//      if(Group == NULL)
//        _Group = DB<LieGroup>().get(G);
//      else
//        _Group = Group;
      _Group = G;

      *this = Weight(Group(),Group().rank());
  
      stringstream Ws(W);
      string s;
      for(int i=0; i<this->cols()-1; i++)
      {
        getline(Ws,s,',');
        (*this)[i] = atof(s.c_str());
      }
      getline(Ws,s,')');
      (*this)[this->cols()-1] = atof(s.c_str());
      
      _positive = true;
      _multiplicity = 1;
      _level = 0;
      

    }
    catch (...) { throw; }
  }

  /* Constructor 6, with json nodes */
  Weight::Weight(const JSONNode &n)
  {   
    if(n.as_string() != "")
      Weight(n.as_string());
    else
    {
      ParseJSON(n);
    }
  }

  /* Constructor 7, with doubles */
  Weight::Weight(const SimpleGroup &G, const double value) : RVector<double>(1)
  {
    try
    {
//      _Group = DB<LieGroup>().get(G.id());
      _Group = G.id();
      (*this)[0] = value;
      _positive = true;
      _multiplicity = 1;
      _level = 0;
    } 
    catch (...) { throw; }
  }

  /* Copy constructor */
  Weight::Weight(const Weight &w) : RVector<double>(w)
  {
    try
    {
//      _Group = &w.Group();
      _Group = w._Group;
      _positive = w.positive();
      _multiplicity = w.multiplicity();
      _level = w.level();
    }
    catch (...) { throw; }
  }

  /* Move constructor */
  Weight::Weight(Weight &&w) : 
    RVector<double>(move(w)),
    _Group(move(w._Group)),
    _positive(move(w._positive)),
    _multiplicity(move(w._multiplicity)),
    _level(move(w._level))
  {
    try
    {
//      w._Group = NULL;
      w._Group = "";
      w._positive = false;
      w._multiplicity = 0;
      w._level = 0;
    }
    catch (...) { throw; }
  }

  /* Destructor */
  Weight::~Weight()
  {
  }

  /* Overloaded = operator */
  Weight &Weight::operator=(const Weight &w)
  {
    try
    {
      if(this == &w) return *this;
      RVector<double>::operator=(w);
//      _Group = &w.Group();
      _Group = w._Group;
      _positive = w.positive();
      _multiplicity = w.multiplicity();
      _level = w.level();
      return *this;
    }
    catch (...) { throw; }
  }

  /* Move = operator */
  Weight &Weight::operator=(Weight &&w)
  {
    try
    {
      if(this == &w) return *this;
      RVector<double>::operator=(move(w));
      _Group = move(w._Group);
      _positive = move(w._positive);
      _multiplicity = move(w._multiplicity);
      _level = move(w._level);
      
//      w._Group = NULL;
      w._Group = "";
      w._positive = false;
      w._multiplicity = 0;
      w._level = 0;
      return *this;
    }
    catch (...) { throw; }
    
  }

  /* Indentifier of the weight */
  string Weight::id() const
  {
    stringstream s;
    s << '(';
    for(int i=0; i<cols()-1; i++) {
      if(fabs((*this)[i]) < pow(10,-5)) 
        s << 0;
      else
        s << setprecision(4) << (*this)[i];
      s << ",";
    }
    s << (*this)[cols()-1] << ')';
//    s << _Group->id();
    s << _Group;
    return s.str();
  }

  /* Returns the Lie Group */
  LieGroup &Weight::Group() const 
  {
//    return *_Group;
    return *DB<LieGroup>().get(_Group);
  }
  
  /* Returns the positiveness of the weight */
  bool Weight::positive() const
  {
    return _positive;
  }

  /* Returns the multiplicity of the weight */
  int Weight::multiplicity() const
  {
    return _multiplicity;
  }

  /* Returns the level of the weight */
  int Weight::level() const
  {
    return _level;
  }

  /* Sets if the weight if positive */
  void Weight::setPositive(bool pos)
  {
    _positive = pos;
  }

  /* Set the multiplicity of the weight */
  void Weight::setMultiplicity(int mult)
  {
    _multiplicity = mult;
  }

  /* Set the level of the weight */
  void Weight::setLevel(int lev)
  {
    _level = lev;
  }

  /* Returns the dual of a weight */
  Weight Weight::Dual()
  {
    try
    {
//      if(_Group->nterms() == 1)
        if(Group().nterms() == 1)
      {
//        Weight w = (*this)*_Group->GetObject(0).G().Transpose();
        Weight w = (*this)*Group().GetObject(0).G().Transpose();
        return w;
      } 
      return *this;
    }
    catch (...) { throw; }
  }

  /* Overloaded + operator */
  Weight Weight::operator+(Weight w)
  {
    try
    {
      Weight v = *this;
      v += w;
      v.setPositive(true);
      for(int i=0; i<v.cols(); i++)
        if(int(v[i]) < 0)
          v.setPositive(false);
      return v;
    } 
    catch (...) { throw; }
  }

  /* Overloaded - operator */
  Weight Weight::operator-(Weight w)
  {
    try
    {
      Weight v = *this;
      v -= w;
      v.setPositive(true);
      for(int i=0; i<v.cols(); i++)
        if(int(v[i]) < 0)
          v.setPositive(false);
      return v;
    }
    catch (...) { throw; }
  }

  /* Overloaded + operator with roots */
  Weight Weight::operator+(Root alpha)
  {
    try
    {
      Weight w = *this;
      w += alpha;
      w.setPositive(true);
      for(int i=0; i<w.cols(); i++)
        if(int(w[i]) < 0)
          w.setPositive(false);
      return w;
    }
    catch (...) { throw; }
  }

  /* Overloaded - operator with roots */
  Weight Weight::operator-(Root alpha)
  {
    try
    {
      Weight w = *this;
      w -= alpha;
      w.setPositive(true);
      for(int i=0; i<w.cols(); i++)
        if(int(w[i]) < 0)
          w.setPositive(false);
      return w;
    }
    catch (...) { throw; }
  }

  /*Overloaded ++ operator */
  Weight Weight::operator++(int n)
  {
    try
    {
      Weight w = *this;
      for(int i=0; i<this->cols(); i++)
        w[i] ++;
      return w;
    } 
    catch (...) { throw; }
  }

  /* Overloaded * operator for scalar products of weights */
  double Weight::operator*(Weight w)
  {
    try
    {
      CVector<double> C = w.Transpose();
      double product = this->Dual()*C;
      return product;
    } 
    catch (...) { throw; }
  }

  /*Overloaded * operator for scalar products with a root */
  double Weight::operator*(Root alpha)
  {
    try
    {
      CVector<double> C = alpha.Transpose();
      double product = this->Dual()*C;
      return product;
    }
    catch (...) { throw; }
  }

  /* Overloaded * operator with column vectors */
  double Weight::operator*(CVector<double> C)
  {
    try
    {
      double result = RVector<double>(*this)*C;
      return result;
    }
    catch (...) { throw; }
  }

  /* Overloaded * operator with matrices */
  Weight Weight::operator*(Matrix<double> M)
  {
    try
    {
      Weight w(Group(), RVector<double>(*this)*M);
      return w;
    }
    catch (...) { throw; }
  }

  /* Overloaded > operator */
  bool Weight::operator>(const Weight w) const
  {
    if(this->level() > w.level())
      return true;
    return false;
  }

  /* Overloaded < operator */
  bool Weight::operator<(const Weight w) const
  {
    if(this->level() < w.level())
      return true;
    return false;
  }

  /* Overloaded == operator */
  bool Weight::operator==(const Weight &w) const
  {
    if(cols() != w.cols()) return false;

    if(RVector<double>::operator==(w))
      return true;

    // TODO: This is hacky, replace with better comparison
    for(int i=0; i<cols(); i++)
      if (fabs((*this)[i] - w[i]) > 1e-3)
        return false;
     return true;
  }

  /* Overloaded != operator */
  bool Weight::operator!=(const Weight &w) const
  {
    return !((*this) == w);
  }

  /* Overloaded == operator with doubles */
  bool Weight::operator==(const double &n) const
  {
    return RVector<double>::operator==(n);
  }

  /* Overloaded != operator with doubles */
  bool Weight::operator!=(const double &n) const
  {
    return RVector<double>::operator!=(n);
  }

  /* Append a weight to the right */
  Weight Weight::Append(Weight w)
  {
    try
    {
      RVector<double>::Append(w);
//      string G(_Group->id());
//      G.push_back('x');
//      G.append(w.Group().id());
//      _Group = DB<LieGroup>().get(G);
      _Group.append(w._Group);
      
      if(positive() and w.positive())
        setPositive(true);
      else 
        setPositive(false);
      return *this;
    }
    catch (...) { throw; }
  }

  /* Print to json format */
  JSONNode Weight::json(string name) const
  {
    try
    {
      if(name == "id")
        return JSONNode("", id());
      else
        return RVector<double>::json(name);
    }
    catch (...) { throw; }
  }

  /* Overloaded * operator with scalars on the right */
  Weight operator*(Weight w, double n)
  {
    try 
    {
      Weight v(w.Group(), RVector<double>(w)*n);
      return v;
    }
    catch (...) { throw; }
  }

  /* Overloaded * operator with scalars on the left */
  Weight operator*(double n, Weight w)
  {
    try
    {
      Weight v(w.Group(), RVector<double>(w)*n);
      return v;
    }
    catch (...) { throw; }
  }

  /* Overloaded * operator with matrices on the left */
  Weight operator*(Matrix<double> M, Weight w)
  {
    try
    {
      return w*(M.Transpose());
    }
    catch (...) { throw; }
  }
}
