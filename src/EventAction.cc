/// \file EventAction.cc
/// \brief Implementation of the QCRNG::EventAction class

#include "EventAction.hh"
#include "RunAction.hh"
#include "OutputManager.hh"
#include "QCRNGConfig.hh"

#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

#include "Randomize.hh"

namespace QCRNG{

  EventAction::EventAction(RunAction* runAction) : fRunAction(runAction) {}

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void EventAction::BeginOfEventAction(const G4Event* event){
    fDetEnergy.clear();
    fDetTime.clear();
    double t = event->GetPrimaryVertex()->GetT0()/ns;

    OutputManager::Instance().FillEvent(event->GetEventID(), t, QCRNGConfig::Instance().activity/(1./second));
    
    if(QCRNGConfig::Instance().verbose > 1)
      G4cout << "event " << event->GetEventID() << " time " << event->GetPrimaryVertex()->GetT0()/ns << " ns" << G4endl;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void EventAction::EndOfEventAction(const G4Event* event){
    
    double decayTime = event->GetPrimaryVertex()->GetT0();
    auto& cfg = QCRNGConfig::Instance();
    
    for(auto& hit : fDetEnergy){
      int det = hit.first;
      
      double e_true = hit.second;
      double tabs_true = fDetTime[det];
      double t_true = tabs_true - decayTime;
      
      double sigmaE = cfg.energyResolution * e_true / 2.355;
      double e_meas = G4RandGauss::shoot(e_true, sigmaE);
      if(e_meas < 0.)
	e_meas = 0.;
      
      double dt_smear = G4RandGauss::shoot(0., cfg.timingResolution);
      double t_meas = t_true + dt_smear;
      double tabs_meas = decayTime + t_meas;

      if(cfg.verbose > 3)
	G4cout << "t_true " << t_true << "\tt_meas " << t_meas << "\ttimingResolution = " << cfg.timingResolution/ns << " ns" << G4endl;
      
      if(e_meas < cfg.threshold)
	continue;

      
      if(cfg.verbose > 1)
	G4cout << "event " << event->GetEventID() << " det " << det << " E " << e_meas/keV << " keV" << " t " << t_meas/ns << " ns" << G4endl;
 
      OutputManager::Instance().FillHit(event->GetEventID(), det, e_true/keV, e_meas/keV, t_true/ns, t_meas/ns, tabs_meas/ns);
    }
    if(event->GetEventID() % 10000 == 0){
      G4cout << "\rProcessed event " << event->GetEventID() << std::flush;
    }
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void EventAction::AddHit(int det, double edep, double time){
    fDetEnergy[det] += edep;

    if(!fDetTime.count(det) || time < fDetTime[det]){
      fDetTime[det] =	time;
    }
  }
}  // namespace QCRNG
