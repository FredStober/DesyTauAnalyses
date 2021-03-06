#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <sstream>
#include <map>

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

#include "DesyTauAnalyses/NTupleMaker/interface/Phys14Tree.h"

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

#define pi 3.14159265358979312
#define d2r 1.74532925199432955e-02
#define r2d 57.2957795130823229

#define electronMass 0
#define muonMass 0.10565837
#define tauMass 1.77682
#define pionMass 0.1396

int main(int argc, char * argv[]) {

  // first argument - config file 
  // second argument - filelist

  using namespace std;

  // **** configuration
  Config cfg(argv[1]);

  // vertex cuts
  const float ndofVertexCut  = cfg.get<float>("NdofVertexCut");   
  const float zVertexCut     = cfg.get<float>("ZVertexCut");
  const float dVertexCut     = cfg.get<float>("DVertexCut");

  // muon cuts
  const float ptMuonLowCut   = cfg.get<float>("ptMuonLowCut");
  const float ptMuonHighCut  = cfg.get<float>("ptMuonHighCut");  
  const float etaMuonCut     = cfg.get<float>("etaMuonCut");
  const float dxyMuonCut     = cfg.get<float>("dxyMuonCut");
  const float dzMuonCut      = cfg.get<float>("dzMuonCut");
  const float isoMuonCut     = cfg.get<float>("isoMuonCut");
  const bool applyMuonId     = cfg.get<bool>("ApplyMuonId");

  // tau cuts
  const float ptTauLowCut    = cfg.get<float>("ptTauLowCut");
  const float ptTauHighCut   = cfg.get<float>("ptTauHighCut");  
  const float etaTauCut      = cfg.get<float>("etaTauCut");
  const bool applyTauId      = cfg.get<bool>("ApplyTauId");

  // pair selection
  const float dRleptonsCut   = cfg.get<float>("dRleptonsCut");

  // additional lepton veto
  const float ptDiMuonVeto   = cfg.get<float>("ptDiMuonVeto");  
  const float etaDiMuonVeto  = cfg.get<float>("etaDiMuonVeto");
  
  // topological cuts
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
  const bool debug = cfg.get<bool>("debug");
  
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
  
  Phys14Tree *otree = new Phys14Tree(tree);

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

      otree->run = int(analysisTree.event_run);
      otree->lumi = int(analysisTree.event_luminosityblock);
      otree->evt = int(analysisTree.event_nr);

      bool overlapEvent = true;
      for (unsigned int iEvent=0; iEvent<runList.size(); ++iEvent) {
	if (runList.at(iEvent)==otree->run && eventList.at(iEvent)==otree->evt) {
	  overlapEvent = false;	  
	}
      }

      if (overlapEvent&&checkOverlap) continue;
      nonOverlap++;

      if (debug) {
	fileOutput << std::endl;
	fileOutput << "Run = " << otree->run << "   Event = " << otree->evt << std::endl;
      }

      // weights
      otree->mcweight = 0;
      otree->puweight = 0;
      otree->trigweight_1 = 0;
      otree->trigweight_2 = 0;
      otree->idweight_1 = 0;
      otree->idweight_2 = 0;
      otree->isoweight_1 = 0;
      otree->isoweight_2 = 0;
      otree->effweight = 0;
      otree->fakeweight = 0;
      otree->embeddedWeight = 0;
      otree->signalWeight = 0;
      otree->weight = 1;
      
      otree->npv = analysisTree.primvertex_count;
      otree->npu = analysisTree.numpileupinteractions;
      otree->rho = analysisTree.rho;
      
      // vertex cuts
      if (fabs(analysisTree.primvertex_z)>zVertexCut) continue;
      if (analysisTree.primvertex_ndof<=ndofVertexCut) continue;
      float dVertex = sqrt(analysisTree.primvertex_x*analysisTree.primvertex_x+
			   analysisTree.primvertex_y*analysisTree.primvertex_y);
      if (dVertex>dVertexCut)
	continue;
      
      if (debug)
	fileOutput << "Vertex cuts are passed " << std::endl;

      // muon selection
      if (debug)
	fileOutput << "# muons = " << analysisTree.muon_count << std::endl;
      vector<int> muons; muons.clear();
      for (unsigned int im = 0; im<analysisTree.muon_count; ++im) {
	bool isGoodGlobal = analysisTree.muon_isGlobal[im] && analysisTree.muon_normChi2[im]<3 
	  && analysisTree.muon_combQ_chi2LocalPosition[im]<12 && analysisTree.muon_combQ_trkKink[im]<20;
	bool isMedium = analysisTree.muon_isLoose[im] && analysisTree.muon_validFraction[im]>0.8 
	  && analysisTree.muon_segmentComp[im] > (isGoodGlobal ? 0.303 : 0.451);
	if (debug)
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

      // tau selection
      if (debug)
	fileOutput << "# taus = " << analysisTree.tau_count << std::endl;
      vector<int> taus; taus.clear();
      for (unsigned int it = 0; it<analysisTree.tau_count; ++it) {
	if (debug)
	  fileOutput << "  " << it 
		     << " pt = " << analysisTree.tau_pt[it] 
		     << " eta = " << analysisTree.tau_eta[it]
		     << " decayModeFinding = " << analysisTree.tau_decayModeFinding[it]
		     << " decayModeFindingNewDMs = " << analysisTree.tau_decayModeFindingNewDMs[it]
		     << " tau_vertexz = " << analysisTree.tau_vertexz[it] <<std::endl;
	if (analysisTree.tau_pt[it]<ptTauLowCut) continue;
	if (fabs(analysisTree.tau_eta[it])>etaTauCut) continue;
	if (applyTauId &&
	    analysisTree.tau_decayModeFinding[it] < 0.5 &&
	    analysisTree.tau_decayModeFindingNewDMs[it] < 0.5) continue;	
	if (analysisTree.tau_vertexz[it]!=analysisTree.primvertex_z) continue;
	taus.push_back(it);
      }

      if (debug) {
	fileOutput << " # selected muons     = " << muons.size() << std::endl;
	fileOutput << " # selected taus = " << taus.size() << std::endl;	
      }
      
      if (muons.size()==0) continue;
      if (taus.size()==0) continue;

      // selecting muon and tau pair (OS or SS);
      int muonIndex = -1;
      int tauIndex = -1;
      
      float isoMuMin = 1e+10;
      float isoTauMin = 1e+10;      
      for (unsigned int im=0; im<muons.size(); ++im) {
	bool isTrigMu17 = false;
	bool isTrigMu24 = false;

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
	if (debug)
	  fileOutput << "Muon " << mIndex << " -> relIso = "<<relIsoMu<<" absIso = "<<absIsoMu<<std::endl;

	for (unsigned int iT=0; iT<analysisTree.trigobject_count; ++iT) {
	  /*if (analysisTree.trigobject_isElectron[iT] ||
	      !analysisTree.trigobject_isMuon[iT] ||
	      analysisTree.trigobject_isTau[iT]) continue;*/
	  
	  float dRtrig = deltaR(analysisTree.muon_eta[mIndex],analysisTree.muon_phi[mIndex],
				analysisTree.trigobject_eta[iT],analysisTree.trigobject_phi[iT]);

	  if (dRtrig < deltaRTrigMatch){
	    if (analysisTree.trigobject_filters[iT][2]) // Mu17 Leg
	      isTrigMu17 = true;	      
	    if (analysisTree.trigobject_filters[iT][0]) // Mu24 Leg
	      isTrigMu24 = true;
	  }
	}
	  
	if (debug)
	  fileOutput << "Muon " << im << " -> isTrigMu17 = " << isTrigMu17 << " ; isTrigMu24 = " << isTrigMu24 << std::endl;

	if ((!isTrigMu17) && (!isTrigMu24)) continue;
      
	for (unsigned int it=0; it<taus.size(); ++it) {
	  unsigned int tIndex = taus.at(it);
	  float absIsoTau = analysisTree.tau_byCombinedIsolationDeltaBetaCorrRaw3Hits[tIndex];
	  float relIsoTau = absIsoTau / analysisTree.tau_pt[tIndex];

	  if (debug)
	    fileOutput << "tau" << tIndex << " -> relIso = "<<relIsoTau<<" absIso = "<<absIsoTau<<std::endl;
	  
	  float dR = deltaR(analysisTree.tau_eta[tIndex],analysisTree.tau_phi[tIndex],
			    analysisTree.muon_eta[mIndex],analysisTree.muon_phi[mIndex]);

	  if (debug)
	    fileOutput << "dR(mu,tau) = " << dR << std::endl;

	  if (dR<dRleptonsCut) continue;
	  
	  bool isTrigTau = false;
	  float dRtrig_min = 9999.;
	  
	  for (unsigned int iT=0; iT<analysisTree.trigobject_count; ++iT) {
	    if (debug){
	      if (analysisTree.trigobject_isElectron[iT] && analysisTree.trigobject_isMuon[iT])
		fileOutput<<" trigObj "<<iT<<" both e and mu"<<std::endl;
	      
	      if (analysisTree.trigobject_isMuon[iT] && analysisTree.trigobject_isTau[iT])
		fileOutput<<" trigObj "<<iT<<" both mu and tau"<<std::endl;
	      
	      if (analysisTree.trigobject_isElectron[iT] && analysisTree.trigobject_isTau[iT])
		fileOutput<<" trigObj "<<iT<<" both e and tau"<<std::endl;
	    }
	    
	    if (analysisTree.trigobject_isElectron[iT] ||
		analysisTree.trigobject_isMuon[iT] ||
		!analysisTree.trigobject_isTau[iT]) continue;
	    
	    float dRtrig = deltaR(analysisTree.tau_eta[tIndex],analysisTree.tau_phi[tIndex],
				  analysisTree.trigobject_eta[iT],analysisTree.trigobject_phi[iT]);
	    
	    if (dRtrig < deltaRTrigMatch){
	      if (analysisTree.trigobject_filters[iT][10] || analysisTree.trigobject_filters[iT][2]){
		isTrigTau = true;
		if (dRtrig < dRtrig_min)
		  dRtrig_min = dRtrig;
	      }
	    }
	  }
	  
	  if (debug)
	    fileOutput << "Tau " << it << " -> isTrigTau = " << isTrigTau << " DR="<<dRtrig_min<<std::endl;

	  bool trigMatch = isTrigMu24 || (isTrigMu17 && isTrigTau);

	  if (isTrigMu24 && analysisTree.hltriggerresults_second[5]==false)
	    if (debug)
	      fileOutput<<"IsoMu24 Trigger Mismatch"<<std::endl;
	     
	  if (isTrigMu17 && isTrigTau && analysisTree.hltriggerresults_second[4]==false)
	    if (debug)
	      fileOutput<<"xMuTau Trigger Mismatch"<<std::endl;
	      
	  if (!trigMatch) continue;
	  
	  bool isKinematicMatch = false;
	  if (isTrigMu24) {
	    if (analysisTree.muon_pt[mIndex]>ptMuonHighCut)
	      isKinematicMatch = true;
	  }	  
	  if (isTrigMu17 && isTrigTau) {
            if (analysisTree.muon_pt[mIndex]>ptMuonLowCut && analysisTree.tau_pt[tIndex]>ptTauHighCut)
              isKinematicMatch = true;
	      }
	  
	  //if (analysisTree.muon_pt[mIndex]>ptMuonLowCut && analysisTree.tau_pt[tIndex]>ptTauHighCut)
	  //  isKinematicMatch = true;
	  
	  if (!isKinematicMatch) continue;
	  
	  bool changePair =  false;
	  
	  if (relIsoMu<isoMuMin) {
	    changePair = true;
	  }
	  else if (fabs(relIsoMu - isoMuMin) < 1.e-5) {
	    if (analysisTree.muon_pt[mIndex] > analysisTree.muon_pt[muonIndex]) {
	      changePair = true;
	    }	    
	    else if (fabs(analysisTree.muon_pt[mIndex] - analysisTree.muon_pt[muonIndex]) < 1.e-5) {
	      if (absIsoTau < isoTauMin) {
		changePair = true;
	      }
	      else if ((absIsoTau - isoTauMin) < 1.e-5){
		if (analysisTree.tau_pt[tIndex] > analysisTree.tau_pt[tauIndex]) {
		  changePair = true;
		}
	      }
	    }
	  }
	  
	  if (changePair){
	    isoMuMin  = relIsoMu;
	    muonIndex = mIndex;
	    isoTauMin = absIsoTau;
	    tauIndex = tIndex;
	  }
	}
      }
      
      if (muonIndex<0) continue;
      if (tauIndex<0) continue;
      
      // filling muon variables
      otree->pt_1 = analysisTree.muon_pt[muonIndex];
      otree->eta_1 = analysisTree.muon_eta[muonIndex];
      otree->phi_1 = analysisTree.muon_phi[muonIndex];
      otree->m_1 = muonMass;
      otree->q_1 = -1;
      if (analysisTree.muon_charge[muonIndex]>0)
        otree->q_1 = 1;
      otree->iso_1 = isoMuMin;
      otree->mva_1 = log(0);
      otree->d0_1 = analysisTree.muon_dxy[muonIndex];
      otree->dZ_1 = analysisTree.muon_dz[muonIndex];

      otree->byCombinedIsolationDeltaBetaCorrRaw3Hits_1 = 0;
      otree->againstElectronLooseMVA5_1 = 0;
      otree->againstElectronMediumMVA5_1 = 0;
      otree->againstElectronTightMVA5_1 = 0;
      otree->againstElectronVLooseMVA5_1 = 0;
      otree->againstElectronVTightMVA5_1 = 0;
      otree->againstMuonLoose3_1 = 0;
      otree->againstMuonTight3_1 = 0;
      
      // filling tau variables
      otree->pt_2 = analysisTree.tau_pt[tauIndex];
      otree->eta_2 = analysisTree.tau_eta[tauIndex];
      otree->phi_2 = analysisTree.tau_phi[tauIndex];
      otree->q_2 = -1;
      if (analysisTree.tau_charge[tauIndex]>0)
	otree->q_2 = 1;
      otree->mva_2 = log(0);
      otree->d0_2 = analysisTree.tau_dxy[tauIndex];
      otree->dZ_2 = analysisTree.tau_dz[tauIndex];
      otree->iso_2 = analysisTree.tau_byCombinedIsolationDeltaBetaCorrRaw3Hits[tauIndex];
      otree->m_2 = analysisTree.tau_mass[tauIndex];

      otree->byCombinedIsolationDeltaBetaCorrRaw3Hits_2 = analysisTree.tau_byCombinedIsolationDeltaBetaCorrRaw3Hits[tauIndex];
      otree->againstElectronLooseMVA5_2 = analysisTree.tau_againstElectronLooseMVA5[tauIndex];
      otree->againstElectronMediumMVA5_2 = analysisTree.tau_againstElectronMediumMVA5[tauIndex];
      otree->againstElectronTightMVA5_2 = analysisTree.tau_againstElectronTightMVA5[tauIndex];
      otree->againstElectronVLooseMVA5_2 = analysisTree.tau_againstElectronVLooseMVA5[tauIndex];
      otree->againstElectronVTightMVA5_2 = analysisTree.tau_againstElectronVTightMVA5[tauIndex];
      otree->againstMuonLoose3_2 = analysisTree.tau_againstMuonLoose3[tauIndex];
      otree->againstMuonTight3_2 = analysisTree.tau_againstMuonTight3[tauIndex];

      // ditau system
      TLorentzVector muonLV; muonLV.SetXYZM(analysisTree.muon_px[muonIndex],
					    analysisTree.muon_py[muonIndex],
					    analysisTree.muon_pz[muonIndex],
					    muonMass);

      TLorentzVector tauLV; tauLV.SetXYZM(analysisTree.tau_px[tauIndex],
					  analysisTree.tau_py[tauIndex],
					  analysisTree.tau_pz[tauIndex],
					  tauMass);

      TLorentzVector dileptonLV = muonLV + tauLV;

      otree->m_vis = dileptonLV.M();
      otree->pt_tt = dileptonLV.Pt();

      // opposite charge
      otree->os = (otree->q_1 * otree->q_2) < 0.;

      // dimuon veto
      otree->dilepton_veto = 0;

      for (unsigned int im = 0; im<analysisTree.muon_count; ++im) {
	if (analysisTree.muon_pt[im]<ptDiMuonVeto) continue;
	if (fabs(analysisTree.muon_eta[im])>etaDiMuonVeto) continue;

	if(!analysisTree.muon_isGlobal[im]) continue;
	if(!analysisTree.muon_isTracker[im]) continue;
	if(!analysisTree.muon_isPF[im]) continue;
	
	if (fabs(analysisTree.muon_dxy[im])>dxyMuonCut) continue;
	if (fabs(analysisTree.muon_dz[im])>dzMuonCut) continue;

	if (otree->q_1 * analysisTree.muon_charge[im] > 0.) continue;
	
	float neutralHadIsoMu = analysisTree.muon_neutralHadIso[im];
	float photonIsoMu = analysisTree.muon_photonIso[im];
	float chargedHadIsoMu = analysisTree.muon_chargedHadIso[im];
	float puIsoMu = analysisTree.muon_puIso[im];
	if (isIsoR03) {
	  neutralHadIsoMu = analysisTree.muon_r03_sumNeutralHadronEt[im];
	  photonIsoMu = analysisTree.muon_r03_sumPhotonEt[im];
	  chargedHadIsoMu = analysisTree.muon_r03_sumChargedHadronPt[im];
	  puIsoMu = analysisTree.muon_r03_sumPUPt[im];
	}
	float neutralIsoMu = neutralHadIsoMu + photonIsoMu - 0.5*puIsoMu;
	neutralIsoMu = TMath::Max(float(0),neutralIsoMu); 
	float absIsoMu = chargedHadIsoMu + neutralIsoMu;
	float relIsoMu = absIsoMu/analysisTree.muon_pt[im];

	if(relIsoMu > 0.3) continue;

	float dr = deltaR(analysisTree.muon_eta[im],analysisTree.muon_phi[im],
			  otree->eta_1,otree->phi_1);
	if(dr<0.15) continue;

	otree->dilepton_veto = 1;
      }
      
      // extra electron veto
      otree->extraelec_veto = 0;

      // extra muon veto
      otree->extramuon_veto = 0;
      
      // svfit variables
      otree->m_sv = -9999;
      otree->pt_sv = -9999;
      otree->eta_sv = -9999;
      otree->phi_sv = -9999;

      // pfmet variables      
      otree->met = TMath::Sqrt(analysisTree.pfmet_ex*analysisTree.pfmet_ex + analysisTree.pfmet_ey*analysisTree.pfmet_ey);
      otree->metphi = TMath::ATan2(analysisTree.pfmet_ey,analysisTree.pfmet_ex);
      otree->metcov00 = analysisTree.pfmet_sigxx;
      otree->metcov01 = analysisTree.pfmet_sigxy;
      otree->metcov10 = analysisTree.pfmet_sigyx;
      otree->metcov11 = analysisTree.pfmet_sigyy;
      
      // bisector of muon and tau transverse momenta
      float muonUnitX = muonLV.Px()/muonLV.Pt();
      float muonUnitY = muonLV.Py()/muonLV.Pt();
	
      float tauUnitX = tauLV.Px()/tauLV.Pt();
      float tauUnitY = tauLV.Py()/tauLV.Pt();

      float zetaX = muonUnitX + tauUnitX;
      float zetaY = muonUnitY + tauUnitY;
      
      float normZeta = TMath::Sqrt(zetaX*zetaX+zetaY*zetaY);

      zetaX = zetaX/normZeta;
      zetaY = zetaY/normZeta;

      float vectorVisX = muonLV.Px() + tauLV.Px();
      float vectorVisY = muonLV.Py() + tauLV.Py();

      otree->pzetavis  = vectorVisX*zetaX + vectorVisY*zetaY;

      

      
      // choosing mva met
      unsigned int iMet = 0;
      for (; iMet<analysisTree.mvamet_count; ++iMet) {
	if ((int)analysisTree.mvamet_channel[iMet]==3) break;
      }
      
      if (iMet>=analysisTree.mvamet_count){
	if(debug)
	  fileOutput<<"MVA MET channel ploblems.."<<std::endl;

	otree->mvamet = log(0);
	otree->mvametphi = log(0);
 	otree->mvacov00 = log(0);
	otree->mvacov01 = log(0);
	otree->mvacov10 = log(0);
	otree->mvacov11 = log(0);

	otree->mt_1 = log(0);
	otree->mt_2 = log(0);
	
	otree->pzetamiss = log(0);	
      }
      else {
	float mvamet_x = analysisTree.mvamet_ex[iMet];
	float mvamet_y = analysisTree.mvamet_ey[iMet];
	float mvamet_x2 = mvamet_x * mvamet_x;
	float mvamet_y2 = mvamet_y * mvamet_y;

	otree->mvamet = TMath::Sqrt(mvamet_x2+mvamet_y2);
	otree->mvametphi = TMath::ATan2(mvamet_y,mvamet_x);
	otree->mvacov00 = analysisTree.mvamet_sigxx[iMet];
	otree->mvacov01 = analysisTree.mvamet_sigxy[iMet];
	otree->mvacov10 = analysisTree.mvamet_sigyx[iMet];
	otree->mvacov11 = analysisTree.mvamet_sigyy[iMet];

	// computation of mt
	otree->mt_1 = sqrt(2*otree->pt_1*otree->mvamet*(1.-cos(otree->phi_1-otree->mvametphi)));
	otree->mt_2 = sqrt(2*otree->pt_2*otree->mvamet*(1.-cos(otree->phi_2-otree->mvametphi)));  
	
	// computation of DZeta variable
	otree->pzetamiss = analysisTree.pfmet_ex*zetaX + analysisTree.pfmet_ey*zetaY;
      }

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
			   otree->eta_1,otree->phi_1);

	float dR2 = deltaR(analysisTree.pfjet_eta[jet],analysisTree.pfjet_phi[jet],
                           otree->eta_2,otree->phi_2);
	
	// pf jet Id
	float energy = analysisTree.pfjet_e[jet];
        float chf = analysisTree.pfjet_chargedhadronicenergy[jet]/energy;
        float nhf = analysisTree.pfjet_neutralhadronicenergy[jet]/energy;
        float phf = analysisTree.pfjet_neutralemenergy[jet]/energy;
        float elf = analysisTree.pfjet_chargedemenergy[jet]/energy;
        float chm = analysisTree.pfjet_chargedmulti[jet];
        float npr = analysisTree.pfjet_chargedmulti[jet] + analysisTree.pfjet_neutralmulti[jet];
	bool isPFJetId = (npr>1 && phf<0.99 && nhf<0.99) && (absJetEta>2.4 || (elf<0.99 && chf>0 && chm>0)); // muon fraction missing

	if(debug)
	  fileOutput<<" jet "<<jet<<": pt="<<analysisTree.pfjet_pt[jet]<<" eta="<<analysisTree.pfjet_eta[jet]
		    <<" dr1="<<dR1<<" dr2="<<dR2<<" npr="<<npr<<" phf="<<phf<<" nhf="<<nhf<<std::endl;

	// apply dR cut
	if (dR1<dRJetLeptonCut) continue;
	if (dR2<dRJetLeptonCut) continue;

	// apply pf jet Id
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

	if(jetPt>ptLeadingJet){
	  ptSubLeadingJet = ptLeadingJet;
	  indexSubLeadingJet = indexLeadingJet;

	  ptLeadingJet = jetPt;
	  indexLeadingJet = jet;
	}
	else if(jetPt>ptSubLeadingJet){
	  ptSubLeadingJet = jetPt;
	  indexSubLeadingJet = jet;
	}	
      }
      
      otree->njets = jets.size();
      otree->njetspt20 = jetspt20.size();
      otree->nbtag = bjets.size();
      
      otree->bpt = -9999;
      otree->beta = -9999;
      otree->bphi = -9999;
      
      if (indexLeadingBJet>=0) {
	otree->bpt = analysisTree.pfjet_pt[indexLeadingBJet];
	otree->beta = analysisTree.pfjet_eta[indexLeadingBJet];
	otree->bphi = analysisTree.pfjet_phi[indexLeadingBJet];
      }
     
      otree->jpt_1 = -9999;
      otree->jeta_1 = -9999;
      otree->jphi_1 = -9999;
      otree->jptraw_1 = -9999;
      otree->jptunc_1 = -9999;
      otree->jmva_1 = -9999;
      otree->jlrm_1 = -9999;
      otree->jctm_1 = -9999;

      if (indexLeadingJet>=0&&indexSubLeadingJet>=0&&indexLeadingJet==indexSubLeadingJet)
	cout << "warning : indexLeadingJet ==indexSubLeadingJet = " << indexSubLeadingJet << endl;

      if (indexLeadingJet>=0) {
	otree->jpt_1 = analysisTree.pfjet_pt[indexLeadingJet];
	otree->jeta_1 = analysisTree.pfjet_eta[indexLeadingJet];
	otree->jphi_1 = analysisTree.pfjet_phi[indexLeadingJet];
	otree->jptraw_1 = analysisTree.pfjet_pt[indexLeadingJet]*analysisTree.pfjet_energycorr[indexLeadingJet];
	otree->jmva_1 = analysisTree.pfjet_pu_jet_full_mva[indexLeadingJet];
      }

      otree->jpt_2 = -9999;
      otree->jeta_2 = -9999;
      otree->jphi_2 = -9999;
      otree->jptraw_2 = -9999;
      otree->jptunc_2 = -9999;
      otree->jmva_2 = -9999;
      otree->jlrm_2 = -9999;
      otree->jctm_2 = -9999;

      if (indexSubLeadingJet>=0) {
	otree->jpt_2 = analysisTree.pfjet_pt[indexSubLeadingJet];
	otree->jeta_2 = analysisTree.pfjet_eta[indexSubLeadingJet];
	otree->jphi_2 = analysisTree.pfjet_phi[indexSubLeadingJet];
	otree->jptraw_2 = analysisTree.pfjet_pt[indexSubLeadingJet]*analysisTree.pfjet_energycorr[indexSubLeadingJet];
	otree->jmva_2 = analysisTree.pfjet_pu_jet_full_mva[indexSubLeadingJet];
      }

      otree->mjj =  -9999;
      otree->jdeta =  -9999;
      otree->njetingap = 0;

      if (indexLeadingJet>=0 && indexSubLeadingJet>=0) {

	TLorentzVector jet1; jet1.SetPxPyPzE(analysisTree.pfjet_px[indexLeadingJet],
					     analysisTree.pfjet_py[indexLeadingJet],
					     analysisTree.pfjet_pz[indexLeadingJet],
					     analysisTree.pfjet_e[indexLeadingJet]);

	TLorentzVector jet2; jet2.SetPxPyPzE(analysisTree.pfjet_px[indexSubLeadingJet],
					     analysisTree.pfjet_py[indexSubLeadingJet],
					     analysisTree.pfjet_pz[indexSubLeadingJet],
					     analysisTree.pfjet_e[indexSubLeadingJet]);

	otree->mjj = (jet1+jet2).M();
	otree->jdeta = abs(analysisTree.pfjet_eta[indexLeadingJet]-
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
	    otree->njetingap++;
	}
      }
      
      otree->Fill();
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
