// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.
//
//                Task to select electrons from dalitz decay
//        It can produce track and pair histograms for selected tracks
//      It creates a bitmap with selections to be stored during skimming
//
#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "Framework/AnalysisDataModel.h"
#include "Framework/ASoAHelpers.h"
#include "Framework/DataTypes.h"
#include "Common/DataModel/Multiplicity.h"
#include "Common/DataModel/EventSelection.h"
#include "Common/DataModel/Centrality.h"
#include "Common/CCDB/TriggerAliases.h"
#include "Common/DataModel/PIDResponse.h"
#include "Common/DataModel/TrackSelectionTables.h"
#include "PWGDQ/DataModel/ReducedInfoTables.h"
#include "PWGDQ/Core/VarManager.h"
#include "PWGDQ/Core/HistogramManager.h"
#include "PWGDQ/Core/AnalysisCut.h"
#include "PWGDQ/Core/AnalysisCompositeCut.h"
#include "PWGDQ/Core/HistogramsLibrary.h"
#include "PWGDQ/Core/CutsLibrary.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;
using namespace o2::aod;
using namespace o2::soa;
using std::array;

using MyEvents = soa::Join<aod::Collisions, aod::EvSels>;

using MyBarrelTracks = soa::Join<aod::Tracks, aod::TracksExtra, aod::TrackSelection, aod::TracksDCA,
                                 aod::pidTPCFullEl, aod::pidTPCFullPi, aod::pidTPCFullMu,
                                 aod::pidTPCFullKa, aod::pidTPCFullPr,
                                 aod::pidTOFFullEl, aod::pidTOFFullPi, aod::pidTOFFullMu,
                                 aod::pidTOFFullKa, aod::pidTOFFullPr, aod::pidTOFbeta>;

constexpr static uint32_t gkEventFillMap = VarManager::ObjTypes::Collision;
constexpr static uint32_t gkTrackFillMap = VarManager::ObjTypes::Track | VarManager::ObjTypes::TrackExtra | VarManager::ObjTypes::TrackDCA | VarManager::ObjTypes::TrackSelection | VarManager::ObjTypes::TrackPID;

struct dalitzPairing {
  Produces<o2::aod::DalitzBits> dalitzbits;
  Preslice<MyBarrelTracks> perCollision = aod::track::collisionId;

  // Configurables
  Configurable<std::string> fConfigEventCuts{"cfgEventCuts", "eventStandardNoINT7", "Event selection"};
  Configurable<std::string> fConfigDalitzTrackCuts{"cfgDalitzTrackCuts", "", "Dalitz track selection cuts, separated by a comma"};
  Configurable<std::string> fConfigDalitzPairCuts{"cfgDalitzPairCuts", "", "Dalitz pair selection cuts"};
  Configurable<std::string> fConfigAddTrackHistogram{"cfgAddTrackHistogram", "", "Comma separated list of histograms"};
  Configurable<bool> fQA{"cfgQA", true, "QA histograms"};
  Configurable<float> fConfigBarrelTrackPINLow{"cfgBarrelLowPIN", 0.1f, "Low pt cut for Dalitz tracks in the barrel"};
  Configurable<float> fConfigEtaCut{"cfgEtaCut", 0.9f, "Eta cut for Dalitz tracks in the barrel"};
  Configurable<float> fConfigTPCNSigLow{"cfgTPCNSigElLow", -3.f, "Low TPCNSigEl cut for Dalitz tracks in the barrel"};
  Configurable<float> fConfigTPCNSigHigh{"cfgTPCNSigElHigh", 3.f, "High TPCNsigEl cut for Dalitz tracks in the barrel"};

  Filter filterBarrelTrack = o2::aod::track::tpcInnerParam >= fConfigBarrelTrackPINLow && nabs(o2::aod::track::eta) <= fConfigEtaCut && o2::aod::pidtpc::tpcNSigmaEl <= fConfigTPCNSigHigh && o2::aod::pidtpc::tpcNSigmaEl >= fConfigTPCNSigLow;

  OutputObj<THashList> fOutputList{"output"}; //! the histogram manager output list
  OutputObj<TList> fStatsList{"Statistics"};  //! skimming statistics

  std::map<int, uint8_t> fTrackmap;
  std::map<int, uint8_t> fDalitzmap;

  AnalysisCompositeCut* fEventCut;
  std::vector<AnalysisCompositeCut> fTrackCuts;
  std::vector<AnalysisCompositeCut> fPairCuts;

