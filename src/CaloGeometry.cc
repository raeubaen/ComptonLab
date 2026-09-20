#include "CaloGeometry.hh"
#include "G4ThreeVector.hh"


G4ThreeVector CaloGeometry::GetCrystalCenter(G4int i,
                                               G4int j,
                                               G4int k) const
{
    G4double x = fX0 + i * fPitch;
    G4double y = fY0;
    G4double z = fZ0;

    return G4ThreeVector(x, y, z);
}
