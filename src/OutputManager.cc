#include "OutputManager.hh"

namespace QCRNG{

  OutputManager& OutputManager::Instance(){
    static OutputManager instance;
    return instance;
  }

  void OutputManager::Open(const char* filename){
    fFile = new TFile(filename, "RECREATE");

    fEventTree = new TTree("events", "Event-level information");
    fEventTree->Branch("eventID", &fEventID);
    fEventTree->Branch("decayTime_ns", &fDecayTime_ns);
    fEventTree->Branch("activity_Bq", &fActivity_Bq);

    fTruthTree = new TTree("truth", "Truth particles");
    fTruthTree->Branch("eventID", &fTruthEventID);
    fTruthTree->Branch("trackID", &fTruthTrackID);
    fTruthTree->Branch("parentID", &fTruthParentID);
    fTruthTree->Branch("pdg", &fTruthPDG);
    fTruthTree->Branch("creator", &fTruthCreatorProcess);
    fTruthTree->Branch("E", &fTruthE_keV);
    fTruthTree->Branch("px", &fTruthPx);
    fTruthTree->Branch("py", &fTruthPy);
    fTruthTree->Branch("pz", &fTruthPz);
    fTruthTree->Branch("t", &fTruthT_ns);

    fHitTree = new TTree("hits", "Detector hits");
    fHitTree->Branch("eventID", &fDetEventID);
    fHitTree->Branch("detID", &fDetID);
    fHitTree->Branch("E_true", &fETrue_keV);
    fHitTree->Branch("E_meas", &fEMeas_keV);
    fHitTree->Branch("t_true", &fTTrue_ns);
    fHitTree->Branch("t_meas", &fTMeas_ns);
    fHitTree->Branch("tabs_meas", &fTAbsMeas_ns);
  }

  void OutputManager::Close(){
    if(!fFile) return;

    fFile->Write();
    fFile->Close();

    delete fFile;
    fFile = nullptr;
    fEventTree = nullptr;
    fTruthTree = nullptr;
    fHitTree = nullptr;
  }

  void OutputManager::FillEvent(int eventID, double decayTime_ns, double activity_Bq){
    fEventID = eventID;
    fDecayTime_ns = decayTime_ns;
    fActivity_Bq = activity_Bq;
    fEventTree->Fill();
  }

  void OutputManager::FillTruth(int eventID, int trackID, int parentID, int pdg, const std::string& creatorProcess,
				double E_keV, double px, double py, double pz, double t_ns){
    fTruthEventID = eventID;
    fTruthTrackID = trackID;
    fTruthParentID = parentID;
    fTruthPDG = pdg;
    fTruthCreatorProcess = creatorProcess;
    fTruthE_keV = E_keV;
    fTruthPx = px;
    fTruthPy = py;
    fTruthPz = pz;
    fTruthT_ns = t_ns;
    fTruthTree->Fill();
  }

  void OutputManager::FillHit(int eventID, int detID, double e_true_keV, double e_meas_keV,
				   double t_true_ns, double t_meas_ns, double tabs_meas_ns){
    fDetEventID = eventID;
    fDetID = detID;
    fETrue_keV = e_true_keV;
    fEMeas_keV = e_meas_keV;
    fTTrue_ns = t_true_ns;
    fTMeas_ns = t_meas_ns;
    fTAbsMeas_ns = tabs_meas_ns;
    fHitTree->Fill();
  }

}

