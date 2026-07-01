/// \file DetectorConstruction.cc
/// \brief Implementation of the QCRNG::DetectorConstruction class

#include "DetectorConstruction.hh"
#include "QCRNGConfig.hh"

#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Trd.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"


namespace QCRNG{
  G4Material* BuildDetectorMaterial(G4NistManager* nist, const G4String& name){
    if(name == "LYSO"){
      auto mat = new G4Material("LYSO", 7.1*g/cm3, 4);
      mat->AddElement(nist->FindOrBuildElement("Lu"), 71.43*perCent);
      mat->AddElement(nist->FindOrBuildElement("Y"), 4.03*perCent);
      mat->AddElement(nist->FindOrBuildElement("Si"), 6.37*perCent);
      mat->AddElement(nist->FindOrBuildElement("O"), 18.17*perCent);
      return mat;
    }

    if(name == "GAGG"){
      auto mat = new G4Material("GAGG", 6.63*g/cm3, 4);
      mat->AddElement(nist->FindOrBuildElement("Gd"), 3);
      mat->AddElement(nist->FindOrBuildElement("Al"), 2);
      mat->AddElement(nist->FindOrBuildElement("Ga"), 3);
      mat->AddElement(nist->FindOrBuildElement("O"), 12);
      return mat;
    }

    G4Exception("BuildDetectorMaterial", "QCRNG001", FatalException, "Unknown detector material. Use LYSO or GAGG.");
    return nullptr;
  }

  
  G4VPhysicalVolume* DetectorConstruction::Construct(){    
    auto nist = G4NistManager::Instance();
    auto& cfg = QCRNGConfig::Instance();
    
    G4bool checkOverlaps = true;

    //
    // Materials
    //
    auto world_mat = nist->FindOrBuildMaterial("G4_AIR");
    auto source_mat = nist->FindOrBuildMaterial("G4_AIR");
    auto det_mat = BuildDetectorMaterial(nist, cfg.detectorMaterial);

    //
    // Geometry parameters
    //
    G4double source_size = cfg.sourceSize;
    G4double det_length  = cfg.detLength;
    G4double gap         = cfg.gap;
    G4double det_angle   = cfg.detAngle;
    
    // compute outer size
    G4double det_outer = source_size + 2.0 * det_length * std::tan(det_angle);
    G4double world_size = 300*mm;

    //
    // World
    //
    auto solidWorld = new G4Box("World", world_size/2, world_size/2, world_size/2);
    auto logicWorld = new G4LogicalVolume(solidWorld, world_mat, "World");
    auto physWorld = new G4PVPlacement(nullptr, {}, logicWorld, "World", nullptr, false, 0, checkOverlaps);
 
    //
    // Source cube
    //
    auto solidSource = new G4Box("Source", source_size/2, source_size/2, source_size/2);
    auto logicSource = new G4LogicalVolume(solidSource, source_mat, "Source");
    new G4PVPlacement(nullptr, {}, logicSource, "Source", logicWorld, false, 0, checkOverlaps);
    
    //
    // Detector
    //
    auto solidDet = new G4Trd("Detector",
			      source_size/2, det_outer/2,
			      source_size/2, det_outer/2,
			      det_length/2);

    auto logicDet = new G4LogicalVolume(solidDet, det_mat, "Detector");

    G4double det_distance = source_size/2 + det_length/2 + gap;

    std::vector<G4ThreeVector> positions = {
      {0,0,+det_distance}, {0,0,-det_distance},
      {+det_distance,0,0}, {-det_distance,0,0},
      {0,+det_distance,0}, {0,-det_distance,0}
    };

    std::vector<G4RotationMatrix*> rotations(6);
    rotations[0] = new G4RotationMatrix();

    rotations[1] = new G4RotationMatrix();
    rotations[1]->rotateY(180*deg);

    rotations[2] = new G4RotationMatrix();
    rotations[2]->rotateY(-90*deg);

    rotations[3] = new G4RotationMatrix();
    rotations[3]->rotateY(90*deg);

    rotations[4] = new G4RotationMatrix();
    rotations[4]->rotateX(90*deg);

    rotations[5] = new G4RotationMatrix();
    rotations[5]->rotateX(-90*deg);

    std::vector<G4String> names = {"det_pz", "det_nz", "det_px", "det_nx", "det_py", "det_ny"};

    for(size_t i=0; i<positions.size(); i++){
      new G4PVPlacement(rotations[i], positions[i], logicDet, names[i], logicWorld, false, i, checkOverlaps);
    }

    auto sourceVis = new G4VisAttributes(G4Colour(0.1, 0.2, 1.0, 1.0));
    sourceVis->SetForceSolid(true);
    logicSource->SetVisAttributes(sourceVis);

    auto detVis = new G4VisAttributes(G4Colour(1.0, 0.9, 0.1, 0.35));
    detVis->SetForceSolid(true);
    logicDet->SetVisAttributes(detVis);

    logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());

    fScoringVolume = logicDet;
 
    // always return the physical World
    //
    return physWorld;
  }

  
}  // namespace QCRNG
