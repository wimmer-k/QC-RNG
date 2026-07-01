#ifndef QCRNGConfig_h
#define QCRNGConfig_h 1

#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4GenericMessenger.hh"
#include "G4String.hh"

namespace QCRNG{

  class QCRNGConfig{
  public:
    static QCRNGConfig& Instance();

    //source
    G4String sourceType = "beta+";
    G4double sourceEnergy = 50.0*keV;
    G4int sourceZ = 11;
    G4int sourceA = 22;
    G4double halfLife = 2.6*year;
    G4double nAtoms = 1.0e9;
    G4bool finiteSource = false;
    G4double activity = 1.0*CLHEP::MBq;

    //detector
    G4double sourceSize = 10*mm;
    G4double detLength = 50*mm;
    G4double gap = 1*mm;
    G4double detAngle = 45*deg;
    G4String detectorMaterial = "LYSO";
    G4double threshold = 50*keV;
    G4double energyResolution = 0.10;
    G4double timingResolution = 0.1*ns;

    //output
    G4int verbose = 1;
    G4String outputFile = "qcrng.root";

  private:
    QCRNGConfig();
    ~QCRNGConfig();

    QCRNGConfig(const QCRNGConfig&) = delete;
    QCRNGConfig& operator=(const QCRNGConfig&) = delete;

    G4GenericMessenger* fSourceMessenger = nullptr;
    G4GenericMessenger* fDetectorMessenger = nullptr;
    G4GenericMessenger* fOutputMessenger = nullptr;
  };

}

#endif
