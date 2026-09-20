#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
#include "TFile.h"
#include "TTree.h"
#include <vector>

class RunAction : public G4UserRunAction {
public:
    RunAction(char*);
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run*) override;
    virtual void EndOfRunAction(const G4Run*) override;
    TTree* GetTree() {return fTree;}
    // Tree & file are public so EventAction can fill them
    // Branch data
    int fEventID;
    double fPrimaryEnergy, fVertexX, fVertexY, fVertexZ;
    double f_calo_ETotal;

    std::vector<int> fHit_calo_ix;
    std::vector<int> fHit_calo_iy;
    std::vector<int> fHit_calo_iz;

    std::vector<double> fHit_calo_avgX;
    std::vector<double> fHit_calo_avgY;
    std::vector<double> fHit_calo_Z;
    std::vector<double> fHit_calo_E;

    G4double fHit_tagger_avgX, fHit_tagger_avgY, fHit_tagger_Z, fHit_tagger_E;
    G4double fHit_st_avgX, fHit_st_avgY, fHit_st_Z, fHit_st_E;

private:
    TTree *fTree;
    TFile *fOutFile;
    char *fFileName;
};
#endif
