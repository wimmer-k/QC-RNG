/// \file PrimaryGeneratorAction.hh
/// \brief Definition of the QCRNG::PrimaryGeneratorAction class

#ifndef QCRNGPrimaryGeneratorAction_h
#define QCRNGPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class G4Box;

namespace QCRNG{

  /// The primary generator action class with particle gun.
  ///

  class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction{
  public:
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction() override;

    // method from the base class
    void GeneratePrimaries(G4Event*) override;

    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
    double GetGlobalTime() const{return fGlobalTime;}
    G4ThreeVector RandomDirection();
    G4ThreeVector RandomSourcePosition();

  private:
    G4ParticleGun* fParticleGun = nullptr;  // pointer a to G4 gun class

    double fGlobalTime = 0.0;    // ns
    double fRemainingAtoms = -1.0;
  };

}  // namespace QCRNG

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
