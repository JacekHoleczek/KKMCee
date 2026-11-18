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


void printhelp(){
  cout<<"Options are: \n"<<"-r Random seed \n" <<"-n Number of events"<<std::endl;
}

int main(int argc, char* argv[]) {
  long NevGen = -1;
  int seed = 12345;
  int nevents = 100;
  if (argc > 1) {
    for (int i = 1; i < argc; ++i)
    {
      if(std::string(argv[i])=="-r") seed = std::stoi(argv[i+1]);
      else if(std::string(argv[i])=="-n") nevents = std::stoi(argv[i+1]);
      else if(std::string(argv[i])=="-h") printhelp();
      else{
        if(std::string(argv[i]).find("-") == std::string::npos) continue;
        cout<<"Unknown option "<<argv[i]<<std::endl;
        printhelp();
        exit(1);
      }
    }

  }
  //
  std::ofstream OutFile("pro.output"); // Logfile output
  TFile *MCgenFile = TFile::Open("mcgen.root", "RECREATE", "Generators");
  // MCgenFile->cd();
  TRandom3 *RN_gen = new TRandom3(); // Central random numb. gen.
  RN_gen->SetSeed(seed);
  //
  KKee2f *KKMCgen = new KKee2f("MCgen"); // MC generator object
  //
  int nb = 10000;
  TH1D *h_NORMA = new TH1D("KKMCgen_NORMA", "Normaliz. histo", nb, 0., nb);
  //
  KKMCgen->Initialize(RN_gen, &OutFile, h_NORMA);
  std::cout << "MainCluster: requested " << nevents << " events" << std::endl;
  /////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////
  HepMC3::WriterAscii text_output("./kkmc.hepmc3"); // output file for events in hepmc3 format
  /////////////////////////////////////////////////////////////
  // Loop over MC events
  for(long iev = 1; iev <= nevents; iev++) {
    KKMCgen->Generate();
    if(KKMCgen->m_WtMain!=0) text_output.write_event(*KKMCgen->m_Hvent); 
    if ((iev % 1000 == 0) || (iev <= 10)) {
      std::cout << "MainCluster: iev = " << iev << std::endl;
      KKMCgen->m_Event->EventPrintAll(); // printing out KKMCee event
      KKMCgen->m_Event->EventPrintAll(&OutFile);
    } // if iev
  } // for iev
  /////////////////////////////////////////////////////////////
  text_output.close();
  /////////////////////////////////////////////////////////////
  KKMCgen->Finalize(); // final printout
  /////////////////////////////////////////////////////////////
  MCgenFile->cd();
  KKMCgen->Write("MCgen", TObject::kOverwrite);
  RN_gen->Write("RN_gen", TObject::kOverwrite);
  MCgenFile->Write();
  delete MCgenFile;
  return 0;
} // main