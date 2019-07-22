#ifndef PLOTTER_H
#define PLOTTER_H

#include "TLegend.h"
#include "TChain.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TTree.h"
#include "TH2.h"
#include "TLine.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

void checkFile(TFile* file)
{
  if (file == 0)
    {
      std::cout << "Error: Unable to Open File..." << endl;
      return;
    }
}

#endif
