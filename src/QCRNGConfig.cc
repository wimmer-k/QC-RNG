#include "QCRNGConfig.hh"
#include "G4UnitsTable.hh"

namespace QCRNG{

  QCRNGConfig& QCRNGConfig::Instance(){
    static QCRNGConfig instance;
    return instance;
  }

  QCRNGConfig::QCRNGConfig(){

    new G4UnitDefinition("becquerel", "Bq", "Activity", 1.0/second);
    new G4UnitDefinition("kilobecquerel", "kBq", "Activity", 1.0e3/second);
    new G4UnitDefinition("megabecquerel", "MBq", "Activity", 1.0e6/second);
    new G4UnitDefinition("gigabecquerel", "GBq", "Activity", 1.0e9/second);

    fSourceMessenger = new G4GenericMessenger(this, "/qcrng/source/", "Source control");
    fSourceMessenger->DeclarePropertyWithUnit("activity", "Bq", activity, "Source activity");
    fSourceMessenger->DeclareProperty("finiteSource", finiteSource, "Use finite radioactive source");
    fSourceMessenger->DeclarePropertyWithUnit("halfLife", "s", halfLife, "Source half-life");
    fSourceMessenger->DeclareProperty("nAtoms", nAtoms, "Initial number of radioactive atoms");

    fDetectorMessenger = new G4GenericMessenger(this, "/qcrng/detector/", "Detector control");
    fDetectorMessenger->DeclareProperty("material", detectorMaterial, "Detector material: LYSO or GAGG");
    fDetectorMessenger->DeclarePropertyWithUnit("threshold", "keV", threshold, "Detector threshold (keV)");
    fDetectorMessenger->DeclareProperty("energyResolution", energyResolution, "Energy resolution (FWHM)");
    fDetectorMessenger->DeclarePropertyWithUnit("timingResolution", "ns", timingResolution, "Timing resolution (ns)");

    fDetectorMessenger->DeclareProperty("verbose", verbose, "Verbosity level");
  }

  QCRNGConfig::~QCRNGConfig(){
    delete fSourceMessenger;
    delete fDetectorMessenger;
  }

}
