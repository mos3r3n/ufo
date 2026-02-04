/*
 * (C) Copyright 2024 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef UFO_FILTERS_OBSFUNCTIONS_ABSNORMALIZEDOMB_H_
#define UFO_FILTERS_OBSFUNCTIONS_ABSNORMALIZEDOMB_H_

#include <string>
#include <vector>

#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/Parameters.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "ufo/filters/obsfunctions/ObsFunctionBase.h"
#include "ufo/filters/Variables.h"

namespace ufo {

// -----------------------------------------------------------------------------

/// \brief Options controlling AbsNormalizedOMB
class AbsNormalizedOMBParameters : public oops::Parameters {
  OOPS_CONCRETE_PARAMETERS(AbsNormalizedOMBParameters, Parameters)

 public:
  /// channels for which AbsNormalizedOMB will be calculated
  oops::RequiredParameter<std::string> chlist{"channels", this};
  oops::RequiredParameter<std::vector<float>> btlim{"btlim", this};
};

// -----------------------------------------------------------------------------

/// \brief Absolute value of the normalized OMB
///
/// Calculate the absolute value of the normalized OMB by the dynamical obs error
///   for IR observations
///
class AbsNormalizedOMB : public ObsFunctionBase<float> {
 public:
  explicit AbsNormalizedOMB(const eckit::LocalConfiguration);
  ~AbsNormalizedOMB();

  void compute(const ObsFilterData &,
               ioda::ObsDataVector<float> &) const;
  const ufo::Variables & requiredVariables() const;
 private:
  AbsNormalizedOMBParameters options_;
  ufo::Variables invars_;
  std::vector<int> channels_;
};

// -----------------------------------------------------------------------------

}  // namespace ufo

#endif  // UFO_FILTERS_OBSFUNCTIONS_ABSNORMALIZEDOMB_H_
