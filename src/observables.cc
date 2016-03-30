/*
 * observables.cc
 * Created by T. Gonzalo on 30/03/16
 * Last modified on 30/03/16
 */

#include "headers.h"

/**************************************************/
/************Class Observables methods*************/
/**************************************************/

namespace Tomb
{
	namespace Observables
	{

		/* Proton decay */
		bool ProtonDecay(List<Field> &Fields) {

			try {
				List<Field> PDFields;
				PDFields.AddTerm(Field(Rrep("(1,0,2,-1/3)A2xA1xU1"), Rrep("(0,0)A1xA12")));
				PDFields.AddTerm(Field(Rrep("(1,0,0,-1/3)A2xA1xU1"), Rrep("(0,0)A1xA12")));
				PDFields.AddTerm(Field(Rrep("(1,0,0,-4/3)A2xA1xU1"), Rrep("(0,0)A1xA12")));
				PDFields.AddTerm(Field(Rrep("(0,1,2,1/3)A2xA1xU1"), Rrep("(0,0)A1xA12")));
				PDFields.AddTerm(Field(Rrep("(0,1,0,1/3)A2xA1xU1"), Rrep("(0,0)A1xA12")));
				PDFields.AddTerm(Field(Rrep("(0,1,0,4/3)A2xA1xU1"), Rrep("(0,0)A1xA12")));
				
				for(auto it = PDFields.begin(); it != PDFields.end(); it++)
					if(Fields.Index(*it) != -1)
						return true;
				
			} catch (...) {
				throw;
			}
		}
	}
}