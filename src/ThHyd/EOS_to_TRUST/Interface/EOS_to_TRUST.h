/****************************************************************************
* Copyright (c) 2023, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

#ifndef EOS_to_TRUST_included
#define EOS_to_TRUST_included

#include <eos++.h>

#ifdef HAS_EOS
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#include <EOS/API/EOS_Std_Error_Handler.hxx>
#include <EOS/API/EOS.hxx>
#pragma GCC diagnostic pop
#endif

#include <Motcle.h>
#include <span.hpp>
#include <vector>
#include <map>

using MSpanD = std::map<std::string, tcb::span<double>>;
using ArrayD = std::array<double,1>;
using VectorD = std::vector<double>;
using SpanD = tcb::span<double>;

namespace NEPTUNE { class EOS; }

class EOS_to_TRUST
{
public :
  EOS_to_TRUST();
  virtual ~EOS_to_TRUST();

  void verify_model_fluid(Motcle& model_name, Motcle& fluid_name);

  // pour les fluides
  // en temperature
  virtual inline void eos_get_rho_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_rho_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_rho_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_h_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_h_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_h_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_cp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_beta_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_mu_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_lambda_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }

  virtual inline void eos_get_cp_mu_lambda_beta_pT(const SpanD P, const SpanD T, MSpanD prop, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_all_pT(MSpanD inter, MSpanD bord, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_all_prop_loi_F5(MSpanD , int ncomp = 1, int id = 0, bool is_liq = true) const { return not_implemented<void>(__func__); }

  // en enthalpie
  virtual inline void eos_get_rho_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_rho_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_rho_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_T_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_T_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_T_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_cp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_cp_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_cp_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_mu_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_mu_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_mu_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_lambda_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_lambda_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_lambda_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }

  // pour la saturation
  virtual inline void eos_get_T_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_d_T_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_p_sat_T(const SpanD T, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_d_p_sat_d_T_T(const SpanD T, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_lvap_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_d_lvap_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_h_l_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_d_h_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_h_v_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_d_h_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_rho_l_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_d_rho_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_rho_v_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_d_rho_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_cp_l_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_d_cp_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_cp_v_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_d_cp_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_lambda_l_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_d_lambda_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_lambda_v_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_d_lambda_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_mu_l_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_d_mu_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_mu_v_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_d_mu_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<void>(__func__); }

  virtual inline void eos_get_all_flux_interfacial(MSpanD sats, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_all_loi_F5(MSpanD sats, int ncomp = 1, int id = 0, bool is_liq = true) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_hv_drhov_loi_F5(MSpanD sats, int ncomp = 1, int id = 0, bool is_liq = true) const { return not_implemented<void>(__func__); }

  virtual inline void eos_get_sigma_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_sigma_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_d_sigma_d_T_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_d_sigma_d_p_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual inline void eos_get_d_sigma_d_h_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }

protected :
  void compute_eos_field(const SpanD P, SpanD res, const char *const pt, const char *const pn, bool is_T = false) const;
  void compute_eos_field(const SpanD P, const SpanD T, SpanD res, const char *const pt, const char *const pn) const;
  void compute_eos_field_h(const SpanD P, const SpanD H, SpanD res, const char *const pt, const char *const pn) const;

#ifdef HAS_EOS
  NEPTUNE::EOS_Std_Error_Handler handler ;
  NEPTUNE::EOS *fluide;
#endif

  template <typename TYPE>
  TYPE not_implemented(const char * nom_funct) const
  {
    cerr << "EOS_to_TRUST::" << nom_funct << " should be implemented in a derived class !" << endl;
    throw;
  }

private:
  struct Supported
  {
    const std::array<Motcle, 3> AVAIL_MODELS = { { "CATHARE2", "REFPROP10", "FLICA4" } };

    const std::array<const char *const, 3> EOS_MODELS = { { "EOS_Cathare2", "EOS_Refprop10", "EOS_FLICA4" } };

    const std::array<Motcle, 18> AVAIL_FLUIDS = { { "WATERLIQUID", "WATERVAPOR",
                                                  "ARGONINCONDENSABLE", "NITROGENINCONDENSABLE",
                                                  "SODIUMLIQUID", "SODIUMVAPOR",
                                                  "IAPWSLIQUID", "IAPWSVAPOR",
                                                  "NITROGENLIQUID", "NITROGENVAPOR",
                                                  "AIRLIQUID", "AIRVAPOR",
                                                  "HELIUMLIQUID", "HELIUMVAPOR",
                                                  "R12LIQUID", "R12VAPOR",
                                                  "WATERLIQUID_F4", "WATERVAPOR_F4"
                                                } }; /* Majuscule car Motcle */

    const std::array<const char *const, 18> EOS_FLUIDS = { { "WaterLiquid", "WaterVapor",
                                                           "ARGONINCONDENSABLE", "NITROGENINCONDENSABLE",
                                                           "SodiumLiquid", "SodiumVapor",
                                                           "IAPWSLiquid", "IAPWSVapor",
                                                           "NitrogenLiquid", "NitrogenVapor",
                                                           "AirLiquid", "AirVapor",
                                                           "HeliumLiquid", "HeliumVapor",
                                                           "R12Liquid", "R12Vapor",
                                                           "WaterLiquid.140-210", "WaterVapor.140-210"
                                                          } };
  };

public:
  Supported supp;
};

// convert Temp span to kelvin ...
inline SpanD Tk_(const SpanD& T)
{
  for (auto& itr : T) itr += 273.15;
  return T;
}

// convert Temp span to Celsius ...
inline SpanD Tc_(const SpanD& T)
{
  for (auto& itr : T) itr -= 273.15;
  return T;
}

#endif /* EOS_to_TRUST_included */
