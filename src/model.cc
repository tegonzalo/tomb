/*
 * model.cc
 * Created by T. Gonzalo on 12/01/16
 * Last modified on 12/01/16
 */

#include "headers.h"

/**************************************************/
/************Class Model methods*************/
/**************************************************/

namespace Tomb
{
	// Definition of static variable
	List<List<Model> > Model::DataBase;
	
	// Member functions

	/* Constructor 0 */
	Model::Model() : List<Theory>() {

		try {
			_nsteps = 0;
			_scales = List<double>();
			
		} catch (...) {
			throw;
		}
	}
	
	/* Constructor 1 */
	Model::Model(Theory theory) : List<Theory>(theory) {

		try {
			_nsteps = 1;
			_scales = List<double>();
			
		} catch (...) {
			throw;
		}
	}

	/* Copy constructor 1 */
	Model::Model(const Model &M) : List<Theory>(M) {
		
		try {
			_nsteps = M.nsteps();
			_scales = M.scales();
			
		} catch (...) {
			throw;
		}
	}
	
	/* Copy constructor 2 */
	Model::Model(const List<Theory> &M) : List<Theory>(M) {
		
		try {
			_nsteps = M.nterms();
			_scales = List<double>();
			
		} catch (...) {
			throw;
		}
	}

	/* Copy constructor 3, with a string */
	Model::Model(std::string s) 
	{
		try
		{
			ParseJSON(libjson::parse(s));
		}
		catch (...)
		{
			throw;
		}
	}
	
	/* Copy constructor 4, with a JSON node */
	Model::Model(const JSONNode &json)
	{
		try
		{
			ParseJSON(json);
		}
		catch (...)
		{
			throw;
		}
	}
	/* Move constructor */
	Model::Model(Model && M) :
		List<Theory>(std::move(M)),
		_nsteps(std::move(M._nsteps)),
		_scales(std::move(M._scales))
	{
		
		try {
			M._nsteps = 0;
			M._scales.Clear();
			
		} catch (...) {
			throw;
		}
	}

	/* Destructor */
	Model::~Model() {
	}

	/* Assignment operator 1 */
	Model &Model::operator=(const Model &M) {
		
		try {
			if(this == &M) return *this;
			   
			List<Theory>::operator=(M);
			
			_nsteps = M.nsteps();
			_scales = M.scales();
			
			return *this;
		} catch (...) {
			throw;
		}
	}

	/* Assignment operator 2 */
	Model &Model::operator=(const List<Theory> &M) {
		
		try {
			if(this == &M) return *this;
			   
			List<Theory>::operator=(M);
			
			_nsteps = M.nterms();
			_scales.Clear();
			
			return *this;
		} catch (...) {
			throw;
		}
	}
	
	/* Move assignment operator */
	Model &Model::operator=(Model &&M) {
		
		try {
			if(this == &M) return *this;
			
			List<Theory>::operator=(std::move(M));
			
			_nsteps = std::move(M._nsteps);
			_scales = std::move(M._scales);
			
			M._nsteps = 0;
			M._scales.Clear();
			
			return *this;
		} catch (...) {
			throw;
		}
	}
	
	
	/* NSteps getter */
	int Model::nsteps() const {
		return _nsteps;
	}
	
	/* Scales getter */
	List<double> Model::scales() const {
		return _scales;
	}
	
	/* Delete term from the model */
	void Model::DeleteTerm(int i)
	{
		try
		{
			List<Theory>::DeleteTerm(i);
			_nsteps--;
		} catch (...)
		{
			throw;
		}
	}
	
	/* Add term to the model */
	void Model::AddTerm(const Theory &theory)
	{
		try
		{
			List<Theory>::AddTerm(theory);
			_nsteps++;
		} catch (...)
		{
			throw;
		}
	}
	
