/*
 * helper_functions.cc
 * Created by T. Gonzalo on 14/01/16
 * Last modified on 14/01/16
 */

#include "headers.h"

namespace Tomb
{
	namespace Strings
	{
		List<std::string> split_string(std::string s, char del)
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
	
	}
	
	namespace Combinatorics
	{
		template<typename TYPE> List<List<TYPE> > permutations(List<TYPE> list, int n, bool repetitions, bool order)
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
		
		
		template List<List<SimpleGroup> > permutations(List<SimpleGroup>, int, bool, bool);
	}
}