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
    
			for(int l = 0; l < depth; l++) { // l
   
				int step = l;
				std::cout << "step = " << step << std::endl;
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
//					if(model.checkChirality())
//						continue;
					
					// Check if it is last step, and if so, check for SM content and exit
					if(chain.depth() == 1 and group == StandardModel::Group) { 
						std::cout << "last step" << std::endl;
						
						if(theory.containsSM())
						{
							// Check for anomaly cancellation and proton decay
	/*anomaly = AnomalyCancellation[reps];
        AppendTo[model[[-1]], "AnomalyFree" -> anomaly];
        model = ProtonDecay[model];*/
							model.DeleteTerm(-1);
							model.AddTerm(theory);
							if(newmodels.Index(model) == -1)
								newmodels.AddTerm(model);
							//success++;
							
							// For every successful model calculate the rges and print to file
	/*rges = getRGEs[gauge][model];
        
        If[OptionValue[PrintFile], 
         PrintToFile[{"Model" -> model, "RGEs" -> rges}]];*/
						} 
						exit(0);			
						continue;
					}
      
      
					// If it is not the last step, calculate the subgroup and subchain
					subgroup = subgroups.GetObject(l+1);
					
					// Build the subchain
					subchain.Clear();
					for(int j=0; j<chain.nterms(); j++)
						subchain.AppendList(chain.GetObject(j).Branches());
					subchain.calculateDepth();
					
					//Calculate the breaking, obtaining the subreps at the end
					List<RVector<double> > mixings;
					List<Sum<Field> > subreps = theory.calculateBreaking(mixings);
					//std::cout << subreps << std::endl;
					
					// Loop over number of subreps, i.e. breaking options 
					for(int k=0; k<subreps.nterms(); k++) {
						
						List<Field> subrep = subreps.GetObject(k);
						RVector<double> mixing = mixings.GetObject(k);
  //   norm = 1;
						// Check if the next step is last step, and if so, check for SM content
						if(subchain.depth() == 1 and subrep.GetObject(0).Group() == StandardModel::Group)
						{
 /*If[
                
        If[ContainsSM[subrep],
          (*Print["is sm"];*)
          If[OptionValue[KeepTiming],
           time3 = SessionTime[];
           
           If[time3 - time2 > maxinstime, maxinstime = time3 - time2; 
            maxinstimebit = "ContainsSM"];
           Print["ContainsSM: ", time3 - time2];
           ];
          
          {subrep, norm} = NormaliseRepstoSM[subrep];
          
          
          If[OptionValue[PrintOut], Print["CONTAINS SM"]];,
          If[OptionValue[PrintOut],
           Print["NO SM: MODEL UNSUCCESSFUL"];
           ];
          Continue[];
          ];
        ];*/
						}
            
      /*

       
       
       
       
       
       
       (* Add normalisation, 
       mixing and anomaly to the previous step (ONLY WORKS FOR THE \
STEP BEFORE SM) *)
       
       If[Length[StringCases[group, "U1"]] > 0 && norm != 1,
        If[! (Head[mixing] === String),
         {mixing, norm} = 
           NormaliseMixing[group, subgroup, mixing, norm];
         ];
        
        newreps = NormaliseReps[reps, norm];

        model = Delete[model, -1];
        model = 
         Append[model, NewTheory[group, chain, newreps, norm]];
        ];
       
       If[! (Head[mixing] === String),
        AppendTo[model[[-1]], "Mixing" -> mixing];
        ];

       (* 
       Calculate the anomaly contribution of the newly renormalised \
reps *)
       anomaly = AnomalyCancellation[reps];
       AppendTo[model[[-1]], "AnomalyFree" -> anomaly];
       
       If[OptionValue[KeepTiming],
        time3 = SessionTime[];
        If[time3 - time2, maxinstime, maxinstime = time2 - time0; 
         maxinstimebit = "/anomaly"];
        Print["anomaly: ", time3 - time2];
        ];
  */     
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
						std::cout << "number of possible reps = " << possiblereps.nterms() << std::endl;
						
						for(List<List<Field> >::iterator it_possreps = possiblereps.begin(); it_possreps != possiblereps.end(); it_possreps ++) 
						{
							theory = Theory(subgroup, subchain, *it_possreps);
							std::cout <<theory << std::endl;
							Model newmodel(model);
							newmodel.AddTerm(theory);
							if(newmodels.Index(newmodel) == -1)
								newmodels.AddTerm(newmodel);
/* 
       Table[
        theory = 
         NewTheory[NewGroup[subgroup], subchain, 
          "Reps" /. possiblereps[[j]]];
        Sow[Append[model, theory]];,
        {j, Length[possiblereps]}
        ];,
	*/
						}
						std::cout << newmodels << std::endl;
					}
					/*
       {k, Length[subreps]}];,
      {i, startat, Length[models]}]
     ];
   
   If[Length[models[[2]]] > 0,
    models = DeleteDuplicates[models[[2, 1]]];,
    If[OptionValue[PrintOut], Print["NO SUCCESSFUL MODELS"]];
    models = {};
    Break[];
    ];,
	*/			}
			
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