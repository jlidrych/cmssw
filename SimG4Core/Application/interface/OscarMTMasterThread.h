#ifndef SimG4Core_OscarMTMasterThread_H
#define SimG4Core_OscarMTMasterThread_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESWatcher.h"

#include "DetectorDescription/Core/interface/DDCompactView.h"
#include "DetectorDescription/DDCMS/interface/DDCompactView.h"

#include "HepPDT/ParticleDataTable.hh"
#include "SimGeneral/HepPDTRecord/interface/PDTRecord.h"

#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace edm {
  class EventSetup;
}

class RunManagerMT;

class DDCompactView;
class MagneticField;

namespace cms {
  class DDCompactView;
}

namespace HepPDT {
  class ParticleDataTable;
}

class OscarMTMasterThread {
public:
  explicit OscarMTMasterThread(const edm::ParameterSet& iConfig);
  ~OscarMTMasterThread();

  void beginRun(const DDCompactView*, const cms::DDCompactView*, const HepPDT::ParticleDataTable*) const;
  void endRun() const;
  void stopThread();

  inline RunManagerMT& runManagerMaster() const { return *m_runManagerMaster; }
  inline RunManagerMT* runManagerMasterPtr() const { return m_runManagerMaster.get(); }
  inline bool isDD4Hep() const { return m_pGeoFromDD4hep; }

private:

  enum class ThreadState { NotExist = 0, BeginRun = 1, EndRun = 2, Destruct = 3 };

  const bool m_pGeoFromDD4hep;

  std::shared_ptr<RunManagerMT> m_runManagerMaster;
  std::thread m_masterThread;

  // ES products needed for Geant4 initialization
  mutable const DDCompactView* m_pDDD = nullptr;
  mutable const cms::DDCompactView* m_pDD4Hep = nullptr;
  mutable const HepPDT::ParticleDataTable* m_pTable = nullptr;

  // status flags
  mutable std::mutex m_protectMutex;
  mutable std::mutex m_threadMutex;
  mutable std::condition_variable m_notifyMasterCv;
  mutable std::condition_variable m_notifyMainCv;

  mutable ThreadState m_masterThreadState;

  mutable bool m_masterCanProceed = false;
  mutable bool m_mainCanProceed = false;
  mutable bool m_firstRun = true;
  mutable bool m_stopped = false;
};

#endif
