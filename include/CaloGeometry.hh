#ifndef CaloGeoMETRY_HH
#define CaloGeoMETRY_HH

#include "G4ThreeVector.hh"
#include "globals.hh"

class CaloGeometry
{
public:

    CaloGeometry() {};
    ~CaloGeometry();

    // Main geometry utility
    G4ThreeVector GetCrystalCenter(G4int i, G4int j, G4int k) const;

    // Getters
    G4double GetPitch() const {return fPitch;};
    G4double GetCrystalLength() const {return fCrystalLength;};
    G4double GetLayerStep() const {return fLayerStep; };

    G4double GetX0() const {return fX0;};
    G4double GetY0() const {return fY0;};
    G4double GetZ0() const {return fZ0;};

    // Setters
    void SetPitch(G4double value) {fPitch = value;};
    void SetCrystalLength(G4double value) { fCrystalLength = value;};
    void SetLayerStep(G4double value) { fLayerStep = value;};

    void SetRefPosition(G4ThreeVector v) {fX0 = v.x(); fY0 = v.y(); fZ0 = v.z();};

private:

    G4double fPitch;
    G4double fCrystalLength;
    G4double fSiPMThickness;
    G4double fElectronicsThickness;

    G4double fLayerStep;

    G4double fX0;
    G4double fY0;
    G4double fZ0;
};

#endif
