/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * subgroup.cc
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 15/02/2012
 */

#include "subgroup.h"

/******************************/
/* Class SubGroup definitions */
/******************************/

namespace Tomb
{
  // Member functions

  /* Constructor, empty */
  SubGroup::SubGroup() : LieGroup()
  {

  }

  /* Constructor 1 */
  SubGroup::SubGroup(LieGroup &SuperGroup) : LieGroup()
  {
    try
    {
      char label = 'A';
      _SuperGroup = &SuperGroup;
      for(int i=0; i<SuperGroup.nterms(); i++)
      {
        Tree<string> tree(SuperGroup.GetObject(i).id());
        tree.setLabel(string(1,label));
        label++;
        _SuperGroups.AddTerm(tree);
        
      }
      _superRank = SuperGroup.rank();
      
      _maximal = false;
      _regular = false;
      _special = false;
    }
    catch (...) { throw; }
  }

  /* Constructor 2 */
  SubGroup::SubGroup(SimpleGroup &SuperGroup) : LieGroup()
  {
    try
    {
      _SuperGroup = DB<LieGroup>().at(SuperGroup.id());
      Tree<string> tree(SuperGroup.id());
      tree.setLabel("");
      _SuperGroups.AddTerm(tree);
      _superRank = SuperGroup.rank();

      _maximal = false;
      _regular = false;
      _special = false;
    }
    catch (...) { throw; }
  }

  /* Constructor 3, with strings */
  SubGroup::SubGroup(const std::string id)
  {
    try
    {
      init(id);
    }
    catch (...) { throw; }
  }

  /* Constructor 3.5, with strings and mixings */
  SubGroup::SubGroup(const std::string id, Matrix<double> Mixing)
  {   
    try
    {   
      // If there is no mixing initialise as usual
      if(Mixing.cols() <= 1)
      {
        init(id);
        return ;
      }
      
      std::stringstream ss(id);
      std::string SGstr, Gstr;

      getline(ss, Gstr, '[');
      getline(ss, SGstr, ']');

      _SuperGroup = DB<LieGroup>().get(SGstr);
    
      std::stringstream ss2(Gstr);
      std::string Gstr2;
    
      List<std::string> ids;
      List<List<double> > mixings;
      List<double> R;
      List<std::string> mixlabels;
      
      int semisimplerank = 0;
    
      // Extract the list of subgoups to parse
      int count = 0, abelians = 0;
      while(getline(ss2, Gstr2,  'x'))
      {
        std::stringstream ss3(Gstr2);
        std::string Gstr3;
        getline(ss3, Gstr3, '(');
        if(Gstr3 == "U1")
        {
          std::string labels;
          getline(ss3, labels, ')');
          for(int i=0; i<ids.nterms(); i++)
          {
            std::stringstream mixss;
            mixss << ids.GetObject(i);
            ids.DeleteTerm(i);
            R = mixings.GetObject(i);
            mixings.DeleteTerm(i);
            std::stringstream ss4(labels);
            std::string label;
            int count = 0;
            while(getline(ss4, label, '+'))
            {
              std::stringstream mixss2;
              mixss2 << mixss.str();
              mixss2 << "xU1(" << label << ")";
              ids.InsertTerm(i,mixss2.str());
              List<double> R2(R);
              R2.AddTerm(Mixing[abelians][count]);
              mixings.InsertTerm(i,R2);
              i++;
              count++;
            }
            abelians ++ ;
          }
          mixlabels.AddTerm(labels);			
        } 
        else
        {
          semisimplerank += SimpleGroup(Gstr3).rank();
          std::stringstream ss4;
          ss4 << Gstr3;
          getline(ss3, Gstr3, ')');
          ss4 << '(' << Gstr3 << ')';
          if(ids.nterms() == 0)
          {
            ids.AddTerm(ss4.str());
            R.AddTerm(1);
            mixings.AddTerm(R);
          }
          else
          {
            for(int i=0; i<ids.nterms(); i++)
            {
              std::stringstream mixss;
              mixss << ids.GetObject(i);
              mixss << "x" << ss4.str();
              ids.DeleteTerm(i);
              ids.InsertTerm(i,mixss.str());
              R = mixings.GetObject(i);
              R.AddTerm(1);
              mixings.DeleteTerm(i);
              mixings.InsertTerm(i,R);   
            }
          }
          mixlabels.AddTerm(Gstr3);
        }
      }
    
      // Finish the ids by adding the supergroup
      for(int i=0; i<ids.nterms(); i++)
      {
        std::stringstream idsstream;
        idsstream << ids.GetObject(i) << '[' << SGstr << ']';
        ids.DeleteTerm(i);
        ids.InsertTerm(i, idsstream.str());
      }
      
      // If there is no U(1) mixing, initialise as usual
      if(ids.nterms() == 1)
      {
        init(id);
        return ;
      }
    
      // Check wether the mixing matrix has the proper shape for all mixings
      int size = 0;
      /*for(int i=0; i<Mixing.rows(); i++) {
        for(int j=0; j<Mixing.cols(); j++) {
          if(Mixing[i][j] != 0) size++;
        }
      }*/
      size = Mixing.cols();
      if(size != ids.nterms())
        throw "SubGroup::SubGroup::Mixing vector of the wrong size";
    
      // Create the subgroup by copying the first on the list of labels
      init(ids.GetObject(0));
    
      // Change the labels
      _labels = mixlabels;
      
      // Change the supergroups
      // Don't do it now, cause it's a pain, maybe we won't need it
          
      // Change the projection matrix
      _Projection = 0;
      for(int i=0; i<ids.nterms(); i++)
      {
        SubGroup Subgroup(ids.GetObject(i));
        Matrix<double> MixMatrix(_Projection.rows());
        MixMatrix = MixMatrix.Identity(_Projection.rows());
        for(int j=0; j<_Projection.rows()-semisimplerank; j++)
        {
          MixMatrix[_Projection.rows()-j-1][_Projection.rows()-j-1] = ids.nterms()*mixings.GetObject(i).GetObject(-j-1);
        }
        _Projection += MixMatrix*Subgroup.Projection()/ids.nterms();
      }
          
    
    }
    catch (...) { throw; }
  }

