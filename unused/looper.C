#include "looper.h"

using namespace std;

void looper()
{
  //===============================================================================|
  // TTBAR LOOPER (BACKGROUND)
  //===============================================================================|
  // Root directory String
  TString directoryString = "/nfs-7/userdata/phchang/WWW_babies";
  directoryString += "/WWW2018_v5.1.6/merged/";

  cout << directoryString << endl;
  // Creates TChain to read in files for ttbar
  TChain ttbar_chain("t");
  ttbar_chain.Add(directoryString + "ttbar_1ltbr_madgraph_1.root");
  ttbar_chain.Add(directoryString + "ttbar_1ltop_madgraph_1.root");
  ttbar_chain.Add(directoryString + "ttbar_semilep_powheg_1.root");
  ttbar_chain.Add(directoryString + "ttbar_dilep_powheg_1.root"  );
  ttbar_chain.Add(directoryString + "ttbar_dilep_powheg_2.root"  );

  // Sets all used branches to active status
  ttbar_chain.SetBranchStatus("*", 0);
  ttbar_chain.SetBranchStatus("el_pt", 1);
  ttbar_chain.SetBranchStatus("evt_scale1fb", 1);
  ttbar_chain.SetBranchStatus("met_pt", 1);
  ttbar_chain.SetBranchStatus("lep_pt", 1);
  ttbar_chain.SetBranchStatus("nb", 1);
  ttbar_chain.SetBranchStatus("MjjL", 1);
  ttbar_chain.SetBranchStatus("nj30", 1);
  
  // Creates histograms to store data from TChain
  TH1D* ttbar_el_pt_hist = new TH1D("ttbar_el_pt", 
				    "ttbar background electron momentum",
				    100, 20, 200);
  TH1D* ttbar_nj30_hist = new TH1D("ttbar_nj30", "ttbar amount of jets",
				     9, 0.5, 9.5);
  TH1D* ttbar_MjjL_hist = new TH1D("ttbar_MjjL", "ttbar leading MjjL", 
				   100, 20, 500);
  TH1D* ttbar_met_pt_hist = new TH1D("ttbar_met_pt", "ttbar met momentum", 
				     100, 20, 200);
  TH1D* ttbar_lep_pt_hist_1 = new TH1D("ttbar_lep_pt_1", "ttbar lep momentum 1",
				     100, 20, 200);
  TH1D* ttbar_lep_pt_hist_2 = new TH1D("ttbar_lep_pt_2", "ttbar lep momentum 2",
				       100, 20, 200);
  TH1D* ttbar_nb_hist = new TH1D("ttbar_nb", "ttbar number of b jets",
				 5, 0.5, 5.5);
  TH1D* ttbar_n_lep_hist = new TH1D("ttbar_n_lep", "ttbar number of leptons", 
				    2, 1.5, 3.5);

  // Sets all branch addresses and declares reference variables
  Float_t  ttbar_el_pt;
  TBranch* ttbar_el_pt_br;
  ttbar_chain.SetBranchAddress("el_pt", &ttbar_el_pt, &ttbar_el_pt_br);

  Float_t  ttbar_evt_scale1fb;
  TBranch* ttbar_evt_scale1fb_br;
  ttbar_chain.SetBranchAddress("evt_scale1fb", &ttbar_evt_scale1fb, 
			       &ttbar_evt_scale1fb_br);

  Int_t    ttbar_nj30;
  TBranch* ttbar_nj30_br;
  ttbar_chain.SetBranchAddress("nj30", &ttbar_nj30, &ttbar_nj30_br);

  Float_t  ttbar_MjjL;
  TBranch* ttbar_MjjL_br;
  ttbar_chain.SetBranchAddress("MjjL", &ttbar_MjjL, &ttbar_MjjL_br);

  Float_t  ttbar_met_pt;
  TBranch* ttbar_met_pt_br;
  ttbar_chain.SetBranchAddress("met_pt", &ttbar_met_pt, &ttbar_met_pt_br);

  vector<Float_t>* ttbar_lep_pt = NULL;
  TBranch*         ttbar_lep_pt_br;
  ttbar_chain.SetBranchAddress("lep_pt", &ttbar_lep_pt, &ttbar_lep_pt_br);

  Int_t    ttbar_nb;
  TBranch* ttbar_nb_br;
  ttbar_chain.SetBranchAddress("nb", &ttbar_nb, &ttbar_nb_br);

  // loops through ttbar data and fills various histograms
  Int_t ttbar_n_entries = ttbar_chain.GetEntries();
  Int_t entry_count = 0; // used for displaying entries to screen

  for (UInt_t i = 0; i < ttbar_n_entries; i++)
    {
      ttbar_chain.GetEntry(i);
      if (ttbar_el_pt > 20 && ttbar_el_pt < 200)
	{
	  entry_count++;
	  ttbar_el_pt_hist->Fill(ttbar_el_pt, 150 * ttbar_evt_scale1fb);
	}
      if (ttbar_nb > 0)
	ttbar_nb_hist->Fill(ttbar_nb, 59 * ttbar_evt_scale1fb);
      
      if (ttbar_MjjL > 20)
	ttbar_MjjL_hist->Fill(ttbar_MjjL, 59 * ttbar_evt_scale1fb);
      
      if (ttbar_met_pt > 20 && ttbar_met_pt < 200)
	ttbar_met_pt_hist->Fill(ttbar_met_pt, 59 * ttbar_evt_scale1fb);
      
      if (ttbar_lep_pt->at(0) > 20 && ttbar_lep_pt->at(0) < 200)
	ttbar_lep_pt_hist_1->Fill(ttbar_lep_pt->at(0), 59 * ttbar_evt_scale1fb);
      
      if (ttbar_lep_pt->at(1) > 20 && ttbar_lep_pt->at(1) < 200)
	ttbar_lep_pt_hist_2->Fill(ttbar_lep_pt->at(1), 59 * ttbar_evt_scale1fb);
      
      if (ttbar_nj30 > 0)
	ttbar_nj30_hist->Fill(ttbar_nj30, 59 * ttbar_evt_scale1fb);
      
      if (ttbar_lep_pt->size() > 0)
	ttbar_n_lep_hist->Fill(ttbar_lep_pt->size(), 59 * ttbar_evt_scale1fb);
    }
  // Displays number of events in histogram
  cout << "==========================" << endl;
  cout << "ttbar: " << entry_count << " events." << endl;
  cout << "==========================" << endl;

  // Output File
  TFile* ttbar_output_file = new TFile("ttbar_distributions_2018.root", "recreate");
  ttbar_output_file->cd();
  ttbar_el_pt_hist->Write();
  ttbar_nb_hist->Write();
  ttbar_MjjL_hist->Write();
  ttbar_met_pt_hist->Write();
  ttbar_lep_pt_hist_1->Write();
  ttbar_lep_pt_hist_2->Write();
  ttbar_nj30_hist->Write();
  ttbar_n_lep_hist->Write();
  ttbar_output_file->Write();
  ttbar_output_file->Close();
  //===============================================================================|

  //===============================================================================|
  // TTGAM LOOPER (BACKGROUND)
  //===============================================================================|
  // Creates TChain to read in files for ttgam
  TChain ttgam_chain("t");
  ttgam_chain.Add(directoryString + "ttgam_1ltbr_madgraph_1.root");
  ttgam_chain.Add(directoryString + "ttgam_1ltop_madgraph_1.root");
  ttgam_chain.Add(directoryString + "ttgam_dilep_madgraph_1.root");

  // Sets all used branches to active status
  ttgam_chain.SetBranchStatus("*", 0);
  ttgam_chain.SetBranchStatus("el_pt", 1);
  ttgam_chain.SetBranchStatus("evt_scale1fb", 1);
  ttgam_chain.SetBranchStatus("met_pt", 1);
  ttgam_chain.SetBranchStatus("lep_pt", 1);
  ttgam_chain.SetBranchStatus("nb", 1);
  ttgam_chain.SetBranchStatus("MjjL", 1);
  ttgam_chain.SetBranchStatus("nj30", 1);
  
  // Creates histograms to store data from TChain
  TH1D* ttgam_el_pt_hist = new TH1D("ttgam_el_pt", 
				    "ttgam background electron momentum",
				    100, 20, 200);
  TH1D* ttgam_nj30_hist = new TH1D("ttgam_nj30", "ttgam amount of jets",
				     9, 0.5, 9.5);
  TH1D* ttgam_MjjL_hist = new TH1D("ttgam_MjjL", "ttgam leading MjjL", 
				   100, 20, 500);
  TH1D* ttgam_met_pt_hist = new TH1D("ttgam_met_pt", "ttgam met momentum", 
				     100, 20, 200);
  TH1D* ttgam_lep_pt_hist_1 = new TH1D("ttgam_lep_pt_1", "ttgam lep momentum 1",
				     100, 20, 200);
  TH1D* ttgam_lep_pt_hist_2 = new TH1D("ttgam_lep_pt_2", "ttgam lep momentum 2",
				       100, 20, 200);
  TH1D* ttgam_nb_hist = new TH1D("ttgam_nb", "ttgam number of b jets",
				 5, 0.5, 5.5);
  TH1D* ttgam_n_lep_hist = new TH1D("ttgam_n_lep", "ttgam number of leptons", 
				    2, 1.5, 3.5);

  // Sets all branch addresses and declares reference variables
  Float_t  ttgam_el_pt;
  TBranch* ttgam_el_pt_br;
  ttgam_chain.SetBranchAddress("el_pt", &ttgam_el_pt, &ttgam_el_pt_br);

  Float_t  ttgam_evt_scale1fb;
  TBranch* ttgam_evt_scale1fb_br;
  ttgam_chain.SetBranchAddress("evt_scale1fb", &ttgam_evt_scale1fb, 
			       &ttgam_evt_scale1fb_br);

  Int_t    ttgam_nj30;
  TBranch* ttgam_nj30_br;
  ttgam_chain.SetBranchAddress("nj30", &ttgam_nj30, &ttgam_nj30_br);

  Float_t  ttgam_MjjL;
  TBranch* ttgam_MjjL_br;
  ttgam_chain.SetBranchAddress("MjjL", &ttgam_MjjL, &ttgam_MjjL_br);

  Float_t  ttgam_met_pt;
  TBranch* ttgam_met_pt_br;
  ttgam_chain.SetBranchAddress("met_pt", &ttgam_met_pt, &ttgam_met_pt_br);

  vector<Float_t>* ttgam_lep_pt = NULL;
  TBranch*         ttgam_lep_pt_br;
  ttgam_chain.SetBranchAddress("lep_pt", &ttgam_lep_pt, &ttgam_lep_pt_br);

  Int_t    ttgam_nb;
  TBranch* ttgam_nb_br;
  ttgam_chain.SetBranchAddress("nb", &ttgam_nb, &ttgam_nb_br);

  // loops through ttgam data and fills various histograms
  Int_t ttgam_n_entries = ttgam_chain.GetEntries();
  entry_count = 0; // used for displaying entries to screen

  for (UInt_t i = 0; i < ttgam_n_entries; i++)
    {
      ttgam_chain.GetEntry(i);
      if (ttgam_el_pt > 20 && ttgam_el_pt < 200)
	{
	  entry_count++;
	  ttgam_el_pt_hist->Fill(ttgam_el_pt, 150 * ttgam_evt_scale1fb);
	}
      if (ttgam_nb > 0)
	ttgam_nb_hist->Fill(ttgam_nb, 59 * ttgam_evt_scale1fb);
      
      if (ttgam_MjjL > 20)
	ttgam_MjjL_hist->Fill(ttgam_MjjL, 59 * ttgam_evt_scale1fb);
      
      if (ttgam_met_pt > 20 && ttgam_met_pt < 200)
	ttgam_met_pt_hist->Fill(ttgam_met_pt, 59 * ttgam_evt_scale1fb);
      
      if (ttgam_lep_pt->at(0) > 20 && ttgam_lep_pt->at(0) < 200)
	ttgam_lep_pt_hist_1->Fill(ttgam_lep_pt->at(0), 59 * ttgam_evt_scale1fb);
      
      if (ttgam_lep_pt->at(1) > 20 && ttgam_lep_pt->at(1) < 200)
	ttgam_lep_pt_hist_2->Fill(ttgam_lep_pt->at(1), 59 * ttgam_evt_scale1fb);
      
      if (ttgam_nj30 > 0)
	ttgam_nj30_hist->Fill(ttgam_nj30, 59 * ttgam_evt_scale1fb);
      
      if (ttgam_lep_pt->size() > 0)
	ttgam_n_lep_hist->Fill(ttgam_lep_pt->size(), 59 * ttgam_evt_scale1fb);
    }
  // Displays number of events in histogram
  cout << "==========================" << endl;
  cout << "ttgam: " << entry_count << " events." << endl;
  cout << "==========================" << endl;

  // Output File
  TFile* ttgam_output_file = new TFile("ttgam_distributions_2018.root", "recreate");
  ttgam_output_file->cd();
  ttgam_el_pt_hist->Write();
  ttgam_nb_hist->Write();
  ttgam_MjjL_hist->Write();
  ttgam_met_pt_hist->Write();
  ttgam_lep_pt_hist_1->Write();
  ttgam_lep_pt_hist_2->Write();
  ttgam_nj30_hist->Write();
  ttgam_n_lep_hist->Write();
  ttgam_output_file->Write();
  ttgam_output_file->Close();
  //===============================================================================|
  
  //===============================================================================|
  // WZ LOOPER (BACKGROUND)
  //===============================================================================|
  // Creates Chain for WZ background el pt
  TChain wz_chain("t");
  wz_chain.Add(directoryString + "wz_3lv_amcatnlo_1.root" );
  wz_chain.Add(directoryString + "wz_3lv_amcatnlo_2.root" );
  wz_chain.Add(directoryString + "wz_3lv_amcatnlo_3.root" );
  wz_chain.Add(directoryString + "wz_3lv_amcatnlo_4.root" );
  wz_chain.Add(directoryString + "wz_3lv_amcatnlo_5.root" );
  wz_chain.Add(directoryString + "wz_3lv_amcatnlo_6.root" );
  wz_chain.Add(directoryString + "wz_3lv_amcatnlo_7.root" );
  wz_chain.Add(directoryString + "wz_3lv_amcatnlo_8.root" );
  wz_chain.Add(directoryString + "wz_3lv_amcatnlo_9.root" );
  wz_chain.Add(directoryString + "wz_3lv_amcatnlo_10.root");
  wz_chain.Add(directoryString + "wz_3lv_amcatnlo_11.root");

  // Sets all used branches to active status
  wz_chain.SetBranchStatus("*", 0);
  wz_chain.SetBranchStatus("el_pt", 1);
  wz_chain.SetBranchStatus("evt_scale1fb", 1);
  wz_chain.SetBranchStatus("met_pt", 1);
  wz_chain.SetBranchStatus("lep_pt", 1);
  wz_chain.SetBranchStatus("nb", 1);
  wz_chain.SetBranchStatus("MjjL", 1);
  wz_chain.SetBranchStatus("nj30", 1);

  // Creates all histograms for wz TChain
  TH1D* wz_el_pt_hist = new TH1D("wz_el_pt",
                                    "wz background electron momentum",
                                    100, 20, 200);
  TH1D* wz_nj30_hist = new TH1D("wz_nj30", "wz amount of jets",
                                     9, 0.5, 9.5);
  TH1D* wz_MjjL_hist = new TH1D("wz_MjjL", "wz leading MjjL",
                                   100, 20, 500);
  TH1D* wz_met_pt_hist = new TH1D("wz_met_pt", "wz met momentum",
                                     100, 20, 200);
  TH1D* wz_lep_pt_hist_1 = new TH1D("wz_lep_pt_1", "wz lep momentum 1",
				       100, 20, 200);
  TH1D* wz_lep_pt_hist_2 = new TH1D("wz_lep_pt_2", "wz lep momentum 2",
                                       100, 20, 200);
  TH1D* wz_nb_hist = new TH1D("wz_nb", "wz number of b jets",
                                 5, 0.5, 5.5);
  TH1D* wz_n_lep_hist = new TH1D("wz_n_lep", "wz number of leptons",
				 2, 1.5, 3.5);


  // Sets branch addresses
  Float_t  wz_el_pt;
  TBranch* wz_el_pt_br;
  wz_chain.SetBranchAddress("el_pt", &wz_el_pt, &wz_el_pt_br);

  Float_t  wz_evt_scale1fb;
  TBranch* wz_evt_scale1fb_br;
  wz_chain.SetBranchAddress("evt_scale1fb", &wz_evt_scale1fb,
                               &wz_evt_scale1fb_br);

  Int_t    wz_nj30;
  TBranch* wz_nj30_br;
  wz_chain.SetBranchAddress("nj30", &wz_nj30, &wz_nj30_br);

  Float_t  wz_MjjL;
  TBranch* wz_MjjL_br;
  wz_chain.SetBranchAddress("MjjL", &wz_MjjL, &wz_MjjL_br);

  Float_t  wz_met_pt;
  TBranch* wz_met_pt_br;
  wz_chain.SetBranchAddress("met_pt", &wz_met_pt, &wz_met_pt_br);

  vector<Float_t>* wz_lep_pt = NULL;
  TBranch*         wz_lep_pt_br;
  wz_chain.SetBranchAddress("lep_pt", &wz_lep_pt, &wz_lep_pt_br);

  Int_t    wz_nb;
  TBranch* wz_nb_br;
  wz_chain.SetBranchAddress("nb", &wz_nb, &wz_nb_br);

  // Loops through all data for wz
  Int_t wz_n_entries = wz_chain.GetEntries();
  entry_count = 0;
  for (UInt_t j = 0; j < wz_n_entries; j++)
    {
      wz_chain.GetEntry(j);
	if (wz_el_pt > 20 && wz_el_pt < 200)
        {
          entry_count++;
          wz_el_pt_hist->Fill(wz_el_pt, 59 * wz_evt_scale1fb);
        }
      if (wz_nb > 0)
        wz_nb_hist->Fill(wz_nb, 59 * wz_evt_scale1fb);

      if (wz_MjjL > 20)
        wz_MjjL_hist->Fill(wz_MjjL, 59 * wz_evt_scale1fb);

      if (wz_met_pt > 20 && wz_met_pt < 200)
        wz_met_pt_hist->Fill(wz_met_pt, 59 * wz_evt_scale1fb);

      if (wz_lep_pt->at(0) > 20 && wz_lep_pt->at(0) < 200)
        wz_lep_pt_hist_1->Fill(wz_lep_pt->at(0), 59 * wz_evt_scale1fb);

      if (wz_lep_pt->at(1) > 20 && wz_lep_pt->at(1) < 200)
        wz_lep_pt_hist_2->Fill(wz_lep_pt->at(1), 59 * wz_evt_scale1fb);

      if (wz_nj30 > 0)
        wz_nj30_hist->Fill(wz_nj30, 59 * wz_evt_scale1fb);

      if (wz_lep_pt->size() > 0)
	wz_n_lep_hist->Fill(wz_lep_pt->size(), 59 * wz_evt_scale1fb);
    }

  // Displays number of events in histogram  
  cout << "==========================" << endl;
  cout << "wz: " << entry_count << " events." << endl;
  cout << "==========================" << endl;

  // Output File
  TFile* wz_output_file = new TFile("wz_distributions_2018.root", "recreate");
  wz_output_file->cd();
  wz_el_pt_hist->Write();
  wz_nb_hist->Write();
  wz_MjjL_hist->Write();
  wz_met_pt_hist->Write();
  wz_lep_pt_hist_1->Write();
  wz_lep_pt_hist_2->Write();
  wz_nj30_hist->Write();
  wz_n_lep_hist->Write();
  wz_output_file->Write();
  wz_output_file->Close();
  //===============================================================================|

  //===============================================================================|
  // WJETS LOOPER (BACKGROUND)
  //===============================================================================|
  // Creates Chain for wjets
  TChain wjets_chain("t");
  wjets_chain.Add(directoryString + "wjets_ht100_madgraph_1.root" );
  wjets_chain.Add(directoryString + "wjets_ht1200_madgraph_1.root");
  wjets_chain.Add(directoryString + "wjets_ht200_madgraph_1.root" );
  wjets_chain.Add(directoryString + "wjets_ht2500_madgraph_1.root");
  wjets_chain.Add(directoryString + "wjets_ht400_madgraph_1.root" );
  wjets_chain.Add(directoryString + "wjets_ht600_madgraph_1.root" );
  wjets_chain.Add(directoryString + "wjets_ht800_madgraph_1.root" );
  wjets_chain.Add(directoryString + "wjets_incl_madgraph_1.root"  );

  // Sets used branches to active status
  wjets_chain.SetBranchStatus("*", 0);
  wjets_chain.SetBranchStatus("el_pt", 1);
  wjets_chain.SetBranchStatus("evt_scale1fb", 1);
  wjets_chain.SetBranchStatus("met_pt", 1);
  wjets_chain.SetBranchStatus("lep_pt", 1);
  wjets_chain.SetBranchStatus("nb", 1);
  wjets_chain.SetBranchStatus("MjjL", 1);
  wjets_chain.SetBranchStatus("nj30", 1);

  // Creates all histograms for wjets TChain
  TH1D* wjets_el_pt_hist = new TH1D("wjets_el_pt",
                                    "wjets background electron momentum",
                                    100, 20, 200);
  TH1D* wjets_nj30_hist = new TH1D("wjets_nj30", "wjets amount of jets",
                                     9, 0.5, 9.5);
  TH1D* wjets_MjjL_hist = new TH1D("wjets_MjjL", "wjets leading MjjL",
                                   100, 20, 500);
  TH1D* wjets_met_pt_hist = new TH1D("wjets_met_pt", "wjets met momentum",
                                     100, 20, 200);
  TH1D* wjets_lep_pt_hist_1 = new TH1D("wjets_lep_pt_1", "wjets lep momentum 1",
				       100, 20, 200);
  TH1D* wjets_lep_pt_hist_2 = new TH1D("wjets_lep_pt_2", "wjets lep momentum 2",
                                       100, 20, 200);
  TH1D* wjets_nb_hist = new TH1D("wjets_nb", "wjets number of b jets",
                                 5, 0.5, 5.5);
  TH1D* wjets_n_lep_hist = new TH1D("wjets_n_lep", "wjets number of leptons",
				    2, 1.5, 3.5);

  // Sets branch addresses
  Float_t  wjets_el_pt;
  TBranch* wjets_el_pt_br;
  wjets_chain.SetBranchAddress("el_pt", &wjets_el_pt, &wjets_el_pt_br);

  Float_t  wjets_evt_scale1fb;
  TBranch* wjets_evt_scale1fb_br;
  wjets_chain.SetBranchAddress("evt_scale1fb", &wjets_evt_scale1fb,
                               &wjets_evt_scale1fb_br);

  Int_t    wjets_nj30;
  TBranch* wjets_nj30_br;
  wjets_chain.SetBranchAddress("nj30", &wjets_nj30, &wjets_nj30_br);

  Float_t  wjets_MjjL;
  TBranch* wjets_MjjL_br;
  wjets_chain.SetBranchAddress("MjjL", &wjets_MjjL, &wjets_MjjL_br);

  Float_t  wjets_met_pt;
  TBranch* wjets_met_pt_br;
  wjets_chain.SetBranchAddress("met_pt", &wjets_met_pt, &wjets_met_pt_br);

  vector<Float_t>* wjets_lep_pt = NULL;
  TBranch*         wjets_lep_pt_br;
  wjets_chain.SetBranchAddress("lep_pt", &wjets_lep_pt, &wjets_lep_pt_br);

  Int_t    wjets_nb;
  TBranch* wjets_nb_br;
  wjets_chain.SetBranchAddress("nb", &wjets_nb, &wjets_nb_br);
  
  // Loops through all data for wjets
  Int_t wjets_n_entries = wjets_chain.GetEntries();
  entry_count = 0;

  for (UInt_t k = 0; k < wjets_n_entries; k++)
    {
      wjets_chain.GetEntry(k);
      if (wjets_el_pt > 20 && wjets_el_pt < 200)
        {
          entry_count++;
          wjets_el_pt_hist->Fill(wjets_el_pt, 59 * wjets_evt_scale1fb);
        }
      if (wjets_nb > 0)
        wjets_nb_hist->Fill(wjets_nb, 59 * wjets_evt_scale1fb);

      if (wjets_MjjL > 20)
        wjets_MjjL_hist->Fill(wjets_MjjL, 59 * wjets_evt_scale1fb);

      if (wjets_met_pt > 20 && wjets_met_pt < 200)
        wjets_met_pt_hist->Fill(wjets_met_pt, 59 * wjets_evt_scale1fb);

      if (wjets_lep_pt->at(0) > 20 && wjets_lep_pt->at(0) < 200)
        wjets_lep_pt_hist_1->Fill(wjets_lep_pt->at(0), 59 * wjets_evt_scale1fb);

      if (wjets_lep_pt->at(1) > 20 && wjets_lep_pt->at(1) < 200)
        wjets_lep_pt_hist_2->Fill(wjets_lep_pt->at(1), 59 * wjets_evt_scale1fb);

      if (wjets_nj30 > 0)
        wjets_nj30_hist->Fill(wjets_nj30, 59 * wjets_evt_scale1fb);

      if (wjets_lep_pt->size() > 0)
	wjets_n_lep_hist->Fill(wjets_lep_pt->size(), 59 * wjets_evt_scale1fb);

    }

  // Displays number of events in histogram                                      
  cout << "==========================" << endl;
  cout << "wjets: " << entry_count << " events." << endl;
  cout << "==========================" << endl;

  // Output File
  TFile* wjets_output_file = new TFile("wjets_distributions_2018.root", "recreate");
  wjets_output_file->cd();
  wjets_el_pt_hist->Write();
  wjets_nb_hist->Write();
  wjets_MjjL_hist->Write();
  wjets_met_pt_hist->Write();
  wjets_lep_pt_hist_1->Write();
  wjets_lep_pt_hist_2->Write();
  wjets_nj30_hist->Write();
  wjets_n_lep_hist->Write();
  wjets_output_file->Write();
  wjets_output_file->Close();
  //===============================================================================|

  //===============================================================================|
  // WWW LOOPER (SIGNAL)
  //===============================================================================|
  // Creates Chain
  TChain www_chain("t");
  www_chain.Add(directoryString + "www_amcatnlo_dilepfilter_1.root");
  
  // Sets all used branches to active status
  www_chain.SetBranchStatus("*", 0);
  www_chain.SetBranchStatus("el_pt", 1);
  www_chain.SetBranchStatus("evt_scale1fb", 1);
  www_chain.SetBranchStatus("met_pt", 1);
  www_chain.SetBranchStatus("lep_pt", 1);
  www_chain.SetBranchStatus("nb", 1);
  www_chain.SetBranchStatus("MjjL", 1);
  www_chain.SetBranchStatus("nj30", 1);

  // Creates all histograms for www TChain
  TH1D* www_el_pt_hist = new TH1D("www_el_pt",
                                    "www signal electron momentum",
                                    100, 20, 200);
  TH1D* www_nj30_hist = new TH1D("www_nj30", "www amount of jets",
				 9, 0.5, 9.5);
  TH1D* www_MjjL_hist = new TH1D("www_MjjL", "www leading MjjL",
                                   100, 20, 500);
  TH1D* www_met_pt_hist = new TH1D("www_met_pt", "www met momentum",
                                     100, 20, 200);
  TH1D* www_lep_pt_hist_1 = new TH1D("www_lep_pt_1", "www lep momentum 1",
				       100, 20, 200);
  TH1D* www_lep_pt_hist_2 = new TH1D("www_lep_pt_2", "www lep momentum 2",
                                       100, 20, 200);
  TH1D* www_nb_hist = new TH1D("www_nb", "www number of b jets",
                                 5, 0.5, 5.5);
  TH1D* www_n_lep_hist = new TH1D("www_n_lep", "www number of leptons",
				  2, 1.5, 3.5);

  // Sets all branch addresses
  Float_t  www_el_pt;
  TBranch* www_el_pt_br;
  www_chain.SetBranchAddress("el_pt", &www_el_pt, &www_el_pt_br);

  Float_t  www_evt_scale1fb;
  TBranch* www_evt_scale1fb_br;
  www_chain.SetBranchAddress("evt_scale1fb", &www_evt_scale1fb,
                               &www_evt_scale1fb_br);

  Int_t    www_nj30;
  TBranch* www_nj30_br;
  www_chain.SetBranchAddress("nj30", &www_nj30, &www_nj30_br);

  Float_t  www_MjjL;
  TBranch* www_MjjL_br;
  www_chain.SetBranchAddress("MjjL", &www_MjjL, &www_MjjL_br);

  Float_t  www_met_pt;
  TBranch* www_met_pt_br;
  www_chain.SetBranchAddress("met_pt", &www_met_pt, &www_met_pt_br);

  vector<Float_t>* www_lep_pt = NULL;
  TBranch*         www_lep_pt_br;
  www_chain.SetBranchAddress("lep_pt", &www_lep_pt, &www_lep_pt_br);

  Int_t    www_nb;
  TBranch* www_nb_br;
  www_chain.SetBranchAddress("nb", &www_nb, &www_nb_br);  

  // Loops through all data for www
  Int_t www_n_entries = www_chain.GetEntries();
  entry_count = 0;
  
  for (UInt_t m = 0; m < www_n_entries; m++)
    {
      www_chain.GetEntry(m);

      if (www_el_pt > 20 && www_el_pt < 200)
        {
          entry_count++;
          www_el_pt_hist->Fill(www_el_pt, 59 * www_evt_scale1fb);
        }
      if (www_nb > 0)
        www_nb_hist->Fill(www_nb, 59 * www_evt_scale1fb);

      if (www_MjjL > 20)
        www_MjjL_hist->Fill(www_MjjL, 59 * www_evt_scale1fb);

      if (www_met_pt > 20 && www_met_pt < 200)
        www_met_pt_hist->Fill(www_met_pt, 59 * www_evt_scale1fb);

      if (www_lep_pt->at(0) > 20 && www_lep_pt->at(0) < 200)
        www_lep_pt_hist_1->Fill(www_lep_pt->at(0), 59 * www_evt_scale1fb);

      if (www_lep_pt->at(1) > 20 && www_lep_pt->at(1) < 200)
        www_lep_pt_hist_2->Fill(www_lep_pt->at(1), 59 * www_evt_scale1fb);

      if (www_nj30 > 0)
        www_nj30_hist->Fill(www_nj30, 59 * www_evt_scale1fb);

      if (www_lep_pt->size() > 0)
	www_n_lep_hist->Fill(www_lep_pt->size(), 59 * www_evt_scale1fb);
    }

  // Displays number of events in histogram  
  cout << "==========================" << endl;
  cout << "www: " << entry_count << " events." << endl;
  cout << "==========================" << endl;

  // Output file
  TFile* www_output_file = new TFile("www_distributions_2018.root", "recreate");
  www_output_file->cd();
  www_el_pt_hist->Write();
  www_nb_hist->Write();
  www_MjjL_hist->Write();
  www_met_pt_hist->Write();
  www_lep_pt_hist_1->Write();
  www_lep_pt_hist_2->Write();
  www_nj30_hist->Write();
  www_n_lep_hist->Write();
  www_output_file->Write();
  www_output_file->Close();
  //===============================================================================|

  //===============================================================================| 
  // WWZ LOOPER (Background)        
  //===============================================================================| 
  // Creates Chain                                                                   
  TChain wwz_chain("t");
  wwz_chain.Add(directoryString + "wwz_amcatnlo_1.root");

  // Sets all used branches to active status                                         
  wwz_chain.SetBranchStatus("*", 0);
  wwz_chain.SetBranchStatus("el_pt", 1);
  wwz_chain.SetBranchStatus("evt_scale1fb", 1);
  wwz_chain.SetBranchStatus("met_pt", 1);
  wwz_chain.SetBranchStatus("lep_pt", 1);
  wwz_chain.SetBranchStatus("nb", 1);
  wwz_chain.SetBranchStatus("MjjL", 1);
  wwz_chain.SetBranchStatus("nj30", 1);

  // Creates all histograms for wwz TChain                                           
  TH1D* wwz_el_pt_hist = new TH1D("wwz_el_pt",
				  "wwz signal electron momentum",
				  100, 20, 200);
  TH1D* wwz_nj30_hist = new TH1D("wwz_nj30", "wwz amount of jets",
                                 9, 0.5, 9.5);
  TH1D* wwz_MjjL_hist = new TH1D("wwz_MjjL", "wwz leading MjjL",
				 100, 20, 500);
  TH1D* wwz_met_pt_hist = new TH1D("wwz_met_pt", "wwz met momentum",
				   100, 20, 200);
  TH1D* wwz_lep_pt_hist_1 = new TH1D("wwz_lep_pt_1", "wwz lep momentum 1",
				     100, 20, 200);
  TH1D* wwz_lep_pt_hist_2 = new TH1D("wwz_lep_pt_2", "wwz lep momentum 2",
				     100, 20, 200);
  TH1D* wwz_nb_hist = new TH1D("wwz_nb", "wwz number of b jets",
			       5, 0.5, 5.5);
  TH1D* wwz_n_lep_hist = new TH1D("wwz_n_lep", "wwz number of leptons",
                                  2, 1.5, 3.5);

  // Sets all branch addresses 
  Float_t  wwz_el_pt;
  TBranch* wwz_el_pt_br;
  wwz_chain.SetBranchAddress("el_pt", &wwz_el_pt, &wwz_el_pt_br);

  Float_t  wwz_evt_scale1fb;
  TBranch* wwz_evt_scale1fb_br;
  wwz_chain.SetBranchAddress("evt_scale1fb", &wwz_evt_scale1fb,
			     &wwz_evt_scale1fb_br);

  Int_t    wwz_nj30;
  TBranch* wwz_nj30_br;
  wwz_chain.SetBranchAddress("nj30", &wwz_nj30, &wwz_nj30_br);

  Float_t  wwz_MjjL;
  TBranch* wwz_MjjL_br;
  wwz_chain.SetBranchAddress("MjjL", &wwz_MjjL, &wwz_MjjL_br);

  Float_t  wwz_met_pt;
  TBranch* wwz_met_pt_br;
  wwz_chain.SetBranchAddress("met_pt", &wwz_met_pt, &wwz_met_pt_br);

  vector<Float_t>* wwz_lep_pt = NULL;
  TBranch*         wwz_lep_pt_br;
  wwz_chain.SetBranchAddress("lep_pt", &wwz_lep_pt, &wwz_lep_pt_br);

  Int_t    wwz_nb;
  TBranch* wwz_nb_br;
  wwz_chain.SetBranchAddress("nb", &wwz_nb, &wwz_nb_br);

  // Loops through all data for wwz                                                  
  Int_t wwz_n_entries = wwz_chain.GetEntries();
  entry_count = 0;

  for (UInt_t o = 0; o < wwz_n_entries; o++)
    {
      wwz_chain.GetEntry(o);

      if (wwz_el_pt > 20 && wwz_el_pt < 200)
        {
          entry_count++;
          wwz_el_pt_hist->Fill(wwz_el_pt, 59 * wwz_evt_scale1fb);
        }
      if (wwz_nb > 0)
        wwz_nb_hist->Fill(wwz_nb, 59 * wwz_evt_scale1fb);

      if (wwz_MjjL > 20)
        wwz_MjjL_hist->Fill(wwz_MjjL, 59 * wwz_evt_scale1fb);

      if (wwz_met_pt > 20 && wwz_met_pt < 200)
        wwz_met_pt_hist->Fill(wwz_met_pt, 59 * wwz_evt_scale1fb);

      if (wwz_lep_pt->at(0) > 20 && wwz_lep_pt->at(0) < 200)
        wwz_lep_pt_hist_1->Fill(wwz_lep_pt->at(0), 59 * wwz_evt_scale1fb);

      if (wwz_lep_pt->at(1) > 20 && wwz_lep_pt->at(1) < 200)
        wwz_lep_pt_hist_2->Fill(wwz_lep_pt->at(1), 59 * wwz_evt_scale1fb);

      if (wwz_nj30 > 0)
        wwz_nj30_hist->Fill(wwz_nj30, 59 * wwz_evt_scale1fb);

      if (wwz_lep_pt->size() > 0)
        wwz_n_lep_hist->Fill(wwz_lep_pt->size(), 59 * wwz_evt_scale1fb);

    }

  // Displays number of events in histogram                                           
  cout << "==========================" << endl;
  cout << "wwz: " << entry_count << " events." << endl;
  cout << "==========================" << endl;

  // Output file                                                                      
  TFile* wwz_output_file = new TFile("wwz_distributions_2018.root", "recreate");
  wwz_output_file->cd();
  wwz_el_pt_hist->Write();
  wwz_nb_hist->Write();
  wwz_MjjL_hist->Write();
  wwz_met_pt_hist->Write();
  wwz_lep_pt_hist_1->Write();
  wwz_lep_pt_hist_2->Write();
  wwz_nj30_hist->Write();
  wwz_n_lep_hist->Write();
  wwz_output_file->Write();
  wwz_output_file->Close();
  //===============================================================================|

  //===============================================================================| 
  // ZZZ LOOPER (Background)        
  //===============================================================================| 
  // Creates Chain                                                                   
  TChain zzz_chain("t");
  zzz_chain.Add(directoryString + "zzz_amcatnlo_1.root");

  // Sets all used branches to active status                                         
  zzz_chain.SetBranchStatus("*", 0);
  zzz_chain.SetBranchStatus("el_pt", 1);
  zzz_chain.SetBranchStatus("evt_scale1fb", 1);
  zzz_chain.SetBranchStatus("met_pt", 1);
  zzz_chain.SetBranchStatus("lep_pt", 1);
  zzz_chain.SetBranchStatus("nb", 1);
  zzz_chain.SetBranchStatus("MjjL", 1);
  zzz_chain.SetBranchStatus("nj30", 1);

  // Creates all histograms for zzz TChain                                           
  TH1D* zzz_el_pt_hist = new TH1D("zzz_el_pt",
				  "zzz signal electron momentum",
				  100, 20, 200);
  TH1D* zzz_nj30_hist = new TH1D("zzz_nj30", "zzz amount of jets",
                                 9, 0.5, 9.5);
  TH1D* zzz_MjjL_hist = new TH1D("zzz_MjjL", "zzz leading MjjL",
				 100, 20, 500);
  TH1D* zzz_met_pt_hist = new TH1D("zzz_met_pt", "zzz met momentum",
				   100, 20, 200);
  TH1D* zzz_lep_pt_hist_1 = new TH1D("zzz_lep_pt_1", "zzz lep momentum 1",
				     100, 20, 200);
  TH1D* zzz_lep_pt_hist_2 = new TH1D("zzz_lep_pt_2", "zzz lep momentum 2",
				     100, 20, 200);
  TH1D* zzz_nb_hist = new TH1D("zzz_nb", "zzz number of b jets",
			       5, 0.5, 5.5);
  TH1D* zzz_n_lep_hist = new TH1D("zzz_n_lep", "zzz number of leptons",
                                  2, 1.5, 3.5);

  // Sets all branch addresses 
  Float_t  zzz_el_pt;
  TBranch* zzz_el_pt_br;
  zzz_chain.SetBranchAddress("el_pt", &zzz_el_pt, &zzz_el_pt_br);

  Float_t  zzz_evt_scale1fb;
  TBranch* zzz_evt_scale1fb_br;
  zzz_chain.SetBranchAddress("evt_scale1fb", &zzz_evt_scale1fb,
			     &zzz_evt_scale1fb_br);

  Int_t    zzz_nj30;
  TBranch* zzz_nj30_br;
  zzz_chain.SetBranchAddress("nj30", &zzz_nj30, &zzz_nj30_br);

  Float_t  zzz_MjjL;
  TBranch* zzz_MjjL_br;
  zzz_chain.SetBranchAddress("MjjL", &zzz_MjjL, &zzz_MjjL_br);

  Float_t  zzz_met_pt;
  TBranch* zzz_met_pt_br;
  zzz_chain.SetBranchAddress("met_pt", &zzz_met_pt, &zzz_met_pt_br);

  vector<Float_t>* zzz_lep_pt = NULL;
  TBranch*         zzz_lep_pt_br;
  zzz_chain.SetBranchAddress("lep_pt", &zzz_lep_pt, &zzz_lep_pt_br);

  Int_t    zzz_nb;
  TBranch* zzz_nb_br;
  zzz_chain.SetBranchAddress("nb", &zzz_nb, &zzz_nb_br);

  // Loops through all data for zzz                                                  
  Int_t zzz_n_entries = zzz_chain.GetEntries();
  entry_count = 0;

  for (UInt_t p = 0; p < zzz_n_entries; p++)
    {
      zzz_chain.GetEntry(p);

      if (zzz_el_pt > 20 && zzz_el_pt < 200)
        {
          entry_count++;
          zzz_el_pt_hist->Fill(zzz_el_pt, 59 * zzz_evt_scale1fb);
        }
      if (zzz_nb > 0)
        zzz_nb_hist->Fill(zzz_nb, 59 * zzz_evt_scale1fb);

      if (zzz_MjjL > 20)
        zzz_MjjL_hist->Fill(zzz_MjjL, 59 * zzz_evt_scale1fb);

      if (zzz_met_pt > 20 && zzz_met_pt < 200)
        zzz_met_pt_hist->Fill(zzz_met_pt, 59 * zzz_evt_scale1fb);

      if (zzz_lep_pt->at(0) > 20 && zzz_lep_pt->at(0) < 200)
        zzz_lep_pt_hist_1->Fill(zzz_lep_pt->at(0), 59 * zzz_evt_scale1fb);

      if (zzz_lep_pt->at(1) > 20 && zzz_lep_pt->at(1) < 200)
        zzz_lep_pt_hist_2->Fill(zzz_lep_pt->at(1), 59 * zzz_evt_scale1fb);

      if (zzz_nj30 > 0)
        zzz_nj30_hist->Fill(zzz_nj30, 59 * zzz_evt_scale1fb);

      if (zzz_lep_pt->size() > 0)
        zzz_n_lep_hist->Fill(zzz_lep_pt->size(), 59 * zzz_evt_scale1fb);

    }

  // Displays number of events in histogram                                           
  cout << "==========================" << endl;
  cout << "zzz: " << entry_count << " events." << endl;
  cout << "==========================" << endl;

  // Output file                                                                      
  TFile* zzz_output_file = new TFile("zzz_distributions_2018.root", "recreate");
  zzz_output_file->cd();
  zzz_el_pt_hist->Write();
  zzz_nb_hist->Write();
  zzz_MjjL_hist->Write();
  zzz_met_pt_hist->Write();
  zzz_lep_pt_hist_1->Write();
  zzz_lep_pt_hist_2->Write();
  zzz_nj30_hist->Write();
  zzz_n_lep_hist->Write();
  zzz_output_file->Write();
  zzz_output_file->Close();
  //===============================================================================|


  //===============================================================================| 
  // ZZ LOOPER (Background)        
  //===============================================================================| 
  // Creates Chain                                                                   
  TChain zz_chain("t");
  zz_chain.Add(directoryString + "zz_4l_powheg_1.root" );
  zz_chain.Add(directoryString + "zz_4l_powheg_2.root" );
  zz_chain.Add(directoryString + "zz_4l_powheg_3.root" );
  zz_chain.Add(directoryString + "zz_4l_powheg_4.root" );
  zz_chain.Add(directoryString + "zz_4l_powheg_5.root" );
  zz_chain.Add(directoryString + "zz_4l_powheg_6.root" );
  zz_chain.Add(directoryString + "zz_4l_powheg_7.root" );
  zz_chain.Add(directoryString + "zz_4l_powheg_8.root" );
  zz_chain.Add(directoryString + "zz_4l_powheg_9.root" );
  zz_chain.Add(directoryString + "zz_4l_powheg_10.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_11.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_12.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_13.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_14.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_15.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_16.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_17.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_18.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_19.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_20.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_21.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_22.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_23.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_24.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_25.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_26.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_27.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_28.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_29.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_30.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_31.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_32.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_33.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_34.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_35.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_36.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_37.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_38.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_39.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_40.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_41.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_42.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_43.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_44.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_45.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_46.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_47.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_48.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_49.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_50.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_51.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_52.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_53.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_54.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_55.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_56.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_57.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_58.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_59.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_60.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_61.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_62.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_63.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_64.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_65.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_66.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_67.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_68.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_69.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_70.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_71.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_72.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_73.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_74.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_75.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_76.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_77.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_78.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_79.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_80.root");
  zz_chain.Add(directoryString + "zz_4l_powheg_81.root");

  // Sets all used branches to active status                                         
  zz_chain.SetBranchStatus("*", 0);
  zz_chain.SetBranchStatus("el_pt", 1);
  zz_chain.SetBranchStatus("evt_scale1fb", 1);
  zz_chain.SetBranchStatus("met_pt", 1);
  zz_chain.SetBranchStatus("lep_pt", 1);
  zz_chain.SetBranchStatus("nb", 1);
  zz_chain.SetBranchStatus("MjjL", 1);
  zz_chain.SetBranchStatus("nj30", 1);

  // Creates all histograms for zz TChain                                           
  TH1D* zz_el_pt_hist = new TH1D("zz_el_pt",
				  "zz signal electron momentum",
				  100, 20, 200);
  TH1D* zz_nj30_hist = new TH1D("zz_nj30", "zz amount of jets",
                                 9, 0.5, 9.5);
  TH1D* zz_MjjL_hist = new TH1D("zz_MjjL", "zz leading MjjL",
				 100, 20, 500);
  TH1D* zz_met_pt_hist = new TH1D("zz_met_pt", "zz met momentum",
				   100, 20, 200);
  TH1D* zz_lep_pt_hist_1 = new TH1D("zz_lep_pt_1", "zz lep momentum 1",
				     100, 20, 200);
  TH1D* zz_lep_pt_hist_2 = new TH1D("zz_lep_pt_2", "zz lep momentum 2",
				     100, 20, 200);
  TH1D* zz_nb_hist = new TH1D("zz_nb", "zz number of b jets",
			       5, 0.5, 5.5);
  TH1D* zz_n_lep_hist = new TH1D("zz_n_lep", "zz number of leptons",
                                  2, 1.5, 3.5);

  // Sets all branch addresses 
  Float_t  zz_el_pt;
  TBranch* zz_el_pt_br;
  zz_chain.SetBranchAddress("el_pt", &zz_el_pt, &zz_el_pt_br);

  Float_t  zz_evt_scale1fb;
  TBranch* zz_evt_scale1fb_br;
  zz_chain.SetBranchAddress("evt_scale1fb", &zz_evt_scale1fb,
			     &zz_evt_scale1fb_br);

  Int_t    zz_nj30;
  TBranch* zz_nj30_br;
  zz_chain.SetBranchAddress("nj30", &zz_nj30, &zz_nj30_br);

  Float_t  zz_MjjL;
  TBranch* zz_MjjL_br;
  zz_chain.SetBranchAddress("MjjL", &zz_MjjL, &zz_MjjL_br);

  Float_t  zz_met_pt;
  TBranch* zz_met_pt_br;
  zz_chain.SetBranchAddress("met_pt", &zz_met_pt, &zz_met_pt_br);

  vector<Float_t>* zz_lep_pt = NULL;
  TBranch*         zz_lep_pt_br;
  zz_chain.SetBranchAddress("lep_pt", &zz_lep_pt, &zz_lep_pt_br);

  Int_t    zz_nb;
  TBranch* zz_nb_br;
  zz_chain.SetBranchAddress("nb", &zz_nb, &zz_nb_br);

  // Loops through all data for zz                                                  
  Int_t zz_n_entries = zz_chain.GetEntries();
  entry_count = 0;

  for (UInt_t q = 0; q < zz_n_entries; q++)
    {
      zz_chain.GetEntry(q);

      if (zz_el_pt > 20 && zz_el_pt < 200)
        {
          entry_count++;
          zz_el_pt_hist->Fill(zz_el_pt, 59 * zz_evt_scale1fb);
        }
      if (zz_nb > 0)
        zz_nb_hist->Fill(zz_nb, 59 * zz_evt_scale1fb);

      if (zz_MjjL > 20)
        zz_MjjL_hist->Fill(zz_MjjL, 59 * zz_evt_scale1fb);

      if (zz_met_pt > 20 && zz_met_pt < 200)
        zz_met_pt_hist->Fill(zz_met_pt, 59 * zz_evt_scale1fb);

      if (zz_lep_pt->at(0) > 20 && zz_lep_pt->at(0) < 200)
        zz_lep_pt_hist_1->Fill(zz_lep_pt->at(0), 59 * zz_evt_scale1fb);

      if (zz_lep_pt->at(1) > 20 && zz_lep_pt->at(1) < 200)
        zz_lep_pt_hist_2->Fill(zz_lep_pt->at(1), 59 * zz_evt_scale1fb);

      if (zz_nj30 > 0)
        zz_nj30_hist->Fill(zz_nj30, 59 * zz_evt_scale1fb);

      if (zz_lep_pt->size() > 0)
        zz_n_lep_hist->Fill(zz_lep_pt->size(), 59 * zz_evt_scale1fb);

    }

  // Displays number of events in histogram                                          
  cout << "==========================" << endl;
  cout << "zz: " << entry_count << " events." << endl;
  cout << "==========================" << endl;

  // Output file                                                                     
  TFile* zz_output_file = new TFile("zz_distributions_2018.root", "recreate");
  zz_output_file->cd();
  zz_el_pt_hist->Write();
  zz_nb_hist->Write();
  zz_MjjL_hist->Write();
  zz_met_pt_hist->Write();
  zz_lep_pt_hist_1->Write();
  zz_lep_pt_hist_2->Write();
  zz_nj30_hist->Write();
  zz_n_lep_hist->Write();
  zz_output_file->Write();
  zz_output_file->Close();
  //===============================================================================|

  //===============================================================================|
  // DATA LOOPER
  //===============================================================================|
  // Creates Chain
  TChain data_chain("t");
  data_chain.Add(directoryString + "data_Run2018Av1_em_1.root");
  data_chain.Add(directoryString + "data_Run2018Av2_ee_1.root");
  data_chain.Add(directoryString + "data_Run2018Av2_mm_1.root");
  data_chain.Add(directoryString + "data_Run2018Bv1_ee_1.root");
  data_chain.Add(directoryString + "data_Run2018Bv1_em_1.root");
  data_chain.Add(directoryString + "data_Run2018Bv1_mm_1.root");
  data_chain.Add(directoryString + "data_Run2018Cv1_ee_1.root");
  data_chain.Add(directoryString + "data_Run2018Cv1_em_1.root");
  data_chain.Add(directoryString + "data_Run2018Cv1_mm_1.root");
  data_chain.Add(directoryString + "data_Run2018Dv2_ee_1.root");
  data_chain.Add(directoryString + "data_Run2018Dv2_em_1.root");
  data_chain.Add(directoryString + "data_Run2018Dv2_mm_1.root");

  // Sets all used branches to active status
  data_chain.SetBranchStatus("*", 0);
  data_chain.SetBranchStatus("el_pt", 1);
  data_chain.SetBranchStatus("evt_scale1fb", 1);
  data_chain.SetBranchStatus("met_pt", 1);
  data_chain.SetBranchStatus("lep_pt", 1);
  data_chain.SetBranchStatus("nb", 1);
  data_chain.SetBranchStatus("MjjL", 1);
  data_chain.SetBranchStatus("nj30", 1);
  
  // Creates histograms data TChain
  TH1D* data_el_pt_hist = new TH1D("data_el_pt",
                                    "data electron momentum",
                                    100, 20, 200);
  TH1D* data_nj30_hist = new TH1D("data_nj30", "data amount of jets",
                                     9, 0.5, 9.5);
  TH1D* data_MjjL_hist = new TH1D("data_MjjL", "data leading MjjL",
                                   100, 20, 500);
  TH1D* data_met_pt_hist = new TH1D("data_met_pt", "data met momentum",
                                     100, 20, 200);
  TH1D* data_lep_pt_hist_1 = new TH1D("data_lep_pt_1", "data lep momentum 1",
				       100, 20, 200);
  TH1D* data_lep_pt_hist_2 = new TH1D("data_lep_pt_2", "data lep momentum 2",
                                       100, 20, 200);
  TH1D* data_nb_hist = new TH1D("data_nb", "data number of b jets",
                                 5, 0.5, 5.5);
  TH1D* data_n_lep_hist = new TH1D("data_n_lep", "data number of leptons",
				   2, 1.5, 3.5);

  // Sets all branch addresses
  Float_t  data_el_pt;
  TBranch* data_el_pt_br;
  data_chain.SetBranchAddress("el_pt", &data_el_pt, &data_el_pt_br);

  Float_t  data_evt_scale1fb;
  TBranch* data_evt_scale1fb_br;
  data_chain.SetBranchAddress("evt_scale1fb", &data_evt_scale1fb,
			     &data_evt_scale1fb_br);

  Int_t    data_nj30;
  TBranch* data_nj30_br;
  data_chain.SetBranchAddress("nj30", &data_nj30, &data_nj30_br);

  Float_t  data_MjjL;
  TBranch* data_MjjL_br;
  data_chain.SetBranchAddress("MjjL", &data_MjjL, &data_MjjL_br);

  Float_t  data_met_pt;
  TBranch* data_met_pt_br;
  data_chain.SetBranchAddress("met_pt", &data_met_pt, &data_met_pt_br);

  vector<Float_t>* data_lep_pt = NULL;
  TBranch*         data_lep_pt_br;
  data_chain.SetBranchAddress("lep_pt", &data_lep_pt, &data_lep_pt_br);

  Int_t    data_nb;
  TBranch* data_nb_br;
  data_chain.SetBranchAddress("nb", &data_nb, &data_nb_br);

  // Loops through data for data
  Int_t data_n_entries = data_chain.GetEntries();
  entry_count = 0;
  
  for (UInt_t n = 0; n < data_n_entries; n++)
    {
      data_chain.GetEntry(n);

      if (data_el_pt > 20 && data_el_pt < 200)
        {
          entry_count++;
          data_el_pt_hist->Fill(data_el_pt);
        }
      if (data_nb > 0)
        data_nb_hist->Fill(data_nb);

      if (data_MjjL > 20)
        data_MjjL_hist->Fill(data_MjjL);

      if (data_met_pt > 20 && data_met_pt < 200)
        data_met_pt_hist->Fill(data_met_pt);

      if (data_lep_pt->at(0) > 20 && data_lep_pt->at(0) < 200)
        data_lep_pt_hist_1->Fill(data_lep_pt->at(0));

      if (data_lep_pt->at(1) > 20 && data_lep_pt->at(1) < 200)
        data_lep_pt_hist_2->Fill(data_lep_pt->at(1));

      if (data_nj30 > 0)
        data_nj30_hist->Fill(data_nj30);

      if (data_lep_pt->size() > 0)
	data_n_lep_hist->Fill(data_lep_pt->size());

    }

  // Displays number of events in histogram 
  cout << "==========================" << endl;
  cout << "data: " << entry_count << " events." << endl;
  cout << "==========================" << endl;

  // Output file
  TFile* data_output_file = new TFile("data_distributions_2018.root", "recreate");
  data_output_file->cd();
  data_el_pt_hist->Write();
  data_nb_hist->Write();
  data_MjjL_hist->Write();
  data_met_pt_hist->Write();
  data_lep_pt_hist_1->Write();
  data_lep_pt_hist_2->Write();
  data_nj30_hist->Write();
  data_n_lep_hist->Write();
  data_output_file->Write();
  data_output_file->Close();
  //===============================================================================|
}

