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
	

	
	/* Generate the models */
	List<Model> Model::generateModels(int nReps, int startAt, bool printToFile) {
		
		try {
			Theory theory = GetObject(0);
			
			Chain chain = theory.BreakingChain();
			Chain subchain = chain;
			int depth = chain.depth();
			
			LieGroup group = theory.Group();
			LieGroup subgroup = group;
			List<SubGroup> subgroups = chain.extractSubgroups();
			
			List<Model> models;
			models.AddTerm(Model(theory));
    
			// Count the number of successes
			int success = 0;
			
			for(int l = 0; l < depth; l++) { // l
   
				int step = l;
				//std::cout << "step = " << step << std::endl;
				int count = 0;
   
				chain = subchain;
				group = LieGroup(subgroup);
   
				List<Model> newmodels;
      
				for(List<Model>::const_iterator i = models.begin() + startAt; i != models.end(); i++) {
      
					// Iterate for the number of models
					Model model = *i;
					//std::cout << "model = " <<*i << std::endl;
					theory = model.GetObject(-1);
					List<Field> fields = theory.Fields();
					
      
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
							//std::cout << theory << std::endl;
							theory.calculateAnomaly();
							//model = ProtonDecay[model];*/
							model.DeleteTerm(-1);
							model.AddTerm(theory);
							if(newmodels.Index(model) == -1)
								newmodels.AddTerm(model);
							success++;
							
							// For every successful model calculate the rges and print to file
	/*rges = getRGEs[gauge][model];
        
        If[OptionValue[PrintFile], 
         PrintToFile[{"Model" -> model, "RGEs" -> rges}]];*/
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
					//std::cout << subreps << std::endl;
										
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
       
/*
       If[
        Length[Select[subrep, Attribute[#]["type"] == "Scalar" &]] > 
         10,
        nreps = OptionValue[NReps];,
        nreps = All;
        ];
       If[Head[mixing] === String,
        possiblereps = GenerateReps[subrep, nreps];,
        possiblereps = GenerateReps[subrep, nreps, mixing];
        ];
  */     
						List<List<Field> > possiblereps;
						possiblereps.AddTerm(subrep);
						//std::cout << "number of possible reps = " << possiblereps.nterms() << std::endl;
						
						for(List<List<Field> >::iterator it_possreps = possiblereps.begin(); it_possreps != possiblereps.end(); it_possreps ++) 
						{
							subtheory = Theory(subgroup, subchain, *it_possreps);
							newmodel.AddTerm(subtheory);
							if(newmodels.Index(newmodel) == -1)
								newmodels.AddTerm(newmodel);
						}
						//std::cout << newmodels << std::endl;
					}
   
				}
			
				models = newmodels;
				
			}
			return models;

		} catch (...) {
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