import FWCore.ParameterSet.Config as cms

hltParticleFlowSuperClusterHGCalFromTICLUnseeded = cms.EDProducer("PFECALSuperClusterProducer",
    BeamSpot = cms.InputTag("hltOnlineBeamSpot"),
    ClusteringType = cms.string('Mustache'),
    ESAssociation = cms.InputTag("hltParticleFlowClusterECALUnseeded"),
    EnergyWeight = cms.string('Raw'),
    PFBasicClusterCollectionBarrel = cms.string('particleFlowBasicClusterECALBarrel'),
    PFBasicClusterCollectionEndcap = cms.string(''),
    PFBasicClusterCollectionPreshower = cms.string('particleFlowBasicClusterECALPreshower'),
    PFClusters = cms.InputTag("hltParticleFlowClusterHGCalFromTICLUnseeded"),
    PFSuperClusterCollectionBarrel = cms.string('particleFlowSuperClusterECALBarrel'),
    PFSuperClusterCollectionEndcap = cms.string(''),
    PFSuperClusterCollectionEndcapWithPreshower = cms.string(''),
    applyCrackCorrections = cms.bool(False),
    barrelRecHits = cms.InputTag("hltEcalRecHit","EcalRecHitsEB"),
    doSatelliteClusterMerge = cms.bool(False),
    dropUnseedable = cms.bool(True),
    endcapRecHits = cms.InputTag("hltEcalRecHit","EcalRecHitsEE"),
    etawidth_SuperClusterBarrel = cms.double(0.04),
    etawidth_SuperClusterEndcap = cms.double(0.04),
    isOOTCollection = cms.bool(False),
    mightGet = cms.optional.untracked.vstring,
    phiwidth_SuperClusterBarrel = cms.double(0.6),
    phiwidth_SuperClusterEndcap = cms.double(0.6),
    regressionConfig = cms.PSet(
        applySigmaIetaIphiBug = cms.bool(False),
        eRecHitThreshold = cms.double(1),
        ecalRecHitsEB = cms.InputTag("hltEcalRecHit","EcalRecHitsEB"),
        ecalRecHitsEE = cms.InputTag("hltEcalRecHit","EcalRecHitsEE"),
        hgcalRecHits = cms.InputTag("hltParticleFlowRecHitHGC"),
        isHLT = cms.bool(True),
        isPhaseII = cms.bool(True),
        regressionKeyEB = cms.string('pfscecal_EBCorrection_offline_v2'),
        regressionKeyEE = cms.string('superclus_hgcal_mean_online'),
        uncertaintyKeyEB = cms.string('pfscecal_EBUncertainty_offline_v2'),
        uncertaintyKeyEE = cms.string('superclus_hgcal_sigma_online'),
        vertexCollection = cms.InputTag("")
    ),
    satelliteClusterSeedThreshold = cms.double(50),
    satelliteMajorityFraction = cms.double(0.5),
    seedThresholdIsET = cms.bool(True),
    thresh_PFClusterBarrel = cms.double(0),
    thresh_PFClusterES = cms.double(0),
    thresh_PFClusterEndcap = cms.double(0.15),
    thresh_PFClusterSeedBarrel = cms.double(1),
    thresh_PFClusterSeedEndcap = cms.double(1),
    thresh_SCEt = cms.double(10.0),
    useDynamicDPhiWindow = cms.bool(True),
    useRegression = cms.bool(True),
    verbose = cms.untracked.bool(False)
)
