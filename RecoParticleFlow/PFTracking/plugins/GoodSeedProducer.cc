// -*- C++ -*-
//
// Package:    PFTracking
// Class:      GoodSeedProducer
//
// Original Author:  Michele Pioppi
// March 2010. F. Beaudette. Produce PreId information

/// \brief Abstract
/*!
\author Michele Pioppi
\date January 2007

 GoodSeedProducer is the class
 for electron preidentification in PFLow FW.
 It reads refitted tracks and PFCluster collection, 
 and following some criteria divides electrons from hadrons.
 Then it saves the seed of the tracks preidentified as electrons.
 It also transform  all the tracks in the first PFRecTrack collection.
*/

#include "CommonTools/BaseParticlePropagator/interface/BaseParticlePropagator.h"
#include "CommonTools/MVAUtils/interface/GBRForestTools.h"
#include "CondFormats/GBRForest/interface/GBRForest.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/EgammaReco/interface/ElectronSeed.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PreId.h"
#include "DataFormats/ParticleFlowReco/interface/PreIdFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrajectorySeed/interface/PropagationDirection.h"
#include "DataFormats/TrajectorySeed/interface/TrajectorySeedCollection.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "RecoParticleFlow/PFClusterTools/interface/PFResolutionMap.h"
#include "RecoParticleFlow/PFTracking/interface/PFGeometry.h"
#include "RecoParticleFlow/PFTracking/interface/PFTrackTransformer.h"
#include "RecoTracker/TransientTrackingRecHit/interface/TkTransientTrackingRecHitBuilder.h"
#include "TrackingTools/PatternTools/interface/Trajectory.h"
#include "TrackingTools/PatternTools/interface/TrajectorySmoother.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "TrackingTools/Records/interface/TransientRecHitRecord.h"
#include "TrackingTools/TrackFitters/interface/TrajectoryFitter.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"

#include "TMath.h"
#include "Math/VectorUtil.h"

#include <fstream>

namespace goodseedhelpers {
  struct HeavyObjectCache {
    constexpr static unsigned int kMaxWeights = 9;
    HeavyObjectCache(const edm::ParameterSet& conf);
    std::array<std::unique_ptr<const GBRForest>, kMaxWeights> gbr;
  };
}  // namespace goodseedhelpers

class GoodSeedProducer final : public edm::stream::EDProducer<edm::GlobalCache<goodseedhelpers::HeavyObjectCache>> {
  typedef TrajectoryStateOnSurface TSOS;

public:
  explicit GoodSeedProducer(const edm::ParameterSet&, const goodseedhelpers::HeavyObjectCache*);

  static std::unique_ptr<goodseedhelpers::HeavyObjectCache> initializeGlobalCache(const edm::ParameterSet& conf) {
    return std::make_unique<goodseedhelpers::HeavyObjectCache>(conf);
  }

  static void globalEndJob(goodseedhelpers::HeavyObjectCache const*) {}
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void beginRun(const edm::Run& run, const edm::EventSetup&) override;
  void produce(edm::Event&, const edm::EventSetup&) override;

  ///Find the bin in pt and eta
  int getBin(float, float);

  void fillPreIdRefValueMap(edm::Handle<reco::TrackCollection> tkhandle,
                            const edm::OrphanHandle<reco::PreIdCollection>&,
                            edm::ValueMap<reco::PreIdRef>::Filler& filler);
  // ----------member data ---------------------------

  ///Name of the Seed(Ckf) Collection
  std::string preidckf_;

  ///Name of the Seed(Gsf) Collection
  std::string preidgsf_;

  ///Name of the preid Collection (FB)
  std::string preidname_;

  // needed by the above
  TkClonerImpl hitCloner;

  ///PFTrackTransformer
  std::unique_ptr<PFTrackTransformer> pfTransformer_;

  ///Number of hits in the seed;
  int nHitsInSeed_;

  ///Minimum transverse momentum and maximum pseudorapidity
  double minPt_;
  double maxPt_;
  double maxEta_;

  double HcalIsolWindow_;
  double EcalStripSumE_minClusEnergy_;
  double EcalStripSumE_deltaEta_;
  double EcalStripSumE_deltaPhiOverQ_minValue_;
  double EcalStripSumE_deltaPhiOverQ_maxValue_;
  double minEoverP_;
  double maxHoverP_;

  ///Cut on the energy of the clusters
  double clusThreshold_;

