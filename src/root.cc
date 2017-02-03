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
  Root::Root(SimpleGroup &G, int dim) : RVector<double>(dim) {
    try {
      _Group = &G;
      _length = calculateLength();
    } catch (...) {
      throw;
    }
  }

  /* Constructor 2 */
  Root::Root(SimpleGroup &G, const RVector<double> &alpha) : RVector<double>(alpha){

    try {
      _Group = &G;
      _length = calculateLength();
    } catch (...) {
      throw;
    }
  }

  /* Constructor 3 */
  Root::Root(const std::string id) : RVector<double>() {
    try {
      std::stringstream ss(id);

      std::string R;
      getline(ss, R, '(');
      getline(ss, R, ')');
      std::string G;
      getline(ss, G, '\0');
      
      _Group = SimpleGroup::get(G);

      *this = Root(Group(),Group().rank());
      
      std::stringstream Rs(R);
      std::string s;
      for(int i=0; i<this->cols()-1; i++) {
        getline(Rs,s,',');
        (*this)[i] = atoi(s.c_str());
      }
      getline(Rs,s,')');
      (*this)[this->cols()-1] = atoi(s.c_str());
    
      _length = calculateLength();
      
    } catch (...) {
      throw;
    }
  }

  /* Copy constructor */
  Root::Root(const Root &alpha) : RVector<double>(alpha) {
    
    try {
      _Group = &alpha.Group();
      _length = alpha.length();
    } catch (...) {
      throw;
    }
  }

  /* Move constructor */
  Root::Root(Root &&alpha) : 
    RVector<double>(std::move(alpha)),
    _Group(std::move(alpha._Group)),
    _length(std::move(alpha._length))
  {
    try {
      alpha._Group = nullptr;
      alpha._length = 0;
    } catch (...) {
      throw;
    }
  }

  /* Destructor */
  Root::~Root() {
    //std::cout << "deleting root" << std::endl;
    //if(_Group != nullptr) delete _Group;
  }

  /* Overloaded = operator */
  Root Root::operator=(const Root &alpha) {
    try {
      if(this == &alpha) return *this;
      //if(_Group != nullptr) delete _Group;
      RVector<double>::operator=(alpha);
      _Group = DB<SimpleGroup>().at(alpha.Group().id());
      _length = alpha.length();
      return *this;
    } catch (...) {
      throw;
    }
  }

  /* Move = operator */
  Root Root::operator=(Root &&alpha) {
    try {
      if(this == &alpha) return *this;
      //if(_Group != nullptr) delete _Group;
      RVector<double>::operator=(std::move(alpha));
      _Group = std::move(alpha._Group);
      _length = std::move(alpha._length);
      alpha._Group = nullptr;
      alpha._length = 0;
      return *this;
    } catch (...) {
      throw;
    }
  }

  /* Indentifier of the root */
  std::string Root::id() const {
    std::stringstream s;
    s << '(';
    for(int i=0; i<cols()-1; i++) {
      s << (*this)[i];
      s << ",";
    }
    s << (*this)[cols()-1] << ')';
    s << Group().id() << std::endl;
    return s.str();
  }

  /* Returns the Group */
  SimpleGroup &Root::Group() const {
    return *_Group;
  }

  /* Returns the value of length */
  double Root::length() const {
    return _length;
  }

  /* Calculate length */
  double Root::calculateLength() {
    
    double length = 0;
    
    if(_Group->type() == 'U' or _Group->type() == 'A' or _Group->type() == 'D') {
      return 2;
    }
    
    int issimpleroot=-1;
    for(int i=0; i<_Group->rank(); i++) {
      if(*this == _Group->Cartan().Row(i)) {
        issimpleroot = i;
      }
    }	
    //std::cout << "issimpleroot = " << issimpleroot << std::endl;
    
    if(issimpleroot>-1) {
      int longroot=0, shortroot=0;
      for(int i=0; i<this->Group().rank(); i++) {
        for(int j=0; j<i; j++) {
          if(this->Group().Cartan()[j][i]!=0 and j!=i) {
            if(this->Group().Cartan()[i][j]/this->Group().Cartan()[j][i] > 1) {
              longroot = i;
              shortroot = j;
            } else if(this->Group().Cartan()[i][j]/this->Group().Cartan()[j][i] < 1) {
              longroot = j;
              shortroot = i;
            }
          }
        }
      }
      //std::cout << "longroot = " << longroot << std::endl;
      //std::cout << "shortroot = " << shortroot << std::endl;

      if(issimpleroot == longroot) {
        //if(this->Group().type() == 'C') {
          //length = 1;
        //} else {
          length = 2;
        //}
      } else if(issimpleroot == shortroot) {
        length = this->Group().Cartan()[shortroot][longroot]/this->Group().Cartan()[longroot][shortroot]*this->Group().SRoot(longroot).length();
      } else {
        //std::cout << "neither" << std::endl;
        if(issimpleroot > longroot) {
          for(int i=longroot; i<issimpleroot; i++) {
            if(this->Group().Cartan()[i][issimpleroot] and issimpleroot != i) {
              Root alpha = this->Group().SRoot(i);
              length = this->Group().Cartan()[issimpleroot][i]/this->Group().Cartan()[i][issimpleroot]*alpha.length();
            }
          }
        } else {
          for(int i=longroot; i>issimpleroot; i--) {
            if(this->Group().Cartan()[i][issimpleroot] != 0 and issimpleroot != i and length == 0) {
              length = this->Group().Cartan()[issimpleroot][i]/this->Group().Cartan()[i][issimpleroot]*this->Group().SRoot(i).length();
            }
          }
        }
      }
    } else {
      length = 0;
      for(int i=0; i<this->Group().rank(); i++) {
        for(int j=0; j<this->Group().rank(); j++) {
          length += (*this)[i]*this->Group().G()[i][j]*(*this)[j];
        }
      }
    }
    
    return length;
  }

  /* Returns the root in the dual basis */
  Root Root::Dual() {
    
    try {
      //Matrix<double> GT = this->Group()->G().Transpose();
      //Root *alpha = new Root((*this)*(GT));
      Root alpha = (*this)*this->Group().G().Transpose();
      //std::cout << alpha->Print() << std::endl;
      //Weight w(this->Group(),R);
      return alpha;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded + operator */
  Root Root::operator+(Root alpha) {
    try {
      Root beta = *this;
      //(Root) RVector<double>(*this)+RVector<double>(alpha);
      beta += alpha;
      return beta;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded - operator */
  Root Root::operator-(Root alpha) {
    try {
      Root beta = *this;
      //Root *beta = new Root(this->Group(), (*this)+alpha*(-1));
      beta -= alpha;
      return beta;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded * operator for scalar products of roots */
  double Root::operator*(Root alpha) {
    try {
      CVector<double> C = alpha.Transpose();
      double product = this->Dual()*C;
      return product;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded * operator with column vectors */
  double Root::operator*(CVector<double> C) {
    try {
      double result = RVector<double>(*this)*C;
      return result;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded * operator with matrices */
  Root Root::operator*(Matrix<double> M) {
    try {
      //Root *alpha = new Root(this->Group(), RVector<double>(*this)*M);
      Root alpha(this->Group(), RVector<double>(*this)*M);
      return alpha;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded * operator with scalars on the right */
  Root operator*(Root alpha, double n) {
    try {
      //Root *beta = new Root(alpha.Group(),RVector<double>(alpha)*n);
      Root beta = alpha;
      beta *= n;
      return beta;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded * operator with scalars on the left */
  Root operator*(double n, Root alpha) {
    try {
      return alpha*n;
    } catch (...) {
      throw;
    }
  }

}
