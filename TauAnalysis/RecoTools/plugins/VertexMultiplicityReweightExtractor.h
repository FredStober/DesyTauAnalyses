#ifndef TauAnalysis_RecoTools_VertexMultiplicityReweightExtractor_h
#define TauAnalysis_RecoTools_VertexMultiplicityReweightExtractor_h

/** \class VertexMultiplicityReweightProducer
 *
 * Reweight Monte Carlo events simulated with pile-up to match the
 * vertex multiplicity distribution observed in the analyzed data sample
 *
 * NOTE:
 *      o weight > 1: fraction of events which given vertex multiplicity higher in data than in (pile-up) MC
 *      o weight = 1: fraction of events which given vertex multiplicity same   in data than in (pile-up) MC
 *      o weight < 1: fraction of events which given vertex multiplicity lower  in data than in (pile-up) MC
 *
 * \authors Christian Veelken
 *
 * \version $Revision: 1.5 $
 *
 * $Id: VertexMultiplicityReweightExtractor.h,v 1.5 2012/04/24 07:21:49 veelken Exp $
 *
 */

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "CommonTools/Utils/interface/StringObjectFunction.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
#include "PhysicsTools/Utilities/interface/Lumi3DReWeighting.h"

#include "TauAnalysis/BgEstimationTools/interface/ObjValExtractorBase.h"

#include <TFile.h>
#include <TH1.h>

class VertexMultiplicityReweightExtractor : public ObjValExtractorBase
{
 public:
  explicit VertexMultiplicityReweightExtractor(const edm::ParameterSet&);
  ~VertexMultiplicityReweightExtractor();

  double operator()(const edm::Event&) const;

 private:
  edm::InputTag src_;
  
  TFile* inputFile_;
  TH1* puDist_data_;

  edm::LumiReWeighting* genLumiReweight_;
  edm::Lumi3DReWeighting* genLumiReweight3d_;

  TH1* recVtxMultiplicityReweight_;

  int bxPrevious_;
  int bxNext_;

  enum { kUndefined, kGenLevel, kGenLevel3d, kRecLevel };
  int type_;
};

#endif

