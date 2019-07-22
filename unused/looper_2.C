#include "looper.h"

using namespace std;

void looper_2()
{
  //===============================================================================|
  // TTZ LOOPER (BACKGROUND) 
  //===============================================================================|
  // Root directory String                                             
  TString directoryString = "/nfs-7/userdata/phchang/WWW_babies";
  directoryString += "/WWW2018_v5.1.6/merged/";

  cout << directoryString << endl;
  // Creates TChain to read in files for ttz
  TChain ttz_chain("t");
  ttz_chain.Add(directoryString + "ttz_ll_mll1_amcatnlo_1.root"     );
  ttz_chain.Add(directoryString + "ttz_llvv_mll10_amcatnlo_1.root"  );
  ttz_chain.Add(directoryString + "ttz_llvv_mll10_amcatnlo_2.root"  );
  ttz_chain.Add(directoryString + "ttz_llvv_mll10_amcatnlo_3.root"  );
  ttz_chain.Add(directoryString + "ttz_llvv_mll10_amcatnlo_4.root"  );
  ttz_chain.Add(directoryString + "ttz_llvv_mll10_amcatnlo_5.root"  );
  ttz_chain.Add(directoryString + "ttz_llvv_mll10_amcatnlo_6.root"  );
  ttz_chain.Add(directoryString + "ttz_llvv_mll10_amcatnlo_7.root"  );
  ttz_chain.Add(directoryString + "ttz_llvv_mll10_amcatnlo_8.root"  );
  ttz_chain.Add(directoryString + "ttz_llvv_mll10_amcatnlo_9.root"  );
  ttz_chain.Add(directoryString + "ttz_llvv_mll10_amcatnlo_10.root" );
  ttz_chain.Add(directoryString + "ttz_llvv_mll10_amcatnlo_11.root" );
  ttz_chain.Add(directoryString + "ttz_llvv_mll10_amcatnlo_12.root" );

  // Sets all used branches to active status
  ttz_chain.SetBranchStatus("*", 0);
  ttz_chain.SetBranchStatus("el_pt", 1);
  ttz_chain.SetBranchStatus("evt_scale1fb", 1);
  ttz_chain.SetBranchStatus("met_pt", 1);
  ttz_chain.SetBranchStatus("lep_pt", 1);
  ttz_chain.SetBranchStatus("nb", 1);
  ttz_chain.SetBranchStatus("MjjL", 1);
  ttz_chain.SetBranchStatus("nj30", 1);

  // Creates histograms to store data from TChain   
  TH1D* ttz_el_pt_hist = new TH1D("ttz_el_pt",
                                    "ttz background electron momentum",
                                    100, 20, 200);
  TH1D* ttz_nj30_hist = new TH1D("ttz_nj30", "ttz amount of jets",
				   9, 0.5, 9.5);
  TH1D* ttz_MjjL_hist = new TH1D("ttz_MjjL", "ttz leading MjjL",
                                   100, 20, 500);
  TH1D* ttz_met_pt_hist = new TH1D("ttz_met_pt", "ttz met momentum",
                                     100, 20, 200);
  TH1D* ttz_lep_pt_hist_1 = new TH1D("ttz_lep_pt_1", "ttz lep momentum 1",
				       100, 20, 200);
  TH1D* ttz_lep_pt_hist_2 = new TH1D("ttz_lep_pt_2", "ttz lep momentum 2",
                                       100, 20, 200);
  TH1D* ttz_nb_hist = new TH1D("ttz_nb", "ttz number of b jets",
                                 5, 0.5, 5.5);
  TH1D* ttz_n_lep_hist = new TH1D("ttz_n_lep", "ttz number of leptons",
                                    2, 1.5, 3.5);

  // Sets all branch addresses and declares reference variables  
  Float_t  ttz_el_pt;
  TBranch* ttz_el_pt_br;
  ttz_chain.SetBranchAddress("el_pt", &ttz_el_pt, &ttz_el_pt_br);

  Float_t  ttz_evt_scale1fb;
  TBranch* ttz_evt_scale1fb_br;
  ttz_chain.SetBranchAddress("evt_scale1fb", &ttz_evt_scale1fb,
			     &ttz_evt_scale1fb_br);

  Int_t    ttz_nj30;
  TBranch* ttz_nj30_br;
  ttz_chain.SetBranchAddress("nj30", &ttz_nj30, &ttz_nj30_br);

  Float_t  ttz_MjjL;
  TBranch* ttz_MjjL_br;
  ttz_chain.SetBranchAddress("MjjL", &ttz_MjjL, &ttz_MjjL_br);

  Float_t  ttz_met_pt;
  TBranch* ttz_met_pt_br;
  ttz_chain.SetBranchAddress("met_pt", &ttz_met_pt, &ttz_met_pt_br);

  vector<Float_t>* ttz_lep_pt = NULL;
  TBranch*         ttz_lep_pt_br;
  ttz_chain.SetBranchAddress("lep_pt", &ttz_lep_pt, &ttz_lep_pt_br);

  Int_t    ttz_nb;
  TBranch* ttz_nb_br;
  ttz_chain.SetBranchAddress("nb", &ttz_nb, &ttz_nb_br);

  // loops through ttbar data and fills various histograms                         
  Int_t ttz_n_entries = ttz_chain.GetEntries();
  Int_t entry_count = 0; // used for displaying entries to screen          

  for (UInt_t i = 0; i < ttz_n_entries; i++)
    {
      ttz_chain.GetEntry(i);
      if (ttz_el_pt > 20 && ttz_el_pt < 200)
        {
          entry_count++;
          ttz_el_pt_hist->Fill(ttz_el_pt, 150 * ttz_evt_scale1fb);
        }
      if (ttz_nb > 0)
        ttz_nb_hist->Fill(ttz_nb, 59 * ttz_evt_scale1fb);

      if (ttz_MjjL > 20)
        ttz_MjjL_hist->Fill(ttz_MjjL, 59 * ttz_evt_scale1fb);

      if (ttz_met_pt > 20 && ttz_met_pt < 200)
        ttz_met_pt_hist->Fill(ttz_met_pt, 59 * ttz_evt_scale1fb);

      if (ttz_lep_pt->at(0) > 20 && ttz_lep_pt->at(0) < 200)
        ttz_lep_pt_hist_1->Fill(ttz_lep_pt->at(0), 59 * ttz_evt_scale1fb);

      if (ttz_lep_pt->at(1) > 20 && ttz_lep_pt->at(1) < 200)
        ttz_lep_pt_hist_2->Fill(ttz_lep_pt->at(1), 59 * ttz_evt_scale1fb);

      if (ttz_nj30 > 0)
        ttz_nj30_hist->Fill(ttz_nj30, 59 * ttz_evt_scale1fb);

      if (ttz_lep_pt->size() > 0)
        ttz_n_lep_hist->Fill(ttz_lep_pt->size(), 59 * ttz_evt_scale1fb);
    }
  // Displays number of events in histogram                                   
  cout << "==========================" << endl;
  cout << "ttz: " << entry_count << " events." << endl;
  cout << "==========================" << endl;

  // Output File                                                                    
  TFile* ttz_output_file = new TFile("ttz_distributions_2018.root", "recreate");
  ttz_output_file->cd();
  ttz_el_pt_hist->Write();
  ttz_nb_hist->Write();
  ttz_MjjL_hist->Write();
  ttz_met_pt_hist->Write();
  ttz_lep_pt_hist_1->Write();
  ttz_lep_pt_hist_2->Write();
  ttz_nj30_hist->Write();
  ttz_n_lep_hist->Write();
  ttz_output_file->Write();
  ttz_output_file->Close();
  //===============================================================================|
}
