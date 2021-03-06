#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <sstream>

#include "TFile.h" 
#include "TH1.h" 
#include "TH2.h"
#include "TGraph.h"
#include "TTree.h"
#include "TROOT.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TRFIOFile.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TChain.h"
#include "TMath.h"

#include "TLorentzVector.h"

#include "TRandom.h"

#include "DesyTauAnalyses/NTupleMaker/interface/Config.h"
#include "DesyTauAnalyses/NTupleMaker/interface/AC1B.h"

const float MuMass = 0.105658367;

int binNumber(float x, int nbins, float * bins) {

  int binN = 0;

  for (int iB=0; iB<nbins; ++iB) {
    if (x>=bins[iB]&&x<bins[iB+1]) {
      binN = iB;
      break;
    }
  }

  return binN;

}

float effBin(float x, int nbins, float * bins, float * eff) {

  int bin = binNumber(x, nbins, bins);

  return eff[bin];

}

double cosRestFrame(TLorentzVector boost, TLorentzVector vect) {

  double bx = -boost.Px()/boost.E();
  double by = -boost.Py()/boost.E();
  double bz = -boost.Pz()/boost.E();

  vect.Boost(bx,by,bz);
  double prod = -vect.Px()*bx-vect.Py()*by-vect.Pz()*bz;
  double modBeta = TMath::Sqrt(bx*bx+by*by+bz*bz); 
  double modVect = TMath::Sqrt(vect.Px()*vect.Px()+vect.Py()*vect.Py()+vect.Pz()*vect.Pz());
  
  double cosinus = prod/(modBeta*modVect);

  return cosinus;

}

double QToEta(double Q) {
  double Eta = - TMath::Log(TMath::Tan(0.5*Q));  
  return Eta;
}

double EtaToQ(double Eta) {
  double Q = 2.0*TMath::ATan(TMath::Exp(-Eta));
  if (Q<0.0) Q += TMath::Pi();
  return Q;
}

double PtoEta(double Px, double Py, double Pz) {

  double P = TMath::Sqrt(Px*Px+Py*Py+Pz*Pz);
  double cosQ = Pz/P;
  double Q = TMath::ACos(cosQ);
  double Eta = - TMath::Log(TMath::Tan(0.5*Q));  
  return Eta;

}

double PtoPhi(double Px, double Py) {
  return TMath::ATan2(Py,Px);
}

double PtoPt(double Px, double Py) {
  return TMath::Sqrt(Px*Px+Py*Py);
}

double dPhiFrom2P(double Px1, double Py1,
		  double Px2, double Py2) {


  double prod = Px1*Px2 + Py1*Py2;
  double mod1 = TMath::Sqrt(Px1*Px1+Py1*Py1);
  double mod2 = TMath::Sqrt(Px2*Px2+Py2*Py2);
  
  double cosDPhi = prod/(mod1*mod2);
  
  return TMath::ACos(cosDPhi);

}

double deltaEta(double Px1, double Py1, double Pz1,
		double Px2, double Py2, double Pz2) {

  double eta1 = PtoEta(Px1,Py1,Pz1);
  double eta2 = PtoEta(Px2,Py2,Pz2);

  double dEta = eta1 - eta2;

  return dEta;

}

double deltaR(double Eta1, double Phi1,
	      double Eta2, double Phi2) {

  double Px1 = TMath::Cos(Phi1);
  double Py1 = TMath::Sin(Phi1);

  double Px2 = TMath::Cos(Phi2);
  double Py2 = TMath::Sin(Phi2);

  double dPhi = dPhiFrom2P(Px1,Py1,Px2,Py2);
  double dEta = Eta1 - Eta2;

  double dR = TMath::Sqrt(dPhi*dPhi+dEta*dEta);

  return dR;

}

double PtEtaToP(double Pt, double Eta) {

  //  double Q = EtaToQ(Eta);

  //double P = Pt/TMath::Sin(Q);
  double P = Pt*TMath::CosH(Eta);

  return P;
}
double Px(double Pt, double Phi){

  double Px=Pt*TMath::Cos(Phi);
  return Px;
}
double Py(double Pt, double Phi){

  double Py=Pt*TMath::Sin(Phi);
  return Py;
}
double Pz(double Pt, double Eta){

  double Pz=Pt*TMath::SinH(Eta);
  return Pz;
}
double InvariantMass(double energy,double Px,double Py, double Pz){

  double M_2=energy*energy-Px*Px-Py*Py-Pz*Pz;
  double M=TMath::Sqrt(M_2);
  return M;


}
double EFromPandM0(double M0,double Pt,double Eta){

  double E_2=M0*M0+PtEtaToP(Pt,Eta)*PtEtaToP(Pt,Eta);
  double E =TMath::Sqrt(E_2);
  return E;

}
bool electronMvaIdTight(float eta, float mva) {

  float absEta = fabs(eta);

  bool passed = false;
  if (absEta<0.8) {
    if (mva>0.73) passed = true;
  }
  else if (absEta<1.479) {
    if (mva>0.57) passed = true;
  }
  else {
    if (mva>0.05) passed = true;
  }

  return passed;

}

bool puJetIdLoose(float eta, float mva) {

  float absEta = fabs(eta);
  bool id = false;

  if (eta<2.5) 
    id = mva>-0.63;
  else if (eta<2.75)
    id = mva>-0.60;
  else if (eta<3.0)
    id = mva>-0.55;
  else if (eta<5.2)
    id = mva>-0.45;

  return id;


}

const float electronMass = 0;
const float muonMass = 0.10565837;
const float pionMass = 0.1396;

