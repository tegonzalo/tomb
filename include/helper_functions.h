/*
 * helper_functions.h
 * Created by T. Gonzalo on 14/01/16
 * Last modified on 14/01/16
 */

#ifndef __HELPER_FUNCTIONS_H
#define __HELPER_FUNCTIONS_H

#include "headers.h"

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
		void UpdateModelProgress(int, int);
		
		void UpdateRepProgress(int);
	}

}

#endif /* __HELPER_FUNCTIONS_H */