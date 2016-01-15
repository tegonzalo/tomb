/*
 * theory.cc
 * Created by T. Gonzalo on 12/01/16
 * Last modified on 12/01/16
 */

#include "headers.h"

/**************************************************/
/************Class Theory methods*************/
/**************************************************/

namespace Tomb
{

	// Member functions

	/* Constructor */
	Theory::Theory(LieGroup &Group, Chain &BreakingChain, List<Field> &Fields) {

		try {
			_Group = Group;
			_BreakingChain = BreakingChain;
			for(int i=0; i<Fields.nterms(); i++)
				std::cout << Fields.GetObject(i).nterms() << std::endl;
			_Fields = Fields;
			std::cout << _Fields <<std::endl;
			_Fields.Order();
			std::cout << _Fields <<std::endl;
			_Mixing = RVector<double>();
			_anomalyFree = true;
			_protonDecay.Clear();
			
		} catch (...) {
			throw;
		}
	}
	
	/* Copy constructor */
	Theory::Theory(const Theory &T) {
		
		try {
			_Group = T.Group();
			_BreakingChain = T.BreakingChain();
			_Fields = T.Fields();
			_Mixing = T.Mixing();
			_anomalyFree = T.anomalyFree();
			_protonDecay = T.protonDecay();
		} catch (...) {
			throw;
		}
	}
	
	
	/* Copy constructor with JSON node */
	Theory::Theory(const JSONNode &json) {
		try {
			ParseJSON(json);
		} catch (...) {
			throw;
		}
	}

	/* Move constructor */
	Theory::Theory(Theory &&T) :
		_Group(std::move(T._Group)),
		_BreakingChain(std::move(T._BreakingChain)),
		_Fields(std::move(T._Fields)),
		_Mixing(std::move(T._Mixing)),
		_anomalyFree(std::move(T._anomalyFree)),
		_protonDecay(std::move(T._protonDecay))
	{
		
		try {
			T._Group = LieGroup();
			T._BreakingChain.Clear();
			T._Fields.Clear();
			T._Mixing = RVector<double>();
			T._anomalyFree = false;
			T._protonDecay.Clear();
			
		} catch (...) {
			throw;
		}
	}

	/* Destructor */
	Theory::~Theory() {
	}

	/* Assignment operator */
	Theory &Theory::operator=(const Theory &T) {
		
		try {
			if(this == &T) return *this;
			
			_Group = T.Group();
			_BreakingChain = T.BreakingChain();
			_Fields = T.Fields();
			_Mixing = T.Mixing();
			_anomalyFree = T.anomalyFree();
			_protonDecay = T.protonDecay();
			
			return *this;
		} catch (...) {
			throw;
		}
	}

	/* Move assignment operator */
	Theory &Theory::operator=(Theory &&T) {
		
		try {
			if(this == &T) return *this;
			
			_Group = std::move(T._Group);
			_BreakingChain = std::move(T._BreakingChain);
			_Fields = std::move(T._Fields);
			_Mixing = std::move(T._Mixing);
			_anomalyFree = std::move(T._anomalyFree);
			_protonDecay = std::move(T._protonDecay);
			
			T._Group = LieGroup();
			T._BreakingChain.Clear();
			T._Fields.Clear();
			T._Mixing = RVector<double>();
			T._anomalyFree = false;
			T._protonDecay.Clear();
			
			return *this;
		} catch (...) {
			throw;
		}
	}
	
	
	/* Group getter */
	LieGroup Theory::Group() const {
		return _Group;
	}
	
	/* Breaking chain getter */
	Chain Theory::BreakingChain() const {
		return _BreakingChain;
	}
	
	/* Fields getter */
	List<Field> Theory::Fields() const {
		return _Fields;
	}
	
	/* Mixing getter */
	RVector<double> Theory::Mixing() const {
		return _Mixing;
	}

	/* anomalyFre getter */
	bool Theory::anomalyFree() const {
		return _anomalyFree;
	}
	
	/* protonDecay getter */
	List<std::string> Theory::protonDecay() const {
		return _protonDecay;
	}
	
	/* Obtain the scalars */
	List<Field> Theory::getScalars() const {
		try {
			List<Field> scalars;
			for(List<Field>::const_iterator it_Fields = _Fields.begin(); it_Fields != _Fields.end(); it_Fields++)
				if(it_Fields->isScalar())
					scalars.AddTerm(*it_Fields);
			return scalars;
		} catch (...) {
			throw;
		}
	}
	
