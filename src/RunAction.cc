/// \file RunAction.cc
/// \brief Implementation of the QCRNG::RunAction class

#include "RunAction.hh"

#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "OutputManager.hh"
#include "QCRNGConfig.hh"

#include "G4AccumulableManager.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

namespace QCRNG{

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void RunAction::BeginOfRunAction(const G4Run*){
    auto& cfg = QCRNGConfig::Instance();

    OutputManager::Instance().Open(cfg.outputFile.c_str());

    if(cfg.verbose > 0)
      G4cout << "Writing output to " << cfg.outputFile << G4endl;

    // inform the runManager to save random number seed
    G4RunManager::GetRunManager()->SetRandomNumberStore(false);
    
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void RunAction::EndOfRunAction(const G4Run* run){
    G4int nofEvents = run->GetNumberOfEvent();
    if (nofEvents == 0) return;


    // Run conditions
    //  note: There is no primary generator action object for "master"
    //        run manager for multi-threaded mode.
    const auto generatorAction = static_cast<const PrimaryGeneratorAction*>(
									    G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
    G4String runCondition;
    if (generatorAction) {
      const G4ParticleGun* particleGun = generatorAction->GetParticleGun();
      runCondition += particleGun->GetParticleDefinition()->GetParticleName();
      runCondition += " of ";
      G4double particleEnergy = particleGun->GetParticleEnergy();
      runCondition += G4BestUnit(particleEnergy, "Energy");
    }
    G4cout << G4endl;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace QCRNG
