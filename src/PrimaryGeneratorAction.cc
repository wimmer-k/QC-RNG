/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the QCRNG::PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include <cmath>

namespace QCRNG
{

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  PrimaryGeneratorAction::PrimaryGeneratorAction()
  {
    G4int n_particle = 1;
    fParticleGun = new G4ParticleGun(n_particle);
    G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("e+");
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleEnergy(0. * keV);
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

    // create event
    fParticleGun->GeneratePrimaryVertex(event);

  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace QCRNG
