/// \file DetectorConstruction.cc
/// \brief Implementation of the QCRNG::DetectorConstruction class

#include "DetectorConstruction.hh"

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

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  DetectorConstruction::DetectorConstruction()
  {
    fMessenger =
      new G4GenericMessenger(
			     this,
			     "/qcrng/geometry/",
			     "Geometry control"
			     );

    fMessenger->DeclarePropertyWithUnit(
					"sourceSize",
					"mm",
					fSourceSize
					);

    fMessenger->DeclarePropertyWithUnit(
					"detLength",
					"mm",
					fDetLength
					);

    fMessenger->DeclarePropertyWithUnit(
					"gap",
					"mm",
					fGap
					);

    fMessenger->DeclarePropertyWithUnit(
					"detAngle",
					"deg",
					fDetAngle
					);
  }

  G4VPhysicalVolume* DetectorConstruction::Construct(){    
    auto nist = G4NistManager::Instance();

    G4bool checkOverlaps = true;

    //
    // Materials
    //
    auto world_mat = nist->FindOrBuildMaterial("G4_AIR");

    auto source_mat = nist->FindOrBuildMaterial("G4_AIR");

    G4double density = 7.1*g/cm3;

    auto det_mat = new G4Material("LYSO", density, 4);

    det_mat->AddElement(nist->FindOrBuildElement("Lu"), 71.43*perCent);
    det_mat->AddElement(nist->FindOrBuildElement("Y"), 4.03*perCent);
    det_mat->AddElement(nist->FindOrBuildElement("Si"),	6.37*perCent);
    det_mat->AddElement(nist->FindOrBuildElement("O"), 18.17*perCent);

    //auto det_mat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

    //
    // Geometry parameters
    //
    G4double source_size = fSourceSize;
    G4double det_length  = fDetLength;
    G4double gap         = fGap;
    G4double det_angle   = fDetAngle;
    
    // compute outer size
    G4double det_outer = source_size + 2.0 * det_length * std::tan(det_angle);

    G4double world_size = 300*mm;

    //
    // World
    //
    auto solidWorld = new G4Box(
				"World",
				world_size/2,
				world_size/2,
				world_size/2
				);

    auto logicWorld = new G4LogicalVolume(
					  solidWorld,
					  world_mat,
					  "World"
					  );

    auto physWorld = new G4PVPlacement(
				       nullptr,
				       {},
				       logicWorld,
				       "World",
				       nullptr,
				       false,
				       0,
				       checkOverlaps
				       );

    //
    // Source cube
    //
    auto solidSource = new G4Box(
		"Source",
		source_size/2,
		source_size/2,
		source_size/2
		);

    auto logicSource = new G4LogicalVolume(
			  solidSource,
			  source_mat,
			  "Source"
			  );

    new G4PVPlacement(
		      nullptr,
		      {},
		      logicSource,
		      "Source",
		      logicWorld,
		      false,
		      0,
		      checkOverlaps
		      );

    //
    // Detector frustum
    //

    auto solidDet = new G4Trd(
			     "Detector",

			     // x half-length:
			     source_size/2,
			     det_outer/2,

			     // y half-length:
			     source_size/2,
			     det_outer/2,

			     // z half-length (= depth)
			     det_length/2
			     );

    auto logicDet = new G4LogicalVolume(
				       solidDet,
				       det_mat,
				       "Detector"
				       );

    G4double det_distance = source_size/2 + det_length/2 + gap;
 
    //
    // Placement info
    //

    std::vector<G4ThreeVector> positions = {
      {0,0,+det_distance},
      {0,0,-det_distance},

      {+det_distance,0,0},
      {-det_distance,0,0},

      {0,+det_distance,0},
      {0,-det_distance,0}
    };

    std::vector<G4RotationMatrix*> rotations(6);

    // +Z
    rotations[0] =
      new G4RotationMatrix();

    // -Z
    rotations[1] =
      new G4RotationMatrix();
    rotations[1]->rotateY(180*deg);

    // +X
    rotations[2] =
      new G4RotationMatrix();
    rotations[2]->rotateY(-90*deg);

    // -X
    rotations[3] =
      new G4RotationMatrix();
    rotations[3]->rotateY(90*deg);

    // +Y
    rotations[4] =
      new G4RotationMatrix();
    rotations[4]->rotateX(90*deg);

    // -Y
    rotations[5] =
      new G4RotationMatrix();
    rotations[5]->rotateX(-90*deg);

    std::vector<G4String> names =
      {
	"det_pz",
	"det_nz",
	"det_px",
	"det_nx",
	"det_py",
	"det_ny"
      };

    for(size_t i=0;i<positions.size();i++)
      {
	new G4PVPlacement(
			  rotations[i],
			  positions[i],
			  logicDet,
			  names[i],
			  logicWorld,
			  false,
			  i,
			  checkOverlaps
			  );
      }
    // source  blue
    auto sourceVis =
      new G4VisAttributes(
			  G4Colour(
				   0.1,   // R
				   0.2,   // G
				   1.0,   // B
				   1.0    // alpha
				   )
			  );

    sourceVis->SetForceSolid(true);

    logicSource->SetVisAttributes(
				  sourceVis
				  );


    // detector  transparent green
    auto detVis =
      new G4VisAttributes(
			  G4Colour(
				   1.0,
				   0.9,
				   0.1,
				   0.35     // transparency
				   )
			  );

    detVis->SetForceSolid(true);

    logicDet->SetVisAttributes(
			       detVis
			       );


    // world invisible
    logicWorld->SetVisAttributes(
				 G4VisAttributes::GetInvisible()
				 );
    
    //
    // Scoring
    //
    fScoringVolume = logicDet;

    // always return the physical World
    //
    return physWorld;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  DetectorConstruction::~DetectorConstruction(){
    delete fMessenger;
  }
  
}  // namespace QCRNG
