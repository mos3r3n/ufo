/*
 * (C) Copyright 2020 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "ufo/predictors/ReciprocalCosineZenithAngle.h"

#include <vector>

#include "ioda/ObsSpace.h"
#include "ioda/ObsVector.h"

#include "ufo/utils/Constants.h"

namespace ufo {

static PredictorMaker<ReciprocalCosineZenithAngle>
       makerFuncReciprocalCosineZenithAngle_("reciprocalCosineZenithAngle");

// -----------------------------------------------------------------------------

ReciprocalCosineZenithAngle::ReciprocalCosineZenithAngle(const Parameters_ & parameters,
                                                         const oops::ObsVariables & vars)
  : PredictorBase(parameters, vars) {
}

// -----------------------------------------------------------------------------

void ReciprocalCosineZenithAngle::compute(const ioda::ObsSpace & odb,
                                          const GeoVaLs &,
                                          const ObsDiagnostics &,
                                          const ObsBias &,
                                          ioda::ObsVector & out) const {
  const std::size_t nlocs = out.nlocs();
  const std::size_t nvars = out.nvars();

  // retrieve the sensor view angle
  std::vector<float> zangle(nlocs, 0.0);
  odb.get_db("MetaData", "sensorZenithAngle", zangle);

  for (std::size_t jloc = 0; jloc < nlocs; ++jloc) {
    for (std::size_t jvar = 0; jvar < nvars; ++jvar) {
      out[jloc*nvars+jvar] = 1.0 / cos(zangle[jloc] * Constants::deg2rad);
    }
  }
}

// -----------------------------------------------------------------------------

}  // namespace ufo
