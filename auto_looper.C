#include "universal.h"

using namespace std;

// Called in the main for loop
// This function applies a pre-selection cut in order to only allow entries in the tree
// that pass all the constraints set in the function
bool preSelection(Bool_t firstgoodvertex, Bool_t Flag_AllEventFilters,
                  Bool_t vetophoton     , Bool_t evt_passgoodrunlist ,
                  Int_t  nVlep          , Int_t nLlep                 )
{
  bool presel = 1;
  presel &= (firstgoodvertex      == 0);
  presel &= (Flag_AllEventFilters >  0);
  presel &= (vetophoton           == 0);
  presel &= (evt_passgoodrunlist  >  0);
  presel &= (nVlep                == 2);
  presel &= (nLlep                == 2);
  return presel;
}

// Called in the main for loop
// Function only allows entries that pass the triggers 
bool triggerSelection(Bool_t passTrigger, Bool_t pass_duplicate_ee_em_mm)
{
  bool trigger;
  trigger = passTrigger * pass_duplicate_ee_em_mm;
  return trigger;
}

// This function checks the choice made by the user in chain_looper.C
// for which lepton pair to check for
bool checkLeptons(Int_t lep_1, Int_t lep_2, Int_t choice)
{
  bool check = false;
  switch(choice)
    {
    case 1:
      if (lep_1 == 11 && lep_2 == 11)
        check = true;
    case 2:
      if ((lep_1 == 11 && lep_2 == 13) || (lep_1 == 13 && lep_2 == 11))
        check = true;
    case 3:
      if (lep_1 == 13 && lep_2 == 13)
        check = true;
    }
  return check;
}