	/* Generate the models */
	void Model::generateModels(int nReps, int startAt) {
		
		try {
			Theory theory = GetObject(0);
			
			Chain chain = theory.BreakingChain();
			Chain subchain = chain;
			int depth = chain.depth();
			
			LieGroup group = theory.Group();
			LieGroup subgroup = group;
			List<SubGroup> subgroups = chain.extractSubgroups();
			
			//List<Model> models;
			//models.AddTerm(Model(theory)));
			List<std::string> models;
			models.AddTerm(Model(theory).json().write_formatted());
    
			// Count the number of successes
			int success = 0;
			
			for(int l = 0; l < depth; l++) { // l
   
				int step = l;
				//std::cout << "step = " << step << std::endl;
   
				chain = subchain;
				group = LieGroup(subgroup);
   
				//List<Model> newmodels;
				List<std::string> newmodels;
				
				if(!models.nterms())
					std::cout << "No models for step " << step << ", exiting..." << std::endl;
      
				//for(List<Model>::const_iterator i = models.begin() + startAt; i != models.end(); i++) {
				for(List<std::string>::const_iterator i = models.begin() + startAt; i != models.end(); i++) {
      
					// Iterate for the number of models
					Model model(*i);
					//std::cout << "model = " <<*i << std::endl;
					theory = model.GetObject(-1);
					List<Field> fields = theory.Fields();
					
					// Update progress
					Progress::UpdateModelProgress(step, models.nterms());
      
					// Check constraints
					if(!theory.chirality())
					{
						std::cerr << "Theory is not chiral" << std::endl;
						continue;
					}
					
					// Check if it is last step, and if so, check for SM content and exit
					if(chain.depth() == 1 and group == StandardModel::Group) { 
						//std::cout << "last step" << std::endl;
						
						if(theory.containsSM())
						{
							//std::cout << "contains sm" << std::endl;
							// Check for anomaly cancellation and proton decay
							theory.calculateAnomaly();
							//std::cout << "anomlay free = " << theory.anomalyFree() << std::endl;
							//model = ProtonDecay[model];

							// Update the model and add to the models list
							model.DeleteTerm(-1);
							model.AddTerm(theory);
							//std::cout << model << std::endl;
							//if(newmodels.Index(model) == -1)
							//{
								//newmodels.AddTerm(model);
							
							// For every successful model calculate the rges and store in the databases
							RGE rges(model);
							//std::cout << rges << std::endl;
							int index = 0;
							if((index = RGE::DataBase.Index(rges)) == -1)
							{
								index = RGE::DataBase.nterms();
								RGE::DataBase.AddTerm(rges);
							}
							//std::cout << RGE::DataBase << std::endl;
							if(Model::DataBase.nterms() <= index)
								Model:DataBase.AddTerm(List<Model>());
							if(Model::DataBase[index].Index(model) == -1)
							{
								Model::DataBase[index].AddTerm(model);
							
								// Update the success counter
								success++;
							}
							//}
							
						}
						
						continue;
					}
      
      
					// If it is not the last step, calculate the subgroup and subchain
					subgroup = subgroups.GetObject(l+1);
					//std::cout << "subgroup = " << subgroup << std::endl;
					
					// Build the subchain
					subchain.Clear();
					for(int j=0; j<chain.nterms(); j++)
						subchain.AppendList(chain.GetObject(j).Branches());
					subchain.calculateDepth();
					
					//Calculate the breaking, obtaining the subreps and mixings at the end
					List<RVector<double> > mixings;
					List<Sum<Field> > subreps = theory.calculateBreaking(mixings);
										
					// Loop over number of subreps, i.e. breaking options 
					for(int k=0; k<subreps.nterms(); k++) {
						List<Field> subrep = subreps.GetObject(k);
						Theory subtheory = Theory(subgroup, subchain, subrep);
						Theory newtheory(theory);
						Model newmodel(model);
						//std::cout << newmodel << std::endl;

						// Check if the next step is last step, and if so, check for SM content and normalise
						if(subchain.depth() == 1 and subrep.GetObject(0).Group() == StandardModel::Group)
						{
							if(subtheory.containsSM())
							{
								double norm = subtheory.normaliseToSM();
								subrep = subtheory.Fields();
								
								// Add normalisation, mixing and anomaly to the previous step (ONLY WORKS FOR THE STEP BEFORE SM)
								if(mixings.nterms() > k and norm != 1)
								{
									newtheory.setMixing(mixings.GetObject(k));
									norm = newtheory.normaliseMixing(norm);
								}
								//std::cout << newtheory << std::endl;
								newtheory.normaliseReps(norm);

								
							} else
								continue;
						}
						// Calculate the anomaly contribution of the newly renormalised reps
						newtheory.calculateAnomaly();
     
						// Replace the last theory of the model with the current theory
						if(newmodel.GetObject(-1) != newtheory)
						{
							newmodel.DeleteTerm(-1);
							newmodel.AddTerm(newtheory);
						}
						
						//Generate all possible combiations of reps
						//std::cout << "Calculating possible reps..." << std::endl;
						// Pasted from the theory and linkedlists code to hopefully speed up things
						/**************************************************************/
						List<Field> scalars = subtheory.getScalars();
						
						// If there are less than 10 reps, don't worry about numbers
						int nreps = scalars.nterms();
						if(scalars.nterms() > 10)
							nreps = nReps;
						
						int total = Combinatorics::sum_of_binomials(scalars.nterms(),nreps);
						
						std::vector<bool> bit_mask(scalars.nterms());
						bool next_bit_mask = false;
						do
						{
							
							Progress::UpdateRepProgress(total);
							
							if(std::count(bit_mask.begin(), bit_mask.end(), true) <= nreps)
							{
								List<Field> subset;
								for(int i=0; i!=bit_mask.size(); i++)
									if(bit_mask[i])
										subset.AddTerm(scalars[i]);
								Model newmodel2(newmodel);
								List<Field> fields(subtheory.getFermions());
								fields.AppendList(subset);
								newmodel2.AddTerm(Theory(subgroup, subchain, fields));
								newmodels.AddTerm(newmodel2.json().write_formatted());
							}
							
							// next_bitmask
							std::size_t i = 0 ;
							for( ; ( i < bit_mask.size() ) && (bit_mask[i] or std::count(bit_mask.begin(),bit_mask.end(),true) >=nreps); ++i )
							{
								bit_mask[i] = false;
							}

							if( i < bit_mask.size())
							{
								if(std::count(bit_mask.begin(), bit_mask.end(), true) < nreps)
										bit_mask[i] = true;
								next_bit_mask = 	true;
								
							}
							else 
								next_bit_mask = false ;
						}
						while(next_bit_mask);
						/***********************************************************************/
						
						//List<List<Field> > possibleReps = subtheory.generatePossibleReps(nReps);
   
						//std::cout << "number of possible reps = " << possibleReps.nterms() << std::endl;

						/*for(List<List<Field> >::iterator it_possreps = possibleReps.begin(); it_possreps != possibleReps.end(); it_possreps ++) 
						{
							Progress::UpdateRepProgress(possibleReps.nterms());
							Model newmodel2(newmodel);
							newmodel2.AddTerm(Theory(subgroup, subchain, *it_possreps));
							newmodels.AddTerm(newmodel2.json().write_formatted());
						}*/
						//std::cout << newmodels << std::endl;
					}
   
				}
			
				models = newmodels;
			}
			
			std::cout << "Number of successful models: " << success << std::endl;
			
			return ;

		} catch (...) {
			throw;
		}
	}
	
	/* Parse a JSON node into a model class */
	void Model::ParseJSON(const JSONNode &json)
	{
		try
		{
			if(nterms())
				Clear();
			
			for(JSONNode::const_iterator it = json.begin(); it != json.end(); it++)
			{
				Theory theory(*it);
				AddTerm(theory);
				
				
			}
			
			_nsteps = nterms();
			
		}
		catch (...) 
		{
			throw;
		}
	}
	
	
	/* Overloaded << operator with models on the right */
	std::ostream &operator<<(std::ostream &stream, const Model &m)
	{
		stream << "{" << std::endl;
		for(Model::const_iterator it = m.begin(); it != m.end(); it++)
			stream << it->json().write_formatted() << "," << std::endl;
		stream << "}";
		return stream;
	}
}