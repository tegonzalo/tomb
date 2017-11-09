/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * model.cc
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 12/01/2016
 */

#include "model.h"

/***************************/
/* Class Model definitions */
/***************************/

namespace Tomb
{
  // Member functions

  /* Constructor 0 */
  Model::Model() : List<Theory>()
  {
    try
    {
      _nsteps = 0;
      _scales = List<double>();
      
    }
    catch (...) { throw; }
  }
  
  /* Constructor 1 */
  Model::Model(Theory theory) : List<Theory>(theory)
  {
    try
    {
      _nsteps = 1;
      _scales = List<double>();
      
    }
    catch (...) { throw; }
  }

  /* Copy constructor 1 */
  Model::Model(const Model &M) : List<Theory>(M)
  {   
    try
    {
      _nsteps = M.nsteps();
      _scales = M.scales();
      
    }
    catch (...) { throw; }
  }
  
  /* Copy constructor 2 */
  Model::Model(const List<Theory> &M) : List<Theory>(M)
  {   
    try
    {
      _nsteps = M.nterms();
      _scales = List<double>();
      
    }
    catch (...) { throw; }
  }

  /* Copy constructor 3, with a string */
  Model::Model(std::string s) 
  {
    try
    {
      ParseJSON(libjson::parse(s));
    }
    catch (...) { throw; }
  }
  
  /* Copy constructor 4, with a JSON node */
  Model::Model(const JSONNode &json)
  {
    try
    {
      ParseJSON(json);
    }
    catch (...) { throw; }
  }

  /* Move constructor */
  Model::Model(Model && M) :
    List<Theory>(std::move(M)),
    _nsteps(std::move(M._nsteps)),
    _scales(std::move(M._scales))
  {    
    try
    {
      M._nsteps = 0;
      M._scales.Clear();
      
    }
    catch (...) { throw; }
  }

  /* Destructor */
  Model::~Model()
  {

  }

  /* Assignment operator 1 */
  Model &Model::operator=(const Model &M)
  {   
    try
    {
      if(this == &M) return *this;
        
      List<Theory>::operator=(M);
      
      _nsteps = M.nsteps();
      _scales = M.scales();
      
      return *this;
    }
    catch (...) { throw; }
  }

  /* Assignment operator 2 */
  Model &Model::operator=(const List<Theory> &M)
  {   
    try
    {
      if(this == &M) return *this;
        
      List<Theory>::operator=(M);
      
      _nsteps = M.nterms();
      _scales.Clear();
      
      return *this;
    }
    catch (...) { throw; }
  }
  
  /* Move assignment operator */
  Model &Model::operator=(Model &&M)
  {   
    try
    {
      if(this == &M) return *this;
      
      List<Theory>::operator=(std::move(M));
      
      _nsteps = std::move(M._nsteps);
      _scales = std::move(M._scales);
      
      M._nsteps = 0;
      M._scales.Clear();
      
      return *this;
    }
    catch (...) { throw; }
  }
  
  
  /* NSteps getter */
  int Model::nsteps() const
  {
    return _nsteps;
  }
  
  /* Scales getter */
  List<double> Model::scales() const
  {
    return _scales;
  }
  
  /* Delete term from the model */
  void Model::DeleteTerm(int i)
  {
    try
    {
      List<Theory>::DeleteTerm(i);
      _nsteps--;
    }
    catch (...) { throw; }
  }
  
  /* Add term to the model */
  void Model::AddTerm(const Theory &theory)
  {
    try
    {
      List<Theory>::AddTerm(theory);
      _nsteps++;
    }
    catch (...) { throw; }
  }
  