	/* Obtain the fermions */
	List<Field> Theory::getFermions() const {
		try {
			List<Field> fermions;
			for(List<Field>::const_iterator it_Fields = _Fields.begin(); it_Fields != _Fields.end(); it_Fields++)
				if(it_Fields->isFermion())
					fermions.AddTerm(*it_Fields);
			return fermions;
		} catch (...) {
			throw;
		}
	}
	
	/* Overloaded == operator */
	bool Theory::operator==(const Theory &theory) const 
	{
		if(Group() == theory.Group() and BreakingChain() == theory.BreakingChain() and Fields() == theory.Fields())
			return true;
		return false;
	}

	/* Overloaded != operator */
	bool Theory::operator!=(const Theory &theory) const
	{
		if(!(Group() == theory.Group() and BreakingChain() == theory.BreakingChain() and Fields() == theory.Fields()))
			return true;
		return false;
	}

	/* Find the breaking reps from this theory to subgroup with optional reps */
	List<Field> Theory::findBreakingReps(SubGroup &subgroup, const List<Field> &reps) {
		try {
			List<Field> breakingreps;
			List<Field> scalars;
			
			if(reps.Empty())
				scalars = getScalars();
			else
				for(List<Field>::const_iterator it_reps = reps.begin(); it_reps != reps.end(); it_reps++)
					if(it_reps->isScalar())
						scalars.AddTerm(*it_reps);
										
			for(List<Field>::iterator it_scalars = scalars.begin(); it_scalars != scalars.end(); it_scalars++) 
			{
				Field scalar = *it_scalars;
				if(scalar.canBreak(_BreakingChain)) {
					// Decompose every scalar into fields in the subgroup
					List<Field> subreps = scalar.Decompose(subgroup);

					// Check whether the decomposition has singlets
					bool singlet = false;
					for(List<Field>::iterator it_subreps = subreps.begin(); it_subreps != subreps.end(); it_subreps++)
					{
						if(it_subreps->isSinglet()) 
							singlet = true;
					}
					// If it has a singlet check that the rep does not decompose into intermediate groups (NOT COMPLETE)
					if(singlet and !(Group().rank() > subgroup.rank() and scalar.dim() == Group().dim()))
						breakingreps.AddTerm(scalar);
				
				}
			}

			return breakingreps;
		} catch (...) {
			throw;
		}
	}
	
	/* Calculates the breaking */
	List<Sum<Field> > Theory::calculateBreaking(List<RVector<double> > &mixings) {
		try {
			
			SubGroup subgroup = BreakingChain().extractSubgroups().GetObject(1);

			List<Field> fermions = getFermions();
			List<Field> scalars = getScalars();
			
			List<Field> breakingreps;

			// If there is a + character in the subgroup, there is U(1) mixing
			if(subgroup.id().find("+") != std::string::npos) {
			
				SubGroup auxsubgroup = subgroup;
				auxsubgroup.DeleteTerm(-1);
				SubGroup semisimplesubgroup(auxsubgroup.id());
				
				breakingreps = findBreakingReps(semisimplesubgroup, scalars);
				
				std::cout << semisimplesubgroup << std::endl;
				std::cout << breakingreps << std::endl;
   
				// Calculate the Mixing of the U(1)s
				List<std::string> labels = Strings::split_string(subgroup.labels().GetObject(-1), '+');
				std::cout << labels << std::endl;
				for(auto j=breakingreps.begin(); j!=breakingreps.end(); j++)
				{
					List<double> charges;
					RVector<double> mixing(2);
					for(int i=0; i<labels.nterms(); i++) {
						std::stringstream groupstream;
						groupstream << Strings::split_string(semisimplesubgroup.id(), '[').GetObject(0) << "xU1(" << labels.GetObject(i) << ")[" << _Group.id() << "]";
						SubGroup subgroup(groupstream.str());
						Sum<Field> subreps = j->Decompose(subgroup);
						Sum<Field>::iterator k;
						for(k=subreps.begin(); k!=subreps.end() and k->dim() != 1 and k->HWeight()[-1] == 0; k++);
						charges.AddTerm(k->HWeight()[-1]);
					}
					// In case of two U(1) Mixing 
					if(charges.GetObject(1) == 0)
						mixing[1] = 1;
					else
					{
						mixing[0] = 1;
						mixing[1] = -charges.GetObject(0)/charges.GetObject(1);
					}
					mixings.AddTerm(mixing);
					
				}
				
				// Create the subgroup with the mixing and calculate the subreps
				List<Sum<Field> > subreps;
				for(int j=0; j<breakingreps.nterms(); j++)
				{
					SubGroup newsubgroup(subgroup.id(), mixings.GetObject(j));
					List<Field> fields = _Fields;
					fields.DeleteObject(breakingreps.GetObject(j),1);
					
					Sum<Field> subrep;
					for(List<Field>::iterator it_fields = fields.begin(); it_fields != fields.end(); it_fields++)
					{
						Sum<Field> auxsubrep = it_fields->Decompose(subgroup);
						subrep.AppendList(auxsubrep);
					}
					subreps.AddTerm(subrep);
				}
				
				return subreps;
			}
  
			// There is no U1 mixing here
			breakingreps = findBreakingReps(subgroup,scalars);
			
			List<Sum<Field> > subreps;
			for(int i=0; i<breakingreps.nterms(); i++) {
				List<Field> fields = _Fields;
				fields.DeleteObject(breakingreps.GetObject(i),1);
				
				Sum<Field> subrep;
				for(List<Field>::iterator it_fields = fields.begin(); it_fields != fields.end(); it_fields++)
				{
					Sum<Field> auxsubrep = it_fields->Decompose(subgroup);
					subrep.AppendList(auxsubrep);
				}
				subreps.AddTerm(subrep);
				mixings.AddTerm(RVector<double>(Group().rank() - subgroup.rank() + 1));
			}

			
			return subreps;
			
		} catch (...) {
			throw;
		}
	}
	
