/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * chain.cc
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 12/01/2016
 */

#include "chain.h"

/***************************/
/* Class Chain definitions */
/***************************/

namespace Tomb
{

  // Member functions

  /* Constructor 0 */
  Chain::Chain() : List<Tree<SimpleGroup> >() 
  {
    try
    {
      _depth = 0;
      
    } catch (...)
    {
      throw;
    }
  }

  /* Constructor 1 */
  Chain::Chain(const Tree<SimpleGroup> &tree) : List<Tree<SimpleGroup> >(tree)
  {
    try
    {
      _depth = 1;
      
    } catch (...)
    {
      throw;
    }
  }
  
  /* Constructor 2 */
  Chain::Chain(const SimpleGroup &group) : List<Tree<SimpleGroup> >(Tree<SimpleGroup>(group))
  {
    try
    {
      _depth = 1;
      
    } catch (...)
    {
      throw;
    }
  }
  
  
  /* Constructor 3 */
  Chain::Chain(const JSONNode &json)
  {
    try
    {
      ParseJSON(json);
      
    } catch (...)
    {
      throw;
    }
  }

  /* Copy constructor 1 */
  Chain::Chain(const Chain &chain) : List<Tree<SimpleGroup> >(chain)
  {
    try
    {
      _depth= chain.depth();
      
    } catch (...)
    {
      throw;
    }
  }
  
  /* Copy constructor 2 */
  Chain::Chain(const List<Tree<SimpleGroup> > &chain) : List<Tree<SimpleGroup> >(chain)
  {
    try
    {
      _depth = chain.nterms();
      
    } catch (...)
    {
      throw;
    }
  }

  /* Move constructor */
  Chain::Chain(Chain && chain) :
    List<Tree<SimpleGroup> >(std::move(chain)),
    _depth(std::move(chain._depth))
  {
    try
    {
      chain._depth = 0;
      
    } catch (...)
    {
      throw;
    }
  }

  /* Destructor */
  Chain::~Chain()
  {
  }

  /* Assignment operator 1 */
  Chain &Chain::operator=(const Chain &chain)
  {
    try
    {
      if(this == &chain) return *this;
        
      List<Tree<SimpleGroup> >::operator=(chain);
      
      _depth = chain.depth();
      
      return *this;
    } catch (...)
    {
      throw;
    }
  }

  /* Assignment operator 2 */
  Chain &Chain::operator=(const List<Tree<SimpleGroup> > &chain)
  {
    try
    {
      if(this == &chain) return *this;
        
      List<Tree<SimpleGroup> >::operator=(chain);
      
      _depth = chain.nterms();
      
      return *this;
    } catch (...)
    {
      throw;
    }
  }
  
  /* Move assignment operator */
  Chain &Chain::operator=(Chain &&chain)
  {
    try
    {
      if(this == &chain) return *this;
      
      List<Tree<SimpleGroup> >::operator=(std::move(chain));
      
      _depth = std::move(chain._depth);
      
      chain._depth = 0;
      
      return *this;
    } catch (...)
    {
      throw;
    }
  }
  
  /* Depth getter */
  int Chain::depth() const
  {
    return _depth;
  }
  
  /* Calculates the depth of the chain */
  int Chain::calculateDepth()
  {
    try
    {
      int maxdepth = 0;
      for(Chain::const_iterator it = this->begin(); it != this->end(); it++)
        if(it->depth() > maxdepth) 
          maxdepth = it->depth();
    
      _depth = maxdepth;
      return maxdepth;
    } catch (...)
    {
      throw;
    }
  }
  
  /* Returns the objects in level i */
  List<SimpleGroup> Chain::getLevel(int i) const
  {
    try
    {
      List<SimpleGroup> level;
      List<std::string> labels;
      for(int j=0; j < nterms(); j++) {
        if(!i) 
        {
          if(labels.Index(GetObject(j).label()) == -1) 
          {
            labels.AddTerm(GetObject(j).label());
            level.AddTerm(GetObject(j).Object());
          }
        } else
        {
          Chain chain(GetObject(j).Branches());
          List<std::string> sublabels = chain.getLabels(i-1);
          List<SimpleGroup> sublevel = chain.getLevel(i-1);
          for(int k=0; k<sublabels.nterms(); k++)
          {
            if(labels.Index(sublabels.GetObject(k)) == -1)
            {
              labels.AddTerm(sublabels.GetObject(k));
              level.AddTerm(sublevel.GetObject(k));
            }
          }
        }
      }
            
      return level;
    } catch (...)
    {
      throw;
    }
  }
  
  /* Returns the labels in level i */
  List<std::string> Chain::getLabels(int i) const
  {
    try
    {
      List<std::string> labels;
      for(int j=0; j < nterms(); j++)
      {
        if(!i)
          labels.AddTerm(GetObject(j).label());
        else
        {
          Chain chain(GetObject(j).Branches());
          labels.AppendList(chain.getLabels(i-1));
        }
      }
      
      labels.EliminateRepeated();
      
      return labels;
    } catch (...)
    {
      throw;
    }
  }

  /* Extracts the subgroups form the chain */
  List<SubGroup> Chain::extractSubgroups() const
  {
    try {
      int nsubgroups = depth();
      List<SubGroup> Subgroups;
      std::stringstream supgroup;

      for(int i=0; i<nsubgroups; i++)
      {
        List<SimpleGroup> level = getLevel(i);
        List<std::string> labels = getLabels(i);
        
        std::stringstream subgroup;
        for(int j=0; j<level.nterms()-1; j++)
          subgroup << level.GetObject(j).id() << "(" << labels.GetObject(j) << ")" << "x";
        subgroup << level.GetObject(-1).id() << "(" << labels.GetObject(-1) << ")";
        
        if(i)
        {
          subgroup << "[" << supgroup.str() << "]";
          SubGroup group(subgroup.str());
          group.Order();
          Subgroups.AddTerm(group);
        } else
        {
          SubGroup group (LieGroup(subgroup.str()),LieGroup(subgroup.str()));
          group.setLabels(labels);
          group.Order();
          Subgroups.AddTerm(group);
        }
        
        supgroup.str(std::string());
        for(int j=0; j<level.nterms()-1; j++)
          supgroup << level.GetObject(j).id() << "x";
        supgroup << level.GetObject(-1).id();
        
      }
      return Subgroups; 
      
    } catch (...)
    {
      throw;
    }
  }

  /* Parses a json object into the attributes of the class */
  void Chain::ParseJSON(const JSONNode & json)
  {
    List<Tree<SimpleGroup> >::ParseJSON(json);
    
    _depth = calculateDepth();
  }
}
