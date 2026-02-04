/*
 * (C) Copyright 2020 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef UFO_PREDICTORS_SYMMCLDIMPACTIRPRED_H_
#define UFO_PREDICTORS_SYMMCLDIMPACTIRPRED_H_

#include "ufo/predictors/PredictorBase.h"

#include "oops/util/parameters/OptionalParameter.h"
#include "oops/util/parameters/Parameter.h"

namespace oops {
  class ObsVariables;
}

namespace ioda {
  class ObsSpace;
}

namespace ufo {

/// Configuration parameters of the SymmCldImpactIRPred predictor.
class SymmCldImpactIRPredParameters : public PredictorParametersBase {
  OOPS_CONCRETE_PARAMETERS(SymmCldImpactIRPredParameters, PredictorParametersBase)

 public:
  /// Power to which to raise the Symmetric Cloud Impact. By default, 1.
  ///
  /// \note If this option is set, a suffix containing its value (even if it's equal to 1) will be
  /// appended to the predictor name.
  oops::OptionalParameter<int> order{"order", this};
};

// -----------------------------------------------------------------------------

/// 
/// Symmetric Cloud Impact (SCI) parameter by Okamoto et al (2014) used as 
/// a cloud-dependent predictor
/// 
/// This implementation follows the existing SymmCldImpactIR obsfunction
///
/// Okamoto, K., McNally, A.P. and Bell, W. (2014), Progress towards the
///   assimilation of all‐sky infrared radiances: an evaluation of cloud
///   effects. Q.J.R. Meteorol. Soc., 140: 1603-1614. doi:10.1002/qj.2242

class SymmCldImpactIRPred : public PredictorBase {
 public:
  /// The type of parameters accepted by the constructor of this predictor.
  /// This typedef is used by the PredictorFactory.
  typedef SymmCldImpactIRPredParameters Parameters_;

  SymmCldImpactIRPred(const Parameters_ &, const oops::ObsVariables &);

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

#endif  // UFO_PREDICTORS_SYMMCLDIMPACTIRPRED_H_
