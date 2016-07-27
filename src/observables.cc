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
        List<Field> PDScalars;
        PDScalars.AddTerm(Field(Rrep("(1,0,2,-0.258)A2xA1xU1"), Rrep("(0,0)A1xA1")));
        PDScalars.AddTerm(Field(Rrep("(1,0,0,-0.258)A2xA1xU1"), Rrep("(0,0)A1xA1")));
        PDScalars.AddTerm(Field(Rrep("(1,0,0,-1.033)A2xA1xU1"), Rrep("(0,0)A1xA1")));
        PDScalars.AddTerm(Field(Rrep("(0,1,2,0.258)A2xA1xU1"), Rrep("(0,0)A1xA1")));
        PDScalars.AddTerm(Field(Rrep("(0,1,0,0.258)A2xA1xU1"), Rrep("(0,0)A1xA1")));
        PDScalars.AddTerm(Field(Rrep("(0,1,0,1.033)A2xA1xU1"), Rrep("(0,0)A1xA1")));
        
        List<Field> PDGauge;
        PDGauge.AddTerm(Field(Rrep("(1,0,1,-0.645)A2xA1xU1"), Rrep("(1,1)A1xA1")));
        PDGauge.AddTerm(Field(Rrep("(1,0,1,0.129)A2xA1xU1"), Rrep("(1,1)A1xA1")));
        PDGauge.AddTerm(Field(Rrep("(0,1,1,0.645)A2xA1xU1"), Rrep("(1,1)A1xA1")));
        PDGauge.AddTerm(Field(Rrep("(0,1,1,-0.129)A2xA1xU1"), Rrep("(1,1)A1xA1")));
        
        for(auto it = Fields.begin(); it != Fields.end(); it++)
        {
          if(it->isScalar())
            if(PDScalars.Index(*it) != -1)
              return true;
          else if(it->isVector())
            if(PDGauge.Index(*it) != -1)
              return true;
        }
        
        return false;
        
      } catch (...) {
        throw;
      }
    }
    
    /* Neutron - antineutron oscillations */
    bool NNOscillations(List<Field> &Fields) {

      try {
        List<List<Field>> NNFields;
        NNFields.AddTerm(Field(Rrep("(2,0,2,0.258)A2xA1xU1"), Rrep("(0,0)A1xA1")));
        NNFields.AddTerm(Field(Rrep("(2,0,0,0.258)A2xA1xU1"), Rrep("(0,0)A1xA1")));
        NNFields.AddTerm(Field(Rrep("(2,0,0,1.033)A2xA1xU1"), Rrep("(0,0)A1xA1")));
        NNFields.AddTerm(Field(Rrep("(1,0,2,-0.258)A2xA1xU1"), Rrep("(0,0)A1xA1")));
        NNFields.AddTerm(Field(Rrep("(1,0,0,-0.258)A2xA1xU1"), Rrep("(0,0)A1xA1")));
        NNFields.AddTerm(Field(Rrep("(2,0,0,-0.516)A2xA1xU1"), Rrep("(0,0)A1xA1")));
        NNFields.AddTerm(Field(Rrep("(1,0,0,0.516)A2xA1xU1"), Rrep("(0,0)A1xA1")));
        
        NNFields.AddTerm(Field(Rrep("(0,2,2,-0.258)A2xA1xU1"), Rrep("(0,0)A1xA1")));
        NNFields.AddTerm(Field(Rrep("(0,2,0,-0.258)A2xA1xU1"), Rrep("(0,0)A1xA1")));
        NNFields.AddTerm(Field(Rrep("(0,2,0,-1.033)A2xA1xU1"), Rrep("(0,0)A1xA1")));
        NNFields.AddTerm(Field(Rrep("(0,1,2,0.258)A2xA1xU1"), Rrep("(0,0)A1xA1")));
        NNFields.AddTerm(Field(Rrep("(0,1,0,0.258)A2xA1xU1"), Rrep("(0,0)A1xA1")));
        NNFields.AddTerm(Field(Rrep("(0,2,0,0.516)A2xA1xU1"), Rrep("(0,0)A1xA1")));
        NNFields.AddTerm(Field(Rrep("(0,1,0,-0.516)A2xA1xU1"), Rrep("(0,0)A1xA1")));
        
        std::map<int, List<std::pair<int,int> > > NNmap;
        
        List<std::pair<int, int> > list;
        list.AddTerm(std::pair<int,int>(1,6));
        list.AddTerm(std::pair<int,int>(1,13));
        list.AddTerm(std::pair<int,int>(8,6));
        list.AddTerm(std::pair<int,int>(8,13));
        list.AddTerm(std::pair<int,int>(4,7));
        list.AddTerm(std::pair<int,int>(4,14));
        list.AddTerm(std::pair<int,int>(11,7));
        list.AddTerm(std::pair<int,int>(11,14));
        NNmap.emplace(1,list);
        NNmap.emplace(8,list);
        
        list.Clear();
        list.AddTerm(std::pair<int,int>(2,6));
        list.AddTerm(std::pair<int,int>(2,13));
        list.AddTerm(std::pair<int,int>(9,6));
        list.AddTerm(std::pair<int,int>(9,13));
        list.AddTerm(std::pair<int,int>(5,7));
        list.AddTerm(std::pair<int,int>(5,14));
        list.AddTerm(std::pair<int,int>(12,7));
        list.AddTerm(std::pair<int,int>(12,14));
        NNmap.emplace(2,list);
        NNmap.emplace(9,list);
        
        list.Clear();
        list.AddTerm(std::pair<int,int>(6,6));
        list.AddTerm(std::pair<int,int>(6,13));
        list.AddTerm(std::pair<int,int>(13,13));
        list.AddTerm(std::pair<int,int>(7,7));
        list.AddTerm(std::pair<int,int>(7,14));
        list.AddTerm(std::pair<int,int>(14,14));
        NNmap.emplace(3,list);
        NNmap.emplace(10,list);
        
        list.Clear();
        list.AddTerm(std::pair<int,int>(4,6));
        list.AddTerm(std::pair<int,int>(4,13));
        list.AddTerm(std::pair<int,int>(11,6));
        list.AddTerm(std::pair<int,int>(11,13));
        list.AddTerm(std::pair<int,int>(4,7));
        list.AddTerm(std::pair<int,int>(4,14));
        list.AddTerm(std::pair<int,int>(11,7));
        list.AddTerm(std::pair<int,int>(11,14));
        NNmap.emplace(4,list);
        NNmap.emplace(11,list);
        
        list.Clear();
        list.AddTerm(std::pair<int,int>(5,6));
        list.AddTerm(std::pair<int,int>(5,13));
        list.AddTerm(std::pair<int,int>(12,6));
        list.AddTerm(std::pair<int,int>(12,13));
        list.AddTerm(std::pair<int,int>(5,7));
        list.AddTerm(std::pair<int,int>(5,14));
        list.AddTerm(std::pair<int,int>(12,7));
        list.AddTerm(std::pair<int,int>(12,14));
        NNmap.emplace(5,list);
        NNmap.emplace(12,list);
        
        /*for(auto it = Fields.begin(); it != Fields.end(); it++)
        {
          int index = -1;
          if((index = NNFields.Index(*it)) != -1)
          {
            List<std::pair<int, int> > pairs = NNmap.at(index);
            for(int k=0; k < pairs.nterms(); k++)
            {
              if(Fields.Index(NNFields[pairs[k].first]) != -1 and Fields.Index(NNFields[pairs[k].second]) != -1)
                return true;
            }
          }
        }*/
        
        return false;
        
      } catch (...) {
        throw;
      }
    }
  }
}