  /* Constructor 4, with json nodes */
  SubGroup::SubGroup(const JSONNode &n)
  {
    try
    {
      if(n.as_string() != "")
        SubGroup(n.as_string());
      else
        ParseJSON(n);
    }
    catch (...) { throw; }
  }

  /* Copy constructor 0 */
  SubGroup::SubGroup(const SubGroup &Group) : LieGroup(Group)
  {
    try
    {
      _SuperGroup = DB<LieGroup>().at(Group.SuperGroup().id());
      _SuperGroups = Group.SuperGroups();
      _Projection = Matrix<double>(Group.Projection());
      _labels = Group.labels();
      _superRank = Group.superRank();
      _maximal = Group.maximal();
      _regular = Group.regular();
      _special = Group.special();
    }
    catch (...) { throw; }
  }

  /* Copy constructor 1 */
  SubGroup::SubGroup(LieGroup &Group, LieGroup &SuperGroup) : LieGroup(Group)
  {
    try
    {   
      if(Group != SuperGroup)
        throw "SubGroup::SubGroup::Nothing to do here";
      else
      {
        _SuperGroup = &SuperGroup;
        char label = 'A';
        for(int i=0; i<Group.ngroups(); i++)
        {
          Tree<string> tree(SuperGroup.GetObject(i).id());
          tree.setLabel(string(1,label));
          label++;
          tree.AddBranch(Tree<string>(Group.GetObject(i).id()));
          _SuperGroups.AddTerm(tree);
        }
      }
      _superRank = SuperGroup.rank();
      
      _Projection = Matrix<double>(_rank, _superRank);
      _Projection = 1;
      
      std::string label = "A";
      _labels.AddTerm(label);
      for(int i=1; i<Group.nterms(); i++)
      {
        label[label.length()-1] ++;
        _labels.AddTerm(label);
      }
      
      if(_rank == _superRank)
        _maximal = true;
      else
        _maximal = false;

      _regular = true;
      _special = false;

      // TODO: Commented out because it causes problems, not sure if required or not
      //FinishSubgroup();
      
    }
    catch (...) { throw; }
  }

  /* Copy constructor 2 */
  SubGroup::SubGroup(LieGroup &Group, SimpleGroup &SuperGroup) : LieGroup(Group)
  {
    try
    {
      _SuperGroup = DB<LieGroup>().at(SuperGroup.id());
      Tree<string> tree(SuperGroup.id());
      tree.setLabel("A");
      for(int j=0; j<Group.nterms(); j++)
        tree.AddBranch(Tree<string>(Group.GetObject(j).id()));
      _SuperGroups.AddTerm(tree);
      _superRank = SuperGroup.rank();
      
      _Projection = Matrix<double>(_rank, _superRank);
      _Projection = 1;
      
      std::string label = "A";
      _labels.AddTerm(label);
      for(int i=1; i<Group.nterms(); i++)
      {
        label[label.length()-1] ++;
        _labels.AddTerm(label);
      }
      
      if(_rank == _superRank)
        _maximal = true;
      else
        _maximal = false;

      _regular = true;
      _special = false;

      FinishSubgroup();
      
    }
    catch (...) { throw; }
  }

