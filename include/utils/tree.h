/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
* \file
* tree.h
*
* \author
* T. Gonzalo (t.e.gonzalo@fys.uio.no)
*
* \date
* 05/06/2014
*/

#ifndef __TREES_H
#define __TREES_H

#include <cmath>
#include <iostream>
#include <sstream>
#include "linkedlists.h"
#include "libjson.h"

/***************************/
/* Class Tree declarations */
/***************************/

namespace Tomb
{
  template <class TYPE> class Tree
  {
    protected:
      TYPE _Object;
      std::string _label;
      List<Tree<TYPE> > _Branches;
      int _nbranches;
      int _nleaves;
      int _depth;
      int _level;
      
    public:
      Tree();
      Tree(const TYPE &);
      Tree(const JSONNode &);
      Tree(const Tree<TYPE> &);
      Tree(Tree<TYPE> &&);
      ~Tree();
      Tree<TYPE> &operator=(const Tree<TYPE> &);
      Tree<TYPE> &operator=(Tree<TYPE> &&);
      
      TYPE Object() const;
      std::string label() const;
      std::string setLabel(std::string);
      void clearLabel();
      List<Tree<TYPE> > Branches() const;
      Tree Branch(int) const;
      List<Tree<TYPE> > Leaves() const;
      Tree Leaf(int) const;
      List<TYPE> Level(int) const;
      int nbranches() const;
      int nleaves() const;
      int depth() const;
      int calculateDepth();
      int level() const;
      int setLevel(int);
      int calculateLevel();
      std::string id() const;
      void AddBranch(const Tree<TYPE> &);
      void InsertBranch(const int, const Tree<TYPE> &);
      void DeleteBranch(const Tree<TYPE> &);
      void DeleteBranch(const int);
      void DeleteBranch(const std::string);
      void AttachToLeaf(const int, const Tree<TYPE> &);
      void AttachToLeaf(const int, const List<Tree<TYPE> > &);
      void DeleteLeaf(const int);
      void ReplaceLeaf(const int, const Tree<TYPE> &);
      void ReplaceLeaf(const int, const List<Tree<TYPE> > &);
      void SwapLeaves(const int, const int);
      void SwapLeafLabels(const int, const int);
      void DeleteLevel(const int);
      bool operator==(const Tree<TYPE> &) const;
      bool operator!=(const Tree<TYPE> &) const;
      JSONNode json(std::string = "") const;
      void ParseJSON(const JSONNode &);
      
  };

  template <class TYPE> std::ostream &operator<<(std::ostream &, const Tree<TYPE> &);
}

/**************************/
/* Class Tree definitions */
/**************************/

namespace Tomb
{
  /* Constructor 0, empty */
  template <class TYPE> Tree<TYPE>::Tree()
  {
    _label = "";
    _nbranches = 0;
    _nleaves = 0;
    _depth = 0;
    _level = 0;
  }

  /* Constructor 1 */
  template <class TYPE> Tree<TYPE>::Tree(const TYPE &Object)
  {
    _Object = Object;
    _label = "";
    _nbranches = 0;
    _nleaves = 1;
    _depth = 1;
    _level = 0;
  }

  /* Constructor 2, with JSON Nodes */
  template <class TYPE> Tree<TYPE>::Tree(const JSONNode &jnode)
  {
     ParseJSON(jnode);
  }

  /* Copy constructor */
  template <class TYPE> Tree<TYPE>::Tree(const Tree<TYPE> &aTree)
  {
    try
    {
      _Object = aTree.Object();
      _label = aTree.label();
      _Branches = aTree.Branches();
      _nbranches = aTree.nbranches();
      _nleaves = aTree.nleaves();
      _depth = aTree.depth();
      _level = aTree.level();
    }
    catch(...) { throw; }
  }

