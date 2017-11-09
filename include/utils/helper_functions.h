/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
* \file
* helper_functions.h
*
* \author
* T. Gonzalo (t.e.gonzalo@fys.uio.no)
*
* \date
* 14/01/2016
*/

#ifndef __HELPER_FUNCTIONS_H
#define __HELPER_FUNCTIONS_H

#include <omp.h>
#include "linkedlists.h"

/*********************************/
/* Helper Functions declarations */
/*********************************/

namespace Tomb
{
  namespace Strings
  {
    List<std::string> split_string(std::string, char);
  
  }
  
  namespace Combinatorics
  {
    template<typename TYPE> List<List<TYPE> > permutations(List<TYPE>, int, bool = true, bool = false);
    
    long int factorial(int);
    
    long int binomial(int, int);
    
    long int sum_of_binomials(int, int);
  }
  
  namespace Progress
  {
    static long int success = 0;
    
    void UpdateModelProgress(int, int, bool = false);
    
    void UpdateRepProgress(int);
  }

}

/**************************************/
/**** Helper Functions definitions ****/
/* (only those required by templates) */
/**************************************/

namespace Tomb
{

  /* Obtains the list of permutations of a list of elements */
  template<typename TYPE> List<List<TYPE> > Combinatorics::permutations(List<TYPE> list, int n, bool repetitions, bool order)
  {
    List<List<TYPE> > result;
 
    if(std::is_same<TYPE, int>::value)
    {
      for(typename List<TYPE>::iterator it = list.begin(); it != list.end(); it++)
      {
        if(n ==1) result.AddTerm(*it);
        else
        {
          List<List<TYPE> > perms = permutations(list, n-1);
          for(auto it2 = perms.begin(); it2 != perms.end(); it2++)
          {
            if(!repetitions and it2->Index(*it) != -1)
              continue;
            List<TYPE> aux(*it);
            aux.AppendList(*it2);
            if(result.Index(aux) == -1)
              result.AddTerm(aux);
          }
        }
      }
     }
     else
     {
       List<int> intlist;
       for(int i = 0; i < list.nterms(); i++)
         intlist.AddTerm(i);
       List<List<int> > intperms = permutations(intlist, n, repetitions, order);
       for(auto it = intperms.begin(); it != intperms.end(); it++)
       {
         List<TYPE> aux;
         for(auto it2 = it->begin(); it2 != it->end(); it2++)
         {
           aux.AddTerm(list[*it2]);
         }
         if(order)
           aux.Order();
         result.AddTerm(aux);
       }
     }

    return result;
  }
}

#endif /* __HELPER_FUNCTIONS_H */
