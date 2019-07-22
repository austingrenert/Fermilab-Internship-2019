#ifndef LOOPER_H
#define LOOPER_H

#include "TChain.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TTree.h"
#include "TH2.h"
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

void checkTree(TTree* tree)
{
  if (tree == 0)
    {
      std::cout << "Error: Unable to Find Tree..." << endl;
      return;
    }
}

#endif
