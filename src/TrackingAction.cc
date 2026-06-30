#include "TrackingAction.hh"
#include "OutputManager.hh"

#include "G4Track.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4VProcess.hh"

namespace QCRNG{

  void TrackingAction::PreUserTrackingAction(const G4Track* track){
    int eventID = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();

    int trackID = track->GetTrackID();
    int parentID = track->GetParentID();
    int pdg = track->GetParticleDefinition()->GetPDGEncoding();
    std::string creator = "primary";
    if(track->GetCreatorProcess())
      creator = track->GetCreatorProcess()->GetProcessName();

    double E = track->GetKineticEnergy()/keV;
    auto p = track->GetMomentumDirection();
    double t = track->GetGlobalTime()/ns;

    OutputManager::Instance().FillTruth(eventID, trackID, parentID, pdg, creator, E, p.x(), p.y(), p.z(), t);
  }

}
