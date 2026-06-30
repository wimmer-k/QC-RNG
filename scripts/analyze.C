#include <map>
#include <vector>
#include <algorithm>
#include <iostream>

struct Hit{
  int det;
  double E;
  double t;
  double tabs;
};

bool IsOpposite(int a, int b){
  return (a==0 && b==1) || (a==1 && b==0) ||
         (a==2 && b==3) || (a==3 && b==2) ||
         (a==4 && b==5) || (a==5 && b==4);
}
bool IsNeighbor(int a, int b){
  return a != b && !IsOpposite(a,b);
}
void analyze(const char* infile="data/test.root", const char* outfile="data/analysis.root"){
  auto f = TFile::Open(infile);
  auto hits = (TTree*)f->Get("hits");

  int eventID, detID;
  double E_meas, t_meas;
  double tabs_meas;

  hits->SetBranchAddress("eventID", &eventID);
  hits->SetBranchAddress("detID", &detID);
  hits->SetBranchAddress("E_meas", &E_meas);
  hits->SetBranchAddress("t_meas", &t_meas);
  hits->SetBranchAddress("tabs_meas", &tabs_meas);

  std::map<int,std::vector<Hit>> ev;
  Long64_t nEntries = hits->GetEntries();
  for(Long64_t i=0; i<nEntries; i++){
    if(i % 100000 == 0 || i == nEntries - 1){
      double frac = double(i + 1) / double(nEntries);
      int percent = int(100.0 * frac);

      std::cout << "\rReading tree: "
		<< percent << "%  ("
		<< i + 1 << "/" << nEntries << ")"
		<< std::flush;
    }

    hits->GetEntry(i);
    ev[eventID].push_back({detID, E_meas, t_meas, tabs_meas});
  }
  std::cout << std::endl;
  
  auto fout = new TFile(outfile, "RECREATE");

  auto hMult = new TH1D("hMult", "Hit multiplicity;N hits;Counts", 10, -0.5, 9.5);
  auto hE = new TH1D("hE", "Single detector energy;E [keV];Counts", 400, 0, 2000);
  auto hEtot = new TH1D("hEtot", "Total event energy;#Sigma E [keV];Counts", 600, 0, 3000);
  auto hIDcoinc = new TH2D("hIDcoinc", "detector pairs in coincidence;det 1;det 2", 6, -0.5, 5.5, 6, -0.5, 5.5);
  auto hIDcoinc511 = new TH2D("hIDcoinc511", "511-511 detector pairs in coincidence;det 1;det 2", 6, -0.5, 5.5, 6, -0.5, 5.5);
  auto hIDcoincadd = new TH2D("hIDcoincadd", "detector pairs in coincidence to be addbacked;det 1;det 2", 6, -0.5, 5.5, 6, -0.5, 5.5);

  auto hdt = new TH1D("hdt", "All hit pairs;#Delta t [ns];Counts", 400, -10, 10);
  auto hdt_opp = new TH1D("hdt_opp", "Opposite detector pairs;#Delta t [ns];Counts", 400, -10, 10);
  auto hdt_511 = new TH1D("hdt_511", "511-511 pairs;#Delta t [ns];Counts", 400, -10, 10);
  auto hdt_511opp = new TH1D("hdt_511opp", "Opposite 511-511 pairs;#Delta t [ns];Counts", 400, -10, 10);

  auto hE_addback_neigh = new TH1D("hE_addback_neigh", "Neighbour addback;E_{1}+E_{2} [keV];Counts", 400, 0, 2000);

  auto hdt_event = new TH1D("hdt_event","Time difference between consecutive events;#Delta t_{event} [ns];Counts", 1000, 0, 1e5);
  auto hdt_event_tabs = new TH2D("hdt_event_tabs","Time difference between consecutive events vs absolute time;t_{abs} [s] ;#Delta t_{event} [s]", 1000, 0, 1e11, 1000, 0, 1e5);
  
  auto hdt_event_good = new TH1D("hdt_event_good","Time difference between consecutive events;#Delta t_{event} [ns];Counts", 1000, 0, 1e5);
  auto hdt_event_good_tabs = new TH2D("hdt_event_good_tabs","Time difference between consecutive events vs absolute time;t_{abs} [s] ;#Delta t_{event} [s]", 1000, 0, 1e11, 1000, 0, 1e5);
  
  double t_prev_event = -1.;
  double t_prev_event_good = -1.;

  Long64_t nEvents = ev.size();
  Long64_t iev = 0;
  
  for(auto& [eid, hs] : ev){
    if(iev % 10000 == 0 || iev == nEvents - 1){
      double frac = double(iev + 1) / double(nEvents);
      int percent = int(100.0 * frac);

      std::cout << "\rAnalyzing events: "
		<< percent << "%  ("
		<< iev + 1 << "/" << nEvents << ")"
		<< std::flush;
    }
    iev++;

    
    hMult->Fill(hs.size());

    double Etot = 0.;
    double t_event = hs[0].tabs;
    for(auto& h : hs){
      Etot += h.E;
      hE->Fill(h.E);
      if(h.tabs < t_event)
	t_event = h.tabs;
    }
    hEtot->Fill(Etot);


    //time to previous event
    if(t_prev_event>0){
      hdt_event->Fill((t_event - t_prev_event)/1e9);
      hdt_event_tabs->Fill(t_event/1e9,(t_event - t_prev_event)/1e9);
      //cout << t_event << "\t" << t_prev_event << "\t" << t_event - t_prev_event << endl;

    }
    t_prev_event = t_event;
    

    for(size_t i=0; i<hs.size(); i++){
      for(size_t j=i+1; j<hs.size(); j++){
        auto& h1 = hs[i];
        auto& h2 = hs[j];

	bool e1_511 = std::abs(h1.E - 511.) < 50.;
	bool e2_511 = std::abs(h2.E - 511.) < 50.;

        double dt = h2.t - h1.t;
	bool opposite = IsOpposite(h1.det, h2.det);
	bool neighbor = IsNeighbor(h1.det, h2.det);
	bool annihi = e1_511 && e2_511;
	
        hdt->Fill(dt);
	
	if(annihi){
	  hdt_511->Fill(dt);
	}
	
        if(opposite){
          hdt_opp->Fill(dt);
	  if(annihi){
	    hdt_511opp->Fill(dt);
	  }
	}
	
	if(fabs(dt)>5)
	  continue;
	
	hIDcoinc->Fill(h1.det, h2.det);
	
        if(neighbor){
          hE_addback_neigh->Fill(h1.E + h2.E);
          hIDcoincadd->Fill(h1.det, h2.det);

	}


	if(annihi){
	  hIDcoinc511->Fill(h1.det, h2.det);
	  
	}

	if(annihi&&opposite){
	  if(t_prev_event_good>0){
	    hdt_event_good->Fill((t_event - t_prev_event_good)/1e9);
	    hdt_event_good_tabs->Fill(t_event/1e9,(t_event - t_prev_event_good)/1e9);
	    //cout << t_event << "\t" << t_prev_event << "\t" << t_event - t_prev_event << endl;

	  }
	  t_prev_event_good = t_event;

	}
      }
    }
  }

  fout->Write();
  fout->Close();

  std::cout << std::endl << "Wrote " << outfile << std::endl;
}