  /* Copy constructor 4 */
  SubGroup::SubGroup(SimpleGroup &Group, SimpleGroup &SuperGroup) : LieGroup(Group)
  {
    try
    {
      _SuperGroup = DB<LieGroup>().at(SuperGroup.id());
      Tree<string> tree(SuperGroup.id());
      tree.setLabel("A");
      if(Group != SuperGroup)
        tree.AddBranch(Tree<string>(Group.id()));
      _SuperGroups.AddTerm(tree);
      _superRank = SuperGroup.rank();
      
      _Projection = Matrix<double>(_rank, _superRank);
      _Projection = 1;
      
      _labels.AddTerm("A");
          
      if(_rank == _superRank)
        _maximal = true;
      else
        _maximal = false;

      _regular = true;
      _special = false;

      FinishSubgroup();
      
    }
    catch (...) { throw; }
  }

  /* Copy constructor 6 */
  SubGroup::SubGroup(SubGroup &Group, SimpleGroup &SuperGroup) : LieGroup(Group)
  {
    try
    {
      _SuperGroup = DB<LieGroup>().at(SuperGroup.id());
      if(Group._SuperGroups.nterms() != 1 or Group.SuperGroup() != SuperGroup)
      {
        Tree<string> tree(SuperGroup.id());
        tree.setLabel("A");
        for(int i=0; i<Group._SuperGroups.nterms(); i++)
          tree.AddBranch(Group._SuperGroups.GetObject(i));
        _SuperGroups.AddTerm(tree);
        _superRank = SuperGroup.rank();
      } 
      else
        _SuperGroups = Group._SuperGroups;
      
      _Projection = Matrix<double>(Group.Projection());
      
      _labels = Group.labels();
      
      
      /*if(!SuperGroups().nterms() or Group.SuperGroup() < SuperGroup)
      {
        _SuperGroups.AddTerm(LieGroup(SuperGroup));
        if(SuperGroup.rank() > Group.SuperGroup().rank())
          _Projection.Append(Matrix<double>(rank(),SuperGroup.rank()-Group.SuperGroup().rank()),"RIGHT");
      }*/
      
      int depth = 0;
      for(int i=0; i<_SuperGroups.nterms(); i++)
        if(depth < _SuperGroups.GetObject(i).depth())
          depth = _SuperGroups.GetObject(i).depth();

      if(_rank == _superRank and depth == 2)
        _maximal = true;
      else
        _maximal = false;

      _regular = Group.regular();
      _special = Group.special();

      FinishSubgroup();
      
    }
    catch (...) { throw; }
  }

