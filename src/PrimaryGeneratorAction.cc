/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the QCRNG::PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"
#include "QCRNGConfig.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
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

  G4ThreeVector PrimaryGeneratorAction::RandomDirection(){
    G4double cost = 2.*G4UniformRand()-1.;
    G4double sint = std::sqrt(1.-cost*cost);
    G4double phi = CLHEP::twopi*G4UniformRand();
    return G4ThreeVector(sint*std::cos(phi), sint*std::sin(phi), cost);
  }

  G4ThreeVector PrimaryGeneratorAction::RandomSourcePosition(){
    auto& cfg = QCRNGConfig::Instance();
    G4double s = cfg.sourceSize;

    return G4ThreeVector(s*(G4UniformRand()-0.5),
			 s*(G4UniformRand()-0.5),
			 s*(G4UniformRand()-0.5));
  }
  
  void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event){
    auto& cfg = QCRNGConfig::Instance();

    fParticleGun->SetParticlePosition(RandomSourcePosition());

    if(fRemainingAtoms < 0)
      fRemainingAtoms = cfg.nAtoms;

    double activity = cfg.activity;
    
    if(cfg.finiteSource){
      double lambda = std::log(2.0)/cfg.halfLife;
      activity = lambda*fRemainingAtoms;
      
      if(fRemainingAtoms <= 0 || activity <= 0)
	G4Exception("PrimaryGeneratorAction::GeneratePrimaries", "QCRNG0001", FatalException, "No remaining ions.");
      
      fRemainingAtoms -= 1.0;
    }
    
    double dt = -std::log(G4UniformRand())/activity;
    fGlobalTime += dt;
    
    fParticleGun->SetParticleTime(fGlobalTime*second);
    
    if(cfg.sourceType == "beta+"){
      auto eplus = G4ParticleTable::GetParticleTable()->FindParticle("e+");
      fParticleGun->SetParticleDefinition(eplus);
      fParticleGun->SetParticleEnergy(cfg.sourceEnergy);
      fParticleGun->SetParticleMomentumDirection(RandomDirection());
      fParticleGun->GeneratePrimaryVertex(event);
    }
    else if(cfg.sourceType == "isotope"){
      auto ion = G4IonTable::GetIonTable()->GetIon(cfg.sourceZ, cfg.sourceA, 0.0);
      
      fParticleGun->SetParticleDefinition(ion);
      fParticleGun->SetParticleEnergy(0.0);
      fParticleGun->SetParticleCharge(0.0);
      fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0,0,1));
      if(cfg.verbose > 0)
	G4cout << "source type " << cfg.sourceType
	       << " Z " << cfg.sourceZ
	       << " A " << cfg.sourceA
	       << " particle " << fParticleGun->GetParticleDefinition()->GetParticleName()
	       << G4endl;
      fParticleGun->GeneratePrimaryVertex(event);
    }
    else if(cfg.sourceType == "gamma"){
      auto gamma = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
      fParticleGun->SetParticleDefinition(gamma);
      fParticleGun->SetParticleEnergy(cfg.sourceEnergy);
      fParticleGun->SetParticleMomentumDirection(RandomDirection());
      fParticleGun->GeneratePrimaryVertex(event);
    }
    else if(cfg.sourceType == "annihil"){
      auto gamma = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
      G4ThreeVector dir = RandomDirection();
      fParticleGun->SetParticleDefinition(gamma);
      fParticleGun->SetParticleEnergy(511.0*keV);
      
      fParticleGun->SetParticleMomentumDirection(dir);
      fParticleGun->GeneratePrimaryVertex(event);
      fParticleGun->SetParticleMomentumDirection(-dir);
      fParticleGun->GeneratePrimaryVertex(event);
    }
    else{
      G4Exception("PrimaryGeneratorAction::GeneratePrimaries", "QCRNG0002", FatalException, "Unknown source type.");
    }

    
  }


  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace QCRNG
