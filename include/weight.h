/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * weight.h
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 13/08/2013
 */

#ifndef __WEIGHT_H
#define __WEIGHT_H

#include <iomanip>
#include "matrix.h"
#include "libjson.h"
#include "root.h"

// Forward declarations
namespace Tomb
{
  class SimpleGroup;
  class LieGroup;
}

using namespace std;

/*****************************/
/* Class Weight declarations */
/*****************************/

namespace Tomb
{
  class Weight : public RVector<double> {
    private:
      LieGroup *_Group = NULL;
      bool _positive = false;
      int _multiplicity = 0;
      int _level = 0;
    public:
      Weight();
      Weight(LieGroup &, int);
      Weight(LieGroup &, const RVector<double> &);
      Weight(const SimpleGroup &, int);
      Weight(const SimpleGroup &, const RVector<double> &);
      Weight(const string &, int);
      Weight(const string &, const RVector<double> &);
      Weight(const string);
//      Weight(const JSONNode &);
      Weight(const SimpleGroup &, const double);
      Weight(const Weight &);
      Weight(Weight &&);
      ~Weight();
      Weight &operator=(const Weight &);
      Weight &operator=(Weight &&);
      
      string id() const;
      LieGroup &Group() const;
      bool positive() const;
      int multiplicity() const;
      int level() const;
      void setPositive(bool);
      void setMultiplicity(int);
      void setLevel(int);
      Weight Dual();
      Weight operator+(Weight);
      Weight operator-(Weight);
      Weight operator+(Root);
      Weight operator-(Root);
      Weight operator++(int);
      double operator*(Weight);
      double operator*(Root);
      double operator*(CVector<double>);
      Weight operator*(Matrix<double>);
      bool operator>(const Weight) const;
      bool operator<(const Weight) const;
      Weight Append(Weight);
//      JSONNode json(string = "") const;
      static Weight* find(const string);
      static Weight* get(const string);
  };

  Weight operator*(Weight, double);
  Weight operator*(double, Weight);
  Weight operator*(Matrix<double>, Weight);
}

#endif /* __WEIGHT_H */
