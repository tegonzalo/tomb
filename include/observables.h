/*
 * observables.h
 * Created by T. Gonzalo on 30/03/16
 * Last modified on 30/03/16
 */


#ifndef __OBSERVABLES_H
#define __OBSERVABLES_H

#include "headers.h"

namespace Tomb
{
	namespace Observables
	{
		bool ProtonDecay(List<Field> &);
		
		bool NNOscillations(List<Field> &);
	}
}

#endif /* __OBSERVABLES_H */