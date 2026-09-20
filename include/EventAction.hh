#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include "G4UserEventAction.hh"
#include "RunAction.hh"
#include <vector>
#include "CrystalSD.hh"

class CrystalHit;

class EventAction : public G4UserEventAction {
public:
    EventAction(RunAction* runAction);
    virtual ~EventAction() {}

    virtual void BeginOfEventAction(const G4Event* event) override;
    virtual void EndOfEventAction(const G4Event* event) override;

private:
    RunAction* fRunAction;  // pointer to RunAction
    CrystalSD* fCrystalSD;   // pointer to the SD
    int fCrystalHCID;
    G4double fPrimaryEnergy, fVertexX, fVertexY, fVertexZ;
    G4int fNcryX;
    G4int fNcryY;
    G4int fNlayer;
    std::vector<int> fHit_calo_ix, fHit_calo_iy, fHit_calo_iz;
    std::vector<double> fHit_calo_avgX, fHit_calo_avgY, fHit_calo_Z, fHit_calo_E;
    G4double fHit_tagger_avgX, fHit_tagger_avgY, fHit_tagger_Z, fHit_tagger_E;
    G4double fHit_st_avgX, fHit_st_avgY, fHit_st_Z, fHit_st_E;
    double f_calo_ETotal;
};

#endif
