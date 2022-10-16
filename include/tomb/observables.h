/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
 * \file
 * observables.h
 *
 * \author
 * T. Gonzalo (t.e.gonzalo@fys.uio.no)
 *
 * \date
 * 30/03/2016
 */

#ifndef __OBSERVABLES_H
#define __OBSERVABLES_H

#include "linkedlists.h"
#include "field.h"

/***************************/
/* Observables declaration */
/***************************/

namespace Tomb
{
  namespace Observables
  {
    bool ProtonDecay(List<Field> &);
		
    bool NNOscillations(List<Field> &);
  }
}

#endif /* __OBSERVABLES_H */
