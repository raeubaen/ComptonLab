#include "Tagger.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include <iostream>

Tagger::Tagger(const G4String& name)
: G4VSensitiveDetector(name), totalE(0.), total_xTimesE(0.), total_yTimesE(0.)
{}


void Tagger::Initialize(G4HCofThisEvent* /*HCE*/)
{
    // Reset energy at the beginning of each event
    totalE = 0.0;
    total_xTimesE = 0.0;
    total_yTimesE = 0.0;
}

G4bool Tagger::ProcessHits(G4Step* step, G4TouchableHistory*) {

    G4Track* track = step->GetTrack();

    G4double edep = step->GetTotalEnergyDeposit();

    G4ThreeVector pos = 0.5 * (step->GetPreStepPoint()->GetPosition()
                         + step->GetPostStepPoint()->GetPosition());

    G4double x = pos.x();
    G4double y = pos.y();

    total_xTimesE += edep*x;
    total_yTimesE += edep*y;

    totalE += edep;

    return true;
}
