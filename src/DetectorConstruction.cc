// DetectorConstruction.cc

#include "DetectorConstruction.hh"
#include "G4SDManager.hh"
#include "CrystalSD.hh" // Includi il nuovo header
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4Tubs.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "ScintillatingTracker.hh"
#include "Tagger.hh"

// Constructor
DetectorConstruction::DetectorConstruction()
: fCrystalMaterial(nullptr), fLogicCrystal(nullptr) {
    fGeometry = new CaloGeometry();
}

// Destructor
DetectorConstruction::~DetectorConstruction() {}

// Define the materials
void DetectorConstruction::DefineMaterials() {


  // Define other materials from NIST database
  G4NistManager* nist = G4NistManager::Instance();
  fCrystalMaterial = nist->FindOrBuildMaterial("G4_PbWO4");

}

G4VPhysicalVolume* DetectorConstruction::Construct() {
    DefineMaterials();
    return ConstructCalorimeter();
}

G4VPhysicalVolume* DetectorConstruction::ConstructCalorimeter() {
    // World
    G4double worldSizeXYZ = 1.2 * m;
    G4Material* worldMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

    G4Box* solidWorld = new G4Box("World", worldSizeXYZ, worldSizeXYZ, worldSizeXYZ);
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, worldMaterial, "World");

    G4VPhysicalVolume* physWorld = new G4PVPlacement(nullptr, G4ThreeVector(), logicWorld, "World", nullptr, false, 0);

    // Crystals
    G4double crystalSizeXZ = 0.4 * cm;
    G4double crystalLength = 4.0 * cm;

    G4Box* solidCrystal = new G4Box("Crystal", crystalSizeXZ / 2, crystalLength / 2, crystalSizeXZ / 2);
    fLogicCrystal = new G4LogicalVolume(solidCrystal, fCrystalMaterial, "Crystal");

    G4double pitch=crystalSizeXZ;
    G4double calosizex=NcryX*pitch;
    G4double calosizey=NcryY*crystalLength;
    G4double calosizez=Nlayer*pitch;
    G4double layerStep = crystalLength;

    G4double caloZoffset = 6*cm;

    G4Box* solidcalo = new G4Box("solidcalo", 0.5*calosizex,0.5*calosizey,0.5*(calosizez+0.4*mm));
    G4LogicalVolume* logiccalo = new G4LogicalVolume(solidcalo, logicWorld->GetMaterial(), "logiccalo");
    new G4PVPlacement(nullptr, G4ThreeVector(0.,0., caloZoffset -0.5*(calosizez+0.4*mm) ), logiccalo, "caloVolume", logicWorld, false, 0); //not sure about it

    // Centering offsets (XY only)
    G4double x0 = - (NcryX - 1) * pitch / 2.0;
    G4double y0 = - (NcryY - 1) * crystalLength / 2.0; // fa 0
    G4double z0 = 0;

    fGeometry->SetPitch(pitch);
    fGeometry->SetLayerStep(layerStep);
    fGeometry->SetCrystalLength(crystalLength);
    fGeometry->SetRefPosition(G4ThreeVector(x0, y0, z0));

    for (int k = 0; k < Nlayer; ++k) {
        for (int j = 0; j < NcryY; ++j) {
            for (int i = 0; i < NcryX; ++i) {

                G4ThreeVector basePos = fGeometry->GetCrystalCenter(i, j, k);

            // Crystal
                new G4PVPlacement(nullptr,
                                basePos,
                                fLogicCrystal,
                                "Crystal",
                                logiccalo,
                                false,
                                i + j*NcryX + k*NcryX*NcryY);
	    }
	}
    }

    G4VisAttributes* invisAttributes = new G4VisAttributes(G4Colour::White()); // Color is optional
    invisAttributes->SetVisibility(false); // Make it invisible
    logicWorld->SetVisAttributes(invisAttributes);
    //logiccalo->SetVisAttributes(invisAttributes);

    G4VisAttributes* crystalVisAtt = new G4VisAttributes(G4Colour(0.0, 1.0, 1.0, 0.1)); // trasparente
    fLogicCrystal->SetVisAttributes(crystalVisAtt);

    // --- Scintillating tracker parameters
    G4double stThickness = 5*mm;  // thin plane
    G4double stPosZ = 3*cm + stThickness/2.0; // just before calorimeter

    // Create the solid
    G4Box* stSolid = new G4Box("st_solid", calosizex/2.0, calosizey/2.0, stThickness/2.0);

    // Get material safely
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* scintillator = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"); // Galactic = vacuum

    // Create logical volume
    G4LogicalVolume* stLogic = new G4LogicalVolume(stSolid, scintillator, "st_logical");
    stLogic->SetVisAttributes(new G4VisAttributes(G4Colour(0, 0, 1.0)));

    // Place it in the world
    new G4PVPlacement(nullptr,
                      G4ThreeVector(0., 0., stPosZ),
                      stLogic,
                      "st_phys",
                      logicWorld,
                      false,
                      0,
                      true); // checkOverlaps = true

    // Attach sensitive detector
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    ScintillatingTracker* st = new ScintillatingTracker("st");
    SDman->AddNewDetector(st);
    stLogic->SetSensitiveDetector(st);



    // --- Target parameters
    G4double tgtThickness = 5*mm;  // thin plane
    G4double tgtPosZ = 1*cm + tgtThickness/2.0; // just before calorimeter

    // Create the solid
    G4Box* tgtSolid = new G4Box("tgt_solid", calosizey/2.0, calosizey/2.0, tgtThickness/2.0);

    // Get material safely
    G4Material* graphite = nist->FindOrBuildMaterial("G4_GRAPHITE"); // Galactic = vacuum

    // Create logical volume
    G4LogicalVolume* tgtLogic = new G4LogicalVolume(tgtSolid, graphite, "tgt_logical");
    tgtLogic->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 1, 1.0)));

    // Place it in the world
    new G4PVPlacement(nullptr,
                      G4ThreeVector(0., 0., tgtPosZ),
                      tgtLogic,
                      "tgt_phys",
                      logicWorld,
                      false,
                      0,
                      true); // checkOverlaps = true




    // --- source parameters
    G4double sourceThickness = 5*mm;  // thin plane
    G4double sourcePosZ = 0.5*cm + sourceThickness/2.0; // just before calorimeter

    // Create the solid
    G4Box* sourceSolid = new G4Box("source_solid", 0.5*mm, 0.5*mm, sourceThickness/2.0);

    // Get material safely
    G4Material* pe = nist->FindOrBuildMaterial("G4_POLYETHYLENE"); // Galactic = vacuum

    // Create logical volume
    G4LogicalVolume* sourceLogic = new G4LogicalVolume(sourceSolid, pe, "source_logical");
    sourceLogic->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 0, 1.0)));

    // Place it in the world
    new G4PVPlacement(nullptr,
                      G4ThreeVector(0., 0., sourcePosZ),
                      sourceLogic,
                      "source_phys",
                      logicWorld,
                      false,
                      0,
                      true); // checkOverlaps = true


    // --- tagger parameters
    G4double taggerThickness = 5*mm;  // thin plane
    G4double taggerPosZ = -3*cm + taggerThickness/2.0; // just before calorimeter

    // Create the solid
    G4Box* taggerSolid = new G4Box("tagger_solid", 1*cm, 1*cm, taggerThickness/2.0);

    // Get material safely
    G4Material* tagger_material = nist->FindOrBuildMaterial("G4_PbWO4"); // Galactic = vacuum

    // Create logical volume
    G4LogicalVolume* taggerLogic = new G4LogicalVolume(taggerSolid, tagger_material, "tagger_logical");
    taggerLogic->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 1.0, 0.0)));

    // Place it in the world
    new G4PVPlacement(nullptr,
                      G4ThreeVector(0., 0., taggerPosZ),
                      taggerLogic,
                      "tagger_phys",
                      logicWorld,
                      false,
                      0,
                      true); // checkOverlaps = true

    // Attach sensitive detector
    Tagger* tagger = new Tagger("tagger");
    SDman->AddNewDetector(tagger);
    taggerLogic->SetSensitiveDetector(tagger);


    return physWorld;

}

void DetectorConstruction::ConstructSDandField() {
    // Creazione del rivelatore sensibile
    CrystalSD* crystalSD = new CrystalSD("CrystalSD", NcryX, NcryY, Nlayer);

    // Registrazione presso il SDManager
    G4SDManager::GetSDMpointer()->AddNewDetector(crystalSD);

    // Associazione del rivelatore sensibile ai volumi logici dei cristalli
    if (fLogicCrystal) {
        fLogicCrystal->SetSensitiveDetector(crystalSD);
        crystalSD->SetGeometry(fGeometry);
    }
}
