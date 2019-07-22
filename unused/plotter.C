#include "universal.h"

using namespace std;

void plot(TString distribution, TString plot_name, Int_t min, Int_t max)
{
  // Directory
  TString directoryString = "~/distributions/";

  // Opens files and retrieves histograms                                             
  TFile* ttbar_input_file = new TFile(directoryString + 
				      "ttbar_distributions_2018.root");
  checkFile(ttbar_input_file);
  ttbar_input_file->cd();
  TH1D* h1 = NULL; ttbar_input_file->GetObject("ttbar_" + distribution, h1);

  TFile* ttgam_input_file = new TFile(directoryString + 
				      "ttgam_distributions_2018.root");
  checkFile(ttgam_input_file);
  ttgam_input_file->cd();
  TH1D* h2 = NULL; ttgam_input_file->GetObject("ttgam_" + distribution, h2);

  TFile* wjets_input_file = new TFile(directoryString +
				      "wjets_distributions_2018.root");
  checkFile(wjets_input_file);
  wjets_input_file->cd();
  TH1D* h3 = NULL; wjets_input_file->GetObject("wjets_" + distribution, h3);

  TFile* wz_input_file    = new TFile(directoryString + 
				      "wz_distributions_2018.root"   );
  checkFile(wz_input_file);
  wz_input_file->cd();
  TH1D* h4 = NULL; wz_input_file->GetObject("wz_" + distribution, h4);

  TFile* www_input_file   = new TFile(directoryString + 
				      "www_distributions_2018.root"  );
  checkFile(www_input_file);
  www_input_file->cd();
  TH1D* h5 = NULL; www_input_file->GetObject("www_" + distribution, h5);

  TFile* zz_input_file   = new TFile(directoryString + 
				     "zz_distributions_2018.root"  );
  checkFile(zz_input_file);
  zz_input_file->cd();
  TH1D* h6 = NULL; zz_input_file->GetObject("zz_" + distribution, h6);

  TFile* dy_input_file   = new TFile(directoryString +
                                     "dy_distributions_2018.root"  );
  checkFile(dy_input_file);
  dy_input_file->cd();
  TH1D* h7 = NULL; dy_input_file->GetObject("dy_" + distribution, h7);

  TFile* ttz_input_file   = new TFile(directoryString +
                                     "ttz_distributions_2018.root"  );
  checkFile(ttz_input_file);
  ttz_input_file->cd();
  TH1D* h8 = NULL; ttz_input_file->GetObject("ttz_" + distribution, h8);

  TFile* ttw_input_file   = new TFile(directoryString +
                                     "ttw_distributions_2018.root"  );
  checkFile(ttw_input_file);
  ttw_input_file->cd();
  TH1D* h9 = NULL; ttw_input_file->GetObject("ttw_" + distribution, h9);

  TFile* tZq_input_file   = new TFile(directoryString +
				      "tZq_distributions_2018.root"  );
  checkFile(tZq_input_file);
  tZq_input_file->cd();
  TH1D* h10 = NULL; tZq_input_file->GetObject("tZq_" + distribution, h10);

  // DATA
  TFile* data_input_file  = new TFile(directoryString + 
				      "data_distributions_2018.root" );
  checkFile(data_input_file);
  data_input_file->cd();
  TH1D* data = NULL; data_input_file->GetObject("data_" + distribution, data);

  // Options for histograms                                                           
  TH1D* ttbar_hist = new TH1D("ttbar_hist", "ttbar_hist", 100, 20, 200);
  ttbar_hist->Add(h1);
  ttbar_hist->SetFillColor(40);
  ttbar_hist->SetLineWidth(1);
  ttbar_hist->SetLineColor(40);

  TH1D* ttgam_hist = new TH1D("ttgam_hist", "ttgam_hist", 100, 20, 200);
  ttgam_hist->Add(h2);
  ttgam_hist->SetFillColor(41);
  ttgam_hist->SetLineWidth(1);
  ttgam_hist->SetLineColor(41);

  TH1D* wjets_hist = new TH1D("wjets_hist", "wjets_hist", 100, 20, 200);
  wjets_hist->Add(h3);
  wjets_hist->SetFillColor(43);
  wjets_hist->SetLineWidth(1);
  wjets_hist->SetLineColor(43);

  TH1D* wz_hist    = new TH1D("wz_hist"   , "wz_hist"   , 100, 20, 200);
  wz_hist->Add(h4);
  wz_hist->SetFillColor(46);
  wz_hist->SetLineWidth(1);
  wz_hist->SetLineColor(46);

  TH1D* www_hist   = new TH1D("www_hist"  , "www_hist"  , 100, 20, 200);
  www_hist->Add(h5);
  www_hist->SetFillColor(47);
  www_hist->SetLineWidth(1);
  www_hist->SetLineColor(47);

  TH1D* zz_hist   = new TH1D("zz_hist"  , "zz_hist"  , 100, 20, 200);
  zz_hist->Add(h6);
  zz_hist->SetFillColor(33);
  zz_hist->SetLineWidth(1);
  zz_hist->SetLineColor(33);

  TH1D* dy_hist   = new TH1D("dy_hist"  , "dy_hist"  , 100, 20, 200);
  dy_hist->Add(h7);
  dy_hist->SetFillColor(36);
  dy_hist->SetLineWidth(1);
  dy_hist->SetLineColor(36);

  TH1D* ttz_hist   = new TH1D("ttz_hist"  , "ttz_hist"  , 100, 20, 200);
  ttz_hist->Add(h8);
  ttz_hist->SetFillColor(38);
  ttz_hist->SetLineWidth(1);
  ttz_hist->SetLineColor(38);

  TH1D* ttw_hist   = new TH1D("ttw_hist"  , "ttw_hist"  , 100, 20, 200);
  ttw_hist->Add(h9);
  ttw_hist->SetFillColor(20);
  ttw_hist->SetLineWidth(1);
  ttw_hist->SetLineColor(20);

  TH1D* tZq_hist   = new TH1D("tZq_hist"  , "tZq_hist"  , 100, 20, 200);
  tZq_hist->Add(h10);
  tZq_hist->SetFillColor(8);
  tZq_hist->SetLineWidth(1);
  tZq_hist->SetLineColor(8);

  // DATA
  TH1D* data_hist  = new TH1D("data_hist" , "data_hist" , 100, 20, 200);
  data_hist->Add(data);
  data_hist->SetMarkerColor(1);
  data_hist->SetMarkerStyle(20);
  data_hist->SetMarkerSize(1);
  data_hist->SetMinimum(min);
  data_hist->SetMaximum(max);

  // Creates Stack histogram                                                          
  THStack* stack_hist = new THStack(distribution + " plot", 
				    plot_name + " Stack Histogram");
  stack_hist->Add(ttbar_hist, "HIST");
  stack_hist->Add(ttgam_hist, "HIST");
  stack_hist->Add(wjets_hist, "HIST");
  stack_hist->Add(wz_hist, "HIST");
  stack_hist->Add(zz_hist, "HIST");
  stack_hist->Add(dy_hist, "HIST");
  stack_hist->Add(ttz_hist, "HIST");
  stack_hist->Add(ttw_hist, "HIST");
  stack_hist->Add(tZq_hist, "HIST");
  stack_hist->SetMinimum(min);
  stack_hist->SetMaximum(max);
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
  stack_hist->SetName(distribution + " plot");
  stack_hist->SetTitle(plot_name);
  stack_hist->GetXaxis()->SetTitle(distribution);
  stack_hist->GetXaxis()->CenterTitle(kTRUE);
  stack_hist->GetXaxis()->SetTitleOffset(1.2);
  stack_hist->GetYaxis()->SetTitle("number of events normalized");
  stack_hist->GetYaxis()->CenterTitle(kTRUE);
  stack_hist->GetYaxis()->SetTitleOffset(1.5);

  TLegend* legend = new TLegend(0.7,0.7,0.48,0.9);
  legend->SetHeader(distribution,"C");
  legend->SetNColumns(3);
  legend->AddEntry("ttbar_hist", "ttbar", "f"  );
  legend->AddEntry("ttgam_hist", "ttgam", "f"  );
  legend->AddEntry("wjets_hist", "wjets", "f"  );
  legend->AddEntry("wz_hist"   , "wz"   , "f"  );
  legend->AddEntry("www_hist"  , "www"  , "f"  );
  legend->AddEntry("zz_hist"   , "zz"   , "f"  );
  legend->AddEntry("dy_hist"   , "dy"   , "f"  );
  legend->AddEntry("ttz_hist"  , "ttz"  , "f"  );
  legend->AddEntry("ttw_hist"  , "ttw"  , "f"  );
  legend->AddEntry("tZq_hist"  , "tZq"  , "f"  );
  legend->AddEntry("data_hist" , "data" , "lep");
  legend->Draw();
  canvas->Update();
}
