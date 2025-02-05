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

/// \brief skim cluster information to write photon cluster table in AO2D.root
/// dependencies: skimmergammacalo, skimmergammaconversions
/// \author marvin.hemmer@cern.ch

// TODO: add o2::aod::EMCALClusterCell and o2::aod::EMCALMatchedTrack to this for cell and track matching

#include "PWGEM/PhotonMeson/DataModel/gammaTables.h"

#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "Framework/AnalysisDataModel.h"

// includes for the R recalculation
#include "DetectorsBase/GeometryManager.h"
#include "DataFormatsParameters/GRPObject.h"
#include "CCDB/BasicCCDBManager.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

struct skimmerGamma {

  Produces<aod::SkimGammas> tableGammaReco;

  // Configurable for histograms
  Configurable<int> nBinsE{"nBinsE", 200, "N bins in E histo"};

  // Configurable for filter/cuts
  Configurable<float> minTime{"minTime", -200., "Minimum cluster time for time cut"};
  Configurable<float> maxTime{"maxTime", +200., "Maximum cluster time for time cut"};
  Configurable<float> minM02{"minM02", 0.0, "Minimum M02 for M02 cut"};
  Configurable<float> maxM02{"maxM02", 1.0, "Maximum M02 for M02 cut"};

  HistogramRegistry historeg{
    "historeg",
    {{"hCaloClusterEIn", "hCaloClusterEIn", {HistType::kTH1F, {{nBinsE, 0., 100.}}}},
     {"hCaloClusterEOut", "hCaloClusterEOut", {HistType::kTH1F, {{nBinsE, 0., 100.}}}}}};

  void init(o2::framework::InitContext&)
  {
    auto hCaloClusterFilter = historeg.add<TH1>("hCaloClusterFilter", "hCaloClusterFilter", kTH1I, {{4, 0, 4}});
    hCaloClusterFilter->GetXaxis()->SetBinLabel(1, "in");
    hCaloClusterFilter->GetXaxis()->SetBinLabel(2, "time cut");
    hCaloClusterFilter->GetXaxis()->SetBinLabel(3, "M02 cut");
    hCaloClusterFilter->GetXaxis()->SetBinLabel(4, "out");

    LOG(info) << "| Timing cut: " << minTime << " < t < " << maxTime << std::endl;
    LOG(info) << "| M02 cut: " << minM02 << " < M02 < " << maxM02 << std::endl;
  }

  void processRec(aod::Collision const&, aod::SkimEMCClusters const& caloclusters)
  {
    for (const auto& calocluster : caloclusters) { // loop of EMC clusters
      historeg.fill(HIST("hCaloClusterEIn"), calocluster.energy());
      historeg.fill(HIST("hCaloClusterFilter"), 0);

      if (calocluster.time() > maxTime || calocluster.time() < minTime) {
        historeg.fill(HIST("hCaloClusterFilter"), 1);
        continue;
      }
      if (calocluster.m02() > maxM02 || calocluster.m02() < minM02) {
        historeg.fill(HIST("hCaloClusterFilter"), 2);
        continue;
      }

      historeg.fill(HIST("hCaloClusterEOut"), calocluster.energy());
      historeg.fill(HIST("hCaloClusterFilter"), 3);

      tableGammaReco(calocluster.collisionId(), 1,
                     calocluster.energy(), calocluster.eta(), calocluster.phi(), calocluster.globalIndex());
    } // end loop of EMC clusters
  }
  PROCESS_SWITCH(skimmerGamma, processRec, "process only reconstructed info", true);
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  WorkflowSpec workflow{adaptAnalysisTask<skimmerGamma>(cfgc)};
  return workflow;
}
