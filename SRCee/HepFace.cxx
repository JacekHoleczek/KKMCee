///////////////////////////////////////////////////////////////////////////////
#include "HepFace.h"

ClassImp(HepFace);

// check energy conservation
void HepFace::MomentumCheck(){

double px = 0.0 , py = 0.0, pz = 0.0 , e = 0.0;

for (auto p : m_Hvent->particles()){
  if (p->status() == 1){
    HepMC3::FourVector m = p->momentum();
    cout << "px=" << m.px() << ", py=" << m.py() << ", pz=" << m.pz() << ", e=" << m.e() << endl;
    px += m.px();
    py += m.py(); 
    pz += m.pz();
    e += m.e();
   } //end if
  }
  cout << "=================== check energy conservation ====================" << endl;
  cout << "px=" << px << ", py=" << py << ", pz=" << pz << ", e=" << e << endl;
}

HepFace::HepFace()
{
  // This constructor is for ROOT streamers ONLY
  // all pointers has to be NULLed
  cout<< "----> HepFace Default Constructor (for ROOT only) "<<endl;
  m_Out= NULL;
  m_Event=NULL;
  m_Hvent=NULL;
  set_run_info=false;
  m_count = 0;
}

///_____________________________________________________________
HepFace::HepFace(ofstream *OutFile)
{
  cout<< "----> HepFace USER Constructor "<<endl;
  m_Out = OutFile;
  m_Event=NULL;
  m_Hvent=NULL;
  set_run_info=false;
  m_count = 0;
}//HepFace

///______________________________________________________________________________________
HepFace::~HepFace()
{
  //Explicit destructor
  cout<< "----> HepFace::HepFace !!!! DESTRUCTOR !!!! "<<endl;
}///destructor

double HepFace::sqr( const Double_t x ){ return x*x;};

///______________________________________________________________________________________
void HepFace::Initialize()
{
  cout  << "----> HepFace::Initialize, Entering "<<endl;
//=================================================================
// BX*** macros are in MCdev/BXFORMAT.h
  BXOPE(*m_Out);
  BXTXT(*m_Out,"========================================");
  BXTXT(*m_Out,"======    HepFace::Initialize     ======");
  BXTXT(*m_Out,"========================================");
  ///////////////////////////////////////////////////
  // clear tau container
  m_tauMdecay.clear();
  m_tauPdecay.clear();
  m_count=0;
}// Initialize

///______________________________________________________________________________________
void HepFace::WriteHEPC(const bool &multiwgt)
{
/////////////////////////////////////////////////////////////////////////////////////////
// This function fills in HEPMC3 event record with the initial beams,
// final fermions, all ISR and FSR photons.
// Note that association of photons in EEX/CEEX matrix elelent
// with the individual fermions is unphysical and should not be treated seriously.
/////////////////////////////////////////////////////////////////////////////////////////
  m_Hvent->clear();
  m_Hvent->set_event_number(m_count);
  if(!set_run_info){
    std::shared_ptr<GenRunInfo> runinfo = std::make_shared<GenRunInfo>();
    if(multiwgt) runinfo->set_weight_names({"Weight", "CEEX0", "CEEX1", "CEEX2"});
    else runinfo->set_weight_names({"Weight"});
    m_Hvent->set_run_info(runinfo);
    set_run_info = true;
  }
// status of outgoing fermions
  int status=1;                                 // stable mu, neutrina
  if(abs(m_Event->m_KFfin)==15) status=1;       // instable tau
  if(abs(m_Event->m_KFfin) >= 1 && abs(m_Event->m_KFfin)  < 5) status=1; // quarks
  // if(m_Event->m_WtMain == 0) return;
//----------------------------------
// beam particles  
  FourVector pe1_v4( Vect4( m_Event->m_Pf1) );
  FourVector pe2_v4( Vect4( m_Event->m_Pf2) );
//
  GenParticlePtr pe1 = std::make_shared<GenParticle>( pe1_v4, m_Event->m_KFini, 4);   
  GenParticlePtr pe2 = std::make_shared<GenParticle>( pe2_v4, - m_Event->m_KFini, 4); // - PDG id antifermion 
  pe1->set_generated_mass(pe1_v4.m());
  pe2->set_generated_mass(pe2_v4.m());
//-----------------------------------
// We construct one vertex with incoming electrons and  
// outgoing FSR and ISR photons and fermions 
  GenVertexPtr vertex = std::make_shared<GenVertex>();
// add incoming electrons to the vertex
  vertex->add_particle_in(pe1);
  vertex->add_particle_in(pe2);

// adding the ISR photons to vertex:
  for(int i=1; i <= m_Event->m_nPhotISR; ++i)
  {
      FourVector tmp_photon_v4=Vect4(m_Event->m_PhotISR[i]);
      GenParticlePtr tmp_photon=std::make_shared<GenParticle>(tmp_photon_v4, 22,1);
      tmp_photon->set_generated_mass(tmp_photon_v4.m());
      vertex->add_particle_out(tmp_photon);
  }

  for(int i=1; i<= m_Event->m_nPhotFSR; i++){
      // create a HEPMC photon
      FourVector tmp_photon_v4=FourVector(Vect4(m_Event->m_PhotFSR[i]));
      GenParticlePtr tmp_photon=std::make_shared<GenParticle>( tmp_photon_v4, 22, 1);
      tmp_photon->set_generated_mass(tmp_photon_v4.m());
      vertex->add_particle_out(tmp_photon);
  }


//-------------------------------------------------------------
// FSR photons:
// we will start from final state fermions
  FourVector pQ3_v4( Vect4( m_Event->m_Qf1) );
  FourVector pQ4_v4( Vect4( m_Event->m_Qf2) );
  GenParticlePtr pQ3 = std::make_shared<GenParticle>( pQ3_v4, m_Event->m_KFfin, status);
  GenParticlePtr pQ4 = std::make_shared<GenParticle>( pQ4_v4, - m_Event->m_KFfin, status);
  pQ3->set_generated_mass(pQ3_v4.m());
  pQ4->set_generated_mass(pQ4_v4.m());

  vertex->add_particle_out(pQ3);
  vertex->add_particle_out(pQ4);

  m_Hvent->add_vertex(vertex);
  WeightContainer wc;
  wc["Weight"]=m_Event->m_WtMain;
  if(multiwgt){
    wc["CEEX0"]=m_Event->m_WtEvent[1];
    wc["CEEX1"]=m_Event->m_WtEvent[2];
    wc["CEEX2"]=m_Event->m_WtEvent[3];
  }
  std::vector<std::string> names;
  std::vector<double> values;
  for (auto it = wc.begin(); it != wc.end(); ++it)
  { names.push_back(it->first); values.push_back(it->second);}
  if(names.size()!=values.size()) {
    cout<<"Error: Mismatch between weight names and values\n"
        <<"Size of Name vec: "<<names.size()<<std::endl
        <<"Size of Value vec: "<<values.size()<<std::endl;
  }
  m_Hvent->weights()=values;
  m_count++;
}//WriteHEPC


