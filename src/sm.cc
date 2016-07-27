/*
* sm.cc
* Created by T. Gonzalo on 13/01/16
* Last modified on 13/01/16
*/


#include "headers.h"

namespace Tomb
{
  // Standard Model
  namespace StandardModel
  {
    extern const LieGroup Group("A2xA1xU1");
    
    extern const Field Q(Rrep("(1,0,1,0.16666667)A2xA1xU1"),Rrep("(1,0)A1xA1"));
    extern const Field L(Rrep("(0,0,1,-0.50000000)A2xA1xU1"),Rrep("(1,0)A1xA1"));
    extern const Field u(Rrep("(0,1,0,-0.66666667)A2xA1xU1"),Rrep("(1,0)A1xA1"));
    extern const Field d(Rrep("(0,1,0,0.33333333)A2xA1xU1"),Rrep("(1,0)A1xA1"));
    extern const Field e(Rrep("(0,0,0,1.00000000)A2xA1xU1"),Rrep("(1,0)A1xA1"));
    extern const Field H(Rrep("(0,0,1,-0.50000000)A2xA1xU1"),Rrep("(0,0)A1xA1"));
    
    extern const List<Field> Reps = std::vector<Field>({Q, Q, Q, L, L, L, u, u, u, d, d, d, e, e, e, H});
  }
}