// Main Looper Function, called by each chain in chain_looper.C
void processChain(TChain &chain, TString process, Bool_t isData, Int_t lep_pair_choice)
{
  // Sets all used branches to active status                                          
  chain.SetBranchStatus("*", 0);
  chain.SetBranchStatus("el_pt", 1);
  chain.SetBranchStatus("evt_scale1fb", 1);
  chain.SetBranchStatus("met_pt", 1);
  chain.SetBranchStatus("lep_pt", 1);
  chain.SetBranchStatus("nb", 1);
  chain.SetBranchStatus("MjjL", 1);
  chain.SetBranchStatus("nj30", 1);
  chain.SetBranchStatus("mu_pt", 1);
  chain.SetBranchStatus("firstgoodvertex", 1);
  chain.SetBranchStatus("Flag_AllEventFilters", 1);
  chain.SetBranchStatus("vetophoton", 1);
  chain.SetBranchStatus("evt_passgoodrunlist", 1);
  chain.SetBranchStatus("nVlep", 1);
  chain.SetBranchStatus("nLlep", 1);
  chain.SetBranchStatus("passTrigger", 1);
  chain.SetBranchStatus("pass_duplicate_ee_em_mm", 1);
  chain.SetBranchStatus("lep_pdgId", 1);
  chain.SetBranchStatus("weight_btagsf", 1);
  chain.SetBranchStatus("lepsf", 1);
  chain.SetBranchStatus("trigsf", 1);

  // Creates histograms to store data from the chain                                       
  TH1D* el_pt_hist    = new TH1D(process + "_el_pt" , process + " electron momentum",
                                 100, 20 , 200);
  TH1D* mu_pt_hist    = new TH1D(process + "_mu_pt" , process + " muon momentum",
                                 100, 20 , 200);
  TH1D* nj30_hist     = new TH1D(process + "_nj30"  , process + " amount of jets"   ,
                                 9  , 0.5, 9.5);
  TH1D* MjjL_hist     = new TH1D(process + "_MjjL"  , process + " leading MjjL"     ,
                                 100, 20 , 500);
  TH1D* met_pt_hist   = new TH1D(process + "_met_pt", process + " met momentum"     ,
                                 100, 20 , 200);
  TH1D* lep_pt_hist_1 = new TH1D(process + "_lep_pt_1", process + " lep momentum 1" ,
                                 100, 20 , 200);
  TH1D* lep_pt_hist_2 = new TH1D(process + "_lep_pt_2", process + " lep momentum 2" ,
                                 100, 20 , 200);
  TH1D* nb_hist       = new TH1D(process + "_nb"    , process + " number of b jets" ,
                                 5  , 0.5, 5.5);
  TH1D* n_lep_hist    = new TH1D(process + "_n_lep" , process + " number of leptons",
                                 2  , 1.5, 3.5);

  // Sets all branch addresses and declares reference variables                            
  Float_t  el_pt;
  TBranch* el_pt_br;
  chain.SetBranchAddress("el_pt", &el_pt, &el_pt_br);

  Float_t  mu_pt;
  TBranch* mu_pt_br;
  chain.SetBranchAddress("mu_pt", &mu_pt, &mu_pt_br);

  Float_t  evt_scale1fb;
  TBranch* evt_scale1fb_br;
  chain.SetBranchAddress("evt_scale1fb", &evt_scale1fb,
                         &evt_scale1fb_br);

  Int_t    nj30;
  TBranch* nj30_br;
  chain.SetBranchAddress("nj30", &nj30, &nj30_br);

  Float_t  MjjL;
  TBranch* MjjL_br;
  chain.SetBranchAddress("MjjL", &MjjL, &MjjL_br);

  Float_t  met_pt;
  TBranch* met_pt_br;
  chain.SetBranchAddress("met_pt", &met_pt, &met_pt_br);

  vector<Float_t>* lep_pt = NULL;
  TBranch*         lep_pt_br;
  chain.SetBranchAddress("lep_pt", &lep_pt, &lep_pt_br);

  Int_t    nb;
  TBranch* nb_br;
  chain.SetBranchAddress("nb", &nb, &nb_br);

  Bool_t   firstgoodvertex;
  TBranch* firstgoodvertex_br;
  chain.SetBranchAddress("firstgoodvertex", &firstgoodvertex, &firstgoodvertex_br);

  Bool_t   Flag_AllEventFilters;
  TBranch* Flag_AllEventFilters_br;
  chain.SetBranchAddress("Flag_AllEventFilters", &Flag_AllEventFilters,
                         &Flag_AllEventFilters_br);

  Bool_t   vetophoton;
  TBranch* vetophoton_br;
  chain.SetBranchAddress("vetophoton", &vetophoton, &vetophoton_br);

  Bool_t   evt_passgoodrunlist;
  TBranch* evt_passgoodrunlist_br;
  chain.SetBranchAddress("evt_passgoodrunlist", &evt_passgoodrunlist,
                         &evt_passgoodrunlist_br);

  Int_t    nVlep;
  TBranch* nVlep_br;
  chain.SetBranchAddress("nVlep", &nVlep, &nVlep_br);

  Int_t    nLlep;
  TBranch* nLlep_br;
  chain.SetBranchAddress("nLlep", &nLlep, &nLlep_br);

  Bool_t   passTrigger;
  TBranch* passTrigger_br;
  chain.SetBranchAddress("passTrigger", &passTrigger, &passTrigger_br);

  Bool_t   pass_duplicate_ee_em_mm;
  TBranch* pass_duplicate_ee_em_mm_br;
  chain.SetBranchAddress("pass_duplicate_ee_em_mm", &pass_duplicate_ee_em_mm,
                         &pass_duplicate_ee_em_mm_br);

  vector<Int_t>* lep_pdgId = NULL;
  TBranch*       lep_pdgId_br;
  chain.SetBranchAddress("lep_pdgId", &lep_pdgId, &lep_pdgId_br);

  Float_t  weight_btagsf;
  TBranch* weight_btagsf_br;
  chain.SetBranchAddress("weight_btagsf", &weight_btagsf, &weight_btagsf_br);

  Float_t  lepsf;
  TBranch* lepsf_br;
  chain.SetBranchAddress("lepsf", &lepsf, &lepsf_br);

  Float_t  trigsf;
  TBranch* trigsf_br;
  chain.SetBranchAddress("trigsf", &trigsf, &trigsf_br);

  // Loops through all data and fills the desired histograms                               
  Int_t n_entries = chain.GetEntries();
  int entry_count = 0;

  // Sets the weight (not used in data, only simulation)                                                                      
  Float_t LUMI_2018 = 59.74;
  Double_t weight;
  Int_t lep_1;
  Int_t lep_2;
  Bool_t lep_pairs = false;
  for (UInt_t i = 0; i < n_entries; i++)
    {
      chain.GetEntry(i); // gets current entry in chain of trees

      bool preCuts = preSelection(firstgoodvertex, Flag_AllEventFilters,
                                  vetophoton     , evt_passgoodrunlist ,
                                  nVlep          , nLlep                );

      preCuts &= (triggerSelection(passTrigger, pass_duplicate_ee_em_mm) > 0);

      lep_1 = abs(lep_pdgId->at(0));
      lep_2 = abs(lep_pdgId->at(1));
      lep_pairs = checkLeptons(lep_1, lep_2, lep_pair_choice);

      if (preCuts && lep_pairs) // parameter applies pre-selection cuts and trigger cuts
        {
          if (isData == 0)
            weight = (LUMI_2018 * evt_scale1fb * trigsf);
          else if (isData == 1)
            weight = 1; // Data isn't weighted

          if (el_pt > 20 && el_pt < 200)
            {
              entry_count++;
              el_pt_hist->Fill(el_pt, weight);
            }

          if (mu_pt > 20 && mu_pt < 200)
            mu_pt_hist->Fill(mu_pt, weight);

          if (nb > 0)
            nb_hist->Fill(nb, weight * weight_btagsf);

          if (MjjL > 20)
            MjjL_hist->Fill(MjjL, weight);

          if (met_pt > 20 && met_pt < 200)
            met_pt_hist->Fill(met_pt, weight);

          if (lep_pt->at(0) > 20 && lep_pt->at(0) < 200)
            lep_pt_hist_1->Fill(lep_pt->at(0), weight * lepsf);

          if (lep_pt->at(1) > 20 && lep_pt->at(1) < 200)
            lep_pt_hist_2->Fill(lep_pt->at(1), weight * lepsf);

          if (nj30 > 0)
            nj30_hist->Fill(nj30, weight);

          if (lep_pt->size() > 0)
            n_lep_hist->Fill(lep_pt->size(), weight);

        }
    }

  // Displays number of events in histograms to the console                                              
  cout << "|================================|" << endl;
  cout << "| " << process << ": " << entry_count << " events." << endl;
  cout << "|================================|" << endl;

  // Output File
  // Saves each chain to its own separate distribution .root file to be opened for plotting
  // writes all the histograms to file
  TString save_directory = "~/root/distributions/";
  TString output_file_name = "_distributions_2018.root";
  output_file_name = process + output_file_name;
  TFile* output_file = new TFile(save_directory + output_file_name, "recreate");
  output_file->cd();
  el_pt_hist->Write();
  mu_pt_hist->Write();
  nb_hist->Write();
  MjjL_hist->Write();
  met_pt_hist->Write();
  lep_pt_hist_1->Write();
  lep_pt_hist_2->Write();
  nj30_hist->Write();
  n_lep_hist->Write();
  output_file->Write();
  output_file->Close();
}
