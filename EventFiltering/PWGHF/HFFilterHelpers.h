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
// O2 includes

/// \file HFFilterHelpers.h
/// \brief Header file with definition of variables, methods, and tables used in the HFFilter.cxx task
///
/// \author Fabrizio Grosa <fabrizio.grosa@cern.ch>, CERN
/// \author Marcel Lesch <marcel.lesch@tum.de>, TUM
/// \author Alexandre Bigot <alexandre.bigot@cern.ch>, Strasbourg University
/// \author Biao Zhang <biao.zhang@cern.ch>, CCNU

#ifndef O2_ANALYSIS_HF_FILTER_HELPERS_H_
#define O2_ANALYSIS_HF_FILTER_HELPERS_H_

#include "Framework/DataTypes.h"
#include "Framework/AnalysisDataModel.h"
#include "Framework/HistogramRegistry.h"
#include "Common/Core/RecoDecay.h"
#include "PWGHF/DataModel/CandidateReconstructionTables.h"
#include "PWGHF/DataModel/CandidateSelectionTables.h"

#include <vector>
#include <array>
#include <string>
#include <cmath>

#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/GenVector/Boost.h"

// CCDB
#include "CCDB/CcdbApi.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

namespace o2::aod
{

namespace hffilters
{

enum HfTriggers {
  kHighPt2P = 0,
  kHighPt3P,
  kBeauty3P,
  kBeauty4P,
  kFemto2P,
  kFemto3P,
  kDoubleCharm2P,
  kDoubleCharm3P,
  kDoubleCharmMix,
  kGammaCharm2P,
  kGammaCharm3P,
  kNtriggersHF
};

enum charmParticles {
  kD0 = 0,
  kDplus,
  kDs,
  kLc,
  kXic,
  kNCharmParticles
};

enum beautyParticles {
  kBplus = 0,
  kB0toDStar,
  kB0,
  kBs,
  kLb,
  kXib,
  kNBeautyParticles
};

enum beautyTrackSelection {
  kRejected = 0,
  kSoftPion,
  kRegular
};

enum PIDSpecies {
  kEl = 0,
  kKa,
  kPi,
  kPr
};

static const std::array<std::string, kNtriggersHF> HfTriggerNames{"highPt", "beauty", "femto", "doubleCharm", "softGamma"};
static const std::array<std::string, kNCharmParticles> charmParticleNames{"D0", "Dplus", "Ds", "Lc", "Xic"};
static const std::array<std::string, kNBeautyParticles> beautyParticleNames{"Bplus", "B0toDStar", "B0", "Bs", "Lb", "Xib"};
static const std::array<int, kNCharmParticles> pdgCodesCharm{421, 411, 431, 4122, 4232};

static const std::tuple pdgCharmDaughters{
  std::array{-321, 211},        // D0
  std::array{-321, 211, 211},   // Dplus
  std::array{321, -321, 211},   // Ds
  std::array{2212, -321, 211},  // Lc
  std::array{2212, -321, 211}}; // Xic

static const float massPi = RecoDecay::getMassPDG(211);
static const float massK = RecoDecay::getMassPDG(321);
static const float massProton = RecoDecay::getMassPDG(2212);
static const float massPhi = RecoDecay::getMassPDG(333);
static const float massD0 = RecoDecay::getMassPDG(421);
static const float massDPlus = RecoDecay::getMassPDG(411);
static const float massDs = RecoDecay::getMassPDG(431);
static const float massLc = RecoDecay::getMassPDG(4122);
static const float massXic = RecoDecay::getMassPDG(4232);
static const float massDStar = RecoDecay::getMassPDG(413);
static const float massBPlus = RecoDecay::getMassPDG(511);
static const float massB0 = RecoDecay::getMassPDG(521);
static const float massBs = RecoDecay::getMassPDG(531);
static const float massLb = RecoDecay::getMassPDG(5122);
static const float massXib = RecoDecay::getMassPDG(5232);
static const float massGamma = RecoDecay::getMassPDG(22);

static const AxisSpec ptAxis{50, 0.f, 50.f};
static const AxisSpec pAxis{50, 0.f, 10.f};
static const AxisSpec kstarAxis{100, 0.f, 1.f};
static const AxisSpec etaAxis{30, -1.5f, 1.5f};
static const AxisSpec nSigmaAxis{100, -10.f, 10.f};
static const AxisSpec alphaAxis{100, -10.f, 10.f};
static const AxisSpec bdtAxis{100, 0.f, 1.f};
static const std::array<AxisSpec, kNCharmParticles + 3> massAxisC = {AxisSpec{100, 1.65f, 2.05f}, AxisSpec{100, 1.65f, 2.05f}, AxisSpec{100, 1.75f, 2.15f}, AxisSpec{100, 2.05f, 2.45f}, AxisSpec{100, 2.25f, 2.65f}, AxisSpec{100, 1.98f, 2.08f}, AxisSpec{100, 1.98f, 2.08f}, AxisSpec{100, 2.08f, 2.18f}};
static const std::array<AxisSpec, kNBeautyParticles> massAxisB = {AxisSpec{100, 5.0f, 5.6f}, AxisSpec{100, 5.0f, 5.6f}, AxisSpec{100, 5.0f, 5.6f}, AxisSpec{100, 5.0f, 5.6f}, AxisSpec{100, 5.3f, 5.9f}, AxisSpec{100, 5.3f, 5.9f}};

/// Single-track cuts for bachelor track of beauty candidates
/// \param track is a track
/// \param pTMinSoftPion min pT for soft pions
/// \param pTMinBeautyBachelor min pT for beauty bachelor pions
/// \param cutsSingleTrackBeauty cuts for all tracks
/// \return 0 if track is rejected, 1 if track is soft pion, 2 if it is regular beauty
template <typename T1, typename T2, typename T3>
int isSelectedTrackForBeauty(const T1& track, const float& pTMinSoftPion, const float& pTMinBeautyBachelor, const T2& pTBinsTrack, const T3& cutsSingleTrackBeauty)
{
  auto pT = track.pt();
  auto pTBinTrack = findBin(pTBinsTrack, pT);
  if (pTBinTrack == -1) {
    return kRejected;
  }

  if (pT < pTMinSoftPion) { // soft pion should be more stringent than usual tracks
    return kRejected;
  }

  if (std::abs(track.eta()) > 0.8) {
    return kRejected;
  }

  if (std::abs(track.dcaZ()) > 2.f) {
    return kRejected;
  }

  if (std::abs(track.dcaXY()) < cutsSingleTrackBeauty.get(pTBinTrack, "min_dcaxytoprimary")) {
    return kRejected; // minimum DCAxy
  }
  if (std::abs(track.dcaXY()) > cutsSingleTrackBeauty.get(pTBinTrack, "max_dcaxytoprimary")) {
    return kRejected; // maximum DCAxy
  }

  // below only regular beauty tracks, not required for soft pions
  if (pT < pTMinBeautyBachelor) {
    return kSoftPion;
  }

  return kRegular;
}

/// Basic selection of proton candidates
/// \param track is a track
/// \param femtoMinProtonPt min pT for proton candidates
/// \param femtoMaxNsigmaProton max Nsigma for proton candidates
/// \param femtoProtonOnlyTOF flag to activate PID selection with TOF only
/// \param computeTPCPostCalib flag to activate TPC PID postcalibrations
/// \param hMapProtonMean map of nSigma mean calibrations
/// \param hMapProtonSigma map of nSigma sigma calibrations
/// \param activateQA flag to activate the filling of QA histos
/// \param hProtonTPCPID histo with NsigmaTPC vs. p
/// \param hProtonTOFPID histo with NsigmaTOF vs. p
/// \return true if track passes all cuts
template <typename T, typename H2, typename H3>
bool isSelectedProton4Femto(const T& track, const float& femtoMinProtonPt, const float& femtoMaxNsigmaProton, const bool femtoProtonOnlyTOF, const bool computeTPCPostCalib, H3 hMapProtonMean, H3 hMapProtonSigma, const int& activateQA, H2 hProtonTPCPID, H2 hProtonTOFPID)
{
  if (track.pt() < femtoMinProtonPt) {
    return false;
  }

  if (std::abs(track.eta()) > 0.8) {
    return false;
  }

  if (track.isGlobalTrack() != (uint8_t) true) {
    return false; // use only global tracks
  }

  float NSigmaTPC = track.tpcNSigmaPr();
  float NSigmaTOF = track.tofNSigmaPr();
  float NSigma;

  if (computeTPCPostCalib) {
    NSigmaTPC = getTPCPostCalib(hMapProtonMean, hMapProtonSigma, track, kPr);
  }

  if (femtoProtonOnlyTOF) {
    NSigma = abs(NSigmaTOF);
  } else {
    NSigma = sqrt(NSigmaTPC * NSigmaTPC + NSigmaTOF * NSigmaTOF);
  }

  if (NSigma > femtoMaxNsigmaProton) {
    return false;
  }

  if (activateQA > 1) {
    hProtonTPCPID->Fill(track.p(), NSigmaTPC);
    hProtonTOFPID->Fill(track.p(), NSigmaTOF);
  }

  return true;
}

/// Basic selection of proton candidates for Lc
/// \param track is a track
/// \param nsigmaTPCProtonLc max NsigmaTPC for proton candidates
/// \param nsigmaTOFProtonLc max NsigmaTOF for proton candidates
/// \param computeTPCPostCalib flag to activate TPC PID postcalibrations
/// \param hMapProtonMean map of nSigma mean calibrations
/// \param hMapProtonSigma map of nSigma sigma calibrations
/// \return true if track passes all cuts
template <typename T, typename H3>
bool isSelectedProton4CharmBaryons(const T& track, const float& nsigmaTPCProtonLc, const float& nsigmaTOFProtonLc, const bool computeTPCPostCalib, H3 hMapProtonMean, H3 hMapProtonSigma)
{
  float NSigmaTPC = track.tpcNSigmaPr();
  float NSigmaTOF = track.tofNSigmaPr();

  if (computeTPCPostCalib) {
    NSigmaTPC = getTPCPostCalib(hMapProtonMean, hMapProtonSigma, track, kPr);
  }

  if (std::abs(NSigmaTPC) > nsigmaTPCProtonLc) {
    return false;
  }
  if (track.hasTOF() && std::abs(NSigmaTOF) > nsigmaTOFProtonLc) {
    return false;
  }

  return true;
}

/// Basic selection of kaon candidates for charm candidates
/// \param track is a track
/// \param nsigmaTPCKaon3Prong max NsigmaTPC for kaon candidates
/// \param nsigmaTOFKaon3Prong max NsigmaTOF for kaon candidates
/// \param computeTPCPostCalib flag to activate TPC PID postcalibrations
/// \param hMapPionMean map of nSigma mean calibrations
/// \param hMapPionSigma map of nSigma sigma calibrations
/// \return true if track passes all cuts
template <typename T, typename H3>
bool isSelectedKaon4Charm3Prong(const T& track, const float& nsigmaTPCKaon3Prong, const float& nsigmaTOFKaon3Prong, const bool computeTPCPostCalib, H3 hMapPionMean, H3 hMapPionSigma)
{
  float NSigmaTPC = track.tpcNSigmaKa();
  float NSigmaTOF = track.tofNSigmaKa();

  if (computeTPCPostCalib) {
    NSigmaTPC = getTPCPostCalib(hMapPionMean, hMapPionSigma, track, kKa); // use pion correction map for kaon for the moment
  }

  if (std::abs(NSigmaTPC) > nsigmaTPCKaon3Prong) {
    return false;
  }
  if (track.hasTOF() && std::abs(NSigmaTOF) > nsigmaTOFKaon3Prong) {
    return false;
  }

  return true;
}

/// Basic additional selection of D+ candidates
/// \param trackOppositeCharge is the opposite charge track
/// \param nsigmaTPCKaon3Prong max NsigmaTPC for kaon candidates
/// \param nsigmaTOFKaon3Prong max NsigmaTOF for kaon candidates
/// \param computeTPCPostCalib flag to activate TPC PID postcalibrations
/// \param hMapPionMean map of nSigma mean calibrations
/// \param hMapPionSigma map of nSigma sigma calibrations
/// \return BIT(0) for Kpipi
template <typename T, typename H3>
int8_t isDplusPreselected(const T& trackOppositeCharge, const float& nsigmaTPCKaon3Prong, const float& nsigmaTOFKaon3Prong, const bool computeTPCPostCalib, H3 hMapPionMean, H3 hMapPionSigma)
{
  int8_t retValue = 0;

  // check PID of opposite charge track
  if (!isSelectedKaon4Charm3Prong(trackOppositeCharge, nsigmaTPCKaon3Prong, nsigmaTOFKaon3Prong, computeTPCPostCalib, hMapPionMean, hMapPionSigma)) {
    return retValue;
  }

  retValue |= BIT(0);
  return retValue;
}

/// Basic additional selection of Ds candidates
/// \param pTrackSameChargeFirst is the first same-charge track momentum
/// \param pTrackSameChargeSecond is the second same-charge track momentum
/// \param pTrackOppositeCharge is the opposite charge track momentum
/// \param trackOppositeCharge is the opposite charge track
/// \param nsigmaTPCKaon3Prong max NsigmaTPC for kaon candidates
/// \param nsigmaTOFKaon3Prong max NsigmaTOF for kaon candidates
/// \param computeTPCPostCalib flag to activate TPC PID postcalibrations
/// \param hMapPionMean map of nSigma mean calibrations
/// \param hMapPionSigma map of nSigma sigma calibrations
/// \return BIT(0) for KKpi, BIT(1) for piKK
template <typename P, typename T, typename H3>
int8_t isDsPreselected(const P& pTrackSameChargeFirst, const P& pTrackSameChargeSecond, const P& pTrackOppositeCharge, const T& trackOppositeCharge, const float& nsigmaTPCKaon3Prong, const float& nsigmaTOFKaon3Prong, const bool computeTPCPostCalib, H3 hMapPionMean, H3 hMapPionSigma)
{
  int8_t retValue = 0;

  // check PID of opposite charge track
  if (!isSelectedKaon4Charm3Prong(trackOppositeCharge, nsigmaTPCKaon3Prong, nsigmaTOFKaon3Prong, computeTPCPostCalib, hMapPionMean, hMapPionSigma)) {
    return retValue;
  }

  // check delta-mass for phi resonance
  auto invMassKKFirst = RecoDecay::m(std::array{pTrackSameChargeFirst, pTrackOppositeCharge}, std::array{massK, massK});
  auto invMassKKSecond = RecoDecay::m(std::array{pTrackSameChargeSecond, pTrackOppositeCharge}, std::array{massK, massK});

  if (std::abs(invMassKKFirst - massPhi) < 0.02) {
    retValue |= BIT(0);
  }
  if (std::abs(invMassKKSecond - massPhi) < 0.02) {
    retValue |= BIT(1);
  }

  return retValue;
}

/// Basic additional selection of Lc->pKpi and Xic->pKpi candidates
/// \param trackSameChargeFirst is the first same-charge track
/// \param trackSameChargeSecond is the second same-charge track
/// \param trackOppositeCharge is the opposite charge track
/// \param nsigmaTPCProtonLc max NsigmaTPC for proton candidates
/// \param nsigmaTOFProtonLc max NsigmaTOF for proton candidates
/// \param nsigmaTPCKaon3Prong max NsigmaTPC for kaon candidates
/// \param nsigmaTOFKaon3Prong max NsigmaTOF for kaon candidates
/// \param computeTPCPostCalib flag to activate TPC PID postcalibrations
/// \param hMapProtonMean map of nSigma mean calibrations for protons
/// \param hMapProtonSigma map of nSigma sigma calibrations for protons
/// \param hMapPionMean map of nSigma mean calibrations for pions
/// \param hMapPionSigma map of nSigma sigma calibrations for pions
/// \return BIT(0) for pKpi, BIT(1) for piKp
template <typename T, typename H3>
int8_t isCharmBaryonPreselected(const T& trackSameChargeFirst, const T& trackSameChargeSecond, const T& trackOppositeCharge, const float& nsigmaTPCProtonLc, const float& nsigmaTOFProtonLc, const float& nsigmaTPCKaon3Prong, const float& nsigmaTOFKaon3Prong, const bool computeTPCPostCalib, H3 hMapProtonMean, H3 hMapProtonSigma, H3 hMapPionMean, H3 hMapPionSigma)
{
  int8_t retValue = 0;
  // check PID of opposite charge track
  if (!isSelectedKaon4Charm3Prong(trackOppositeCharge, nsigmaTPCKaon3Prong, nsigmaTOFKaon3Prong, computeTPCPostCalib, hMapPionMean, hMapPionSigma)) {
    return retValue;
  }

  if (isSelectedProton4CharmBaryons(trackSameChargeFirst, nsigmaTPCProtonLc, nsigmaTOFProtonLc, computeTPCPostCalib, hMapProtonMean, hMapProtonSigma)) {
    retValue |= BIT(0);
  }
  if (isSelectedProton4CharmBaryons(trackSameChargeSecond, nsigmaTPCProtonLc, nsigmaTOFProtonLc, computeTPCPostCalib, hMapProtonMean, hMapProtonSigma)) {
    retValue |= BIT(1);
  }

  return retValue;
}

/// Basic additional selection of D0 candidates
/// \param trackPos is the positive track
/// \param trackNeg is the negative track
/// \param nsigmaTPCPionKaonDzero max NsigmaTPC for pion/kaon candidates
/// \param nsigmaTOFPionKaonDzero max NsigmaTOF for pion/kaon candidates
/// \param computeTPCPostCalib flag to activate TPC PID postcalibrations
/// \param hMapPionMean map of nSigma mean calibrations
/// \param hMapPionSigma map of nSigma sigma calibrations
/// \return BIT(0) for D0, BIT(1) for D0bar
template <typename T, typename H3>
int8_t isDzeroPreselected(const T& trackPos, const T& trackNeg, const float& nsigmaTPCPionKaonDzero, const float& nsigmaTOFPionKaonDzero, const bool computeTPCPostCalib, H3 hMapPionMean, H3 hMapPionSigma)
{
  int8_t retValue = 0;

  float NSigmaPiTPCPos = trackPos.tpcNSigmaPi();
  float NSigmaPiTOFPos = trackPos.tofNSigmaPi();
  float NSigmaKaTPCPos = trackPos.tpcNSigmaKa();
  float NSigmaKaTOFPos = trackPos.tofNSigmaKa();

  float NSigmaPiTPCNeg = trackNeg.tpcNSigmaPi();
  float NSigmaPiTOFNeg = trackNeg.tofNSigmaPi();
  float NSigmaKaTPCNeg = trackNeg.tpcNSigmaKa();
  float NSigmaKaTOFNeg = trackNeg.tofNSigmaKa();

  if (computeTPCPostCalib) {
    NSigmaPiTPCPos = getTPCPostCalib(hMapPionMean, hMapPionSigma, trackPos, kPi);
    NSigmaPiTPCNeg = getTPCPostCalib(hMapPionMean, hMapPionSigma, trackNeg, kPi);
    NSigmaKaTPCPos = getTPCPostCalib(hMapPionMean, hMapPionSigma, trackPos, kKa); // use pion correction map for kaon for the moment
    NSigmaKaTPCNeg = getTPCPostCalib(hMapPionMean, hMapPionSigma, trackNeg, kKa); // use pion correction map for kaon for the moment
  }

  if ((std::abs(NSigmaPiTPCPos) <= nsigmaTPCPionKaonDzero && (!trackPos.hasTOF() || std::abs(NSigmaPiTOFPos) <= nsigmaTOFPionKaonDzero)) && (std::abs(NSigmaKaTPCNeg) <= nsigmaTPCPionKaonDzero && (!trackNeg.hasTOF() || std::abs(NSigmaKaTOFNeg) <= nsigmaTOFPionKaonDzero))) {
    retValue |= BIT(0);
  }
  if ((std::abs(NSigmaPiTPCNeg) <= nsigmaTPCPionKaonDzero && (!trackNeg.hasTOF() || std::abs(NSigmaPiTOFNeg) <= nsigmaTOFPionKaonDzero)) && (std::abs(NSigmaKaTPCPos) <= nsigmaTPCPionKaonDzero && (!trackPos.hasTOF() || std::abs(NSigmaKaTOFPos) <= nsigmaTOFPionKaonDzero))) {
    retValue |= BIT(1);
  }

  return retValue;
}

/// Mass selection of D0 candidates to build Bplus candidates
/// \param pTrackPos is the positive track momentum
/// \param pTrackNeg is the negative track momentum
/// \param ptD is the pt of the D0 meson candidate
/// \param isSelected is the flag containing the selection tag for the D0 candidate
/// \param deltaMassCharmHadronForBeauty is the maximum delta mass value
/// \param activateQA flag to activate the filling of QA histos
/// \param hMassVsPt histo with invariant mass vs pt
/// \return 1 for D0, 2 for D0bar, 3 for both
template <typename T, typename H2>
int8_t isSelectedD0InMassRange(const T& pTrackPos, const T& pTrackNeg, const float& ptD, int8_t isSelected, const float& deltaMassCharmHadronForBeauty, const int& activateQA, H2 hMassVsPt)
{
  int8_t retValue = 0;
  if (TESTBIT(isSelected, 0)) {
    auto invMassD0 = RecoDecay::m(std::array{pTrackPos, pTrackNeg}, std::array{massPi, massK});
    if (activateQA) {
      hMassVsPt->Fill(ptD, invMassD0);
    }
    if (std::abs(invMassD0 - massD0) < deltaMassCharmHadronForBeauty) {
      retValue |= BIT(0);
    }
  }
  if (TESTBIT(isSelected, 1)) {
    auto invMassD0bar = RecoDecay::m(std::array{pTrackPos, pTrackNeg}, std::array{massK, massPi});
    if (activateQA) {
      hMassVsPt->Fill(ptD, invMassD0bar);
    }
    if (std::abs(invMassD0bar - massD0) < deltaMassCharmHadronForBeauty) {
      retValue |= BIT(1);
    }
  }

  return retValue;
}

/// Mass selection of D+ candidates to build B0 candidates
/// \param pTrackSameChargeFirst is the first same-charge track momentum
/// \param pTrackSameChargeFirst is the second same-charge track momentum
/// \param pTrackSameChargeFirst is the opposite charge track momentum
/// \param ptD is the pt of the D+ meson candidate
/// \param deltaMassCharmHadronForBeauty is the maximum delta mass value
/// \param activateQA flag to activate the filling of QA histos
/// \param hMassVsPt histo with invariant mass vs pt
/// \return BIT(0) (==1) for D+, 0 otherwise
template <typename T, typename H2>
int8_t isSelectedDplusInMassRange(const T& pTrackSameChargeFirst, const T& pTrackSameChargeSecond, const T& pTrackOppositeCharge, const float& ptD, const float& deltaMassCharmHadronForBeauty, const int& activateQA, H2 hMassVsPt)
{
  auto invMassDplus = RecoDecay::m(std::array{pTrackSameChargeFirst, pTrackSameChargeSecond, pTrackOppositeCharge}, std::array{massPi, massPi, massK});
  if (activateQA) {
    hMassVsPt->Fill(ptD, invMassDplus);
  }

  if (std::abs(invMassDplus - massDPlus) > deltaMassCharmHadronForBeauty) {
    return 0;
  }

  return BIT(0);
}

/// Mass selection of of Ds candidates to build Bs candidates
/// \param pTrackSameChargeFirst is the first same-charge track momentum
/// \param pTrackSameChargeFirst is the second same-charge track momentum
/// \param pTrackSameChargeFirst is the opposite charge track momentum
/// \param ptD is the pt of the Ds meson candidate
/// \param isSelected is the flag containing the selection tag for the Ds candidate
/// \param deltaMassCharmHadronForBeauty is the maximum delta mass value
/// \param activateQA flag to activate the filling of QA histos
/// \param hMassVsPt histo with invariant mass vs pt
/// \return BIT(0) for KKpi, BIT(1) for piKK, BIT(2) for phipi, BIT(3) for piphi
template <typename T, typename H2>
int8_t isSelectedDsInMassRange(const T& pTrackSameChargeFirst, const T& pTrackSameChargeSecond, const T& pTrackOppositeCharge, const float& ptD, int8_t isSelected, const float& deltaMassCharmHadronForBeauty, const int& activateQA, H2 hMassVsPt)
{
  int8_t retValue = 0;
  if (TESTBIT(isSelected, 0)) {
    auto invMassDsToKKPi = RecoDecay::m(std::array{pTrackSameChargeFirst, pTrackOppositeCharge, pTrackSameChargeSecond}, std::array{massK, massK, massPi});
    if (activateQA) {
      hMassVsPt->Fill(ptD, invMassDsToKKPi);
    }
    if (std::abs(invMassDsToKKPi - massDs) < deltaMassCharmHadronForBeauty) {
      retValue |= BIT(0);
    }
  }
  if (TESTBIT(isSelected, 1)) {
    auto invMassDsToPiKK = RecoDecay::m(std::array{pTrackSameChargeFirst, pTrackOppositeCharge, pTrackSameChargeSecond}, std::array{massPi, massK, massK});
    if (activateQA) {
      hMassVsPt->Fill(ptD, invMassDsToPiKK);
    }
    if (std::abs(invMassDsToPiKK - massDs) < deltaMassCharmHadronForBeauty) {
      retValue |= BIT(1);
    }
  }

  return retValue;
}

/// Mass selection of Lc candidates to build Lb candidates
/// \param pTrackSameChargeFirst is the first same-charge track momentum
/// \param pTrackSameChargeSecond is the second same-charge track momentum
/// \param pTrackOppositeCharge is the opposite charge track momentum
/// \param ptLc is the pt of the D0 meson candidate
/// \param isSelected is the flag containing the selection tag for the D0 candidate
/// \param deltaMassCharmHadronForBeauty is the maximum delta mass value
/// \param activateQA flag to activate the filling of QA histos
/// \param hMassVsPt histo with invariant mass vs pt
/// \return BIT(0) for pKpi with mass cut, BIT(1) for piKp with mass cut
template <typename T, typename H2>
int8_t isSelectedLcInMassRange(const T& pTrackSameChargeFirst, const T& pTrackSameChargeSecond, const T& pTrackOppositeCharge, const float& ptLc, const int8_t isSelected, const float& deltaMassCharmHadronForBeauty, const int& activateQA, H2 hMassVsPt)
{
  int8_t retValue = 0;
  if (TESTBIT(isSelected, 0)) {
    auto invMassLcToPKPi = RecoDecay::m(std::array{pTrackSameChargeFirst, pTrackOppositeCharge, pTrackSameChargeSecond}, std::array{massProton, massK, massPi});
    if (activateQA) {
      hMassVsPt->Fill(ptLc, invMassLcToPKPi);
    }
    if (std::abs(invMassLcToPKPi - massLc) < deltaMassCharmHadronForBeauty) {
      retValue |= BIT(0);
    }
  }
  if (TESTBIT(isSelected, 1)) {
    auto invMassLcToPiKP = RecoDecay::m(std::array{pTrackSameChargeFirst, pTrackOppositeCharge, pTrackSameChargeSecond}, std::array{massPi, massK, massProton});
    if (activateQA) {
      hMassVsPt->Fill(ptLc, invMassLcToPiKP);
    }
    if (std::abs(invMassLcToPiKP - massLc) < deltaMassCharmHadronForBeauty) {
      retValue |= BIT(1);
    }
  }

  return retValue;
}

/// Mass selection of Xic candidates to build Lb candidates
/// \param pTrackSameChargeFirst is the first same-charge track momentum
/// \param pTrackSameChargeSecond is the second same-charge track momentum
/// \param pTrackOppositeCharge is the opposite charge track momentum
/// \param ptXic is the pt of the D0 meson candidate
/// \param isSelected is the flag containing the selection tag for the D0 candidate
/// \param deltaMassCharmHadronForBeauty is the maximum delta mass value
/// \param activateQA flag to activate the filling of QA histos
/// \param hMassVsPt histo with invariant mass vs pt
/// \return BIT(0) for pKpi with mass cut, BIT(1) for piKp with mass cut
template <typename T, typename H2>
int8_t isSelectedXicInMassRange(const T& pTrackSameChargeFirst, const T& pTrackSameChargeSecond, const T& pTrackOppositeCharge, const float& ptXic, const int8_t isSelected, const float& deltaMassCharmHadronForBeauty, const int& activateQA, H2 hMassVsPt)
{
  int8_t retValue = 0;
  if (TESTBIT(isSelected, 0)) {
    auto invMassXicToPKPi = RecoDecay::m(std::array{pTrackSameChargeFirst, pTrackOppositeCharge, pTrackSameChargeSecond}, std::array{massProton, massK, massPi});
    if (activateQA) {
      hMassVsPt->Fill(ptXic, invMassXicToPKPi);
    }
    if (std::abs(invMassXicToPKPi - massXic) < deltaMassCharmHadronForBeauty) {
      retValue |= BIT(0);
    }
  }
  if (TESTBIT(isSelected, 1)) {
    auto invMassXicToPiKP = RecoDecay::m(std::array{pTrackSameChargeFirst, pTrackOppositeCharge, pTrackSameChargeSecond}, std::array{massPi, massK, massProton});
    if (activateQA) {
      hMassVsPt->Fill(ptXic, invMassXicToPiKP);
    }
    if (std::abs(invMassXicToPiKP - massXic) < deltaMassCharmHadronForBeauty) {
      retValue |= BIT(1);
    }
  }

  return retValue;
}

/// Basic selection of gamma candidates
/// \param gamma is the gamma candidate
/// \param gammaCosinePA is the cosp of the gamma
/// \param hGammaSelected is the pointer to the QA histo for selected gammas
/// \param hGammaEtaBefore is the pointer to the QA histo for gamma eta before selection
/// \param hGammaArmPodBefore is the pointer to the QA histo AP plot before selection
/// \param hGammaEtaAfter is the pointer to the QA histo for gamma eta before selection
/// \param hGammaArmPodAfter is the pointer to the QA histo AP plot before selection
/// \return true if gamma passes all cuts
template <typename T, typename H1, typename H2>
bool isSelectedGamma(const T& gamma, const float& gammaCosinePA, const int& activateQA, H1 hGammaSelected, H1 hGammaEtaBefore, H1 hGammaEtaAfter, H2 hGammaArmPodBefore, H2 hGammaArmPodAfter)
{
  if (activateQA > 1) {
    hGammaSelected->Fill(0);
    hGammaEtaBefore->Fill(gamma.eta());
    hGammaArmPodBefore->Fill(gamma.alpha(), gamma.qtarm());
  }
  if (std::abs(gamma.eta()) > 0.8) {
    if (activateQA > 1)
      hGammaSelected->Fill(1);
    return false;
  }

  if (gamma.v0radius() < 0. || gamma.v0radius() > 180.) {
    if (activateQA > 1)
      hGammaSelected->Fill(2);
    return false;
  }

  if ((std::pow(gamma.alpha() / 0.95, 2) + std::pow(gamma.qtarm() / 0.05, 2)) >= 1) {
    if (activateQA > 1)
      hGammaSelected->Fill(3);
    return false;
  }

  if (std::abs(gamma.psipair()) > 0.1) {
    if (activateQA > 1)
      hGammaSelected->Fill(4);
    return false;
  }

  if (gammaCosinePA < 0.85) {
    if (activateQA > 1)
      hGammaSelected->Fill(5);
    return false;
  }

  if (activateQA > 1) {
    hGammaSelected->Fill(6);
    hGammaEtaAfter->Fill(gamma.eta());
    hGammaArmPodAfter->Fill(gamma.alpha(), gamma.qtarm());
  }
  return true;
}

/// Single-track cuts for bachelor track of beauty candidates
/// \param scores is a 3-element array with BDT out scores
/// \param thresholdBDTScores is the LabelledArray containing the BDT cut values
/// \return 0 if rejected, otherwise bitmap with BIT(RecoDecay::OriginType::Prompt) and/or BIT(RecoDecay::OriginType::NonPrompt) on
template <typename T, typename U>
int8_t isBDTSelected(const T& scores, const U& thresholdBDTScores)
{
  int8_t retValue = 0;
  if (scores.size() < 3) {
    return retValue;
  }

  if (scores[0] > thresholdBDTScores.get(0u, "BDTbkg")) {
    return retValue;
  }
  if (scores[1] > thresholdBDTScores.get(0u, "BDTprompt")) {
    retValue |= BIT(RecoDecay::OriginType::Prompt);
  }
  if (scores[2] > thresholdBDTScores.get(0u, "BDTnonprompt")) {
    retValue |= BIT(RecoDecay::OriginType::NonPrompt);
  }

  return retValue;
}

/// Computation of the relative momentum between particle pairs
/// \param track is a track
/// \param ProtonMass is the mass of a proton
/// \param CharmCandMomentum is the three momentum of a charm candidate
/// \param CharmMass is the mass of the charm hadron
/// \return relative momentum of pair
template <typename T>
float computeRelativeMomentum(const T& track, const std::array<float, 3>& CharmCandMomentum, const float& CharmMass)
{
  ROOT::Math::PxPyPzMVector part1(track.px(), track.py(), track.pz(), massProton);
  ROOT::Math::PxPyPzMVector part2(CharmCandMomentum[0], CharmCandMomentum[1], CharmCandMomentum[2], CharmMass);

  ROOT::Math::PxPyPzMVector trackSum = part1 + part2;
  ROOT::Math::Boost boostv12{trackSum.BoostToCM()};
  ROOT::Math::PxPyPzMVector part1CM = boostv12(part1);
  ROOT::Math::PxPyPzMVector part2CM = boostv12(part2);
  ROOT::Math::PxPyPzMVector trackRelK = part1CM - part2CM;

  float kStar = 0.5 * trackRelK.P();
  return kStar;
} // float computeRelativeMomentum(const T& track, const std::array<float, 3>& CharmCandMomentum, const float& CharmMass)

/// Computation of the number of candidates in an event that do not share daughter tracks
/// \return 0 or 1 in case of less than 2 independent candidates in a single event, 2 otherwise
template <typename T>
int computeNumberOfCandidates(std::vector<std::vector<T>> indices)
{
  if (indices.size() < 2) {
    return indices.size();
  }

  std::vector<int> numIndependentCand{};
  for (auto iCand{0u}; iCand < indices.size(); ++iCand) {
    int nIndependent = 0;
    for (auto iCandSecond{0u}; iCandSecond < indices.size(); ++iCandSecond) {
      if (iCand == iCandSecond) {
        continue;
      } else {
        bool hasOverlap = false;
        for (auto idxFirst{0u}; idxFirst < indices[iCand].size(); ++idxFirst) {
          for (auto idxSecond{0u}; idxSecond < indices[iCandSecond].size(); ++idxSecond) {
            if (indices[iCand][idxFirst] == indices[iCandSecond][idxSecond]) {
              hasOverlap = true;
              break;
            }
          }
        }
        if (!hasOverlap) {
          nIndependent++;
        }
      }
    }
    numIndependentCand.push_back(nIndependent);
  }
  std::sort(numIndependentCand.begin(), numIndependentCand.end());

  if (numIndependentCand.back() == 0) {
    return numIndependentCand.back();
  }

  return 2;
}

/// ML helper methods

/// Iinitialisation of ONNX session
/// \param onnxFile is the onnx file name
/// \param partName is the particle name
/// \param env is the ONNX environment
/// \param sessionOpt is the ONNX session options
/// \param inputShapes is the input shape
/// \param dataType is the data type (1=float, 11=double)
/// \param loadModelsFromCCDB is the flag to decide whether the ONNX file is read from CCDB or not
/// \param ccdbApi is the CCDB API
/// \param mlModelPathCCDB is the model path in CCDB
/// \param timestampCCDB is the CCDB timestamp
/// \return the pointer to the ONNX Ort::Experimental::Session
Ort::Experimental::Session* InitONNXSession(std::string& onnxFile, std::string partName, Ort::Env& env, Ort::SessionOptions& sessionOpt, std::vector<std::vector<int64_t>>& inputShapes, int& dataType, bool loadModelsFromCCDB, o2::ccdb::CcdbApi& ccdbApi, std::string mlModelPathCCDB, long timestampCCDB)
{
  // hard coded, we do not let the user change this
  sessionOpt.SetIntraOpNumThreads(1);
  sessionOpt.SetInterOpNumThreads(1);
  Ort::Experimental::Session* session = nullptr;

  std::map<std::string, std::string> metadata;
  bool retrieveSuccess = true;
  if (loadModelsFromCCDB && timestampCCDB > 0) {
    retrieveSuccess = ccdbApi.retrieveBlob(mlModelPathCCDB + partName, ".", metadata, timestampCCDB, false, onnxFile);
  }
  if (retrieveSuccess) {
    session = new Ort::Experimental::Session{env, onnxFile, sessionOpt};
    inputShapes = session->GetInputShapes();
    if (inputShapes[0][0] < 0) {
      LOGF(warning, Form("Model for %s with negative input shape likely because converted with hummingbird, setting it to 1.", partName.data()));
      inputShapes[0][0] = 1;
    }

    Ort::TypeInfo typeInfo = session->GetInputTypeInfo(0);
    auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();
    dataType = tensorInfo.GetElementType();
  } else {
    LOG(fatal) << "Error encountered while fetching/loading the ML model from CCDB! Maybe the ML model doesn't exist yet for this runnumber/timestamp?";
  }

  return session;
}

/// Iinitialisation of ONNX session
/// \param inputFeatures is the vector with input features
/// \param session is the ONNX Ort::Experimental::Session
/// \param inputShapes is the input shape
/// \return the array with the three output scores
template <typename T>
std::array<T, 3> PredictONNX(std::vector<T>& inputFeatures, std::shared_ptr<Ort::Experimental::Session>& session, std::vector<std::vector<int64_t>>& inputShapes)
{
  std::array<T, 3> scores{-1., 2., 2.};
  std::vector<Ort::Value> inputTensor{};
  inputTensor.push_back(Ort::Experimental::Value::CreateTensor<T>(inputFeatures.data(), inputFeatures.size(), inputShapes[0]));

  // double-check the dimensions of the input tensor
  if (inputTensor[0].GetTensorTypeAndShapeInfo().GetShape()[0] > 0) { // vectorial models can have negative shape if the shape is unknown
    assert(inputTensor[0].IsTensor() && inputTensor[0].GetTensorTypeAndShapeInfo().GetShape() == inputShapes[0]);
  }
  try {
    auto outputTensor = session->Run(session->GetInputNames(), inputTensor, session->GetOutputNames());
    assert(outputTensor.size() == session->GetOutputNames().size() && outputTensor[1].IsTensor());
    auto typeInfo = outputTensor[1].GetTensorTypeAndShapeInfo();
    assert(typeInfo.GetElementCount() == 3); // we need multiclass
    scores[0] = outputTensor[1].GetTensorMutableData<T>()[0];
    scores[1] = outputTensor[1].GetTensorMutableData<T>()[1];
    scores[2] = outputTensor[1].GetTensorMutableData<T>()[2];
  } catch (const Ort::Exception& exception) {
    LOG(error) << "Error running model inference: " << exception.what();
  }

  return scores;
}

/// PID postcalibrations

/// compute TPC postcalibrated nsigma based on calibration histograms from CCDB
/// \param hCalibMean calibration histograms of mean from CCDB
/// \param hCalibSigma calibration histograms of sigma from CCDB
/// \param track is the track
/// \param pidSpecies is the PID species
/// \return the corrected Nsigma value for the PID species
template <typename T>
float getTPCPostCalib(const TH3F* hCalibMean, const TH3F* hCalibSigma, const T& track, const int pidSpecies)
{
  auto tpcNCls = track.tpcNClsFound();
  auto tpcPin = track.tpcInnerParam();
  auto eta = track.eta();
  auto tpcNSigma = 0.;

  if (pidSpecies == kKa) {
    tpcNSigma = track.tpcNSigmaKa();
  } else if (pidSpecies == kPi) {
    tpcNSigma = track.tpcNSigmaPi();
  } else if (pidSpecies == kPr) {
    tpcNSigma = track.tpcNSigmaPr();
  } else {
    LOG(fatal) << "Wrong PID Species be selected, please check!";
  }
  auto binTPCNCls = hCalibMean->GetXaxis()->FindBin(tpcNCls);
  binTPCNCls = (binTPCNCls == 0 ? 1 : binTPCNCls);
  binTPCNCls = std::min(hCalibMean->GetXaxis()->GetNbins(), binTPCNCls);
  auto binPin = hCalibMean->GetYaxis()->FindBin(tpcPin);
  binPin = (binPin == 0 ? 1 : binPin);
  binPin = std::min(hCalibMean->GetYaxis()->GetNbins(), binPin);
  auto binEta = hCalibMean->GetZaxis()->FindBin(eta);
  binEta = (binEta == 0 ? 1 : binEta);
  binEta = std::min(hCalibMean->GetZaxis()->GetNbins(), binEta);

  auto mean = hCalibMean->GetBinContent(binTPCNCls, binPin, binEta);
  auto width = hCalibSigma->GetBinContent(binTPCNCls, binPin, binEta);

  return (tpcNSigma - mean) / width;
}
} // namespace hffilters

/// definition of tables

namespace extra2Prong
{
DECLARE_SOA_INDEX_COLUMN(Collision, collision);
} // namespace extra2Prong
namespace extra3Prong
{
DECLARE_SOA_INDEX_COLUMN(Collision, collision);
} // namespace extra3Prong
DECLARE_SOA_TABLE(Colls2Prong, "AOD", "COLLSID2P", o2::aod::extra2Prong::CollisionId);
DECLARE_SOA_TABLE(Colls3Prong, "AOD", "COLLSID3P", o2::aod::extra3Prong::CollisionId);

namespace hftraining
{
DECLARE_SOA_COLUMN(InvMassD0, invMassD0, float);                 //!
DECLARE_SOA_COLUMN(InvMassD0bar, invMassD0bar, float);           //!
DECLARE_SOA_COLUMN(InvMassDplus, invMassDplus, float);           //!
DECLARE_SOA_COLUMN(InvMassDsToKKPi, invMassDsToKKPi, float);     //!
DECLARE_SOA_COLUMN(InvMassDsToPiKK, invMassDsToPiKK, float);     //!
DECLARE_SOA_COLUMN(InvMassLcToPKPi, invMassLcToPKPi, float);     //!
DECLARE_SOA_COLUMN(InvMassLcToPiKP, invMassLcToPiKP, float);     //!
DECLARE_SOA_COLUMN(InvMassXicToPKPi, invMassXicToPKPi, float);   //!
DECLARE_SOA_COLUMN(InvMassXicToPiKP, invMassXicToPiKP, float);   //!
DECLARE_SOA_COLUMN(PT2Prong, pT2Prong, float);                   //!
DECLARE_SOA_COLUMN(PT3Prong, pT3Prong, float);                   //!
DECLARE_SOA_COLUMN(DeltaMassKKFirst, deltaMassKKFirst, float);   //!
DECLARE_SOA_COLUMN(DeltaMassKKSecond, deltaMassKKSecond, float); //!
DECLARE_SOA_COLUMN(PT1, pT1, float);                             //!
DECLARE_SOA_COLUMN(DCAPrimXY1, dcaPrimXY1, float);               //!
DECLARE_SOA_COLUMN(DCAPrimZ1, dcaPrimZ1, float);                 //!
DECLARE_SOA_COLUMN(NsigmaPiTPC1, nsigmaPiTPC1, float);           //!
DECLARE_SOA_COLUMN(NsigmaKaTPC1, nsigmaKaTPC1, float);           //!
DECLARE_SOA_COLUMN(NsigmaPrTPC1, nsigmaPrTPC1, float);           //!
DECLARE_SOA_COLUMN(NsigmaPiTOF1, nsigmaPiTOF1, float);           //!
DECLARE_SOA_COLUMN(NsigmaKaTOF1, nsigmaKaTOF1, float);           //!
DECLARE_SOA_COLUMN(NsigmaPrTOF1, nsigmaPrTOF1, float);           //!
DECLARE_SOA_COLUMN(PT2, pT2, float);                             //!
DECLARE_SOA_COLUMN(DCAPrimXY2, dcaPrimXY2, float);               //!
DECLARE_SOA_COLUMN(DCAPrimZ2, dcaPrimZ2, float);                 //!
DECLARE_SOA_COLUMN(NsigmaPiTPC2, nsigmaPiTPC2, float);           //!
DECLARE_SOA_COLUMN(NsigmaKaTPC2, nsigmaKaTPC2, float);           //!
DECLARE_SOA_COLUMN(NsigmaPrTPC2, nsigmaPrTPC2, float);           //!
DECLARE_SOA_COLUMN(NsigmaPiTOF2, nsigmaPiTOF2, float);           //!
DECLARE_SOA_COLUMN(NsigmaKaTOF2, nsigmaKaTOF2, float);           //!
DECLARE_SOA_COLUMN(NsigmaPrTOF2, nsigmaPrTOF2, float);           //!
DECLARE_SOA_COLUMN(PT3, pT3, float);                             //!
DECLARE_SOA_COLUMN(DCAPrimXY3, dcaPrimXY3, float);               //!
DECLARE_SOA_COLUMN(DCAPrimZ3, dcaPrimZ3, float);                 //!
DECLARE_SOA_COLUMN(NsigmaPiTPC3, nsigmaPiTPC3, float);           //!
DECLARE_SOA_COLUMN(NsigmaKaTPC3, nsigmaKaTPC3, float);           //!
DECLARE_SOA_COLUMN(NsigmaPrTPC3, nsigmaPrTPC3, float);           //!
DECLARE_SOA_COLUMN(NsigmaPiTOF3, nsigmaPiTOF3, float);           //!
DECLARE_SOA_COLUMN(NsigmaKaTOF3, nsigmaKaTOF3, float);           //!
DECLARE_SOA_COLUMN(NsigmaPrTOF3, nsigmaPrTOF3, float);           //!
DECLARE_SOA_COLUMN(FlagOrigin, flagOrigin, int8_t);              //!
DECLARE_SOA_COLUMN(Channel, channel, int8_t);                    //!
DECLARE_SOA_COLUMN(HFSelBit, hfselbit, int8_t);                  //!
} // namespace hftraining

DECLARE_SOA_TABLE(HFTrigTrain2P, "AOD", "HFTRIGTRAIN2P", //!
                  hftraining::InvMassD0,
                  hftraining::InvMassD0bar,
                  hftraining::PT2Prong,
                  hftraining::PT1,
                  hftraining::DCAPrimXY1,
                  hftraining::DCAPrimZ1,
                  hftraining::NsigmaPiTPC1,
                  hftraining::NsigmaKaTPC1,
                  hftraining::NsigmaPiTOF1,
                  hftraining::NsigmaKaTOF1,
                  hftraining::PT2,
                  hftraining::DCAPrimXY2,
                  hftraining::DCAPrimZ2,
                  hftraining::NsigmaPiTPC2,
                  hftraining::NsigmaKaTPC2,
                  hftraining::NsigmaPiTOF2,
                  hftraining::NsigmaKaTOF2,
                  hftraining::FlagOrigin);
DECLARE_SOA_TABLE(HFTrigTrain3P, "AOD", "HFTRIGTRAIN3P", //!
                  hftraining::InvMassDplus,
                  hftraining::InvMassDsToKKPi,
                  hftraining::InvMassDsToPiKK,
                  hftraining::InvMassLcToPKPi,
                  hftraining::InvMassLcToPiKP,
                  hftraining::InvMassXicToPKPi,
                  hftraining::InvMassXicToPiKP,
                  hftraining::PT3Prong,
                  hftraining::DeltaMassKKFirst,
                  hftraining::DeltaMassKKSecond,
                  hftraining::PT1,
                  hftraining::DCAPrimXY1,
                  hftraining::DCAPrimZ1,
                  hftraining::NsigmaPiTPC1,
                  hftraining::NsigmaKaTPC1,
                  hftraining::NsigmaPrTPC1,
                  hftraining::NsigmaPiTOF1,
                  hftraining::NsigmaKaTOF1,
                  hftraining::NsigmaPrTOF1,
                  hftraining::PT2,
                  hftraining::DCAPrimXY2,
                  hftraining::DCAPrimZ2,
                  hftraining::NsigmaPiTPC2,
                  hftraining::NsigmaKaTPC2,
                  hftraining::NsigmaPrTPC2,
                  hftraining::NsigmaPiTOF2,
                  hftraining::NsigmaKaTOF2,
                  hftraining::NsigmaPrTOF2,
                  hftraining::PT3,
                  hftraining::DCAPrimXY3,
                  hftraining::DCAPrimZ3,
                  hftraining::NsigmaPiTPC3,
                  hftraining::NsigmaKaTPC3,
                  hftraining::NsigmaPrTPC3,
                  hftraining::NsigmaPiTOF3,
                  hftraining::NsigmaKaTOF3,
                  hftraining::NsigmaPrTOF3,
                  hftraining::FlagOrigin,
                  hftraining::Channel,
                  hftraining::HFSelBit);

namespace hfoptimisationTree
{
DECLARE_SOA_COLUMN(CollisionIndex, collisionIndex, int); //!
DECLARE_SOA_COLUMN(ParticleID, particleID, int);         //!
DECLARE_SOA_COLUMN(Pt, pt, float);                       //!
DECLARE_SOA_COLUMN(BkgBDT, bkgBDT, float);               //!
DECLARE_SOA_COLUMN(PromptBDT, promptBDT, float);         //!
DECLARE_SOA_COLUMN(NonpromptBDT, nonpromptBDT, float);   //!
DECLARE_SOA_COLUMN(DCAXY, dcaXY, float);                 //!
DECLARE_SOA_COLUMN(KStar, kStar, float);                 //!
DECLARE_SOA_COLUMN(NsigmaPrTPC, nsigmaPrTPC, float);     //!
DECLARE_SOA_COLUMN(NsigmaPrTOF, nsigmaPrTOF, float);     //!
} // namespace hfoptimisationTree

DECLARE_SOA_TABLE(HFOptimisationTreeBeauty, "AOD", "HFOPTIMTREEB", //!
                  hfoptimisationTree::CollisionIndex,
                  hfoptimisationTree::ParticleID,
                  hfoptimisationTree::Pt,
                  hfoptimisationTree::BkgBDT,
                  hfoptimisationTree::PromptBDT,
                  hfoptimisationTree::NonpromptBDT,
                  hfoptimisationTree::DCAXY);
DECLARE_SOA_TABLE(HFOptimisationTreeCharm, "AOD", "HFOPTIMTREEC", //!
                  hfoptimisationTree::CollisionIndex,
                  hfoptimisationTree::ParticleID,
                  hfoptimisationTree::Pt,
                  hfoptimisationTree::BkgBDT,
                  hfoptimisationTree::PromptBDT,
                  hfoptimisationTree::NonpromptBDT);
DECLARE_SOA_TABLE(HFOptimisationTreeFemto, "AOD", "HFOPTIMTREEF", //!
                  hfoptimisationTree::CollisionIndex,
                  hfoptimisationTree::ParticleID,
                  hfoptimisationTree::Pt,
                  hfoptimisationTree::BkgBDT,
                  hfoptimisationTree::PromptBDT,
                  hfoptimisationTree::NonpromptBDT,
                  hfoptimisationTree::KStar,
                  hfoptimisationTree::NsigmaPrTPC,
                  hfoptimisationTree::NsigmaPrTOF);
DECLARE_SOA_TABLE(HFOptimisationTreeCollisions, "AOD", "HFOPTIMTREECOLL", //!
                  hfoptimisationTree::CollisionIndex)
} // namespace o2::aod

#endif // O2_ANALYSIS_HF_FILTER_HELPERS_
