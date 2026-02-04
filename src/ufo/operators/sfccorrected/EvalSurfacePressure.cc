/*
 * (C) Copyright 2024, UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <string>
#include <vector>

#include "ufo/operators/sfccorrected/EvalSurfacePressure.h"

#include "eckit/exception/Exceptions.h"
#include "ioda/ObsSpace.h"
#include "oops/util/Logger.h"
#include "ufo/GeoVaLs.h"
#include "ufo/utils/Constants.h"
#include "ufo/utils/VertInterp.interface.h"
#include "ufo/variabletransforms/Formulas.h"

namespace ufo {

namespace {
SurfaceOperatorMaker<stationPressure_WRFDA> makerSP_WRFDA_("stationPressure_WRFDA");
}  // namespace

// ----------------------------------------
// Pressure operator using WRFDA method
// ----------------------------------------
stationPressure_WRFDA::stationPressure_WRFDA(const std::string & name,
                                              const Parameters_ & params)
     : SurfaceOperatorBase(name, params)
{
  oops::Variables vars;
  vars.push_back(oops::Variable(params_.geovarGeomZ.value()));
  vars.push_back(oops::Variable(params_.geovarSfcGeomZ.value()));
  vars.push_back(oops::Variable("air_pressure"));
  vars.push_back(oops::Variable("virtual_temperature"));
  vars.push_back(oops::Variable("surface_pressure"));
  requiredVars_ += vars;
}

void stationPressure_WRFDA::simobs(const ufo::GeoVaLs & gv,
                                   const ioda::ObsSpace & obsdb,
                                   std::vector<float> & hofx) const {
  oops::Log::trace() << "stationPressure_WRFDA::simobs starting" << std::endl;

  // Setup parameters used throughout
  const size_t nobs = obsdb.nlocs();
  const oops::Variable geomz_var = oops::Variable(params_.geovarGeomZ.value());
  const int surface_level_index = gv.nlevs(geomz_var) - 1;
  const float missing = util::missingValue<float>();

  // Create arrays needed
  std::vector<float> model_pressure_surface(nobs), model_height_level1(nobs),
                     model_height_surface(nobs), model_virtual_T(nobs);
  std::vector<float> obs_pressure(nobs), obs_virtual_T(nobs),
                     obs_lats(nobs), obs_height(nobs);
  std::vector<float> adjusted_model_surface_virtual_T(nobs);
  std::vector<float> adjusted_station_height_virtual_T(nobs);
  std::vector<float> adjusted_model_surface_height_pressure(nobs);

  // Read other data in
  obsdb.get_db("MetaData", "stationElevation", obs_height);
  obsdb.get_db("ObsValue", "stationPressure", obs_pressure);
  obsdb.get_db("ObsValue", "virtualTemperature", obs_virtual_T);
  obsdb.get_db("MetaData", "latitude", obs_lats);

  gv.get(model_pressure_surface, oops::Variable("surface_pressure"));
  gv.getAtLevel(model_virtual_T, oops::Variable("virtual_temperature"), surface_level_index);

  // Get level 1 height.  If geopotential then convert to geometric height.
  gv.getAtLevel(model_height_level1, geomz_var, surface_level_index);
  if (params_.geovarGeomZ.value().find("geopotential") != std::string::npos) {
     oops::Log::trace()  <<
        "ObsSfcCorrected::simulateObs do geopotential conversion for model level 1"
        << std::endl;
     for (size_t iloc = 0; iloc < nobs; ++iloc) {
       model_height_level1[iloc] = formulas::Geopotential_to_Geometric_Height(obs_lats[iloc],
         model_height_level1[iloc]);
     }
  }

  // Get surface height.  If geopotential then convert to geometric height.
  gv.get(model_height_surface, oops::Variable(params_.geovarSfcGeomZ.value()));
  if (params_.geovarSfcGeomZ.value().find("geopotential") != std::string::npos) {
     oops::Log::trace()  << "ObsSfcCorrected::simulateObs do geopotential conversion surface"
                          << std::endl;
     for (size_t iloc = 0; iloc < nobs; ++iloc) {
       model_height_surface[iloc] = formulas::Geopotential_to_Geometric_Height(obs_lats[iloc],
              model_height_surface[iloc]);
     }
  }

  // Loop to calculate hofx
  for (size_t iloc = 0; iloc < nobs; ++iloc) {
    hofx[iloc] = missing;
    if (obs_height[iloc] != missing && model_pressure_surface[iloc] != missing &&
         model_height_surface[iloc] != missing) {
       // Find model surface virtual temperature
       adjusted_model_surface_virtual_T[iloc] = model_virtual_T[iloc] +
               ufo::Constants::Lclr * (model_height_level1[iloc] - model_height_surface[iloc]);
       if (obs_virtual_T[iloc] != missing) {
          adjusted_station_height_virtual_T[iloc] = 0.5 * (
               adjusted_model_surface_virtual_T[iloc] + obs_virtual_T[iloc]);
       } else {
         adjusted_station_height_virtual_T[iloc] = adjusted_model_surface_virtual_T[iloc];
       }
       // Extrapolate pressure from station height to model surface height
       if (obs_pressure[iloc] != missing) {
         float val = (model_height_surface[iloc] - obs_height[iloc]) *
             (ufo::Constants::grav /
                (ufo::Constants::rd * adjusted_station_height_virtual_T[iloc]));
         adjusted_model_surface_height_pressure[iloc] = obs_pressure[iloc] * std::exp(-val);
       } else {
         adjusted_model_surface_height_pressure[iloc] = obs_pressure[iloc];
       }
       if (adjusted_model_surface_height_pressure[iloc] != missing) {
         hofx[iloc] = obs_pressure[iloc] - adjusted_model_surface_height_pressure[iloc] +
                      model_pressure_surface[iloc];
       } else {
         hofx[iloc] = model_pressure_surface[iloc];
       }
    }
  }
  oops::Log::trace() << "stationPressure_WRFDA::simobs complete" << std::endl;
  }

void stationPressure_WRFDA::settraj() const {
  throw eckit::Exception("stationPressure_WRFDA::settraj not yet implemented");
}

void stationPressure_WRFDA::TL() const {
  throw eckit::Exception("stationPressure_WRFDA::TL not yet implemented");
}

void stationPressure_WRFDA::AD() const {
  throw eckit::Exception("stationPressure_WRFDA::AD not yet implemented");
}

}  // namespace ufo
