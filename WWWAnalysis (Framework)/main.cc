#include "main.h"

//_______________________________________________________________________________________________________
int main(int argc, char** argv)
{

//********************************************************************************
//
// 0. Notes on global configuration classes and global TTree reader class
//
//********************************************************************************

    // 1.

    // There is a global variable defined in AnalysisConfig.cc
    //
    //   AnalysisConfig ana;
    //
    // This class instance contains configuration that persists through the entire run time.
    // (e.g. number of events, the input file name list, etc. that will not change through run time)
    // (see AnalysisConfig.h to get a feeling)

    // 2.

    // There is a global variable defined in AnalysisConfig.cc
    //
    //   InputConfig input;
    //
    // This class instance contains configuration that persists only through a single ROOT file in the chain of input ROOT files.
    // Everytime looper loads new file, the configuration in this class is reconfigured to handle sample dependent behaviors.
    // (e.g. is_data? is_sig? is_bkg? etc.)
    // (see InputConfig.h to get a feeling)

    // 3.

    // Then there is a global variable defined in wwwtree.cc
    //
    //    wwwtree www;
    //
    // This is an instance which helps read variables in the WWW baby TTree
    // It is a giant wrapper that facilitates reading TBranch values.
    // e.g. if there is a TBranch named "lep_pt" which is a std::vector<float> then, one can access the branch via
    //
    //    std::vector<float> lep_pt = www.lep_pt();
    //
    // and no need for "SetBranchAddress" and declaring variable shenanigans necessary.
    // This is a standard thing SNT does pretty much every looper we use.

    // 4.

    // The there are a few scale factor reader classes that are also globally defined
    // Those are defined in scalefactors.h
    // They are nothing more than just reading TH's from ROOT files and retrieving the contents in a simpler way

//********************************************************************************
//
// 1. Parsing options
//
//********************************************************************************

    // cxxopts is just a tool to parse argc, and argv easily

    // Grand option setting
    cxxopts::Options options("\n  $ doAnalysis",  "\n         **********************\n         *                    *\n         * Run 2 WWW Analysis *\n         *                    *\n         **********************\n");

    // Read the options
    options.add_options()
        ("i,input"       , "Comma separated input file list OR if just a directory is provided it will glob all in the directory BUT must end with '/' for the path", cxxopts::value<std::string>())
        ("T,tree"        , "Name of the TTree in the input file to loop over"                                                    , cxxopts::value<std::string>())
        ("o,output"      , "Output file name"                                                                                    , cxxopts::value<std::string>())
        ("n,nevents"     , "N events to loop over"                                                                               , cxxopts::value<int>()->default_value("-1"))
        ("t,test"        , "Run test job. i.e. overrides output option to 'test.root' and 'recreate's the file.")
        ("H,hist"        , "Book histogram")
        ("C,cutflow"     , "Book cutflows")
        ("L,eventlist"   , "Book event list")
        ("S,systematics" , "Also consider systematics (NOTE: Generally, this limits the number of histograms and cutflows. use -f,--force to run all.)")
        ("f,force"       , "force process all and not limit any number of histograms and cutflows")
        ("F,fake"        , "The event weight will be multiplied by fake weights")
        ("u,user_study"  , "Enable user_study function for analyzers to make their own studies")
        ("j,nsplit_jobs" , "Enable splitting jobs by N blocks (--job_index must be set)", cxxopts::value<int>())
        ("I,job_index"   , "job_index of split jobs (--nsplit_jobs must be set. index starts from 0. i.e. 0, 1, 2, 3, etc...)", cxxopts::value<int>())
        ("h,help"        , "Print help")
        ;

    auto result = options.parse(argc, argv);

    // NOTE: When an option was provided (e.g. -i or --input), then the result.count("<option name>") is more than 0
    // Therefore, the option can be parsed easily by asking the condition if (result.count("<option name>");
    // That's how the several options are parsed below

    //_______________________________________________________________________________
    // --help
    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        exit(1);
    }

    //_______________________________________________________________________________
    // --input
    if (result.count("input"))
    {
        ana.input_file_list_tstring = result["input"].as<std::string>();
    }
    else
    {
        std::cout << options.help() << std::endl;
        std::cout << "ERROR: Input list is not provided! Check your arguments" << std::endl;
        exit(1);
    }

    //_______________________________________________________________________________
    // --nevents
    ana.n_events = result["nevents"].as<int>();

    //_______________________________________________________________________________
    // --test
    if (result.count("test"))
    {
        ana.output_tfile = new TFile("test.root", "recreate");
    }
    else
    {
        //_______________________________________________________________________________
        // --output
        if (result.count("output"))
        {
            ana.output_tfile = new TFile(result["output"].as<std::string>().c_str(), "create");
            if (not ana.output_tfile->IsOpen())
            {
                std::cout << options.help() << std::endl;
                std::cout << "ERROR: output already exists! provide new output name or delete old file. OUTPUTFILE=" << result["output"].as<std::string>() << std::endl;
                exit(1);
            }
        }
        else
        {
            std::cout << options.help() << std::endl;
            std::cout << "ERROR: Output file name is not provided! Check your arguments" << std::endl;
            exit(1);
        }
    }

    //_______________________________________________________________________________
    // --tree
    if (result.count("tree"))
    {
        ana.input_tree_name = result["tree"].as<std::string>();
    }
    else
    {
        std::cout << options.help() << std::endl;
        std::cout << "ERROR: Input tree name is not provided! Check your arguments" << std::endl;
        exit(1);
    }

    //_______________________________________________________________________________
    // --hist
    if (result.count("hist"))
    {
        ana.do_histograms = true;
    }
    else
    {
        ana.do_histograms = false;
    }

    //_______________________________________________________________________________
    // --cutflow
    if (result.count("cutflow"))
    {
        ana.do_cutflow = true;
    }
    else
    {
        ana.do_cutflow = false;
    }

    //_______________________________________________________________________________
    // --eventlist
    if (result.count("eventlist"))
    {
        ana.do_eventlist = true;
    }
    else
    {
        ana.do_eventlist = false;
    }

    //_______________________________________________________________________________
    // --systematics
    if (result.count("systematics"))
    {
        ana.do_systematics = true;
    }
    else
    {
        ana.do_systematics = false;
    }

    //_______________________________________________________________________________
    // --force
    if (result.count("force"))
    {
        ana.do_force = true;
    }
    else
    {
        ana.do_force = false;
    }

    //_______________________________________________________________________________
    // --fake
    if (result.count("fake"))
    {
        ana.do_fake_estimation = true;
    }
    else
    {
        ana.do_fake_estimation = false;
    }

    //_______________________________________________________________________________
    // --user_study
    if (result.count("user_study"))
    {
        ana.do_user_study = true;
    }
    else
    {
        ana.do_user_study = false;
    }

    //_______________________________________________________________________________
    // --nsplit_jobs
    if (result.count("nsplit_jobs"))
    {
        ana.nsplit_jobs = result["nsplit_jobs"].as<int>();
        if (ana.nsplit_jobs <= 0)
        {
            std::cout << options.help() << std::endl;
            std::cout << "ERROR: option string --nsplit_jobs" << ana.nsplit_jobs << " has zero or negative value!" << std::endl;
            std::cout << "I am not sure what this means..." << std::endl;
            exit(1);
        }
    }
    else
    {
        ana.nsplit_jobs = -1;
    }

    //_______________________________________________________________________________
    // --nsplit_jobs
    if (result.count("job_index"))
    {
        ana.job_index = result["job_index"].as<int>();
        if (ana.job_index < 0)
        {
            std::cout << options.help() << std::endl;
            std::cout << "ERROR: option string --job_index" << ana.job_index << " has negative value!" << std::endl;
            std::cout << "I am not sure what this means..." << std::endl;
            exit(1);
        }
    }
    else
    {
        ana.job_index = -1;
    }


    // Sanity check for split jobs (if one is set the other must be set too)
    if (result.count("job_index") or result.count("nsplit_jobs"))
    {
        // If one is not provided then throw error
        if ( not (result.count("job_index") and result.count("nsplit_jobs")))
        {
            std::cout << options.help() << std::endl;
            std::cout << "ERROR: option string --job_index and --nsplit_jobs must be set at the same time!" << std::endl;
            exit(1);
        }
        // If it is set then check for sanity
        else
        {
            if (ana.job_index >= ana.nsplit_jobs)
            {
                std::cout << options.help() << std::endl;
                std::cout << "ERROR: --job_index >= --nsplit_jobs ! This does not make sense..." << std::endl;
                exit(1);
            }
        }
    }

    //
    // Printing out the option settings overview
    //
    std::cout <<  "=========================================================" << std::endl;
    std::cout <<  " Setting of the analysis job based on provided arguments " << std::endl;
    std::cout <<  "---------------------------------------------------------" << std::endl;
    std::cout <<  " ana.input_file_list_tstring: " << ana.input_file_list_tstring <<  std::endl;
    std::cout <<  " ana.output_tfile: " << ana.output_tfile->GetName() <<  std::endl;
    std::cout <<  " ana.n_events: " << ana.n_events <<  std::endl;
    std::cout <<  " ana.do_cutflow: " << ana.do_cutflow <<  std::endl;
    std::cout <<  " ana.do_eventlist: " << ana.do_eventlist <<  std::endl;
    std::cout <<  " ana.do_histograms: " << ana.do_histograms <<  std::endl;
    std::cout <<  " ana.do_systematics: " << ana.do_systematics <<  std::endl;
    std::cout <<  " ana.do_force: " << ana.do_force <<  std::endl;
    std::cout <<  " ana.do_fake_estimation: " << ana.do_fake_estimation <<  std::endl;
    std::cout <<  " ana.do_user_study: " << ana.do_user_study <<  std::endl;
    std::cout <<  " ana.nsplit_jobs: " << ana.nsplit_jobs <<  std::endl;
    std::cout <<  " ana.job_index: " << ana.job_index <<  std::endl;
    std::cout <<  "=========================================================" << std::endl;

//********************************************************************************
//
// 2. Opening input baby files
//
//********************************************************************************

    // Create the TChain that holds the TTree's of the baby ntuples
    ana.events_tchain = RooUtil::FileUtil::createTChain(ana.input_tree_name, ana.input_file_list_tstring);

    // Create a Looper object to loop over input files
    // the "www" object is defined in the wwwtree.h/cc
    // This is an instance which helps read variables in the WWW baby TTree
    // It is a giant wrapper that facilitates reading TBranch values.
    // e.g. if there is a TBranch named "lep_pt" which is a std::vector<float> then, one can access the branch via
    //
    //    std::vector<float> lep_pt = www.lep_pt();
    //
    // and no need for "SetBranchAddress" and declaring variable shenanigans necessary
    // This is a standard thing SNT does pretty much every looper we use
    ana.looper.init(ana.events_tchain, &www, ana.n_events);

//********************************************************************************
//
// Interlude... notes on RooUtil framework (This is a custom thing built by P. Chang)
//
//********************************************************************************

    // ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
    // Quick tutorial on RooUtil::Cutflow object cut tree formation
    // ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
    //
    // The RooUtil::Cutflow object facilitates creating a tree structure of cuts
    //
    // To add cuts to each node of the tree with cuts defined, use "addCut" or "addCutToLastActiveCut"
    // The "addCut" or "addCutToLastActiveCut" accepts three argument, <name>, and two lambda's that define the cut selection, and the weight to apply to that cut stage
    //
    // e.g. To create following cut-tree structure one does
    //
    //             (Root) <--- Always exists as soon as RooUtil::Cutflow object is created. But this is basically hidden underneath and users do not have to care
    //                |
    //            CutWeight
    //            |       |
    //     CutPreSel1    CutPreSel2
    //       |                  |
    //     CutSel1           CutSel2
    //
    //
    //   code:
    //
    //      // Create the object (Root node is created as soon as the instance is created)
    //      RooUtil::Cutflow cutflow;
    //
    //      cutflow.addCut("CutWeight"                 , <lambda> , <lambda>); // CutWeight is added below "Root"-node
    //      cutflow.addCutToLastActiveCut("CutPresel1" , <lambda> , <lambda>); // The last "active" cut is "CutWeight" since I just added that. So "CutPresel1" is added below "CutWeight"
    //      cutflow.addCutToLastActiveCut("CutSel1"    , <lambda> , <lambda>); // The last "active" cut is "CutPresel1" since I just added that. So "CutSel1" is added below "CutPresel1"
    //
    //      cutflow.getCut("CutWeight"); // By "getting" the cut object, this makes the "CutWeight" the last "active" cut.
    //      cutflow.addCutToLastActiveCut("CutPresel2" , <lambda> , <lambda>); // The last "active" cut is "CutWeight" since I "getCut" on it. So "CutPresel2" is added below "CutWeight"
    //      cutflow.addCutToLastActiveCut("CutSel2"    , <lambda> , <lambda>); // The last "active" cut is "CutPresel2" since I just added that. So "CutSel2" is added below "CutPresel1"
    //
    // (Side note: "UNITY" lambda is defined in the framework to just return 1. This so that use don't have to type [&]() {return 1;} so many times.)
    //
    // Once the cutflow is formed, create cutflow histograms can be created by calling RooUtil::Cutflow::bookCutflows())
    // This function looks through the terminating nodes of the tree structured cut tree. and creates a histogram that will fill the yields
    // For the example above, there are two terminationg nodes, "CutSel1", and "CutSel2"
    // So in this case Root::Cutflow::bookCutflows() will create two histograms. (Actually four histograms.)
    //
    //  - TH1F* type object :  CutSel1_cutflow (4 bins, with first bin labeled "Root", second bin labeled "CutWeight", third bin labeled "CutPreSel1", fourth bin labeled "CutSel1")
    //  - TH1F* type object :  CutSel2_cutflow (...)
    //  - TH1F* type object :  CutSel1_rawcutflow (...)
    //  - TH1F* type object :  CutSel2_rawcutflow (...)
    //                                ^
    //                                |
    // NOTE: There is only one underscore "_" between <CutName>_cutflow or <CutName>_rawcutflow
    //
    // And later in the loop when RooUtil::Cutflow::fill() function is called, the tree structure will be traversed through and the appropriate yields will be filled into the histograms
    //
    // After running the loop check for the histograms in the output root file

    // ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
    // Quick tutorial on RooUtil::Histograms object
    // ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
    //
    // The RooUtil::Histograms object facilitates book keeping histogram definitions
    // And in conjunction with RooUtil::Cutflow object, one can book same histograms across different cut stages easily without copy pasting codes many times by hand.
    //
    // The histogram addition happens in two steps.
    // 1. Defining histograms
    // 2. Booking histograms to cuts
    //
    // Histograms are defined via following functions
    //
    //      RooUtil::Histograms::addHistogram       : Typical 1D histogram (TH1F*) "Fill()" called once per event
    //      RooUtil::Histograms::addVecHistogram    : Typical 1D histogram (TH1F*) "Fill()" called multiple times per event
    //      RooUtil::Histograms::add2DHistogram     : Typical 2D histogram (TH2F*) "Fill()" called once per event
    //      RooUtil::Histograms::add2DVecHistogram  : Typical 2D histogram (TH2F*) "Fill()" called multiple times per event
    // e.g.
    //
    //    RooUtil::Histograms histograms;
    //    histograms.addHistogram   ("MllSS"    , 180 , 0. , 300. , [&]() { return www.MllSS()  ; }); // The lambda returns float
    //    histograms.addVecHistogram("AllLepPt" , 180 , 0. , 300. , [&]() { return www.lep_pt() ; }); // The lambda returns vector<float>
    //
    // The addVecHistogram will have lambda to return vector<float> and it will loop over the values and call TH1F::Fill() for each item
    //
    // To book histograms to cuts one uses
    //
    //      RooUtil::Cutflow::bookHistogramsForCut()
    //      RooUtil::Cutflow::bookHistogramsForCutAndBelow()
    //      RooUtil::Cutflow::bookHistogramsForCutAndAbove()
    //      RooUtil::Cutflow::bookHistogramsForEndCuts()
    //
    // e.g. Given a tree like the following, we can book histograms to various cuts as we want
    //
    //              Root
    //                |
    //            CutWeight
    //            |       |
    //     CutPreSel1    CutPreSel2
    //       |                  |
    //     CutSel1           CutSel2
    //
    // For example,
    //
    //    1. book a set of histograms to one cut:
    //
    //       cutflow.bookHistogramsForCut(histograms, "CutPreSel2")
    //
    //    2. book a set of histograms to a cut and below
    //
    //       cutflow.bookHistogramsForCutAndBelow(histograms, "CutWeight") // will book a set of histograms to CutWeight, CutPreSel1, CutPreSel2, CutSel1, and CutSel2
    //
    //    3. book a set of histograms to a cut and above (... useless...?)
    //
    //       cutflow.bookHistogramsForCutAndAbove(histograms, "CutPreSel2") // will book a set of histograms to CutPreSel2, CutWeight (nothing happens to Root node)
    //
    //    4. book a set of histograms to a terminating nodes
    //
    //       cutflow.bookHistogramsForEndCuts(histograms) // will book a set of histograms to CutSel1 and CutSel2
    //
    // The naming convention of the booked histograms are as follows
    //
    //   cutflow.bookHistogramsForCut(histograms, "CutSel1");
    //
    //  - TH1F* type object : CutSel1__MllSS;
    //  - TH1F* type object : CutSel1__AllLepPt;
    //                               ^^
    //                               ||
    // NOTE: There are two underscores "__" between <CutName>__<HistogramName>
    //
    // And later in the loop when RooUtil::CutName::fill() function is called, the tree structure will be traversed through and the appropriate histograms will be filled with appropriate variables
    // After running the loop check for the histograms in the output root file

