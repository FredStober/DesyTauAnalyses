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
#include "TH1D.h"
#include "TChain.h"
#include "TMath.h"
#include "Riostream.h"

#include "TRandom.h"

#include "AnalysisMacro.h"

int main(int argc, char * argv[]) {

  // first argument - config file 
  // second argument - filelist

  using namespace std;

  //const int CutNumb = 8;
  //string CutList[CutNumb]={"No cut","Trigger","1l","lept-Veto","b-Veto","MET $>$ 50","MET $>$ 100","dPhi $>$ 1"};

  // **** configuration
  Config cfg(argv[1]);
  string SelectionSign="eltau";

  // kinematic cuts on electrons
  const Float_t ptElectronLowCut   = cfg.get<Float_t>("ptElectronLowCut");
  const Float_t ptElectronHighCut  = cfg.get<Float_t>("ptElectronHighCut");
  const Float_t etaElectronCut     = cfg.get<Float_t>("etaElectronCut");
  const Float_t dxyElectronCut     = cfg.get<Float_t>("dxyElectronCut");
  const Float_t dzElectronCut      = cfg.get<Float_t>("dzElectronCut");
  const Float_t isoElectronLowCut  = cfg.get<Float_t>("isoElectronLowCut");
  const Float_t isoElectronHighCut = cfg.get<Float_t>("isoElectronHighCut");
  const bool applyElectronId     = cfg.get<bool>("ApplyElectronId");

  // kinematic cuts on muons
  const Float_t ptMuonLowCut   = cfg.get<Float_t>("ptMuonLowCut");
  const Float_t ptMuonHighCut  = cfg.get<Float_t>("ptMuonHighCut");
  const Float_t etaMuonCut     = cfg.get<Float_t>("etaMuonCut");
  const Float_t dxyMuonCut     = cfg.get<Float_t>("dxyMuonCut");
  const Float_t dzMuonCut      = cfg.get<Float_t>("dzMuonCut");
  const Float_t isoMuonLowCut  = cfg.get<Float_t>("isoMuonLowCut");
  const Float_t isoMuonHighCut = cfg.get<Float_t>("isoMuonHighCut");
  const bool applyMuonId     = cfg.get<bool>("ApplyMuonId");
  
  // kinematic cuts on Jets
  const Float_t etaJetCut   = cfg.get<Float_t>("etaJetCut");
  const Float_t ptJetCut   = cfg.get<Float_t>("ptJetCut");
  
  
  // topological cuts
  const Float_t dRleptonsCut   = cfg.get<Float_t>("dRleptonsCut");
  const Float_t dZetaCut       = cfg.get<Float_t>("dZetaCut");
  const bool oppositeSign    = cfg.get<bool>("oppositeSign");
  const Float_t taupt    = cfg.get<Float_t>("taupt");
  const Float_t taueta    = cfg.get<Float_t>("taueta");
  const Float_t decayModeFinding    = cfg.get<Float_t>("decayModeFinding");
  const Float_t   decayModeFindingNewDMs  = cfg.get<Float_t>("decayModeFindingNewDMs");
  const Float_t   againstElectronVLooseMVA5  = cfg.get<Float_t>("againstElectronVLooseMVA5");
  const Float_t   againstMuonTight3  = cfg.get<Float_t>("againstMuonTight3");
  const Float_t   vertexz =  cfg.get<Float_t>("vertexz");
  const Float_t   byCombinedIsolationDeltaBetaCorrRaw3Hits = cfg.get<Float_t>("byCombinedIsolationDeltaBetaCorrRaw3Hits");
  


  const Float_t Lumi   = cfg.get<Float_t>("Lumi");

  const Float_t bTag 	     = cfg.get<Float_t>("bTag");
  const Float_t metcut         = cfg.get<Float_t>("metcut");
 
  CutList.clear();
  CutList.push_back("No cut");
  CutList.push_back("Trigger");
  CutList.push_back("$\e$");
  CutList.push_back("$tau_h$");
  CutList.push_back("2nd lept-Veto");
  CutList.push_back("3rd lept-Veto");
  CutList.push_back("b-Veto ");
  CutList.push_back("MET $>$ 50");
  CutList.push_back("MET $>$ 100");
  CutList.push_back("dPhi > 1");
 

  int CutNumb = int(CutList.size());
  xs=1;fact=1;fact2=1;
 
        
  ifstream ifs("xsecs");
  string line;

  while(std::getline(ifs, line)) // read one line from ifs
    {
		
      fact=fact2=1;
      istringstream iss(line); // access line as a stream

      // we only need the first two columns
      string dt;
      iss >> dt >> xs >> fact >> fact2;
      //ifs >> dt >> xs; // no need to read further
      //cout<< " "<<dt<<"  "<<endl;
      //cout<< "For sample ========================"<<dt<<" xsecs is "<<xs<<" XSec "<<XSec<<"  "<<fact<<"  "<<fact2<<endl;
      //if (dt==argv[2]) {
      //if (std::string::npos != dt.find(argv[2])) {
      if (  dt == argv[2]) {
	XSec= xs*fact*fact2;
	cout<<" Found the correct cross section "<<xs<<" for Dataset "<<dt<<" XSec "<<XSec<<endl;
      }
        
    }

  if (XSec<0) {cout<<" Something probably wrong with the xsecs...please check  - the input was "<<argv[2]<<endl;return 0;}




	
  bool doThirdLeptVeto=true;
  bool doElVeto=true;

  //CutList[CutNumb]=CutListt[CutNumb];

  // file name and tree name
  std::string rootFileName(argv[2]);
  std::ifstream fileList(argv[2]);
  std::ifstream fileList0(argv[2]);
  std::string ntupleName("makeroottree/AC1B");

  TString TStrName(rootFileName);
  std::cout <<TStrName <<std::endl;  

  // output fileName with histograms
  TFile * file = new TFile(TStrName+TString(".root"),"update");
  file->mkdir(SelectionSign.c_str());
  file->cd(SelectionSign.c_str());

  int nFiles = 0;
  int nEvents = 0;
  int selEvents = 0;

  int nTotalFiles = 0;
  int iCut=0;
  double CFCounter[CutNumb];
  double statUnc[CutNumb];
  int iCFCounter[CutNumb];
  for (int i=0;i < CutNumb; i++){
    CFCounter[i] = 0;
    iCFCounter[i] = 0;
    statUnc[i] =0;
  }

  std::string dummy;
  // count number of files --->
  while (fileList0 >> dummy) nTotalFiles++;
 
  SetupHists(CutNumb); 
  //nTotalFiles=10;
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
    //NE=1000;

    for (int iE=0;iE<NE;++iE)
      inputEventsH->Fill(0.);

    AC1B analysisTree(_tree);
    
    Long64_t numberOfEntries = analysisTree.GetEntries();
    //numberOfEntries = 1000;
    
    std::cout << "      number of entries in Tree = " << numberOfEntries << std::endl;
    //numberOfEntries = 10000;
    for (Long64_t iEntry=0; iEntry<numberOfEntries; iEntry++) { 
     
      analysisTree.GetEntry(iEntry);
      nEvents++;
       
      JetsMV.clear();
      ElMV.clear();
      TauMV.clear();
      MuMV.clear();
      LeptMV.clear();

      Float_t MET = sqrt ( analysisTree.pfmet_ex*analysisTree.pfmet_ex + analysisTree.pfmet_ey*analysisTree.pfmet_ey);
      
      METV.SetPx(analysisTree.pfmet_ex);	      
      METV.SetPy(analysisTree.pfmet_ey);
 
      if (nEvents%10000==0) 
	cout << "      processed " << nEvents << " events" << endl; 
 

      for (unsigned int ij = 0; ij<analysisTree.pfjet_count; ++ij) {
	JetsV.SetPxPyPzE(analysisTree.pfjet_px[ij], analysisTree.pfjet_py[ij], analysisTree.pfjet_pz[ij], analysisTree.pfjet_e[ij]);
	JetsMV.push_back(JetsV);
      } 



      for (unsigned int im = 0; im<analysisTree.muon_count; ++im) {
	MuV.SetPtEtaPhiM(analysisTree.muon_pt[im], analysisTree.muon_eta[im], analysisTree.muon_phi[im], muonMass);
	MuMV.push_back(MuV);

      }

      for (unsigned int ie = 0; ie<analysisTree.electron_count; ++ie) {
	ElV.SetPtEtaPhiM(analysisTree.electron_pt[ie], analysisTree.electron_eta[ie], analysisTree.electron_phi[ie], electronMass);
	ElMV.push_back(ElV);
      }
   
      for (unsigned int it = 0; it<analysisTree.tau_count; ++it) {
	TauV.SetPtEtaPhiM(analysisTree.tau_pt[it], analysisTree.tau_eta[it], analysisTree.tau_phi[it], tauMass);
	TauMV.push_back(TauV);
      }


      Float_t weight = 1;
      iCut = 0;
      
      Double_t EvWeight = 1.0;
      EvWeight *= weight ;
      
      FillMainHists(iCut, EvWeight, ElMV, MuMV, TauMV,JetsMV,METV, analysisTree, SelectionSign);
      CFCounter[iCut]+= weight;
      iCFCounter[iCut]++;
      iCut++;
      // hnJets[->Fill(pfjet_count);

      //      std::cout << "Entry : " << iEntry << std::endl;
      //      std::cout << "Number of gen particles = " << analysisTree.genparticles_count << std::endl;
      //      std::cout << "Number of taus  = " << analysisTree.tau_count << std::endl;
      //      std::cout << "Number of jets  = " << analysisTree.pfjet_count << std::endl;
      //      std::cout << "Number of muons = " << analysisTree.muon_count << std::endl;
      
      // **** Analysis of generator info
      // int indexW  = -1;
      // int indexNu = -1; 
      // int indexMu = -1;
      // int indexE  = -1;
      // int nGenMuons = 0;
      // int nGenElectrons = 0;
      // for (unsigned int igen=0; igen<analysisTree.genparticles_count; ++igen) {

      // 	Float_t pxGen = analysisTree.genparticles_px[igen];
      // 	Float_t pyGen = analysisTree.genparticles_py[igen];
      // 	Float_t pzGen = analysisTree.genparticles_pz[igen];
      // 	Float_t etaGen = PtoEta(pxGen,pyGen,pzGen);
      // 	Float_t ptGen  = PtoPt(pxGen,pyGen);

      // 	if (fabs(analysisTree.genparticles_pdgid[igen])==24 && analysisTree.genparticles_status[igen]==62) 
      // 	  indexW = igen;
      // 	if ((fabs(analysisTree.genparticles_pdgid[igen])==12 
      // 	     ||fabs(analysisTree.genparticles_pdgid[igen])==14
      // 	     ||fabs(analysisTree.genparticles_pdgid[igen])==16) 
      // 	    && analysisTree.genparticles_info[igen]== (1<<1) )
      // 	  indexNu = igen;

      // 	if (fabs(analysisTree.genparticles_pdgid[igen])==13) {
      // 	  if ( analysisTree.genparticles_info[igen]== (1<<1) ) {
      // 	    indexMu = igen;
      // 	    if (fabs(etaGen)<2.3 && ptGen>10.)
      // 	      nGenMuons++;
      // 	  }
      // 	}
      // 	if (fabs(analysisTree.genparticles_pdgid[igen])==11) {
      // 	  if ( analysisTree.genparticles_info[igen]== (1<<1) ) {
      // 	    indexE = igen;
      // 	    if (fabs(etaGen)<2.3 && ptGen>10.)
      // 	      nGenElectrons++;
      // 	  }
      // 	}
      // }

      // trigger selection
 
      //selecTable.Fill(1,0, weight );      
      bool trigAccept = false;

      for (int i=0; i<kMaxhltriggerresults; ++i) {
	//  for muel if ((i==5||i==6)&&  analysisTree.hltriggerresults_second[i]==1) {
//for mutau	if ((i==0 || i==2)&&  analysisTree.hltriggerresults_second[i]==1) {
	if ( ( i==2 || i==3)  &&  analysisTree.hltriggerresults_second[i]==1) {
	//    	  std::cout << analysisTree.run_hltnames->at(i) << " : " << analysisTree.hltriggerresults_second[i] << std::endl;
	  trigAccept = true;
	}
      }
      if (!trigAccept) continue;

      //Trigger
      FillMainHists(iCut, EvWeight, ElMV, MuMV, TauMV,JetsMV,METV,analysisTree, SelectionSign);
      CFCounter[iCut]+= weight;
      iCFCounter[iCut]++;
      iCut++;
      /////now clear the Mu.El.Jets again to fill them again after cleaning
      MuMV.clear();
      ElMV.clear();
      // electron selection
      unsigned int el_index=-1;
      unsigned int tau_index=-1;




	vector<int> electrons; electrons.clear();
      for (unsigned int ie = 0; ie<analysisTree.electron_count; ++ie) {
	electronPtAllH->Fill(analysisTree.electron_pt[ie],weight);
	if (analysisTree.electron_pt[ie]<ptElectronHighCut) continue;
	if (fabs(analysisTree.electron_eta[ie])>etaElectronCut) continue;
	if (fabs(analysisTree.electron_dxy[ie])>dxyElectronCut) continue;
	if (fabs(analysisTree.electron_dz[ie])>dzElectronCut) continue;
	Float_t neutralIso = 
	  analysisTree.electron_neutralHadIso[ie] + 
	  analysisTree.electron_photonIso[ie] - 
	  0.5*analysisTree.electron_puIso[ie];
	neutralIso = TMath::Max(Float_t(0),neutralIso); 
	Float_t absIso = analysisTree.electron_chargedHadIso[ie] + neutralIso;
	Float_t relIso = absIso/analysisTree.electron_pt[ie];
        hel_relISO[1]->Fill(relIso,weight);
	if (relIso>isoElectronHighCut) continue;
	if (relIso<isoElectronLowCut) continue;
	bool electronMvaId = electronMvaIdTight(analysisTree.electron_superclusterEta[ie],
						analysisTree.electron_mva_id_nontrigPhys14[ie]);
	if (!electronMvaId&&applyElectronId) continue;
	electrons.push_back(ie);
        ElV.SetPtEtaPhiM(analysisTree.electron_pt[ie], analysisTree.electron_eta[ie], analysisTree.electron_phi[ie], electronMass);
        ElMV.push_back(ElV);
	LeptMV.push_back(ElV);
        //hel_miniISO[1]->Fill(analysisTree.electron_miniISO[ie],weight);
      }

      sort(LeptMV.begin(), LeptMV.end(),ComparePt);
      if (LeptMV.size() == 0 ) continue; 

       el_index=electrons[0];

      FillMainHists(iCut, EvWeight, ElMV, MuMV, TauMV,JetsMV,METV, analysisTree, SelectionSign);
      CFCounter[iCut]+= weight;
      iCFCounter[iCut]++;
      iCut++;


//*================================================
//in future, if needed to add cone R03 for isolation
//electron_chargedHadIso => r03_sumChargedHadronPt
//electron_neutralHadIso => r03_sumNeutralHadronEt
//electron_phostonIso => r03_sumPhotonEt
//puIso => r03_sumPUPt

//================================================

      vector<int> tau; tau.clear();
      for (unsigned int it = 0; it<analysisTree.tau_count; ++it) {
	tauPtAllH->Fill(analysisTree.tau_pt[it],weight);
	tauEtaAllH->Fill(analysisTree.tau_eta[it],weight);

	if (analysisTree.tau_pt[it] < 20 || fabs(analysisTree.tau_eta[it])> 2.3) continue;
	if (analysisTree.tau_decayModeFinding[it]<decayModeFinding && analysisTree.tau_decayModeFindingNewDMs[it]<decayModeFindingNewDMs) continue;
	if (analysisTree.tau_againstElectronVLooseMVA5[it]<againstElectronVLooseMVA5) continue;
	if (analysisTree.tau_againstMuonTight3[it]<againstMuonTight3) continue;
	if ( fabs(analysisTree.tau_vertexz[it] - analysisTree.primvertex_z ) > vertexz ) continue;
	if (analysisTree.tau_byCombinedIsolationDeltaBetaCorrRaw3Hits[it] > byCombinedIsolationDeltaBetaCorrRaw3Hits ) continue;
	tau.push_back(it);


      }

      tau_index=tau[0];
   
      float dR = deltaR(analysisTree.electron_eta[electrons[0]],analysisTree.electron_phi[electrons[0]],
			    analysisTree.muon_eta[tau[0]],analysisTree.muon_phi[tau[0]]);

      if (dR<dRleptonsCut) continue;

      FillMainHists(iCut, EvWeight, ElMV, MuMV, TauMV,JetsMV,METV, analysisTree, SelectionSign);
      CFCounter[iCut]+= weight;
      iCFCounter[iCut]++;
      iCut++;

      bool ElVeto=false;
      if (doElVeto){

	  for (unsigned int ievv = 0; ievv<analysisTree.electron_count; ++ievv) {

       if ( ievv != el_index ){
	 //   Float_t neutralIsoV = analysisTree.electron_r03_sumNeutralHadronEt[iev] + analysisTree.electron_r03_sumNeutralHadronEt[iev] + analysisTree.electron_r03_sumPhotonEt[iev] -  4*TMath::Pi()*(0.3*0.3)*analysisTree.rho[iev];
	 //   Float_t IsoWithEA =  analysisTree.electron_r03_sumChargedHadronPt[iev] + TMath::Max(Float_t(0), neutralIsoV);
	 //   Float_t relIsoV = IsoWithEA/analysisTree.electron_pt[iev];

	Float_t neutralIso =   analysisTree.electron_neutralHadIso[ievv] + 	  analysisTree.electron_photonIso[ievv] -  0.5*analysisTree.electron_puIso[ievv];
	neutralIso = TMath::Max(Float_t(0),neutralIso); 
	Float_t absIso = analysisTree.electron_chargedHadIso[ievv] + neutralIso;
	Float_t relIsoV = absIso/analysisTree.electron_pt[ievv];
        hel_relISO[1]->Fill(relIsoV,weight);
	if (relIsoV>isoElectronHighCut) continue;
	if (relIsoV<isoElectronLowCut) continue;

             bool ElVetoID = electronVetoTight(analysisTree.electron_superclusterEta[ievv], analysisTree.electron_eta[ievv],analysisTree.electron_phi[ievv],  analysisTree.electron_full5x5_sigmaietaieta[ievv], 
			     analysisTree.electron_ehcaloverecal[ievv],  analysisTree.electron_dxy[ievv], analysisTree.electron_dz[ievv], analysisTree.electron_ooemoop[ievv],
			     relIsoV,analysisTree.electron_nmissinginnerhits[ievv],analysisTree.electron_pass_conversion[ievv]);

	    if ( analysisTree.electron_charge[ievv] != analysisTree.electron_charge[electrons[0]] && analysisTree.electron_pt[ievv] > 15 &&  fabs(analysisTree.electron_eta[ievv]) < 2.5 && fabs(analysisTree.electron_dxy[ievv])<0.045
		&& fabs(analysisTree.electron_dz[ievv]) < 0.2 && relIsoV< 0.3 && ElVetoID) 
		    ElVeto=true;
		  	}
		}
	}

      if (ElVeto) continue;
      FillMainHists(iCut, EvWeight, ElMV, MuMV, TauMV,JetsMV,METV, analysisTree, SelectionSign);
      CFCounter[iCut]+= weight;
      iCFCounter[iCut]++;
      iCut++;


      bool ThirdLeptVeto=false;
      if (doThirdLeptVeto){
  	if (analysisTree.electron_count>0) {
	  for (unsigned int iev = 0; iev<analysisTree.electron_count; ++iev) {
		if (iev !=el_index) {

	 //   Float_t neutralIsoV = analysisTree.electron_r03_sumNeutralHadronEt[iev] + analysisTree.electron_r03_sumNeutralHadronEt[iev] + analysisTree.electron_r03_sumPhotonEt[iev] -  4*TMath::Pi()*(0.3*0.3)*analysisTree.rho[iev];
	 //   Float_t IsoWithEA =  analysisTree.electron_r03_sumChargedHadronPt[iev] + TMath::Max(Float_t(0), neutralIsoV);
	 //   Float_t relIsoV = IsoWithEA/analysisTree.electron_pt[iev];

	Float_t neutralIso = 
	  analysisTree.electron_neutralHadIso[iev] + 	  analysisTree.electron_photonIso[iev] -  0.5*analysisTree.electron_puIso[iev];
	neutralIso = TMath::Max(Float_t(0),neutralIso); 
	Float_t absIso = analysisTree.electron_chargedHadIso[iev] + neutralIso;
	Float_t relIsoV = absIso/analysisTree.electron_pt[iev];
        hel_relISO[1]->Fill(relIsoV,weight);
	if (relIsoV>isoElectronHighCut) continue;
	if (relIsoV<isoElectronLowCut) continue;
             bool ElVetoID = electronVetoTight(analysisTree.electron_superclusterEta[iev], analysisTree.electron_eta[iev],analysisTree.electron_phi[iev],  analysisTree.electron_full5x5_sigmaietaieta[iev], 
			     analysisTree.electron_ehcaloverecal[iev],  analysisTree.electron_dxy[iev], analysisTree.electron_dz[iev], analysisTree.electron_ooemoop[iev],
			     relIsoV,analysisTree.electron_nmissinginnerhits[iev],analysisTree.electron_pass_conversion[iev]);


	    if ( analysisTree.electron_pt[iev] > 10 &&  fabs(analysisTree.electron_eta[iev]) < 2.5 && fabs(analysisTree.electron_dxy[iev])<0.045
		&& fabs(analysisTree.electron_dz[iev]) < 0.2 && relIsoV< 0.3 && ElVetoID) ThirdLeptVeto=true;

	  }
	}

	}

     	if (analysisTree.muon_count>0){
	  for (unsigned int imvv = 0; imvv<analysisTree.muon_count; ++imvv) {
	    Float_t neutralIso = 
	      analysisTree.muon_neutralHadIso[imvv] + 
	      analysisTree.muon_photonIso[imvv] - 
	      0.5*analysisTree.muon_puIso[imvv];
	    neutralIso = TMath::Max(Float_t(0),neutralIso); 
	    Float_t absIso = analysisTree.muon_chargedHadIso[imvv] + neutralIso;
	    Float_t relIso = absIso/analysisTree.muon_pt[imvv];
	    if ( analysisTree.muon_isMedium[imvv] &&  analysisTree.muon_pt[imvv]> 10 &&  fabs(analysisTree.muon_eta[imvv])< 2.4 && fabs(analysisTree.muon_dxy[imvv])<0.045 
		 && fabs(analysisTree.muon_dz[imvv] < 0.2 && relIso< 0.3 && analysisTree.muon_isMedium[imvv]) )
	
	      ThirdLeptVeto=true;
	}

      }
      }
      if (ThirdLeptVeto) continue;



      FillMainHists(iCut, EvWeight, ElMV, MuMV, TauMV,JetsMV,METV, analysisTree, SelectionSign);
      CFCounter[iCut]+= weight;
      iCFCounter[iCut]++;
      iCut++;
      //	for (unsigned int j=0;j<LeptMV.size();++j) cout<<" j "<<j<<"  "<<LeptMV.at(j).Pt()<<endl;
      //	cout<<""<<endl;
      ////////jets cleaning 
      Float_t DRmax=0.4;
      vector<int> jets; jets.clear();
      TLorentzVector leptonsV, muonJ, jetsLV;
      
      //      continue;
      
      //JetsV.SetPxPyPzE(analysisTree.pfjet_px[ij], analysisTree.pfjet_py[ij], analysisTree.pfjet_pz[ij], analysisTree.pfjet_e[ij]);
      for (unsigned int il = 0; il<LeptMV.size(); ++il) {
      
	for (unsigned int ij = 0; ij<JetsMV.size(); ++ij) {
        
	  if(fabs(JetsMV.at(ij).Eta())>etaJetCut) continue;
	  if(fabs(JetsMV.at(ij).Pt())<ptJetCut) continue;
      
	  Float_t Dr= deltaR(LeptMV.at(il).Eta(), LeptMV.at(il).Phi(),JetsMV.at(ij).Eta(),JetsMV.at(ij).Phi());

	  if (  Dr  < DRmax) {
	     
	    JetsMV.erase (JetsMV.begin()+ij);
	  }	
		       
	}
      }
      


      // selecting muon and electron pair (OS or SS);
      Float_t ptScalarSum = -1;
      



      bool btagged= false;
      for (unsigned int ib = 0; ib <analysisTree.pfjet_count;ib++){
	if (analysisTree.pfjet_btag[ib][6]  > bTag) btagged = true;
	//cout<<" pfjet_b "<<ib<<"  "<<analysisTree.pfjet_btag[ib][6]<<endl;
      }
      if (btagged || JetsMV.size()>3) continue;

      // Jets
      FillMainHists(iCut, EvWeight, ElMV, MuMV, TauMV,JetsMV,METV, analysisTree, SelectionSign);
      CFCounter[iCut]+= weight;
      iCFCounter[iCut]++;
      iCut++;
      // pt Scalar
      // computations of kinematic variables


      Float_t ETmiss = TMath::Sqrt(analysisTree.pfmet_ex*analysisTree.pfmet_ex + analysisTree.pfmet_ey*analysisTree.pfmet_ey);

      // bisector of electron and muon transverse momenta

      // computation of MT variable
      Float_t dPhi=-999; 


      dPhi=dPhiFrom2P( LeptMV.at(0).Px(), LeptMV.at(0).Py(), analysisTree.pfmet_ex,  analysisTree.pfmet_ey );
      //MT = TMath::Sqrt(2*LeptMV.at(0).Pt()*ETmiss*(1-TMath::Cos(dPhi)));


      // filling histograms after dilepton selection

      
      // ETmissH->Fill(ETmiss,weight);
      // MtH->Fill(MT,weight);
      
      if (ETmiss < metcut) continue;
      FillMainHists(iCut, EvWeight, ElMV, MuMV, TauMV,JetsMV,METV, analysisTree, SelectionSign);
      CFCounter[iCut]+= weight;
      iCFCounter[iCut]++;
      iCut++;
       
      if (ETmiss < 2*metcut) continue;
      FillMainHists(iCut, EvWeight, ElMV, MuMV, TauMV,JetsMV,METV, analysisTree, SelectionSign);
      CFCounter[iCut]+= weight;
      iCFCounter[iCut]++;
      iCut++;

      // topological cut
      //if (DZeta<dZetaCut) continue;
      if (dPhi>1) continue; 
       
      FillMainHists(iCut, EvWeight, ElMV, MuMV, TauMV,JetsMV,METV, analysisTree, SelectionSign);
      CFCounter[iCut]+= weight;
      iCFCounter[iCut]++;
      iCut++;
      
      //      std::cout << std::endl;
      
      selEvents++;
      
    } // end of file processing (loop over events in one file)
    nFiles++;
    delete _tree;
    file_->Close();
    delete file_;
  }

  cout << endl << "Finished event loop" << endl;
  for (int i=0;i<CutNumb;++i){
    CFCounter[i] *= Float_t(XSec*Lumi/inputEventsH->GetSum());
    if (iCFCounter[i] <0.2) statUnc[i] =0;
    else statUnc[i] = CFCounter[i]/sqrt(iCFCounter[i]);
  }

  //write out cutflow
  ofstream tfile;
  // TString outname = argv[argc-1];
  TString outname=argv[2];
  TString textfilename = "cutflow_"+outname+"_"+SelectionSign+".txt";
  tfile.open(textfilename);
  tfile << "########################################" << endl;
  //tfile << "Cut efficiency numbers:" << endl;

  //    tfile << " Cut "<<"\t & \t"<<"#Evnts for "<<Lumi/1000<<" fb-1 & \t"<<" Uncertainty \t"<<" cnt\t"<<endl;
  for(int ci = 0; ci < CutNumb; ci++)
    {
      tfile << CutList[ci]<<"\t & \t"
	    << CFCounter[ci]  <<"\t & \t"<< statUnc[ci] <<"\t & \t"<< iCFCounter[ci] << endl;
      CutFlow->SetBinContent(1+ci,CFCounter[ci]);
    }

  tfile.close();
  //ofstream tfile1;
  //TString textfile_Con = "CMG_cutflow_Con_Mu_"+outname+".txt";
  //tfile1.open(textfile_Con);
  //tfile1 << "########################################" << endl;
  //tfile1 << "RCS:" << endl;






  std::cout << std::endl;
  int allEvents = int(inputEventsH->GetEntries());
  std::cout << "Total number of input events    = " << allEvents << std::endl;
  std::cout << "Total number of events in Tree  = " << nEvents << std::endl;
  std::cout << "Total number of selected events = " << selEvents << std::endl;
  std::cout << std::endl;
  
  file->cd(SelectionSign.c_str());
  hxsec->Fill(XSec);
  hxsec->Write();
  inputEventsH->Write();
  CutFlow->Write();

  muonPtAllH->Write();
  electronPtAllH->Write();

  // histograms (dilepton selection)
  electronPtH->Write();  
  electronEtaH ->Write();
  muonPtH ->Write();
  muonEtaH ->Write();

  dileptonMassH ->Write();
  dileptonPtH ->Write();
  dileptonEtaH ->Write();
  dileptondRH ->Write();
  ETmissH ->Write();
  MtH ->Write();
  DZetaH ->Write();

  // histograms (dilepton selection + DZeta cut DZeta)
  electronPtSelH ->Write();
  electronEtaSelH ->Write();
  muonPtSelH  ->Write();
  muonEtaSelH ->Write();

  dileptonMassSelH ->Write();
  dileptonPtSelH ->Write();
  dileptonEtaSelH ->Write();
  dileptondRSelH ->Write();
  ETmissSelH ->Write();
  MtSelH ->Write();
  DZetaSelH ->Write();

  /*
    for(int cj = 0; cj < CutNumb; cj++)
    {
    file->cd("");
    //outf->mkdir(CutList[cj]);
    //outf->cd(CutList[cj]);
    h0JetpT[cj]->Write();
    hnJet[cj]->Write();
    hnOver[cj]->Write();
    hnBJet[cj]->Write();
    hnEl[cj]->Write();
    hElpt[cj]->Write();
    hnMu[cj]->Write();
    hMupt[cj]->Write();
    hLepeta[cj]->Write();
    hMET[cj]->Write();
    hHT[cj]->Write();
    hST[cj]->Write();
    hToppT[cj]->Write();
    hnTop[cj]->Write();
    hWTagpT[cj]->Write();
    hWTagMass[cj]->Write();
    hnW[cj]->Write();
    hWmassTagpT[cj]->Write();
    hWmassTagMass[cj]->Write();
    hnWmass[cj]->Write();
    hdPhiMETLep[cj]->Write();
    hdPhiJMET[cj]->Write();

    }
  */
  
  file->Write();
  file->Close();
  
  delete file;
  
}



