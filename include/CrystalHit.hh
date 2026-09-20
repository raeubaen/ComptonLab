#ifndef CRYSTAL_HIT_HH
#define CRYSTAL_HIT_HH

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class CrystalHit : public G4VHit {
public:
    CrystalHit()
        : ix(-1), iy(-1), iz(-1), pos(G4ThreeVector()), edep(0.), xTimesE(0.), yTimesE(0.) {}
    virtual ~CrystalHit() {}

    // Setters
    void SetIndices(G4int i, G4int j, G4int k) { ix=i; iy=j; iz=k; }
    void SetPos(const G4ThreeVector& p) { pos = p; }
    void AddEnergy(G4double e) { edep += e; }
    void AddXtimesE(G4double _xTimesE) { xTimesE += _xTimesE; }
    void AddYtimesE(G4double _yTimesE) { yTimesE += _yTimesE; }

    // Getters
    G4int GetIx() const { return ix; }
    G4int GetIy() const { return iy; }
    G4int GetIz() const { return iz; }
    G4ThreeVector GetPos() const { return pos; }
    G4double GetEnergyDep() const { return edep; }
    G4double GetXtimesE() const { return xTimesE; }
    G4double GetYtimesE() const { return yTimesE; }

private:
    G4int ix, iy, iz;          // indices in the crystal grid
    G4ThreeVector pos;         // absolute position in world coordinates
    G4double edep, xTimesE, yTimesE;             // deposited energy, x*E, y*E
};

typedef G4THitsCollection<CrystalHit> CrystalHitsCollection;

#endif
