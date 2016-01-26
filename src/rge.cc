/*
 * rge.cc
 * Created by T. Gonzalo on 22/01/16
 * Last modified on 22/01/16
 */

#include "headers.h"

/**************************************************/
/************Class RGE methods*************/
/**************************************************/

namespace Tomb
{
	// Definition of static variable
	List<RGE> RGE::DataBase;
	
	// Member functions

	/* Constructor with model */
	RGE::RGE(const Model &model)
	{
		try 
		{
			_ncouplings = 0;
			for(int i=0; i<model.nterms(); i++)
				if(model[i].Group().nterms() > _ncouplings)
					_ncouplings = model[i].Group().nterms();
			_bSUSY = slopesMatrix(model, &SUSYslopes);
			_bSM = slopesMatrix(model, &SMslopes);
			_Mixing = mixingMatrix(model);
			
		} catch (...)
		{
			throw;
		}
	}
	
	/* Constructor with theory */
	RGE::RGE(const Theory &theory)
	{
		try
		{
			_ncouplings = theory.Group().nterms();
			_bSUSY = slopesMatrix(Model(theory), &SUSYslopes);
			_bSM = slopesMatrix(Model(theory), &SMslopes);
			_Mixing = mixingMatrix(Model(theory));
			
		} catch (...)
		{
			throw;
		}
	}
	
	/* Copy constructor */
	RGE::RGE(const RGE &rge)
	{
		try
		{
			_ncouplings = rge.ncouplings();
			_bSUSY = rge.bSUSY();
			_bSM = rge.bSM();
			_Mixing = rge.Mixing();
		} catch (...)
		{
			throw;
		}
	}
	
	
	/* Copy constructor with JSON node */
	RGE::RGE(const JSONNode &json)
	{
		try
		{
			ParseJSON(json);
		} catch (...)
		{
			throw;
		}
	}

	/* Copy constructor with strings */
	RGE::RGE(std::string id)
	{
		try
		{
			// Missing
		}
		catch (...)
		{
			throw;
		}
	}

	/* Move constructor */
	RGE::RGE(RGE &&rge) :
		_ncouplings(std::move(rge._ncouplings)),
		_bSUSY(std::move(rge._bSUSY)),
		_bSM(std::move(rge._bSM)),
		_Mixing(std::move(rge._Mixing))
	{
		try
		{
			rge._ncouplings = 0;
			rge._bSUSY = Matrix<double>();
			rge._bSM = Matrix<double>();
			rge._Mixing = Matrix<double>();
			
		} catch (...)
		{
			throw;
		}
	}

	/* Destructor */
	RGE::~RGE()
	{
	}

	/* Assignment operator */
	RGE &RGE::operator=(const RGE &rge)
	{
		
		try
		{
			if(this == &rge) return *this;
			
			_ncouplings = rge.ncouplings();
			_bSUSY = rge.bSUSY();
			_bSM = rge.bSM();
			_Mixing = rge.Mixing();
			
			return *this;
		} catch (...)
		{
			throw;
		}
	}

	/* Move assignment operator */
	RGE &RGE::operator=(RGE &&rge)
	{
		try
		{
			if(this == &rge) return *this;
			
			_ncouplings = std::move(rge._ncouplings);
			_bSUSY = std::move(rge._bSUSY);
			_bSM = std::move(rge._bSM);
			_Mixing = std::move(rge._Mixing);
			
			rge._ncouplings = 0;
			rge._bSUSY = Matrix<double>();
			rge._bSM = Matrix<double>();
			rge._Mixing = Matrix<double>();
			
			return *this;
		} catch (...)
		{
			throw;
		}
	}
	
	/* Number of couplings getter */
	int RGE::ncouplings() const
	{
		return _ncouplings;
	}
	
	/* SUSY slopes getter */
	Matrix<double> RGE::bSUSY() const
	{
		return _bSUSY;
	}
	
	/* Non-SUSY slopes getter */
	Matrix<double> RGE::bSM() const
	{
		return _bSM;
	}
	
	/* Mixing matrix getter */
	Matrix<double> RGE::Mixing() const
	{
		return _Mixing;
	}
	
	/* Function for calculating SUSY slopes */
	List<double> RGE::SUSYslopes(const List<Field> &fields)
	{
		try 
		{
			List<double> slopes;
			for(int j = 0; j<fields[0].nterms(); j++)
			{
				double slope = 0;
				for(auto it_fields = fields.begin(); it_fields != fields.end(); it_fields++)
					slope += it_fields->DynkinIndex().GetObject(j);
				slope -= 3*fields[0].Group().Casimir().GetObject(j);
				slopes.AddTerm(slope);
			}
			return slopes;
		}
		catch (...)
		{
			throw;
		}
	}
	
	/* Function for calculating non-SUSY slopes */
	List<double> RGE::SMslopes(const List<Field> &fields)
	{
		try
		{
			List<double> slopes;
			for(int j = 0; j<fields[0].nterms(); j++)
			{
				double slope = 0;
				for(auto it_fields = fields.begin(); it_fields != fields.end(); it_fields++)
				{
					if(it_fields->isFermion())
						slope += (2./3)*it_fields->DynkinIndex().GetObject(j);
					else if(it_fields->isScalar())
						slope += (1./3)*it_fields->DynkinIndex().GetObject(j);
				}
				slope -= (11./3)*fields[0].Group().Casimir().GetObject(j);
				slopes.AddTerm(slope);
			}
			return slopes;
		}
		catch (...)
		{
			throw;
		}
	}
	
