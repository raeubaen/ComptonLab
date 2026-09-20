#include "RunAction.hh"
#include "G4Run.hh"

RunAction::RunAction(char *fFileName)
: G4UserRunAction(),
  fOutFile(nullptr),
  fTree(nullptr),
  fEventID(0),
  fFileName(fFileName)
{}

RunAction::~RunAction()
{
	if(fOutFile) {
        if(fOutFile->IsOpen()) fOutFile->Close();
        delete fOutFile;
	}
}

void RunAction::BeginOfRunAction(const G4Run*)
{
    fOutFile = new TFile(fFileName, "RECREATE");
    fTree = new TTree("events","events per event");

    fTree->Branch("EventID", &fEventID);
    fTree->Branch("PrimaryEnergy", &fPrimaryEnergy);

    fTree->Branch("CaloEnergyTotal", &f_calo_ETotal);

    fTree->Branch("Calo_Hit_ix", &fHit_calo_ix);
    fTree->Branch("Calo_Hit_iy", &fHit_calo_iy);
    fTree->Branch("Calo_Hit_iz", &fHit_calo_iz);

    fTree->Branch("Vertex_x", &fVertexX);
    fTree->Branch("Vertex_y", &fVertexY);
    fTree->Branch("Vertex_z", &fVertexZ);

    fTree->Branch("Calo_Hit_avgX", &fHit_calo_avgX);
    fTree->Branch("Calo_Hit_avgY", &fHit_calo_avgY);
    fTree->Branch("Calo_Hit_Z", &fHit_calo_Z);
    fTree->Branch("Calo_Hit_E", &fHit_calo_E);

    fTree->Branch("Tagger_Hit_avgX", &fHit_tagger_avgX);
    fTree->Branch("Tagger_Hit_avgY", &fHit_tagger_avgY);
    fTree->Branch("Tagger_Hit_Z", &fHit_tagger_Z);
    fTree->Branch("Tagger_Hit_E", &fHit_tagger_E);

    fTree->Branch("Tracker_Hit_avgX", &fHit_st_avgX);
    fTree->Branch("Tracker_Hit_avgY", &fHit_st_avgY);
    fTree->Branch("Tracker_Hit_Z", &fHit_st_Z);
    fTree->Branch("Tracker_Hit_E", &fHit_st_E);

}

void RunAction::EndOfRunAction(const G4Run*)
{
    if(fOutFile) {
        fTree->Write();
        fOutFile->Close();
    }
}