///______________________________________________________________________________________
void HepFace::FillHep3(int N, int IST, int ID, int JMO1, int JMO2, int JDA1, int JDA2, float P4[], float &PINV, bool PHFLAG)
{/////////////////////////////////////////////////////////////////////////////////////////
// This function is called inside fortran TAUOLA separately for every tau decay product
// It collects information on the decay products to be used in tauolaToHEPMC3
// for appending HEPMC3 event with tau decay products
/////////////////////////////////////////////////////////////////////////////////////////
// Create a HEPMC3 particle from a tau decay
  if(  IST==1){
  FourVector p_v4( Vect4(P4) );  // Create a HEPMC3 particle from a tau decay
  GenParticlePtr ptemp = std::make_shared<GenParticle>( p_v4, ID, IST);
//////////////////////
// calculating mass from 4-momentum of tauola is not precise
// for light leptons true mass is substituted in TauPair::Tralo4
  double mass = PINV;
  mass = p_v4.m();     // the same as PINV
  if( abs(ID) == 22 )  mass=0.0; // photon
  if( abs(ID) == 12 || abs(ID) == 14 || abs(ID) == 16)  mass=0.0; // neutrinos
  if( abs(ID) == 11 )  mass=0.51099907e-3; // electron
  if( abs(ID) == 13 )  mass=0.1056583e0;   // muon
  ptemp->set_generated_mass(mass);
//////////////////////
// store it in the vector which will be used later to fill the HEPMC record
  if (N == 1)
     m_tauMdecay.push_back(ptemp); // N = 1 is tau-
  else if (N == -1)
     m_tauPdecay.push_back(ptemp); // N = -1 is tau+
  else {
     cout <<"HepFace::FillHep3 something went wrong!!!!!!!!!!" << endl;
     cout<<"==================================FillHep3=============================================="<<endl;
     cout<<"   N= "<< N<<"   IST= "<< IST<<"   ID= "<< ID<<endl;
     cout<<"   JMO1= "<< JMO1<<"   JMO2= "<< JMO2<<"   JDA1= "<< JDA1<<"   JDA2= "<< JDA2<<endl;
     for(int i=0; i<4; i++) cout<<"  P4["<<i<<"]=  "<<P4[i]; cout<<endl;
     cout<<"  Mass PINV="<<PINV<< "   PHFLAG= "<<PHFLAG<<"   mass= "<<mass<<endl;
     cout<<"========================================================================================"<<endl;
     exit(99);
  }
  }//if IST
}//FillHep3


void HepFace::tauolaToHEPMC3(){
///////////////////////////////////////////////////////////////////////////////////
// tauolaToHEPMC3() after the tau decay is completed
// this function saves all tau decay products to HEPMC3 event m_Hvent
//////////////////////////////////////////////////////////////////////////////////
  GenVertexPtr tauPlus =   std::make_shared<GenVertex>();   
  GenVertexPtr tauMinus =  std::make_shared<GenVertex>();
  for (auto p: m_Hvent->particles()){
    if (p->pid() == 15 && p->status()==1){ // is ustable tau-
      p->set_status(2);
      tauMinus->add_particle_in(p);        // add tau to the vertex
      // add decay products of the tau to the vertex
      for (auto i : m_tauMdecay)  tauMinus->add_particle_out(i);
    } // end if tau -
    else if (p->pid() == -15 && p->status()==1 ){ // is unstable tau+
      p->set_status(2);
      tauPlus->add_particle_in(p);  // add tau to the vertex
      // add decay products of the tau to the vertex
      for (auto i : m_tauPdecay) tauPlus->add_particle_out(i);
     }// end if tau+
  }// end loop over particles
  m_Hvent->add_vertex(tauMinus); // add vertex to the event
  m_Hvent->add_vertex(tauPlus);
  // clear tau container
  m_tauMdecay.clear();
  m_tauPdecay.clear(); 
}//tauolaToHEPMC3
