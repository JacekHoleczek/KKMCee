// ---------------------------------------------------------- //
// MainKKMCee.cxx : main program steering generation of 2f    //
//                  events saving the result in HEPMC3 format //
//                  for FCCee studies                         //
//                                                            //
// Author: G Ganis / CERN (inspired from MainMini.cxx)        //
// Initial date: 14 July 2022                                 //
// Update:
// ---------------------------------------------------------- //

#include "KKee2f.h"
#include "KKevent.h"

#include "TFile.h"
#include "TRandom3.h"
#include "TH1.h"
#include "TTimeStamp.h"

#include "HepMC3/GenEvent.h"
#include "HepMC3/WriterAscii.h"

#include <fstream>
#include <iostream>
#include <string>

// ---- Utility functions protototype ----------------------- //
int argparser(int argc, char **argv);
void printhelp();
void printstatvar();

// ---- Static variables ------------------------------------ //
static std::string sFileLog = "pro.output";
static std::string sMCgen = "mcgen.root";
static std::string sFileOut = "kkmcee.hepmc3";
static long sInitSeed = -1;
static int sNbin = -1;
static long sNGenEvt = -1;

// ---- Main Program ---------------------------------------- //
int main(int argc, char **argv)
{
    int rc = 0;
    // Parse arguments
    if ((rc = argparser(argc, argv)) < 0) {
        cout << "argv[0]: problem parsing arguments" << endl;
        exit(1);
    }
    cout << argv[0] <<": command line arguments processed: " << rc << endl;

//    printstatvar();
//    if (1) exit(0);

    // Logfile output
    ofstream fileOut(sFileLog, ios::out);

    // Summary file in ROOT format
    TFile* fileMCgen = new TFile(sMCgen.c_str(), "RECREATE", "Generators");
    fileMCgen->cd();

    // HepMC3 output
    HepMC3::WriterAscii* hepMCWriter = new HepMC3::WriterAscii(sFileOut);

    // Random generation (TODO: set random seed)
    TRandom3* genRN = new TRandom3();
    if (sInitSeed < 0) {
       TTimeStamp ts;
       sInitSeed = ts.GetSec() * 10000 + ts.GetNanoSec();
    }
    genRN->SetSeed(sInitSeed);

    // MC generator object initialization
    KKee2f* genKKMC = new KKee2f("MCgen");
    if (sNbin < 0) sNbin = 1000;
    TH1D* hNorm= new TH1D("genKKMC_NORMA", "Normalization histo", sNbin, 0, sNbin);
    genKKMC->Initialize(genRN, &fileOut, hNorm);

    // Number of events to be generated
    if (sNGenEvt < 0) {
       cout << "argv[0]: ********************************** "<<endl;
       cout << "argv[0]: enter number of MC events: [" << sNGenEvt << "] ";
       cin >> sNGenEvt; cout<<" requested "<< sNGenEvt <<" events"<<endl;
    }

    // Loop over MC events
    for (long iev = 1; iev <= sNGenEvt; iev++) {
        genKKMC->Generate();
        if ((iev/100)*100 == iev || iev<=10) {
           cout<<"argv[0]: iev = "<<iev<<endl;
           genKKMC->m_Event->EventPrintAll();
        }
        // Dump event to file in HepMC3 ascii format
        hepMCWriter->write_event(*(genKKMC->m_Hvent));
        // genKKMC->m_Hvent->EventPrintAll(&fileHepMC);
    }

    // Finalize generation
    genKKMC->Finalize(); // final printout
    cout << "  |--------------------| "<<endl<<flush;
    cout << "  |  MainKKMCee ended  | "<<endl<<flush;
    cout << "  |--------------------| "<<endl<<flush;

    // Finalize files
    hepMCWriter->close();
    fileMCgen->cd();
    genKKMC->Write("MCgen",TObject::kOverwrite);
    genRN->Write("RN_gen",TObject::kOverwrite);
    fileMCgen->Write();
    delete fileMCgen;

    // Done
    return 0;
}


// ---- Utility functions ------------------------------------- //
int argparser(int argc, char **argv)
{
    // Parse command line arguments

    int rc = 0;
    if (argc <= 1) return rc;

    for ( int ia = 1; ia < argc ; ia++) {
        char *c = argv[ia];
        if ( *c == '-') {
           ++rc;
           ++c;
           if ( *c == 'h') {
               printhelp();
               exit(0);
           } else if (*c == 'l') {
               ++ia;
               sFileLog = argv[ia];
           } else if (*c == 'm') {
               ++ia;
               sMCgen = argv[ia];
           } else if (*c == 'o') {
               ++ia;
               sFileOut = argv[ia];
           } else if (*c == 's') {
               ++ia;
               sInitSeed = std::stol(argv[ia]);
           } else if (*c == 'b') {
               ++ia;
               sNbin = std::stoi(argv[ia]);
           } else if (*c == 'n') {
               ++ia;
               sNGenEvt = std::stol(argv[ia]);
           }
        }
    }

    return rc;
}

void printhelp()
{
    // Dump the static vars
    
    cout << " -D file with defaults  [./KKMCee_Defaults]"<< endl;
    cout << " -c config file         [./pro.input]  "  << endl;
    cout << " -l log file            [pro.output]" << endl;
    cout << " -m MC root file        [mcgen.root] " << endl;
    cout << " -o HEPMC3 output file  [kkmcee.hepmc3]" << endl;
    cout << " -s initial Seed        [derived from current time]" << endl;
    cout << " -b normalization bins  [1000]" << endl;
    cout << " -n number of events    [10]" << endl;
    cout << " -h print this help " << endl;

}

void printstatvar()
{
    // Dump the static vars
    
    cout << " Log file:           " << sFileLog << endl;
    cout << " MC root file:       " << sMCgen << endl;
    cout << " HEPMC3 output file: " << sFileOut << endl;
    cout << " Initial Seed:       " << sInitSeed << endl;
    cout << " Normalization bins: " << sNbin << endl;
    cout << " Number of Events:   " << sNGenEvt << endl;

}