  /* Move constructor */
  template <class TYPE> Tree<TYPE>::Tree(Tree<TYPE> &&aTree) :
    _Object(std::move(aTree._Object)),
    _label(std::move(aTree._label)),
    _nbranches(std::move(aTree._nbranches)),
    _Branches(std::move(aTree._Branches)),
    _nleaves(std::move(aTree._nleaves)),
    _depth(std::move(aTree._depth)),
    _level(std::move(aTree._level))
  {
    try
    {
      aTree._label = "";
      aTree._nbranches = 0;
      aTree._Branches.Clear();
      aTree._nleaves = 0;
      aTree._depth = 0;
      aTree._level = 0;
    }
    catch(...) { throw; }
  }

  /* Destructor */
  template <class TYPE> Tree<TYPE>::~Tree()
  {
    //if(_Object != nullptr) delete _Object;
    _Branches.Clear();
  }


  /* Overloaded = operator */
  template <class TYPE> Tree<TYPE> &Tree<TYPE>::operator=(const Tree<TYPE> &aTree)
  {   
    try
    {
      if(this != &aTree) return *this;
      this->~Tree();
      _Object = aTree.Object();
      _label = aTree.label();
      _Branches = aTree.Branches();
      _nbranches = aTree.nbranches();
      _nleaves = aTree.nleaves();
      _depth = aTree.depth();
      _level = aTree.level();

      return *this;
    }
    catch(...) { throw; }
  }

  /* Move = operator */
  template <class TYPE> Tree<TYPE> &Tree<TYPE>::operator=(Tree<TYPE> &&aTree)
  {   
    try
    {
      if(this == &aTree) return *this;
      
      //this->~Tree();
      _Object = std::move(aTree._Object);
      _label = std::move(aTree._label);
      _nbranches = std::move(aTree._nbranches);
      _Branches.Clear();
      if(_nbranches) _Branches = std::move(aTree._Branches);
      _nleaves = std::move(aTree._nleaves);
      _depth = std::move(aTree._depth);
      _level = std::move(aTree._level);
        
      aTree._label = "";
      aTree._nbranches = 0;
      aTree._Branches.Clear();
      aTree._nleaves = 0;
      aTree._depth = 0;
      aTree._level = 0;
      
    }
    catch(...) { throw; }
  }

  /* Returns the object */
  template <class TYPE> TYPE Tree<TYPE>::Object() const
  {
    return _Object;
  }

  /* Returns the label of the object */
  template <class TYPE> std::string Tree<TYPE>::label() const
  {
    return _label;
  }

  /* Sets the label of the object */
  template<class TYPE> std::string Tree<TYPE>::setLabel(std::string label)
  {
    _label = label;
    return _label;
  }

  /* Clear all labels from the tree */
  template <class TYPE> void Tree<TYPE>::clearLabel()
  {
    try
    {
      
      setLabel("");
      for(int j=0; j<nbranches(); j++)
      {
        Tree<TYPE> branch = Branch(j);
        branch.clearLabel();
        _Branches.DeleteTerm(j);
        _Branches.InsertTerm(j,branch);
      }
          
    }
    catch (...) { throw; }
  }

  /* Returns the list of branches */
  template <class TYPE> List<Tree<TYPE> > Tree<TYPE>::Branches() const
  {
    return _Branches;
  }

  /* Returns the branch i */
  template <class TYPE> Tree<TYPE> Tree<TYPE>::Branch(const int i) const
  {
    return _Branches.GetObject(i);
  }

  /* Retuns the list of leaves */
  template <class TYPE> List<Tree<TYPE> > Tree<TYPE>::Leaves() const
  {   
    try
    {
      if(!nbranches())
        return *this;
      
      List<Tree> Leaves;
      for(int i=0; i<nbranches(); i++)
      {
        if(Branch(i).depth() == 1)
          Leaves.AddTerm(Branch(i));
        else
          Leaves.AppendList(Branch(i).Leaves());
      }
      
      return Leaves;
      
    }
    catch (...) { throw; }
  }