  ///Min and MAx allowed values forEoverP
  double minEp_;
  double maxEp_;

  ///Produce the Seed for Ckf tracks?
  bool produceCkfseed_;

  ///  switch to disable the pre-id
  bool disablePreId_;

  ///Produce the pre-id debugging collection
  bool producePreId_;

  /// Threshold to save Pre Idinfo
  double PtThresholdSavePredId_;

  ///vector of thresholds for different bins of eta and pt
  float thr[150];

  // ----------access to event data
  edm::ParameterSet conf_;
  edm::EDGetTokenT<reco::PFClusterCollection> pfCLusTagPSLabel_;
  edm::EDGetTokenT<reco::PFClusterCollection> pfCLusTagECLabel_;
  edm::EDGetTokenT<reco::PFClusterCollection> pfCLusTagHCLabel_;
  std::vector<edm::EDGetTokenT<std::vector<Trajectory>>> trajContainers_;
  std::vector<edm::EDGetTokenT<reco::TrackCollection>> tracksContainers_;

  std::string fitterName_;
  std::string smootherName_;
  std::string propagatorName_;
  std::string trackerRecHitBuilderName_;

  std::unique_ptr<PFResolutionMap> resMapEtaECAL_;
  std::unique_ptr<PFResolutionMap> resMapPhiECAL_;

  const edm::ESGetToken<TrajectoryFitter, TrajectoryFitter::Record> fitterToken_;
  const edm::ESGetToken<TrajectorySmoother, TrajectoryFitter::Record> smootherToken_;
  const edm::ESGetToken<TransientTrackingRecHitBuilder, TransientRecHitRecord> trackerRecHitBuilderToken_;
  const edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> magneticFieldToken_;
  const edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> magneticFieldTokenBeginRun_;

  ///TRACK QUALITY
  bool useQuality_;
  reco::TrackBase::TrackQuality trackQuality_;

  ///VARIABLES NEEDED FOR TMVA
  float nhit, dpt, chired, chiRatio;
  float chikfred, trk_ecalDeta, trk_ecalDphi;
  double Min_dr_;

  ///USE OF TMVA
  bool useTmva_;

  ///B field
  math::XYZVector B_;

  /// Map used to create the TrackRef, PreIdRef value map
  std::map<reco::TrackRef, unsigned> refMap_;
};

using namespace edm;
using namespace std;
using namespace reco;

void GoodSeedProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<double>("MaxEOverP", 3.0);
  desc.add<std::string>("Smoother", "GsfTrajectorySmoother_forPreId");
  desc.add<bool>("UseQuality", true);
  desc.add<edm::InputTag>("PFPSClusterLabel", {"particleFlowClusterPS"});
  desc.add<std::string>("ThresholdFile", "RecoParticleFlow/PFTracking/data/Threshold.dat");
  desc.add<std::string>("TMVAMethod", "BDT");
  desc.add<double>("MaxEta", 2.4);
  desc.add<std::string>("EtaMap", "RecoParticleFlow/PFBlockProducer/data/resmap_ECAL_eta.dat");
  desc.add<std::string>("PhiMap", "RecoParticleFlow/PFBlockProducer/data/resmap_ECAL_phi.dat");
  desc.add<std::string>("PreCkfLabel", "SeedsForCkf");
  desc.add<int>("NHitsInSeed", 3);
  desc.add<std::string>("Fitter", "GsfTrajectoryFitter_forPreId");
  desc.add<std::string>("TTRHBuilder", "WithAngleAndTemplate");
  desc.add<std::string>("PreGsfLabel", "SeedsForGsf");
  desc.add<double>("MinEOverP", 0.3);
  desc.add<std::string>("Weights1", "RecoParticleFlow/PFTracking/data/MVA_BDTTrackDrivenSeed_cat1.xml");
  desc.add<std::string>("Weights2", "RecoParticleFlow/PFTracking/data/MVA_BDTTrackDrivenSeed_cat2.xml");
  desc.add<std::string>("Weights3", "RecoParticleFlow/PFTracking/data/MVA_BDTTrackDrivenSeed_cat3.xml");
  desc.add<std::string>("Weights4", "RecoParticleFlow/PFTracking/data/MVA_BDTTrackDrivenSeed_cat4.xml");
  desc.add<std::string>("Weights5", "RecoParticleFlow/PFTracking/data/MVA_BDTTrackDrivenSeed_cat5.xml");
  desc.add<std::string>("Weights6", "RecoParticleFlow/PFTracking/data/MVA_BDTTrackDrivenSeed_cat6.xml");
  desc.add<std::string>("Weights7", "RecoParticleFlow/PFTracking/data/MVA_BDTTrackDrivenSeed_cat7.xml");
  desc.add<std::string>("Weights8", "RecoParticleFlow/PFTracking/data/MVA_BDTTrackDrivenSeed_cat8.xml");
  desc.add<std::string>("Weights9", "RecoParticleFlow/PFTracking/data/MVA_BDTTrackDrivenSeed_cat9.xml");
  desc.add<edm::InputTag>("PFEcalClusterLabel", {"particleFlowClusterECAL"});
  desc.add<edm::InputTag>("PFHcalClusterLabel", {"particleFlowClusterHCAL"}),
      desc.add<std::string>("PSThresholdFile", "RecoParticleFlow/PFTracking/data/PSThreshold.dat");
  desc.add<double>("MinPt", 2.0);
  desc.add<std::vector<edm::InputTag>>("TkColList", {edm::InputTag("generalTracks")});
  desc.addUntracked<bool>("UseTMVA", true);
  desc.add<std::string>("TrackQuality", "highPurity");
  desc.add<double>("MaxPt", 50.0);
  desc.add<bool>("ApplyIsolation", false);
  desc.add<double>("EcalStripSumE_deltaPhiOverQ_minValue", -0.1);
  desc.add<double>("EcalStripSumE_minClusEnergy", 0.1);
  desc.add<double>("EcalStripSumE_deltaEta", 0.03);
  desc.add<double>("EcalStripSumE_deltaPhiOverQ_maxValue", 0.5);
  desc.add<double>("EOverPLead_minValue", 0.95);
  desc.add<double>("HOverPLead_maxValue", 0.05);
  desc.add<double>("HcalWindow", 0.184);
  desc.add<double>("ClusterThreshold", 0.5);
  desc.add<bool>("UsePreShower", false);
  desc.add<std::string>("PreIdLabel", "preid");
  desc.addUntracked<bool>("ProducePreId", true);
  desc.addUntracked<double>("PtThresholdSavePreId", 1.0);
  desc.add<double>("Min_dr", 0.2);
  descriptions.addWithDefaultLabel(desc);
}

