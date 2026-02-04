/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <string>
#include <vector>

#include "ufo/predictors/ClearBkgBrightnessTemperature.h"

#include "ioda/ObsVector.h"
#include "ioda/ObsSpace.h"

#include "ufo/ObsDiagnostics.h"

#include "oops/util/missingValues.h"

namespace ufo {

static PredictorMaker<ClearBkgBrightnessTemperature>
       makerFuncClearBkgBrightnessTemperature_("clearBkgBrightnessTemperature");

// -----------------------------------------------------------------------------

ClearBkgBrightnessTemperature::ClearBkgBrightnessTemperature(const Parameters_ & parameters,
                                                             const oops::ObsVariables & vars)
  : PredictorBase(parameters, vars) {

  if (vars.size() > 0) {
    hdiags_ += oops::ObsVariables({"brightness_temperature_assuming_clear_sky"}, vars.channels());
    hdiags_ += oops::ObsVariables({"brightness_temperature"}, vars.channels());
  } else {
    oops::Log::error() << "Channels size is ZERO !" << std::endl;
    ABORT("Channels size is ZERO !");
  }

}

// -----------------------------------------------------------------------------

void ClearBkgBrightnessTemperature::compute(const ioda::ObsSpace & odb,
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

  for (std::size_t jloc = 0; jloc < nlocs; ++jloc) {
    for (std::size_t jvar = 0; jvar < nvars; ++jvar) {
      hdiags = "brightness_temperature_assuming_clear_sky_" + std::to_string(vars_.channels()[jvar]);
      ydiags.get(clr, hdiags);
      hdiags = "brightness_temperature_" + std::to_string(vars_.channels()[jvar]);
      ydiags.get(bak, hdiags);
      // Temporarily account for ZERO clear-sky BT output from CRTM
      // TODO(JJG): change CRTM clear-sky behavior
      if (clr[jloc] > -1.0 && clr[jloc] < 1.0 || clr[jloc] == fmiss) clr[jloc] = bak[jloc];
      if (clr[jloc] != fmiss && bak[jloc] != fmiss) {
        out[jloc*nvars + jvar] = clr[jloc];
      } else {
        out[jloc*nvars + jvar] = fmiss;
      }
    }
  }

}


// -----------------------------------------------------------------------------

}  // namespace ufo
