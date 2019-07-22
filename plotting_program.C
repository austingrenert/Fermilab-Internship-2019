#include "plotting.C"
#include "universal.h"

using namespace std;

void plotting_program()
{
  // When adding a distribution in auto_looper.C, append the name of the distribution    
  // to vector called distributions to update this file                                  
  // Vector of all the different distributions currently being used in plotting          
  vector<TString> distributions =
    {"el_pt", "mu_pt", "nj30", "MjjL", "met_pt", "lep_pt_1", "lep_pt_2", "nb",
  //----1--------2-------3-------4---------5----------6-----------7-------8---           
     "n_lep"};
  //----9-----

  vector<TString> distribution_display = 
    {"e_{pt}", "#mu_{pt}", "# Jets", "MjjL", "Missing p_{t}", "l_{pt}^1", 
  //-----1--------------2------------3-------4-----------5---------------6--------
     "l_{pt}^2", "# B Jets", "# l"};
  //-------7-------------8--------9----

  // UI
  // displays a program that lists out all the saved distributions and allows the user to choose
  // which one they would like to plot, and then calls function plot from plotting.C
  cout << "|=============================================================|" << endl;
  cout << "|               Welcome to the plotting program.              |" << endl;
  cout << "|=============================================================|" << endl;
  cout << "|Enter in a number below to plot that particular distribution:|" << endl;
  for (Int_t i = 1; i <= distributions.size(); i++)
    {
      cout << "|    " << setw(1) << left << i
           << ": " << setw(54) << left << distributions[i - 1]
           << "|" << endl;
    }
  cout << "|=============================================================|" << endl << endl;
  while (kTRUE)
    {
      Int_t choice;
      cout << "Which distribution would you like to plot (0 to exit): ";
      cin  >> choice;
      if (choice > 0 && choice <= distributions.size())
        {
          plot(distributions[choice - 1], "Plot of " + distributions[choice - 1], 
	       distribution_display[choice - 1]);
        }
      else if (choice == 0)
        {
          break;
        }
      else
        {
          cout << "Error: Invalid Input..." << endl;
        }
    }
}