  /* Copy constructor 7 */
  SubGroup::SubGroup(SubGroup &Group, SubGroup &SuperGroup) : LieGroup(Group)
  {
    try
    {
      _SuperGroup = DB<LieGroup>().at(SuperGroup.SuperGroup().id());
      _SuperGroups = SuperGroup._SuperGroups;
      _superRank = SuperGroup.rank(); 
      
      bool replace = true;
      for(int i=0; i<Group._SuperGroups.nterms(); i++)
        if(Group._SuperGroups.GetObject(i).nbranches())
          if(Group._SuperGroups.GetObject(i).Level(0) != Group._SuperGroups.GetObject(i).Level(1))
            replace = false;
      
      int count = 0;
      for(int i=0; i<_SuperGroups.nterms(); i++)
      {
        Tree<string> tree = _SuperGroups.GetObject(i);
        for(int j=0; j<tree.nleaves(); j++)
        {
          if(tree.Leaf(j).level() > 0)
          {
            if(Group.nterms() != SuperGroup.nterms() and replace)
            {
              if(Group._SuperGroups.GetObject(count).nbranches())
              {
                tree.ReplaceLeaf(j, Group._SuperGroups.GetObject(count).Branches());
                j += Group._SuperGroups.GetObject(count).nleaves() - 1;
              }
              else
              {
                int nleaves = tree.nleaves();
                do
                {
                  tree.DeleteLeaf(j);
                  if(nleaves > tree.nleaves()) j--;
                }
                while (tree.nleaves() == nleaves and tree.Leaf(j).level() > 0);
              }
            }
            else
            {
              if(Group._SuperGroups.GetObject(count).nbranches())
              {
                tree.AttachToLeaf(j, Group._SuperGroups.GetObject(count).Branches());
                j += Group._SuperGroups.GetObject(count).nleaves() - 1;
              }
              else
              {
                int nleaves = tree.nleaves();
                do
                {
                  tree.DeleteLeaf(j);
                  if(nleaves > tree.nleaves()) j--;
                }
                while (tree.nleaves() == nleaves and tree.Leaf(j).level() > 0);
              }
            }	
            count ++;
          }
        }
        if(tree != _SuperGroups.GetObject(i))
        {
          _SuperGroups.DeleteTerm(i);
          _SuperGroups.InsertTerm(i,tree);
        }
      }	
      
      _Projection = Matrix<double>(Group.Projection()*SuperGroup.Projection());

      int depth = 0;
      for(int i=0; i<_SuperGroups.nterms(); i++)
        if(depth < _SuperGroups.GetObject(i).depth())
          depth = _SuperGroups.GetObject(i).depth();

      if(depth == 2)
        _maximal = true;
      else
        _maximal = false;

      _regular = Group.regular();
      _special = Group.special();
      
      if(Group._SuperGroups.nterms() > 1)
      {
        for(int i=0; i<Group.nterms(); i++)
        {
          char where = Group.labels().GetObject(i)[0];
          where -= 'A';
          string label = SuperGroup.labels().GetObject(int(where));
          label.append(Group.labels().GetObject(i).substr(1));
          _labels.AddTerm(label);
        }
      }
      else
        _labels = Group.labels();
      
      FinishSubgroup(); 
    }
    catch (...) { throw; }
  }

  /* Move constructor */
  SubGroup::SubGroup(SubGroup &&Group) : 
    LieGroup(std::move(Group)),
    _SuperGroup(std::move(Group._SuperGroup)),
    _SuperGroups(std::move(Group._SuperGroups)),
    _Projection(std::move(Group._Projection)),
    _labels(std::move(Group._labels)),
    _superRank(std::move(Group._superRank)),
    _maximal(std::move(Group._maximal)),
    _regular(std::move(Group._regular)),
    _special(std::move(Group._special))
  {
    try
    {
      Group._SuperGroup = NULL;
      Group._SuperGroups.Clear();
      Group._Projection = Matrix<double>();
      Group._labels.Clear();
      Group._superRank = 0;
      Group._maximal = false;
      Group._regular = false;;
      Group._special = false;
      
    }
    catch (...) { throw; }
  }

  /* Destructor */
  SubGroup::~SubGroup()
  {

  }

  /* Assignment operator */
  SubGroup &SubGroup::operator=(const SubGroup &Group)
  {
    try
    {
      if(this == &Group) return *this;
      
      LieGroup::operator=(Group);
      
      _SuperGroup = DB<LieGroup>().at(Group.SuperGroup().id());
      _SuperGroups = Group._SuperGroups;
      _Projection = Matrix<double>(Group.Projection());
      _labels = Group.labels();
      _superRank = Group.superRank();
      _maximal = Group.maximal();
      _regular = Group.regular();
      _special = Group.special();
      
      return *this;
      
    }
    catch (...) { throw; }
    
  }

  /* Move assignment operator */
  SubGroup &SubGroup::operator=(SubGroup &&Group)
  {
    try
    {
      if(this == &Group) return *this;
      
      LieGroup::operator=(std::move(Group));
      
      _SuperGroup = std::move(Group._SuperGroup);
      _SuperGroups = std::move(Group._SuperGroups);
      _Projection = std::move(Group._Projection);
      _labels = std::move(Group._labels);
      _superRank = std::move(Group._superRank);
      _maximal = std::move(Group._maximal);
      _regular = std::move(Group._regular);
      _special = std::move(Group._special);
      
      Group._SuperGroup = NULL;
      Group._SuperGroups.Clear();
      Group._Projection = Matrix<double>();
      Group._labels.Clear();
      Group._superRank = 0;
      Group._maximal = false;
      Group._regular = false;;
      Group._special = false;
      
      return *this;
      
    }
    catch (...) { throw; }
    
  }

