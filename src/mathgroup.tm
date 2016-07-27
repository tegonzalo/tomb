:Begin:
:Function:       getGroup
:Pattern:        GetGroup[rank_Integer, type_String]
:Arguments:      {rank, type}
:ArgumentTypes:  {Integer, String}
:ReturnType:     Manual
:End:


:Begin:
:Function: 	getGroup
:Pattern:	GetGroup[id_String]
:Arguments:	{id}
:ArgumentTypes:	{String}
:ReturnType:	Manual
:End:
:Evaluate: GetGroup::usage = "GetGroup[rank, type] gets the information of the group defined by rank and type.
                GetGroup[id] gets the information of the group defined by id"

:Begin:
:Function:       getReps
:Pattern:        GetReps[id_String, maxdim_Integer]
:Arguments:      {id, maxdim}
:ArgumentTypes:  {String, Integer}
:ReturnType:     Manual
:End:
:Evaluate: GetReps::usage = "GetReps[id, maxdim] gets the representations of the group identified by id up to dimension maxdim."

:Begin:
:Function:       getSubgroups
:Pattern:        GetSubgroups[id_String]
:Arguments:      {id}
:ArgumentTypes:  {String}
:ReturnType:     Manual
:End:
:Evaluate: GetSubgroups::usage = "GetSubgroups[id] gets the subgroups of the group identified by id."

:Begin:
:Function:       getRep
:Pattern:        GetRep[id_String]
:Arguments:      {id}
:ArgumentTypes:  {String}
:ReturnType:     Manual
:End:

:Begin:
:Function:	getRep
:Pattern:	GetRep[dimId_String, groupId_String]
:Arguments:	{dimId, groupId}
:ArgumentTypes:	{String, String}
:ReturnType:	Manual
:End:
:Evaluate: GetRep::usage = "GetRep[id] gets the information of the rep defined by id.
              GetRep[dimId, groupId] gets the information of the first rep of dimension dim of the group defined by groupId" 

:Begin:
:Function:	getSubgroup
:Pattern:	GetSubgroup[id_String]
:Arguments:	{id}
:ArgumentTypes:	{String}
:ReturnType:	Manual
:End:

:Begin:
:Function:	getSubgroup
:Pattern:	GetSubgroup[id_String, MixingId_String]
:Arguments:	{id, MixingId}
:ArgumentTypes:	{String, String}
:ReturnType:	Manual
:End:
:Evaluate: GetSubgroup::usage = "GetSubgroup[id] gets the information of the subgroup defined by id and GetSubgroup[id, MixingId] gets the information of the subgroup defined by id with the mixing MixingId"

:Begin:
:Function:		getBreakingChains
:Pattern:		GetBreakingChains[SuperId_String, SubId_String]
:Arguments:		{SuperId, SubId}
:ArgumentTypes:	{String, String}
:ReturnType:	Manual
:End:
:Evaluate: GetBreakingChains::usage = "GetBreakingChains[SuperId,SubId] gets all the breaking chains from the group defined by SuperId to the subgroup defined by SubId"

:Begin:
:Function:		getDecomposeRep
:Pattern:		GetDecomposeRep[RepId_String, SubgroupId_String, MixingId_String]
:Arguments:		{RepId, SubgroupId, MixingId}
:ArgumentTypes:	{String, String, String}
:ReturnType:	Manual
:End:
:Evaluate: GetDecomposeRep::usage = "GetDecomposeRep[RepId,SubgroupId,MixingId] decomposes the rep defined by RepId into the reps of the group defined by SubgroupId and the mixing matrix in MixingId"

:Begin:
:Function:		getWeights
:Pattern:		GetWeights[RepId_String]
:Arguments:		{RepId}
:ArgumentTypes:		{String}
:ReturnType:		Manual
:End:
:Evaluate: GetWeights::usage = "GetWeights[RepId] gets the weights from the representation defined by RepId"

:Begin:
:Function:		getDirectPoduct
:Pattern:		GetDirectProduct[RepId1_String,RepId2_String]
:Arguments:		{RepId1, RepId2}
:ArgumentTypes:		{String, String}
:ReturnType:		Manual
:End:
:Evaluate: GetDirectProduct::usage = "GetDirectProduct[RepId1,RepId2] calculates the direct product of the representations defined by RepId1 and RepId2"

:Begin:
:Function:		getInvariants
:Pattern:		GetInvariants[RepsId_String,dim_Integer]
:Arguments:		{RepsId, dim}
:ArgumentTypes:		{String, Integer}
:ReturnType:		Manual
:End:
:Evaluate: GetInvariants::usage = "GetInvariants[RepsId,dim] builds the group invariants using the reps defined by RepsId up to dimension dim"