//********************************************************************************
//
// 3. Defining histograms
//
//********************************************************************************

    // The RooUtil::Histograms class holds the definitions of histograms with the lambda expression for the variable
    // Then this is used in conjunction with RooUtil::Cutflow to book histograms at different cut stage
    // This is so that the users don't have to copy paste a thousands lines of codes if they want to book more histograms at different cut stages

    ana.histograms.addHistogram("MllSS"                    ,  180 , 0.      , 500.   , [&]() { return www.MllSS()                                                                  ; });
    ana.histograms.addHistogram("MllSS_wide"               ,  180 , 0.      , 2000.  , [&]() { return www.MllSS()                                                                  ; });
    ana.histograms.addHistogram("MllZ"                     ,  180 , 60.     , 120.   , [&]() { return www.MllSS()                                                                  ; });
    ana.histograms.addHistogram("MllZZoom"                 ,  180 , 80.     , 100.   , [&]() { return www.MllSS()                                                                  ; });
    ana.histograms.addHistogram("MllOnOff"                 ,  180 , 30.     , 150.   , [&]() { return fabs(www.Mll3L()-91.1876)<fabs(www.Mll3L1()-91.1876)?www.Mll3L():www.Mll3L1(); });
    ana.histograms.addHistogram("Mll3L"                    ,  180 , 0.      , 300.   , [&]() { return www.Mll3L()                                                                  ; });
    ana.histograms.addHistogram("Mll3L1"                   ,  180 , 0.      , 300.   , [&]() { return www.Mll3L1()                                                                 ; });
    ana.histograms.addHistogram("nSFOSinZ"                 ,  3   , 0.      , 3.     , [&]() { return www.nSFOSinZ()                                                               ; });
    ana.histograms.addHistogram("M3l"                      ,  180 , 0.      , 150.   , [&]() { return www.M3l()                                                                    ; });
    ana.histograms.addHistogram("Pt3lGCR"                  ,  180 , 0.      , 100.   , [&]() { return www.Pt3l()                                                                   ; });
    ana.histograms.addHistogram("Pt3l"                     ,  180 , 0.      , 300.   , [&]() { return www.Pt3l()                                                                   ; });
    ana.histograms.addHistogram("Ptll"                     ,  180 , 0.      , 300.   , [&]() { return www.Pt3l()                                                                   ; });
    ana.histograms.addHistogram("nvtx"                     ,  60  , 0.      , 60.    , [&]() { return www.nVert()                                                                  ; });
    ana.histograms.addHistogram("MjjZoom"                  ,  180 , 0.      , 150.   , [&]() { return www.Mjj()                                                                    ; });
    ana.histograms.addHistogram("Mjj"                      ,  180 , 1.      , 300.   , [&]() { return www.Mjj()                                                                    ; });
    ana.histograms.addHistogram("MjjLZoom"                 ,  180 , 0.      , 300.   , [&]() { return www.MjjL()                                                                   ; });
    ana.histograms.addHistogram("MjjL"                     ,  180 , 0.      , 750.   , [&]() { return www.MjjL()                                                                   ; });
    ana.histograms.addHistogram("DetajjL"                  ,  180 , 0.      , 5.     , [&]() { return www.DetajjL()                                                                ; });
    ana.histograms.addHistogram("MjjVBF"                   ,  180 , 0.      , 750.   , [&]() { return www.MjjVBF()                                                                 ; });
    ana.histograms.addHistogram("DetajjVBF"                ,  180 , 0.      , 8.     , [&]() { return www.DetajjVBF()                                                              ; });
    ana.histograms.addHistogram("MjjVBS"                   ,  180 , 0.      , 750.   , [&]() { return www.Mjj()                                                                    ; });
    ana.histograms.addHistogram("DetajjVBS"                ,  180 , 0.      , 3.     , [&]() { return www.DetajjL()                                                                ; });
    ana.histograms.addHistogram("MET"                      ,  180 , 0.      , 180.   , [&]() { return www.met_pt()                                                                 ; });
    ana.histograms.addHistogram("METWide"                  ,  180 , 0.      , 300.   , [&]() { return www.met_pt()                                                                 ; });
    ana.histograms.addHistogram("jets_pt0"                 ,  180 , 0.      , 250    , [&]() { return www.jets_p4().size() > 0 ? www.jets_p4()[0].pt()  : -999                     ; });
    ana.histograms.addHistogram("jets_pt1"                 ,  180 , 0.      , 150    , [&]() { return www.jets_p4().size() > 1 ? www.jets_p4()[1].pt()  : -999                     ; });
    ana.histograms.addHistogram("jets_pt2"                 ,  180 , 0.      , 150    , [&]() { return www.jets_p4().size() > 2 ? www.jets_p4()[2].pt()  : -999                     ; });
    ana.histograms.addHistogram("jets_eta0"                ,  180 , -5.0    , 5.0    , [&]() { return www.jets_p4().size() > 0 ? www.jets_p4()[0].eta() : -999                     ; });
    ana.histograms.addHistogram("jets_eta1"                ,  180 , -5.0    , 5.0    , [&]() { return www.jets_p4().size() > 1 ? www.jets_p4()[1].eta() : -999                     ; });
    ana.histograms.addHistogram("jets_eta2"                ,  180 , -5.0    , 5.0    , [&]() { return www.jets_p4().size() > 2 ? www.jets_p4()[2].eta() : -999                     ; });
    ana.histograms.addHistogram("lep_pt0"                  ,  180 , 0.      , 250    , [&]() { return www.lep_pt().size() > 0 ? www.lep_pt()[0]  : -999                            ; });
    ana.histograms.addHistogram("lep_pt1"                  ,  180 , 0.      , 150    , [&]() { return www.lep_pt().size() > 1 ? www.lep_pt()[1]  : -999                            ; });
    ana.histograms.addHistogram("lep_pt2"                  ,  180 , 0.      , 150    , [&]() { return www.lep_pt().size() > 2 ? www.lep_pt()[2]  : -999                            ; });
    ana.histograms.addHistogram("el_pt2"                   ,  180 , 0.      , 150    , [&]() { return www.lep_pt().size() > 2 and abs(www.lep_pdgId()[2]) == 11 ? www.lep_pt()[2] : -999; });
    ana.histograms.addHistogram("mu_pt2"                   ,  180 , 0.      , 150    , [&]() { return www.lep_pt().size() > 2 and abs(www.lep_pdgId()[2]) == 13 ? www.lep_pt()[2] : -999; });
    ana.histograms.addHistogram("lep_eta0"                 ,  180 , -2.5    , 2.5    , [&]() { return www.lep_pt().size() > 0 ? www.lep_eta()[0] : -999                            ; });
    ana.histograms.addHistogram("lep_eta1"                 ,  180 , -2.5    , 2.5    , [&]() { return www.lep_pt().size() > 1 ? www.lep_eta()[1] : -999                            ; });
    ana.histograms.addHistogram("lep_eta2"                 ,  180 , -2.5    , 2.5    , [&]() { return www.lep_pt().size() > 2 ? www.lep_eta()[2] : -999                            ; });
    ana.histograms.addHistogram("lep_eta_fwd"              ,  {0., 1.6, 2.4}         , [&]() { return www.lep_pt().size() > 2 ? std::max(std::max(fabs(www.lep_eta()[0]), fabs(www.lep_eta()[1])), fabs(www.lep_eta()[2])) : -999; });
    ana.histograms.addHistogram("el_eta2"                  ,  180 , -2.5    , 2.5    , [&]() { return www.lep_pt().size() > 2 and abs(www.lep_pdgId()[2]) == 11 ? www.lep_eta()[2] : -999; });
    ana.histograms.addHistogram("mu_eta2"                  ,  180 , -2.5    , 2.5    , [&]() { return www.lep_pt().size() > 2 and abs(www.lep_pdgId()[2]) == 13 ? www.lep_eta()[2] : -999; });
    ana.histograms.addHistogram("lep_phi0"                 ,  180 , -3.1416 , 3.1416 , [&]() { return www.lep_pt().size() > 0 ? www.lep_phi()[0] : -999                            ; });
    ana.histograms.addHistogram("lep_phi1"                 ,  180 , -3.1416 , 3.1416 , [&]() { return www.lep_pt().size() > 1 ? www.lep_phi()[1] : -999                            ; });
    ana.histograms.addHistogram("lep_phi2"                 ,  180 , -3.1416 , 3.1416 , [&]() { return www.lep_pt().size() > 2 ? www.lep_phi()[2] : -999                            ; });
    ana.histograms.addHistogram("el_phi2"                  ,  180 , -3.1416 , 3.1416 , [&]() { return www.lep_pt().size() > 2 and abs(www.lep_pdgId()[2]) == 11 ? www.lep_phi()[2] : -999                            ; });
    ana.histograms.addHistogram("mu_phi2"                  ,  180 , -3.1416 , 3.1416 , [&]() { return www.lep_pt().size() > 2 and abs(www.lep_pdgId()[2]) == 13 ? www.lep_phi()[2] : -999                            ; });
    ana.histograms.addHistogram("lep_relIso03EAv2Lep0"     ,  180 , 0.0     , 0.4    , [&]() { return www.lep_pt().size() > 0 ? www.lep_relIso03EAv2Lep()[0] : -999                ; });
    ana.histograms.addHistogram("lep_relIso03EAv2Lep1"     ,  180 , 0.0     , 0.4    , [&]() { return www.lep_pt().size() > 1 ? www.lep_relIso03EAv2Lep()[1] : -999                ; });
    ana.histograms.addHistogram("lep_relIso03EAv2Lep2"     ,  180 , 0.0     , 0.4    , [&]() { return www.lep_pt().size() > 2 ? www.lep_relIso03EAv2Lep()[2] : -999                ; });
    ana.histograms.addHistogram("lep_relIso03EAv2Max"      ,  180 , 0.0     , 0.4    , [&]() { return www.lep_pt().size() > 2 ? std::max(std::max(fabs(www.lep_relIso03EAv2Lep()[0]), fabs(www.lep_relIso03EAv2Lep()[1])), fabs(www.lep_relIso03EAv2Lep()[2])) : -999; });
    ana.histograms.addHistogram("lep_ptcorr"               ,  180 , 0.      , 150    , [&]() { return fakerates.getPtCorr()                                                        ; });
    ana.histograms.addHistogram("el_ptcorr"                ,  {0., 20., 25., 30., 35., 50., 150.}, [&]() { return abs(www.lep_pdgId()[fakerates.getFakeLepIndex()]) == 11 ? fakerates.getPtCorr() : 0; });
    ana.histograms.addHistogram("mu_ptcorr"                ,  {0., 20., 25., 30., 35., 50., 150.}, [&]() { return abs(www.lep_pdgId()[fakerates.getFakeLepIndex()]) == 13 ? fakerates.getPtCorr() : 0; });
    ana.histograms.addHistogram("el_ptcorr_cen"            ,  {0., 20., 25., 30., 35., 50., 150.}, [&]() { return abs(www.lep_pdgId()[fakerates.getFakeLepIndex()]) == 11 and fabs(www.lep_pdgId()[fakerates.getFakeLepIndex()]) < 1.6 ? fakerates.getPtCorr() : 0; });
    ana.histograms.addHistogram("mu_ptcorr_cen"            ,  {0., 20., 25., 30., 35., 50., 150.}, [&]() { return abs(www.lep_pdgId()[fakerates.getFakeLepIndex()]) == 13 and fabs(www.lep_pdgId()[fakerates.getFakeLepIndex()]) < 1.6 ? fakerates.getPtCorr() : 0; });
    ana.histograms.addHistogram("el_ptcorr_fwd"            ,  {0., 20., 25., 30., 35., 50., 150.}, [&]() { return abs(www.lep_pdgId()[fakerates.getFakeLepIndex()]) == 11 and fabs(www.lep_pdgId()[fakerates.getFakeLepIndex()]) >=1.6 ? fakerates.getPtCorr() : 0; });
    ana.histograms.addHistogram("mu_ptcorr_fwd"            ,  {0., 20., 25., 30., 35., 50., 150.}, [&]() { return abs(www.lep_pdgId()[fakerates.getFakeLepIndex()]) == 13 and fabs(www.lep_pdgId()[fakerates.getFakeLepIndex()]) >=1.6 ? fakerates.getPtCorr() : 0; });
    ana.histograms.addHistogram("nj"                       ,  7   , 0.      , 7.     , [&]() { return www.nj()                                                                     ; });
    ana.histograms.addHistogram("nj30"                     ,  7   , 0.      , 7.     , [&]() { return www.nj30()                                                                   ; });
    ana.histograms.addHistogram("nb"                       ,  5   , 0.      , 5.     , [&]() { return www.nb()                                                                     ; });
    ana.histograms.addHistogram("MTmin"                    ,  180 , 0.      , 300.   , [&]() { return www.MTmin()                                                                  ; });
    ana.histograms.addHistogram("MTmax"                    ,  180 , 0.      , 300.   , [&]() { return www.MTmax()                                                                  ; });
    ana.histograms.addHistogram("MTmax3L"                  ,  180 , 0.      , 300.   , [&]() { return www.MTmax3L()                                                                ; });
    ana.histograms.addHistogram("MT3rd"                    ,  180 , 0.      , 300.   , [&]() { return www.MT3rd()                                                                  ; });
    ana.histograms.addHistogram("dEtall"                   ,  180 , 0.      , 5.     , [&]() { return www.lep_eta().size() > 1 ? fabs(www.lep_eta()[0] - www.lep_eta()[1]) : -999  ; });
    ana.histograms.addHistogram("dRll"                     ,  180 , 0.      , 8.     , [&]() { return www.lep_p4().size() > 1 ? RooUtil::Calc::DeltaR(www.lep_p4()[0], www.lep_p4()[1]) : -999  ; });
    ana.histograms.addHistogram("dPhill"                   ,  180 , 0.      , 3.1416 , [&]() { return www.lep_p4().size() > 1 ? fabs(RooUtil::Calc::DeltaPhi(www.lep_p4()[0], www.lep_p4()[1])) : -999  ; });
    ana.histograms.addHistogram("JetCentrality"            ,  180 , 0.      , 10.    , [&]()
            {
                if (www.lep_eta().size() > 1 and www.jets_p4().size() > 0)
                {
                    float detall = fabs(www.lep_eta()[0] - www.lep_eta()[1]);
                    float avgeta = (www.lep_eta()[0] + www.lep_eta()[1]) / 2.;
                    float cen = fabs(www.jets_p4()[0].eta() - avgeta) / (detall / 2.);
                    return cen;
                }
                else
                {
                    return float(-999);
                }
            });

    ana.histograms.addHistogram("MljMin" , 180 , 0. , 300. , [&]()
            {
                if (not (www.lep_p4().size() > 0 and www.jets_p4().size() > 0))
                    return float(-999);
                float mlj0 = (www.lep_p4()[0] + www.jets_p4()[0]).mass();
                float mlj1 = (www.lep_p4()[1] + www.jets_p4()[0]).mass();
                return mlj0 < mlj1 ? mlj0 : mlj1;
            });

    ana.histograms.addHistogram("MljMax" , 180 , 0. , 600. , [&]()
            {
                if (not (www.lep_p4().size() > 1 and www.jets_p4().size() > 0))
                    return float(-999);
                float mlj0 = (www.lep_p4()[0] + www.jets_p4()[0]).mass();
                float mlj1 = (www.lep_p4()[1] + www.jets_p4()[0]).mass();
                return mlj0 < mlj1 ? mlj1 : mlj0;
            });

    ana.histograms.addHistogram("DRljMin" , 180 , 0. , 6.   , [&]()
            {
                if (not (www.lep_p4().size() > 0 and www.jets_p4().size() > 0))
                    return float(-999);
                float drlj0 = RooUtil::Calc::DeltaR(www.lep_p4()[0], www.jets_p4()[0]);
                float drlj1 = RooUtil::Calc::DeltaR(www.lep_p4()[1], www.jets_p4()[0]);
                return drlj0 < drlj1 ? drlj0 : drlj1;
            });

    ana.histograms.addHistogram("DRljMax" , 180 , 0. , 6.   , [&]()
            {
                if (not (www.lep_p4().size() > 0 and www.jets_p4().size() > 0))
                    return float(-999);
                float drlj0 = RooUtil::Calc::DeltaR(www.lep_p4()[0], www.jets_p4()[0]);
                float drlj1 = RooUtil::Calc::DeltaR(www.lep_p4()[1], www.jets_p4()[0]);
                return drlj0 < drlj1 ? drlj1 : drlj0;
            });

    ana.histograms.addHistogram("Ptllj" , 180 , 0. , 300. , [&]()
            {
                if (not (www.lep_p4().size() > 1 and www.jets_p4().size() > 0))
                    return float(-999);
                return (www.lep_p4()[0] + www.lep_p4()[1] + www.jets_p4()[0]).pt();
            });

    ana.histograms.addHistogram("Mllj" , 180 , 0. , 600. , [&]()
            {
                if (not (www.lep_p4().size() > 1 and www.jets_p4().size() > 0))
                    return float(-999);
                return (www.lep_p4()[0] + www.lep_p4()[1] + www.jets_p4()[0]).mass();
            });

    ana.histograms.addHistogram("MTj" , 180 , 0. , 300., [&]()
            {
                if (not (www.jets_p4().size() > 0))
                    return float(-999);
                float phi1 = www.jets_p4()[0].Phi();
                float phi2 = www.met_phi();
                float Et1  = www.jets_p4()[0].Et();
                float Et2  = www.met_pt();
                return float(sqrt(2*Et1*Et2*(1.0 - cos(phi1-phi2))));
            });

    // One bin histogram to hold the total yield at this cut stage
    ana.histograms.addHistogram("yield"                    ,  1   , 0.      , 1      , [&]() { return 0                                                                            ; });

    // Because RooUtil::Histograms uses lambda you can compute much more complicated variables on the fly your need
    // Let's define a complex histogram. (This is a variable Yifan is looking into.)
    // This is going to plot a new variable where it plots the minimum DR of the two opposite sign leptons.
    // This is the variable that WH->WW analysis in HIG uses to fit to extract signal.
    ana.histograms.addHistogram("minDRllOS", 180, 0., 4.,
                [&]()
                {

                    // "www" objects contain lepton 4-vectors and pdgID
                    std::vector<LV> lep_p4 = www.lep_p4();
                    std::vector<int> lep_pdgId = www.lep_pdgId();

                    // Loop over and for each opposite sign pair compute DR and choose the smallest
                    float minDR = 999;
                    bool os_pair_found = false;
                    for (unsigned int ii = 0; ii < lep_pdgId.size(); ++ii)
                    {
                        for (unsigned int jj = ii + 1; jj < lep_pdgId.size(); ++jj)
                        {
                            if (lep_pdgId[ii] * lep_pdgId[jj] < 0) // If opposite sign lepton
                            {
                                os_pair_found = true;
                                float thisDR = RooUtil::Calc::DeltaR(lep_p4[ii], lep_p4[jj]);
                                if (thisDR < minDR)
                                {
                                    minDR = thisDR;
                                }
                            }
                        }
                    }

                    if (not os_pair_found) // If same-sign event it will not find anything, then set to -999
                        minDR = -999;

                    return minDR;
                }
            );

    // Another new variable where we compute max(Mlj) for the lepton with minimum dphi
    ana.histograms.addHistogram("maxMljminDPhi", 180, 0., 300.,
                [&]()
                {

                    float result = -999;
                    // If less than two leptons don't even bother
                    if (www.lep_p4().size() < 2)
                        return result;

                    // "www" objects contain lepton 4-vectors and pdgID
                    std::vector<LV> lep_p4 = www.lep_p4();
                    std::vector<LV> jets_p4 = www.jets_p4();

                    // For the jet selected as W-jets (minDR of central >30 GeV jets)
                    float minDR = 999;
                    int ii_selected = -1;
                    int jj_selected = -1;
                    for (unsigned int ii = 0; ii < jets_p4.size(); ++ii)
                    {
                        for (unsigned int jj = ii + 1; jj < jets_p4.size(); ++jj)
                        {

                            if (jets_p4[ii].pt() > 30. and jets_p4[jj].pt() > 30. and abs(jets_p4[ii].eta()) < 2.5 and abs(jets_p4[jj].eta()) < 2.5) // If pt condition met
                            {
                                float thisDR = RooUtil::Calc::DeltaR(jets_p4[ii], jets_p4[jj]);
                                if (thisDR < minDR)
                                {
                                    minDR = thisDR;
                                    ii_selected = ii;
                                    jj_selected = jj;
                                }
                            }
                        }
                    }

                    // If no two jets found then don't bother
                    if (ii_selected < 0 or jj_selected < 0)
                        return result;

                    // Now tag the lepton with min-dphi to the tagged boson
                    LV Wboson = jets_p4[ii_selected] + jets_p4[jj_selected];
                    float dphi0 = fabs(RooUtil::Calc::DeltaPhi(lep_p4[0], Wboson));
                    float dphi1 = fabs(RooUtil::Calc::DeltaPhi(lep_p4[1], Wboson));

                    LV lep = dphi0 < dphi1 ? lep_p4[0] : lep_p4[1];

                    float mlj0 = (lep + jets_p4[ii_selected]).mass();
                    float mlj1 = (lep + jets_p4[jj_selected]).mass();

                    result = mlj0 > mlj1 ? mlj0 : mlj1;

                    return result;

                }
            );

    // Another new variable where we compute min(Mlj) for the lepton with minimum DR
    ana.histograms.addHistogram("maxMljminDR", 180, 0., 300.,
                [&]()
                {

                    float result = -999;
                    // If less than two leptons don't even bother
                    if (www.lep_p4().size() < 2)
                        return result;

                    // "www" objects contain lepton 4-vectors and pdgID
                    std::vector<LV> lep_p4 = www.lep_p4();
                    std::vector<LV> jets_p4 = www.jets_p4();

                    // For the jet selected as W-jets (minDR of central >30 GeV jets)
                    float minDR = 999;
                    int ii_selected = -1;
                    int jj_selected = -1;
                    for (unsigned int ii = 0; ii < jets_p4.size(); ++ii)
                    {
                        for (unsigned int jj = ii + 1; jj < jets_p4.size(); ++jj)
                        {

                            if (jets_p4[ii].pt() > 30. and jets_p4[jj].pt() > 30. and abs(jets_p4[ii].eta()) < 2.5 and abs(jets_p4[jj].eta()) < 2.5) // If pt condition met
                            {
                                float thisDR = RooUtil::Calc::DeltaR(jets_p4[ii], jets_p4[jj]);
                                if (thisDR < minDR)
                                {
                                    minDR = thisDR;
                                    ii_selected = ii;
                                    jj_selected = jj;
                                }
                            }
                        }
                    }

                    // If no two jets found then don't bother
                    if (ii_selected < 0 or jj_selected < 0)
                        return result;

                    // Now tag the lepton with min-dphi to the tagged boson
                    LV Wboson = jets_p4[ii_selected] + jets_p4[jj_selected];
                    float dphi0 = fabs(RooUtil::Calc::DeltaR(lep_p4[0], Wboson));
                    float dphi1 = fabs(RooUtil::Calc::DeltaR(lep_p4[1], Wboson));

                    LV lep = dphi0 < dphi1 ? lep_p4[0] : lep_p4[1];

                    float mlj0 = (lep + jets_p4[ii_selected]).mass();
                    float mlj1 = (lep + jets_p4[jj_selected]).mass();

                    result = mlj0 > mlj1 ? mlj0 : mlj1;

                    return result;

                }
            );

    ana.histograms.addVecHistogram("el_photon_pt" , 180 , 0. , 250 , [&]()
            {
                std::vector<float> rtn;
                for (unsigned ilep = 0; ilep < www.lep_pdgId().size(); ++ilep)
                    if (abs(www.lep_pdgId()[ilep]) == 11 and www.lep_motherIdSS()[ilep] == -3)
                        rtn.push_back(www.lep_pt()[ilep]);
                return rtn;
            });

    ana.histograms.addVecHistogram("el_MVA" , 180 , 5 , 8 , [&]()
            {
                std::vector<float> rtn;
                for (unsigned ilep = 0; ilep < www.lep_pdgId().size(); ++ilep)
                    if (abs(www.lep_pdgId()[ilep]) == 11 and www.lep_motherIdSS()[ilep] == -3)
                        rtn.push_back(www.lep_MVA()[ilep]);
                return rtn;
            });
    ana.histograms.addVecHistogram("el_MVA_2017" , 180 , 0.95 , 1 , [&]()
            {
                std::vector<float> rtn;
                for (unsigned ilep = 0; ilep < www.lep_pdgId().size(); ++ilep)
                    // if (abs(www.lep_pdgId()[ilep]) == 11 and www.lep_motherIdSS()[ilep] == -3)
                    //     rtn.push_back(www.lep_MVA()[ilep]);
                    if (abs(www.lep_pdgId()[ilep]) == 11)
                        rtn.push_back(www.lep_MVA()[ilep]);
                return rtn;
            });
    //ana.histograms.addVecHistogram("jets_csv", 180 , 0. , 10  , [&]() { return www.jets_csv(); });
    ana.histograms.addHistogram("jets_btag_score1", 100, 0. , 1   , [&]()
	    {
	      float max_btag_score = 0;
	      for (unsigned int ijet = 0; ijet < www.jets_btag_score().size(); ijet++)
		{
		  if (www.jets_btag_score()[ijet] > max_btag_score)
		    max_btag_score = www.jets_btag_score()[ijet];
		}
	      return max_btag_score;
	    });

    ana.histograms.addHistogram("Mbb", 10, 0. , 300 , [&]()
	    {
	      int leading_btag_index     = -1;
	      int sub_leading_btag_index = -1;
	      float first_btag_score     =  0;
	      float second_btag_score    =  0;
	      float current_btag_score; // for use in loop
	      
	      for (unsigned int ijet = 0; ijet < www.jets_btag_score().size(); ijet++)
		{
		  current_btag_score = www.jets_btag_score()[ijet];
		  if (current_btag_score > second_btag_score && current_btag_score > first_btag_score)
		    {
		      second_btag_score = first_btag_score;
		      sub_leading_btag_index = leading_btag_index;
		      first_btag_score = current_btag_score;
		      leading_btag_index = ijet;
		    }
		  else if (current_btag_score > second_btag_score && current_btag_score <= first_btag_score)
		    {
		      second_btag_score = current_btag_score;
		      sub_leading_btag_index = ijet;
		    }
		  else if (current_btag_score == second_btag_score && current_btag_score < first_btag_score)
		    {
		      if (www.jets_p4()[ijet].pt() > www.jets_p4()[sub_leading_btag_index].pt())
			{
			  second_btag_score = current_btag_score;
			  sub_leading_btag_index = ijet;
			}
		      continue;
		    }
		  else if (current_btag_score == second_btag_score && current_btag_score == first_btag_score)
		    {
		      if (www.jets_p4()[ijet].pt() > www.jets_p4()[sub_leading_btag_index].pt() || www.jets_p4()[ijet].pt() > www.jets_p4()[leading_btag_index].pt())
			{
			  if (www.jets_p4()[sub_leading_btag_index].pt() > www.jets_p4()[leading_btag_index].pt())
			    leading_btag_index = ijet;
			  else
			    sub_leading_btag_index = ijet;
	      
			}
		      continue;
		    }
		  else
		    continue;
		}

	      float Mbb = www.jets_p4()[leading_btag_index].mass() + www.jets_p4()[sub_leading_btag_index].mass();
	      if (input.is_data && Mbb > 90)
		std::cout << Mbb << endl;
	      return Mbb;
	    }); 

    ana.histograms.addHistogram("bjet_pt1", 180, 0., 300, [&]()
	    {
	      int leading_btag_index;
	      int sub_leading_btag_index;
	      Lambdas::GetBJetsIndex(leading_btag_index, sub_leading_btag_index);

	      return www.jets_p4()[leading_btag_index].pt();
	    });

    ana.histograms.addHistogram("bjet_pt2", 180, 0., 300, [&]()
	    {
	      int leading_btag_index;
	      int sub_leading_btag_index;
	      Lambdas::GetBJetsIndex(leading_btag_index, sub_leading_btag_index);

	      return www.jets_p4()[sub_leading_btag_index].pt();
	    });

    ana.histograms.addHistogram("DRbb", 20, 0, 1, [&]()
	    {
	      int leading_btag_index;
	      int sub_leading_btag_index;
	      Lambdas::GetBJetsIndex(leading_btag_index, sub_leading_btag_index);
	      
	      float deltaPhi = www.jets_p4()[leading_btag_index].phi() - www.jets_p4()[sub_leading_btag_index].phi();
	      float deltaEta = www.jets_p4()[leading_btag_index].eta() - www.jets_p4()[sub_leading_btag_index].eta();

	      float deltaR   = sqrt(pow(deltaEta, 2.0) + pow(deltaPhi, 2.0));
	      return deltaR;
	    });
	
    ana.histograms.addHistogram("Mjets", 10 , 0. , 300 , [&]() 
	    {
	      float mass = 0;
	      int amount_of_jets = www.jets_p4().size();
	      for (int jet = 0; jet < amount_of_jets; jet++)
		{
		  mass += www.jets_p4()[jet].mass();
		}
	      return mass;
	    });