  /* Returns the leaf i */
  template <class TYPE> Tree<TYPE> Tree<TYPE>::Leaf(const int i) const
  {
    try
    {
      return Leaves().GetObject(i);
    }
    catch (...) { throw; }
  }

  /* Returns the list of objects at level i */
  template <class TYPE> List<TYPE> Tree<TYPE>::Level(int i) const
  {
    try
    {
      List<TYPE> Objects;
      
      if(i - level() >= depth())
        throw "Tree::Level::Level does not exist";

      if(i == level())
        return Object();
      
      for(int j=0; j<nbranches(); j++)
        Objects.AppendList(Branch(j).Level(i));
      
      return Objects;
      
    }
    catch (...) { throw; }
  }

  /* Returns the number of branches */
  template <class TYPE> int Tree<TYPE>::nbranches() const
  {
    return _nbranches;
  }

  /* Returns the number of low branches */
  template <class TYPE> int Tree<TYPE>::nleaves() const
  {
    return _nleaves;
  }

  /* Returns the depth of the tree */
  template <class TYPE> int Tree<TYPE>::depth() const
  {
    return _depth;
  }

  /* Calculates the depth of the tree */
  template <class TYPE> int Tree<TYPE>::calculateDepth()
  {   
    try
    {
      int depth = 1;
      for(int i=0; i<Branches().nterms(); i++)
        if(Branches().GetObject(i).depth() + 1 > depth)
          depth = Branches().GetObject(i).depth() + 1;
      return depth;
    }
    catch (...) { throw; }
  }

  /* Teturns the level of the tree */
  template <class TYPE> int Tree<TYPE>::level() const
  {
    return _level;
  }

  /* Sets the level of the tree */
  template <class TYPE> int Tree<TYPE>::setLevel(int level)
  {
    if(level >= 0) 
    {
      _level = level;
      for(int i=0; i<nbranches(); i++)
      {
        Tree<TYPE> branch = Branch(i);
        branch.setLevel(level+1);
        _Branches.DeleteTerm(i);
        _Branches.InsertTerm(i,branch);
      }
    }
    else
      throw "Tree::setLevel::Wrong level";
  }

  /* Adds a branch */
  template <class TYPE> void Tree<TYPE>::AddBranch(const Tree<TYPE> &aTree)
  {
    try
    {
      std::string label = "";
      
      if(aTree.label() == "")
      {
        if(_nbranches > 1)
        {
          label = _Branches.GetObject(_nbranches-1).label();
          label[label.length()-1] ++;
        }
        else if(_nbranches)
        {
          Tree<TYPE> tree = Branch(0);
          label = _label;
          label.append("A");
          tree.setLabel(label);
          _Branches.DeleteTerm(0);
          _Branches.InsertTerm(0,tree);
          label[label.length()-1] ++;
        }
        else
        {
          label = _label;
        }
      }
      else
        label = aTree.label();
      Tree<TYPE> tree = aTree;
      tree.setLabel(label);
      tree.setLevel(level()+1);
      //tree.setParent(this);
      _Branches.AddTerm(tree);
      _nbranches++;
      
      if(_nbranches == 1)
        _nleaves = aTree.nleaves();
      else
        _nleaves += aTree.nleaves();
      
      if(aTree.depth() + 1 > _depth)
        _depth = aTree.depth() + 1;
      
    }
    catch (...) { throw; }	
    
  }

