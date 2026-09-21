#include "EventAction.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "CrystalHit.hh"
#include "ScintillatingTracker.hh"
#include "G4PhysicalVolumeStore.hh"
#include "Tagger.hh"
#include "ScintillatingTracker.hh"

EventAction::EventAction(RunAction* runAction)
: G4UserEventAction(), fRunAction(runAction), fCrystalHCID(-1)
{
  fNcryX = -999;
}

void EventAction::BeginOfEventAction(const G4Event* event)
{

    //std::cout << std::endl << "NEW EVENT" << std::endl;

    if (fNcryX == -999){
      G4SDManager* sdman = G4SDManager::GetSDMpointer();
      fCrystalSD = dynamic_cast<CrystalSD*>(sdman->FindSensitiveDetector("CrystalSD"));

      if (fCrystalSD) {
          fNcryX  = fCrystalSD->GetNcryX();
          fNcryY  = fCrystalSD->GetNcryY();
          fNlayer = fCrystalSD->GetNlayer();
      }
    }

    fPrimaryEnergy = 0.0;
    fVertexX = 0.0;
    fVertexY = 0.0;
    fVertexZ = 0.0;
    fHit_calo_ix.clear();
    fHit_calo_iy.clear();
    fHit_calo_iz.clear();
    fHit_calo_avgX.clear();
    fHit_calo_avgY.clear();
    fHit_calo_Z.clear();
    fHit_calo_E.clear();
    f_calo_ETotal = 0.0;
    fHit_tagger_avgX = 0.0;
    fHit_tagger_avgY = 0.0;
    fHit_tagger_Z = 0.0;
    fHit_st_avgX = 0.0;
    fHit_st_avgY = 0.0;
    fHit_st_Z = 0.0;

    if(fCrystalHCID == -1)
        fCrystalHCID = G4SDManager::GetSDMpointer()->GetCollectionID("CrystalHitsCollection");
}