  /* Generate models */
  long int Model::generateModels(int nReps, int startAt)
  {   
    try
    {

      double time0 = omp_get_wtime();

      Theory theory = GetObject(-1);
      
      Chain chain = theory.BreakingChain();
      Chain subchain = chain;
      int depth = chain.depth();

      static int maxdepth = 0;
      if(depth > maxdepth) maxdepth = depth;

      if(omp_get_wtime() - time0 > 0.02)
        std::cout << "Mark 1: " << omp_get_wtime() - time0 << std::endl;
      double time1 = omp_get_wtime();
      
      List<string> subgroups;
      if(omp_get_wtime() - time1 > 0.02)
        std::cout << "Mark 1.0: " << omp_get_wtime() - time1 << std::endl;
      double time2 = omp_get_wtime();
      #pragma omp critical
      {
        LieGroup group = theory.Group();
        //std::cout << "Mark 1.1: " << omp_get_wtime() - time2 << std::endl;
        //time2 = omp_get_wtime();
        LieGroup subgroup = group;
        //std::cout << "Mark 1.2: " << omp_get_wtime() - time2 << std::endl;
        //time2 = omp_get_wtime();
        subgroups = chain.extractSubgroups();
        //std::cout << "Mark 1.3: " << omp_get_wtime() - time2 << std::endl;
      }

      if(omp_get_wtime() - time1 > 0.02)
        std::cout << "Mark 2: " << omp_get_wtime() -time1 << std::endl;
      time1 = omp_get_wtime();
      
      // Check constraints
      if(!theory.chirality())
      {
        std::cerr << "Theory is not chiral" << std::endl;
        return 0;
      }

      if(omp_get_wtime() - time1 > 0.02)
        std::cout << "Mark 3: " << omp_get_wtime() - time1 << std::endl;
      time1 = omp_get_wtime();      

      // Calculate the anomaly contribution of the newly renormalised reps
      theory.calculateAnomaly();
  
      if(omp_get_wtime() - time1 > 0.02)
        std::cout << "Mark 4: " << omp_get_wtime() - time1 << std::endl;
      time1 = omp_get_wtime();

      // Replace the last theory of the model with the current theory
      if(GetObject(-1) != theory)
      {
        DeleteTerm(-1);
        AddTerm(theory);
      }
 
      if(omp_get_wtime() - time1 > 0.02)
        std::cout << "Mark 5: " << omp_get_wtime() - time1 << std::endl;
      time1 = omp_get_wtime();

      if(depth == 1)
      {
        if(theory.containsSM())
        {
          //std::cout << "Calculating observables" << std::endl;
          //calculateObservables();
                    
          // For every successful model calculate the rges and store in the databases
          #pragma omp critical
          {
            RGE rges(*this);

            // Set the RGEs and models to the database
            int index = 0;
            string key = "";
            if((key = DB<RGE>().key(rges)) == "")
            {
              index = DB<RGE>().size();
              key = RGE::key(index);
              DB<RGE>().set(key, &rges);
            }
            else
              index = stoi(key.substr(4,4));
            if((key = DB<Model>().key(*this)) == "")
            {
              int index2 = Model::nmodels(index);
              key = Model::key(index,index2); 
              DB<Model>().set(key, this);
            
              // Update the success counter
              Progress::success++;
            }
            
            // If there are too many models already in the DB, flush
            // TODO: Finish this
            if(DB<RGE>().size() > 1000)
            {
             // DB<RGE>().flush();
             // DB<Model>().flush();
            }
          }
       }
        else
          return 0;
        
      }
      else
      {
        // If it is not the last step, calculate the subgroup and subchain
        SubGroup subgroup = subgroups.GetObject(1);
 
        // Build the subchain
        subchain.Clear();
        for(int j=0; j<chain.nterms(); j++)
          subchain.AppendList(chain.GetObject(j).Branches());
        subchain.calculateDepth();
        
        //Calculate the breaking, obtaining the subreps and mixings at the end
        List<RVector<double> > mixings;
        List<Sum<Field> > subreps;
        #pragma omp critical
        subreps = theory.calculateBreaking(mixings);
        
        // Loop over number of subreps, i.e. breaking options 
        for(int k=0; k<subreps.nterms(); k++)
        {
          LieGroup group(subgroup);
          List<Field> subrep = subreps[k];

          Theory subtheory = Theory(group, subchain, subrep);
          Theory newtheory(theory);

          Model model = *this;

          // Check if the next step is last step, and if so, check for SM content and normalise
          if(subchain.depth() == 1 and subrep[0].Group() == StandardModel::Group)
          {
            if(subtheory.containsSM())
            {
              double norm = subtheory.normaliseToSM();
              subrep = subtheory.Fields();
                  
              // Add normalisation, mixing and anomaly to the previous step 
              // TODO : ONLY WORKS FOR THE STEP BEFORE SM
              if(mixings.nterms() > k and norm != 1)
              {
                newtheory.setMixing(mixings.GetObject(k));
                norm = newtheory.normaliseMixing(norm);
              }
              newtheory.normaliseReps(norm);

              if(model.GetObject(-1) != newtheory)
              {
                model.DeleteTerm(-1);
                model.AddTerm(newtheory);
              }
            }
            else
              return 0;
          }

          if(omp_get_wtime() - time1 > 0.02)
            cout << "Mark 6: " << omp_get_wtime() - time1 << std::endl;
          time1 = omp_get_wtime();           
              
          //Generate all possible combiations of reps
          //std::cout << "Calculating possible reps..." << std::endl;
          // Pasted from the theory and linkedlists code to hopefully speed up things
          List<Field> scalars = subtheory.getScalars();
          
          // If there are less than 10 reps, don't worry about numbers
          int nreps = scalars.nterms();
          if(scalars.nterms() > 10)
            nreps = nReps;
          
          int total = Combinatorics::sum_of_binomials(scalars.nterms(),nreps);
            
          std::vector<bool> bit_mask(scalars.nterms());
          
          bool next_bit_mask = false;
          
          // Try getting bit_masks first
          std::vector<std::vector<bool> > bit_masks;
          do
          {
            if(std::count(bit_mask.begin(), bit_mask.end(), true) <= nreps)
            {
              bit_masks.push_back(bit_mask);

              // next_bitmask
              std::size_t i = 0 ;
              for( ; ( i < bit_mask.size() ) && (bit_mask[i] or std::count(bit_mask.begin(),bit_mask.end(),true) >=nreps); ++i )
                bit_mask[i] = false;
              if( i < bit_mask.size())
              {
                if(std::count(bit_mask.begin(), bit_mask.end(), true) < nreps)
                    bit_mask[i] = true;
                next_bit_mask = true;
              
              }
              else 
                next_bit_mask = false ;
            }
          }
          while(next_bit_mask);

          if(omp_get_wtime()-time1 > 0.02)
            cout << "Time for bitmask: " << omp_get_wtime() - time1 << endl;    

          // Parallel for loop
          #pragma omp parallel default(shared)
          {
            #pragma omp for nowait
            for(int i=0; i<bit_masks.size(); i++)
            {
              bit_mask = bit_masks[i];
              
              #pragma omp critical
              Progress::UpdateModelProgress(maxdepth - depth, total*subreps.nterms());

              if(std::count(bit_mask.begin(), bit_mask.end(), true) <= nreps)
              {
                List<Field> subset;
                for(int i=0; i!=bit_mask.size(); i++)
                  if(bit_mask[i])
                    subset.AddTerm(scalars[i]);

                List<Field> fields = subtheory.getFermions();
                fields.AppendList(subset);
                Model newmodel(model);
                newmodel.AddTerm(Theory(subgroup, subchain, fields));

                time1 = omp_get_wtime();
                // Recursive call
                newmodel.generateModels(nReps);
                double time2 = omp_get_wtime();  
                if(time2-time1 > 0.02)
                  cout << "Time in substep = " << time2-time1 << endl;
              }
            }
          }
        }
        
      }
      return Progress::success;
      
    }
    catch (...) { throw; }
  }
  