  /* Inserts a branch */
  template <class TYPE> void Tree<TYPE>::InsertBranch(const int i, const Tree<TYPE> &aTree)
  {
    try
    {
      std::string label = "";

      if (aTree.label() == "")
      {
        if(_nbranches > 1/* and i > 0*/)
        {
          label = _Branches.GetObject(i-1).label();
          label[label.length()-1] ++;
        }
        else if(_nbranches/* and i > 0*/)
        {	
          Tree<TYPE> tree = Branch(0);
          label = _label;
          label.append("A");
          tree.setLabel(label);
          _Branches.DeleteTerm(0);
          _Branches.InsertTerm(0,tree);
          label[label.length()-1] ++;
          /*} else if(_nbranches and i == 0) {
          label = _label;
          label[label.length()-1] ++;	*/
        }
        else
          label = _label;
      }
      else
        label = aTree.label();
     
      Tree<TYPE> tree = aTree;
      tree.setLabel(label);
      tree.setLevel(level()+1);
      //tree.setParent(this);
      _Branches.InsertTerm(i,tree);
      _nbranches++;
      
      for(int j=i+1; j<_nbranches; j++)
      {
        Tree<TYPE> tree2 = Branch(j);
        label = _Branches.GetObject(i-1).label();
        label[label.length()-1] ++;
        tree.setLabel(label);
        _Branches.DeleteTerm(j);
        _Branches.InsertTerm(j,tree2);
      }
      
      if(_nbranches == 1)
        _nleaves = aTree.nleaves();
      else
        _nleaves += aTree.nleaves();
      
      if(aTree.depth() + 1 > _depth)
        _depth = aTree.depth() + 1;

    }
    catch (...) { throw; }	
    
  }

  /* Deletes a branch, by branch */
  template <class TYPE> void Tree<TYPE>::DeleteBranch(const Tree<TYPE> &aTree)
  {
    try
    {
      int i = _Branches.Index(aTree);
      _Branches.DeleteTerm(aTree);
      _nbranches--;
      
      if(_nbranches)
        _nleaves -= aTree.nleaves();
      else
        _nleaves = 1;
      
      if(aTree.depth() + 1 == _depth)
        _depth = calculateDepth();
      
  /*		if(nbranches() == 1) {
        Tree<TYPE> branch = Branch(0);
        std::string label = branch.label();
        label.pop_back();
        branch.setLabel(label);
        _Branches.DeleteTerm(0);
        _Branches.AddTerm(branch);
      } else {
        for(int j=i; j<_Branches.nterms(); j++) {
          Tree<TYPE> branch = Branch(j);
          std::string label = branch.label();
          label[label.length()-1] --;
          branch.setLabel(label);
          _Branches.DeleteTerm(j);
          _Branches.InsertTerm(j,branch);
        }
      }
  */		
    }
    catch(...) { throw; }
  }

  /* Deletes a branch, by index */
  template <class TYPE> void Tree<TYPE>::DeleteBranch(const int i)
  {
    try
    {
      _nbranches--;
      if(_nbranches)
        _nleaves -= Branch(i).nleaves();
      else
        _nleaves = 1;

      _Branches.DeleteTerm(i);
      _depth = calculateDepth();
  /*		
      if(nbranches() == 1) {
        Tree<TYPE> branch = Branch(0);
        std::string label = branch.label();
        label.pop_back();
        branch.setLabel(label);
        _Branches.DeleteTerm(0);
        _Branches.AddTerm(branch);
      } else {
        for(int j=i; j<_Branches.nterms(); j++) {
          Tree<TYPE> branch = Branch(j);
          std::string label = branch.label();
          label[label.length()-1] --;
          branch.setLabel(label);
          _Branches.DeleteTerm(j);
          _Branches.InsertTerm(j,branch);
        }
      }
  */
    }
    catch(...) { throw; }
  }

  /* Deletes a branch, by label */
  template <class TYPE> void Tree<TYPE>::DeleteBranch(const std::string label)
  {
    try
    {
      for(int i=0; i<_nbranches; i++)
      {
        if(Branch(i).label() == label)
        {
          _nbranches--;
          if(_nbranches)
            _nleaves -= Branch(i).nleaves();
          else
            _nleaves = 1;

          _Branches.DeleteTerm(i);
          _depth = calculateDepth();
        }
      }
    }
    catch (...) { throw; }
  }