//********************************************************************************
//
// 4. Creating the analysis using RooUtil::Cutflow class
//
//********************************************************************************

    // Set the cutflow object output file
    ana.cutflow.setTFile(ana.output_tfile);

    //_______________________________________________________________________________
    //
    // Create cut tree structure
    //_______________________________________________________________________________

    // NOTE: All of the functions that are being called under namespace "Lambdas" are defined in lambdas.cc

    // Event Weights
    // At this stage of the cut, we're passing every event and just setting the event weights
    ana.cutflow.addCut("CutWeight", UNITY,              Lambdas::EventWeight);
    //                              ^^^^^               ^^^^^^^^^^^^^^^^^^^^
    //                           Pass all events        apply event weights (see lambdas.cc)

    // Preliminary selections
    ana.cutflow.addCutToLastActiveCut("CutPreliminary", Lambdas::PreliminarySelection, UNITY);

    // Trigger selection
    ana.cutflow.addCutToLastActiveCut("CutTrigger", Lambdas::TriggerSelection, Lambdas::TriggerScaleFactor);

    //===================
    // N lep requirements
    //===================
    // Now adding various lepton channel categorizations based on number of veto/loose/tight leptons
    // The lepton counters are "nVlep", "nLlep", "nTlep" for (veto, loose, tight)
    // Depending on the input.baby_type the loose may have different pt thresholds (cf. https://github.com/cmstas/VVVBabyMaker/blob/866347c30f4f880031e67534563e5e5e379b98cb/dilepbabymaker/ScanChain_v2.cc#L2375)
    // So it is safer to always require proper pt cuts on each leptons, even if it may be redundant

    // Dilep selection for signal region plots
    ana.cutflow.getCut("CutTrigger"); // Retrieve the CutTrigger and add CutSRDilep to the CutTrigger node
    ana.cutflow.addCutToLastActiveCut("CutSRDilep", Lambdas::CutSRDilep, Lambdas::LeptonScaleFactor);

    // Trilep selection for signal region plots
    ana.cutflow.getCut("CutTrigger"); // Retrieve the CutTrigger and add CutSRTrilep to the CutTrigger node
    ana.cutflow.addCutToLastActiveCut("CutSRTrilep", Lambdas::CutSRTrilep, Lambdas::LeptonScaleFactor);

    // Three lepton selection for lost-lepton control regions
    ana.cutflow.getCut("CutTrigger"); // Retrieve the CutTrigger and add CutWZCRTrilep to the CutTrigger node
    ana.cutflow.addCutToLastActiveCut("CutWZCRTrilep", Lambdas::CutCRTrilep, Lambdas::LeptonScaleFactor);

    // Dilep selection for application region plots
    ana.cutflow.getCut("CutTrigger"); // Retrieve the CutTrigger and add CutARDilep to the CutTrigger node
    ana.cutflow.addCutToLastActiveCut("CutARDilep", Lambdas::CutARDilep, UNITY); // No lepton scale factors applied for the application region. (no proper lepton scale factors for loose leptons derived)

    // Trilep selection for signal region plots
    ana.cutflow.getCut("CutTrigger"); // Retrieve the CutTrigger and add CutARDilep to the CutTrigger node
    ana.cutflow.addCutToLastActiveCut("CutARTrilep", Lambdas::CutARTrilep, UNITY); // No lepton scale factors applied for the application region. (no proper lepton scale factors for loose leptons derived)
    
    // Custom WWjjH signal region cuts
    ana.cutflow.getCut("CutTrigger");
    ana.cutflow.addCutToLastActiveCut("CutSRSSDilepDijets", Lambdas::CutSRDilep, UNITY); // Ensures that the cut only has 2 leptons


    auto nominal_analysis = [&]()
    {
        //************************************************************************************************************************************************************************************************
        //
        //
        // SIGNAL REGION CUTS
        //
        //
        //************************************************************************************************************************************************************************************************

        // WWjjH electron-electron cut
        ana.cutflow.getCut("CutSRSSDilepDijets");
	ana.cutflow.addCutToLastActiveCut("SRSSeeDD"           , Lambdas::isSRSSeeChannel                                     , UNITY);
     // ana.cutflow.addCutToLastActiveCut("SRSSeeZVetoDD"      , Lambdas::ZVetoSS                                             , UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSeePreSelDD"     , Lambdas::SSPreSelection  (Variation::JES, Variation::Nominal, 1), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("SRSSeeNj2DD"        , Lambdas::TwoCenJet30     (Variation::JES, Variation::Nominal), UNITY);
     // ana.cutflow.addCutToLastActiveCut("SRSSeeKinSelDD"     , Lambdas::SRSSeeSelection (Variation::JES, Variation::Nominal), UNITY);
     // ana.cutflow.addCutToLastActiveCut("SRSSeeLowDetajjDD"  , Lambdas::LowDEtajj       (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSeeHighMjjDD"    , Lambdas::HighMjj         (Variation::JES, Variation::Nominal), UNITY);
     // ana.cutflow.addCutToLastActiveCut("SRSSeeMjjInDD"      , Lambdas::MjjIn           (Variation::JES, Variation::Nominal), UNITY);
	ana.cutflow.addCutToLastActiveCut("SRSSeeNb2DD"        , Lambdas::TwoBJets        (Variation::JES, Variation::Nominal), UNITY);
	ana.cutflow.addCutToLastActiveCut("SRSSeelljjbbFullDD" , UNITY                                                        , UNITY);

	// WWjjH electron-muon cut
        ana.cutflow.getCut("CutSRSSDilepDijets");
        ana.cutflow.addCutToLastActiveCut("SRSSemDD"           , Lambdas::isSRSSemChannel                                     , UNITY);
     // ana.cutflow.addCutToLastActiveCut("SRSSemZVetoDD"      , Lambdas::ZVetoSS                                             , UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSemPreSelDD"     , Lambdas::SSPreSelection  (Variation::JES, Variation::Nominal, 1), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("SRSSemNj2DD"        , Lambdas::TwoCenJet30     (Variation::JES, Variation::Nominal), UNITY);
     // ana.cutflow.addCutToLastActiveCut("SRSSemKinSelDD"     , Lambdas::SRSSeeSelection (Variation::JES, Variation::Nominal), UNITY);  
     // ana.cutflow.addCutToLastActiveCut("SRSSemLowDetajjDD"  , Lambdas::LowDEtajj       (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSemHighMjjDD"    , Lambdas::HighMjj         (Variation::JES, Variation::Nominal), UNITY);
     // ana.cutflow.addCutToLastActiveCut("SRSSemMjjInDD"      , Lambdas::MjjIn           (Variation::JES, Variation::Nominal), UNITY); 
        ana.cutflow.addCutToLastActiveCut("SRSSemNb2DD"        , Lambdas::TwoBJets        (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSemlljjbbFullDD" , UNITY                                                        , UNITY);        
	
	// WWjjH muon-muon cut
	ana.cutflow.getCut("CutSRSSDilepDijets");
        ana.cutflow.addCutToLastActiveCut("SRSSmmDD"           , Lambdas::isSRSSmmChannel                                     , UNITY);
     // ana.cutflow.addCutToLastActiveCut("SRSSmmZVetoDD"      , Lambdas::ZVetoSS                                             , UNITY); 
        ana.cutflow.addCutToLastActiveCut("SRSSmmPreSelDD"     , Lambdas::SSPreSelection  (Variation::JES, Variation::Nominal, 1), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("SRSSmmNj2DD"        , Lambdas::TwoCenJet30     (Variation::JES, Variation::Nominal), UNITY);
     // ana.cutflow.addCutToLastActiveCut("SRSSmmKinSelDD"     , Lambdas::SRSSeeSelection (Variation::JES, Variation::Nominal), UNITY); 
     // ana.cutflow.addCutToLastActiveCut("SRSSmmLowDetajjDD"  , Lambdas::LowDEtajj       (Variation::JES, Variation::Nominal), UNITY);   
        ana.cutflow.addCutToLastActiveCut("SRSSmmHighMjjDD"    , Lambdas::HighMjj         (Variation::JES, Variation::Nominal), UNITY);
     // ana.cutflow.addCutToLastActiveCut("SRSSmmMjjInDD"      , Lambdas::MjjIn           (Variation::JES, Variation::Nominal), UNITY);   
        ana.cutflow.addCutToLastActiveCut("SRSSmmNb2DD"        , Lambdas::TwoBJets        (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSmmlljjbbFullDD" , UNITY                                                        , UNITY);

//=========================================================================================================================================================================|
        ana.cutflow.getCut("CutSRDilep");
        ana.cutflow.addCutToLastActiveCut("SRSSee"           , Lambdas::isSRSSeeChannel                                     , UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSeeZVeto"      , Lambdas::ZVetoSS                                             , UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSeePreSel"     , Lambdas::SSPreSelection  (Variation::JES, Variation::Nominal), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("SRSSeeNj2"        , Lambdas::TwoCenJet30     (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSeeKinSel"     , Lambdas::SRSSeeSelection (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSeeLowDetajj"  , Lambdas::LowDEtajj       (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSeeLowMjj"     , Lambdas::LowMjj          (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSeeMjjIn"      , Lambdas::MjjIn           (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSeeFull"       , UNITY                                                        , UNITY); // Adding one more node to have a clean name of "<Region>Full"
        ana.cutflow.getCut("SRSSeeLowMjj");
        ana.cutflow.addCutToLastActiveCut("SRSSSideeeMjjOut" , Lambdas::MjjOut          (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSSideeeFull"   , UNITY                                                        , UNITY); // Adding one more node to have a clean name of "<Region>Full"

        ana.cutflow.getCut("CutSRDilep");
        ana.cutflow.addCutToLastActiveCut("SRSSem"           , Lambdas::isSRSSemChannel                                     , UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSemPreSel"     , Lambdas::SSPreSelection  (Variation::JES, Variation::Nominal), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("SRSSemNj2"        , Lambdas::TwoCenJet30     (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSemKinSel"     , Lambdas::SRSSemSelection (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSemLowDetajj"  , Lambdas::LowDEtajj       (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSemLowMjj"     , Lambdas::LowMjj          (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSemMjjIn"      , Lambdas::MjjIn           (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSemFull"       , UNITY                                                        , UNITY); // Adding one more node to have a clean name of "<Region>Full"
        ana.cutflow.getCut("SRSSemLowMjj");
        ana.cutflow.addCutToLastActiveCut("SRSSSideemMjjOut" , Lambdas::MjjOut          (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSSideemFull"   , UNITY                                                        , UNITY); // Adding one more node to have a clean name of "<Region>Full"

        ana.cutflow.getCut("CutSRDilep");
        ana.cutflow.addCutToLastActiveCut("SRSSmm"           , Lambdas::isSRSSmmChannel                                     , UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSmmPreSel"     , Lambdas::SSPreSelection  (Variation::JES, Variation::Nominal), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("SRSSmmNj2"        , Lambdas::TwoCenJet30     (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSmmKinSel"     , Lambdas::SRSSmmSelection (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSmmLowDetajj"  , Lambdas::LowDEtajj       (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSmmLowMjj"     , Lambdas::LowMjj          (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSmmMjjIn"      , Lambdas::MjjIn           (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSmmFull"       , UNITY                                                        , UNITY); // Adding one more node to have a clean name of "<Region>Full"
        ana.cutflow.getCut("SRSSmmLowMjj");
        ana.cutflow.addCutToLastActiveCut("SRSSSidemmMET"    , Lambdas::SRSSSidemmSel   (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSSidemmMjjOut" , Lambdas::MjjOut          (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SRSSSidemmFull"   , UNITY                                                        , UNITY); // Adding one more node to have a clean name of "<Region>Full"

        ana.cutflow.getCut("CutSRTrilep");
        ana.cutflow.addCutToLastActiveCut("SR0SFOS"          , Lambdas::is0SFOS                                             , UNITY);
        ana.cutflow.addCutToLastActiveCut("SR0SFOSPreSel"    , Lambdas::ThreeLepPresel  (Variation::JES, Variation::Nominal), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("SR0SFOSDYVeto"    , Lambdas::DYVetoes0SFOS                                       , UNITY);
        ana.cutflow.addCutToLastActiveCut("SR0SFOSKinSel"    , Lambdas::KinSel0SFOS     (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SR0SFOSFull"      , UNITY                                                        , UNITY);

        ana.cutflow.getCut("CutSRTrilep");
        ana.cutflow.addCutToLastActiveCut("SR1SFOS"          , Lambdas::is1SFOS                                             , UNITY);
        ana.cutflow.addCutToLastActiveCut("SR1SFOSPreSel"    , Lambdas::ThreeLepPresel  (Variation::JES, Variation::Nominal), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("SR1SFOSDYVeto"    , Lambdas::DYVetoes1SFOS                                       , UNITY);
        ana.cutflow.addCutToLastActiveCut("SR1SFOSZVeto"     , Lambdas::ZVeto3L                                             , UNITY);
        ana.cutflow.addCutToLastActiveCut("SR1SFOSKinSel"    , Lambdas::KinSel1SFOS     (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SR1SFOSFull"      , UNITY                                                        , UNITY);

        ana.cutflow.getCut("CutSRTrilep");
        ana.cutflow.addCutToLastActiveCut("SR2SFOS"          , Lambdas::is2SFOS                                             , UNITY);
        ana.cutflow.addCutToLastActiveCut("SR2SFOSPreSel"    , Lambdas::ThreeLepPresel  (Variation::JES, Variation::Nominal), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("SR2SFOSDYVeto"    , Lambdas::DYVetoes2SFOS                                       , UNITY);
        ana.cutflow.addCutToLastActiveCut("SR2SFOSZVeto"     , Lambdas::ZVeto3L                                             , UNITY);
        ana.cutflow.addCutToLastActiveCut("SR2SFOSKinSel"    , Lambdas::KinSel2SFOS     (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("SR2SFOSFull"      , UNITY                                                        , UNITY);

        //************************************************************************************************************************************************************************************************
        //
        //
        // LOST LEPTON CONTROL REGIONS
        //
        //
        //************************************************************************************************************************************************************************************************

        ana.cutflow.getCut("CutWZCRTrilep");
        ana.cutflow.addCutToLastActiveCut("WZCRSSee"           , Lambdas::isSRSSeeChannel                                      , UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCRSSeeZVeto"      , Lambdas::ZVetoSS                                              , UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCRSSeePreSel"     , Lambdas::SSPreSelection   (Variation::JES, Variation::Nominal), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("WZCRSSeeNj2"        , Lambdas::TwoCenJet30      (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCRSSeeKinSel"     , Lambdas::SRSSeeSelection  (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCRSSeeLowDetajj"  , Lambdas::LowDEtajj        (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCRSSeeLowMjj"     , Lambdas::LowMjj           (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCRSSeeFull"       , Lambdas::HasZ_SS                                              , UNITY);

        ana.cutflow.getCut("CutWZCRTrilep");
        ana.cutflow.addCutToLastActiveCut("WZCRSSem"           , Lambdas::isSRSSemChannel                                      , UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCRSSemPreSel"     , Lambdas::SSPreSelection   (Variation::JES, Variation::Nominal), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("WZCRSSemNj2"        , Lambdas::TwoCenJet30      (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCRSSemKinSel"     , Lambdas::SRSSemSelection  (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCRSSemLowDetajj"  , Lambdas::LowDEtajj        (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCRSSemLowMjj"     , Lambdas::LowMjj           (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCRSSemFull"       , Lambdas::HasZ_SS                                              , UNITY);

        ana.cutflow.getCut("CutWZCRTrilep");
        ana.cutflow.addCutToLastActiveCut("WZCRSSmm"           , Lambdas::isSRSSmmChannel                                      , UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCRSSmmPreSel"     , Lambdas::SSPreSelection   (Variation::JES, Variation::Nominal), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("WZCRSSmmNj2"        , Lambdas::TwoCenJet30      (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCRSSmmKinSel"     , Lambdas::SRSSmmSelection  (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCRSSmmLowDetajj"  , Lambdas::LowDEtajj        (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCRSSmmLowMjj"     , Lambdas::LowMjj           (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCRSSmmFull"       , Lambdas::HasZ_SS                                              , UNITY);

        ana.cutflow.getCut("CutWZCRTrilep");
        ana.cutflow.addCutToLastActiveCut("WZCR1SFOS"          , Lambdas::is1SFOS                                              , UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCR1SFOSPreSel"    , Lambdas::ThreeLepPresel   (Variation::JES, Variation::Nominal), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("WZCR1SFOSDYVeto"    , Lambdas::DYVetoes1SFOS                                        , UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCR1SFOSKinSel"    , Lambdas::KinSel1SFOS      (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCR1SFOSFull"      , Lambdas::HasZ_3L                                              , UNITY);

        ana.cutflow.getCut("CutWZCRTrilep");
        ana.cutflow.addCutToLastActiveCut("WZCR2SFOS"          , Lambdas::is2SFOS                                              , UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCR2SFOSPreSel"    , Lambdas::ThreeLepPresel   (Variation::JES, Variation::Nominal), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("WZCR2SFOSDYVeto"    , Lambdas::DYVetoes2SFOS                                        , UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCR2SFOSKinSel"    , Lambdas::KinSel2SFOS      (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("WZCR2SFOSFull"      , Lambdas::HasZ_3L                                              , UNITY);

        //************************************************************************************************************************************************************************************************
        //
        //
        // APPLICATION REGIONS
        //
        //
        //************************************************************************************************************************************************************************************************

        ana.cutflow.getCut("CutARDilep");
        ana.cutflow.addCutToLastActiveCut("ARSSee"           , Lambdas::isSRSSeeChannel                                     , UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSeeZVeto"      , Lambdas::ZVetoSS                                             , UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSeePreSel"     , Lambdas::SSPreSelection  (Variation::JES, Variation::Nominal), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("ARSSeeNj2"        , Lambdas::TwoCenJet30     (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSeeKinSel"     , Lambdas::SRSSeeSelection (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSeeLowDetajj"  , Lambdas::LowDEtajj       (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSeeLowMjj"     , Lambdas::LowMjj          (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSeeMjjIn"      , Lambdas::MjjIn           (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSeeFull"       , UNITY                                                        , UNITY); // Adding one more node to have a clean name of "<Region>Full"
        ana.cutflow.getCut("ARSSeeLowMjj");
        ana.cutflow.addCutToLastActiveCut("ARSSSideeeMjjOut" , Lambdas::MjjOut          (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSSideeeFull"   , UNITY                                                        , UNITY); // Adding one more node to have a clean name of "<Region>Full"

        ana.cutflow.getCut("CutARDilep");
        ana.cutflow.addCutToLastActiveCut("ARSSem"           , Lambdas::isSRSSemChannel                                     , UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSemPreSel"     , Lambdas::SSPreSelection  (Variation::JES, Variation::Nominal), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("ARSSemNj2"        , Lambdas::TwoCenJet30     (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSemKinSel"     , Lambdas::SRSSemSelection (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSemLowDetajj"  , Lambdas::LowDEtajj       (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSemLowMjj"     , Lambdas::LowMjj          (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSemMjjIn"      , Lambdas::MjjIn           (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSemFull"       , UNITY                                                        , UNITY); // Adding one more node to have a clean name of "<Region>Full"
        ana.cutflow.getCut("ARSSemLowMjj");
        ana.cutflow.addCutToLastActiveCut("ARSSSideemMjjOut" , Lambdas::MjjOut          (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSSideemFull"   , UNITY                                                        , UNITY); // Adding one more node to have a clean name of "<Region>Full"

        ana.cutflow.getCut("CutARDilep");
        ana.cutflow.addCutToLastActiveCut("ARSSmm"           , Lambdas::isSRSSmmChannel                                     , UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSmmPreSel"     , Lambdas::SSPreSelection  (Variation::JES, Variation::Nominal), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("ARSSmmNj2"        , Lambdas::TwoCenJet30     (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSmmKinSel"     , Lambdas::SRSSmmSelection (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSmmLowDetajj"  , Lambdas::LowDEtajj       (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSmmLowMjj"     , Lambdas::LowMjj          (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSmmMjjIn"      , Lambdas::MjjIn           (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSmmFull"       , UNITY                                                        , UNITY); // Adding one more node to have a clean name of "<Region>Full"
        ana.cutflow.getCut("ARSSmmLowMjj");
        ana.cutflow.addCutToLastActiveCut("ARSSSidemmMET"    , Lambdas::SRSSSidemmSel   (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSSidemmMjjOut" , Lambdas::MjjOut          (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("ARSSSidemmFull"   , UNITY                                                        , UNITY); // Adding one more node to have a clean name of "<Region>Full"

        ana.cutflow.getCut("CutARTrilep");
        ana.cutflow.addCutToLastActiveCut("AR0SFOS"          , Lambdas::is0SFOS                                             , UNITY);
        ana.cutflow.addCutToLastActiveCut("AR0SFOSPreSel"    , Lambdas::ThreeLepPresel  (Variation::JES, Variation::Nominal), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("AR0SFOSDYVeto"    , Lambdas::DYVetoes0SFOS                                       , UNITY);
        ana.cutflow.addCutToLastActiveCut("AR0SFOSKinSel"    , Lambdas::KinSel0SFOS     (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("AR0SFOSFull"      , UNITY                                                        , UNITY);

        ana.cutflow.getCut("CutARTrilep");
        ana.cutflow.addCutToLastActiveCut("AR1SFOS"          , Lambdas::is1SFOS                                             , UNITY);
        ana.cutflow.addCutToLastActiveCut("AR1SFOSPreSel"    , Lambdas::ThreeLepPresel  (Variation::JES, Variation::Nominal), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("AR1SFOSDYVeto"    , Lambdas::DYVetoes1SFOS                                       , UNITY);
        ana.cutflow.addCutToLastActiveCut("AR1SFOSZVeto"     , Lambdas::ZVeto3L                                             , UNITY);
        ana.cutflow.addCutToLastActiveCut("AR1SFOSKinSel"    , Lambdas::KinSel1SFOS     (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("AR1SFOSFull"      , UNITY                                                        , UNITY);

        ana.cutflow.getCut("CutARTrilep");
        ana.cutflow.addCutToLastActiveCut("AR2SFOS"          , Lambdas::is2SFOS                                             , UNITY);
        ana.cutflow.addCutToLastActiveCut("AR2SFOSPreSel"    , Lambdas::ThreeLepPresel  (Variation::JES, Variation::Nominal), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("AR2SFOSDYVeto"    , Lambdas::DYVetoes2SFOS                                       , UNITY);
        ana.cutflow.addCutToLastActiveCut("AR2SFOSZVeto"     , Lambdas::ZVeto3L                                             , UNITY);
        ana.cutflow.addCutToLastActiveCut("AR2SFOSKinSel"    , Lambdas::KinSel2SFOS     (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("AR2SFOSFull"      , UNITY                                                        , UNITY);

        //************************************************************************************************************************************************************************************************
        //
        //
        // N-jet 1 bin selection
        //
        //
        //************************************************************************************************************************************************************************************************

        ana.cutflow.getCut("SRSSmmPreSel");
        ana.cutflow.addCutToLastActiveCut("EXSSmmNj1OneJet" , Lambdas::OneCenJet30 (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("EXSSmmNj1DRljMin", Lambdas::Nj1DRljMin  (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("EXSSmmNj1Full"   , UNITY                                                    , UNITY);
        ana.cutflow.getCut("SRSSemPreSel");
        ana.cutflow.addCutToLastActiveCut("EXSSemNj1Channel", Lambdas::isSSem                                          , UNITY);
        ana.cutflow.addCutToLastActiveCut("EXSSemNj1OneJet" , Lambdas::OneCenJet30 (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("EXSSemNj1DRljMin", Lambdas::Nj1DRljMin  (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("EXSSemNj1Full"   , UNITY                                                    , UNITY);
        ana.cutflow.getCut("SRSSemPreSel");
        ana.cutflow.addCutToLastActiveCut("EXSSmeNj1Channel", Lambdas::isSSme                                          , UNITY);
        ana.cutflow.addCutToLastActiveCut("EXSSmeNj1OneJet" , Lambdas::OneCenJet30 (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("EXSSmeNj1DRljMin", Lambdas::Nj1DRljMin  (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("EXSSmeNj1Full"   , UNITY                                                    , UNITY);
        ana.cutflow.getCut("SRSSeePreSel");
        ana.cutflow.addCutToLastActiveCut("EXSSeeNj1OneJet" , Lambdas::OneCenJet30 (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("EXSSeeNj1DRljMin", Lambdas::Nj1DRljMin  (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("EXSSeeNj1Full"   , UNITY                                                    , UNITY);

        ana.cutflow.getCut("WZCRSSeePreSel");
        ana.cutflow.addCutToLastActiveCut("EXCRSSeeNjLeq1"    , Lambdas::LeqOneJet30(Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("EXCRSSeeNj1KinSel" , Lambdas::Nj1CRKinSel(Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("EXCRSSeeNj1Full"   , Lambdas::HasZ_SS                                        , UNITY);

        ana.cutflow.getCut("WZCRSSemPreSel");
        ana.cutflow.addCutToLastActiveCut("EXCRSSemNjChannel" , Lambdas::isSSem                                         , UNITY);
        ana.cutflow.addCutToLastActiveCut("EXCRSSemNjLeq1"    , Lambdas::LeqOneJet30(Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("EXCRSSemNj1KinSel" , Lambdas::Nj1CRKinSel(Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("EXCRSSemNj1Full"   , Lambdas::HasZ_SS                                        , UNITY);

        ana.cutflow.getCut("WZCRSSemPreSel");
        ana.cutflow.addCutToLastActiveCut("EXCRSSmeNjChannel" , Lambdas::isSSme                                         , UNITY);
        ana.cutflow.addCutToLastActiveCut("EXCRSSmeNjLeq1"    , Lambdas::LeqOneJet30(Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("EXCRSSmeNj1KinSel" , Lambdas::Nj1CRKinSel(Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("EXCRSSmeNj1Full"   , Lambdas::HasZ_SS                                        , UNITY);

        ana.cutflow.getCut("WZCRSSmmPreSel");
        ana.cutflow.addCutToLastActiveCut("EXCRSSmmNjLeq1"    , Lambdas::LeqOneJet30(Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("EXCRSSmmNj1KinSel" , Lambdas::Nj1CRKinSel(Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("EXCRSSmmNj1Full"   , Lambdas::HasZ_SS                                        , UNITY);

        //************************************************************************************************************************************************************************************************
        //
        //
        // N-btag control region (mainly for fakes)
        //
        //
        //************************************************************************************************************************************************************************************************

        ana.cutflow.getCut("CutSRDilep");
        ana.cutflow.addCutToLastActiveCut("CRBTagee"           , Lambdas::isSRSSeeChannel                                     , UNITY);
        ana.cutflow.addCutToLastActiveCut("CRBTageeZVeto"      , Lambdas::ZVetoSS                                             , UNITY);
        ana.cutflow.addCutToLastActiveCut("CRBTageePreSel"     , Lambdas::SSPreSelection  (Variation::JES, Variation::Nominal, true), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("CRBTageeNj2"        , Lambdas::TwoCenJet30     (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("CRBTageeKinSel"     , Lambdas::SRSSeeSelection (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("CRBTageeFull"       , UNITY                                                        , UNITY); // Adding one more node to have a clean name of "<Region>Full"

        ana.cutflow.getCut("CutSRDilep");
        ana.cutflow.addCutToLastActiveCut("CRBTagem"           , Lambdas::isSRSSemChannel                                     , UNITY);
        ana.cutflow.addCutToLastActiveCut("CRBTagemPreSel"     , Lambdas::SSPreSelection  (Variation::JES, Variation::Nominal, true), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("CRBTagemNj2"        , Lambdas::TwoCenJet30     (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("CRBTagemKinSel"     , Lambdas::SRSSemSelection (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("CRBTagemFull"       , UNITY                                                        , UNITY); // Adding one more node to have a clean name of "<Region>Full"

        ana.cutflow.getCut("CutSRDilep");
        ana.cutflow.addCutToLastActiveCut("CRBTagmm"           , Lambdas::isSRSSmmChannel                                     , UNITY);
        ana.cutflow.addCutToLastActiveCut("CRBTagmmPreSel"     , Lambdas::SSPreSelection  (Variation::JES, Variation::Nominal, true), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("CRBTagmmNj2"        , Lambdas::TwoCenJet30     (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("CRBTagmmKinSel"     , Lambdas::SRSSmmSelection (Variation::JES, Variation::Nominal), UNITY);
        ana.cutflow.addCutToLastActiveCut("CRBTagmmFull"       , UNITY                                                        , UNITY); // Adding one more node to have a clean name of "<Region>Full"

        ana.cutflow.getCut("CutSRTrilep");
        ana.cutflow.addCutToLastActiveCut("CRBTag0SFOS"          , Lambdas::is0SFOS                                             , UNITY);
        ana.cutflow.addCutToLastActiveCut("CRBTag0SFOSPreSel"    , Lambdas::ThreeLepPresel  (Variation::JES, Variation::Nominal, true), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("CRBTag0SFOSDYVeto"    , Lambdas::DYVetoes0SFOS                                       , UNITY);
        ana.cutflow.addCutToLastActiveCut("CRBTag0SFOSFull"      , UNITY                                                        , UNITY);

        ana.cutflow.getCut("CutSRTrilep");
        ana.cutflow.addCutToLastActiveCut("CRBTag1SFOS"          , Lambdas::is1SFOS                                             , UNITY);
        ana.cutflow.addCutToLastActiveCut("CRBTag1SFOSPreSel"    , Lambdas::ThreeLepPresel  (Variation::JES, Variation::Nominal, true), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("CRBTag1SFOSDYVeto"    , Lambdas::DYVetoes1SFOS                                       , UNITY);
        ana.cutflow.addCutToLastActiveCut("CRBTag1SFOSZVeto"     , Lambdas::ZVeto3L                                             , UNITY);
        ana.cutflow.addCutToLastActiveCut("CRBTag1SFOSFull"      , UNITY                                                        , UNITY);

        ana.cutflow.getCut("CutSRTrilep");
        ana.cutflow.addCutToLastActiveCut("CRBTag2SFOS"          , Lambdas::is2SFOS                                             , UNITY);
        ana.cutflow.addCutToLastActiveCut("CRBTag2SFOSPreSel"    , Lambdas::ThreeLepPresel  (Variation::JES, Variation::Nominal, true), Lambdas::BTagScaleFactor);
        ana.cutflow.addCutToLastActiveCut("CRBTag2SFOSDYVeto"    , Lambdas::DYVetoes2SFOS                                       , UNITY);
        ana.cutflow.addCutToLastActiveCut("CRBTag2SFOSZVeto"     , Lambdas::ZVeto3L                                             , UNITY);
        ana.cutflow.addCutToLastActiveCut("CRBTag2SFOSFull"      , UNITY                                                        , UNITY);

        //************************************************************************************************************************************************************************************************
        //
        //
        // Gamma CR
        //
        //
        //************************************************************************************************************************************************************************************************

        ana.cutflow.getCut("CutSRTrilep");
        ana.cutflow.addCutToLastActiveCut("GammaCR", Lambdas::GammaCR, UNITY);


    };

    // Here I create hook for users to add various cuts and histograms of their choice to make their own studies
    // The separation between the main parts and the user_study helps to not make main analysis code too bloated
    // An example of how to add new cuts are shown in template_user_study.h
    // Copy the file template_user_study.h -> user_study.h after implementing user's additional cutflows
    // Then recompile the code and provide the option -u,--user_study as the option to the ./doAnalysis
    // OR just use process.sh script with option -u which will relay the --user_study option to the ./doAnalysis
    if (ana.do_user_study)
    {

        #if __has_include ("user_study.h")
        #include "user_study.h"
        #endif

    }
    // If --user_study option is not provided, it defaults to main analysis
    else
    {

        nominal_analysis();

    }

//*************************************************************************************************************
//
// 6. Book systematic variations
//
//*************************************************************************************************************

    // The systematics can be booked using various funcitons:
    //
    //   RooUtil::Cutflow::addCutSyst
    //   RooUtil::Cutflow::addWgtSyst
    //
    // There are two types of systematics where one only affects the weighting of each events
    // And another that affects which events passes certain cuts or not
    //
    // 1. Weight variations
    //
    //   The addWgtSyst("NameOfWgtSystUpOrDown", <lambda>);
    //
    //   The lambda is the weight that modifies the event weight (i.e. lepsf_up / lepsf) and not just the up variation weight alone (i.e. not lepsf_up alone)
    //
    // 2. Cut variations
    //
    //   This happens in two steps
    //   First, add a variation to a specific cut that gets affected by it
    //   For example, let's say
    //
    //     - SRSS*Pre, SRSS*Nj2, SRSS*LowDetajj, are affected by it.
    //
    //   then one can call, addCutSyst("JESUp", {"Pre", "Nj2", "LowDetajj", ... });
    //   too let RooUtil::Cutflow object know that there are JESUp variations for these cuts
    //   Later, when printCuts() function is called to print the cut structure user can see the booked systematic variations on the right side of the print out.
    //
    //   Then once these variations are booked to each cut, one defines them later using RooUtil::Cutflow::setCutSyst()
    //
    //   setCutSyst("SRSSmmPre", "JESUp", <lambda for cut>, <lambda for weight>);
    //

    if (ana.do_systematics)
    {
        ana.cutflow.addWgtSyst("LepSFUp"    , Lambdas::LepSFVariation     (Variation::Up   ));
        ana.cutflow.addWgtSyst("LepSFDown"  , Lambdas::LepSFVariation     (Variation::Down ));
        ana.cutflow.addWgtSyst("TrigSFUp"   , Lambdas::TriggerSFVariation (Variation::Up   ));
        ana.cutflow.addWgtSyst("TrigSFDown" , Lambdas::TriggerSFVariation (Variation::Down ));
        ana.cutflow.addWgtSyst("BTagLFUp"   , Lambdas::BTagLFVariation    (Variation::Up   ));
        ana.cutflow.addWgtSyst("BTagLFDown" , Lambdas::BTagLFVariation    (Variation::Down ));
        ana.cutflow.addWgtSyst("BTagHFUp"   , Lambdas::BTagHFVariation    (Variation::Up   ));
        ana.cutflow.addWgtSyst("BTagHFDown" , Lambdas::BTagHFVariation    (Variation::Down ));
        ana.cutflow.addWgtSyst("PileupUp"   , Lambdas::PileupVariation    (Variation::Up   ));
        ana.cutflow.addWgtSyst("PileupDown" , Lambdas::PileupVariation    (Variation::Down ));
        ana.cutflow.addWgtSyst("PDFUp"      , Lambdas::PDFVariation       (Variation::Up   ));
        ana.cutflow.addWgtSyst("PDFDown"    , Lambdas::PDFVariation       (Variation::Down ));
        ana.cutflow.addWgtSyst("QsqUp"      , Lambdas::QsqVariation       (Variation::Up   ));
        ana.cutflow.addWgtSyst("QsqDown"    , Lambdas::QsqVariation       (Variation::Down ));
        ana.cutflow.addWgtSyst("AlphaSUp"   , Lambdas::AlphaSVariation    (Variation::Up   ));
        ana.cutflow.addWgtSyst("AlphaSDown" , Lambdas::AlphaSVariation    (Variation::Down ));

        if (ana.do_fake_estimation)
        {
            ana.cutflow.addWgtSyst("FakeUp"              , Lambdas::FakeFactorVariation(Variation::Full        , Variation::Up  ));
            ana.cutflow.addWgtSyst("FakeDown"            , Lambdas::FakeFactorVariation(Variation::Full        , Variation::Down));
            ana.cutflow.addWgtSyst("FakeRateUp"          , Lambdas::FakeFactorVariation(Variation::Rate        , Variation::Up  ));
            ana.cutflow.addWgtSyst("FakeRateDown"        , Lambdas::FakeFactorVariation(Variation::Rate        , Variation::Down));
            ana.cutflow.addWgtSyst("FakeRateElUp"        , Lambdas::FakeFactorVariation(Variation::RateEl      , Variation::Up  ));
            ana.cutflow.addWgtSyst("FakeRateElDown"      , Lambdas::FakeFactorVariation(Variation::RateEl      , Variation::Down));
            ana.cutflow.addWgtSyst("FakeRateMuUp"        , Lambdas::FakeFactorVariation(Variation::RateMu      , Variation::Up  ));
            ana.cutflow.addWgtSyst("FakeRateMuDown"      , Lambdas::FakeFactorVariation(Variation::RateMu      , Variation::Down));
            ana.cutflow.addWgtSyst("FakeClosureUp"       , Lambdas::FakeFactorVariation(Variation::Closure     , Variation::Up  ));
            ana.cutflow.addWgtSyst("FakeClosureDown"     , Lambdas::FakeFactorVariation(Variation::Closure     , Variation::Down));
            ana.cutflow.addWgtSyst("FakeClosureElUp"     , Lambdas::FakeFactorVariation(Variation::ClosureEl   , Variation::Up  ));
            ana.cutflow.addWgtSyst("FakeClosureElDown"   , Lambdas::FakeFactorVariation(Variation::ClosureEl   , Variation::Down));
            ana.cutflow.addWgtSyst("FakeClosureMuUp"     , Lambdas::FakeFactorVariation(Variation::ClosureMu   , Variation::Up  ));
            ana.cutflow.addWgtSyst("FakeClosureMuDown"   , Lambdas::FakeFactorVariation(Variation::ClosureMu   , Variation::Down));
            ana.cutflow.addWgtSyst("FakeRateSSElUp"      , Lambdas::FakeFactorVariation(Variation::RateSSEl    , Variation::Up  ));
            ana.cutflow.addWgtSyst("FakeRateSSElDown"    , Lambdas::FakeFactorVariation(Variation::RateSSEl    , Variation::Down));
            ana.cutflow.addWgtSyst("FakeRateSSMuUp"      , Lambdas::FakeFactorVariation(Variation::RateSSMu    , Variation::Up  ));
            ana.cutflow.addWgtSyst("FakeRateSSMuDown"    , Lambdas::FakeFactorVariation(Variation::RateSSMu    , Variation::Down));
            ana.cutflow.addWgtSyst("FakeRate3LElUp"      , Lambdas::FakeFactorVariation(Variation::Rate3LEl    , Variation::Up  ));
            ana.cutflow.addWgtSyst("FakeRate3LElDown"    , Lambdas::FakeFactorVariation(Variation::Rate3LEl    , Variation::Down));
            ana.cutflow.addWgtSyst("FakeRate3LMuUp"      , Lambdas::FakeFactorVariation(Variation::Rate3LMu    , Variation::Up  ));
            ana.cutflow.addWgtSyst("FakeRate3LMuDown"    , Lambdas::FakeFactorVariation(Variation::Rate3LMu    , Variation::Down));
            ana.cutflow.addWgtSyst("FakeClosureSSElUp"   , Lambdas::FakeFactorVariation(Variation::ClosureSSEl , Variation::Up  ));
            ana.cutflow.addWgtSyst("FakeClosureSSElDown" , Lambdas::FakeFactorVariation(Variation::ClosureSSEl , Variation::Down));
            ana.cutflow.addWgtSyst("FakeClosureSSMuUp"   , Lambdas::FakeFactorVariation(Variation::ClosureSSMu , Variation::Up  ));
            ana.cutflow.addWgtSyst("FakeClosureSSMuDown" , Lambdas::FakeFactorVariation(Variation::ClosureSSMu , Variation::Down));
            ana.cutflow.addWgtSyst("FakeClosure3LElUp"   , Lambdas::FakeFactorVariation(Variation::Closure3LEl , Variation::Up  ));
            ana.cutflow.addWgtSyst("FakeClosure3LElDown" , Lambdas::FakeFactorVariation(Variation::Closure3LEl , Variation::Down));
            ana.cutflow.addWgtSyst("FakeClosure3LMuUp"   , Lambdas::FakeFactorVariation(Variation::Closure3LMu , Variation::Up  ));
            ana.cutflow.addWgtSyst("FakeClosure3LMuDown" , Lambdas::FakeFactorVariation(Variation::Closure3LMu , Variation::Down));
        }
        else
        {
            ana.cutflow.addWgtSyst("FakeUp"              , UNITY);
            ana.cutflow.addWgtSyst("FakeDown"            , UNITY);
            ana.cutflow.addWgtSyst("FakeRateUp"          , UNITY);
            ana.cutflow.addWgtSyst("FakeRateDown"        , UNITY);
            ana.cutflow.addWgtSyst("FakeRateElUp"        , UNITY);
            ana.cutflow.addWgtSyst("FakeRateElDown"      , UNITY);
            ana.cutflow.addWgtSyst("FakeRateMuUp"        , UNITY);
            ana.cutflow.addWgtSyst("FakeRateMuDown"      , UNITY);
            ana.cutflow.addWgtSyst("FakeRateSSElUp"      , UNITY);
            ana.cutflow.addWgtSyst("FakeRateSSElDown"    , UNITY);
            ana.cutflow.addWgtSyst("FakeRateSSMuUp"      , UNITY);
            ana.cutflow.addWgtSyst("FakeRateSSMuDown"    , UNITY);
            ana.cutflow.addWgtSyst("FakeRate3LElUp"      , UNITY);
            ana.cutflow.addWgtSyst("FakeRate3LElDown"    , UNITY);
            ana.cutflow.addWgtSyst("FakeRate3LMuUp"      , UNITY);
            ana.cutflow.addWgtSyst("FakeRate3LMuDown"    , UNITY);
            ana.cutflow.addWgtSyst("FakeClosureUp"       , UNITY);
            ana.cutflow.addWgtSyst("FakeClosureDown"     , UNITY);
            ana.cutflow.addWgtSyst("FakeClosureElUp"     , UNITY);
            ana.cutflow.addWgtSyst("FakeClosureElDown"   , UNITY);
            ana.cutflow.addWgtSyst("FakeClosureMuUp"     , UNITY);
            ana.cutflow.addWgtSyst("FakeClosureMuDown"   , UNITY);
            ana.cutflow.addWgtSyst("FakeClosureSSElUp"   , UNITY);
            ana.cutflow.addWgtSyst("FakeClosureSSElDown" , UNITY);
            ana.cutflow.addWgtSyst("FakeClosureSSMuUp"   , UNITY);
            ana.cutflow.addWgtSyst("FakeClosureSSMuDown" , UNITY);
            ana.cutflow.addWgtSyst("FakeClosure3LElUp"   , UNITY);
            ana.cutflow.addWgtSyst("FakeClosure3LElDown" , UNITY);
            ana.cutflow.addWgtSyst("FakeClosure3LMuUp"   , UNITY);
            ana.cutflow.addWgtSyst("FakeClosure3LMuDown" , UNITY);
        }

        // Declare cut varying systematics to cuts with the patterns provided in the vector
        ana.cutflow.addCutSyst("JESUp"    , {"jj", "PreSel", "Nj2", "KinSel", "SidemmMET", "OneJet", "NjLeq1", "DRljMin"});
        ana.cutflow.addCutSyst("JESDown"  , {"jj", "PreSel", "Nj2", "KinSel", "SidemmMET", "OneJet", "NjLeq1", "DRljMin"});

        // // 2016 v1.2.2 baby ntuple does not have jer variation
        // ana.cutflow.addCutSyst("JER"      , {"jj", "PreSel", "Nj", "KinSel", "SidemmMET"});
        // ana.cutflow.addCutSyst("JERUp"    , {"jj", "PreSel", "Nj", "KinSel", "SidemmMET"});
        // ana.cutflow.addCutSyst("JERDown"  , {"jj", "PreSel", "Nj", "KinSel", "SidemmMET"});

        // Lambda to add variations
        auto set_syst_cuts = [&](TString systname, Variation::ExpSyst expsyst, Variation::Var var)
        {
            ana.cutflow.setCutSyst("SRSSeePreSel"      , systname, Lambdas::SSPreSelection (expsyst, var) , Lambdas::BTagScaleFactor );
            ana.cutflow.setCutSyst("SRSSeeNj2"         , systname, Lambdas::TwoCenJet30    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SRSSeeKinSel"      , systname, Lambdas::SRSSeeSelection(expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SRSSeeLowDetajj"   , systname, Lambdas::LowDEtajj      (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SRSSeeLowMjj"      , systname, Lambdas::LowMjj         (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SRSSeeMjjIn"       , systname, Lambdas::MjjIn          (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SRSSSideeeMjjOut"  , systname, Lambdas::MjjOut         (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SRSSemPreSel"      , systname, Lambdas::SSPreSelection (expsyst, var) , Lambdas::BTagScaleFactor );
            ana.cutflow.setCutSyst("SRSSemNj2"         , systname, Lambdas::TwoCenJet30    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SRSSemKinSel"      , systname, Lambdas::SRSSemSelection(expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SRSSemLowDetajj"   , systname, Lambdas::LowDEtajj      (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SRSSemLowMjj"      , systname, Lambdas::LowMjj         (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SRSSemMjjIn"       , systname, Lambdas::MjjIn          (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SRSSSideemMjjOut"  , systname, Lambdas::MjjOut         (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SRSSmmPreSel"      , systname, Lambdas::SSPreSelection (expsyst, var) , Lambdas::BTagScaleFactor );
            ana.cutflow.setCutSyst("SRSSmmNj2"         , systname, Lambdas::TwoCenJet30    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SRSSmmKinSel"      , systname, Lambdas::SRSSmmSelection(expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SRSSmmLowDetajj"   , systname, Lambdas::LowDEtajj      (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SRSSmmLowMjj"      , systname, Lambdas::LowMjj         (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SRSSmmMjjIn"       , systname, Lambdas::MjjIn          (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SRSSSidemmMET"     , systname, Lambdas::SRSSSidemmSel  (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SRSSSidemmMjjOut"  , systname, Lambdas::MjjOut         (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SR0SFOSPreSel"     , systname, Lambdas::ThreeLepPresel (expsyst, var) , Lambdas::BTagScaleFactor );
            ana.cutflow.setCutSyst("SR0SFOSKinSel"     , systname, Lambdas::KinSel0SFOS    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SR1SFOSPreSel"     , systname, Lambdas::ThreeLepPresel (expsyst, var) , Lambdas::BTagScaleFactor );
            ana.cutflow.setCutSyst("SR1SFOSKinSel"     , systname, Lambdas::KinSel1SFOS    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("SR2SFOSPreSel"     , systname, Lambdas::ThreeLepPresel (expsyst, var) , Lambdas::BTagScaleFactor );
            ana.cutflow.setCutSyst("SR2SFOSKinSel"     , systname, Lambdas::KinSel2SFOS    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("WZCRSSeePreSel"    , systname, Lambdas::SSPreSelection (expsyst, var) , Lambdas::BTagScaleFactor );
            ana.cutflow.setCutSyst("WZCRSSeeNj2"       , systname, Lambdas::TwoCenJet30    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("WZCRSSeeKinSel"    , systname, Lambdas::SRSSeeSelection(expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("WZCRSSeeLowDetajj" , systname, Lambdas::LowDEtajj      (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("WZCRSSeeLowMjj"    , systname, Lambdas::LowMjj         (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("WZCRSSemPreSel"    , systname, Lambdas::SSPreSelection (expsyst, var) , Lambdas::BTagScaleFactor );
            ana.cutflow.setCutSyst("WZCRSSemNj2"       , systname, Lambdas::TwoCenJet30    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("WZCRSSemKinSel"    , systname, Lambdas::SRSSemSelection(expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("WZCRSSemLowDetajj" , systname, Lambdas::LowDEtajj      (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("WZCRSSemLowMjj"    , systname, Lambdas::LowMjj         (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("WZCRSSmmPreSel"    , systname, Lambdas::SSPreSelection (expsyst, var) , Lambdas::BTagScaleFactor );
            ana.cutflow.setCutSyst("WZCRSSmmNj2"       , systname, Lambdas::TwoCenJet30    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("WZCRSSmmKinSel"    , systname, Lambdas::SRSSmmSelection(expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("WZCRSSmmLowDetajj" , systname, Lambdas::LowDEtajj      (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("WZCRSSmmLowMjj"    , systname, Lambdas::LowMjj         (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("WZCR1SFOSPreSel"   , systname, Lambdas::ThreeLepPresel (expsyst, var) , Lambdas::BTagScaleFactor );
            ana.cutflow.setCutSyst("WZCR1SFOSKinSel"   , systname, Lambdas::KinSel1SFOS    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("WZCR2SFOSPreSel"   , systname, Lambdas::ThreeLepPresel (expsyst, var) , Lambdas::BTagScaleFactor );
            ana.cutflow.setCutSyst("WZCR2SFOSKinSel"   , systname, Lambdas::KinSel2SFOS    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("ARSSeePreSel"      , systname, Lambdas::SSPreSelection (expsyst, var) , Lambdas::BTagScaleFactor );
            ana.cutflow.setCutSyst("ARSSeeNj2"         , systname, Lambdas::TwoCenJet30    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("ARSSeeKinSel"      , systname, Lambdas::SRSSeeSelection(expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("ARSSeeLowDetajj"   , systname, Lambdas::LowDEtajj      (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("ARSSeeLowMjj"      , systname, Lambdas::LowMjj         (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("ARSSeeMjjIn"       , systname, Lambdas::MjjIn          (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("ARSSSideeeMjjOut"  , systname, Lambdas::MjjOut         (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("ARSSemPreSel"      , systname, Lambdas::SSPreSelection (expsyst, var) , Lambdas::BTagScaleFactor );
            ana.cutflow.setCutSyst("ARSSemNj2"         , systname, Lambdas::TwoCenJet30    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("ARSSemKinSel"      , systname, Lambdas::SRSSemSelection(expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("ARSSemLowDetajj"   , systname, Lambdas::LowDEtajj      (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("ARSSemLowMjj"      , systname, Lambdas::LowMjj         (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("ARSSemMjjIn"       , systname, Lambdas::MjjIn          (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("ARSSSideemMjjOut"  , systname, Lambdas::MjjOut         (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("ARSSmmPreSel"      , systname, Lambdas::SSPreSelection (expsyst, var) , Lambdas::BTagScaleFactor );
            ana.cutflow.setCutSyst("ARSSmmNj2"         , systname, Lambdas::TwoCenJet30    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("ARSSmmKinSel"      , systname, Lambdas::SRSSmmSelection(expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("ARSSmmLowDetajj"   , systname, Lambdas::LowDEtajj      (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("ARSSmmLowMjj"      , systname, Lambdas::LowMjj         (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("ARSSmmMjjIn"       , systname, Lambdas::MjjIn          (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("ARSSSidemmMET"     , systname, Lambdas::SRSSSidemmSel  (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("ARSSSidemmMjjOut"  , systname, Lambdas::MjjOut         (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("AR0SFOSPreSel"     , systname, Lambdas::ThreeLepPresel (expsyst, var) , Lambdas::BTagScaleFactor );
            ana.cutflow.setCutSyst("AR0SFOSKinSel"     , systname, Lambdas::KinSel0SFOS    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("AR1SFOSPreSel"     , systname, Lambdas::ThreeLepPresel (expsyst, var) , Lambdas::BTagScaleFactor );
            ana.cutflow.setCutSyst("AR1SFOSKinSel"     , systname, Lambdas::KinSel1SFOS    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("AR2SFOSPreSel"     , systname, Lambdas::ThreeLepPresel (expsyst, var) , Lambdas::BTagScaleFactor );
            ana.cutflow.setCutSyst("AR2SFOSKinSel"     , systname, Lambdas::KinSel2SFOS    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("EXSSeeNj1OneJet"   , systname, Lambdas::OneCenJet30    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("EXSSeeNj1DRljMin"  , systname, Lambdas::Nj1DRljMin     (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("EXSSemNj1OneJet"   , systname, Lambdas::OneCenJet30    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("EXSSemNj1DRljMin"  , systname, Lambdas::Nj1DRljMin     (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("EXSSmeNj1OneJet"   , systname, Lambdas::OneCenJet30    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("EXSSmeNj1DRljMin"  , systname, Lambdas::Nj1DRljMin     (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("EXSSmmNj1OneJet"   , systname, Lambdas::OneCenJet30    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("EXSSmmNj1DRljMin"  , systname, Lambdas::Nj1DRljMin     (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("EXCRSSeeNjLeq1"    , systname, Lambdas::LeqOneJet30    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("EXCRSSeeNj1KinSel" , systname, Lambdas::Nj1CRKinSel    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("EXCRSSemNjLeq1"    , systname, Lambdas::LeqOneJet30    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("EXCRSSemNj1KinSel" , systname, Lambdas::Nj1CRKinSel    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("EXCRSSmeNjLeq1"    , systname, Lambdas::LeqOneJet30    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("EXCRSSmeNj1KinSel" , systname, Lambdas::Nj1CRKinSel    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("EXCRSSmmNjLeq1"    , systname, Lambdas::LeqOneJet30    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("EXCRSSmmNj1KinSel" , systname, Lambdas::Nj1CRKinSel    (expsyst, var) , UNITY                    );
            ana.cutflow.setCutSyst("CRBTageePreSel"    , systname, Lambdas::SSPreSelection (expsyst, var, true), Lambdas::BTagScaleFactor);
            ana.cutflow.setCutSyst("CRBTageeNj2"       , systname, Lambdas::TwoCenJet30    (expsyst, var), UNITY);
            ana.cutflow.setCutSyst("CRBTageeKinSel"    , systname, Lambdas::SRSSeeSelection(expsyst, var), UNITY);
            ana.cutflow.setCutSyst("CRBTagemPreSel"    , systname, Lambdas::SSPreSelection (expsyst, var, true), Lambdas::BTagScaleFactor);
            ana.cutflow.setCutSyst("CRBTagemNj2"       , systname, Lambdas::TwoCenJet30    (expsyst, var), UNITY);
            ana.cutflow.setCutSyst("CRBTagemKinSel"    , systname, Lambdas::SRSSemSelection(expsyst, var), UNITY);
            ana.cutflow.setCutSyst("CRBTagmmPreSel"    , systname, Lambdas::SSPreSelection (expsyst, var, true), Lambdas::BTagScaleFactor);
            ana.cutflow.setCutSyst("CRBTagmmNj2"       , systname, Lambdas::TwoCenJet30    (expsyst, var), UNITY);
            ana.cutflow.setCutSyst("CRBTagmmKinSel"    , systname, Lambdas::SRSSmmSelection(expsyst, var), UNITY);
            ana.cutflow.setCutSyst("CRBTag0SFOSPreSel" , systname, Lambdas::ThreeLepPresel (expsyst, var, true), Lambdas::BTagScaleFactor);
            ana.cutflow.setCutSyst("CRBTag1SFOSPreSel" , systname, Lambdas::ThreeLepPresel (expsyst, var, true), Lambdas::BTagScaleFactor);
            ana.cutflow.setCutSyst("CRBTag2SFOSPreSel" , systname, Lambdas::ThreeLepPresel (expsyst, var, true), Lambdas::BTagScaleFactor);
        };

        // Actually set the door
        set_syst_cuts("JESUp", Variation::JES, Variation::Up);
        set_syst_cuts("JESDown", Variation::JES, Variation::Down);

        // // If newer year also set JER
        // set_syst_cuts("JER", Variation::JER, Variation::Nominal);
        // set_syst_cuts("JERUp", Variation::JER, Variation::Up);
        // set_syst_cuts("JERDown", Variation::JER, Variation::Down);

    }



//*************************************************************************************************************
//
// 7. Booking histograms and cutflows
//
//*************************************************************************************************************

    // So far we have defined a tree structure of cuts (RooUtil::Cutflow object)
    // Also we defined a list of histograms (RooUtil::Histograms)

    // Now we need to book the histograms for cutflows and variable distribution histograms
    // So during the loop it actually fills them

    // First of all, if the ana.do_systematics == true, we are not going to run all cutflows, nor all histograms
    // We will only run histograms at the very end of the cuts, this is to speed things up

    // NOTE: Booking cutflows generally take longer than booking a lot of histograms only at the end cut level
    // I haven't fully understood why. I think it might be due to the map look up time. (TString EqualTo calls)

    // If --force was provided skip this special if statement and go to else statement and do things normally
    if (ana.do_systematics and not ana.do_force)
    {
        // Book histograms only at the end cuts
        ana.cutflow.bookHistogramsForEndCuts(ana.histograms);

        // And NO book cutflows!
    }
    else
    {

        // Book histograms
        if (ana.do_histograms)
        {
  	    ana.cutflow.bookHistogramsForCutAndBelow(ana.histograms, "CutSRSSDilepDijets");
            ana.cutflow.bookHistogramsForCutAndBelow(ana.histograms, "CutSRDilep");
            ana.cutflow.bookHistogramsForCutAndBelow(ana.histograms, "CutSRTrilep");
            ana.cutflow.bookHistogramsForCutAndBelow(ana.histograms, "CutWZCRTrilep");
            ana.cutflow.bookHistogramsForCutAndBelow(ana.histograms, "CutARDilep");
            ana.cutflow.bookHistogramsForCutAndBelow(ana.histograms, "CutARTrilep");
        }

        // Book cutflows
        if (ana.do_cutflow)
            ana.cutflow.bookCutflows();

        // Book eventlist
        if (ana.do_eventlist)
            ana.cutflow.bookEventLists();

    }


    //
    // Print cut structure before starting the loop just to visually see it
    //
    // Print once before starting any loop (at this point, "pass|weight" columns will be entirely empty since it's not showing for a any specific event
    ana.cutflow.printCuts();

    //
    // The following function will print out the tree structure for example
    //
    // RooUtil::Cutflow::printCuts();
    //
    // <Program is running>
    // ...
    // ...
    // ...
    // RooUtil:: Cut name                                                     |pass|weight|systs
    // RooUtil:: ======================================================================================
    // RooUtil:: Root                                                         | 0 | 0.000000|
    // RooUtil::   CutWeight                                                  | 0 | 0.000000|
    // RooUtil::     CutPreliminary                                           | 0 | 0.000000|
    // RooUtil::       CutTrigger                                             | 0 | 0.000000|
    // RooUtil::        +CutSRDilep                                           | 0 | 0.000000|
    // RooUtil::        | +SRSSmm                                             | 0 | 0.000000|
    // RooUtil::        | |  SRSSmmTVeto                                      | 0 | 0.000000|
    // RooUtil::        | |    SRSSmmNj2                                      | 0 | 0.000000|
    // RooUtil::        | |      SRSSmmNb0                                    | 0 | 0.000000|
    // RooUtil::        | |        SRSSmmMjjW                                 | 0 | 0.000000|
    // RooUtil::        | |          SRSSmmMjjL                               | 0 | 0.000000|
    // RooUtil::        | |            SRSSmmDetajjL                          | 0 | 0.000000|
    // RooUtil::        | |              SRSSmmMET                            | 0 | 0.000000|
    // RooUtil::        | |                SRSSmmMllSS                        | 0 | 0.000000|
    // RooUtil::        | |                  SRSSmmFull                       | 0 | 0.000000|
    // RooUtil::        | +SRSSem                                             | 0 | 0.000000|
    // RooUtil::        | |  SRSSemTVeto                                      | 0 | 0.000000|
    // RooUtil::        | |    SRSSemNj2                                      | 0 | 0.000000|
    // RooUtil::        | |      SRSSemNb0                                    | 0 | 0.000000|
    // RooUtil::        | |        SRSSemMjjW                                 | 0 | 0.000000|
    // RooUtil::        | |          SRSSemMjjL                               | 0 | 0.000000|
    // RooUtil::        | |            SRSSemDetajjL                          | 0 | 0.000000|
    // RooUtil::        | |              SRSSemMET                            | 0 | 0.000000|
    // RooUtil::        | |                SRSSemMllSS                        | 0 | 0.000000|
    // RooUtil::        | |                  SRSSemMTmax                      | 0 | 0.000000|
    // RooUtil::        | |                    SRSSemFull                     | 0 | 0.000000|
    // RooUtil::        | +SRSSee                                             | 0 | 0.000000|
    // RooUtil::        | |  SRSSeeZeeVt                                      | 0 | 0.000000|
    // RooUtil::        | |    SRSSeeTVeto                                    | 0 | 0.000000|
    // RooUtil::        | |      SRSSeeNj2                                    | 0 | 0.000000|
    // RooUtil::        | |        SRSSeeNb0                                  | 0 | 0.000000|
    // RooUtil::        | |          SRSSeeMjjW                               | 0 | 0.000000|
    // RooUtil::        | |            SRSSeeMjjL                             | 0 | 0.000000|
    // RooUtil::        | |              SRSSeeDetajjL                        | 0 | 0.000000|
    // RooUtil::        | |                SRSSeeMET                          | 0 | 0.000000|
    // RooUtil::        | |                  SRSSeeMllSS                      | 0 | 0.000000|
    // RooUtil::        | |                    SRSSeeFull                     | 0 | 0.000000|
    // ...
    // ...
    // ...
    //
    // (note, the above printout was from an older version of the code, so the cut structure changed a little bit)
    //
    // Also, for debugging purpose, one could print this per event.
    // If printCuts() is called AFTER "RooUtil::Cutflow::fill()" is called,
    // Then, the "pass|weight" columns on the right will be filled with whether at certain cut stage event passes or not.
    // along with event weights
    //

//*************************************************************************************************************
//
// 8. Looping events
//
//*************************************************************************************************************

    while (ana.looper.nextEvent())
    {

        // If a new file was opened after "looper.nextEvent" was called configure the sample dependent settings in class InputConfig;
        if (ana.looper.isNewFileInChain())
        {
            std::cout << "new file in chain" << std::endl;
            input.determine_input_settings(ana.looper.getCurrentFileName(), ana.input_tree_name);
            if (input.year == 2016) fakerates.load2016(); // Not properly implemented
            if (input.year == 2017) fakerates.load2017();
            if (input.year == 2018) fakerates.load2018();
        }

        // If splitting jobs are requested then determine whether to process the event or not based on remainder
        if (result.count("job_index") and result.count("nsplit_jobs"))
        {
            if (ana.looper.getNEventsProcessed() % ana.nsplit_jobs != (unsigned int) ana.job_index)
                continue;
        }

        // // For memoize function. This is to cache results for each event. this_run/lumi/evt is a global variable defined in lambdas.cc
        // this_run = www.run();
        // this_lumi = www.lumi();
        // this_evt = www.evt();

        // // Theory related weights from h_neventsinfile in each input root file but only set files when new file opens
        // // NOTE if there was a continue statement prior to this it can mess it up
        // if (looper.isNewFileInChain() and not isData) theoryweight.setFile(looper.getCurrentFileName());

        // Set the event list variables for EventList booking
        if (ana.do_eventlist and not (ana.do_systematics and not ana.do_force))
            ana.cutflow.setEventID(www.run(), www.lumi(), www.evt());

        // This magic "fill()" function will now go through all the cut nodes in the RooUtil::Cutflow and evaluate whether it passes the cut or not
        // And also fill histograms for all the booked histograms and fill all the book cutflows
        ana.cutflow.fill();
	
	if (ana.cutflow.getCut("SRSSeelljjbbFullDD").pass)
	  {
	    std::cout << std::endl;
	    std::cout << " www.nb() " << www.nb() << "\t";
	    std::cout << " Lambdas::GetMbb() " << Lambdas::GetMbb() << endl;
	  }
        // if (ana.cutflow.getCut("SRSSmmFull").pass)
        // {
        //     std::cout << std::endl;
            // std::cout <<  " www.run(): " << www.run() <<  " www.lumi(): " << www.lumi() <<  " www.evt(): " << www.evt() <<  std::endl;
            // std::cout <<  " www.CMS4path(): " << www.CMS4path() <<  " www.CMS4index(): " << www.CMS4index() <<  std::endl;
            // std::cout <<  " Lambdas::EventWeight(): " << Lambdas::EventWeight() <<  std::endl;
            // std::cout <<  " www.lep_pdgId()[0]: " << www.lep_pdgId()[0] <<  " www.lep_pdgId()[1]: " << www.lep_pdgId()[1] <<  std::endl;
            // std::cout <<  " www.lep_motherIdSS()[0]: " << www.lep_motherIdSS()[0] <<  " www.lep_motherIdSS()[1]: " << www.lep_motherIdSS()[1] <<  std::endl;
            // std::cout <<  " www.lep_convVeto()[0]: " << www.lep_convVeto()[0] <<  " www.lep_convVeto()[1]: " << www.lep_convVeto()[1] <<  std::endl;
            // std::cout <<  " www.lep_lostHits()[0]: " << www.lep_lostHits()[0] <<  " www.lep_lostHits()[1]: " << www.lep_lostHits()[1] <<  std::endl;

#ifdef WITHCORE
            // cms4reader.open();
            // cms4reader.printLeptons();
            // cms4reader.printPartonsWithCondition("===All Particles===");
#endif

        // }

        // if (ana.cutflow.getCut("SRSSmmFull").pass || ana.cutflow.getCut("SRSSemFull").pass || ana.cutflow.getCut("SRSSeeFull").pass)
        // {
        //     std::cout <<  " www.lep_pdgId()[0]: " << www.lep_pdgId()[0] <<  " www.lep_pdgId()[1]: " << www.lep_pdgId()[1] <<  std::endl;
        //     std::cout <<  " www.lep_motherIdSS()[0]: " << www.lep_motherIdSS()[0] <<  " www.lep_motherIdSS()[1]: " << www.lep_motherIdSS()[1] <<  std::endl;
        //     std::cout <<  " www.lep_convVeto()[0]: " << www.lep_convVeto()[0] <<  " www.lep_convVeto()[1]: " << www.lep_convVeto()[1] <<  std::endl;
        //     std::cout <<  " www.lep_lostHits()[0]: " << www.lep_lostHits()[0] <<  " www.lep_lostHits()[1]: " << www.lep_lostHits()[1] <<  std::endl;
        // }

        // // Must come after .fill() so that the cuts are all evaulated
        // if (eventlist.has(www.run(), www.lumi(), www.evt()))
        // {
        //     std::cout <<  "***************************" << std::endl;
        //     std::cout <<  "**** Cutflow Challenge ****" << std::endl;
        //     std::cout <<  "***************************" << std::endl;
        //     std::cout <<  " Checking following events:" << std::endl;
        //     std::cout <<  " www.run(): " << www.run() <<  " www.lumi(): " << www.lumi() <<  " www.evt(): " << www.evt() <<  std::endl;
        //     std::cout <<  " Below shows whether it passes or fails the cuts" << std::endl;
        //     cutflow.printCuts();
        // }

        // if (ana.cutflow.getCut("SRSSmmFull").pass)
        // {

        //     float pt0 = www.lep_p4()[0].pt();
        //     float pt1 = www.lep_p4()[1].pt();
        //     float eta0 = www.lep_p4()[0].eta();
        //     float eta1 = www.lep_p4()[1].eta();
        //     float leadeta  = std::min(fabs(eta0), (float)2.39);
        //     float traileta = std::min(fabs(eta1), (float)2.39);
        //     float leadpt   = std::min(pt0, (float)199.);
        //     float trailpt  = std::min(pt1, (float)199.);

        //     float bigeta   = leadeta > traileta ? leadeta  : traileta;
        //     float smalleta = leadeta > traileta ? traileta : leadeta;

        //     // is mm events
        //     if (abs(www.lep_pdgId()[0]) == 13 && abs(www.lep_pdgId()[1]) == 13)
        //     {
        //         // related to lepton legs
        //         float e_l0 = trigsf_mu_lead(leadpt, leadeta);
        //         float e_t1 = trigsf_mu_trail(trailpt, traileta);
        //         float d_l0 = trigsf_mu_lead(leadpt, leadeta, 1) - trigsf_mu_lead(leadpt, leadeta);
        //         float d_t1 = trigsf_mu_trail(trailpt, traileta, 1) - trigsf_mu_trail(trailpt, traileta);
        //         float tempeff = 1.0;
        //         float temperr = 0.0;
        //         std::tie(tempeff, temperr) = getCombinedTrigEffandError(e_l0, 0., 0., e_t1, d_l0, 0., 0., d_t1);
        //         // dz
        //         float dzeff = 0.241 * trigsf_dimu_dz(smalleta, bigeta) + (1 - 0.241) * 1; // Because DZ filter only affects Period H
        //         float dzerr = 0.241 * (trigsf_dimu_dz(smalleta, bigeta, 1) - trigsf_dimu_dz(smalleta, bigeta));
        //         float eff = tempeff * dzeff;
        //         float err = eff * sqrt(pow(temperr / tempeff, 2) + pow(dzerr / dzeff, 2));
        //         // And the fractino of period H is calculated from here: http://www.t2.ucsd.edu/tastwiki/bin/view/CMS/Run2_Data2016
        //         // 8.636 + 0.221 / 36.814 = 0.241

        //         std::cout <<  " tempeff: " << tempeff <<  " temperr: " << temperr <<  std::endl;
        //         std::cout <<  " dzeff: " << dzeff <<  " dzerr: " << dzerr <<  std::endl;
        //         std::cout <<  " eff: " << eff <<  " err: " << err <<  std::endl;
        //         std::cout <<  " (eff+err)/eff: " << (eff+err)/eff <<  " (eff-err)/eff: " << (eff-err)/eff <<  std::endl;

        //         std::cout <<  " www.lep_p4()[0].pt(): " << www.lep_p4()[0].pt() <<  " www.lep_p4()[0].eta(): " << www.lep_p4()[0].eta() <<  " www.lep_pdgId()[0]: " << www.lep_pdgId()[0] <<  std::endl;
        //         std::cout <<  " www.lep_p4()[1].pt(): " << www.lep_p4()[1].pt() <<  " www.lep_p4()[1].eta(): " << www.lep_p4()[1].eta() <<  " www.lep_pdgId()[1]: " << www.lep_pdgId()[1] <<  std::endl;
        //         float trigsf = Lambdas::TriggerScaleFactor();
        //         float trigsfup = Lambdas::TriggerSFVariation(Variation::Up)();
        //         float trigsfdn = Lambdas::TriggerSFVariation(Variation::Down)();
        //         std::cout <<  " trigsf: " << trigsf <<  " trigsfup: " << trigsfup <<  " trigsfdn: " << trigsfdn <<  std::endl;

        //     }
        // }

    }

    // If bookEventLists has been called then print the event lists into a text file for terminating node cuts (i.e. the final selections applied. e.g. SRSSeeFull, ..., etc.)
    if (ana.do_eventlist and not (ana.do_systematics and not ana.do_force))
    {
        std::vector<TString> regions = ana.cutflow.getCut("Root").getEndCuts();
        for (auto& region : regions)
        {
            TString output_tfile_name = ana.output_tfile->GetName(); // get the output file name
            TString suffix = TString::Format(".%s.txt", region.Data());
            output_tfile_name.ReplaceAll(".root", suffix); // replace .root with suffix if .root exists
            if (not output_tfile_name.Contains(suffix)) output_tfile_name += suffix; // if no suffix exists, then append suffix
            ana.cutflow.getCut(region).writeEventList(output_tfile_name);
        }
    }

    // Once done with the loop, now save all the histograms to the output file
    ana.cutflow.saveOutput();

}

