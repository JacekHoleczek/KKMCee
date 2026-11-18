///////////////////////////////////////////////////////////////////////////////
// This module handles main Monte Carlo loop and writing histograms onto disk.
// It is UNIVERSAL, works for any MC generator for event analysis.
// MC event generator and analysis is encapsulated inside RobolA module.
// Three different disk files are used
///////////////////////////////////////////////////////////////////////////////
// C++ headers
using namespace std;
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>

// ROOT headers
#include "TApplication.h"
#include "TString.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "TCollection.h"
#include "TFile.h"

// OUR headers
#include "TSemaf.h"
#include "TRobol.h"

#if 0 /* 0 or 1 */
#include "TRobolKKMC.h"
#include "TRobolFoam.h"
#endif /* 0 or 1 */

int main(int argc, char **argv)
{
  //++++++++++++++++++++++++++++++++++++++++
  {
    // make sure that the autoloading of ROOT libraries works
    TApplication *a = new TApplication("MainMCdev", &argc, argv);
    if (a->InputFiles()) { // process command line macro files
      TIter n(a->InputFiles());
      TObjString *f;
      while ((f = (TObjString *)n())) a->ProcessFile(f->String());
    }
    if (a->QuitOpt()) return 0; // "-q" was given so exit now
    // make sure that the corresponding shared libraries are "required"
    const char *c = TSemaf::Class_Name(); c = c;
#if 0 /* 0 or 1 */
    c = TRobolKKMC::Class_Name(); c = c;
    c = TRobolFoam::Class_Name(); c = c;
#endif /* 0 or 1 */
  }
  //++++++++++++++++++++++++++++++++++++++++
  TFile HistoFile("histo.root","UPDATE","histograms");
  TFile MCgenFile("mcgen.root","UPDATE","Generators");
#if 0 /* 0 or 1 */
  cout<<"------------------------------MCgenFile->ls----------------------------------"<<endl;
  MCgenFile.ls();
  MCgenFile.GetListOfKeys()->Print();
  cout<<"------------------------------MCgenFile.ShowStreamerInfo---------------------------"<<endl;
  MCgenFile.ShowStreamerInfo();
  cout<<"------------------------------HistoFile.ShowStreamerInfo---------------------------"<<endl;
  HistoFile.ShowStreamerInfo();
  cout<<"------------------------------MCgenFile-------------------------------------"<<endl;
#endif /* 0 or 1 */
  
  char chcyc[100];          // Cycle text variable
  Text_t *Tcycle;           // Cycle text variable
  TFile *SemafFile;    // Semaphore and loop params
  SemafFile = TFile::Open("semaf.root","UPDATE","Semaphor");
  if ((!SemafFile) || SemafFile->IsZombie()) return 1; // just a precaution
  ofstream   OutFile("pro.output",ios::out);  // Logfile output
  cout    <<endl<<flush;
  cout    << "  |--------------------| "<<endl<<flush;
  cout    << "  | MainMCdev  Entering| "<<endl<<flush;
  cout    << "  |--------------------| "<<endl<<flush;
  OutFile <<endl<<flush;
  OutFile << "  |--------------------| "<<endl<<flush;
  OutFile << "  | MainMCdev  Entering| "<<endl<<flush;
  OutFile << "  |--------------------| "<<endl<<flush;
  double NevTot =2e14; // Total (limit) of events, redefined below
  double NGroup =2e5;  // no. of events in the group, TO BE TUNED
  long   iLoop;        // external loop event counter
  long   iGroup;       // internal loop event counter
  double iEvent = 0;   // serial number of event in the MC generation
  //////////////////////////////////////////////////////////////////////////
  TString Status = "START";   // to be overwitten
  //------------------------------------------------------------------------
  SemafFile->cd();
  SemafFile->ls();
  TSemaf *Semafor;
  SemafFile->GetObject("Semafor", Semafor); /// read semafor from the disk
  if (!Semafor) return 2; // just a precaution
  Status   = Semafor->m_flag;
  NevTot   = Semafor->m_nevtot;
  NGroup   = Semafor->m_nevgrp;
  cout<<"||||||||||||||||||||||||||||||||||||||||||||||||||"<<endl;
  cout<<"||| MainMCdev:  Status  = "<<  Status << endl;
  cout<<"||| MainMCdev:  NevTot  = "<<  NevTot << endl;
  cout<<"||| MainMCdev:  NGroup  = "<<  NGroup << endl;
  Semafor->m_flag = "CONTINUE";
  Semafor->Write("Semafor",TObject::kOverwrite);      /// write modified semaphore
  SemafFile->Write("",TObject::kOverwrite,0);         /// write modified semaphore
  SemafFile->ls();
  SemafFile->Close();
  //--------------------------------------------------------------------------
  if( Status == "STOP") cout<<"MainPr: +++Warning+++ Status=STOP in semaphore"<<endl;
  //////////////////////////////////////////////////////////////////////////
  TRobol *RoboT = (TRobol*)MCgenFile.Get("RoboT");   /// read analysis object
  if (!RoboT) return 3; // just a precaution
  RoboT->Initialize(
        &OutFile,   /// Central log-file for messages
        &MCgenFile,  /// ROOT disk file for CRNG and MC gen.
        &HistoFile); /// ROOT disk file for histograms
  cout<< "||||||||||||||||||||||||||||||||||||||||||||||||||"<<endl;
  cout<< "||| MainMCdev: to be generated "<<NevTot<< " events "<<endl;
  cout<< "||||||||||||||||||||||||||||||||||||||||||||||||||"<<endl;
  //[[[[[[[[[[
  //  cout<<"------------------------------HistoFile.ls----------------------------------"<<endl;
  //  HistoFile.ls();
  //  cout<<"------------------------------HistoFile.ShowStreamerInfo---------------------------"<<endl;
  //  HistoFile.ShowStreamerInfo();
  //  exit(99);
  //]]]]]]]]]]
  ///***********************************************************************
  ///          MAIN LOOP OVER MONTE CARLO EVENTS
  for ( iLoop = 0; ; iLoop++)
    {
      for ( iGroup = 0; iGroup< NGroup; iGroup++) 
	{
	  iEvent++;
	  ///***************************
	  RoboT->Production(iEvent); //
	  ///***************************
	  if( iEvent  >= NevTot )  break;  // production stopped
	}; // for iGroup
      //
      cout    <<"iEvent = "<<iEvent<<endl<<flush;
      OutFile <<"iEvent = "<<iEvent<<endl<<flush;
      ///////////////////////////////////////////////////////////////
      // Disk Write for all histos and r.n. generator
      sprintf(chcyc,"*;%li",iLoop); Tcycle = chcyc;
      //cout<<"Main iLoop, Tcycle = "<< iLoop <<"|||"<< Tcycle <<"|||"<<endl;
      HistoFile.cd();
      HistoFile.Write();  //!!!
      HistoFile.Delete(Tcycle);
      HistoFile.Flush();
      //
      MCgenFile.cd();
      MCgenFile.Write();  //!!!
      MCgenFile.Delete(Tcycle);
      ///////////////////////////////////////////////////////////////
      // Checks semaphore for each group of NGroup events
      SemafFile = TFile::Open("semaf.root","UPDATE","Semaphor");
      if ((!SemafFile) || SemafFile->IsZombie()) return 1; // just a precaution
      SemafFile->GetObject("Semafor", Semafor);  // read semafor from the disk
      if (!Semafor) return 2; // just a precaution
      Status   = Semafor->m_flag;                   // get flag
      Semafor->m_nevent = iEvent;                   // record no. of events
      Semafor->Write("Semafor",TObject::kOverwrite);// write modified semaphore
      SemafFile->Write("",TObject::kOverwrite);
      SemafFile->Close();
      //-------------------------------------------------------------
      if( iEvent  >= NevTot )      break;  // production stoped
      if( Status == "STOP" )       break;  // production stoped
      ///////////////////////////////////////////////////////////////
    } // for iLoop
  ///            End of MC LOOP
  ///************************************************************************
  SemafFile = TFile::Open("semaf.root","UPDATE","Semaphor");
  if ((!SemafFile) || SemafFile->IsZombie()) return 1; // just a precaution
  SemafFile->GetObject("Semafor", Semafor);
  if (!Semafor) return 2; // just a precaution
  Semafor->m_flag = "CONTINUE";  
  Semafor->Write("Semafor",TObject::kOverwrite);// write modified semaphore
  SemafFile->Write("",TObject::kOverwrite);
  SemafFile->Close();
  cout<< "||||||||||||||||||||||||||||||||||||||||||||||"<<endl;
  cout<< "||  MainPr: Generated "<<iEvent<< " events    "<<endl;
  cout<< "||||||||||||||||||||||||||||||||||||||||||||||"<<endl;
  //
  RoboT->Finalize();
  //////////////////////////////////////////////////////////////////////
  //  LAST WRITEs
  MCgenFile.cd();
  RoboT->Write("RoboT",TObject::kOverwrite);  // only for tests
  //////////////////////////////////////////////////////////////////////
  //      Some TESTS and control printouts
  cout<<"------------------------------HistoFile.ls----------------------------------"<<endl;
  HistoFile.ls();
  //cout<<"------------------------------HistoFile.Map---------------------------------"<<endl;
  //HistoFile.Map();
  //cout<<"-------------------------MCgenFile.ShowStreamerInfo-------------------------"<<endl;
  //MCgenFile.ShowStreamerInfo();
  //cout<<"-------------------------HistoFile.ShowStreamerInfo-------------------------"<<endl;
  //HistoFile.ShowStreamerInfo();
#if 0 /* 0 or 1 */
  cout<<"------------------------HistoFile.GetListOfKeys-----------------------------"<<endl;
  HistoFile.GetListOfKeys()->Print();
  cout<<"------------------------MCgenFile.GetListOfKeys-----------------------------"<<endl;
  MCgenFile.GetListOfKeys()->Print();
  cout<<"-------------------------------SemafFile.ls---------------------------------"<<endl;
  SemafFile->ls();
#endif /* 0 or 1 */
  cout<<"---------------------------------end---------------------------------------"<<endl;
  //////////////////////////////////////////////////////////////////////
  //                 CLOSE ALL FILES
  HistoFile.Close();
  MCgenFile.Close();
  OutFile.close();
  cout    <<endl<<flush;
  cout    << "  |--------------------| "<<endl<<flush;
  cout    << "  |  MainMCdev  Ended  | "<<endl<<flush;
  cout    << "  |--------------------| "<<endl<<flush;
  OutFile <<endl<<flush;
  OutFile << "  |--------------------| "<<endl<<flush;
  OutFile << "  |  MainMCdev  Ended  | "<<endl<<flush;
  OutFile << "  |--------------------| "<<endl<<flush;
  return 0;
  }