  /* Attaches a tree to a leaf */
  template <class TYPE> void Tree<TYPE>::AttachToLeaf(const int i, const Tree<TYPE> &aTree)
  {
    try
    {
      int count = 0;

      if(i >= nleaves() or (!nbranches() and i != 0))
        throw "Tree::AttachToLeaf::Could not add the leaf";

      if(!nbranches() and i==0)
      {
        std::string label = this->label();
        Tree<TYPE> tree = aTree;
        tree.setLabel(label);
        AddBranch(tree);
        return ;
      }
      for(int j=0; j<nbranches() and count <= i; j++)
      {
        if(Branch(j).nleaves() > i - count)
        {
          Tree<TYPE> branch = Branch(j);
          branch.AttachToLeaf(i - count, aTree);
          DeleteBranch(j);
          InsertBranch(j,branch);
        }
        count += Branch(j).nleaves();
      }
    }
    catch (...) { throw; }
  }

  /* Attaches a list of branches to a leaf */
  template <class TYPE> void Tree<TYPE>::AttachToLeaf(const int i, const List<Tree<TYPE> > &branches)
  {
    try
    {
      int count = 0;
      if(i >= nleaves() or (!nbranches() and i != 0))
        throw "Tree::AttachToLeaf::Could not add the leaf";
      
      if(branches.nterms() == 1)
      {
        AttachToLeaf(i, branches.GetObject(0));
        return ;
      }

      if(!nbranches() and i==0)
      {
        std::string label = this->label();
        label.append("A");
        for(int k=0; k<branches.nterms(); k++)
        {
          Tree<TYPE> tree = branches.GetObject(k);
          tree.setLabel(label);
          AddBranch(tree);
          label[label.length()-1]++;
        }
        return ;
      } 
      for(int j=0; j<nbranches() and count <= i; j++)
      {
        if(Branch(j).nleaves() > i - count)
        {
          Tree<TYPE> branch = Branch(j);
          branch.AttachToLeaf(i - count, branches);
          DeleteBranch(j);
          InsertBranch(j,branch);
        }
        count += Branch(j).nleaves();
      }

    }
    catch (...) { throw; }
  }

  /* Deletes a leaf from the lowest level */
  template <class TYPE> void Tree<TYPE>::DeleteLeaf(const int i)
  {
    try
    {
      int count = 0;
      if(i >= nleaves() or !nbranches())
        throw "Tree::DeleteLeaf::Could not delete the leaf";
      for(int j=0; j<nbranches(); j++)
      {
        if(Branch(j).depth() > 1)
        {
          if(Branch(j).nleaves() > i - count)
          {
            Tree<TYPE> branch = Branch(j);
            int nleaves = branch.nleaves();
            branch.DeleteLeaf(i - count);
            DeleteBranch(j);
            InsertBranch(j,branch);
          }
          count += Branch(j).nleaves();
        }
        else
        {
          if(i==count)
          {
            DeleteBranch(j);
            j--;
          }
          count++;
        }
      }	
      
    }
    catch (...) { throw; }
  }

  /* Replaces a leaf with a tree */
  template <class TYPE> void Tree<TYPE>::ReplaceLeaf(const int i, const Tree<TYPE> &tree)
  {
    try
    {
      int count = 0;
      if(i >= nleaves() or !nbranches())
        throw "Tree::ReplaceLeaf::Could not replace the leaf";

      for(int j=0; j<nbranches(); j++)
      {
        if(Branch(j).depth() > 1)
        {
          if(Branch(j).nleaves() > i - count)
          {
            Tree<TYPE> branch = Branch(j);
            int nleaves = branch.nleaves();
            branch.ReplaceLeaf(i - count, tree);
            DeleteBranch(j);
            InsertBranch(j,branch);
          }
          count += Branch(j).nleaves();
        }
        else
        {
          if(i==count)
          {
            std::string label = Branch(j).label();
            DeleteBranch(j);
            Tree<TYPE> tree2 = tree;
            tree2.setLabel(label);
            InsertBranch(j, tree2);
          }
          count++;
        }
      }	
      
    }
    catch (...) { throw; }
  }

