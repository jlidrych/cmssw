// -*- C++ -*-
//
// Package:    SiStripQualityStatistics
// Class:      SiStripQualityStatistics
//
/**\class SiStripQualityStatistics SiStripQualityStatistics.h CalibTracker/SiStripQuality/plugins/SiStripQualityStatistics.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Domenico GIORDANO
//         Created:  Wed Oct  3 12:11:10 CEST 2007
//
//

// system include files
#include <memory>
#include <vector>

// user include files
#include "CalibFormats/SiStripObjects/interface/SiStripQuality.h"
#include "CalibTracker/SiStripCommon/interface/SiStripDetInfoFileReader.h"
#include "CalibTracker/SiStripQuality/interface/SiStripQualityWithFromFedErrorsHelper.h"
#include "CommonTools/TrackerMap/interface/TrackerMap.h"
#include "DQM/SiStripCommon/interface/TkHistoMap.h"
#include "DQMServices/Core/interface/DQMEDHarvester.h"
#include "DataFormats/SiStripDetId/interface/StripSubdetector.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "Geometry/Records/interface/TrackerTopologyRcd.h"

class SiStripFedCabling;

class SiStripQualityStatistics : public DQMEDHarvester {
public:
  explicit SiStripQualityStatistics(const edm::ParameterSet&);
  ~SiStripQualityStatistics() override = default;

  void endRun(edm::Run const&, edm::EventSetup const&) override;
  void dqmEndJob(DQMStore::IBooker&, DQMStore::IGetter&) override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void updateAndSave(const SiStripQuality* siStripQuality);
  void SetBadComponents(int, int, SiStripQuality::BadComponent&);

  edm::RunID run_;
  std::string dataLabel_;
  const std::string TkMapFileName_;
  const bool saveTkHistoMap_;
  //Global Info
  int NTkBadComponent[4];  //k: 0=BadModule, 1=BadFiber, 2=BadApv, 3=BadStrips
  int NTkBadComponentCooled[4]; //k: 0=BadModule, 1=BadFiber, 2=BadApv, 3=BadStrips
  int NTkBadComponentCooledRef[4]; //k: 0=BadModule, 1=BadFiber, 2=BadApv, 3=BadStrips
  int NBadComponent[4][19][4];
  //legend: NBadComponent[i][j][k]= SubSystem i, layer/disk/wheel j, BadModule/Fiber/Apv k
  //     i: 0=TIB, 1=TID, 2=TOB, 3=TEC
  //     k: 0=BadModule, 1=BadFiber, 2=BadApv, 3=BadStrips
  std::stringstream ssV[4][19];

  TrackerMap *tkMap, *tkMapFullIOVs;
  SiStripDetInfo detInfo_;
  std::unique_ptr<TkHistoMap> tkhisto;
  const edm::ESGetToken<TrackerTopology, TrackerTopologyRcd> tTopoToken_;
  const edm::ESGetToken<TkDetMap, TrackerTopologyRcd> tkDetMapToken_;
  std::unique_ptr<TrackerTopology> tTopo_;
  SiStripQualityWithFromFedErrorsHelper withFedErrHelper_;
  ///////
  std::vector<uint32_t> detId;
  std::vector<double> fraction; 
  std::vector<double> NStrips{0.,0.,0.,0.};
  //  std::vector<std::array> detIdFraction;
  std::vector<std::vector<double>> detIdFraction;
  std::vector<std::vector<double>> modules;
  std::vector<std::vector<double>> modulesRef;
  std::vector<uint32_t> cooled{
    470307621, 470307622, 470307653, 470307654, 470307684, 470307688, 470307716, 470307720, 470307724, 470307749, 470307750, 470307753, 470307754, 470307757, 470307758, 470307780, 470307784, 470307788, 470307812, 470307816, 470307820, 470307824, 470307828, 470311717, 470311718, 470311721, 470311722, 470311749, 470311750, 470311753, 470311754, 470311780, 470311784, 470311788, 470311812, 470311816, 470311820, 470311824, 470311845, 470311846, 470311849, 470311850, 470311876, 470311880, 470311884, 470311888, 470311908, 470311912, 470311916, 470311920, 470311924,470340389, 470340390, 470340421, 470340422, 470340452, 470340456, 470340484, 470340488, 470340492, 470340517, 470340518, 470340521, 470340522, 470340525, 470340526, 470340548, 470340552, 470340556, 470340580, 470340584, 470340588, 470340592, 470340596, 470344485, 470344486, 470344489, 470344490, 470344517, 470344518, 470344521, 470344522, 470344548, 470344552, 470344556, 470344580, 470344584, 470344588, 470344592, 470344613, 470344614, 470344617, 470344618, 470344644, 470344648, 470344652, 470344656, 470344676, 470344680, 470344684, 470344688, 470344692,470373189, 470373190, 470373220, 470373224, 470373252, 470373256, 470373260, 470373285, 470373286, 470373289, 470373290, 470373293, 470373294, 470373316, 470373320, 470373324, 470373348, 470373352, 470373360, 470373364, 470377285, 470377286, 470377289, 470377290, 470377316, 470377320, 470377324, 470377348, 470377352, 470377356, 470377360, 470377381, 470377382, 470377385, 470377386, 470377412, 470377416, 470377420, 470377424, 470377444, 470377448, 470377452, 470377456, 470377460, 470405988, 470405992, 470406020, 470406024, 470406028, 470406053, 470406054, 470406057, 470406058, 470406061, 470406062, 470406084, 470406088, 470406092, 470406116, 470406120, 470406124, 470406128, 470406132, 470410084, 470410088, 470410092, 470410116, 470410120, 470410124, 470410128, 470410149, 470410150, 470410153, 470410154, 470410180, 470410184, 470410188, 470410192, 470410212, 470410216, 470410220, 470410224, 470410228, 369157732, 369157736, 369157740, 369157748, 369157752, 369157756, 369157764, 369157768, 369157772, 369157780, 369157784, 369157788, 369157796, 369157800, 369157804, 369157812, 369157816, 369157820, 369157828, 369157832, 369157836, 369158772, 369158776, 369158780, 369158788, 369158792, 369158796, 369158804, 369158808, 369158812, 369158820, 369158824, 369158828, 369158836, 369158840, 369158844, 369158852, 369158856, 369158860, 369158868, 369158872, 369158876, 369158888, 369158892, 369173732, 369173736, 369173740, 369173748, 369173752, 369173756, 369173764, 369173768, 369173772, 369173780, 369173784, 369173788, 369173796, 369173800, 369173804, 369173812, 369173816, 369173820, 369173828, 369173832, 369173836, 369174772, 369174776, 369174780, 369174788, 369174792, 369174796, 369174804, 369174808, 369174812, 369174820, 369174824, 369174828, 369174836, 369174840, 369174844, 369174852, 369174856, 369174860
      };

  std::vector<uint32_t> cooledRef{
    470045477, 470045478, 470045509, 470045510, 470045540, 470045544, 470045572, 470045576, 470045580, 470045605, 470045606, 470045609, 470045610, 470045613, 470045614, 470045636, 470045640, 470045644, 470045668, 470045672, 470045676, 470045680, 470045684, 470049829, 470049830, 470049833, 470049834, 470049861, 470049862, 470049865, 470049866, 470049892, 470049896, 470049900, 470049924, 470049928, 470049932, 470049936, 470049957, 470049958, 470049961, 470049962, 470049988, 470049992, 470049996, 470050000, 470050020, 470050024, 470050028, 470050032, 470050036, 470078245, 470078246, 470078277, 470078278, 470078308, 470078312, 470078340, 470078344, 470078348, 470078373, 470078374, 470078377, 470078378, 470078381, 470078382, 470078404, 470078408, 470078412, 470078436, 470078440, 470078444, 470078448, 470078452, 470082597, 470082598, 470082601, 470082602, 470082629, 470082630, 470082633, 470082634, 470082660, 470082664, 470082668, 470082692, 470082696, 470082700, 470082704, 470082725, 470082726, 470082729, 470082730, 470082756, 470082760, 470082764, 470082768, 470082788, 470082792, 470082796, 470082800, 470082804, 470111045, 470111046, 470111076, 470111080, 470111108, 470111112, 470111116, 470111141, 470111142, 470111145, 470111146, 470111149, 470111150, 470111172, 470111176, 470111180, 470111204, 470111208, 470111212, 470111216, 470111220, 470115397, 470115398, 470115401, 470115402, 470115428, 470115432, 470115436, 470115460, 470115464, 470115468, 470115472, 470115493, 470115494, 470115497, 470115498, 470115524, 470115528, 470115532, 470115536, 470115556, 470115560, 470115564, 470115568, 470115572, 470143844, 470143848, 470143876, 470143880, 470143884, 470143909, 470143910, 470143913, 470143914, 470143917, 470143918, 470143940, 470143944, 470143948, 470143972, 470143976, 470143980, 470143984, 470143988, 470148196, 470148200, 470148204, 470148228, 470148232, 470148236, 470148240, 470148261, 470148262, 470148265, 470148266, 470148292, 470148296, 470148300, 470148304, 470148324, 470148328, 470148332, 470148336, 470148340, 369153636, 369153640, 369153644, 369153652, 369153656, 369153660, 369153668, 369153672, 369153676, 369153684, 369153688, 369153692, 369153700, 369153704, 369153708, 369153716, 369153720, 369153724, 369153732, 369153736, 369153740, 369154676, 369154680, 369154684, 369154692, 369154696, 369154700, 369154708, 369154712, 369154716, 369154724, 369154728, 369154732, 369154740, 369154744, 369154748, 369154756, 369154760, 369154764, 369154772, 369154776, 369154780, 369154788, 369154792, 369154796, 369169636, 369169640, 369169644, 369169652, 369169656, 369169660, 369169668, 369169672, 369169676, 369169684, 369169688, 369169692, 369169700, 369169704, 369169708, 369169716, 369169720, 369169724, 369169732, 369169736, 369169740, 369170676, 369170680, 369170684, 369170692, 369170696, 369170700, 369170708, 369170712, 369170716, 369170724, 369170728, 369170732, 369170740, 369170744, 369170748, 369170756, 369170760, 369170764
};

};

SiStripQualityStatistics::SiStripQualityStatistics(const edm::ParameterSet& iConfig)
    : TkMapFileName_(iConfig.getUntrackedParameter<std::string>("TkMapFileName", "")),
      saveTkHistoMap_(iConfig.getUntrackedParameter<bool>("SaveTkHistoMap", true)),
      tkMap(nullptr),
      tkMapFullIOVs(nullptr),
      tTopoToken_(esConsumes<edm::Transition::EndRun>()),
      tkDetMapToken_(esConsumes<edm::Transition::EndRun>()),
      withFedErrHelper_{iConfig, consumesCollector(), true} {
  detInfo_ = SiStripDetInfoFileReader::read(
      iConfig.getUntrackedParameter<edm::FileInPath>("file", edm::FileInPath(SiStripDetInfoFileReader::kDefaultFile))
          .fullPath());

  tkMapFullIOVs = new TrackerMap("BadComponents");
  tkhisto = nullptr;
}

void SiStripQualityStatistics::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.addUntracked<std::string>("TkMapFileName", "");
  desc.addUntracked<bool>("SaveTkHistoMap", true);
  desc.addUntracked<edm::FileInPath>("file", edm::FileInPath(SiStripDetInfoFileReader::kDefaultFile));
  SiStripQualityWithFromFedErrorsHelper::fillDescription(desc);
  descriptions.add("siStripQualityStatistics", desc);
}

void SiStripQualityStatistics::dqmEndJob(DQMStore::IBooker& booker, DQMStore::IGetter& getter) {
  if (withFedErrHelper_.addBadCompFromFedErr()) {
    updateAndSave(&withFedErrHelper_.getMergedQuality(getter));
  }
  std::string filename = TkMapFileName_;
  if (!filename.empty()) {
    tkMapFullIOVs->save(false, 0, 0, filename);
    filename.erase(filename.begin() + filename.find('.'), filename.end());
    tkMapFullIOVs->print(false, 0, 0, filename);

    if (saveTkHistoMap_) {
      tkhisto->save(filename + ".root");
      tkhisto->saveAsCanvas(filename + "_Canvas.root", "E");
    }
  }
}

void SiStripQualityStatistics::endRun(edm::Run const& run, edm::EventSetup const& iSetup) {
  tTopo_ = std::make_unique<TrackerTopology>(iSetup.getData(tTopoToken_));
  if ((!tkhisto) && (!TkMapFileName_.empty())) {
    //here the baseline (the value of the empty,not assigned bins) is put to -1 (default is zero)
    tkhisto = std::make_unique<TkHistoMap>(&iSetup.getData(tkDetMapToken_), "BadComp", "BadComp", -1.);
  }

  if (withFedErrHelper_.endRun(iSetup) && !withFedErrHelper_.addBadCompFromFedErr()) {
    run_ = run.id();
    updateAndSave(&iSetup.getData(withFedErrHelper_.qualityToken()));
  }
}

void SiStripQualityStatistics::updateAndSave(const SiStripQuality* siStripQuality) {
  for (int i = 0; i < 4; ++i) {
    NTkBadComponent[i] = 0;
    for (int j = 0; j < 19; ++j) {
      ssV[i][j].str("");
      for (int k = 0; k < 4; ++k)
        NBadComponent[i][j][k] = 0;
    }
  }

  if (tkMap)
    delete tkMap;
  tkMap = new TrackerMap("BadComponents");

  std::stringstream ss;
  std::vector<uint32_t> detids = detInfo_.getAllDetIds();
  std::vector<uint32_t>::const_iterator idet = detids.begin();
  for (; idet != detids.end(); ++idet) {
    ss << "detid " << (*idet) << " IsModuleUsable " << siStripQuality->IsModuleUsable((*idet)) << "\n";
    if (siStripQuality->IsModuleUsable((*idet))){
      tkMap->fillc(*idet, 0x00ff00);
      DetId detectorId = DetId((*idet));
      int subDet = detectorId.subdetId();
      if (subDet == StripSubdetector::TIB) {
	NStrips[0] +=128. * detInfo_.getNumberOfApvsAndStripLength((*idet)).first;
      }
      else if (subDet == StripSubdetector::TID) {
	NStrips[1] +=128. * detInfo_.getNumberOfApvsAndStripLength((*idet)).first;
      }
      else if (subDet == StripSubdetector::TOB) {
        NStrips[2] +=128. * detInfo_.getNumberOfApvsAndStripLength((*idet)).first ;     
      } 
      else if (subDet == StripSubdetector::TEC) {
	NStrips[3] +=128. * detInfo_.getNumberOfApvsAndStripLength((*idet)).first;
      }
      
      /*
      bool isIn = false;
      for(size_t ix = 0; ix<cooled.size(); ++ix){
        if((*idet)==cooled[ix])
          isIn = true;
      }
      if(isIn){
	std::cout<<"module is cooled is: "<<(*idet)<<" x "<<" number of strips: "<<(128. * detInfo_.getNumberOfApvsAndStripLength((*idet)).first)<<std::endl;
      }
      */
    }
  }
  LogDebug("SiStripQualityStatistics") << ss.str() << std::endl;

  std::vector<SiStripQuality::BadComponent> BC = siStripQuality->getBadComponentList();

  int CountIn=0;
  for (size_t i = 0; i < BC.size(); ++i) {
    //&&&&&&&&&&&&&
    //Full Tk
    //&&&&&&&&&&&&&
    // std::cout<<"BC[i].BadModule: "<<BC[i].BadModule<<" BC[i].BadFibers: "<<BC[i].BadFibers<<" BC[i].BadApvs: "<<BC[i].BadApvs<<std::endl;
    
    if (BC[i].BadModule){
      NTkBadComponent[0]++;
      //      std::cout<<"NTkBadComponent[0]++"<<std::endl;
    }
    if (BC[i].BadFibers){
      NTkBadComponent[1] += ((BC[i].BadFibers >> 2) & 0x1) + ((BC[i].BadFibers >> 1) & 0x1) + ((BC[i].BadFibers) & 0x1);
      // std::cout<<"((BC[i].BadFibers >> 2) & 0x1) + ((BC[i].BadFibers >> 1) & 0x1) + ((BC[i].BadFibers) & 0x1): ==> "<<((BC[i].BadFibers >> 2) & 0x1) + ((BC[i].BadFibers >> 1) & 0x1) + ((BC[i].BadFibers) & 0x1)<<std::endl;
    }
    if (BC[i].BadApvs){
      NTkBadComponent[2] += ((BC[i].BadApvs >> 5) & 0x1) + ((BC[i].BadApvs >> 4) & 0x1) + ((BC[i].BadApvs >> 3) & 0x1) +
                            ((BC[i].BadApvs >> 2) & 0x1) + ((BC[i].BadApvs >> 1) & 0x1) + ((BC[i].BadApvs) & 0x1);
      //std::cout<<"((BC[i].BadApvs >> 5) & 0x1) + ((BC[i].BadApvs >> 4) & 0x1) + ((BC[i].BadApvs >> 3) & 0x1) + ((BC[i].BadApvs >> 2) & 0x1) + ((BC[i].BadApvs >> 1) & 0x1) + ((BC[i].BadApvs) & 0x1) ==> "<< ((BC[i].BadApvs >> 5) & 0x1) + ((BC[i].BadApvs >> 4) & 0x1) + ((BC[i].BadApvs >> 3) & 0x1) + ((BC[i].BadApvs >> 2) & 0x1) + ((BC[i].BadApvs >> 1) & 0x1) + ((BC[i].BadApvs)& 0x1)<<std::endl;
    }
    //&&&&&&&&&&&&&&&&&
    //Single SubSystem
    //&&&&&&&&&&&&&&&&&
    int component;
    DetId detectorId = DetId(BC[i].detid);
    int subDet = detectorId.subdetId();
    
    for(size_t ix = 0; ix<cooled.size(); ++ix){
      if(detectorId==cooled[ix]){
	//      std::cout<<"module is cooled is: "<<(*idet)<<" x "<<std::endl;
	CountIn++;
	if (BC[i].BadModule)
	  NTkBadComponentCooled[0]++;
	if (BC[i].BadFibers)
	  NTkBadComponentCooled[1] += ((BC[i].BadFibers >> 2) & 0x1) + ((BC[i].BadFibers >> 1) & 0x1) + ((BC[i].BadFibers) & 0x1);
	if (BC[i].BadApvs)
	  NTkBadComponentCooled[2] += ((BC[i].BadApvs >> 5) & 0x1) + ((BC[i].BadApvs >> 4) & 0x1) + ((BC[i].BadApvs >> 3) & 0x1) + ((BC[i].BadApvs >> 2) & 0x1) + ((BC[i].BadApvs >> 1) & 0x1) + ((BC[i].BadApvs) & 0x1);
      }
    }

  for(size_t ix = 0; ix<cooledRef.size(); ++ix){
    if(detectorId==cooledRef[ix]){
      //      std::cout<<"module is cooled is: "<<(*idet)<<" x "<<std::endl;
      CountIn++;
      if (BC[i].BadModule)
	NTkBadComponentCooledRef[0]++;
      if (BC[i].BadFibers)
	NTkBadComponentCooledRef[1] += ((BC[i].BadFibers >> 2) & 0x1) + ((BC[i].BadFibers >> 1) & 0x1) + ((BC[i].BadFibers) & 0x1);
      if (BC[i].BadApvs)
	NTkBadComponentCooledRef[2] += ((BC[i].BadApvs >> 5) & 0x1) + ((BC[i].BadApvs >> 4) & 0x1) + ((BC[i].BadApvs >> 3) & 0x1) + ((BC[i].BadApvs >> 2) & 0x1) + ((BC[i].BadApvs >> 1) & 0x1) + ((BC[i].BadApvs) & 0x1);
    }
  }

    
    if (subDet == StripSubdetector::TIB) {
      //&&&&&&&&&&&&&&&&&
      //TIB
      //&&&&&&&&&&&&&&&&&

      component = tTopo_->tibLayer(BC[i].detid);
      SetBadComponents(0, component, BC[i]);

    } else if (subDet == StripSubdetector::TID) {
      //&&&&&&&&&&&&&&&&&
      //TID
      //&&&&&&&&&&&&&&&&&

      component = tTopo_->tidSide(BC[i].detid) == 2 ? tTopo_->tidWheel(BC[i].detid) : tTopo_->tidWheel(BC[i].detid) + 3;
      SetBadComponents(1, component, BC[i]);

    } else if (subDet == StripSubdetector::TOB) {
      //&&&&&&&&&&&&&&&&&
      //TOB
      //&&&&&&&&&&&&&&&&&

      component = tTopo_->tobLayer(BC[i].detid);
      SetBadComponents(2, component, BC[i]);

    } else if (subDet == StripSubdetector::TEC) {
      //&&&&&&&&&&&&&&&&&
      //TEC
      //&&&&&&&&&&&&&&&&&

      component = tTopo_->tecSide(BC[i].detid) == 2 ? tTopo_->tecWheel(BC[i].detid) : tTopo_->tecWheel(BC[i].detid) + 9;
      SetBadComponents(3, component, BC[i]);
    }
  }
  std::cout<<"CountIn: "<<CountIn<<std::endl;
  std::cout<<"cooled: "<<cooled.size()<<std::endl;
  //&&&&&&&&&&&&&&&&&&
  // Single Strip Info
  //&&&&&&&&&&&&&&&&&&
  float percentage = 0;
  float absnumber = 0;
  SiStripQuality::RegistryIterator rbegin = siStripQuality->getRegistryVectorBegin();
  SiStripQuality::RegistryIterator rend = siStripQuality->getRegistryVectorEnd();

  for (SiStripBadStrip::RegistryIterator rp = rbegin; rp != rend; ++rp) {
    uint32_t detid = rp->detid;

    int subdet = -999;
    int component = -999;
    DetId detectorId = DetId(detid);
    int subDet = detectorId.subdetId();
    if (subDet == StripSubdetector::TIB) {
      subdet = 0;
      component = tTopo_->tibLayer(detid);
    } else if (subDet == StripSubdetector::TID) {
      subdet = 1;
      component = tTopo_->tidSide(detid) == 2 ? tTopo_->tidWheel(detid) : tTopo_->tidWheel(detid) + 3;
    } else if (subDet == StripSubdetector::TOB) {
      subdet = 2;
      component = tTopo_->tobLayer(detid);
    } else if (subDet == StripSubdetector::TEC) {
      subdet = 3;
      component = tTopo_->tecSide(detid) == 2 ? tTopo_->tecWheel(detid) : tTopo_->tecWheel(detid) + 9;
    }

    SiStripQuality::Range sqrange = SiStripQuality::Range(siStripQuality->getDataVectorBegin() + rp->ibegin,
                                                          siStripQuality->getDataVectorBegin() + rp->iend);

    percentage = 0;
    absnumber = 0;
    for (int it = 0; it < sqrange.second - sqrange.first; it++) {
      unsigned int range = siStripQuality->decode(*(sqrange.first + it)).range;
      NTkBadComponent[3] += range;
      //      std::cout<<"range: "<<range<<" detectorId: "<<detid<<std::endl;
      NBadComponent[subdet][0][3] += range;
      NBadComponent[subdet][component][3] += range;
      percentage += range;
      absnumber +=range;

      for(size_t ix = 0; ix<cooled.size(); ++ix){
	if(detid==cooled[ix])
	  NTkBadComponentCooled[3] += range;
      }      

      for(size_t ix = 0; ix<cooledRef.size(); ++ix){
	if(detid==cooledRef[ix])
          NTkBadComponentCooledRef[3] += range;
      }


    }
    if (percentage != 0)
      percentage /= 128. * detInfo_.getNumberOfApvsAndStripLength(detid).first;
    if (percentage > 1)
      edm::LogError("SiStripQualityStatistics") << "PROBLEM detid " << detid << " value " << percentage << std::endl;


    bool isIn = false;
    for(size_t ix = 0; ix<cooled.size(); ++ix){
      if((detid)==cooled[ix])
	isIn = true;
    }
    if(isIn){
      //      std::cout<<"module is cooled is: "<<(detid)<<" x "<<" number of strips: "<<(128. * detInfo_.getNumberOfApvsAndStripLength((detid)).first)<<" bad one: "<<absnumber<<" ==> "<<percentage<<std::endl;
      std::vector<double> helper{absnumber, (128. * detInfo_.getNumberOfApvsAndStripLength((detid)).first)};
      modules.push_back(helper);
    }
    
    bool isInRef =false;
    for(size_t ix = 0; ix<cooledRef.size(); ++ix){
      if((detid)==cooledRef[ix])
        isInRef = true;
    }
    if(isInRef){
      //      std::cout<<"module is cooled is: "<<(detid)<<" x "<<" number of strips: "<<(128. * detInfo_.getNumberOfApvsAndStripLength((detid)).first)<<" bad one: "<<absnumber<<" ==> "<<percentage<<std::endl;
      std::vector<double> helper{absnumber, (128. * detInfo_.getNumberOfApvsAndStripLength((detid)).first)};
      modulesRef.push_back(helper);
    }
    


    //------- Global Statistics on percentage of bad components along the IOVs ------//
    tkMapFullIOVs->fill(detid, percentage);
    if (tkhisto != nullptr)
      tkhisto->fill(detid, percentage);
    //  std::array detidfrac[2] = {detid, percentage};
    //detIdFraction.push_back(detidfrac);
    std::vector<double> detidfraction_helper{(double)subdet,(double)detid, percentage,absnumber, 128. * detInfo_.getNumberOfApvsAndStripLength(detid).first};
    detIdFraction.push_back(detidfraction_helper);
    //    detId.push_back(detid);
    //fraction.push_back(percentage);
  }

  //&&&&&&&&&&&&&&&&&&
  // printout
  //&&&&&&&&&&&&&&&&&&

  ss.str("");
  ss << "\n-----------------\nNew IOV starting from run " << run_.run() << "\n-----------------\n";
  ss << "\n-----------------\nGlobal Info\n-----------------";
  ss << "\nBadComponent \t   Modules \tFibers "
        "\tApvs\tStrips\n----------------------------------------------------------------";
  ss << "\nTracker:\t\t" << NTkBadComponent[0] << "\t" << NTkBadComponent[1] << "\t" << NTkBadComponent[2] << "\t"
     << NTkBadComponent[3];
  ss << "\n";
  ss << "\nTIB:\t\t\t" << NBadComponent[0][0][0] << "\t" << NBadComponent[0][0][1] << "\t" << NBadComponent[0][0][2]
     << "\t" << NBadComponent[0][0][3];
  ss << "\nTID:\t\t\t" << NBadComponent[1][0][0] << "\t" << NBadComponent[1][0][1] << "\t" << NBadComponent[1][0][2]
     << "\t" << NBadComponent[1][0][3];
  ss << "\nTOB:\t\t\t" << NBadComponent[2][0][0] << "\t" << NBadComponent[2][0][1] << "\t" << NBadComponent[2][0][2]
     << "\t" << NBadComponent[2][0][3];
  ss << "\nTEC:\t\t\t" << NBadComponent[3][0][0] << "\t" << NBadComponent[3][0][1] << "\t" << NBadComponent[3][0][2]
     << "\t" << NBadComponent[3][0][3];
  ss << "\n";

  for (int i = 1; i < 5; ++i)
    ss << "\nTIB Layer " << i << " :\t\t" << NBadComponent[0][i][0] << "\t" << NBadComponent[0][i][1] << "\t"
       << NBadComponent[0][i][2] << "\t" << NBadComponent[0][i][3];
  ss << "\n";
  for (int i = 1; i < 4; ++i)
    ss << "\nTID+ Disk " << i << " :\t\t" << NBadComponent[1][i][0] << "\t" << NBadComponent[1][i][1] << "\t"
       << NBadComponent[1][i][2] << "\t" << NBadComponent[1][i][3];
  for (int i = 4; i < 7; ++i)
    ss << "\nTID- Disk " << i - 3 << " :\t\t" << NBadComponent[1][i][0] << "\t" << NBadComponent[1][i][1] << "\t"
       << NBadComponent[1][i][2] << "\t" << NBadComponent[1][i][3];
  ss << "\n";
  for (int i = 1; i < 7; ++i)
    ss << "\nTOB Layer " << i << " :\t\t" << NBadComponent[2][i][0] << "\t" << NBadComponent[2][i][1] << "\t"
       << NBadComponent[2][i][2] << "\t" << NBadComponent[2][i][3];
  ss << "\n";
  for (int i = 1; i < 10; ++i)
    ss << "\nTEC+ Disk " << i << " :\t\t" << NBadComponent[3][i][0] << "\t" << NBadComponent[3][i][1] << "\t"
       << NBadComponent[3][i][2] << "\t" << NBadComponent[3][i][3];
  for (int i = 10; i < 19; ++i)
    ss << "\nTEC- Disk " << i - 9 << " :\t\t" << NBadComponent[3][i][0] << "\t" << NBadComponent[3][i][1] << "\t"
       << NBadComponent[3][i][2] << "\t" << NBadComponent[3][i][3];
  ss << "\n";

  ss << "\n----------------------------------------------------------------\n\t\t   Detid  \tModules Fibers "
        "Apvs\n----------------------------------------------------------------";
  for (int i = 1; i < 5; ++i)
    ss << "\nTIB Layer " << i << " :" << ssV[0][i].str();
  ss << "\n";
  for (int i = 1; i < 4; ++i)
    ss << "\nTID+ Disk " << i << " :" << ssV[1][i].str();
  for (int i = 4; i < 7; ++i)
    ss << "\nTID- Disk " << i - 3 << " :" << ssV[1][i].str();
  ss << "\n";
  for (int i = 1; i < 7; ++i)
    ss << "\nTOB Layer " << i << " :" << ssV[2][i].str();
  ss << "\n";
  for (int i = 1; i < 10; ++i)
    ss << "\nTEC+ Disk " << i << " :" << ssV[3][i].str();
  for (int i = 10; i < 19; ++i)
    ss << "\nTEC- Disk " << i - 9 << " :" << ssV[3][i].str();


  ss << "\n";
  ss << "\n----------------------------------------------------------------\n\tDetid  \t\t Fraction of bad components";
  std::vector<const char*> subdet{"TIB","TID","TOB","TEC"};
  for(size_t i = 0; i < subdet.size(); ++i){
    ss << "\n-----------------";
    ss << "\n\t\t"<<subdet[i];
    for(size_t ix = 0; ix < detIdFraction.size(); ++ix)
      if((size_t)detIdFraction[ix][0]==i)
	ss <<"\n\t"<<(uint32_t)detIdFraction[ix][1]<<"  \t"<<detIdFraction[ix][2];
  }
  ss << "\n";
  ss << "\n-------------------------------";
  ss << "\n\t Summary:";
  for(size_t i = 0; i < subdet.size(); ++i){
    int count = 0;
    int NStrip = 0;
    int NBadStrip =0;
    for(size_t ix = 0; ix < detIdFraction.size(); ++ix){
      if((size_t)detIdFraction[ix][0]==i){
	count++;
	NStrip +=detIdFraction[ix][4];
	NBadStrip +=detIdFraction[ix][3];
      }
    }
    ss << "\n\t"<<subdet[i]<<":\t"<<count<<" detIds with bad strips";
    ss << "\n\t"<<" Total number of bad strips: "<<NBadStrip;
    ss << "\n\t"<<" Total number of strips for these detIds: "<<NStrip;
    ss << "\n\t"<<" Total number of active strips: "<< NStrips[i];
  }
  ss << "\n";
  ss << "\n-------------------------------";
  ss << "\n";
  int NStrip = 0;
  int NBadStrip =0;
  for(size_t ix = 0; ix < modules.size(); ++ix){
    NBadStrip +=modules[ix][0];
    NStrip +=modules[ix][1];
  }
  ss <<"\n Total number of bad strips: "<<NBadStrip;
  ss <<"\n Total number of strips for these detIds: "<<NStrip;
  ss <<"\n Fraction: "<<(double)NBadStrip/(double)NStrip;


  ss << "\n-------------------------------";
  ss << "\n--- Reference: ----------------";
  int NStripRef = 0;
  int NBadStripRef =0;
  for(size_t ix = 0; ix < modulesRef.size(); ++ix){
    NBadStripRef +=modulesRef[ix][0];
    NStripRef +=modulesRef[ix][1];
  }
  ss <<"\n Total number of bad strips: "<<NBadStripRef;
  ss <<"\n Total number of strips for these detIds: "<<NStripRef;
  ss <<"\n Fraction: "<<(double)NBadStripRef/(double)NStripRef;


  edm::LogInfo("SiStripQualityStatistics") << ss.str() << std::endl;

  std::cout<<"NTkBadComponentCooled..."<<std::endl;
  std::cout<<NTkBadComponentCooled[0]<<" "<<NTkBadComponentCooled[1]<<" "<<NTkBadComponentCooled[2]<<" "<<NTkBadComponentCooled[3]<<std::endl;

  std::cout<<"NTkBadComponentCooledRef..."<<std::endl;
  std::cout<<NTkBadComponentCooledRef[0]<<" "<<NTkBadComponentCooledRef[1]<<" "<<NTkBadComponentCooledRef[2]<<" "<<NTkBadComponentCooledRef[3]<<std::endl;

  std::cout<<"Total number of bad strips/Total number of strips for these detIds:"<<std::endl;
  std::cout<<NBadStrip<<" "<<NStrip<<std::endl;
  std::cout<<"Reference total number of bad strips/Total number of strips for these detIds:"<<std::endl;
  std::cout<<NBadStripRef<<" "<<NStripRef<<std::endl;

  std::string filename = TkMapFileName_;
  std::stringstream sRun;
  sRun.str("");
  sRun << "_Run_" << std::setw(6) << std::setfill('0') << run_.run() << std::setw(0);

  if (!filename.empty()) {
    filename.insert(filename.find('.'), sRun.str());
    tkMap->save(true, 0, 0, filename);
    filename.erase(filename.begin() + filename.find('.'), filename.end());
    tkMap->print(true, 0, 0, filename);
  }
}

