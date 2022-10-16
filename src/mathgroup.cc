/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * mathgroup.cc
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 21/07/2014
 */

#include <ctime>
#include "files.h"
#include "database.h"
#include "simplegroup.h"
#include "liegroup.h"
#include "subgroup.h"
#include "irrep.h"

#ifdef HAVE_MATHEMATICA
#include MATHEMATICA_MATHLINK_H

/*************************************************/
/* Main function and getter methods for MathLink */
/*************************************************/

using namespace Tomb;

void getGroup(int rank, const char *type)
{
  try
  {
    SimpleGroup G(rank, type[0]);
    G.Irreps();
    G.Subgroups();
    MLPutString(stdlink, G.json().write_formatted().c_str());
  }
  catch (std::exception &e)
  {
    std::stringstream excep;
    excep << "Exception caught: " << e.what() << std::endl;
    MLPutString(stdlink, excep.str().c_str());
  }	
  return ;
}

void getGroup(const char *id)
{
  try
  {
    LieGroup G(id);
    G.Order();
    G.Reps();
    G.Subgroups();
    MLPutString(stdlink, G.json().write_formatted().c_str());
  }
  catch (std::exception &e)
  {
    std::stringstream excep;
    excep << "Exception caught: " << e.what() << std::endl;
    MLPutString(stdlink, excep.str().c_str());
  }
  return ;
}

void getReps(const char *id, int maxdim)
{
  try
  {
    SimpleGroup G(id);
    List<Irrep> Reps = G.Irreps(maxdim);
    MLPutString(stdlink, Reps.json().write_formatted().c_str());
  }
  catch (std::exception &e)
  {
    std::stringstream excep;
    excep << "Exception caught: " << e.what() << std::endl;
    MLPutString(stdlink, excep.str().c_str());
  }
  return ;
}

void getSubgroups(const char *id)
{
  try
  {
    SimpleGroup G(id);
    List<SubGroup> Subgroups = G.Subgroups();
    MLPutString(stdlink, Subgroups.json().write_formatted().c_str());
  }
  catch (std::exception &e)
  {
    std::stringstream excep;
    excep << "Exception caught: " << e.what() << std::endl;
    MLPutString(stdlink, excep.str().c_str());
  }
  return ;
}

void getRep(const char *id)
{
  try
  {
    Rrep R(id);
    if(R.nirreps() == 1)
    {
      Irrep R1(R.GetObject(0));
      MLPutString(stdlink, R1.json().write_formatted().c_str());
    }
    else
    {
      MLPutString(stdlink, R.json().write_formatted().c_str());
    }
  }
  catch (std::exception &e)
  {
    std::stringstream excep;
    excep << "Exception caught: " << e.what() << std::endl;
    MLPutString(stdlink, excep.str().c_str());
  }
  return ;
}

void getRep(const char *dimId, const char *group)
{
  try
  {
    LieGroup G(group);
    List<double> dims;
    // Parse the json string for the dimensions
    JSONNode json = libjson::parse(dimId);
    JSONNode::const_iterator it = json.begin();
    dims.Empty();
    while(it != json.end())
    {
      dims.AddTerm(it->as_float());
      ++it;
    }
    
    List<Irrep> Irreps;
    
/*    for(int i=0; i<dims.nterms(); i++)
    {
      double dim = dims.GetObject(i);
      SimpleGroup G1 = G.GetObject(i);
      if(G1.abelian())
        Irreps.AddTerm(Irrep(G1, Weight(G1, dim)));
      else
      {
        if(dim > 0)
        {
          List<Irrep> Irreps2 = G1.Irreps(int(dim));
          List<Irrep>::iterator it_Irreps2;
          for(it_Irreps2 = Irreps2.begin(); it_Irreps2 != Irreps2.end() and it_Irreps2->dim() != int(dim); it_Irreps2++);
          Irreps.AddTerm(*it_Irreps2);
        }
        else
        {
          List<Irrep> Irreps2 = G1.Irreps(int(-dim));
          List<Irrep>::iterator it_Irreps2;
          for(it_Irreps2 = Irreps2.begin(); it_Irreps2 != Irreps2.end() and (it_Irreps2->dim() != int(-dim) or (it_Irreps2->dim() == int(-dim) and !it_Irreps2->conjugate())); it_Irreps2++);
          Irreps.AddTerm(*it_Irreps2);
        }
      }
    }
    
    Rrep Rep(Irreps.GetObject(0));
    for(int i=1; i<Irreps.nterms(); i++)
      Rep.AddIrrep(Irreps.GetObject(i));
    
    MLPutString(stdlink, Rep.json().write_formatted().c_str());
*/
  }
  catch (std::exception &e)
  {
    std::stringstream excep;
    excep << "Exception caught: " << e.what() << std::endl;
    MLPutString(stdlink, excep.str().c_str());
  }
  return ;
}