  /* Replaces a leaf with list of branches */
  template <class TYPE> void Tree<TYPE>::ReplaceLeaf(const int i, const List<Tree> &branches)
  {
    try
    {
      int count = 0;
      if(i >= nleaves() or !nbranches())
        throw "Tree::ReplaceLeaf::Could not replace the leaf";
      
      if(branches.nterms())
      {
        ReplaceLeaf(i, branches.GetObject(0));
        return ;
      }

      for(int j=0; j<nbranches(); j++)
      {
        if(Branch(j).depth() > 1)
        {
          if(Branch(j).nleaves() > i - count)
          {
            Tree<TYPE> branch = Branch(j);
            branch.ReplaceLeaf(i - count, branches);
            DeleteBranch(j);
            InsertBranch(j,branch);
          }
          count += Branch(j).nleaves();
        }
        else
        {
          if(i==count)
          {
            DeleteBranch(j);
            for(int k=0; k<branches.nterms(); k++)
              InsertBranch(j, branches.GetObject(k));
          }
          count++;
        }
      }	
      
    
    }
    catch (...) { throw; }
  }

  /* Swaps two leaves */
  template <class TYPE> void Tree<TYPE>::SwapLeaves(const int i1, const int i2)
  {
    try
    {
      if(i1>=nleaves()  or i2>=nleaves())
        throw "Tree::SwapLeaves::Could not swap leaves";

      int count = 0;
      int branch1 = -1, branch2 = -1;
      for(int j=0; j<nbranches(); j++)
      {
        if(Branch(j).nleaves() > i1 - count and Branch(j).nleaves() > i2 - count and i1 >= count and i2 >= count)
        {
          Tree<TYPE> branch = Branch(j);
          branch.SwapLeaves(i1 - count, i2 - count);
          DeleteBranch(j);
          InsertBranch(j, branch);
          branch1 = branch2 = 0;
        }
        else if(Branch(j).nleaves() > i1 - count and i1 >= count)
          branch1 = j;
        else if(Branch(j).nleaves() > i2 - count and i2 >= count)
          branch2 = j;	
        count += Branch(j).nleaves();
      }
      
      if(branch1 == -1 or branch2 == -1)
        throw "Tree::SwapLeaves::Error while swapping";
      
      if(branch1 != branch2)
        _Branches.SwapTerms(branch1, branch2);
      
    }
    catch (...) { throw; }
  }

  /* Swaps the labels of two leaves */
  template <class TYPE> void Tree<TYPE>::SwapLeafLabels(const int i1, const int i2)
  {   
    try
    {
      if(i1>=nleaves()  or i2>=nleaves())
        throw "Tree::SwapLeafLabels::Could not swap leaves";

      int count = 0;
      int branch1 = -1, branch2 = -1;
      for(int j=0; j<nbranches(); j++)
      {
        if(Branch(j).nleaves() > i1 - count and Branch(j).nleaves() > i2 - count and i1 >= count and i2 >= count)
        {
          Tree<TYPE> branch = Branch(j);
          branch.SwapLeafLabels(i1 - count, i2 - count);
          DeleteBranch(j);
          InsertBranch(j, branch);
          branch1 = branch2 = 0;
        }
        else if(Branch(j).nleaves() > i1 - count and i1 >= count)
          branch1 = j;
        else if(Branch(j).nleaves() > i2 - count and i2 >= count)
          branch2 = j;
        count += Branch(j).nleaves();
      }

      if(branch1 == -1 or branch2 == -1)
        throw "Tree::SwapLeafLabels::Error while swapping";

      if(branch1 != branch2)
      {
        Tree<TYPE> tree1 = _Branches.GetObject(branch1);
        Tree<TYPE> tree2 = _Branches.GetObject(branch2);
        tree1.setLabel(tree2.label());
        tree2.setLabel(_Branches.GetObject(branch1).label());
        _Branches.DeleteTerm(branch1);
        _Branches.InsertTerm(branch1,tree1);
        _Branches.DeleteTerm(branch2);
        _Branches.InsertTerm(branch2,tree2);
      }

    }
    catch (...) { throw; }
  }

