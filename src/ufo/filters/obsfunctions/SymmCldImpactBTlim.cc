/*
 * (C) Copyright 2024 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "ufo/filters/obsfunctions/SymmCldImpactBTlim.h"

#include <algorithm>
#include <cmath>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "ioda/ObsDataVector.h"
#include "oops/util/IntSetParser.h"
#include "oops/util/Logger.h"
#include "oops/util/missingValues.h"
#include "ufo/filters/ObsFilterData.h"
#include "ufo/filters/Variable.h"
#include "ufo/utils/Constants.h"

namespace ufo {

static ObsFunctionMaker<SymmCldImpactBTlim> makerSCIIR_("SymmCldImpactBTlim");

// -----------------------------------------------------------------------------

SymmCldImpactBTlim::SymmCldImpactBTlim(const eckit::LocalConfiguration config)
  : invars_(), channels_() {
  oops::Log::debug() << "SymmCldImpactBTlim: config = " << config << std::endl;
  // Initialize options
  options_.deserialize(config);

  // Get channels from options
  std::string chlist = options_.chlist;
  std::set<int> channelset = oops::parseIntSet(chlist);
  std::copy(channelset.begin(), channelset.end(), std::back_inserter(channels_));

  // Include required variables from ObsDiag
  //invars_ += Variable("ObsDiag/brightness_temperature_assuming_clear_sky", channels_);
  invars_ += Variable("HofX/brightnessTemperature", channels_);
}

// -----------------------------------------------------------------------------

SymmCldImpactBTlim::~SymmCldImpactBTlim() {}

// -----------------------------------------------------------------------------

void SymmCldImpactBTlim::compute(const ObsFilterData & in,
                                    ioda::ObsDataVector<float> & SCI) const {
  const float missing = util::missingValue<float>();

  // Get dimensions
  size_t nlocs = in.nlocs();

  // Allocate vectors common across channels
  //std::vector<float> clr(nlocs);
  std::vector<float> bak(nlocs);
  std::vector<float> obs(nlocs);
  std::vector<float> bias(nlocs);

  const std::vector<float> btlim = options_.btlim.value();

  float Cmod, Cobs;

  for (size_t ich = 0; ich < SCI.nvars(); ++ich) {
    // Get channel-specific clr, bak, obs, and bias
    //in.get(Variable("ObsDiag/brightness_temperature_assuming_clear_sky", channels_)[ich], clr);
    in.get(Variable("HofX/brightnessTemperature", channels_)[ich], bak);
    in.get(Variable("ObsValue/brightnessTemperature", channels_)[ich], obs);
    if (in.has(Variable("ObsBiasData/brightnessTemperature", channels_)[ich])) {
      in.get(Variable("ObsBiasData/brightnessTemperature", channels_)[ich], bias);
    } else {
      std::fill(bias.begin(), bias.end(), 0.0f);
    }
    for (size_t iloc = 0; iloc < nlocs; ++iloc) {
      if (bak[iloc] != missing &&
          obs[iloc] != missing && bias[iloc] != missing) {
        // HofX contains bias correction; subtracting it here
        Cmod = std::max(0.0f, btlim[ich] - bak[iloc] );
        Cobs = std::max(0.0f, btlim[ich] - obs[iloc] + bias[iloc]);
        SCI[ich][iloc] = 0.5f * (Cmod + Cobs);
      } else {
        SCI[ich][iloc] = missing;
      }
    }
  }
}

// -----------------------------------------------------------------------------

const ufo::Variables & SymmCldImpactBTlim::requiredVariables() const {
  return invars_;
}

// -----------------------------------------------------------------------------

}  // namespace ufo
