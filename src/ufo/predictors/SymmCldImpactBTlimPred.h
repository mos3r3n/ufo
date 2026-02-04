/*
 * (C) Copyright 2020 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef UFO_PREDICTORS_SYMMCLDIMPACTBTLIMPRED_H_
#define UFO_PREDICTORS_SYMMCLDIMPACTBTLIMPRED_H_

#include <string>
#include <vector>

#include "ufo/predictors/PredictorBase.h"

#include "oops/util/parameters/OptionalParameter.h"
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/RequiredParameter.h"

namespace oops {
  class ObsVariables;
}

namespace ioda {
  class ObsSpace;
}

namespace ufo {

/// Configuration parameters of the SymmCldImpactBTlimPred predictor.
class SymmCldImpactBTlimPredParameters : public PredictorParametersBase {
  OOPS_CONCRETE_PARAMETERS(SymmCldImpactBTlimPredParameters, PredictorParametersBase)

 public:
  /// Power to which to raise the Symmetric Cloud Impact. By default, 1.
  ///
  /// \note If this option is set, a suffix containing its value (even if it's equal to 1) will be
  /// appended to the predictor name.
  oops::OptionalParameter<int> order{"order", this};

  /// channels for which SCI will be calculated
  oops::RequiredParameter<std::string> chlist{"channels", this};
  oops::RequiredParameter<std::vector<float>> btlim{"btlim", this};
};

// -----------------------------------------------------------------------------

/// 
/// Symmetric Cloud Impact (SCI) parameter by Harnisch et al. (2016) used as 
/// a cloud-dependent predictor
/// 
/// This implementation follows the SymmCldImpactBTlim obsfunction
///
/// Harnisch, F., M. Weissmann, and Á. Periáñez, 2016: Error model for the assimilation
///   of cloud-affected infrared satellite observations in an ensemble data assimilation system.
///   Quart. J. Roy. Meteor. Soc., 142, 1797−1808, https://doi.org/10.1002/qj.2776.

class SymmCldImpactBTlimPred : public PredictorBase {
 public:
  /// The type of parameters accepted by the constructor of this predictor.
  /// This typedef is used by the PredictorFactory.
  typedef SymmCldImpactBTlimPredParameters Parameters_;

  SymmCldImpactBTlimPred(const Parameters_ &, const oops::ObsVariables &);

  void compute(const ioda::ObsSpace &,
               const GeoVaLs &,
               const ObsDiagnostics &,
               const ObsBias &,
               ioda::ObsVector &) const override;

 private:
  SymmCldImpactBTlimPredParameters options_;
  std::vector<int> channels_;
  int order_;
};

// -----------------------------------------------------------------------------

}  // namespace ufo

#endif  // UFO_PREDICTORS_SYMMCLDIMPACTBTLIMPRED_H_
