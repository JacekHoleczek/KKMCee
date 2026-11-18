// -*- C++ -*-
#include "Rivet/Analysis.hh"
#include "Rivet/Tools/Logging.hh"
#include "Rivet/Math/Constants.hh"
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/ChargedFinalState.hh"
#include "Rivet/Projections/FastJets.hh"
#include "Rivet/Projections/IdentifiedFinalState.hh"
#include "Rivet/Projections/VisibleFinalState.hh"
#include "Rivet/Projections/VetoedFinalState.hh"
#include "Rivet/Projections/MissingMomentum.hh"
#include "Rivet/Projections/LeadingParticlesFinalState.hh"
#include "Rivet/Projections/ZFinder.hh"


namespace Rivet {


  /// @brief Add a short analysis description here
  class leptons : public Analysis {
  public:

    /// Constructor
    double m_Emax, m_maxK0;
    leptons()
      : Analysis("kkmc-leptons")
    { }
    map<string, Histo1DPtr> histos;
    double _dR;
    /// Book histograms and initialise projections before the run
    void init() {
      vector<PdgId> lep_ids, neu_ids;
      lep_ids += PID::MUON;
      lep_ids += -PID::MUON;
      lep_ids += +PID::TAU;
      lep_ids += -PID::TAU;
      lep_ids += -5;
      lep_ids += 5;
      neu_ids += -14;
      neu_ids += 14;
      neu_ids += -16;
      neu_ids += 16;



      Cut cut = (Cuts::E > 0 * GeV && Cuts::pT > 0);

      IdentifiedFinalState Photons(Cuts::E > 0 * GeV);
      const IdentifiedFinalState lep(lep_ids);
      const IdentifiedFinalState neu(neu_ids);
      Photons.acceptId(22);
      // DressedLeptons Dressed_1(Photons, lep, 0.0, cut, 1);//, cut, 1);
      _dR=0.0;
      FinalState fs;
      // if (getOption("MODE")!=)
      ZFinder zfinder(fs, cut, PID::MUON, 0.0*GeV, 116.0*GeV, _dR, ZFinder::ClusterPhotons::NODECAY, ZFinder::AddPhotons::NO);

      declareProjection(lep, "lep");
      declareProjection(zfinder, "ZFinder");
      declareProjection(neu, "neutrinos");
      declareProjection(Photons, "bos");

 
      // Initialise and register projections
      declare(FinalState(), "FS");
      m_Emax   = sqrtS()/GeV;
      m_maxK0 = 0;
      int mass_bin = m_Emax/2;
      std::string ll = "\\ell\\bar{\\ell}";
      // // Book histograms
      book(histos["lep_mass"],"svar",mass_bin, -10,m_Emax*1.1);
      book(histos["lep_mass_peak"]  ,"lep-mass-peak",mass_bin,64,m_Emax);
      book(histos["pT"]         ,"pT",mass_bin, -10,m_Emax/2.+10);
      book(histos["Photon_pT"]  ,"Photon_pT",mass_bin, -10,m_Emax/2.+10);
      book(histos["lep_pT"]     ,"lep-pT",mass_bin, -10,m_Emax/2.+10);
      book(histos["lepbar_pT"]      ,"lepbar-pT",mass_bin, -10,m_Emax/2.+10);
      book(histos["lep_E"]      ,"lep-E",20, -10,m_Emax/2.+10);
      book(histos["lepbar_E"]       ,"lepbar-E",20, -10,m_Emax/2.+10);
      book(histos["coslep"]         ,"coslep",20,-1.2,1.2);
      book(histos["cosScatter"]     ,"cosScatter",40,-1.2,1.2);
      book(histos["coslepbar"]      ,"coslepbar",20,-1.2,1.2);

      book(histos["lep_px"]     ,"lep-px",20, -m_Emax/2.-10,m_Emax/2.+10);
      book(histos["lepbar_px"]      ,"lepbar-px",20, -m_Emax/2.-10,m_Emax/2.+10);
      book(histos["photon_px"]      ,"photon_x",20, -m_Emax/2.,m_Emax/2.);
      book(histos["photon_py"]      ,"photon_y",20, -m_Emax/2.,m_Emax/2.);
      book(histos["photon_pz"]      ,"photon_z",20, -m_Emax/2.,m_Emax/2.);
      

      book(histos["lep_py"]     ,"lep-py",20, -m_Emax/2.-10,m_Emax/2.+10);
      book(histos["lepbar_py"]      ,"lepbar-py",20, -m_Emax/2.-10,m_Emax/2.+10);


      book(histos["lep_pz"]     ,"lep-pz",20, -m_Emax/2.-10,m_Emax/2.+10);
      book(histos["lepbar_pz"]      ,"lepbar-pz",20, -m_Emax/2.-10,m_Emax/2.+10);
      book(histos["KKMC"]           ,"NGamma",21, -1,20);
      book(histos["photon_E"]       ,"Photon-Energy", mass_bin, -10, m_Emax+10);
      book(histos["xs"]       ,"xs", 1, 0, 1);
 
    }