GoodSeedProducer::GoodSeedProducer(const ParameterSet& iConfig, const goodseedhelpers::HeavyObjectCache*)
    : pfTransformer_(nullptr),
      conf_(iConfig),
      resMapEtaECAL_(nullptr),
      resMapPhiECAL_(nullptr),
      fitterToken_(esConsumes(edm::ESInputTag("", iConfig.getParameter<string>("Fitter")))),
      smootherToken_(esConsumes(edm::ESInputTag("", iConfig.getParameter<string>("Smoother")))),
      trackerRecHitBuilderToken_(esConsumes(edm::ESInputTag("", iConfig.getParameter<std::string>("TTRHBuilder")))),
      magneticFieldToken_(esConsumes()),
      magneticFieldTokenBeginRun_(esConsumes<edm::Transition::BeginRun>()) {
  LogInfo("GoodSeedProducer") << "Electron PreIdentification started  ";

  //now do what ever initialization is needed
  std::vector<edm::InputTag> tags = iConfig.getParameter<vector<InputTag>>("TkColList");
  for (unsigned int i = 0; i < tags.size(); ++i) {
    trajContainers_.push_back(consumes<vector<Trajectory>>(tags[i]));
    tracksContainers_.push_back(consumes<reco::TrackCollection>(tags[i]));
  }

  minPt_ = iConfig.getParameter<double>("MinPt");
  maxPt_ = iConfig.getParameter<double>("MaxPt");
  maxEta_ = iConfig.getParameter<double>("MaxEta");

  HcalIsolWindow_ = iConfig.getParameter<double>("HcalWindow");
  EcalStripSumE_minClusEnergy_ = iConfig.getParameter<double>("EcalStripSumE_minClusEnergy");
  EcalStripSumE_deltaEta_ = iConfig.getParameter<double>("EcalStripSumE_deltaEta");
  EcalStripSumE_deltaPhiOverQ_minValue_ = iConfig.getParameter<double>("EcalStripSumE_deltaPhiOverQ_minValue");
  EcalStripSumE_deltaPhiOverQ_maxValue_ = iConfig.getParameter<double>("EcalStripSumE_deltaPhiOverQ_maxValue");
  minEoverP_ = iConfig.getParameter<double>("EOverPLead_minValue");
  maxHoverP_ = iConfig.getParameter<double>("HOverPLead_maxValue");

  pfCLusTagECLabel_ = consumes<reco::PFClusterCollection>(iConfig.getParameter<InputTag>("PFEcalClusterLabel"));

  pfCLusTagHCLabel_ = consumes<reco::PFClusterCollection>(iConfig.getParameter<InputTag>("PFHcalClusterLabel"));

  pfCLusTagPSLabel_ = consumes<reco::PFClusterCollection>(iConfig.getParameter<InputTag>("PFPSClusterLabel"));

  preidgsf_ = iConfig.getParameter<string>("PreGsfLabel");
  preidckf_ = iConfig.getParameter<string>("PreCkfLabel");
  preidname_ = iConfig.getParameter<string>("PreIdLabel");

  fitterName_ = iConfig.getParameter<string>("Fitter");
  smootherName_ = iConfig.getParameter<string>("Smoother");

  nHitsInSeed_ = iConfig.getParameter<int>("NHitsInSeed");

  clusThreshold_ = iConfig.getParameter<double>("ClusterThreshold");

  minEp_ = iConfig.getParameter<double>("MinEOverP");
  maxEp_ = iConfig.getParameter<double>("MaxEOverP");

  //collection to produce
  produceCkfseed_ = iConfig.getUntrackedParameter<bool>("ProduceCkfSeed", false);

  // to disable the electron part (for HI collisions for examples)
  disablePreId_ = iConfig.getUntrackedParameter<bool>("DisablePreId", false);

  producePreId_ = iConfig.getUntrackedParameter<bool>("ProducePreId", true);
  //  if no electron, cannot produce the preid
  if (disablePreId_)
    producePreId_ = false;
  PtThresholdSavePredId_ = iConfig.getUntrackedParameter<double>("PtThresholdSavePreId", 1.);

  LogDebug("GoodSeedProducer") << "Seeds for GSF will be produced ";

  // no disablePreId_ switch here. The collection will be empty if it is true
  produces<ElectronSeedCollection>(preidgsf_);

  if (produceCkfseed_) {
    LogDebug("GoodSeedProducer") << "Seeds for CKF will be produced ";
    produces<TrajectorySeedCollection>(preidckf_);
  }

  if (producePreId_) {
    LogDebug("GoodSeedProducer") << "PreId debugging information will be produced ";

    produces<PreIdCollection>(preidname_);
    if (tracksContainers_.size() == 1)  // do not make a value map if more than one input track collection
      produces<edm::ValueMap<reco::PreIdRef>>(preidname_);
  }

  useQuality_ = iConfig.getParameter<bool>("UseQuality");
  trackQuality_ = TrackBase::qualityByName(iConfig.getParameter<std::string>("TrackQuality"));

  useTmva_ = iConfig.getUntrackedParameter<bool>("UseTMVA", false);

  Min_dr_ = iConfig.getParameter<double>("Min_dr");

  trackerRecHitBuilderName_ = iConfig.getParameter<std::string>("TTRHBuilder");
}

//
// member functions
//

