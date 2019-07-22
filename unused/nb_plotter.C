#include "plotter.h"

using namespace std;

void nb_plotter()
{
  // Opens files and retrieves histograms
  TFile* ttbar_input_file = new TFile("ttbar_distributions_2018.root");
  checkFile(ttbar_input_file);
  ttbar_input_file->cd();
  TH1D* h1 = NULL; ttbar_input_file->GetObject("ttbar_nb", h1);

  TFile* wjets_input_file = new TFile("wjets_distributions_2018.root");
  checkFile(wjets_input_file);
  wjets_input_file->cd();
  TH1D* h2 = NULL; wjets_input_file->GetObject("wjets_nb", h2);

  TFile* wz_input_file    = new TFile("wz_distributions_2018.root"   );
  checkFile(wz_input_file);
  wz_input_file->cd();
  TH1D* h3 = NULL; wz_input_file->GetObject("wz_nb", h3);

  TFile* www_input_file   = new TFile("www_distributions_2018.root"  );
  checkFile(www_input_file);
  www_input_file->cd();
  TH1D* h4 = NULL; www_input_file->GetObject("www_nb", h4);

  TFile* data_input_file  = new TFile("data_distributions_2018.root" );
  checkFile(data_input_file);
  data_input_file->cd();
  TH1D* h5 = NULL; data_input_file->GetObject("data_nb", h5);

  // Options for histograms
  TH1D* ttbar_hist = new TH1D("ttbar_hist", "ttbar_hist", 5, 0.5, 5.5);
  ttbar_hist->Add(h1);
  ttbar_hist->SetFillColor(2);
  ttbar_hist->SetLineWidth(2);
  ttbar_hist->SetLineColor(2);

  TH1D* wjets_hist = new TH1D("wjets_hist", "wjets_hist", 5, 0.5, 5.5);
  wjets_hist->Add(h2);
  wjets_hist->SetFillColor(3);
  wjets_hist->SetLineWidth(2);
  wjets_hist->SetLineColor(3);

  TH1D* wz_hist    = new TH1D("wz_hist"   , "wz_hist"   , 5, 0.5, 5.5);
  wz_hist->Add(h3);
  wz_hist->SetFillColor(4);
  wz_hist->SetLineWidth(2);
  wz_hist->SetLineColor(4);

  TH1D* www_hist   = new TH1D("www_hist"  , "www_hist"  , 5, 0.5, 5.5);
  www_hist->Add(h4);
  www_hist->SetFillColor(5);
  www_hist->SetLineWidth(2);
  www_hist->SetLineColor(5);

  TH1D* data_hist  = new TH1D("data_hist" , "data_hist" , 5, 0.5, 5.5);
  data_hist->Add(h5);
  data_hist->SetMarkerColor(1);
  data_hist->SetMarkerStyle(20);
  data_hist->SetMarkerSize(1);
  data_hist->SetMinimum(0);
  data_hist->SetMaximum(20000);

  
  // Creates Stack histogram
  THStack* stack_hist = new THStack("nb_stack", "Number of B jets Histograms");
  stack_hist->Add(ttbar_hist, "HIST");
  stack_hist->Add(wjets_hist, "HIST");
  stack_hist->Add(wz_hist, "HIST");
  stack_hist->Add(www_hist, "HIST");
  stack_hist->SetMinimum(0);
  stack_hist->SetMaximum(20000);

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
  stack_hist->SetName("nb plot");
  stack_hist->SetTitle("Plot of the number of b jets");
  stack_hist->GetXaxis()->SetTitle("# b jets");
  stack_hist->GetXaxis()->CenterTitle(kTRUE);
  stack_hist->GetXaxis()->SetTitleOffset(1.2);
  stack_hist->GetYaxis()->SetTitle("number of events normalized");
  stack_hist->GetYaxis()->CenterTitle(kTRUE);
  stack_hist->GetYaxis()->SetTitleOffset(1.5);

  TRatioPlot* rp = new TRatioPlot(stack_hist, data_hist);
  //  rp->Draw();
 
  TLegend* legend = new TLegend(0.7,0.7,0.48,0.9);
  legend->SetHeader("nb","C");
  legend->AddEntry("ttbar_hist","ttbar","f");
  legend->AddEntry("wjets_hist","wjets","f");
  legend->AddEntry("wz_hist","wz","f");
  legend->AddEntry("www_hist", "www", "f");
  legend->AddEntry("data_hist", "data", "lep");
  legend->Draw();
  canvas->Update();

}