    /// Perform the per-event analysis
    void analyze(const Event& evt) {
      Particles leptons;
      Particle lep;
      Particle lepbar;
      Particles photons;
      FourMomentum PhotonSum({0,0,0,0});
      const ZFinder& zfinder = apply<ZFinder>(evt, "ZFinder");
      if (zfinder.bosons().size() != 1) vetoEvent;
      for(auto & p: zfinder.constituents()) {
        leptons.push_back(p);
        if(p.pid()>0) lep=p;
        else lepbar=p;
      }
      FourMomentum lp, lm;

      if(leptons.size()!=2) vetoEvent;
      // correct for mistake in fortran
      if(leptons[0].charge() > 0){
        lep = leptons[0];
        lepbar = leptons[1];
      }
      else{
        lep = leptons[1];
        lepbar = leptons[0];
      }
      for( auto & p : applyProjection<IdentifiedFinalState>(evt, "bos").particlesByPt()) {
        if(p.pid()==22) photons.push_back(p);
        PhotonSum+=p.momentum();
        // if(p.E() < 0.001) vetoEvent;
      }
      for(auto & p: applyProjection<IdentifiedFinalState>(evt, "neutrinos").particlesByPt()) {
        leptons.push_back(p);
        if(p.pid()>0) lep=p;
        else lepbar=p;
      }
      // if(photons.size()==0) vetoEvent;
      // if(PhotonSum.E()>m_Emax*0.2) vetoEvent;
      if(PhotonSum.E() > m_maxK0) m_maxK0=PhotonSum.E();
      
      // double weight=evt.weight();
      // Particles leptons(lep_cand.begin(),lep_cand.end());
      if(leptons.size()==2){
        FourMomentum boson = leptons[0].momentum()+leptons[1].momentum();
       double pT= (leptons[0].momentum()+leptons[1].momentum()).pT();
       double pT1=(lep).pT();
       double pT2=(lepbar).pT();
       // if(pT1 < 20 || pT2 < 20) vetoEvent;
       double E1=lep.E();
       double E2=(lepbar).E();
       double mass=(leptons[0].momentum()+leptons[1].momentum()).mass();
       double coslep    = cos(lep.theta());
       coslep = lep.pz()/sqrt(sqr(lep.px())+sqr(lep.py())+sqr(lep.pz()));
       double coslepbar = cos(lepbar.theta());
       coslep = lep.pz()/sqrt(sqr(lep.px())+sqr(lep.py())+sqr(lep.pz()));
      double cosScat = boson.pz()/sqrt(sqr(boson.px())+sqr(boson.py())+sqr(boson.pz()));
       if(cosScat==cosScat){
          histos["cosScatter"]->fill(cosScat);
        }
       if(mass >= 64. || mass <= 120.)  histos["lep_mass_peak"]->fill(mass); 
       histos["lep_mass"]->fill(mass);
       histos["pT"] ->fill(pT);
       histos["lep_pT"] ->fill(pT1);
       histos["lepbar_pT"] ->fill(pT2);
       histos["lep_E"] ->fill(E1);
       histos["lepbar_E"] ->fill(E2);
       histos["coslep"]->fill(coslep);
       histos["coslepbar"]->fill(coslepbar);


       histos["lep_px"] ->fill(lep.momentum().px());
       histos["lepbar_px"] ->fill(lepbar.momentum().px());

       histos["lep_py"] ->fill(lep.momentum().py());
       histos["lepbar_py"] ->fill(lepbar.momentum().py());

       histos["lep_pz"] ->fill(lep.momentum().pz());
       histos["lepbar_pz"] ->fill(lepbar.momentum().pz());
       histos["KKMC"] ->fill(photons.size());
       // if(photons.size()!=0){
       histos["Photon_pT"]->fill(PhotonSum.pT());
       histos["photon_E"]->fill(PhotonSum.E());
       histos["photon_px"]->fill(PhotonSum.px());
       histos["photon_py"]->fill(PhotonSum.py());
       histos["photon_pz"]->fill(PhotonSum.pz());
       histos["xs"]->fill(0.5);
      // }
    }
  }


    /// Normalise histograms etc., after the run
    void finalize() {

      //      const double norm = 1.0;
      const double norm = crossSection()/sumOfWeights()/picobarn;
      for (std::map<string, Histo1DPtr>::iterator mit=histos.begin();
       mit!=histos.end();mit++) {
      Histo1DPtr histo = mit->second;
      scale(histo, norm);  
      // scale(histo, norm);
      } 
    }



  };


  // The hook for the plugin system
  DECLARE_RIVET_PLUGIN(leptons);


}