  /* Initialises some variables */
  void SubGroup::init(const std::string Id)
  {   
    try
    {
      
      std::stringstream ss(Id);
      
      std::string SGstr, Gstr;
      
      getline(ss, Gstr, '[');
      getline(ss, SGstr, ']');
      
      if(Strings::split_string(Id,'+').nterms() > 1)
      {
        ss.str("");
        ss << Strings::split_string(Id,'+').GetObject(0) << ")[" << SGstr << "]";
        getline(ss, Gstr, '[');
        getline(ss, SGstr, ']');
      }
      
      std::string id = ss.str();
      _SuperGroup = DB<LieGroup>().get(SGstr);
      std::stringstream ss2(Gstr);
      std::string str,label;
          
//      SubGroup Subgroup(*_SuperGroup);

      char clabel = 'A';
      for(int i=0; i<_SuperGroup->nterms(); i++)
      {
        Tree<string> tree(_SuperGroup->GetObject(i).id());
        tree.setLabel(string(1,clabel));
        clabel++;
        _SuperGroups.AddTerm(tree);

      }

      _superRank = _SuperGroup->rank();

      _maximal = false;
      _regular = false;
      _special = false;


      List<std::string> labels;
          
      while(getline(ss2, str, '('))
      {
        AddTerm(*DB<SimpleGroup>().get(str));
        getline(ss2, label, ')');
        labels.AddTerm(label);
        getline(ss2, str, 'x');
      }
      setLabels(labels);
      Order();
        
      if(Strings::split_string(Id,'+').nterms() > 1)
      {
        std::string label = Strings::split_string(Strings::split_string(Id,'(').GetObject(-1),')').GetObject(0);
        
        List<std::string> oldlabels = this->labels();
        oldlabels.DeleteTerm(-1);
        oldlabels.AddTerm(label);
        setLabels(oldlabels);
      }

      // If the subgroup is already in the database, do nothing
      SubGroup *G = DB<SubGroup>().find(Id);
      if(G != NULL)
      {
        *this = *G;
        return ;
      }
        
      DB<SubGroup>().set(Id, this);
      
    }
    catch (...) { throw; }
  }


  /* Returns the list of Supergroups */
  List<Tree<string> > SubGroup::SuperGroups() const
  {
    SubGroup *G = DB<SubGroup>().find(id());
    if(G != NULL)
      return G->_SuperGroups;
    return _SuperGroups;
  }

  /* Returns the hightest Supergroup */
  LieGroup SubGroup::SuperGroup() const
  {
    return *_SuperGroup;
  }

  /* Returns the Supergroup at level i */
  LieGroup SubGroup::SuperGroup(int i) const
  {
    try
    {
      string Supergroup;
      for(int j=0; j<_SuperGroups.nterms(); j++)
      {
        if(_SuperGroups.GetObject(j).nbranches())
        {
          List<string> level = _SuperGroups.GetObject(j).Level(i);
          for(int k=0; k<level.nterms(); k++)
          {
            Supergroup.append(level.GetObject(k));
            Supergroup.append("x");
          }
        }
      }
      Supergroup.pop_back();
      return *DB<LieGroup>().get(Supergroup);
    }
    catch (...) { throw; }
  }

  /* Returns the Projection matrix */
  Matrix<double> SubGroup::Projection() const
  {	
    return _Projection;
  }

  /* Sets the projection matrix of a subgroup */
  Matrix<double> SubGroup::setProjection(Matrix<double> Proj)
  {
    _Projection = Proj;
    return _Projection;
  }

  /* Returns the labels */
  List<std::string> SubGroup::labels() const
  {
    return _labels;
  }

  /* Set the labels */
  void SubGroup::setLabels(const List<std::string> labels)
  {
    _labels = labels;
  }

  /* Adds a simple group to the Subgroup */
  void SubGroup::AddTerm(const SimpleGroup &Group, int where)
  {
    try
    {

      LieGroup::AddTerm(Group);

      if(Group.rank() == _superRank)
        _maximal = true;

      if(where < _SuperGroups.nterms())
        _SuperGroups.GetObject(0).AddBranch(Tree<string>(Group.id()));
      else
        throw "SubGroup::AddTerm::Element doesn't exist";

      if(!_Projection.rows() and !_Projection.cols())
        _Projection = Matrix<double>(Group.rank(), _superRank);
      else
        _Projection.Append(Matrix<double>(Group.rank(), _superRank));
      
      std::string label = "";
      if(_labels.nterms() > 1)
      {
        label.append(_labels.GetObject(-1));
        label[label.length()-1]++;
      }
      else if(_labels.nterms() == 1)
      {
        _labels.DeleteTerm(0);
        label.append("A");
        _labels.AddTerm(label);
        label[label.length()-1]++;
      }	
      _labels.AddTerm(label);
      
      
    }
    catch (...) { throw; }
  }
      
