/*
 * (C) Copyright 2024 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef UFO_FILTERS_OBSFUNCTIONS_SYMMCLDIMPACTBTLIM_H_
#define UFO_FILTERS_OBSFUNCTIONS_SYMMCLDIMPACTBTLIM_H_

#include <string>
#include <vector>

#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/Parameters.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "ufo/filters/obsfunctions/ObsFunctionBase.h"
#include "ufo/filters/Variables.h"

namespace ufo {

// -----------------------------------------------------------------------------

/// \brief Options controlling Symmetric Cloud Impact for IR instruments
class SymmCldImpactBTlimParameters : public oops::Parameters {
  OOPS_CONCRETE_PARAMETERS(SymmCldImpactBTlimParameters, Parameters)

 public:
  /// channels for which SCI will be calculated
  oops::RequiredParameter<std::string> chlist{"channels", this};
  oops::RequiredParameter<std::vector<float>> btlim{"btlim", this};
};

// -----------------------------------------------------------------------------

/// \brief Harnisch et al. symmetric cloud impact (SCI) function
///
/// Harnisch, F., M. Weissmann, and Á. Periáñez, 2016: Error model for the assimilation
///   of cloud-affected infrared satellite observations in an ensemble data assimilation system.
///   Quart. J. Roy. Meteor. Soc., 142, 1797−1808, https://doi.org/10.1002/qj.2776.
///
class SymmCldImpactBTlim : public ObsFunctionBase<float> {
 public:
  explicit SymmCldImpactBTlim(const eckit::LocalConfiguration);
  ~SymmCldImpactBTlim();

  void compute(const ObsFilterData &,
               ioda::ObsDataVector<float> &) const;
  const ufo::Variables & requiredVariables() const;
 private:
  SymmCldImpactBTlimParameters options_;
  ufo::Variables invars_;
  std::vector<int> channels_;
};

// -----------------------------------------------------------------------------

}  // namespace ufo

#endif  // UFO_FILTERS_OBSFUNCTIONS_SYMMCLDIMPACTBTLIM_H_