	/* Check if the field content reproduces the SM at least */
	bool Theory::containsSM() const
	{
		try
		{
			// Recast the hypercharge normalisation of the SMreps

			List<Field>::const_iterator pos = _Fields.begin();
			while(pos != _Fields.end() and (pos->dim() != 1 or pos->HWeight()[-1] == 0 or !pos->isFermion())) pos++;
			double norm = pos->HWeight()[-1]/StandardModel::e.HWeight()[-1];
			std::cout << norm << std::endl;

			List<Field> SMReps;
			for(List<Field>::const_iterator it = StandardModel::Reps.begin(); it != StandardModel::Reps.end(); it++)
			{
				Weight HW = it->HWeight();
				HW[-1]*= norm;
				SMReps.AddTerm(Field(Rrep(StandardModel::Group, HW),it->LorentzRep()));
			}
			std::cout << SMReps << std::endl;
			std::cout << _Fields << std::endl;
			
			/*

SMReps = Table[
   Charge = 
    SetAccuracy[Attribute[SMreps[[i]]]["HWeight"][[1, -1]]*Norm, 6];
   Id = StringSplit[Attribute[SMreps[[i]]]["id"], ","];
   Id = Id[[1]] <> ", " <> Id[[2]] <> ", " <> Id[[3]] <> "," <> 
     ToString[Charge] <> ")A2xA1xU1";
   (*Print[Id];
   Print[Attribute[SMreps[[i]]]["type"]];*)
   
   NewRep[Id, Attribute[SMreps[[i]]]["type"]],
   {i, Length[SMreps]}];

If[OptionValue[PrintOut],
  Print[SMReps];
  Print[Reps];
  ];
If[Length[SMReps] > Length[Reps], Return[False]];

(* Check if the SM is contained *)
Deleted = 0;
Do[
  Pos = FirstPosition[Reps, SMReps[[i]]];
  If[OptionValue[PrintOut], Print[Pos]];
  If[Head[Pos] === Missing, Return[False];, Reps = Delete[Reps, Pos];];
  Deleted++;,
  {i, Length[SMReps]}];
If[Deleted != Length[SMReps], Out = False; Return[False]];*/

			return true;
			
		} catch (...)
		{
			throw;
		}
	}

	/* Returns the json string */
	JSONNode Theory::json() const {
		
		JSONNode json;
		
		json.push_back(JSONNode("Group", _Group.id()));
		json.push_back(_BreakingChain.json("BreakingChain"));
		json.push_back(_Fields.json("Fields"));
		
		return json;
	}
	
	/* Parses a json object into the attributes of the class */
	void Theory::ParseJSON(const JSONNode & json) {
		
		for(JSONNode::const_iterator i = json.begin(); i != json.end(); i++) {
			
			// get the node name and value as a string
			std::string node_name = i->name();

			// find out where to store the values
			if(node_name == "Group") {
				_Group = LieGroup(i->as_string());
			} else if(node_name =="BreakingChain") {
				_BreakingChain.ParseJSON(*i);
			} else if(node_name == "Fields") {
				_Fields.ParseJSON(*i);
			}
		}
	}
	
	/* Overloaded << operator with theories on the right */
	std::ostream &operator<<(std::ostream &stream, const Theory &t) {
		stream << t.json().write_formatted();
		return stream;
	}
}