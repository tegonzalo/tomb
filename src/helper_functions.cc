/*
 * helper_functions.cc
 * Created by T. Gonzalo on 14/01/16
 * Last modified on 14/01/16
 */

#include "linkedlists.h"

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
}