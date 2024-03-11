import FWCore.ParameterSet.Config as cms

hltFixedGridRhoProducerFastjetAllTau = cms.EDProducer("FixedGridRhoProducerFastjet",
    gridSpacing = cms.double(0.55),
    maxRapidity = cms.double(5.0),
    pfCandidatesTag = cms.InputTag("particleFlowTmp")
)
