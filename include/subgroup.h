/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * subgroup.h
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 15/02/2012
 */

#ifndef __SUBGROUP_H
#define __SUBGROUP_H

#include "matrix.h"
#include "linkedlists.h"
#include "libjson.h"
#include "helper_functions.h"
#include "liegroup.h"
#include "simplegroup.h"

/*******************************/
/* Class SubGroup declarations */
/*******************************/

namespace Tomb
{
  class SubGroup: public LieGroup {

    private:
      List<Tree<SimpleGroup> > _SuperGroups;
      Matrix<double> _Projection;
      List<std::string> _labels;
      bool _maximal = false;
      bool _regular = false;
      bool _special = false;

    public:
      SubGroup();
      SubGroup(const LieGroup &);
      SubGroup(const SimpleGroup &);
      SubGroup(const std::string);
      SubGroup(const std::string, Matrix<double>);
      SubGroup(const JSONNode &);
      SubGroup(const SubGroup &);
      SubGroup(const LieGroup &, const LieGroup &);
      SubGroup(const LieGroup &, const SimpleGroup &);
      SubGroup(const SimpleGroup &, const SimpleGroup &);
      SubGroup(const SubGroup &, const SimpleGroup &);
      SubGroup(const SubGroup &, const SubGroup &);
      SubGroup(SubGroup &&);
      ~SubGroup();
      SubGroup &operator=(const SubGroup &);
      SubGroup &operator=(SubGroup &&);
      
      void init(const std::string);
      List<Tree<SimpleGroup> > SuperGroups() const;
      LieGroup SuperGroup() const;
      LieGroup SuperGroup(int) const;
      Matrix<double> Projection() const;
      Matrix<double> SetProjection(Matrix<double>);
      List<std::string> labels() const;
      void setLabels(const List<std::string>);
      void AddTerm(const SimpleGroup &, int = 0);
      void AddTerm(const LieGroup &, int = 0);
      void AddTerm(const SubGroup &, int = 0);
      void DeleteTerm(int);
      void Order();
      bool isSubgroupOf(LieGroup) const;
      bool isSubgroupOf(SimpleGroup) const;
      bool maximal() const;
      bool setMaximal(bool);
      bool regular() const;
      bool setRegular(bool);
      bool special() const;
      bool setSpecial(bool);
      std::string id() const;
      bool operator==(const SubGroup &) const;
      bool operator!=(const SubGroup &) const;
      JSONNode json(std::string = "") const;
      void ParseJSON(const JSONNode &, std::string = "");
  };	
  
}

#endif /* __SUBGROUP_H */
