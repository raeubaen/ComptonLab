#include "PrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4Box.hh"
#include "G4RandomDirection.hh"
#include "G4LogicalVolume.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(new G4ParticleGun(1)) {}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{

    auto source = G4PhysicalVolumeStore::GetInstance()->GetVolume("source_phys");
    auto box = static_cast<G4Box*>(source->GetLogicalVolume()->GetSolid());

    G4ThreeVector localPos(
        (2.*G4UniformRand()-1.) * box->GetXHalfLength(),
        (2.*G4UniformRand()-1.) * box->GetYHalfLength(),
        (2.*G4UniformRand()-1.) * box->GetZHalfLength()
    );

    G4ThreeVector pos = source->GetObjectRotationValue() * localPos
                      + source->GetObjectTranslation();

    fParticleGun->SetParticlePosition(pos);


    // direzione isotropa
    G4ThreeVector dir = G4RandomDirection();


    // Find the particle table
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    // Find the specific particle pointer
    G4ParticleDefinition* particle = particleTable->FindParticle("gamma");

    // Set it to your fParticleGun instance
    fParticleGun->SetParticleDefinition(particle);

    // gamma 1
    fParticleGun->SetParticleEnergy(511.*keV);
    fParticleGun->SetParticlePosition(pos);
    fParticleGun->SetParticleMomentumDirection(dir);
    fParticleGun->GeneratePrimaryVertex(anEvent);

    // gamma 2: back-to-back
    fParticleGun->SetParticleMomentumDirection(-dir);
    fParticleGun->GeneratePrimaryVertex(anEvent);

}