  /* Adds a liegroup to the Subgroup */
  void SubGroup::AddTerm(const LieGroup &Group, int where)
  {
    try
    {
      for(int i=0; i<Group.ngroups(); i++)
        LieGroup::AddTerm(Group.GetObject(i));
      
      if(where < _SuperGroups.nterms())
      {
        /*Tree<SimpleGroup> Object = _SuperGroups.GetObject(where);
        for(int j=0; j<Group.ngroups(); j++)
        {
          //if(Object.Object() != Group.GetObject(j)) {
            Object.AddBranch(*DB<SimpleGroup>().get(Group.GetObject(j).id()));
          //}
        }
        _SuperGroups.DeleteTerm(where);
        _SuperGroups.InsertTerm(where,Object);*/
        for(int j=0; j<Group.ngroups(); j++)
          _SuperGroups.GetObject(where).AddBranch(Group.GetObject(j).id());
      }
      else
        throw "SubGroup::AddTerm::Element doesn't exist";
      
      if(!_Projection.rows() and !_Projection.cols())
        _Projection = Matrix<double>(_rank, _superRank);
      _Projection.Append(Matrix<double>(_rank, _superRank));
      
    }
    catch (...) { throw; }
  }

  /* Adds a subgroup to the Subgroup */
  void SubGroup::AddTerm(const SubGroup &Group, int where)
  {
    try
    {
      for(int i=0; i<Group.ngroups(); i++)
        LieGroup::AddTerm(Group.GetObject(i));

      if(where < _SuperGroups.nterms())
      {
        /*Tree<SimpleGroup> Object = _SuperGroups.GetObject(where);
        for(int j=0; j<Group.ngroups(); j++)
        {
          //if(Object.Object() != Group.GetObject(j)) {
            Object.AddBranch(*DB<SimpleGroup>().get(Group.GetObject(j).id()));
          //}
        }
        _SuperGroups.DeleteTerm(where);
        _SuperGroups.InsertTerm(where,Object);*/
        for(int j=0; j<Group.ngroups(); j++)
          _SuperGroups.GetObject(where).AddBranch(Group.GetObject(j).id());
      }
      else
        throw "SubGroup::AddTerm::Element doesn't exist";
      if(!_Projection.rows() and !_Projection.cols())
        _Projection = Group.Projection();
      else
      {
        Matrix<double> AuxProjection = Group.Projection();
//TODO: new added bit, maybe not 100% correct
        if(_superRank > Group.rank())
          AuxProjection.Append(Matrix<double>(Group.rank(), _Projection.cols()),"LEFT");
        _Projection.Append(Matrix<double>(_Projection.rows(), Group.SuperGroup().rank()),"RIGHT");
        _Projection.Append(AuxProjection);
      }
      
      std::string label = "";
      if(_labels.nterms()) {
        label = _labels.GetObject(_labels.nterms()-1)[0];
        label[0] ++;
      } else {
        label = "A";
      }
      
      if(Group.nterms() > 1) {
        label.append("A");	
        _labels.AddTerm(label);
        for(int i=1; i<Group.nterms(); i++) {
          label[label.length()-1]++;
          _labels.AddTerm(label);
        }
      } else {
        _labels.AddTerm(label);
      }
      
    }
    catch (...) { throw; }
  }

 /* Deletes a subgroup of the Subgroup */
  void SubGroup::DeleteTerm(int del)
  {
    try
    {
      int i = del;
      if(del < 0)
        i = nterms() + i;
      int rank = 0;
      for(int j=0; j<i; j++)
        rank += GetObject(j).rank();

      for(int j=0; j<GetObject(i).rank(); j++)
        _Projection.DeleteRow(rank);

      int count = 0;
      for(int j=0; j<_SuperGroups.nterms(); j++)
      {
        if(_SuperGroups.GetObject(j).nbranches() <=  i - count)
          count += _SuperGroups.GetObject(j).nbranches();
        else
        {
          /*Tree<SimpleGroup> tree = _SuperGroups.GetObject(j);
          tree.DeleteLeaf(i - count);
          _SuperGroups.DeleteTerm(j);
          _SuperGroups.InsertTerm(j,tree);*/
          _SuperGroups.GetObject(j).DeleteLeaf(i-count);
          count++;
        }
      }
      
      _labels.DeleteTerm(i);
      
      _maximal = false;
      
      LieGroup::DeleteTerm(i);
      
    }
    catch(...) { throw; }
  }

