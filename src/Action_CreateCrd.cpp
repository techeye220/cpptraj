#include "Action_CreateCrd.h"
#include "CpptrajStdio.h"

Action_CreateCrd::Action_CreateCrd() {}

void Action_CreateCrd::Help() {
  mprintf("createcrd [<name>] [ parm <name> | parmindex <#> ]\n");
}

Action::RetType Action_CreateCrd::Init(ArgList& actionArgs, TopologyList* PFL, FrameList* FL,
                                       DataSetList* DSL, DataFileList* DFL, int debugIn)
{
  // Keywords
  Topology* parm = PFL->GetParm( actionArgs );
  if (parm == 0) {
    mprinterr("Error: createcrd: No parm files loaded.\n");
    return Action::ERR;
  }
  pindex_ = parm->Pindex();
  // DataSet
  coords_ = (DataSet_Coords*)DSL->AddSet(DataSet::COORDS, actionArgs.GetStringNext(), "CRD");
  if (coords_ == 0) return Action::ERR;
  // Do not set topology here since it may be modified later.

  mprintf("    CREATECRD: Saving coordinates from Top %s to %s\n",
          parm->c_str(), coords_->Legend().c_str());
  return Action::OK;
}

Action::RetType Action_CreateCrd::Setup(Topology* currentParm, Topology** parmAddress) {
  // Set COORDS topology now if not already set.
  if (currentParm->Pindex() == pindex_ && coords_->Top().Natom() == 0)
    coords_->SetTopology( *currentParm );
  return Action::OK;
}

Action::RetType Action_CreateCrd::DoAction(int frameNum, Frame* currentFrame, 
                                           Frame** frameAddress) 
{
  coords_->AddFrame( *currentFrame );
  return Action::OK;
}