void EventAction::EndOfEventAction(const G4Event* event)
{
    auto hc = event->GetHCofThisEvent();
    if (!hc) return;

    // --- Crystal hits
    auto hitsCollection = static_cast<G4THitsCollection<CrystalHit>*>(hc->GetHC(fCrystalHCID));
    std::map<int, CrystalHit*> crystalMap;

    if (hitsCollection) {
        for (int i = 0; i < hitsCollection->entries(); i++) {
            auto hit = (*hitsCollection)[i];
            if (hit->GetEnergyDep() <= 0.) continue;

      	    //Compute total energy
      	    f_calo_ETotal += hit->GetEnergyDep();

            // Compute unique crystal ID
            int id = hit->GetIx() + hit->GetIy() * fNcryX + hit->GetIz() * fNcryX * fNcryY;

            // Only one hit per crystal
            if (crystalMap.find(id) == crystalMap.end()) {
                // Clone the first hit
                CrystalHit* newHit = new CrystalHit(*hit);
                crystalMap[id] = newHit;
            } else {
                // Accumulate energy for repeated hits
                crystalMap[id]->AddEnergy(hit->GetEnergyDep());
                crystalMap[id]->AddXtimesE(hit->GetXtimesE());
                crystalMap[id]->AddYtimesE(hit->GetYtimesE());
            }
        }

        // Fill event vectors from the map
        for (const auto& [id, hit] : crystalMap) {
            fHit_calo_ix.push_back(hit->GetIx());
            fHit_calo_iy.push_back(hit->GetIy());
            fHit_calo_iz.push_back(hit->GetIz());

            auto center = hit->GetPos();
            if (hit->GetEnergyDep() > 0.) {
              fHit_calo_avgX.push_back(hit->GetXtimesE() / hit->GetEnergyDep());
              fHit_calo_avgY.push_back(hit->GetYtimesE() / hit->GetEnergyDep());
            }
            else {
              fHit_calo_avgX.push_back(-999.);
              fHit_calo_avgY.push_back(-999.);
            }

            fHit_calo_Z.push_back(center.z());

            fHit_calo_E.push_back(hit->GetEnergyDep() / CLHEP::MeV);

            delete hit;  // free the cloned hit
        }
    }

    // --- Scintillating Tracker
    auto st = static_cast<ScintillatingTracker*>(G4SDManager::GetSDMpointer()->FindSensitiveDetector("st"));
    fHit_st_E = st ? st->GetTotalEnergy() / CLHEP::MeV : 0.;
    fHit_st_avgX = st ? st->GetAverageX() : -999.;
    fHit_st_avgY = st ? st->GetAverageY() : -999.;
    fHit_st_Z = G4PhysicalVolumeStore::GetInstance()->GetVolume("st_phys")->GetObjectTranslation().z();


    // --- Tagger
    auto tagger = static_cast<Tagger*>(G4SDManager::GetSDMpointer()->FindSensitiveDetector("tagger"));
    fHit_tagger_E = tagger ? tagger->GetTotalEnergy() / CLHEP::MeV : 0.;
    fHit_tagger_avgX = tagger ? tagger->GetAverageX() : -999.;
    fHit_tagger_avgY = tagger ? tagger->GetAverageY() : -999.;
    fHit_tagger_Z = G4PhysicalVolumeStore::GetInstance()->GetVolume("tagger_phys")->GetObjectTranslation().z();


    //Energy of primary particle

    G4double PrimaryEnergy = 0.0;
    G4double x_v=0.0, y_v=0.0, z_v=0.0;
    G4PrimaryVertex* vertex = event->GetPrimaryVertex();
    if (vertex) {
    	G4PrimaryParticle* particle = vertex->GetPrimary();
    	if (particle) {
     		PrimaryEnergy = particle->GetTotalEnergy();
        x_v = vertex->GetX0();
        y_v = vertex->GetY0();
        z_v = vertex->GetZ0();
		}
    	}

    std::cout << "Event ID = " << event->GetEventID() << " Primary energy = " << PrimaryEnergy << std::endl;

    // --- Fill the tree via RunAction
    fRunAction->fEventID = event->GetEventID();
    fRunAction->fPrimaryEnergy = PrimaryEnergy;
    fRunAction->fVertexX = x_v;
    fRunAction->fVertexY = y_v;
    fRunAction->fVertexZ = z_v;

    fRunAction->fHit_calo_ix = fHit_calo_ix;
    fRunAction->fHit_calo_iy = fHit_calo_iy;
    fRunAction->fHit_calo_iz = fHit_calo_iz;

    fRunAction->fHit_calo_avgX = fHit_calo_avgX;
    fRunAction->fHit_calo_avgY = fHit_calo_avgY;
    fRunAction->fHit_calo_Z = fHit_calo_Z;
    fRunAction->fHit_calo_E = fHit_calo_E;

    fRunAction->fHit_tagger_E = fHit_tagger_E;
    fRunAction->fHit_tagger_avgX = fHit_tagger_avgX;
    fRunAction->fHit_tagger_avgY = fHit_tagger_avgY;
    fRunAction->fHit_tagger_Z = fHit_tagger_Z;

    fRunAction->fHit_st_E = fHit_st_E;
    fRunAction->fHit_st_avgX = fHit_st_avgX;
    fRunAction->fHit_st_avgY = fHit_st_avgY;
    fRunAction->fHit_st_Z = fHit_st_Z;

    fRunAction->f_calo_ETotal = f_calo_ETotal;

    fRunAction->GetTree()->Fill();

    // --- Clear vectors for next event
    fHit_calo_ix.clear();
    fHit_calo_iy.clear();
    fHit_calo_iz.clear();
    fHit_calo_avgX.clear();
    fHit_calo_avgY.clear();
    fHit_calo_Z.clear();
    fHit_calo_E.clear();
    fHit_tagger_E = 0.;
    fHit_tagger_avgX = 0.;
    fHit_tagger_avgY = 0.;
    fHit_tagger_Z = 0.;
    fHit_st_E = 0.;
    fHit_st_avgX = 0.;
    fHit_st_avgY = 0.;
    fHit_st_Z = 0.;
    f_calo_ETotal = 0.;
}
