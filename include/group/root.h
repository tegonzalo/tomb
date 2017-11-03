/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * root.h
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 13/08/2013
 */

#ifndef __ROOT_H
#define __ROOT_H

#include "matrix.h"
#include "database.h"

// Forward declarations
namespace Tomb
{
  class SimpleGroup;
}

using namespace std;

/***************************/
/* Class Root declarations */
/***************************/

namespace Tomb 
{
  class Root : public RVector<double> {
		
    private:
     double _length = 0;
    public:
      string _Group = "";

      Root(const SimpleGroup &, int);
      Root(const SimpleGroup &, const RVector<double> &);
      Root(const string);
      Root(const JSONNode &);
      Root(const Root &);
      Root(Root &&);
      ~Root();
      Root operator=(const Root &);
      Root operator=(Root &&);
			
      string id() const;
      SimpleGroup &Group() const;
      string GroupId() const;
      double calculateLength(const SimpleGroup &);
      double length() const;
      Root Dual();
      JSONNode json(string = "") const;
      Root operator+(Root);
      Root operator-(Root);
      double operator*(Root);
     double operator*(CVector<double>);
     Root operator*(Matrix<double>);
			
  };

  Root operator*(Root, double);
  Root operator*(double, Root);
}

#endif /* __ROOT_H */
