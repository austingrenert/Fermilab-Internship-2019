#include "plotter.h"

using namespace std;

void el_pt_plotter()
{
  // Opens files and retrieves histograms
  TFile* ttbar_input_file = new TFile("ttbar_distributions_2018.root");
  checkFile(ttbar_input_file);
  ttbar_input_file->cd();
  TH1D* h1 = NULL; ttbar_input_file->GetObject("ttbar_el_pt", h1);

  TFile* ttgam_input_file = new TFile("ttgam_distributions_2018.root");
  checkFile(ttgam_input_file);
  ttgam_input_file->cd();
  TH1D* h9 = NULL; ttgam_input_file->GetObject("ttgam_el_pt", h9);

  TFile* wjets_input_file = new TFile("wjets_distributions_2018.root");
  checkFile(wjets_input_file);
  wjets_input_file->cd();
  TH1D* h2 = NULL; wjets_input_file->GetObject("wjets_el_pt", h2);

  TFile* wz_input_file    = new TFile("wz_distributions_2018.root"   );
  checkFile(wz_input_file);
  wz_input_file->cd();
  TH1D* h3 = NULL; wz_input_file->GetObject("wz_el_pt", h3);

  TFile* www_input_file   = new TFile("www_distributions_2018.root"  );
  checkFile(www_input_file);
  www_input_file->cd();
  TH1D* h4 = NULL; www_input_file->GetObject("www_el_pt", h4);

  TFile* wwz_input_file   = new TFile("wwz_distributions_2018.root"  );
  checkFile(wwz_input_file);
  wwz_input_file->cd();
  TH1D* h6 = NULL; wwz_input_file->GetObject("wwz_el_pt", h6);

  TFile* zzz_input_file   = new TFile("zzz_distributions_2018.root"  );
  checkFile(zzz_input_file);
  zzz_input_file->cd();
  TH1D* h7 = NULL; zzz_input_file->GetObject("zzz_el_pt", h7);

  TFile* zz_input_file   = new TFile("zz_distributions_2018.root"  );
  checkFile(zz_input_file);
  zz_input_file->cd();
  TH1D* h8 = NULL; zz_input_file->GetObject("zz_el_pt", h8);

  TFile* data_input_file  = new TFile("data_distributions_2018.root" );
  checkFile(data_input_file);
  data_input_file->cd();
  TH1D* h5 = NULL; data_input_file->GetObject("data_el_pt", h5);

  // Options for histograms
  TH1D* ttbar_hist = new TH1D("ttbar_hist", "ttbar_hist", 100, 20, 200);
  ttbar_hist->Add(h1);
  ttbar_hist->SetFillColor(2);
  ttbar_hist->SetLineWidth(1);
  ttbar_hist->SetLineColor(2);

  TH1D* ttgam_hist = new TH1D("ttgam_hist", "ttgam_hist", 100, 20, 200);
  ttgam_hist->Add(h9);
  ttgam_hist->SetFillColor(6);
  ttgam_hist->SetLineWidth(1);
  ttgam_hist->SetLineColor(6);

  TH1D* wjets_hist = new TH1D("wjets_hist", "wjets_hist", 100, 20, 200);
  wjets_hist->Add(h2);
  wjets_hist->SetFillColor(3);
  wjets_hist->SetLineWidth(1);
  wjets_hist->SetLineColor(3);

  TH1D* wz_hist    = new TH1D("wz_hist"   , "wz_hist"   , 100, 20, 200);
  wz_hist->Add(h3);
  wz_hist->SetFillColor(4);
  wz_hist->SetLineWidth(1);
  wz_hist->SetLineColor(4);

  TH1D* www_hist   = new TH1D("www_hist"  , "www_hist"  , 100, 20, 200);
  www_hist->Add(h4);
  www_hist->SetFillColor(5);
  www_hist->SetLineWidth(1);
  www_hist->SetLineColor(5);

  TH1D* wwz_hist   = new TH1D("wwz_hist"  , "wwz_hist"  , 100, 20, 200);
  wwz_hist->Add(h6);
  wwz_hist->SetFillColor(1);
  wwz_hist->SetLineWidth(1);
  wwz_hist->SetLineColor(1);

  TH1D* zzz_hist   = new TH1D("zzz_hist"  , "zzz_hist"  , 100, 20, 200);
  zzz_hist->Add(h7);
  zzz_hist->SetFillColor(8);
  zzz_hist->SetLineWidth(1);
  zzz_hist->SetLineColor(8);

  TH1D* zz_hist   = new TH1D("zz_hist"  , "zz_hist"  , 100, 20, 200);
  zz_hist->Add(h8);
  zz_hist->SetFillColor(7);
  zz_hist->SetLineWidth(1);
  zz_hist->SetLineColor(7);

  TH1D* data_hist  = new TH1D("data_hist" , "data_hist" , 100, 20, 200);
  data_hist->Add(h5);
  data_hist->SetMarkerColor(1);
  data_hist->SetMarkerStyle(20);
  data_hist->SetMarkerSize(1);
  data_hist->SetMinimum(0);
  data_hist->SetMaximum(2500);
  
  // Creates Stack histogram
  THStack* stack_hist = new THStack("el_pt_stack", "Electron Momentum Histograms");
  stack_hist->Add(ttbar_hist, "HIST");
  stack_hist->Add(ttgam_hist, "HIST");
  stack_hist->Add(wjets_hist, "HIST");
  stack_hist->Add(wz_hist, "HIST");
  stack_hist->Add(wwz_hist, "HIST");
  stack_hist->Add(zzz_hist, "HIST");
  stack_hist->Add(zz_hist, "HIST");
  stack_hist->SetMinimum(0);
  stack_hist->SetMaximum(2500);
  data_hist->Sumw2();

  // Drawing Options
  Double_t w = 700;
  Double_t h = 750;
  TCanvas* canvas = NULL;
  canvas = new TCanvas("canvas", "canvas", w, h);
  canvas->SetWindowSize(w + (w - canvas->GetWw()), h + (h - canvas->GetWh()));
  canvas->SetBorderSize(2);
  canvas->SetLeftMargin(0.119326);
  canvas->SetFillColor(10);
  canvas->SetGrid();
  canvas->cd();

  stack_hist->Draw();
  data_hist->Draw("E1, SAME");
  www_hist->Draw("HIST, SAME");
  stack_hist->SetName("el_pt plot");
  stack_hist->SetTitle("Plot of Electron Momentum");
  stack_hist->GetXaxis()->SetTitle("el_pt [GeV]");
  stack_hist->GetXaxis()->CenterTitle(kTRUE);
  stack_hist->GetXaxis()->SetTitleOffset(1.2);
  stack_hist->GetYaxis()->SetTitle("number of events normalized");
  stack_hist->GetYaxis()->CenterTitle(kTRUE);
  stack_hist->GetYaxis()->SetTitleOffset(1.5);

  TRatioPlot* rp = new TRatioPlot(stack_hist, data_hist);
  //  rp->Draw();
 
  TLegend* legend = new TLegend(0.7,0.7,0.48,0.9);
  legend->SetHeader("el_pt","C");
  legend->SetNColumns(2);
  legend->AddEntry("ttbar_hist", "ttbar", "f"  );
  legend->AddEntry("ttgam_hist", "ttgam", "f"  );
  legend->AddEntry("wjets_hist", "wjets", "f"  );
  legend->AddEntry("wz_hist"   , "wz"   , "f"  );
  legend->AddEntry("www_hist"  , "www"  , "f"  );
  legend->AddEntry("wwz_hist"  , "wwz"  , "f"  );
  legend->AddEntry("zzz_hist"  , "zzz"  , "f"  );
  legend->AddEntry("zz_hist"   , "zz"   , "f"  );
  legend->AddEntry("data_hist" , "data" , "lep");
  legend->Draw();
  canvas->Update();

}