void SiStripQualityStatistics::SetBadComponents(int i, int component, SiStripQuality::BadComponent& BC) {
  int napv = detInfo_.getNumberOfApvsAndStripLength(BC.detid).first;

  ssV[i][component] << "\n\t\t " << BC.detid << " \t " << BC.BadModule << " \t " << ((BC.BadFibers) & 0x1) << " ";
  if (napv == 4)
    ssV[i][component] << "x " << ((BC.BadFibers >> 1) & 0x1);

  if (napv == 6)
    ssV[i][component] << ((BC.BadFibers >> 1) & 0x1) << " " << ((BC.BadFibers >> 2) & 0x1);
  ssV[i][component] << " \t " << ((BC.BadApvs) & 0x1) << " " << ((BC.BadApvs >> 1) & 0x1) << " ";
  if (napv == 4)
    ssV[i][component] << "x x " << ((BC.BadApvs >> 2) & 0x1) << " " << ((BC.BadApvs >> 3) & 0x1);
  if (napv == 6)
    ssV[i][component] << ((BC.BadApvs >> 2) & 0x1) << " " << ((BC.BadApvs >> 3) & 0x1) << " "
                      << ((BC.BadApvs >> 4) & 0x1) << " " << ((BC.BadApvs >> 5) & 0x1) << " ";

  if (BC.BadApvs) {
    NBadComponent[i][0][2] += ((BC.BadApvs >> 5) & 0x1) + ((BC.BadApvs >> 4) & 0x1) + ((BC.BadApvs >> 3) & 0x1) +
                              ((BC.BadApvs >> 2) & 0x1) + ((BC.BadApvs >> 1) & 0x1) + ((BC.BadApvs) & 0x1);
    NBadComponent[i][component][2] += ((BC.BadApvs >> 5) & 0x1) + ((BC.BadApvs >> 4) & 0x1) +
                                      ((BC.BadApvs >> 3) & 0x1) + ((BC.BadApvs >> 2) & 0x1) +
                                      ((BC.BadApvs >> 1) & 0x1) + ((BC.BadApvs) & 0x1);
    tkMap->fillc(BC.detid, 0xff0000);
  }
  if (BC.BadFibers) {
    NBadComponent[i][0][1] += ((BC.BadFibers >> 2) & 0x1) + ((BC.BadFibers >> 1) & 0x1) + ((BC.BadFibers) & 0x1);
    NBadComponent[i][component][1] +=
        ((BC.BadFibers >> 2) & 0x1) + ((BC.BadFibers >> 1) & 0x1) + ((BC.BadFibers) & 0x1);
    tkMap->fillc(BC.detid, 0x0000ff);
  }
  if (BC.BadModule) {
    NBadComponent[i][0][0]++;
    NBadComponent[i][component][0]++;
    tkMap->fillc(BC.detid, 0x0);
  }
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(SiStripQualityStatistics);
