/*
* helper_functions.cc
* Created by T. Gonzalo on 14/01/16
* Last modified on 14/01/16
*/

#include "headers.h"

namespace Tomb
{
  /* Splits a string at either side of the delimitor */
  List<std::string> Strings::split_string(std::string s, char del)
  {
    List<std::string> result;
    size_t current;
    size_t next = -1;
    do
    {
      current = next + 1;
      next = s.find_first_of(del, current);
      result.AddTerm(s.substr(current, next - current));
    } while (next != std::string::npos);
    
    return result;
  
  }
  
  /* Obtains the list of permutations of a list of elements */
  template<typename TYPE> List<List<TYPE> > Combinatorics::permutations(List<TYPE> list, int n, bool repetitions, bool order)
  {
    List<List<TYPE> > result;

    for(typename List<TYPE>::iterator it = list.begin(); it != list.end(); it++)
    {
      if(n ==1) result.AddTerm(*it);
      else
      {
        List<List<TYPE> > perms = permutations(list, n-1);
        for(typename List<List<TYPE> >::iterator it2 = perms.begin(); it2 != perms.end(); it2++)
        {
          if(!repetitions and it2->Index(*it) != -1)
            continue;
          List<TYPE> aux(*it);
          aux.AppendList(*it2);
          if(order)
            aux.Order();
          if(result.Index(aux) == -1)
            result.AddTerm(aux);
        } 
      }
    }
      
    return result;
  }	
  
  /* Declaration for permutations of SimpleGroups */
  template List<List<SimpleGroup> > Combinatorics::permutations(List<SimpleGroup>, int, bool, bool);
  
  
  /* Factorial */
  long int Combinatorics::factorial(int n)
  {
    if(n <= 1)
      return 1;
    return n*Combinatorics::factorial(n-1);
  }
  
  /* Binomial number */
  long int Combinatorics::binomial(int n, int k)
  {
    long int bin=1;
    for(int i=0; i < k; i++)
      bin *= n-i;
    return double(bin)/Combinatorics::factorial(k);
  }
  
  /* Sum of binomial numbers */
  long int Combinatorics::sum_of_binomials(int n, int k)
  {
    long int sum = 0;
    for(int i=0; i<=k; i++)
      sum += Combinatorics::binomial(n,i);
    return sum;
  }
    
  /* Updates the progess bar for the models */
  void Progress::UpdateModelProgress(int step, int nModels, bool noUpdate)
  {
    //static List<int> counters(std::vector<int>(10)); // Vector of counters
    //static List<int> nModelsVec(std::vector<int>(10)); // Vector of number of models
    //double progress = double(counters[step])/nModels; // Counts models per step
    
                int thread_id = omp_get_thread_num();
                int thread_max = omp_get_max_threads();

                //std::cout << "thread id = " << thread_id << " of " << thread_max << " in step " << step << std::endl;

                static std::vector<std::vector<int> > counters(thread_max); // Vector of counters
                static std::vector<std::vector<int> > nModelsVec(thread_max); // Vector of number of models
                double progress = 0, totalcounter = 0;


    if(!nModels) return ;
    
                // Initialze the vectors
                if(counters[thread_id].size() <= step)
                  counters[thread_id].push_back(0);
                if(nModelsVec[thread_id].size() <= step)
                  nModelsVec[thread_id].push_back(0);
    
                //std::cout << counters[thread_id][step] << std::endl;
                //std::cout << nModelsVec[thread_id][step] << std::endl;
                // Set up progress message
    if(counters[thread_id][step] == 0)
    {
      counters[thread_id][step] ++;
      nModelsVec[thread_id][step] = nModels;
    }
    //std::cout << counters[thread_id][step] << std::endl;
                //std::cout << nModelsVec[thread_id][step] << std::endl;
                
                int barWidth = 70;
    
                for(int i=0; i<thread_max; i++)
                {
                  if(counters[i].size() > step)
                  {
                    totalcounter += counters[i][step];
                    progress += double(counters[i][step])/nModels;
                  }
                }
              // std::cout << totalcounter << std::endl;

                if(!thread_id and (!step or step and nModels > 100))
                {
            std::cout <<  "Step " << step << ": [";
      int pos = barWidth * progress;
      for (int i = 0; i < barWidth; ++i)
      {
      if (i < pos) 
        std::cout << "=";
      else if (i == pos) 
        std::cout << ">";
      else 
        std::cout << " ";
      }
      std::cout << "] " << int(progress * 100.0) << " % : " << totalcounter << "/" << nModels << "\r";
      std::cout.flush();

      std::cout << std::endl;
          }

    if(!noUpdate) counters[thread_id][step] ++;

    if(totalcounter >= nModelsVec[thread_id][step])
    {
      counters[thread_id][step] = 0;
      nModelsVec[thread_id][step] = 0;
    }

  }
  
  /* Updates the progess bar for the reps */
  void Progress::UpdateRepProgress(int nreps)
  {
    if(nreps <= 500)
      return ;
    
    static int count = 1; // Counter
  
    // Set up progress message
    if(count == 1)
    {
      std::cout << "Generating reps " << std::endl;;
      std::cout << "[";
    }
    int barWidth = 70;
    int pos = nreps/barWidth;
    
    if(!((count-1)%pos))
        std::cout << ".";
      //else if (i == pos) 
      //	std::cout << ">";
      //else 
    //		std::cout << " ";
    if(count == nreps)
      std::cout << "] " << std::endl;
    std::cout.flush();
  
    count++;
    if(count > nreps)
      count = 1;

  }
}
