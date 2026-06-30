/// \file SteppingAction.cc
/// \brief Implementation of the QCRNG::SteppingAction class

#include "SteppingAction.hh"

#include "DetectorConstruction.hh"
#include "EventAction.hh"

#include "G4Event.hh"
#include "G4LogicalVolume.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"

namespace QCRNG
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(EventAction* eventAction) : fEventAction(eventAction) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step){
  if(!fScoringVolume){
    const auto detConstruction = static_cast<const DetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume = detConstruction->GetScoringVolume();
  }

  // get volume of the current step
  G4LogicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

  // check if we are in scoring volume
  if (volume != fScoringVolume)
    return;

  // collect energy deposited in this step
  auto edep = step->GetTotalEnergyDeposit();
  auto time = step->GetPreStepPoint()->GetGlobalTime();
  auto det  = step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber();
  if(edep>0){
    //    G4cout << "event " << G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID()
    //	   << " det " << det << " edep " << edep/keV << " keV time " << time/ns << " ns" << G4endl;
    auto eventAction = static_cast<EventAction*>(G4EventManager::GetEventManager()->GetUserEventAction());

    eventAction->AddHit(det, edep, time);
  }
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace QCRNG