void getSubgroup(const char *id)
{
  try 
  { 
    SubGroup G(id);
    MLPutString(stdlink, G.json().write_formatted().c_str());
  }
  catch (std::exception &e)
  {
    std::stringstream excep;
    excep << "Exception caught: " << e.what() << std::endl;
    MLPutString(stdlink, excep.str().c_str());
  }
  return ;
}	

void getSubgroup(const char *id, const char *MixingId)
{
  try
  { 
    Matrix<double> Mixing(MixingId);		
    if(Mixing.rows() > Mixing.cols())
    {
      Mixing = Mixing.Transpose();
    }
    SubGroup G(id, Mixing);
    MLPutString(stdlink, G.json().write_formatted().c_str());
  }
  catch (std::exception &e)
  {
    std::stringstream excep;
    excep << "Exception caught: " << e.what() << std::endl;
    MLPutString(stdlink, excep.str().c_str());
  }
  return ;
}

void getBreakingChains(const char *SuperId, const char *SubId)
{
  try
  {
    LieGroup G(SuperId);
    LieGroup SubG(SubId);
    List<List<Tree<SimpleGroup> > > Chains = G.BreakingChains(SubG);
    MLPutString(stdlink, Chains.json("nolabel").write_formatted().c_str());
  }
  catch (std::exception &e)
  {
    std::stringstream excep;
    excep << "Exception caught: " << e.what() << std::endl;
    MLPutString(stdlink, excep.str().c_str());
  }
  return ;
}

void getDecomposeRep(const char *RepId, const char *SubgroupId, const char *MixingId)
{
  try
  {
    Rrep R(RepId);
    //LieGroup G = R.Group();
    
    Matrix<double> Mixing(MixingId);
    if(Mixing.rows() > Mixing.cols()) 
      Mixing = Mixing.Transpose();
    //List<SubGroup> Subgroups = G.Subgroups();
    //int i=0;
    //while(i< Subgroups.nterms() and Subgroups.GetObject(i).id() != SubgroupId) {
    //	i++;	
    //}
    Sum<Rrep> Dec;
    //if(i<Subgroups.nterms()) {
    //	SubGroup SubG = G.Subgroups().GetObject(i);
    SubGroup SubG = SubGroup(SubgroupId, Mixing);
    //MLPutString(stdlink, SubgroupId);
    //if(Subgroups.Index(SubG) > -1) {
      Dec = R.Decompose(SubG);
    //}
    MLPutString(stdlink, Dec.json().write_formatted().c_str());
  }
  catch (std::exception &e)
  {
    std::stringstream excep;
    excep << "Exception caught: " << e.what() << std::endl;
    MLPutString(stdlink, excep.str().c_str());
  }
  return ;
}

void getWeights(const char *RepId)
{
  try
  {

    Rrep R(RepId);
  
    List<Weight> Weights = R.Weights();
  
    MLPutString(stdlink, Weights.json().write_formatted().c_str());

  }
  catch (std::exception &e)
  {
    std::stringstream excep;
    excep << "Exception caught: " << e.what() << std::endl;
    MLPutString(stdlink, excep.str().c_str());
  }
}

void getDirectPoduct(const char *RepId1, const char *RepId2)
{
  try
  {
    Rrep R1(RepId1);
    Rrep R2(RepId2);
    
    List<Rrep> Prod = R1*R2;
    
    MLPutString(stdlink, Prod.json().write_formatted().c_str());
    
  }
  catch (std::exception &e)
  {
    std::stringstream excep;
    excep << "Exception caught: " << e.what() << std::endl;
    MLPutString(stdlink, excep.str().c_str());
  }
}

void getInvariants(const char *RepsId, int dim)
{  
  try
  {
    List<Rrep> Reps;

    JSONNode json = libjson::parse(RepsId);
    JSONNode::const_iterator it = json.begin();
    Reps.Empty();
    while(it != json.end())
    {
      Rrep Rep(it->as_string());
      Reps.AddTerm(Rep);
      ++it;
    }
  
    LieGroup G = Reps.GetObject(0).Group();
  
//    List<Product<Rrep> > Invariants = G.Invariants(Reps,dim);
  
//    MLPutString(stdlink,  Invariants.json().write_formatted().c_str());
  
  }
  catch (std::exception &e)
  {
    std::stringstream excep;
    excep << "Exception caught: " << e.what() << std::endl;
    MLPutString(stdlink, excep.str().c_str());
  }
}

int main(int argc, char *argv[])
{
  
  return MLMain(argc,argv);

}

#endif /* HAVE_MATHEMATICA */
