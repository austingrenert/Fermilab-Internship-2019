#include "universal.h"

using namespace std;

// Called by main plot function
// Function that returns the maximum of two values
Double_t max(Double_t a, Double_t b)
{
  if (a >= b)
    return a;
  if (b > a)
    return b;
}

// Called by main plot function
// Retrieves each histogram by name (string reference) and adds it to the stack
// of histograms
void getHist(TString directory, TString process, TString distribution,
             Int_t color, THStack* &stack, TLegend* &legend)
{
  TFile* input_file = new TFile(directory + process + "_distributions_2018.root");
  input_file->cd();
  checkFile(input_file);
  TH1D* h = NULL; input_file->GetObject(process + "_" + distribution, h);

  TH1D* hist = new TH1D(process + "_hist", process + "_hist", h->GetNbinsX(),
                        h->GetXaxis()->GetXmin(), h->GetXaxis()->GetXmax());
  hist->Add(h);
  hist->SetFillColor(color);
  hist->SetLineWidth(1);
  hist->SetLineColor(color);

  stack->Add(hist, "HIST");
  legend->AddEntry(hist, process, "f");
}

// Main Plotting function
// called in plotting_program.C to plot whatever distribution the user inputs
void plot(TString distribution, TString plot_name, TString distribution_display)
{
  // Directory                                                                           
  TString directory = "~/root/distributions/";

  // SETS UP CANVAS FOR PLOT                                                                    
  Double_t w = 700;
  Double_t h = 750;
  TCanvas* canvas = NULL;
  canvas = new TCanvas("canvas", "canvas", w, h);
  canvas->SetWindowSize(w + (w - canvas->GetWw()), h + (h - canvas->GetWh()));
  canvas->SetBorderSize(2);
  canvas->SetLeftMargin(0.10);
  canvas->SetFillColor(10);
  canvas->SetGrid();
  canvas->cd();

  // THStack
  // Creates stack of histograms
  THStack* stack_hist = new THStack(distribution + " plot",
                                    plot_name + " Stack Histogram");

  // TLegend
  // Creates a legend for the plot
  TLegend* legend = new TLegend(0.6,0.6,0.9,0.9);
  legend->SetHeader(distribution_display, "C");
  legend->SetNColumns(4);
  legend->SetMargin(0.2);

  //============================================================================================================================|
  //============================================================================================================================|
  // AREA TO EDIT WHEN ADDING NEW PROCESSES                                              
  // - when adding a new process, add a string of that process at the end of process_names
  // - after that add a value for the color of that process at the end of colors
  // - done, only those 2 steps when adding processes 
  // Vector of the names of the process' used in plotting                                

  vector<TString> process_names =
  {"ttbar", "ttgam", "wjets", "wz", "zz", "dy", "ttz", "ttw", "tZq", "vh", "ggh",
   "tgjets", "vbsww", "sts", "wg", "zg", "wzg"};

  vector<Int_t> colors =
  {40, 41, 43, 46, 33, 36, 38, 20, 8, kOrange - 3, kPink - 3, kTeal + 3, kAzure + 3,
   kOrange + 3, kAzure - 6, kOrange + 5, kViolet + 3};

  //============================================================================================================================|
  //============================================================================================================================|
  // for each process added, retrieves the desired histograms and adds them to stack

  for (Int_t i = 0; i < process_names.size(); i++)
    {
      getHist(directory, process_names[i], distribution, colors[i], stack_hist, legend);
    }

  //============================================================================================================================|
  // SIGNAL and DATA
  // signal and data distributions are plotted side by side with stack histogram and are
  // not a part of it
  TFile* data_input_file = new TFile(directory + "data_distributions_2018.root");
  checkFile(data_input_file);
  data_input_file->cd();
  TH1D* data = NULL; data_input_file->GetObject("data_" + distribution, data);
  TH1D* data_hist = new TH1D("data_hist", "data_hist", data->GetNbinsX(),
			     data->GetXaxis()->GetXmin(), data->GetXaxis()->GetXmax());
  data_hist->Add(data);
  data_hist->SetMarkerColor(1);
  data_hist->SetMarkerStyle(20);
  data_hist->SetMarkerSize(1);

  // Sets range
  Double_t data_max = data_hist->GetMaximum();
  Double_t stack_max = stack_hist->GetMaximum();
  data_hist->SetMinimum(1);
  data_hist->SetMaximum(max(data_max, stack_max));

  TFile* signal_input_file = new TFile(directory + "www_distributions_2018.root");
  checkFile(signal_input_file);
  signal_input_file->cd();
  TH1D* signal = NULL; signal_input_file->GetObject("www_" + distribution, signal);
  TH1D* www_hist = new TH1D("www_hist", "www_hist", signal->GetNbinsX(),
			    signal->GetXaxis()->GetXmin(), signal->GetXaxis()->GetXmin());
  www_hist->SetFillColor(1);
  www_hist->SetLineWidth(1);
  www_hist->SetLineColor(1);

  legend->AddEntry(www_hist, "www", "f");
  legend->AddEntry(data_hist, "data", "lep");

  // DRAWS STACK HISTOGRAM                                                               
  stack_hist->SetMinimum(1);
  stack_hist->SetMaximum(max(data_max, stack_max));
  stack_hist->Draw();
  data_hist->Draw("E1, SAME");
  www_hist->Draw("HIST, SAME");
  stack_hist->SetName(distribution_display + " plot");
  stack_hist->SetTitle("Plot of " + distribution_display + "(mm)");
  stack_hist->GetXaxis()->SetTitle(distribution_display);
  stack_hist->GetXaxis()->CenterTitle(kTRUE);
  stack_hist->GetXaxis()->SetTitleOffset(1.4);
  stack_hist->GetYaxis()->SetTitle("events (normalized)");
  stack_hist->GetYaxis()->CenterTitle(kTRUE);
  stack_hist->GetYaxis()->SetTitleOffset(1.5);

  // Ratio Plot                                                                          
  TRatioPlot* rp = new TRatioPlot(stack_hist, data_hist);
  rp->Draw();
  TPad* upper_pad(rp->GetUpperPad());
  upper_pad->SetLogy();

  rp->GetLowerPad()->cd();
  rp->GetLowerRefGraph()->SetMinimum(0.);
  rp->GetLowerRefGraph()->SetMaximum(2.0);
  rp->GetLowYaxis()->SetNdivisions(505);

  rp->GetUpperPad()->cd();
  legend->Draw();
  canvas->Update();
}
