/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * root.cc
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 13/08/2013
 */

#include "root.h"
#include "simplegroup.h"

/**************************/
/* Class Root definitions */
/**************************/

namespace Tomb
{

  /* Constructor 1 */
  Root::Root(const SimpleGroup &G, int dim) : RVector<double>(dim)
  {
    try
    {
      _Group = G.id();
      _length = calculateLength(G);
   }
    catch (...) { throw; }
  }

  /* Constructor 2 */
  Root::Root(const SimpleGroup &G, const RVector<double> &alpha) : RVector<double>(alpha)
  {
    try
    {
      _Group = G.id();
      _length = calculateLength(G);
    }
    catch (...) { throw; }
  }

  /* Constructor 3 */
  Root::Root(const string id) : RVector<double>()
  {
    try
    {
      stringstream ss(id);

      string R;
      getline(ss, R, '(');
      getline(ss, R, ')');
      string G;
      getline(ss, G, '\0');
      
      //_Group = DB<SimpleGroup>().get(G);
      _Group = G;

      *this = Root(Group(),Group().rank());
      
      stringstream Rs(R);
      string s;
      for(int i=0; i<this->cols()-1; i++)
      {
        getline(Rs,s,',');
        (*this)[i] = atoi(s.c_str());
      }
      getline(Rs,s,')');
      (*this)[this->cols()-1] = atoi(s.c_str());
    
      _length = calculateLength(Group());
      
    }
    catch (...) { throw; }
  }

  /* Constructor 4, with json nodes */
  Root::Root(const JSONNode &n)
  {
    if(n.as_string() != "")
      Root(n.as_string());
    else
    {
      throw "Root::Root::No ParseJSON for roots";
    }
  }



  /* Copy constructor */
  Root::Root(const Root &alpha) : RVector<double>(alpha)
  {   
    try
    {
      _Group = alpha._Group;
      _length = alpha.length();
    }
    catch (...) { throw; }
  }

  /* Move constructor */
  Root::Root(Root &&alpha) : 
    RVector<double>(move(alpha)),
    _Group(move(alpha._Group)),
    _length(move(alpha._length))
  {
    try
    {
      alpha._Group = "";
      alpha._length = 0;
    }
    catch (...) { throw; }
  }

  /* Destructor */
  Root::~Root()
  {
 
  }

  /* Overloaded = operator */
  Root Root::operator=(const Root &alpha)
  {
    try
    {
      if(this == &alpha) return *this;
      //if(_Group != nullptr) delete _Group;
      RVector<double>::operator=(alpha);
      _Group = alpha._Group;
      _length = alpha.length();
      return *this;
    }
    catch (...) { throw; }
  }

  /* Move = operator */
  Root Root::operator=(Root &&alpha)
  {
    try
    {
      if(this == &alpha) return *this;
      //if(_Group != nullptr) delete _Group;
      RVector<double>::operator=(move(alpha));
      _Group = move(alpha._Group);
      _length = move(alpha._length);
      alpha._Group = "";
      alpha._length = 0;
      return *this;
    }
    catch (...) { throw; }
  }

  /* Indentifier of the root */
  string Root::id() const
  {
    stringstream s;
    s << '(';
    for(int i=0; i<cols()-1; i++)
    {
      s << (*this)[i];
      s << ",";
    }
    s << (*this)[cols()-1] << ')';
    s << _Group;
    return s.str();
  }

  /* Returns the Group */
  SimpleGroup &Root::Group() const
  {
    return *DB<SimpleGroup>().get(_Group);
  }

  /* Returns the id of the group */
  string Root::GroupId() const
  {
    return _Group;
  }

  /* Returns the value of length */
  double Root::length() const
  {
    return _length;
  }

  /* Calculate length */
  double Root::calculateLength(const SimpleGroup &G)
  {
    double length = 0;
    
    if(G.type() == 'U' or G.type() == 'A' or G.type() == 'D')
      return 2;
    
    int issimpleroot=-1;
    for(int i=0; i<G.rank(); i++)
      if(*this == G.Cartan().Row(i))
        issimpleroot = i;
    
    if(issimpleroot>-1)
    {
      int longroot=0, shortroot=0;
      for(int i=0; i<G.rank(); i++)
      {
        for(int j=0; j<i; j++)
        {
          if(G.Cartan()[j][i]!=0 and j!=i)
          {
            if(G.Cartan()[i][j]/G.Cartan()[j][i] > 1) 
            {
              longroot = i;
              shortroot = j;
            }
            else if(G.Cartan()[i][j]/G.Cartan()[j][i] < 1)
            {
              longroot = j;
              shortroot = i;
            }
          }
        }
      }

      if(issimpleroot == longroot)
          length = 2;
      else if(issimpleroot == shortroot)
        length = G.Cartan()[shortroot][longroot]/G.Cartan()[longroot][shortroot]*G.SRoot(longroot).length();
      else
      {
        if(issimpleroot > longroot)
        {
          for(int i=longroot; i<issimpleroot; i++)
          {
            if(G.Cartan()[i][issimpleroot] and issimpleroot != i)
            {
              Root alpha = G.SRoot(i);
              length = G.Cartan()[issimpleroot][i]/G.Cartan()[i][issimpleroot]*alpha.length();
            }
          }
        }
        else
        {
          for(int i=longroot; i>issimpleroot; i--)
            if(G.Cartan()[i][issimpleroot] != 0 and issimpleroot != i and length == 0)
              length = G.Cartan()[issimpleroot][i]/G.Cartan()[i][issimpleroot]*G.SRoot(i).length();
        }
      }
    }
    else
    {
      length = 0;
      for(int i=0; i<G.rank(); i++)
        for(int j=0; j<G.rank(); j++)
          length += (*this)[i]*G.G()[i][j]*(*this)[j];
    }
    
    return length;
  }

  /* Returns the root in the dual basis */
  Root Root::Dual()
  {   
    try
    {
      Root alpha = (*this)*this->Group().G().Transpose();
      return alpha;
    }
    catch (...) { throw; }
  }

  /* Print to json format */
  JSONNode Root::json(string name) const
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

  /* Overloaded + operator */
  Root Root::operator+(Root alpha)
  {
    try
    {
      Root beta = *this;
      beta += alpha;
      return beta;
    }
    catch (...) { throw; }
  }

  /* Overloaded - operator */
  Root Root::operator-(Root alpha)
  {
    try
    {
      Root beta = *this;
      beta -= alpha;
      return beta;
    }
    catch (...) { throw; }
  }

  /* Overloaded * operator for scalar products of roots */
  double Root::operator*(Root alpha)
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
  double Root::operator*(CVector<double> C)
  {
    try
    {
      double result = RVector<double>(*this)*C;
      return result;
    }
    catch (...) { throw; }
  }

  /* Overloaded * operator with matrices */
  Root Root::operator*(Matrix<double> M)
  {
    try
    {
      Root alpha(this->Group(), RVector<double>(*this)*M);
      return alpha;
    }
    catch (...) { throw; }
  }

  /* Overloaded * operator with scalars on the right */
  Root operator*(Root alpha, double n)
  {
    try
    {
      Root beta = alpha;
      beta *= n;
      return beta;
    }
    catch (...) { throw; }
  }

  /* Overloaded * operator with scalars on the left */
  Root operator*(double n, Root alpha)
  {
    try
    {
      return alpha*n;
    }
    catch (...) { throw; }
  }

}
