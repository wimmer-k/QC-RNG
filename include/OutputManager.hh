#ifndef OutputManager_h
#define OutputManager_h 1

#include "TFile.h"
#include "TTree.h"

#include <string>

namespace QCRNG{

  class OutputManager{
  public:
    static OutputManager& Instance();

    void Open(const char* filename="qcrng.root");
    void Close();

    void FillEvent(int eventID, double decayTime_ns, double activity_Bq);
    void FillHit(int eventID, int detID, double e_true_keV, double e_meas_keV, double t_true_ns, double t_meas_ns, double tabs_meas_ns);
    void FillTruth(int eventID, int trackID, int parentID, int pdg, const std::string& creatorProcess,
		   double E_keV, double px, double py, double pz, double t_ns);
    
  private:
    OutputManager() = default;
    ~OutputManager() = default;

    TFile* fFile = nullptr;
    TTree* fEventTree = nullptr;
    TTree* fHitTree = nullptr;
    TTree* fTruthTree = nullptr;

    int fEventID;
    double fDecayTime_ns;
    double fActivity_Bq;

    int fDetEventID;
    int fDetID;
    double fETrue_keV;
    double fEMeas_keV;
    double fTTrue_ns;
    double fTMeas_ns;
    double fTAbsMeas_ns;

    int fTruthEventID;
    int fTruthTrackID;
    int fTruthParentID;
    int fTruthPDG;
    std::string fTruthCreatorProcess;
    double fTruthE_keV;
    double fTruthPx;
    double fTruthPy;
    double fTruthPz;
    double fTruthT_ns;
  };

}

#endif