  HistogramManager* fHistMan;

  void init(o2::framework::InitContext&)
  {
    // Event cuts
    fEventCut = new AnalysisCompositeCut(true);
    TString eventCutStr = fConfigEventCuts.value;
    fEventCut->AddCut(dqcuts::GetAnalysisCut(eventCutStr.Data()));

    // Barrel track cuts
    TString cutNamesStr = fConfigDalitzTrackCuts.value;
    if (!cutNamesStr.IsNull()) {
      std::unique_ptr<TObjArray> objArray(cutNamesStr.Tokenize(","));
      for (int icut = 0; icut < objArray->GetEntries(); ++icut) {
        fTrackCuts.push_back(*dqcuts::GetCompositeCut(objArray->At(icut)->GetName()));
      }
    }

    // Pair cuts
    TString cutNamesPairStr = fConfigDalitzPairCuts.value;
    if (!cutNamesPairStr.IsNull()) {
      std::unique_ptr<TObjArray> objArray(cutNamesPairStr.Tokenize(","));
      for (int icut = 0; icut < objArray->GetEntries(); ++icut) {
        fPairCuts.push_back(*dqcuts::GetCompositeCut(objArray->At(icut)->GetName()));
      }
    }

    if (fTrackCuts.size() != fPairCuts.size()) {
      LOGF(fatal, "YOU SHOULD PROVIDE THE SAME NUMBER OF TRACK AND PAIR CUTS");
    }

    VarManager::SetUseVars(AnalysisCut::fgUsedVars); // provide the list of required variables so that VarManager knows what to fill
    VarManager::SetDefaultVarNames();
    fHistMan = new HistogramManager("analysisHistos", "aa", VarManager::kNVars);
    fHistMan->SetUseDefaultVariableNames(kTRUE);
    fHistMan->SetDefaultVarNames(VarManager::fgVariableNames, VarManager::fgVariableUnits);

    // Create the histogram class names to be added to the histogram manager
    TString histClasses = "";

    if (fQA) {
      auto trackCut = fTrackCuts.begin();
      for (auto pairCut = fPairCuts.begin(); pairCut != fPairCuts.end(); pairCut++, trackCut++) {
        histClasses += Form("TrackBarrel_%s_%s;", (*trackCut).GetName(), (*pairCut).GetName());
        histClasses += Form("Pair_%s_%s;", (*trackCut).GetName(), (*pairCut).GetName());
      }
    }

    // Define histograms

    std::unique_ptr<TObjArray> objArray(histClasses.Tokenize(";"));
    for (Int_t iclass = 0; iclass < objArray->GetEntries(); ++iclass) {
      TString classStr = objArray->At(iclass)->GetName();
      fHistMan->AddHistClass(classStr.Data());

      if (classStr.Contains("Event")) {
        dqhistograms::DefineHistograms(fHistMan, objArray->At(iclass)->GetName(), "event", "");
      }
      TString histTrackName = fConfigAddTrackHistogram.value;
      if (classStr.Contains("Track")) {
        dqhistograms::DefineHistograms(fHistMan, objArray->At(iclass)->GetName(), "track", histTrackName);
      }

      if (classStr.Contains("Pair")) {
        dqhistograms::DefineHistograms(fHistMan, objArray->At(iclass)->GetName(), "pair", "barreldalitz");
      }
    }

    fStatsList.setObject(new TList());
    fStatsList->SetOwner(kTRUE);

    // Dalitz selection statistics: one bin for each (track,pair) selection
    TH1I* histTracks = new TH1I("TrackStats", "Dalitz selection statistics", fPairCuts.size(), -0.5, fPairCuts.size() - 0.5);
    auto trackCut = fTrackCuts.begin();
    int icut = 0;
    for (auto pairCut = fPairCuts.begin(); pairCut != fPairCuts.end(); pairCut++, trackCut++, icut++) {
      histTracks->GetXaxis()->SetBinLabel(icut + 1, Form("%s_%s", (*trackCut).GetName(), (*pairCut).GetName()));
    }
    if (fQA) {
      fStatsList->Add(histTracks);
    }

    VarManager::SetUseVars(fHistMan->GetUsedVars()); // provide the list of required variables so that VarManager knows what to fill
    fOutputList.setObject(fHistMan->GetMainHistogramList());
  }

