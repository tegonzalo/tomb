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
	
	/* Updates the progess bar */
	void Progress::UpdateProgress(int step, int nmodels)
	{
		static int count = 1; // Counter
		double progress = double(count)/nmodels; // Counts models per step
		//time = 0;(*Counts run time*)
		//eta = 0;(*Estimated time to completion*)
		//status = "";(*Status message string*)
		//startTime = SessionTime[];(*Start time*)
		//success = 0;(*Number of successful models *)
		
		//time = SessionTime[] - startTime;
		//eta = time*(Length[models] - count + 1)/(count + 1);
   
		// Set up progress message
		if(count == 1)
			std::cout << "Generating models for step " << step << std::endl;
		int barWidth = 70;

		std::cout << "[";
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
		std::cout << "] " << int(progress * 100.0) << " % : " << count << "/" << nmodels << "\r";
		std::cout.flush();

		std::cout << std::endl;
	
		count++;
		if(count > nmodels)
			count = 1;
  
//   ];
/*

    {"Elapsed Time", Dynamic[DisplayTime[time]]},
    {"ETA for step", Dynamic[DisplayTime[eta]]},
    {"Successful Models", Dynamic[ToString[success]]})*/
	}
}