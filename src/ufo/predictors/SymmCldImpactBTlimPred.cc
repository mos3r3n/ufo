/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <sstream>

#include "ufo/predictors/SymmCldImpactBTlimPred.h"

#include "ioda/ObsVector.h"
#include "ioda/ObsSpace.h"

#include "ufo/ObsDiagnostics.h"

#include "oops/util/missingValues.h"

#include "ioda/ObsDataVector.h"
#include "oops/util/IntSetParser.h"
#include "oops/util/Logger.h"
#include "ufo/filters/Variable.h"
#include "ufo/utils/Constants.h"

namespace ufo {

static PredictorMaker<SymmCldImpactBTlimPred>
       makerFuncSymmCldImpactBTlimPred_("symmetricCloudImpactBTlimPred");

// -----------------------------------------------------------------------------

SymmCldImpactBTlimPred::SymmCldImpactBTlimPred(const Parameters_ & parameters,
                                         const oops::ObsVariables & vars)
  : PredictorBase(parameters, vars),
    order_(parameters.order.value().value_or(1))
{
  if (parameters.order.value() != boost::none) {
    // override the predictor name to distinguish between symmetricCloudImpactIR predictors of different orders
    name() = name() + "_order_" + std::to_string(order_);
  }

  // Initialize options
  options_ = parameters;

  // Get channels from options
  std::string chlist = options_.chlist;
  std::set<int> channelset = oops::parseIntSet(chlist);
  std::copy(channelset.begin(), channelset.end(), std::back_inserter(channels_));

  if (vars.size() > 0) {
      if (vars.channels() == channels_) {
        hdiags_ += oops::ObsVariables({"brightness_temperature"}, channels_);
      } else {
        oops::Log::error() << "Channels list is different than the channels list provided for BTlim !" << std::endl;
        ABORT("Channels list and channels list in BTlim are different !");
      }
  } else {
    oops::Log::error() << "Channels size is ZERO !" << std::endl;
    ABORT("Channels size is ZERO !");
  }
}

// -----------------------------------------------------------------------------

void SymmCldImpactBTlimPred::compute(const ioda::ObsSpace & odb,
                                  const GeoVaLs &,
                                  const ObsDiagnostics & ydiags,
                                  const ObsBias &,
                                  ioda::ObsVector & out) const {
  // Get required parameters
  const std::size_t nlocs = out.nlocs();
  const std::size_t nvars = out.nvars();

  const float fmiss = util::missingValue<float>();

  std::string hdiags;
  std::vector<float> bias(nlocs,0.0);
  std::vector<float> bak(nlocs,0.0);
  std::vector<float> obs(nlocs,0.0);
  float Cmod, Cobs;
  std::vector<float> scivalue(nlocs,0.0);

  const std::vector<float> btlim = options_.btlim.value();

  for (std::size_t jloc = 0; jloc < nlocs; ++jloc) {
    for (std::size_t jvar = 0; jvar < nvars; ++jvar) {
      hdiags = "brightness_temperature_" + std::to_string(channels_[jvar]);
      ydiags.get(bak, hdiags);
      odb.get_db("ObsValue", "brightnessTemperature", obs, {channels_[jvar]});
      if (odb.has("ObsBiasData", "brightnessTemperature")) {
        odb.get_db("ObsBiasData", "brightnessTemperature", bias, {channels_[jvar]});
      } else {
        std::fill(bias.begin(), bias.end(), 0.0f);
      }
      if (bak[jloc] != fmiss && obs[jloc] != fmiss && bias[jloc] != fmiss) {
        Cmod = std::max(0.0f, btlim[jvar] - bak[jloc] );
        Cobs = std::max(0.0f, btlim[jvar] - obs[jloc] + bias[jloc]);
        scivalue[jloc] = 0.5f * (Cmod + Cobs);
        out[jloc*nvars + jvar] = pow(scivalue[jloc], order_);
      } else {
        out[jloc*nvars + jvar] = fmiss;
      }
    }
  }

}


// -----------------------------------------------------------------------------

}  // namespace ufo