  template <uint32_t TTrackFillMap, typename TTracks>
  void runTrackSelection(TTracks const& tracksBarrel)
  {
    for (auto& track : tracksBarrel) {
      uint8_t filterMap = uint8_t(0);
      VarManager::FillTrack<TTrackFillMap>(track);
      int i = 0;
      for (auto cut = fTrackCuts.begin(); cut != fTrackCuts.end(); ++cut, ++i) {
        if ((*cut).IsSelected(VarManager::fgValues)) {
          filterMap |= (uint8_t(1) << i);
        }
      }
      if (filterMap) {
        fTrackmap[track.globalIndex()] = filterMap;
      }
    } // end loop over tracks
  }

  template <int TPairType, uint32_t TTrackFillMap, typename TTracks>
  void runDalitzPairing(TTracks const& tracks1, TTracks const& tracks2)
  {
    for (auto& [track1, track2] : o2::soa::combinations(CombinationsStrictlyUpperIndexPolicy(tracks1, tracks2))) {
      if (track1.sign() * track2.sign() > 0) {
        continue;
      }

      uint8_t twoTracksFilterMap = fTrackmap[track1.globalIndex()] & fTrackmap[track2.globalIndex()];
      if (!twoTracksFilterMap)
        continue;

      // pairing
      VarManager::FillPair<TPairType, TTrackFillMap>(track1, track2);

      // Fill pair selection map and fill pair histogram
      int icut = 0;
      auto trackCut = fTrackCuts.begin();
      for (auto pairCut = fPairCuts.begin(); pairCut != fPairCuts.end(); pairCut++, trackCut++, icut++) {
        if (!(twoTracksFilterMap & (uint8_t(1) << icut))) {
          continue;
        }
        if ((*pairCut).IsSelected(VarManager::fgValues)) {
          fDalitzmap[track1.globalIndex()] |= (uint8_t(1) << icut);
          fDalitzmap[track2.globalIndex()] |= (uint8_t(1) << icut);
          if (fQA) {
            fHistMan->FillHistClass(Form("Pair_%s_%s", (*trackCut).GetName(), (*pairCut).GetName()), VarManager::fgValues);
          }
        }
      }
    } // end of tracksP,N loop

    // Fill Hists
    if (fQA) {
      for (auto& track : tracks1) {
        uint8_t filterMap = fDalitzmap[track.globalIndex()];
        if (!filterMap) {
          continue;
        }
        VarManager::FillTrack<TTrackFillMap>(track);

        int icut = 0;
        auto trackCut = fTrackCuts.begin();
        for (auto pairCut = fPairCuts.begin(); pairCut != fPairCuts.end(); pairCut++, trackCut++, icut++) {
          if (filterMap & (uint8_t(1) << icut)) {
            ((TH1I*)fStatsList->At(0))->Fill(icut);
            fHistMan->FillHistClass(Form("TrackBarrel_%s_%s", (*trackCut).GetName(), (*pairCut).GetName()), VarManager::fgValues);
          }
        }
      }
    }
  }

  void processFullTracks(MyEvents const& collisions, soa::Filtered<MyBarrelTracks> const& filteredTracks, MyBarrelTracks const& tracks)
  {
    const int pairType = VarManager::kDecayToEE;
    fDalitzmap.clear();

    for (auto& collision : collisions) {
      fTrackmap.clear();
      VarManager::ResetValues(0, VarManager::kNBarrelTrackVariables);
      VarManager::FillEvent<gkEventFillMap>(collision);
      bool isEventSelected = fEventCut->IsSelected(VarManager::fgValues);

      if (isEventSelected) {
        auto groupedFilteredTracks = filteredTracks.sliceBy(perCollision, collision.globalIndex());
        runTrackSelection<gkTrackFillMap>(groupedFilteredTracks);
        runDalitzPairing<pairType, gkTrackFillMap>(groupedFilteredTracks, groupedFilteredTracks);
      }
    }

    for (auto& track : tracks) { // Fill dalitz bits
      dalitzbits(fDalitzmap[track.globalIndex()]);
    }
  }

  void processDummy(MyEvents&)
  {
  }

  PROCESS_SWITCH(dalitzPairing, processFullTracks, "Run Dalitz selection on AO2D tables", false);
  PROCESS_SWITCH(dalitzPairing, processDummy, "Do nothing", false);
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<dalitzPairing>(cfgc)};
}
