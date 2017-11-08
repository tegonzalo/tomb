/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * field.cc
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 13/01/2016
 */

#include "field.h"

/***************************/
/* Class Field definitions */
/***************************/

namespace Tomb
{

  // Member functions

  /* Constructor */
  Field::Field(Rrep GaugeRep, Rrep LorentzRep) : Rrep(GaugeRep)
  {
    try 
    {
      _LorentzRep = LorentzRep;
      
    } catch (...) { throw; }
  }
  
  /* Constructor wiht gauge rep and string */
  Field::Field(Rrep GaugeRep, std::string LorentzRep) : Rrep(GaugeRep)
  {
    try
    {
      if(LorentzRep == "Scalar")
        _LorentzRep = Rrep("(0,0)A1xA1");
      else if(LorentzRep == "Fermion")
        _LorentzRep = Rrep("(1,0)A1xA1");
      else if(LorentzRep == "Antifermion")
        _LorentzRep = Rrep("(0,1)A1xA1");
      else if(LorentzRep == "Vector")
        _LorentzRep = Rrep("(1,1)A1xA1");
      else
        _LorentzRep = Rrep(LorentzRep);
    }
    catch (...) { throw; }
  }
  
  /* Constructor with strings */
  Field::Field(std::string id)
  {
    try
    {
      std::stringstream idstream(id);
      std::string gaugeid, lorentzid;
      
      idstream >> gaugeid;
      idstream >> lorentzid;
      
      _LorentzRep = Rrep(lorentzid);
      
      *this = Field(Rrep(gaugeid),_LorentzRep);
      
    }
    catch (...) { throw; }
  }
  
  /* Copy constructor */
  Field::Field(const Field &f) : Rrep(f)
  {
    try
    {
      _LorentzRep = f.LorentzRep();
    }
    catch (...) { throw; }
  }
  
  
  /* Copy constructor with JSON node */
  Field::Field(const JSONNode &json)
  {
    try
    {
      ParseJSON(json);
    }
    catch (...) { throw; }
  }

  /* Move constructor */
  Field::Field(Field &&f) :
    Rrep(std::move(f)),
    _LorentzRep(std::move(f._LorentzRep))
  {
    try
    {
      f._LorentzRep = Rrep();
      
    }
    catch (...) { throw; }
  }

  /* Destructor */
  Field::~Field()
  {
  }

  /* Assignment operator */
  Field &Field::operator=(const Field &f)
  { 
    try
    {
      if(this == &f) return *this;
      
      
      Rrep::operator=(f);
      _LorentzRep = f.LorentzRep();
      
      return *this;
    }
    catch (...) { throw; }
  }

  /* Move assignment operator */
  Field &Field::operator=(Field &&f)
  {
    try
    {
      if(this == &f) return *this;
      
      Rrep::operator=(std::move(f));
      _LorentzRep = std::move(f._LorentzRep);
      
      f._LorentzRep = Rrep();
      
      return *this;
    }
    catch (...) { throw; }
  }
  
  /* Returns the id of the field */
  std::string Field::id() const
  {
    try
    {
      std::stringstream stream;
      stream << Rrep::id() << " " << _LorentzRep.id();
      return stream.str();
    }
    catch (...) { throw; }
  }
  
  /* Breaking chain getter */
  Rrep Field::LorentzRep() const
  {
    return _LorentzRep;
  }
  
  /* Obtains the Lorentz group from the Lorentz rep */
  LieGroup Field::LorentzGroup() const
  {
    return _LorentzRep.Group();
  }

  /* Is the field a scalar in Lorentz */
  bool Field::isScalar() const
  {
    try
    {
      if(_LorentzRep.isSinglet())
        return true;
      else
        return false;
      
    } catch (...)
    {
      throw;
    }
  }
  
  /* Is the field a fermion in Lorentz */
  bool Field::isFermion() const
  {
    try
    {
      if(_LorentzRep.dim() == 2)
        return true;
      else
        return false;
      
    } catch (...)
    {
      throw;
    }
  }
  
  /* Is the field a vector in Lorentz */
  bool Field::isVector() const
  {
    try
    {
      if(_LorentzRep.dim() == 4 and _LorentzRep.GetObject(0).dim() == 2)
        return true;
      else
        return false;
      
    } catch (...)
    {
      throw;
    }
  }
  
  /* Calculates whether the rep can break into the chain */
  bool Field::canBreak(Chain &chain) const
  {
    try
    {
      if(isSinglet()) return false;
      bool canbreak = false;
      for(int i=0; i<chain.nterms(); i++)
      {
        std::string group = chain.GetObject(i).Object();
        std::string label = chain.GetObject(i).label();
        List<std::string> subgroup = Chain(chain.GetObject(i).Branches()).getLevel(0);
        List<std::string> labels = Chain(chain.GetObject(i).Branches()).getLabels(0);

        // Conditions for breaking on the chain
        if(subgroup.nterms()==0 or subgroup.nterms() > 1 or group != subgroup.GetObject(0) or label != labels.GetObject(0))
        {
          if(GetObject(i).isSinglet())
            return false;
        } else
        {
          if(!GetObject(i).isSinglet())
            return false;
        }
      }
      
      return true;
    }
    catch (...) { throw; }
  }
  
  /* Decompose the field into subfields of the subgroup */
  Sum<Field> Field::Decompose(SubGroup &subgroup)
  {
    try
    {
      Sum<Rrep> gaugesubreps = Rrep::Decompose(subgroup);
      
      Sum<Field> subreps;
      
      for(Sum<Rrep>::iterator it = gaugesubreps.begin(); it != gaugesubreps.end(); it++)
      {
        Field subrep(*it, _LorentzRep);
        subreps.AddTerm(subrep);
      }
      
      return subreps;

    }
    catch (...) { throw; }
  }
  
  /* Overloaded == operator */
  bool Field::operator==(const Field &f) const
  {
    return (Rrep::operator==(f) and LorentzRep() == f.LorentzRep());
  }
  
  /* Overloaded != operator */
  bool Field::operator!=(const Field &f) const
  {
    return !(*this == f);
  }
  
  /* Overloaded < operator */
  bool Field::operator<(const Field &f) const
  {
    if(isScalar() and f.isFermion()) return true;
    if((isScalar() and f.isScalar()) or (isFermion() and f.isFermion()))
      if(Rrep::operator<(f))
        return true;
    return false;
  }
  
  /* Overloaded > operator */
  bool Field::operator>(const Field &f) const
  {
    if ((*this)<f)
      return false;
    return true;
  }

  /* Returns the json string */
  JSONNode Field::json(std::string name) const
  {
    return JSONNode("", id());
  }

  /* Parses a json object into the attributes of the class */
  void Field::ParseJSON(const JSONNode & json)
  {
    try
    {
      std::stringstream idstream(json.as_string());
      std::string gaugeid, lorentzid;
    
      idstream >> gaugeid;
      idstream >> lorentzid;
    
      _LorentzRep = Rrep(lorentzid);
      
      *this = Field(Rrep(gaugeid),_LorentzRep);
      
    } catch (...)
    {
      throw;
    }
  }
  
  /* Overloaded << operator with theories on the right */
  std::ostream &operator<<(std::ostream &stream, const Field &f)
  {
    stream << f.label();
    if(f.isScalar())
      stream << "_S";
    else if(f.isFermion())
      stream << "_F";
    else if(f.isVector())
      stream << "_V";
    return stream;
  }
}
