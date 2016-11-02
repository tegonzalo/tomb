/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
* \file
* field.h
*
* \author
* T. Gonzalo (t.e.gonzalo@fys.uio.no)
*
* \date
* 13/01/2016
*/

#ifndef __FIELD_H
#define __FIELD_H

#include "libjson.h"
#include "liegroup.h"
#include "subgroup.h"
#include "rrep.h"
#include "chain.h"

namespace Tomb
{

  class Field : public Rrep
  {
    
    protected:
      
      //Rrep _GaugeRep;
      Rrep _LorentzRep;
      
    public:
      
      Field(Rrep, Rrep);
      Field(Rrep, std::string);
      Field(std::string);
      Field(const Field &);
      Field(const JSONNode &);
      Field(Field &&);
      ~Field();
      Field &operator=(const Field &);
      Field &operator=(Field &&);
      
      std::string id() const;
      
      //Rrep GaugeRep() const;
      Rrep LorentzRep() const;
      
      //LieGroup GaugeGroup() const;
      LieGroup LorentzGroup() const;
      
      bool isScalar() const;
      bool isFermion() const;
      bool isVector() const;
      
      bool canBreak(Chain &) const;
      Sum<Field> Decompose(SubGroup &);
      
      bool operator==(const Field &) const;
      bool operator!=(const Field &) const;
      bool operator<(const Field &) const;
      bool operator>(const Field &) const;
      
      JSONNode json(std::string = "id") const;
      void ParseJSON(const JSONNode &);
  };
  
  
  std::ostream &operator<<(std::ostream &, const Field &);


}

#endif /* __FIELD_H */
