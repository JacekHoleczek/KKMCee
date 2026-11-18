//
// Usage example:
//   /full/path/to/MainMini 10000
//
// Notes:
//   1. You MUST have the "./KKMCee_defaults" file.
//   2. You MUST have the "./pro.input" file.
//   3. You MUST have the generated "./DIZET-table1" file.
//

#include "KKee2f.h"
#include "KKevent.h"

#include "TFile.h"
#include "TRandom3.h"
#include "TH1.h"

#include "HepMC3/GenEvent.h"
#include "HepMC3/WriterAscii.h"

#include <cstdio>
#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
  long NevGen = -1;
  if (argc > 1) { // if any arguments are given ...
    // ... the first argument is the requested number of events
    if (sscanf(argv[1], "%ld", &NevGen) != 1) return 1; // just a precaution
    if (NevGen < 1) return 1; // just a precaution
  }
  //
  std::ofstream OutFile("pro.output"); // Logfile output
  TFile *MCgenFile = TFile::Open("mcgen.root", "RECREATE", "Generators");
  // MCgenFile->cd();
  TRandom3 *RN_gen = new TRandom3(); // Central random numb. gen.
  ULong_t iniseed = 54217137; RN_gen->SetSeed(iniseed);
  //
  KKee2f *KKMCgen = new KKee2f("MCgen"); // MC generator object
  //
  int nb = 10000;
  TH1D *h_NORMA = new TH1D("KKMCgen_NORMA", "Normaliz. histo", nb, 0., nb);
  //
  KKMCgen->Initialize(RN_gen, &OutFile, h_NORMA);
  /////////////////////////////////////////////////////////////
  std::cout << "MainMini: **********************************" << std::endl;
  if (NevGen < 1) { // ask the user
    NevGen = 10000;
    std::cout << "MainMini: type in no. of MC events (e.g., " << NevGen << "): ";
    std::cin >> NevGen;
  }
  if (NevGen < 1) NevGen = 1; // just a precaution
  std::cout << "MainMini: requested " << NevGen << " events" << std::endl;
  /////////////////////////////////////////////////////////////
  HepMC3::WriterAscii text_output(KKMCgen->hepmc_out_name); // output file for events in hepmc3 format
  std::ofstream LHE_out = KKMCgen->m_Event->LHEF_init("out.lhe"); //output LHE file
  /////////////////////////////////////////////////////////////
  // Loop over MC events
  for(long iev = 1; iev <= NevGen; iev++) {
    KKMCgen->Generate();
    if(KKMCgen->save_to_hepmc){
    text_output.write_event(*KKMCgen->m_Hvent); 
    }
    KKMCgen->m_Event->LHEF_Fill(&LHE_out); //saving event in LHE file
    if ((iev % 1000 == 0) || (iev <= 10)) {
      std::cout << "MainMini: iev = " << iev << std::endl;
      KKMCgen->m_Event->EventPrintAll(); // printing out KKMCee event
      KKMCgen->m_Event->EventPrintAll(&OutFile);
    } // if iev
  } // for iev
  /////////////////////////////////////////////////////////////
  text_output.close();
  KKMCgen->m_Event->LHEF_Close(&LHE_out);
  /////////////////////////////////////////////////////////////
  KKMCgen->Finalize(); // final printout
  /////////////////////////////////////////////////////////////
  MCgenFile->cd();
  KKMCgen->Write("MCgen", TObject::kOverwrite);
  RN_gen->Write("RN_gen", TObject::kOverwrite);
  MCgenFile->Write();
  delete MCgenFile;
  //
  // delete KKMCgen;
  // delete RN_gen;
  //
  std::cout << "  |--------------------| " << std::endl;
  std::cout << "  |  MainMini  Ended   | " << std::endl;
  std::cout << "  |--------------------| " << std::endl;
  //
  return 0;
} // main

// end of file MainMini.cxx

