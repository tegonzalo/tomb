/*
 * field.cc
 * Created by T. Gonzalo on 13/01/16
 * Last modified on 13/01/16
 */

#include "headers.h"

/**************************************************/
/************Class Field methods*************/
/**************************************************/

namespace Tomb
{

	// Member functions

	/* Constructor */
	Field::Field(Rrep GaugeRep, Rrep LorentzRep) : Rrep(GaugeRep)
	{
		try 
		{
			//_GaugeRep = GaugeRep;
			_LorentzRep = LorentzRep;
			
		} catch (...)
		{
			throw;
		}
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
			
			//_GaugeRep = Rrep(gaugeid);
			_LorentzRep = Rrep(lorentzid);
			
			*this = Field(Rrep(gaugeid),_LorentzRep);
			
		} catch (...)
		{
			throw;
		}
	}
	
	/* Copy constructor */
	Field::Field(const Field &f) : Rrep(f)
	{
		try
		{
			//_GaugeRep = f.GaugeRep();
			_LorentzRep = f.LorentzRep();
		} catch (...)
		{
			throw;
		}
	}
	
	
	/* Copy constructor with JSON node */
	Field::Field(const JSONNode &json)
	{
		try
		{
			ParseJSON(json);
		} catch (...)
		{
			throw;
		}
	}

	/* Move constructor */
	Field::Field(Field &&f) :
		//_GaugeRep(std::move(f._GaugeRep)),
		Rrep(std::move(f)),
		_LorentzRep(std::move(f._LorentzRep))
	{
		try
		{
			//f._GaugeRep = Rrep();
			f._LorentzRep = Rrep();
			
		} catch (...)
		{
			throw;
		}
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
			
			//_GaugeRep = f.GaugeRep();
			
			Rrep::operator=(f);
			_LorentzRep = f.LorentzRep();
			
			return *this;
		} catch (...)
		{
			throw;
		}
	}

	/* Move assignment operator */
	Field &Field::operator=(Field &&f)
	{
		try
		{
			if(this == &f) return *this;
			
			//_GaugeRep = std::move(f._GaugeRep);
			Rrep::operator=(std::move(f));
			_LorentzRep = std::move(f._LorentzRep);
			
			//f._GaugeRep = Rrep();
			f._LorentzRep = Rrep();
			
			return *this;
		} catch (...)
		{
			throw;
		}
	}
	
	/* Returns the id of the field */
	std::string Field::id() const
	{
		try
		{
			std::stringstream stream;
			//stream << _GaugeRep.id() << " " << _LorentzRep.id();
			stream << Rrep::id() << " " << _LorentzRep.id();
			return stream.str();
		} catch (...)
		{
			throw;
		}
	}
	
	/* Gauge rep getter */
	/*Rrep Field::GaugeRep() const
	{
		return _GaugeRep;
	}*/
	
	/* Breaking chain getter */
	Rrep Field::LorentzRep() const
	{
		return _LorentzRep;
	}
	
	/* Obtains the Gauge group from the Gauge rep */
	/*LieGroup Field::GaugeGroup() const
	{
		return _GaugeRep.Group();
	}*/
	
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
		try {
			//if(GaugeRep().isSinglet()) return false;
			if(isSinglet()) return false;
			bool canbreak = false;
			for(int i=0; i<chain.nterms(); i++)
			{
				
				SimpleGroup group = chain.GetObject(i).Object();
				std::string label = chain.GetObject(i).label();
				List<SimpleGroup> subgroup = Chain(chain.GetObject(i).Branches()).getLevel(0);
				List<std::string> labels = Chain(chain.GetObject(i).Branches()).getLabels(0);


				// Conditions for breaking on the chain
				if(subgroup.nterms()==0 or subgroup.nterms() > 1 or group != subgroup.GetObject(0) or label != labels.GetObject(0))
				{
					//if(GaugeRep().GetObject(i).isSinglet())
					if(GetObject(i).isSinglet())
						return false;
				} else
				{
					//if(!GaugeRep().GetObject(i).isSinglet())
					if(!GetObject(i).isSinglet())
						return false;
				}
			}
			
			return true;
		} catch (...)
		{
			throw;
		}
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

		} catch (...)
		{
			throw;
		}
	}
	
	/* Overloaded == operator */
	bool Field::operator==(const Field &f) const
	{
		// return (GaugeRep() == f.GaugeRep() and LorentzRep() == f.LorentzRep());
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
			//if(GaugeRep() < f.GaugeRep())
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
		
			//_GaugeRep = Rrep(gaugeid);
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
		//stream << f.GaugeRep().label() << "_" << (f.isScalar() ? "S" : "F");
		stream << f.label() << "_" << (f.isScalar() ? "S" : "F");
		return stream;
	}
}