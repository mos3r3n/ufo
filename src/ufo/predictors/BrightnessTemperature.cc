/*
 * (C) Copyright 2020 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "ufo/predictors/BrightnessTemperature.h"

#include <vector>

#include "ioda/ObsVector.h"
#include "ioda/ObsSpace.h"

namespace ufo {

static PredictorMaker<BrightnessTemperature>
       makerFuncBrightnessTemperature_("brightnessTemperature");

// -----------------------------------------------------------------------------
BrightnessTemperature::BrightnessTemperature(const Parameters_ & parameters, const oops::ObsVariables & vars)
  : PredictorBase(parameters, vars),
    order_(parameters.order.value().value_or(1))
{
  if (parameters.order.value() != boost::none) {
    // override the predictor name to distinguish between brightnessTemperature predictors of different orders
    name() = name() + "_order_" + std::to_string(order_);
  }
}

// -----------------------------------------------------------------------------

void BrightnessTemperature::compute(const ioda::ObsSpace & odb,
                                    const GeoVaLs &,
                                    const ObsDiagnostics & ydiags,
                                    const ObsBias &,
                                    ioda::ObsVector & out) const {
  const std::size_t nlocs = out.nlocs();
  const std::size_t nvars = out.nvars();

  // retrieve the sensor view angle
  std::vector<float> btemp(nlocs, 0.0);

  for (std::size_t jloc = 0; jloc < nlocs; ++jloc) {
    for (std::size_t jvar = 0; jvar < nvars; ++jvar) {
      odb.get_db("ObsValue", "brightnessTemperature", btemp, {vars_.channels()[jvar]});
      out[jloc*nvars+jvar] = btemp[jloc];
    }
  }

}


// -----------------------------------------------------------------------------

}  // namespace ufo