// ------------ method called to produce the data  ------------
void GoodSeedProducer::produce(Event& iEvent, const EventSetup& iSetup) {
  LogDebug("GoodSeedProducer") << "START event: " << iEvent.id().event() << " in run " << iEvent.id().run();
  //Create empty output collections
  auto output_preid = std::make_unique<ElectronSeedCollection>();
  auto output_nopre = std::make_unique<TrajectorySeedCollection>();
  auto output_preidinfo = std::make_unique<PreIdCollection>();
  auto preIdMap_p = std::make_unique<edm::ValueMap<reco::PreIdRef>>();
  edm::ValueMap<reco::PreIdRef>::Filler mapFiller(*preIdMap_p);

  std::unique_ptr<TrajectoryFitter> fitter;
  std::unique_ptr<TrajectorySmoother> smoother;

  //Tracking Tools
  if (!disablePreId_) {
    auto const& aFitter = &iSetup.getData(fitterToken_);
    auto const& aSmoother = &iSetup.getData(smootherToken_);

    smoother.reset(aSmoother->clone());
    fitter = aFitter->clone();
    auto const& theTrackerRecHitBuilder = &iSetup.getData(trackerRecHitBuilderToken_);
    hitCloner = static_cast<TkTransientTrackingRecHitBuilder const*>(theTrackerRecHitBuilder)->cloner();
    fitter->setHitCloner(&hitCloner);
    smoother->setHitCloner(&hitCloner);
  }

  // clear temporary maps
  refMap_.clear();

  //Magnetic Field
  auto const& magneticField = &iSetup.getData(magneticFieldToken_);

  //Handle input collections
  //ECAL clusters
  Handle<PFClusterCollection> theECPfClustCollection;
  iEvent.getByToken(pfCLusTagECLabel_, theECPfClustCollection);

  vector<PFCluster const*> basClus;
  for (auto const& klus : *theECPfClustCollection.product()) {
    if (klus.correctedEnergy() > clusThreshold_)
      basClus.push_back(&klus);
  }

  //HCAL clusters
  Handle<PFClusterCollection> theHCPfClustCollection;
  iEvent.getByToken(pfCLusTagHCLabel_, theHCPfClustCollection);

  //PS clusters
  Handle<PFClusterCollection> thePSPfClustCollection;
  iEvent.getByToken(pfCLusTagPSLabel_, thePSPfClustCollection);

  //Vector of track collections
  for (unsigned int istr = 0; istr < tracksContainers_.size(); ++istr) {
    //Track collection
    Handle<TrackCollection> tkRefCollection;
    iEvent.getByToken(tracksContainers_[istr], tkRefCollection);
    const TrackCollection& Tk = *(tkRefCollection.product());

    LogDebug("GoodSeedProducer") << "Number of tracks in collection "
                                 << "tracksContainers_[" << istr << "] to be analyzed " << Tk.size();

    //loop over the track collection
    for (unsigned int i = 0; i < Tk.size(); ++i) {
      if (useQuality_ && (!(Tk[i].quality(trackQuality_))))
        continue;

      reco::PreId myPreId;
      bool GoodPreId = false;

      TrackRef trackRef(tkRefCollection, i);
      math::XYZVectorF tkmom(Tk[i].momentum());
      auto tketa = tkmom.eta();
      auto tkpt = std::sqrt(tkmom.perp2());
      auto const& Seed = (*trackRef->seedRef());

      if (!disablePreId_) {
        int ipteta = getBin(Tk[i].eta(), Tk[i].pt());
        int ibin = ipteta * 9;

        // FIXME the original code was buggy should be outerMomentum...
        float oPTOB = 1.f / std::sqrt(Tk[i].innerMomentum().mag2());
        //  float chikfred=Tk[i].normalizedChi2();
        float nchi = Tk[i].normalizedChi2();

        int nhitpi = Tk[i].found();
        float EP = 0;

        // set track info
        myPreId.setTrack(trackRef);
        //CLUSTERS - TRACK matching

        auto pfmass = 0.0005;
        auto pfoutenergy = sqrt((pfmass * pfmass) + Tk[i].outerMomentum().Mag2());

        XYZTLorentzVector mom = XYZTLorentzVector(
            Tk[i].outerMomentum().x(), Tk[i].outerMomentum().y(), Tk[i].outerMomentum().z(), pfoutenergy);
        XYZTLorentzVector pos =
            XYZTLorentzVector(Tk[i].outerPosition().x(), Tk[i].outerPosition().y(), Tk[i].outerPosition().z(), 0.);

        BaseParticlePropagator theOutParticle(RawParticle(mom, pos, Tk[i].charge()), 0, 0, B_.z());

        theOutParticle.propagateToEcalEntrance(false);

        float toteta = 1000.f;
        float totphi = 1000.f;
        float dr = 1000.f;
        float EE = 0.f;
        float feta = 0.f;
        GlobalPoint ElecTrkEcalPos(0, 0, 0);

        PFClusterRef clusterRef;
        math::XYZPoint meanShowerSaved;
        if (theOutParticle.getSuccess() != 0) {
          ElecTrkEcalPos = GlobalPoint(theOutParticle.particle().vertex().x(),
                                       theOutParticle.particle().vertex().y(),
                                       theOutParticle.particle().vertex().z());

          constexpr float psLim = 2.50746495928f;  // std::sinh(1.65f);
          bool isBelowPS = (ElecTrkEcalPos.z() * ElecTrkEcalPos.z()) > (psLim * psLim) * ElecTrkEcalPos.perp2();
          // bool isBelowPS=(std::abs(ElecTrkEcalPos.eta())>1.65f);

          unsigned clusCounter = 0;
          float max_ee = 0;
          for (auto aClus : basClus) {
            float tmp_ep = float(aClus->correctedEnergy()) * oPTOB;
            if ((tmp_ep < minEp_) | (tmp_ep > maxEp_)) {
              ++clusCounter;
              continue;
            }

            double ecalShowerDepth = PFCluster::getDepthCorrection(aClus->correctedEnergy(), isBelowPS, false);
            auto mom = theOutParticle.particle().momentum().Vect();
            auto meanShower = ElecTrkEcalPos + GlobalVector(mom.x(), mom.y(), mom.z()).unit() * ecalShowerDepth;

            float etarec = meanShower.eta();
            float phirec = meanShower.phi();

            float tmp_phi = std::abs(aClus->positionREP().phi() - phirec);
            if (tmp_phi > float(TMath::Pi()))
              tmp_phi -= float(TMath::TwoPi());

            float tmp_dr = std::sqrt(std::pow(tmp_phi, 2.f) + std::pow(aClus->positionREP().eta() - etarec, 2.f));

            if (tmp_dr < dr) {
              dr = tmp_dr;
              if (dr < Min_dr_) {  // find the most closest and energetic ECAL cluster
                if (aClus->correctedEnergy() > max_ee) {
                  toteta = aClus->positionREP().eta() - etarec;
                  totphi = tmp_phi;
                  EP = tmp_ep;
                  EE = aClus->correctedEnergy();
                  feta = aClus->positionREP().eta();
                  clusterRef = PFClusterRef(theECPfClustCollection, clusCounter);
                  meanShowerSaved = meanShower;
                }
              }
            }
            ++clusCounter;
          }
        }
        float trk_ecalDeta_ = fabs(toteta);
        float trk_ecalDphi_ = fabs(totphi);

        //Resolution maps
        auto ecaletares = resMapEtaECAL_->GetBinContent(resMapEtaECAL_->FindBin(feta, EE));
        auto ecalphires = resMapPhiECAL_->GetBinContent(resMapPhiECAL_->FindBin(feta, EE));

        //geomatrical compatibility
        float chieta = (toteta != 1000.f) ? toteta / ecaletares : toteta;
        float chiphi = (totphi != 1000.f) ? totphi / ecalphires : totphi;
        float chichi = sqrt(chieta * chieta + chiphi * chiphi);

        //Matching criteria
        float eta_cut = thr[ibin + 0];
        float phi_cut = thr[ibin + 1];
        float ep_cutmin = thr[ibin + 2];
        bool GoodMatching =
            ((trk_ecalDeta_ < eta_cut) && (trk_ecalDphi_ < phi_cut) && (EP > ep_cutmin) && (nhitpi > 10));

        bool EcalMatching = GoodMatching;

        if (tkpt > maxPt_)
          GoodMatching = true;
        if (tkpt < minPt_)
          GoodMatching = false;

        math::XYZPoint myPoint(ElecTrkEcalPos.x(), ElecTrkEcalPos.y(), ElecTrkEcalPos.z());
        myPreId.setECALMatchingProperties(
            clusterRef, myPoint, meanShowerSaved, std::abs(toteta), std::abs(totphi), chieta, chiphi, chichi, EP);
        myPreId.setECALMatching(EcalMatching);

        bool GoodRange = ((std::abs(tketa) < maxEta_) & (tkpt > minPt_));
        //KF FILTERING FOR UNMATCHED EVENTS
        int hit1max = int(thr[ibin + 3]);
        float chiredmin = thr[ibin + 4];
        bool GoodKFFiltering = ((nchi > chiredmin) | (nhitpi < hit1max));

        myPreId.setTrackFiltering(GoodKFFiltering);

        bool GoodTkId = false;

        if ((!GoodMatching) && (GoodKFFiltering) && (GoodRange)) {
          chired = 1000;
          chiRatio = 1000;
          dpt = 0;
          nhit = nhitpi;
          chikfred = nchi;
          trk_ecalDeta = trk_ecalDeta_;
          trk_ecalDphi = trk_ecalDphi_;

          Trajectory::ConstRecHitContainer tmp;
          for (auto const& hit : Tk[i].recHits())
            tmp.push_back(hit->cloneSH());
          auto const& theTrack = Tk[i];
          GlobalVector gv(theTrack.innerMomentum().x(), theTrack.innerMomentum().y(), theTrack.innerMomentum().z());
          GlobalPoint gp(theTrack.innerPosition().x(), theTrack.innerPosition().y(), theTrack.innerPosition().z());
          GlobalTrajectoryParameters gtps(gp, gv, theTrack.charge(), &*magneticField);
          TrajectoryStateOnSurface tsos(gtps, theTrack.innerStateCovariance(), *tmp[0]->surface());
          Trajectory&& FitTjs = fitter->fitOne(Seed, tmp, tsos);

          if (FitTjs.isValid()) {
            Trajectory&& SmooTjs = smoother->trajectory(FitTjs);
            if (SmooTjs.isValid()) {
              //Track refitted with electron hypothesis

              float pt_out = SmooTjs.firstMeasurement().updatedState().globalMomentum().perp();
              float pt_in = SmooTjs.lastMeasurement().updatedState().globalMomentum().perp();
              dpt = (pt_in > 0) ? fabs(pt_out - pt_in) / pt_in : 0.;
              // the following is simply the number of degrees of freedom
              chiRatio = SmooTjs.chiSquared() / Tk[i].chi2();
              chired = chiRatio * chikfred;
            }
          }

          //TMVA Analysis
          if (useTmva_) {
            float vars[10] = {nhit, chikfred, dpt, EP, chiRatio, chired, trk_ecalDeta, trk_ecalDphi, tkpt, tketa};

            float Ytmva = globalCache()->gbr[ipteta]->GetClassifier(vars);

            float BDTcut = thr[ibin + 5];
            if (Ytmva > BDTcut)
              GoodTkId = true;
            myPreId.setMVA(GoodTkId, Ytmva);
            myPreId.setTrackProperties(chired, chiRatio, dpt);
          } else {
            float chiratiocut = thr[ibin + 6];
            float gschicut = thr[ibin + 7];
            float gsptmin = thr[ibin + 8];

            GoodTkId = ((dpt > gsptmin) & (chired < gschicut) & (chiRatio < chiratiocut));
          }
        }

        GoodPreId = GoodTkId | GoodMatching;

        myPreId.setFinalDecision(GoodPreId);

#ifdef EDM_ML_DEBUG
        if (GoodPreId)
          LogDebug("GoodSeedProducer") << "Track (pt= " << Tk[i].pt() << "GeV/c, eta= " << Tk[i].eta()
                                       << ") preidentified for agreement between  track and ECAL cluster";
        if (GoodPreId && (!GoodMatching))
          LogDebug("GoodSeedProducer") << "Track (pt= " << Tk[i].pt() << "GeV/c, eta= " << Tk[i].eta()
                                       << ") preidentified only for track properties";
#endif

      }  // end of !disablePreId_

      if (GoodPreId) {
        //NEW SEED with n hits
        ElectronSeed NewSeed(Seed);
        NewSeed.setCtfTrack(trackRef);
        output_preid->push_back(NewSeed);
      } else {
        if (produceCkfseed_) {
          output_nopre->push_back(Seed);
        }
      }
      if (producePreId_ && myPreId.pt() > PtThresholdSavePredId_) {
        // save the index of the PreId object as to be able to create a Ref later
        refMap_[trackRef] = output_preidinfo->size();
        output_preidinfo->push_back(myPreId);
      }
    }  //end loop on track collection
  }  //end loop on the vector of track collections

  // no disablePreId_ switch, it is simpler to have an empty collection rather than no collection
  iEvent.put(std::move(output_preid), preidgsf_);
  if (produceCkfseed_)
    iEvent.put(std::move(output_nopre), preidckf_);
  if (producePreId_) {
    const edm::OrphanHandle<reco::PreIdCollection> preIdRefProd = iEvent.put(std::move(output_preidinfo), preidname_);
    // now make the Value Map, but only if one input collection
    if (tracksContainers_.size() == 1) {
      Handle<TrackCollection> tkRefCollection;
      iEvent.getByToken(tracksContainers_[0], tkRefCollection);
      fillPreIdRefValueMap(tkRefCollection, preIdRefProd, mapFiller);
      mapFiller.fill();
      iEvent.put(std::move(preIdMap_p), preidname_);
    }
  }
  // clear temporary maps
  refMap_.clear();
}