	/* Calculates the matrix of slopes using a slopes function */
	Matrix<double> RGE::slopesMatrix(const Model &model, List<double> (*slopesFunction)(const List<Field> &))
	{
		try
		{
			Matrix<double> RGEs;
			Chain prevchain;
			
			for(int i = 0; i < model.nsteps(); i++)
			{
				Theory theory = model[i];
				List<Field> fields = theory.Fields();
				Chain chain = theory.BreakingChain();
				LieGroup group = theory.Group();
				SubGroup subgroup = chain.extractSubgroups()[0];

				List<double> slopes = (*slopesFunction)(fields);
				
				List<double> slopesOrdered;
				for(int j=0; j<chain.nterms(); j++)
				{
					int pos = subgroup.labels().Index(chain[j].label());
					//if(chain.GetObject(j).label().find("+") != std::string::npos)
					//{
						//if(nmixings != std::count(chain.GetObject(j).label().begin(), chain.GetObject(j).label().end(),'+'))
						//	slopesOrdered.AddTerm("-");
						//else
						//	slopesOrdered.AddTerm(slopes[pos]);
					//} else
						slopesOrdered.AddTerm(slopes[pos]);
				}
				
				if(!i)
				{
					CVector<double> column(slopesOrdered.nterms());
					for(int j=0; j<slopesOrdered.nterms(); j++)
						column[j] = slopesOrdered[j];
					RGEs = column;
				}
				else
				{
					CVector<double> column(std::max(RGEs.rows(),slopesOrdered.nterms()));
					int l=0, m=0;
					for(int j=0; j<prevchain.nterms(); j++)
					{
						if(prevchain[j].nbranches() == 0)
							l++;
						else
							for(int k=0; k<prevchain[j].nbranches(); k++)
							{
								column[l] = slopesOrdered[m];
								if(k)
									RGEs.InsertRow(l,RGEs.Row(l-1));
								l++;
								m++;
							}
					}
					RGEs.InsertColumn(i,column);
				}
			
				prevchain = chain;
			}
				
			return RGEs;
		}
		catch (...)
		{
			throw;
		}
	}

	/* Calculates the matrix of mixings */
	Matrix<double> RGE::mixingMatrix(const Model &model)
	{
		try
		{
			Matrix<double> Mix;
			Chain prevchain;
			//std::cout << model.nsteps() << std::endl;
			
			for(int i = 0; i < model.nsteps(); i++)
			{
				//std::cout << "i = " << i << std::endl;
				Theory theory = model[i];
				Chain chain = theory.BreakingChain();
				RVector<double> Mixing = theory.Mixing();

				List<int> indices;
				if(Mixing.cols())
				{
					List<std::string> labels = Strings::split_string(chain.getLabels(1).GetObject(-1),'+');
					List<std::string> labels2(labels);
					labels.Order("ASC");
					//std::cout << labels << std::endl;
					for(int j=0; j<labels.nterms(); j++)
						indices.AddTerm(labels.Index(labels2[j]));
					//std::cout << indices << std::endl;
				}
							
				if(!i)
				{
					CVector<double> column(chain.nterms());
					if(Mixing.cols())
						for(int j=0; j<chain.nterms(); j++)
							if(chain[j].nbranches() == 1 and chain[j].Branch(0).label().find("+") != std::string::npos)
								column[j] = Mixing[indices[j]];
					Mix = column;
				}
				else
				{
					CVector<double> column(std::max(Mix.rows(),chain.nterms()));
					//std::cout << "column = " << std::endl << column << std::endl;
					int l=0, m=0;
					for(int j=0; j<prevchain.nterms(); j++)
					{
						if(prevchain[j].nbranches() == 0)
							l++;
						else
							for(int k=0; k<prevchain[j].nbranches(); k++)
							{
								if(prevchain[j].Branch(k).nbranches() == 1 and prevchain[j].Branch(k).Branch(0).label().find("+") != std::string::npos)
									column[l] = Mixing[indices[m++]];
								if(k)
									Mix.InsertRow(l,Mix.Row(l-1));
								l++;
							}
					}
					Mix.InsertColumn(i,column);
				}
			
				prevchain = chain;
				
				//std::cout << "Mix = " << std::endl << Mix << std::endl;
			}
			
			return Mix;
		}
		catch (...)
		{
			throw;
		}
	}
	
	/* Overloaded == operator */
	bool RGE::operator==(const RGE &rge) const
	{
		return (bSUSY() == rge.bSUSY() and bSM() == rge.bSM() and Mixing().Abs() == rge.Mixing().Abs());
	}
	
	/* Overloaded != operator */
	bool RGE::operator!=(const RGE &rge) const
	{
		 return !(*this == rge);
	}
	
	/* Returns the json string */
	JSONNode RGE::json(std::string name) const
	{
		JSONNode json;
		
		json.push_back(JSONNode("ncouplings",_ncouplings));
		json.push_back(_bSUSY.json("bSUSY"));
		json.push_back(_bSM.json("bSM"));
		json.push_back(_Mixing.json("Mixing"));
		
		return json;
	}

	/* Parses a json object into the attributes of the class */
	void RGE::ParseJSON(const JSONNode & json)
	{
		try
		{
			JSONNode::const_iterator i = json.begin();
			
			while (i != json.end()){
		
				// get the node name and value as a string
				std::string node_name = i -> name();

				// find out where to store the values
				if(node_name == "ncouplings") {
					_ncouplings = i->as_int();
				} else if(node_name =="bSUSY") {
					_bSUSY.ParseJSON(*i);
				} else if(node_name == "bSM") {
					_bSM.ParseJSON(*i);
				} else if(node_name == "Mixing") {
					_Mixing.ParseJSON(*i);
				}
				
				//increment the iterator
				++i;
			}
			
		} catch (...)
		{
			throw;
		}
	}
	
	/* Overloaded << operator with theories on the right */
	std::ostream &operator<<(std::ostream &stream, const RGE &rge)
	{
		stream << rge.json().write_formatted();
		return stream;
	}
}