  /* Finish the subgroup by adding it to the database */
  void SubGroup::FinishSubgroup()
  {
    try
    {
      // If the subgroup is already in the database, do nothing
      //SubGroup *G = DB<SubGroup>().find(id());
      //if(G == NULL)
      //  DB<SubGroup>().set(id(), this);
      // Actually always set it to the database, in case it has better info
      DB<SubGroup>().set(id(), this, true);

      // Add the liegroup corresponding to the subgroup to the database
      LieGroup *LG = DB<LieGroup>().find(lg_id());
      if(LG != NULL)
        return ;
      LieGroup::init();
 
    }
    catch (...) { throw; }
  }

  /* Order the groups in the subgroup and the projection matrix */
  void SubGroup::Order()
  {
    try
    { 
      if(nterms() <= 1)
        return ;
      
      int row1 = 0;
      for(int i=0; i<nterms(); i++)
      {
        int rank1 = GetObject(i).rank();
        int toswap = i;
        int row2,auxrow = row1 + rank1;
        for(int j=i+1; j<nterms(); j++)
        {
          // Compare first the elements themselves and if they are equal compare their labels
          if(GetObject(toswap) < GetObject(j))
          {
            toswap = j;
            row2 = auxrow;
          }
          else if (GetObject(toswap) == GetObject(j))
          {
            std::string label1 = _labels.GetObject(toswap);
            std::string label2 = _labels.GetObject(j);
            int k=0;
            while(k < label1.length() and k < label2.length() and label1[k] == label2[k]) k++; 
            if(label1[k] > label2[k])
            {
              toswap = j;
              row2 = auxrow;
            }
          }
          auxrow += GetObject(j).rank();
        }
        
        if(toswap != i)
        {
          int rank2 = GetObject(toswap).rank();
          SwapTerms(i,toswap);
          _Projection.MoveRows(row2, row2+rank2-1, row1);
          _Projection.MoveRows(row1+rank2, row1+rank2+rank1-1, row2+rank2-rank1);
          _Casimir.SwapTerms(i, toswap);
          if(!_maximal or SuperGroup().nterms() > 1)
            _labels.SwapTerms(i, toswap);

          row1 += rank2;
        } 
        else
          row1 += rank1;				
      }
      
      _label = this->Print();	
      
    }
    catch (...) { throw; }
  }

  /* Returns whether the subgroup is a subgroup of another Lie Group */
  bool SubGroup::isSubgroupOf(LieGroup Group) const
  {
    try
    {
      if(this->rank() > Group.rank() or this->dim() > Group.dim())
        return false;
      else
      {
        if(this->abelian())
          return true;       

        if(SuperGroup() == Group && this->Projection() != 0)
          return true;
        
        if(SuperGroup().isSubgroupOf(Group))
          return true;
        
        //This makes calculating subgroups recursive
        LieGroup *G = DB<LieGroup>().find(Group.id());
        if(G != NULL and G->_Subgroups.nterms())
        {
          if(G->_Subgroups.Index(*this) == -1)
            return false;
          else
            return true;
        }
        else
          throw "Subgroup::isSubgroupOf::Unknown Lie group";
      }
      return false;
    }
    catch (...) { throw; }
  }

  /* Returns whether the subgroup is a subgroup of another Lie Group */
  bool SubGroup::isSubgroupOf(SimpleGroup Group) const
  {
    try
    {
      return isSubgroupOf(LieGroup(Group));
    }
    catch (...) { throw; }
  }

  /* Returns the rank of the supergroup */
  int SubGroup::superRank() const
  {
    return _superRank;
  }

  /* Returns whether the subgroup is maximal or not */
  bool SubGroup::maximal() const
  {
    return _maximal;
  }

  /* Sets the subgroup to be maximal */
  bool SubGroup::setMaximal(bool maximal)
  {
    _maximal = maximal;
    return _maximal;
  }

  /* Returns whether the subgroup is regular or not */
  bool SubGroup::regular() const
  {
    return _regular;
  }

  /* Sets the subgroup to be regular */
  bool SubGroup::setRegular(bool regular)
  {
    _regular = regular;
    return _regular;
  }

  /* Returns whether the subgroup is singular or not */
  bool SubGroup::special() const
  {
    return _special;
  }

  /* Sets the subgroup to be special */
  bool SubGroup::setSpecial(bool special)
  {
    _special = special;
    return _special;
  }

