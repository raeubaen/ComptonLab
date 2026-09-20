#ifndef ScintillatingTracker_H
#define ScintillatingTracker_H

#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"

class ScintillatingTracker : public G4VSensitiveDetector {
public:
    ScintillatingTracker(const G4String& name);
    virtual ~ScintillatingTracker() {}
    virtual void Initialize(G4HCofThisEvent* hce) override;
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory*);

    G4double GetTotalEnergy() const { return totalE; }
    G4double GetAverageX() const { if (totalE > 0.) {return total_xTimesE/totalE;} else { return -999.; }; }
    G4double GetAverageY() const { if (totalE > 0.) {return total_yTimesE/totalE;} else { return -999.; }; }

private:
    G4double totalE, total_xTimesE, total_yTimesE;
};

#endif
