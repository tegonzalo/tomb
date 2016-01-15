/*
 * sm.h
 * Created by T. Gonzalo on 13/01/16
 * Last modified on 13/01/16
 */


#ifndef __SM_H
#define __SM_H

#include "headers.h"

namespace Tomb
{
	// Standard Model
	namespace StandardModel
	{
		extern const LieGroup Group;
		
		extern const Field Q;
		extern const Field L;
		extern const Field u;
		extern const Field d;
		extern const Field e;
		extern const Field H;
		
		extern const List<Field> Reps;
	}
}

#endif /* __SM_H */