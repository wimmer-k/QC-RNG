/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the QCRNG::PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"
#include "QCRNGConfig.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include <cmath>

namespace QCRNG{

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  PrimaryGeneratorAction::PrimaryGeneratorAction(){
    G4cout << "PrimaryGeneratorAction constructed"<< G4endl;

        G4int n_particle = 1;
    fParticleGun = new G4ParticleGun(n_particle);
    G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("e+");
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleEnergy(50*keV);
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  PrimaryGeneratorAction::~PrimaryGeneratorAction(){
    delete fParticleGun;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event){
    // Source position
    fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, 0));

    // isotropic direction
    G4double cost = 2.*G4UniformRand()-1.;
    G4double sint = std::sqrt(1.-cost*cost);
    G4double phi = CLHEP::twopi*G4UniformRand();
    G4ThreeVector dir(sint*std::cos(phi), sint*std::sin(phi), cost);
    fParticleGun->SetParticleMomentumDirection(dir);


    auto& cfg = QCRNGConfig::Instance();
    if(fRemainingAtoms < 0)
      fRemainingAtoms = cfg.nAtoms;
    double activity = cfg.activity;

    if(cfg.finiteSource){
      double lambda = std::log(2.0)/cfg.halfLife;
      activity = lambda*fRemainingAtoms;

      if(fRemainingAtoms <= 0 || activity <= 0){
	G4Exception("PrimaryGeneratorAction::GeneratePrimaries","QCRNG0001", FatalException, "Source exhausted.");
      }

      fRemainingAtoms -= 1.0;
    }
    //time in ns

    double dt = -std::log(G4UniformRand())/activity;
    fGlobalTime += dt;

    fParticleGun->SetParticleTime(fGlobalTime*second);

   
    // create event
    fParticleGun->GeneratePrimaryVertex(event);

  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace QCRNG