  /* Identifier of the Subgroup */
  string SubGroup::id() const
  {   
    if(!nterms())
      return "";
    
    stringstream s;

    for(int i=0; i<nterms()-1; i++)
      s << GetObject(i).id() << "(" << _labels.GetObject(i) << ")" << "x";
    s << GetObject(nterms()-1).id() << "(" << _labels.GetObject(nterms()-1) << ")";
    
    s << "[";
    for(int i = 0; i < _SuperGroups.nterms()-1; i++)
      s << _SuperGroups.GetObject(i).Object() << "x";
    s << _SuperGroups.GetObject(-1).Object() << "]";
    string str = s.str();
    
    return str;
  }

  /* Identifier of the Liegroup */
  string SubGroup::lg_id() const
  {
    return LieGroup::id();
  }

  /* Overloaded == operator */
  bool SubGroup::operator==(const SubGroup &Group) const
  {   
    try
    {
      if(this->ngroups() != Group.ngroups() or this->rank() != Group.rank())
        return false;

      if(this->lg_id() != Group.lg_id())
        return false;
      
      if(maximal() != Group.maximal() or regular() != Group.regular() or special() != Group.special())
        return false;
      
      if(this->Projection() == Group.Projection())
        return true;
      if(_SuperGroup->ngroups() == _ngroups)
      {
        // FIXME: Not working, but would be better to tell subgroups apart
        Rrep GenRep = _SuperGroup->GeneratingRep();
        SubGroup sg1(*this);
        SubGroup sg2(Group);
        if(GenRep.Decompose(sg1) == GenRep.Decompose(sg2))
          return true;
        else
          return false;
      }

      if(id() != Group.id()) return false;
      else return true;
      
      if(labels() == Group.labels())
        return true;
      
      return false;
      
      if(SuperGroups() == Group.SuperGroups())
        return true;
      
      return false;
    }
    catch (...) { throw; }
  }

  /* Overloaded != operator */
  bool SubGroup::operator!=(const SubGroup &Group) const
  {
    try
    {
      return !((*this) == Group);
    }
    catch (...) { throw; }
  }
  
  /* Returns the json string */
  JSONNode SubGroup::json(std::string name) const
  {   
    try
    {
      if(name == "id")
        return JSONNode("", id());

      JSONNode json = LieGroup::json();
      json.pop_back("id");
      json.pop_back("Reps");
      json.pop_back("MaxSubgroups");
      json.pop_back("Subgroups");
      json.pop_back("BrChains");

      json.push_back(JSONNode("id",id()));
      json.push_back(JSONNode("SuperGroup", _SuperGroup->id()));
      json.push_back(_SuperGroups.json("SuperGroups"));
      json.push_back(_Projection.json("Projection"));

      JSONNode labels(JSON_ARRAY);
      labels.set_name("labels");
      for(int i=0; i<_labels.nterms(); i++)
        labels.push_back(JSONNode("",_labels.GetObject(i)));
      json.push_back(labels);
      
      json.push_back(JSONNode("superRank",_superRank));
      json.push_back(JSONNode("maximal",_maximal));
      json.push_back(JSONNode("regular",_regular));
      json.push_back(JSONNode("special",_special));
    
      return json;
    
    }
    catch (...) { throw; }
  }

  /* Parses a json object into the attributes of the class */
  void SubGroup::ParseJSON(const JSONNode & n)
  {
    try
    {   
      JSONNode json = n;
      std::string id = json.at("id").as_string();
      
      std::stringstream ss(id), ss2;
      
      while(getline(ss, id, '('))
      {
        ss2 << id;
        getline(ss, id, ')');
      }
      getline(ss2,id,'[');
      json.pop_back("id");
      json.insert(json.begin(),JSONNode("id",id));
      
      LieGroup::ParseJSON(json);
      JSONNode::const_iterator i = json.begin();
      while (i != json.end())
      {
        // get the node name and value as a string
        std::string node_name = i -> name();

        // find out where to store the values
        if(node_name == "SuperGroup")
          _SuperGroup = DB<LieGroup>().at(i->as_string());
        else if(node_name == "SuperGroups")
            _SuperGroups.ParseJSON(*i);
        else if(node_name == "Projection")
          _Projection.ParseJSON(*i);
        else if(node_name == "labels" and !_labels.nterms())
        {
          JSONNode::const_iterator it = i->begin();
          _labels.Clear();
          while(it != i->end())
          {
            _labels.AddTerm(it->as_string());
            ++it;
          }
        }
        else if(node_name == "maximal")
          _maximal = i->as_bool();
        else if(node_name == "regular")
          _regular = i->as_bool();
        else if(node_name == "special")
          _special = i->as_bool();	
    
        //increment the iterator
        ++i;
      }
    }
    catch (...) { throw; }
  }

}