  /* Calculates the observables for every theory of the model */
  void Model::calculateObservables()
  {
/*    try
    {
      for(int i=0; i < nterms(); i++)
      {
          
        List<Field> fields(GetObject(i).getScalars()), auxfields;
        
        if(GetObject(i).Group() != StandardModel::Group)
        {
          List<SubGroup> subgroups = GetObject(i).BreakingChain().extractSubgroups();
          
          // Calculate the gauge reps for the Lie Group, and cast them to scalar for convenience
          List<Rrep> adjoints = GetObject(i).Group().AdjointReps();
          for(auto adj = adjoints.begin(); adj != adjoints.end(); adj++)
            fields.AddTerm(Field(*adj,"Vector"));
          
          // Loop over the subgroups to decompose down the SM group
          for(int j=1; j < subgroups.nterms(); j++)
          {
            SubGroup newsubgroup(subgroups.GetObject(j));
            
            if(GetObject(i+j-1).Mixing().cols())
              newsubgroup = SubGroup(newsubgroup.id(), GetObject(i+j-1).Mixing());
            if(GetObject(i+j-1).norm() != 1)
              fields = Theory::normaliseReps(fields,GetObject(i+j-1).norm());
            
            auxfields = fields;
            fields.Clear();
            for(auto fi = auxfields.begin(); fi != auxfields.end(); fi++)
              fields.AppendList(fi->Decompose(newsubgroup));
            
          }
          
          //std::cout << "fields = " << fields << std::endl;
          
        }
        
        
        // Update this theory
        Theory theory(GetObject(i));
        theory.calculateObservables(fields);
        
        DeleteTerm(i);
        InsertTerm(i, theory);
      }
    }
    catch (...) { throw; }*/
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
    catch (...) { throw; }
  }

  /* Returns the number of models with the RGE index */
  int Model::nmodels(int index)
  {
    stringstream skey;
    skey << "Model_" << setfill('0') << setw(4) << index << "_";
    for(int i=0; i<DB<Model>().size(); i++)
    {
       stringstream skey2;
       skey2 << skey.str();
       skey2 << setfill('0') << setw(4) << i;
       if(!DB<Model>().check(skey2.str()))
         return i;
    }
    return DB<Model>().size();
  }

  /* Create the key for the Model from RGE and model indices */
  string Model::key(int index1, int index2)
  {
    stringstream skey;
    skey << "Model_" << setfill('0') << setw(4) << index1 << "_";
    skey << setfill('0') << setw(4) << index2;
    return skey.str();
  }
}
