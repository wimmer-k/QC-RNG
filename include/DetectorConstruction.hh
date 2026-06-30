/// \file DetectorConstruction.hh
/// \brief Definition of the QCRNG::DetectorConstruction class

#ifndef QCRNGDetectorConstruction_h
#define QCRNGDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

namespace QCRNG{

  /// Detector construction class to define materials and geometry.

  class DetectorConstruction : public G4VUserDetectorConstruction{
  public:
    DetectorConstruction();
    ~DetectorConstruction() override;

    G4VPhysicalVolume* Construct() override;

    G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; }

  protected:
    G4LogicalVolume* fScoringVolume = nullptr;

  private:
    G4GenericMessenger* fGeometryMessenger = nullptr;

    G4double fSourceSize = 10*mm;
    G4double fDetLength  = 50*mm;
    G4double fGap        = 2*mm;
    G4double fDetAngle   = 45*deg;

  };

}  // namespace QCRNG

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
