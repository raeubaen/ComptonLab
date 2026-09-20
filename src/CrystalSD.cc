// CrystalSD.cc
#include "CrystalSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "CrystalHit.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4VTouchable.hh"
#include "G4Track.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include <vector>

CrystalSD::CrystalSD(const G4String& name, G4int nx, G4int ny, G4int nz)
    : G4VSensitiveDetector(name),
      fHitsCollection(nullptr),
      fNcryX(nx),
      fNcryY(ny),
      fNlayer(nz)
{
    collectionName.push_back("CrystalHitsCollection");

}

CrystalSD::~CrystalSD()
{
    fHitsCollection = nullptr;  // SDManager owns the hits
}

void CrystalSD::Initialize(G4HCofThisEvent* hce)
{
    // Create hits collection
    fHitsCollection = new CrystalHitsCollection(SensitiveDetectorName, collectionName[0]);

    // Register the hits collection with the SD manager
    G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
    hce->AddHitsCollection(hcID, fHitsCollection);

    // Clear map

    fCrystalHitMap.clear();

    // Pre-create hits for all crystals in this SD
    for (G4int iz = 0; iz < fNlayer; ++iz) {
        for (G4int iy = 0; iy < fNcryY; ++iy) {
            for (G4int ix = 0; ix < fNcryX; ++ix) {
                G4int id = ix + iy * fNcryX + iz * fNcryX * fNcryY;
                CrystalHit* hit = new CrystalHit();
                hit->SetIndices(ix, iy, iz);

                hit->SetPos(fGeometry->GetCrystalCenter(ix, iy, iz));

                fHitsCollection->insert(hit);
                fCrystalHitMap.push_back(hit);
            }
        }
    }
}

G4bool CrystalSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    G4double edep = step->GetTotalEnergyDeposit();
    if (edep == 0.) return false;

    const G4VTouchable* touchable = step->GetPreStepPoint()->GetTouchable();
    G4int crystalID = touchable->GetReplicaNumber(0);

    if (crystalID < 0 || crystalID >= static_cast<G4int>(fCrystalHitMap.size())) return false;

    G4ThreeVector pos = 0.5 * (step->GetPreStepPoint()->GetPosition()
                         + step->GetPostStepPoint()->GetPosition());

    G4double x = pos.x();
    G4double y = pos.y();


    // Add energy
    fCrystalHitMap[crystalID]->AddEnergy(edep);
    fCrystalHitMap[crystalID]->AddXtimesE(x*edep);
    fCrystalHitMap[crystalID]->AddYtimesE(y*edep);

    return true;
}

