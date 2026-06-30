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

    G4double halfLife = 2.6*year;
    G4double nAtoms = 1.0e9;
    G4bool finiteSource = false;
    
    G4double activity = 1.0*CLHEP::MBq;
    
    G4String detectorMaterial = "LYSO";
    G4double threshold = 50*keV;
    G4double energyResolution = 0.10;
    G4double timingResolution = 0.1*ns;

    G4int verbose = 1;

  private:
    QCRNGConfig();
    ~QCRNGConfig();

    QCRNGConfig(const QCRNGConfig&) = delete;
    QCRNGConfig& operator=(const QCRNGConfig&) = delete;

    G4GenericMessenger* fSourceMessenger = nullptr;
    G4GenericMessenger* fDetectorMessenger = nullptr;
  };

}

#endif
