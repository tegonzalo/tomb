/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * sm.h
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 13/01/2016
 */

#ifndef __SM_H
#define __SM_H

#include "liegroup.h"
#include "field.h"

/*******************************/
/* Standard Model declarations */
/*******************************/

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