// intialize the cross-thread cache to hold gbr trees and resolution maps
namespace goodseedhelpers {
  HeavyObjectCache::HeavyObjectCache(const edm::ParameterSet& conf) {
    // mvas
    const bool useTmva = conf.getUntrackedParameter<bool>("UseTMVA", false);

    if (useTmva) {
      std::array<edm::FileInPath, kMaxWeights> weights = {{edm::FileInPath(conf.getParameter<string>("Weights1")),
                                                           edm::FileInPath(conf.getParameter<string>("Weights2")),
                                                           edm::FileInPath(conf.getParameter<string>("Weights3")),
                                                           edm::FileInPath(conf.getParameter<string>("Weights4")),
                                                           edm::FileInPath(conf.getParameter<string>("Weights5")),
                                                           edm::FileInPath(conf.getParameter<string>("Weights6")),
                                                           edm::FileInPath(conf.getParameter<string>("Weights7")),
                                                           edm::FileInPath(conf.getParameter<string>("Weights8")),
                                                           edm::FileInPath(conf.getParameter<string>("Weights9"))}};

      for (UInt_t j = 0; j < gbr.size(); ++j) {
        gbr[j] = createGBRForest(weights[j]);
      }
    }
  }
}  // namespace goodseedhelpers

// ------------ method called once each job just before starting event loop  ------------
void GoodSeedProducer::beginRun(const edm::Run& run, const EventSetup& es) {
  //Magnetic Field
  auto const& magneticField = &es.getData(magneticFieldTokenBeginRun_);
  B_ = magneticField->inTesla(GlobalPoint(0, 0, 0));

  pfTransformer_ = std::make_unique<PFTrackTransformer>(B_);
  pfTransformer_->OnlyProp();

  //Resolution maps
  FileInPath ecalEtaMap(conf_.getParameter<string>("EtaMap"));
  FileInPath ecalPhiMap(conf_.getParameter<string>("PhiMap"));
  resMapEtaECAL_ = std::make_unique<PFResolutionMap>("ECAL_eta", ecalEtaMap.fullPath().c_str());
  resMapPhiECAL_ = std::make_unique<PFResolutionMap>("ECAL_phi", ecalPhiMap.fullPath().c_str());

  //read threshold
  FileInPath parFile(conf_.getParameter<string>("ThresholdFile"));
  ifstream ifs(parFile.fullPath().c_str());
  for (int iy = 0; iy < 81; ++iy)
    ifs >> thr[iy];
}

