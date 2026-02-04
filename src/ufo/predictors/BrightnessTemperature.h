/*
 * (C) Copyright 2020 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef UFO_PREDICTORS_BRIGHTNESSTEMPERATURE_H_
#define UFO_PREDICTORS_BRIGHTNESSTEMPERATURE_H_

#include "oops/util/parameters/OptionalParameter.h"
#include "oops/util/parameters/Parameter.h"

#include "ufo/predictors/PredictorBase.h"

namespace oops {
  class ObsVariables;
}

namespace ioda {
  class ObsSpace;
}

namespace ufo {

/// Configuration parameters of the BrightnessTemperature predictor.
class BrightnessTemperatureParameters : public PredictorParametersBase {
  OOPS_CONCRETE_PARAMETERS(BrightnessTemperatureParameters, PredictorParametersBase);

 public:
  /// Power to which to raise the observed brightness temperature. By default, 1.
  ///
  /// \note If this option is set, a suffix containing its value (even if it's equal to 1) will be
  /// appended to the predictor name.
  oops::OptionalParameter<int> order{"order", this};
};

// -----------------------------------------------------------------------------

class BrightnessTemperature : public PredictorBase {
 public:
  /// The type of parameters accepted by the constructor of this predictor.
  /// This typedef is used by the PredictorFactory.
  typedef BrightnessTemperatureParameters Parameters_;

  BrightnessTemperature(const Parameters_ &, const oops::ObsVariables &);

  void compute(const ioda::ObsSpace &,
               const GeoVaLs &,
               const ObsDiagnostics &,
               const ObsBias &,
               ioda::ObsVector &) const override;

 private:
  int order_;
};

// -----------------------------------------------------------------------------

}  // namespace ufo

#endif  // UFO_PREDICTORS_BRIGHTNESSTEMPERATURE_H_
