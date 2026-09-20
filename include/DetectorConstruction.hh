// DetectorConstruction.hh

#ifndef DETECTOR_CONSTRUCTION_HH
#define DETECTOR_CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "CaloGeometry.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

private:
  G4LogicalVolume* fLogicCrystal; // Logical volume of a single crystal
  G4Material* fCrystalMaterial;   // Material for the crystals
  G4Material* fMylar;             // Material for the wrapping
  G4Material* fAluminum;          // Material for the honeycomb structure
  G4Material* fFR4;               // Material for the electronic boards
  G4Material* fSiPM;              // Material for the SiPMs
  G4Material* fKapton;   // Material for the Kapton
  int NcryX = 8;
  int NcryY = 1;
  int Nlayer = 1;
  void DefineMaterials();
  G4VPhysicalVolume* ConstructCalorimeter();
  CaloGeometry* fGeometry;
};

#endif