  /* Deletes level i */
  template <class TYPE> void Tree<TYPE>::DeleteLevel(const int i)
  {
    try
    {
      
      if(i == 0)
        throw "Tree::DeleteLevel::Can't delete the first level";
      
      if(i == level()+1)
      {
        int branches = nbranches();
        int count = 0;
        for(int j=0; j<branches; j++)
        {
          Tree<TYPE> tree = Branch(0);
          tree.setLevel(tree.level()+1);
          DeleteBranch(0);
          for(int k=0; k<tree.nbranches(); k++)
          {
            AddBranch(tree.Branch(k));
            count ++;
          }
        }
      }
      else
      {
        for(int j=0; j<nbranches(); j++)
        {
          Tree<TYPE> tree = Branch(j);
          tree.DeleteLevel(i);
          DeleteBranch(j);
          InsertBranch(j, tree);
        }
      }
      
    }
    catch (...) { throw; }
  }

  /* Overloaded == operator */
  template <class TYPE> bool Tree<TYPE>::operator==(const Tree<TYPE> &aTree) const
  {
    if(_Object == aTree.Object() and this->Branches() == aTree.Branches())
      return true;
    return false;
  }

  /* Overloaded != operator */
  template <class TYPE> bool Tree<TYPE>::operator!=(const Tree<TYPE> &aTree) const
  {
    if(_Object != aTree.Object() or this->Branches() != aTree.Branches())
      return true;
    return false;
  }

  /* Prints to json format */
  template <class TYPE> JSONNode Tree<TYPE>::json(std::string name) const
  {   
    try
    {   
      JSONNode tree;
      if(name != "" and name != "id")
        tree.set_name(name);
      
      JSONNode node = Branches().json(name);
      stringstream labelstream;
      labelstream << _Object;
      string label = labelstream.str();
      if(this->label() != "")
      {
        label.append("(");
        label.append(this->label());
        label.append(")");
      }
      node.set_name(label);
      
      tree.push_back(node);
      
      return tree;
    
    }
    catch (...) { throw; }
  }

  /* Parses a json object into the attributes of the class */
  template <class TYPE> void Tree<TYPE>::ParseJSON(const JSONNode &n)
  {   
    try
    {
      
      JSONNode::const_iterator i = n.begin();
      

      std::stringstream ss;
      std::string str1,str2;
      
      ss << i->name();
      getline(ss,str1,'(');
      
      _Object = str1;
      
      getline(ss,str2,')');
      
      if(str1 == str2)
        _label = "A";
      else
        _label = str2;
      _Branches.ParseJSON(*i);
      _nbranches = _Branches.nterms();
      _nleaves = 0;
      if(_nbranches)
      {
      //	std::string label = "A";
        for(int j=0; j<_nbranches; j++)
        {
          Tree<TYPE> branch = Branch(j);
          branch.setLevel(branch.level()+1);
          //if(_nbranches > 1) {
          //	branch.setLabel(label);
          //	label[label.length()-1]++;
          //}
          _Branches.DeleteTerm(j);
          _Branches.InsertTerm(j,branch);
          _nleaves += _Branches.GetObject(j).nleaves();
        }
      }
      else
        _nleaves = 1;
      _depth = calculateDepth();
      _level = 0;
      
      
    }
    catch (...) { throw; }
  }

  /* Overloaded << operator */
  template <class TYPE> std::ostream &operator<<(std::ostream &stream, const Tree<TYPE> &Tree)
  {
    stream << Tree.json().write_formatted() << std::endl;
    return stream;
  }
}

#endif /* __TREES_H */
