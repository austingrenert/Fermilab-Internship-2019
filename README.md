# Fermilab-Internship-2019
This repository contains all code, files, and images I created / worked on during my 10 week Fermilab Summer Internship in 2019. All code in the files was coded by me in C++.

# ROOT
Root is the data analysis framework used to plot various distributions for different processes produced during collisions at the LHC (Large Hadron Collider) in CERN, Framework reference is at this link
  - Root Cern Reference: https://root.cern/doc/master/index.html

# Steps Taken
In order to plot data and simulation for analysis, there are 3 steps.
- Step 1: Access .root files containing data trees for all the different distributions (i.e. electron momentum, muon momentum, number of leptons, quark jets, etc.)
- Step 2: Loop through all entries in trees, apply cuts, and save those histograms into a new .root file
- Step 3: Access the files that contain the data after cuts and plot using various display options

# Files
  - chain_looper.C: creates a TChain for each set of .root files corresponding to the different processes (ttbar, ttgam, wz, zz,       etc), and calls the looper in auto_looper.C for each chain
  - auto_looper.C: for each TChain in chain_looper.C, loop through entries, apply cuts, and then save that data in a new .root file corresponding to the process name.
  - plotting.C: contains the main function for opening the .root files after cuts and then plotting that data in the desired form. (data vs. simulation in the form of a stack histogram)
  - plotting_program.C: calls plotting.C, allows user to plot all plots from one program instead of multiple
  - universal.h: universal header file containg various c++ library includes as well as many ROOT framework library includes and a useful function for checking file input.
  
# Other
All files were coded in Linux command line on a mac laptop. Before copying all files to the local system, all coding was done on a remote machine through an SSH connection. So basic knowledge and skills surrounding Linux command line (i.e. navigating through directories, opening and editing files, copying files on both local and remote systems, and downloading files from github) were aquired.

# WWWAnalysis
The files contained in this folder were part of a bigger framework created by another employee at fermilab. All the files contained are files that I have personally gone through and edited in order to allow the framework to analyze the new simulation sample for the particular particle interaction that we were looking for. These files are big, the credit goes to Philip Chang of fermilab for creating this framework and helping me understand it.