int GoodSeedProducer::getBin(float eta, float pt) {
  int ie = 0;
  int ip = 0;
  if (fabs(eta) < 0.8)
    ie = 0;
  else {
    if (fabs(eta) < 1.479)
      ie = 1;
    else
      ie = 2;
  }
  if (pt < 6)
    ip = 0;
  else {
    if (pt < 12)
      ip = 1;
    else
      ip = 2;
  }
  int iep = ie * 3 + ip;
  LogDebug("GoodSeedProducer") << "Track pt =" << pt << " eta=" << eta << " bin=" << iep;
  return iep;
}

void GoodSeedProducer::fillPreIdRefValueMap(Handle<TrackCollection> tracks,
                                            const edm::OrphanHandle<reco::PreIdCollection>& preidhandle,
                                            edm::ValueMap<reco::PreIdRef>::Filler& filler) {
  std::vector<reco::PreIdRef> values;

  unsigned ntracks = tracks->size();
  for (unsigned itrack = 0; itrack < ntracks; ++itrack) {
    reco::TrackRef theTrackRef(tracks, itrack);
    std::map<reco::TrackRef, unsigned>::const_iterator itcheck = refMap_.find(theTrackRef);
    if (itcheck == refMap_.end()) {
      // the track has been early discarded
      values.push_back(reco::PreIdRef());
    } else {
      edm::Ref<reco::PreIdCollection> preIdRef(preidhandle, itcheck->second);
      values.push_back(preIdRef);
      //	 std::cout << " Checking Refs " << (theTrackRef==preIdRef->trackRef()) << std::endl;
    }
  }
  filler.insert(tracks, values.begin(), values.end());
}

DEFINE_FWK_MODULE(GoodSeedProducer);
