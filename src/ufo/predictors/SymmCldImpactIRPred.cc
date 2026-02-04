/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <string>
#include <vector>

#include "ufo/predictors/SymmCldImpactIRPred.h"

#include "ioda/ObsVector.h"
#include "ioda/ObsSpace.h"

#include "ufo/ObsDiagnostics.h"

#include "oops/util/missingValues.h"

namespace ufo {

static PredictorMaker<SymmCldImpactIRPred>
       makerFuncSymmCldImpactIRPred_("symmetricCloudImpactIR");

// -----------------------------------------------------------------------------

SymmCldImpactIRPred::SymmCldImpactIRPred(const Parameters_ & parameters,
                                         const oops::ObsVariables & vars)
  : PredictorBase(parameters, vars),
    order_(parameters.order.value().value_or(1))
{
  if (parameters.order.value() != boost::none) {
    // override the predictor name to distinguish between symmetricCloudImpactIR predictors of different orders
    name() = name() + "_order_" + std::to_string(order_);
  }

  if (vars.size() > 0) {
    hdiags_ += oops::ObsVariables({"brightness_temperature_assuming_clear_sky"}, vars.channels());
    hdiags_ += oops::ObsVariables({"brightness_temperature"}, vars.channels());
  } else {
    oops::Log::error() << "Channels size is ZERO !" << std::endl;
    ABORT("Channels size is ZERO !");
  }
}

// -----------------------------------------------------------------------------

void SymmCldImpactIRPred::compute(const ioda::ObsSpace & odb,
                                  const GeoVaLs &,
                                  const ObsDiagnostics & ydiags,
                                  const ObsBias &,
                                  ioda::ObsVector & out) const {
  // Get required parameters
  const std::size_t nlocs = out.nlocs();
  const std::size_t nvars = out.nvars();

  const float fmiss = util::missingValue<float>();

  std::string hdiags;
  std::vector<float> clr(nlocs,0.0);
  std::vector<float> bak(nlocs,0.0);
  std::vector<float> obs(nlocs,0.0);
  float Cmod, Cobs;
  std::vector<float> scivalue(nlocs,0.0);

  for (std::size_t jloc = 0; jloc < nlocs; ++jloc) {
    for (std::size_t jvar = 0; jvar < nvars; ++jvar) {
      hdiags = "brightness_temperature_assuming_clear_sky_" + std::to_string(vars_.channels()[jvar]);
      ydiags.get(clr, hdiags);
      hdiags = "brightness_temperature_" + std::to_string(vars_.channels()[jvar]);
      ydiags.get(bak, hdiags);
      odb.get_db("ObsValue", "brightnessTemperature", obs, {vars_.channels()[jvar]});
      // Temporarily account for ZERO clear-sky BT output from CRTM
      // TODO(JJG): change CRTM clear-sky behavior
      if (clr[jloc] > -1.0 && clr[jloc] < 1.0 || clr[jloc] == fmiss) clr[jloc] = bak[jloc];
      if (clr[jloc] != fmiss && bak[jloc] != fmiss && obs[jloc] != fmiss) {
        Cmod = abs(bak[jloc] - clr[jloc]);
        Cobs = abs(obs[jloc] - clr[jloc]);
        scivalue[jloc] = 0.5 * (Cmod + Cobs);
        out[jloc*nvars + jvar] = pow(scivalue[jloc], order_);
      } else {
        out[jloc*nvars + jvar] = fmiss;
      }
    }
  }

}


// -----------------------------------------------------------------------------

}  // namespace ufo
