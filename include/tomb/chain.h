/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
* \file
* chain.h
*
* \author
* T. Gonzalo (t.e.gonzalo@fys.uio.no)
*
* \date
* 12/01/2016 
*/

#ifndef __CHAIN_H
#define __CHAIN_H

#include "linkedlists.h"
#include "tree.h"
#include "libjson.h"
#include "simplegroup.h"
#include "subgroup.h"

/****************************/
/* Class Chain declarations */
/****************************/

namespace Tomb
{
  class Chain : public List<Tree<SimpleGroup> >
  {
    
    protected:
      
      int _depth;
      
    public:
      
      Chain();
      Chain(const Tree<SimpleGroup> &);
      Chain(const SimpleGroup &);
      Chain(const JSONNode &);
      Chain(const Chain &);
      Chain(const List<Tree<SimpleGroup> > &);
      Chain(Chain &&);
      ~Chain();
      Chain &operator=(const Chain &);
      Chain &operator=(const List<Tree<SimpleGroup> > &);
      Chain &operator=(Chain &&);
      
      int depth() const;
      int calculateDepth();
      
      List<SimpleGroup> getLevel(int) const;
      List<std::string> getLabels(int) const;

      List<SubGroup> extractSubgroups() const;
      
      void ParseJSON(const JSONNode &);
  };
  
}

#endif /* __CHAIN_H */
