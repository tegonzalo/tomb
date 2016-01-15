/*
 * field.h
 * Created by T. Gonzalo on 13/01/16
 * Last modified on 13/01/16
 */

#ifndef __FIELD_H
#define __FIELD_H

#include "headers.h"

namespace Tomb
{

	class Field : public Rrep
	{
		
		protected:
			
			//Rrep _GaugeRep;
			Rrep _LorentzRep;
			
		public:
			
			Field(Rrep, Rrep);
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
			Sum<Field> Decompose(SubGroup);
			
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