int main(int argc, char * argv[]) {

  // first argument - config file 
  // second argument - filelist

  using namespace std;

  // **** configuration
  Config cfg(argv[1]);

  // kinematic cuts on electrons
  const float ptElectronLowCut   = cfg.get<float>("ptElectronLowCut");
  const float ptElectronHighCut  = cfg.get<float>("ptElectronHighCut");
  const float etaElectronCut     = cfg.get<float>("etaElectronCut");
  const float dxyElectronCut     = cfg.get<float>("dxyElectronCut");
  const float dzElectronCut      = cfg.get<float>("dzElectronCut");
  const float isoElectronLowCut  = cfg.get<float>("isoElectronLowCut");
  const float isoElectronHighCut = cfg.get<float>("isoElectronHighCut");
  const bool applyElectronId     = cfg.get<bool>("ApplyElectronId");

  // kinematic cuts on muons
  const float ptMuonLowCut   = cfg.get<float>("ptMuonLowCut");
  const float ptMuonHighCut  = cfg.get<float>("ptMuonHighCut");
  const float etaMuonCut     = cfg.get<float>("etaMuonCut");
  const float dxyMuonCut     = cfg.get<float>("dxyMuonCut");
  const float dzMuonCut      = cfg.get<float>("dzMuonCut");
  const float isoMuonLowCut  = cfg.get<float>("isoMuonLowCut");
  const float isoMuonHighCut = cfg.get<float>("isoMuonHighCut");
  const bool applyMuonId     = cfg.get<bool>("ApplyMuonId");

  // vertex cuts
  const float ndofVertexCut  = cfg.get<float>("NdofVertexCut");   
  const float zVertexCut     = cfg.get<float>("ZVertexCut");
  const float dVertexCut     = cfg.get<float>("DVertexCut");

  // topological cuts
  const float dRleptonsCut   = cfg.get<float>("dRleptonsCut");
  const float dZetaCut       = cfg.get<float>("dZetaCut");
  const bool oppositeSign    = cfg.get<bool>("oppositeSign");

  const float deltaRTrigMatch = cfg.get<float>("DRTrigMatch");
  const bool isIsoR03 = cfg.get<bool>("IsIsoR03");
  
  const float jetEtaCut = cfg.get<float>("JetEtaCut");
  const float jetPtLowCut = cfg.get<float>("JetPtLowCut");
  const float jetPtHighCut = cfg.get<float>("JetPtHighCut");
  const float dRJetLeptonCut = cfg.get<float>("dRJetLeptonCut");
  const bool applyJetPfId = cfg.get<bool>("ApplyJetPfId");
  const bool applyJetPuId = cfg.get<bool>("ApplyJetPuId");

  const float bJetEtaCut = cfg.get<float>("bJetEtaCut");
  const float btagCut = cfg.get<float>("btagCut");

  // check overlap
  const bool checkOverlap = cfg.get<bool>("CheckOverlap");

  // **** end of configuration

  // file name and tree name
  std::string rootFileName(argv[2]);
  std::ifstream fileList(argv[2]);
  std::ifstream fileList0(argv[2]);
  std::string ntupleName("makeroottree/AC1B");

  TString TStrName(rootFileName);
  std::cout <<TStrName <<std::endl;  

  // output fileName with histograms
  TFile * file = new TFile(TStrName+TString(".root"),"recreate");
  file->cd("");

  TH1F * inputEventsH = new TH1F("inputEventsH","",1,-0.5,0.5);
  TTree * tree = new TTree("TauCheck","TauCheck");
   // Declaration of leaf types

  Int_t           run;
  Int_t           lumi;
  Int_t           evt;
  Int_t           npv;
  Int_t           npu;
  Float_t         rho;
  Float_t         mcweight;
  Float_t         puweight;
  Float_t         trigweight_1;
  Float_t         trigweight_2;
  Float_t         idweight_1;
  Float_t         idweight_2;
  Float_t         isoweight_1;
  Float_t         isoweight_2;
  Float_t         effweight;
  Float_t         fakeweight;
  Float_t         embeddedWeight;
  Float_t         signalWeight;
  Float_t         weight;
  Float_t         m_vis;
  Float_t         m_sv;
  Float_t         pt_sv;
  Float_t         eta_sv;
  Float_t         phi_sv;
  Float_t         pt_1;
  Float_t         phi_1;
  Float_t         eta_1;
  Float_t         m_1;
  Int_t           q_1;
  Float_t         iso_1;
  Float_t         mva_1;
  Float_t         d0_1;
  Float_t         dZ_1;
  Float_t         mt_1;
  Float_t         pt_2;
  Float_t         phi_2;
  Float_t         eta_2;
  Float_t         m_2;
  Int_t           q_2;
  Float_t         iso_2;
  Float_t         d0_2;
  Float_t         dZ_2;
  Float_t         mva_2;
  Float_t         mt_2;

  Char_t          os;
  Char_t          dilepton_veto;
  Char_t          extraelec_veto;
  Char_t          extramuon_veto;

  Float_t         byCombinedIsolationDeltaBetaCorrRaw3Hits_1;
  Float_t         againstElectronLooseMVA5_1;
  Float_t         againstElectronMediumMVA5_1;
  Float_t         againstElectronTightMVA5_1;
  Float_t         againstElectronVLooseMVA5_1;
  Float_t         againstElectronVTightMVA5_1;
  Float_t         againstMuonLoose3_1;
  Float_t         againstMuonTight3_1;
  Float_t         byCombinedIsolationDeltaBetaCorrRaw3Hits_2;
  Float_t         againstElectronLooseMVA5_2;
  Float_t         againstElectronMediumMVA5_2;
  Float_t         againstElectronTightMVA5_2;
  Float_t         againstElectronVLooseMVA5_2;
  Float_t         againstElectronVTightMVA5_2;
  Float_t         againstMuonLoose3_2;
  Float_t         againstMuonTight3_2;

  Float_t         met;
  Float_t         metphi;
  Float_t         metcov00;
  Float_t         metcov01;
  Float_t         metcov10;
  Float_t         metcov11;

  Float_t         mvamet;
  Float_t         mvametphi;
  Float_t         mvacov00;
  Float_t         mvacov01;
  Float_t         mvacov10;
  Float_t         mvacov11;

  Float_t         pt_tt;
  Float_t         pzetavis;
  Float_t         pzetamiss;
  Float_t         mva_gf;

  Int_t           njets;
  Int_t           njetspt20;
  Float_t         jpt_1;
  Float_t         jeta_1;
  Float_t         jphi_1;
  Float_t         jptraw_1;
  Float_t         jptunc_1;
  Float_t         jmva_1;
  Float_t         jlrm_1;
  Int_t           jctm_1;
  Float_t         jpt_2;
  Float_t         jeta_2;
  Float_t         jphi_2;
  Float_t         jptraw_2;
  Float_t         jptunc_2;
  Float_t         jmva_2;
  Float_t         jlrm_2;
  Int_t           jctm_2;
  Float_t         mjj;
  Float_t         jdeta;
  Int_t           njetingap;

  Int_t           nbtag;
  Float_t         bpt;
  Float_t         beta;
  Float_t         bphi;
  

  tree->Branch("run", &run, "run/I");
  tree->Branch("lumi", &lumi, "lumi/I");
  tree->Branch("evt", &evt, "evt/I");
  tree->Branch("npv", &npv, "npv/I");
  tree->Branch("npu", &npu, "npu/I");
  tree->Branch("rho", &rho, "rho/F");

  tree->Branch("mcweight", &mcweight, "mcweight/F");
  tree->Branch("puweight", &puweight, "puweight/F");
  tree->Branch("trigweight_1", &trigweight_1, "trigweight_1/F");
  tree->Branch("trigweight_2", &trigweight_2, "trigweight_2/F");
  tree->Branch("idweight_1", &idweight_1, "idweight_1/F");
  tree->Branch("idweight_2", &idweight_2, "idweight_2/F");
  tree->Branch("isoweight_1", &isoweight_1, "isoweight_1/F");
  tree->Branch("isoweight_2", &isoweight_2, "isoweight_2/F");
  tree->Branch("effweight", &effweight, "effweight/F");
  tree->Branch("fakeweight", &fakeweight, "fakeweight/F");
  tree->Branch("embeddedWeight", &embeddedWeight, "embeddedWeight/F");
  tree->Branch("signalWeight", &signalWeight, "signalWeight/F");
  tree->Branch("weight", &weight, "weight/F");

  tree->Branch("m_vis", &m_vis, "m_vis/F");
  tree->Branch("m_sv", &m_sv, "m_sv/F");
  tree->Branch("pt_sv", &pt_sv, "pt_sv/F");
  tree->Branch("eta_sv", &eta_sv, "eta_sv/F");
  tree->Branch("phi_sv", &phi_sv, "phi_sv/F");

  tree->Branch("pt_1", &pt_1, "pt_1/F");
  tree->Branch("phi_1", &phi_1, "phi_1/F");
  tree->Branch("eta_1", &eta_1, "eta_1/F");
  tree->Branch("m_1", &m_1, "m_1/F");
  tree->Branch("q_1", &q_1, "q_1/I");
  tree->Branch("iso_1", &iso_1, "iso_1/F");
  tree->Branch("mva_1", &mva_1, "mva_1/F");
  tree->Branch("d0_1", &d0_1, "d0_1/F");
  tree->Branch("dZ_1", &dZ_1, "dZ_1/F");
  tree->Branch("mt_1", &mt_1, "mt_1/F");

  tree->Branch("pt_2", &pt_2, "pt_2/F");
  tree->Branch("phi_2", &phi_2, "phi_2/F");
  tree->Branch("eta_2", &eta_2, "eta_2/F");
  tree->Branch("m_2", &m_2, "m_2/F");
  tree->Branch("q_2", &q_2, "q_2/I");
  tree->Branch("iso_2", &iso_2, "iso_2/F");
  tree->Branch("d0_2", &d0_2, "d0_2/F");
  tree->Branch("dZ_2", &dZ_2, "dZ_2/F");
  tree->Branch("mva_2", &mva_2, "mva_2/F");
  tree->Branch("mt_2", &mt_2, "mt_2/F");

  tree->Branch("os", &os, "os/C");
  tree->Branch("dilepton_veto", &dilepton_veto, "dilepton_veto/C");
  tree->Branch("extraelec_veto", &extraelec_veto, "extraelec_veto/C");
  tree->Branch("extramuon_veto", &extramuon_veto, "extramuon_veto/C");

  tree->Branch("byCombinedIsolationDeltaBetaCorrRaw3Hits_1", &byCombinedIsolationDeltaBetaCorrRaw3Hits_1, "byCombinedIsolationDeltaBetaCorrRaw3Hits_1/F");
  tree->Branch("againstElectronLooseMVA5_1", &againstElectronLooseMVA5_1, "againstElectronLooseMVA5_1/F");
  tree->Branch("againstElectronMediumMVA5_1", &againstElectronMediumMVA5_1, "againstElectronMediumMVA5_1/F");
  tree->Branch("againstElectronTightMVA5_1", &againstElectronTightMVA5_1, "againstElectronTightMVA5_1/F");
  tree->Branch("againstElectronVLooseMVA5_1", &againstElectronVLooseMVA5_1, "againstElectronVLooseMVA5_1/F");
  tree->Branch("againstElectronVTightMVA5_1", &againstElectronVTightMVA5_1, "againstElectronVTightMVA5_1/F");
  tree->Branch("againstMuonLoose3_1", &againstMuonLoose3_1, "againstMuonLoose3_1/F");
  tree->Branch("againstMuonTight3_1", &againstMuonTight3_1, "againstMuonTight3_1/F");

  tree->Branch("byCombinedIsolationDeltaBetaCorrRaw3Hits_2", &byCombinedIsolationDeltaBetaCorrRaw3Hits_2, "byCombinedIsolationDeltaBetaCorrRaw3Hits_2/F");
  tree->Branch("againstElectronLooseMVA5_2", &againstElectronLooseMVA5_2, "againstElectronLooseMVA5_2/F");
  tree->Branch("againstElectronMediumMVA5_2", &againstElectronMediumMVA5_2, "againstElectronMediumMVA5_2/F");
  tree->Branch("againstElectronTightMVA5_2", &againstElectronTightMVA5_2, "againstElectronTightMVA5_2/F");
  tree->Branch("againstElectronVLooseMVA5_2", &againstElectronVLooseMVA5_2, "againstElectronVLooseMVA5_2/F");
  tree->Branch("againstElectronVTightMVA5_2", &againstElectronVTightMVA5_2, "againstElectronVTightMVA5_2/F");
  tree->Branch("againstMuonLoose3_2", &againstMuonLoose3_2, "againstMuonLoose3_2/F");
  tree->Branch("againstMuonTight3_2", &againstMuonTight3_2, "againstMuonTight3_2/F");

  tree->Branch("met", &met, "met/F");
  tree->Branch("metphi", &metphi, "metphi/F");
  tree->Branch("metcov00", &metcov00, "metcov00/F");
  tree->Branch("metcov01", &metcov01, "metcov01/F");
  tree->Branch("metcov10", &metcov10, "metcov10/F");
  tree->Branch("metcov11", &metcov11, "metcov11/F");

  tree->Branch("mvamet", &mvamet, "mvamet/F");
  tree->Branch("mvametphi", &mvametphi, "mvametphi/F");
  tree->Branch("mvacov00", &mvacov00, "mvacov00/F");
  tree->Branch("mvacov01", &mvacov01, "mvacov01/F");
  tree->Branch("mvacov10", &mvacov10, "mvacov10/F");
  tree->Branch("mvacov11", &mvacov11, "mvacov11/F");

  tree->Branch("pt_tt", &pt_tt, "pt_tt/F");
  tree->Branch("pzetavis", &pzetavis, "pzetavis/F");
  tree->Branch("pzetamiss", &pzetamiss, "pzetamiss/F");
  tree->Branch("mva_gf", &mva_gf, "mva_gf/F");

  tree->Branch("njets", &njets, "njets/I");
  tree->Branch("njetspt20", &njetspt20, "njetspt20/I");

  tree->Branch("jpt_1", &jpt_1, "jpt_1/F");
  tree->Branch("jeta_1", &jeta_1, "jeta_1/F");
  tree->Branch("jphi_1", &jphi_1, "jphi_1/F");
  tree->Branch("jptraw_1", &jptraw_1, "jptraw_1/F");
  tree->Branch("jptunc_1", &jptunc_1, "jptunc_1/F");
  tree->Branch("jmva_1", &jmva_1, "jmva_1/F");
  tree->Branch("jlrm_1", &jlrm_1, "jlrm_1/F");
  tree->Branch("jctm_1", &jctm_1, "jctm_1/I");

  tree->Branch("jpt_2", &jpt_2, "jpt_2/F");
  tree->Branch("jeta_2", &jeta_2, "jeta_2/F");
  tree->Branch("jphi_2", &jphi_2, "jphi_2/F");
  tree->Branch("jptraw_2", &jptraw_2, "jptraw_2/F");
  tree->Branch("jptunc_2", &jptunc_2, "jptunc_2/F");
  tree->Branch("jmva_2", &jmva_2, "jlrm_2/F");
  tree->Branch("jlrm_2", &jlrm_2, "jlrm_2/F");
  tree->Branch("jctm_2", &jctm_2, "jctm_2/I");

  tree->Branch("mjj", &mjj, "mjj/F");
  tree->Branch("jdeta", &jdeta, "jdeta/F");
  tree->Branch("njetingap", &njetingap, "njetingap/I");

  tree->Branch("nbtag", &nbtag, "nbtag/I");
  tree->Branch("bpt", &bpt, "bpt/F");
  tree->Branch("beta", &beta, "beta/F");
  tree->Branch("bphi", &bphi, "bphi/F");
  
  int nTotalFiles = 0;
  std::string dummy;
  // count number of files --->
  while (fileList0 >> dummy) nTotalFiles++;

  int nEvents = 0;
  int selEvents = 0;
  int nFiles = 0;
  
  vector<int> runList; runList.clear();
  vector<int> eventList; eventList.clear();

  int nonOverlap = 0;

  std::ifstream fileEvents("overlap.txt");
  int Run, Event, Lumi;
  if (checkOverlap) {
    std::cout << "Non-overlapping events ->" << std::endl;
    while (fileEvents >> Run >> Event >> Lumi) {
      runList.push_back(Run);
      eventList.push_back(Event);
      std::cout << Run << ":" << Event << std::endl;
    }
    std::cout << std::endl;
  }
  std::ofstream fileOutput("overlap.out");


  for (int iF=0; iF<nTotalFiles; ++iF) {

    std::string filen;
    fileList >> filen;

    std::cout << "file " << iF+1 << " out of " << nTotalFiles << " filename : " << filen << std::endl;
    TFile * file_ = TFile::Open(TString(filen));
    
    TTree * _tree = NULL;
    _tree = (TTree*)file_->Get(TString(ntupleName));
  
    if (_tree==NULL) continue;
    
    TH1D * histoInputEvents = NULL;
   
    histoInputEvents = (TH1D*)file_->Get("makeroottree/nEvents");
    
    if (histoInputEvents==NULL) continue;
    
    int NE = int(histoInputEvents->GetEntries());
    
    std::cout << "      number of input events    = " << NE << std::endl;
    
    for (int iE=0;iE<NE;++iE)
      inputEventsH->Fill(0.);

    AC1B analysisTree(_tree);
    
    Long64_t numberOfEntries = analysisTree.GetEntries();
    
    std::cout << "      number of entries in Tree = " << numberOfEntries << std::endl;
    
    for (Long64_t iEntry=0; iEntry<numberOfEntries; iEntry++) { 
    
      analysisTree.GetEntry(iEntry);
      nEvents++;
      
      if (nEvents%10000==0) 
	cout << "      processed " << nEvents << " events" << endl; 

      run = int(analysisTree.event_run);
      lumi = int(analysisTree.event_luminosityblock);
      evt = int(analysisTree.event_nr);

      bool overlapEvent = true;
      for (unsigned int iEvent=0; iEvent<runList.size(); ++iEvent) {
	if (runList.at(iEvent)==run&&eventList.at(iEvent)==evt) {
	  overlapEvent = false;
	  
	}
      }

      if (overlapEvent&&checkOverlap) continue;
      nonOverlap++;

      if (checkOverlap) {
	fileOutput << std::endl;
	fileOutput << "Run = " << run << "   Event = " << evt << std::endl;
      }

      // weights
      mcweight = 0;
      puweight = 0;
      trigweight_1 = 0;
      trigweight_2 = 0;
      idweight_1 = 0;
      idweight_2 = 0;
      isoweight_1 = 0;
      isoweight_2 = 0;
      effweight = 0;
      fakeweight = 0;
      embeddedWeight = 0;
      signalWeight = 0;
      weight = 1;
      
      npv = analysisTree.primvertex_count;
      npu = analysisTree.numpileupinteractions;
      rho = analysisTree.rho;

      // vertex cuts
      if (fabs(analysisTree.primvertex_z)>zVertexCut) continue;
      if (analysisTree.primvertex_ndof<ndofVertexCut) continue;
      float dVertex = (analysisTree.primvertex_x*analysisTree.primvertex_x+
		       analysisTree.primvertex_y*analysisTree.primvertex_y);
      if (dVertex>dVertexCut) continue;

      if (checkOverlap)
	fileOutput << "Vertex cuts are passed " << std::endl;


      //                         HLT_Mu23_Ele12                     ||      HLT_Mu8_Ele23       
      //      bool trigAccept = (analysisTree.hltriggerresults_second[5]==1)||(analysisTree.hltriggerresults_second[6]==1);
      //      if (!trigAccept) continue;
      
      // electron selection
      vector<int> electrons; electrons.clear();
      if (checkOverlap)
	fileOutput << "# electrons = " << analysisTree.electron_count << std::endl;
      for (unsigned int ie = 0; ie<analysisTree.electron_count; ++ie) {
	bool electronMvaId = electronMvaIdTight(analysisTree.electron_superclusterEta[ie],
						analysisTree.electron_mva_id_nontrigPhys14[ie]);
	if (checkOverlap)
	  fileOutput << "  " << ie 
		     << " pt = " << analysisTree.electron_pt[ie] 
		     << " eta = " << analysisTree.electron_eta[ie]
		     << " dxy = " << analysisTree.electron_dxy[ie]
		     << " dz  = " << analysisTree.electron_dz[ie]
		     << " passConv = " << analysisTree.electron_pass_conversion[ie]
		     << " nmisshits = " << int(analysisTree.electron_nmissinginnerhits[ie])
		     << " mvaTight = " << electronMvaId << std::endl;
	if (analysisTree.electron_pt[ie]<ptElectronLowCut) continue;
	if (fabs(analysisTree.electron_eta[ie])>etaElectronCut) continue;
	if (fabs(analysisTree.electron_dxy[ie])>dxyElectronCut) continue;
	if (fabs(analysisTree.electron_dz[ie])>dzElectronCut) continue;
	if (!electronMvaId&&applyElectronId) continue;
	if (!analysisTree.electron_pass_conversion[ie]&&applyElectronId) continue;
	if (analysisTree.electron_nmissinginnerhits[ie]!=0&&applyElectronId) continue;
	electrons.push_back(ie);
      }

      // muon selection
      if (checkOverlap)
	fileOutput << "# muons = " << analysisTree.muon_count << std::endl;
      vector<int> muons; muons.clear();
      for (unsigned int im = 0; im<analysisTree.muon_count; ++im) {
	bool isGoodGlobal = analysisTree.muon_isGlobal[im] && analysisTree.muon_normChi2[im]<3 
	  && analysisTree.muon_combQ_chi2LocalPosition[im]<12 && analysisTree.muon_combQ_trkKink[im]<20;
	bool isMedium = analysisTree.muon_isLoose[im] && analysisTree.muon_validFraction[im]>0.8 
	  && analysisTree.muon_segmentComp[im] > (isGoodGlobal ? 0.303 : 0.451);
	if (checkOverlap)
	  fileOutput << "  " << im 
		     << " pt = " << analysisTree.muon_pt[im] 
		     << " eta = " << analysisTree.muon_eta[im]
		     << " dxy = " << analysisTree.muon_dxy[im]
		     << " dz  = " << analysisTree.muon_dz[im]
		     << " medium = " << isMedium 
		     << " goodGlobal = " << isGoodGlobal 
		     << " looseMuon = " << analysisTree.muon_isLoose[im]
		     << " isGlobal = " << analysisTree.muon_isGlobal[im]
		     << " normChi2 = " << analysisTree.muon_normChi2[im]
		     << " chi2LocalPos = " << analysisTree.muon_combQ_chi2LocalPosition[im]
		     << " trkKink = " << analysisTree.muon_combQ_trkKink[im]
		     << " validFraction = " << analysisTree.muon_validFraction[im] 
		     << " segmentComp = " << analysisTree.muon_segmentComp[im] << std::endl;
	if (analysisTree.muon_pt[im]<ptMuonLowCut) continue;
	if (fabs(analysisTree.muon_eta[im])>etaMuonCut) continue;
	if (fabs(analysisTree.muon_dxy[im])>dxyMuonCut) continue;
	if (fabs(analysisTree.muon_dz[im])>dzMuonCut) continue;
	if (applyMuonId && !isMedium) continue;
	muons.push_back(im);
      }

      if (checkOverlap) {
	fileOutput << "# selected electrons = " << electrons.size() << std::endl;
	fileOutput << "# selected muons     = " << muons.size() << std::endl;
      }

      if (electrons.size()==0) continue;
      if (muons.size()==0) continue;
      
      // selecting muon and electron pair (OS or SS);
      int electronIndex = -1;
      int muonIndex = -1;

      float isoMuMin = 1e+10;
      float isoEleMin = 1e+10;
      for (unsigned int im=0; im<muons.size(); ++im) {
	bool isMu23 = false;
	bool isMu8 = false;
	unsigned int mIndex  = muons.at(im);
	float neutralHadIsoMu = analysisTree.muon_neutralHadIso[mIndex];
	float photonIsoMu = analysisTree.muon_photonIso[mIndex];
	float chargedHadIsoMu = analysisTree.muon_chargedHadIso[mIndex];
	float puIsoMu = analysisTree.muon_puIso[mIndex];
	if (isIsoR03) {
	  neutralHadIsoMu = analysisTree.muon_r03_sumNeutralHadronEt[mIndex];
	  photonIsoMu = analysisTree.muon_r03_sumPhotonEt[mIndex];
	  chargedHadIsoMu = analysisTree.muon_r03_sumChargedHadronPt[mIndex];
	  puIsoMu = analysisTree.muon_r03_sumPUPt[mIndex];
	}
	float neutralIsoMu = neutralHadIsoMu + photonIsoMu - 0.5*puIsoMu;
	neutralIsoMu = TMath::Max(float(0),neutralIsoMu); 
	float absIsoMu = chargedHadIsoMu + neutralIsoMu;
	float relIsoMu = absIsoMu/analysisTree.muon_pt[mIndex];
	for (unsigned int iT=0; iT<analysisTree.trigobject_count; ++iT) {
	  if (analysisTree.trigobject_filters[iT][3]) { // Mu23 Leg
	    float dRtrig = deltaR(analysisTree.muon_eta[mIndex],analysisTree.muon_phi[mIndex],
				  analysisTree.trigobject_eta[iT],analysisTree.trigobject_phi[iT]);
	    if (dRtrig<deltaRTrigMatch) {
	      isMu23 = true;
	    }
	  }
	  if (analysisTree.trigobject_filters[iT][4]) { // Mu8 Leg
	    float dRtrig = deltaR(analysisTree.muon_eta[mIndex],analysisTree.muon_phi[mIndex],
				  analysisTree.trigobject_eta[iT],analysisTree.trigobject_phi[iT]);
	    if (dRtrig<deltaRTrigMatch) {
	      isMu8 = true;
	    }
	  }
	}
	if (checkOverlap)
	  fileOutput << "Muon " << im << " -> isMu23 = " << isMu23 << " ; isMu8 = " << isMu23 << std::endl;

	if ((!isMu23) && (!isMu8)) continue;

	for (unsigned int ie=0; ie<electrons.size(); ++ie) {

	  unsigned int eIndex = electrons.at(ie);

	  float dR = deltaR(analysisTree.electron_eta[eIndex],analysisTree.electron_phi[eIndex],
			    analysisTree.muon_eta[mIndex],analysisTree.muon_phi[mIndex]);

	  if (checkOverlap)
	    fileOutput << "dR(mu,e) = " << dR << std::endl;

	  if (dR<dRleptonsCut) continue;

	  bool isEle23 = false;
	  bool isEle12 = false;

	  for (unsigned int iT=0; iT<analysisTree.trigobject_count; ++iT) {
	    if (analysisTree.trigobject_filters[iT][9]) { // Ele23 Leg
	      float dRtrig = deltaR(analysisTree.electron_eta[eIndex],analysisTree.electron_phi[eIndex],
				    analysisTree.trigobject_eta[iT],analysisTree.trigobject_phi[iT]);
	      if (dRtrig<deltaRTrigMatch) {
		isEle23 = true;
	      }
	    }
	  }

	  for (unsigned int iT=0; iT<analysisTree.trigobject_count; ++iT) {
	    if (analysisTree.trigobject_filters[iT][8]) { // Ele12 Leg
	      float dRtrig = deltaR(analysisTree.electron_eta[eIndex],analysisTree.electron_phi[eIndex],
				    analysisTree.trigobject_eta[iT],analysisTree.trigobject_phi[iT]);
	      if (dRtrig<deltaRTrigMatch) {
		isEle12 = true;
	      }
	    }
	  }

	  if (checkOverlap)
	    fileOutput << "Electron " << ie << " -> isEle23 = " << isEle23 << " ; isEle12 = " << isEle12 << std::endl;

	  bool trigMatch = (isMu23&&isEle12) || (isMu8&&isEle23);

	  if (!trigMatch) continue;
	  
	  bool isKinematicMatch = false;
	  if (isMu23&&isEle12) {
	    if (analysisTree.muon_pt[mIndex]>ptMuonHighCut&&analysisTree.electron_pt[eIndex]>ptElectronLowCut)
	      isKinematicMatch = true;
	  }
	  if (isMu8&&isEle23) {
            if (analysisTree.muon_pt[mIndex]>ptMuonLowCut&&analysisTree.electron_pt[eIndex]>ptElectronHighCut)
              isKinematicMatch = true;
          }
	  if (!isKinematicMatch) continue;
	  if (checkOverlap)
	    fileOutput << " pair is taken " << std::endl;

	  float neutralHadIsoEle = analysisTree.electron_neutralHadIso[eIndex];
	  float photonIsoEle = analysisTree.electron_photonIso[eIndex];
	  float chargedHadIsoEle = analysisTree.electron_chargedHadIso[eIndex];
	  float puIsoEle = analysisTree.electron_puIso[eIndex];
	  if (isIsoR03) {
	    neutralHadIsoEle = analysisTree.electron_r03_sumNeutralHadronEt[eIndex];
	    photonIsoEle = analysisTree.electron_r03_sumPhotonEt[eIndex];
	    chargedHadIsoEle = analysisTree.electron_r03_sumChargedHadronPt[eIndex];
	    puIsoEle = analysisTree.electron_r03_sumPUPt[eIndex];
	  }
	  float neutralIsoEle = neutralHadIsoEle + photonIsoEle - 0.5*puIsoEle;
	  neutralIsoEle = TMath::Max(float(0),neutralIsoEle); 
	  float absIsoEle =  chargedHadIsoEle + neutralIsoEle;
	  float relIsoEle = absIsoEle/analysisTree.electron_pt[eIndex];

	  if (int(mIndex)!=muonIndex) {
	    if (relIsoMu<isoMuMin) {
	      isoMuMin  = relIsoMu;
	      muonIndex = mIndex;
	      isoEleMin = relIsoEle;
	      electronIndex = eIndex;
	    }
	  }
	  else {
	    if (relIsoEle<isoEleMin) {
	      isoEleMin = relIsoEle;
	      electronIndex = eIndex;
	    }
	  }
	  
	}
      }

      if (electronIndex<0) continue;
      if (muonIndex<0) continue;

      // filling muon variables
      pt_2 = analysisTree.muon_pt[muonIndex];
      eta_2 = analysisTree.muon_eta[muonIndex];
      phi_2 = analysisTree.muon_phi[muonIndex];
      q_2 = -1;
      if (analysisTree.muon_charge[muonIndex]>0)
	q_2 = 1;
      mva_2 = -9999;
      d0_2 = analysisTree.muon_dxy[muonIndex];
      dZ_2 = analysisTree.muon_dz[muonIndex];
      iso_2 = isoMuMin;
      m_2 = muonMass;
      
      // filling muon variables
      pt_1 = analysisTree.electron_pt[electronIndex];
      eta_1 = analysisTree.electron_eta[electronIndex];
      phi_1 = analysisTree.electron_phi[electronIndex];
      q_1 = -1;
      if (analysisTree.electron_charge[electronIndex]>0)
        q_1 = 1;
      mva_1 = analysisTree.electron_mva_id_nontrigPhys14[electronIndex];
      d0_1 = analysisTree.electron_dxy[electronIndex];
      dZ_1 = analysisTree.electron_dz[electronIndex];
      iso_1 = isoEleMin;
      m_1 = electronMass;

      TLorentzVector muonLV; muonLV.SetXYZM(analysisTree.muon_px[muonIndex],
					    analysisTree.muon_py[muonIndex],
					    analysisTree.muon_pz[muonIndex],
					    muonMass);

      TLorentzVector electronLV; electronLV.SetXYZM(analysisTree.electron_px[electronIndex],
						    analysisTree.electron_py[electronIndex],
						    analysisTree.electron_pz[electronIndex],
						    electronMass);

      TLorentzVector dileptonLV = muonLV + electronLV;

      m_vis = dileptonLV.M();
      met = TMath::Sqrt(analysisTree.pfmet_ex*analysisTree.pfmet_ex + analysisTree.pfmet_ey*analysisTree.pfmet_ey);
      metphi = TMath::ATan2(analysisTree.pfmet_ey,analysisTree.pfmet_ex);
      metcov00 = analysisTree.pfmet_sigxx;
      metcov01 = analysisTree.pfmet_sigxy;
      metcov10 = analysisTree.pfmet_sigyx;
      metcov11 = analysisTree.pfmet_sigyy;

      // visible ditau pt 
      pt_tt = dileptonLV.Pt();

      // bisector of electron and muon transverse momenta
      float electronUnitX = electronLV.Px()/electronLV.Pt();
      float electronUnitY = electronLV.Py()/electronLV.Pt();
	
      float muonUnitX = muonLV.Px()/muonLV.Pt();
      float muonUnitY = muonLV.Py()/muonLV.Pt();

      float zetaX = electronUnitX + muonUnitX;
      float zetaY = electronUnitY + muonUnitY;
      
      float normZeta = TMath::Sqrt(zetaX*zetaX+zetaY*zetaY);

      zetaX = zetaX/normZeta;
      zetaY = zetaY/normZeta;

      // choosing mva met
      unsigned int metEMu = 0;
      for (unsigned int iMet=0; iMet<analysisTree.mvamet_count; ++iMet) {
	if (analysisTree.mvamet_channel[iMet]==1) metEMu = iMet;
      }
      float mvamet_x = analysisTree.mvamet_ex[metEMu];
      float mvamet_y = analysisTree.mvamet_ey[metEMu];
      float mvamet_x2 = mvamet_x * mvamet_x;
      float mvamet_y2 = mvamet_y * mvamet_y;

      mvamet = TMath::Sqrt(mvamet_x2+mvamet_y2);
      mvametphi = TMath::ATan2(mvamet_y,mvamet_x);
      mvacov00 = analysisTree.mvamet_sigxx[metEMu];
      mvacov01 = analysisTree.mvamet_sigxy[metEMu];
      mvacov10 = analysisTree.mvamet_sigyx[metEMu];
      mvacov11 = analysisTree.mvamet_sigyy[metEMu];

      float vectorX = analysisTree.pfmet_ex + muonLV.Px() + electronLV.Px();
      float vectorY = analysisTree.pfmet_ey + muonLV.Py() + electronLV.Py();
      
      float vectorVisX = muonLV.Px() + electronLV.Px();
      float vectorVisY = muonLV.Py() + electronLV.Py();

      // computation of DZeta variable
      pzetamiss = analysisTree.pfmet_ex*zetaX + analysisTree.pfmet_ey*zetaY;
      pzetavis  = vectorVisX*zetaX + vectorVisY*zetaY;

      // counting jets
      vector<unsigned int> jets; jets.clear();
      vector<unsigned int> jetspt20; jetspt20.clear();
      vector<unsigned int> bjets; bjets.clear();

      int indexLeadingJet = -1;
      float ptLeadingJet = -1;

      int indexSubLeadingJet = -1;
      float ptSubLeadingJet = -1;
      
      int indexLeadingBJet = -1;
      float ptLeadingBJet = -1;

      for (unsigned int jet=0; jet<analysisTree.pfjet_count; ++jet) {

	float absJetEta = fabs(analysisTree.pfjet_eta[jet]);
	if (absJetEta>jetEtaCut) continue;

	float jetPt = analysisTree.pfjet_pt[jet];
	if (jetPt<jetPtLowCut) continue;

	float dR1 = deltaR(analysisTree.pfjet_eta[jet],analysisTree.pfjet_phi[jet],
			   eta_1,phi_1);
	if (dR1<dRJetLeptonCut) continue;

	float dR2 = deltaR(analysisTree.pfjet_eta[jet],analysisTree.pfjet_phi[jet],
                           eta_2,phi_2);
        if (dR2<dRJetLeptonCut) continue;

	// pf jet Id
	float energy = analysisTree.pfjet_e[jet];
        float chf = analysisTree.pfjet_chargedhadronicenergy[jet]/energy;
        float nhf = analysisTree.pfjet_neutralhadronicenergy[jet]/energy;
        float phf = analysisTree.pfjet_neutralemenergy[jet]/energy;
        float elf = analysisTree.pfjet_chargedemenergy[jet]/energy;
        float chm = analysisTree.pfjet_chargedmulti[jet];
        float npr = analysisTree.pfjet_chargedmulti[jet] + analysisTree.pfjet_neutralmulti[jet];
	bool isPFJetId = (npr>1 && phf<0.99 && nhf<0.99) && (absJetEta>2.4 || (elf<0.99 && chf>0 && chm>0));
	if (applyJetPfId&&!isPFJetId) continue;

	// pu jet Id
	if (applyJetPuId&&!puJetIdLoose(analysisTree.pfjet_eta[jet],analysisTree.pfjet_pu_jet_full_mva[jet])) continue;

	jetspt20.push_back(jet);

	if (absJetEta<bJetEtaCut && analysisTree.pfjet_btag[jet][6]>btagCut) { // b-jet
	  bjets.push_back(jet);
	  if (jetPt>ptLeadingBJet) {
	    ptLeadingBJet = jetPt;
	    indexLeadingBJet = jet;
	  }
	} 

	if (jetPt<jetPtHighCut) continue;

	jets.push_back(jet);

	if (indexLeadingJet>=0) {
	  if (jetPt<ptLeadingJet&&jetPt>ptSubLeadingJet) {
	    indexSubLeadingJet = jet;
	    ptSubLeadingJet = jetPt;
	  }
	}

	if (jetPt>ptLeadingJet) {
	  indexLeadingJet = jet;
	  ptLeadingJet = jetPt;
	}
      }
	
      njets = jets.size();
      njetspt20 = jetspt20.size();
      nbtag = bjets.size();
      
      bpt = -9999;
      beta = -9999;
      bphi = -9999;
      
      if (indexLeadingBJet>=0) {
	bpt = analysisTree.pfjet_pt[indexLeadingBJet];
	beta = analysisTree.pfjet_eta[indexLeadingBJet];
	bphi = analysisTree.pfjet_phi[indexLeadingBJet];
      }
     
      jpt_1 = -9999;
      jeta_1 = -9999;
      jphi_1 = -9999;
      jptraw_1 = -9999;
      jptunc_1 = -9999;
      jmva_1 = -9999;
      jlrm_1 = -9999;
      jctm_1 = -9999;

      if (indexLeadingJet>=0&&indexSubLeadingJet>=0&&indexLeadingJet==indexSubLeadingJet)
	cout << "warning : indexLeadingJet ==indexSubLeadingJet = " << indexSubLeadingJet << endl;

      if (indexLeadingJet>=0) {
	jpt_1 = analysisTree.pfjet_pt[indexLeadingJet];
	jeta_1 = analysisTree.pfjet_eta[indexLeadingJet];
	jphi_1 = analysisTree.pfjet_phi[indexLeadingJet];
	jptraw_1 = analysisTree.pfjet_pt[indexLeadingJet]*analysisTree.pfjet_energycorr[indexLeadingJet];
	jmva_1 = analysisTree.pfjet_pu_jet_full_mva[indexLeadingJet];
      }

      jpt_2 = -9999;
      jeta_2 = -9999;
      jphi_2 = -9999;
      jptraw_2 = -9999;
      jptunc_2 = -9999;
      jmva_2 = -9999;
      jlrm_2 = -9999;
      jctm_2 = -9999;

      if (indexSubLeadingJet>=0) {
	jpt_2 = analysisTree.pfjet_pt[indexSubLeadingJet];
	jeta_2 = analysisTree.pfjet_eta[indexSubLeadingJet];
	jphi_2 = analysisTree.pfjet_phi[indexSubLeadingJet];
	jptraw_2 = analysisTree.pfjet_pt[indexSubLeadingJet]*analysisTree.pfjet_energycorr[indexSubLeadingJet];
	jmva_2 = analysisTree.pfjet_pu_jet_full_mva[indexSubLeadingJet];
      }

      mjj =  -9999;
      jdeta =  -9999;
      njetingap = 0;

      if (indexLeadingJet>=0 && indexSubLeadingJet>=0) {

	TLorentzVector jet1; jet1.SetPxPyPzE(analysisTree.pfjet_px[indexLeadingJet],
					     analysisTree.pfjet_py[indexLeadingJet],
					     analysisTree.pfjet_pz[indexLeadingJet],
					     analysisTree.pfjet_e[indexLeadingJet]);

	TLorentzVector jet2; jet2.SetPxPyPzE(analysisTree.pfjet_px[indexSubLeadingJet],
					     analysisTree.pfjet_py[indexSubLeadingJet],
					     analysisTree.pfjet_pz[indexSubLeadingJet],
					     analysisTree.pfjet_e[indexSubLeadingJet]);

	mjj = (jet1+jet2).M();
	jdeta = abs(analysisTree.pfjet_eta[indexLeadingJet]-
		    analysisTree.pfjet_eta[indexSubLeadingJet]);
 
	float etamax = analysisTree.pfjet_eta[indexLeadingJet];
	float etamin = analysisTree.pfjet_eta[indexSubLeadingJet];
	if (etamax<etamin) {
	  float tmp = etamax;
	  etamax = etamin;
	  etamin = tmp;
	}
	for (unsigned int jet=0; jet<jetspt20.size(); ++jet) {
	  int index = jetspt20.at(jet);
	  float etaX = analysisTree.pfjet_eta[index];
	  if (index!=indexLeadingJet&&index!=indexSubLeadingJet&&etaX>etamin&&etaX<etamax) 
	    njetingap++;
	}


      }

      tree->Fill();
      selEvents++;
      
    } // end of file processing (loop over events in one file)
    nFiles++;
    delete _tree;
    file_->Close();
    delete file_;
  }
  std::cout << std::endl;
  int allEvents = int(inputEventsH->GetEntries());
  std::cout << "Total number of input events    = " << allEvents << std::endl;
  std::cout << "Total number of events in Tree  = " << nEvents << std::endl;
  std::cout << "Total number of selected events = " << selEvents << std::endl;
  std::cout << std::endl;
  
  file->cd("");
  file->Write();
  file->Close();
  delete file;
  
}



