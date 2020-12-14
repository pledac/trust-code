/****************************************************************************
* Copyright (c) 2020, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Op_Diff_CoviMAC_Elem.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////
#include <Op_Diff_CoviMAC_Elem.h>
#include <Pb_Multiphase.h>
#include <Schema_Temps_base.h>
#include <Zone_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <IntLists.h>
#include <DoubleLists.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Neumann_paroi.h>
#include <Echange_contact_CoviMAC.h>
#include <Echange_externe_impose.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Champ_P0_CoviMAC.h>
#include <Champ_front_calc.h>
#include <Modele_turbulence_scal_base.h>
#include <Synonyme_info.h>
#include <communications.h>
#include <MD_Vector_base.h>
#include <cmath>

Implemente_instanciable_sans_constructeur( Op_Diff_CoviMAC_Elem          , "Op_Diff_CoviMAC_Elem|Op_Diff_CoviMAC_var_Elem"                                , Op_Diff_CoviMAC_base ) ;
Implemente_instanciable( Op_Dift_CoviMAC_Elem          , "Op_Dift_CoviMAC_P0_CoviMAC|Op_Dift_CoviMAC_var_P0_CoviMAC"                    , Op_Diff_CoviMAC_Elem ) ;
Implemente_instanciable( Op_Diff_Nonlinear_CoviMAC_Elem, "Op_Diff_nonlinear_CoviMAC_Elem|Op_Diff_nonlinear_CoviMAC_var_Elem"            , Op_Diff_CoviMAC_Elem ) ;
Implemente_instanciable( Op_Dift_Nonlinear_CoviMAC_Elem, "Op_Dift_CoviMAC_P0_CoviMAC_nonlinear|Op_Dift_CoviMAC_var_P0_CoviMAC_nonlinear", Op_Diff_CoviMAC_Elem ) ;

Op_Diff_CoviMAC_Elem::Op_Diff_CoviMAC_Elem()
{
  declare_support_masse_volumique(1);
}

Sortie& Op_Diff_CoviMAC_Elem::printOn( Sortie& os ) const
{
  return Op_Diff_CoviMAC_base::printOn( os );
}

Sortie& Op_Dift_CoviMAC_Elem::printOn( Sortie& os ) const
{
  return Op_Diff_CoviMAC_base::printOn( os );
}

Sortie& Op_Diff_Nonlinear_CoviMAC_Elem::printOn( Sortie& os ) const
{
  return Op_Diff_CoviMAC_base::printOn( os );
}

Sortie& Op_Dift_Nonlinear_CoviMAC_Elem::printOn( Sortie& os ) const
{
  return Op_Diff_CoviMAC_base::printOn( os );
}

Entree& Op_Diff_CoviMAC_Elem::readOn( Entree& is )
{
  return Op_Diff_CoviMAC_base::readOn( is );
}

Entree& Op_Diff_Nonlinear_CoviMAC_Elem::readOn( Entree& is )
{
  return Op_Diff_CoviMAC_base::readOn( is );
}

Entree& Op_Dift_CoviMAC_Elem::readOn( Entree& is )
{
  return Op_Diff_CoviMAC_base::readOn( is );
}

Entree& Op_Dift_Nonlinear_CoviMAC_Elem::readOn( Entree& is )
{
  return Op_Diff_CoviMAC_base::readOn( is );
}

void Op_Diff_CoviMAC_Elem::completer()
{
  Op_Diff_CoviMAC_base::completer();
  const Equation_base& eq = equation();
  const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, eq.inconnue().valeur());
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 1)
    Cerr << "Op_Diff_CoviMAC_Elem : largeur de joint insuffisante (minimum 1)!" << finl, Process::exit();
  ch.init_cl();
  flux_bords_.resize(zone.premiere_face_int(), ch.valeurs().line_size());

  const RefObjU& modele_turbulence = eq.get_modele(TURBULENCE);
  if (modele_turbulence.non_nul())
    {
      const Modele_turbulence_scal_base& mod_turb = ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur());
      const Champ_Fonc& lambda_t = mod_turb.conductivite_turbulente();
      associer_diffusivite_turbulente(lambda_t);
    }
}

void Op_Diff_CoviMAC_Elem::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const std::string nom_inco = equation().inconnue().le_nom().getString();
  Matrice_Morse *mat = matrices.count(nom_inco) && !semi_impl.count(nom_inco) ? matrices.at(nom_inco) : NULL, mat2;
  if (!mat) return; //rien a faire si pas de bloc diagonal ou si semi-implicite
  const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins();
  int i, j, e, eb, f, n, N = ch.valeurs().line_size(), ne_tot = zone.nb_elem_tot();

  IntTrav stencil(0, 2), tpfa(0, N);
  stencil.set_smart_resize(1), zone.creer_tableau_faces(tpfa), tpfa = 1;

  update_nu();
  /* stencils du flux : ceux (reduits) de update_nu si nu constant ou scalaire, ceux (complets) de la zone sinon */
  const IntTab& feb_d = nu_constant_ ? phif_d : zone.feb_d, &feb_j = nu_constant_ ? phif_j : zone.feb_j;
  Cerr << "Op_Diff_CoviMAC_Elem::dimensionner() : ";

  /* flux a deux points aux faces de bord (sauf Neumann )*/
  for (f = 0; f < zone.premiere_face_int(); f++) if (ch.fcl(f, 0) != 4 && ch.fcl(f, 0) != 5)
      for (n = 0, e = f_e(f, 0); n < N; n++) stencil.append_line(N * e + n, N * e + n);

  /* nu grad T aux faces internes */
  for (f = zone.premiere_face_int(); f < zone.nb_faces(); f++) for (i = 0; i < 2; i++) if ((e = f_e(f, i)) < zone.nb_elem())
        for (j = feb_d(f); j < feb_d(f + 1); j++) for (eb = feb_j(j), n = 0; n < N; n++)
            stencil.append_line(N * e + n, N * (eb < ne_tot ? eb : f_e(eb - ne_tot, 0)) + n), tpfa(f, n) &= (j < zone.feb_d(f) + 2);

  tableau_trier_retirer_doublons(stencil);
  Cerr << "width " << Process::mp_sum(stencil.dimension(0)) * 1. / (N * zone.zone().md_vector_elements().valeur().nb_items_seq_tot())
       << " " << mp_somme_vect(tpfa) * 100. / (N * zone.md_vector_faces().valeur().nb_items_seq_tot()) << "% TPFA " << finl;
  Matrix_tools::allocate_morse_matrix(N * zone.nb_elem_tot(), N * zone.nb_elem_tot(), stencil, mat2);
  mat->nb_colonnes() ? *mat += mat2 : *mat = mat2;
}

void Op_Diff_CoviMAC_Elem::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  Matrice_Morse *mat = matrices.count(nom_inco) && !semi_impl.count(nom_inco) ? matrices[nom_inco] : NULL;
  update_nu();
  const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const ArrOfInt& i_bord = zone.ind_faces_virt_bord();
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();
  const IntTab& f_e = zone.face_voisins();
  const DoubleVect& fs = zone.face_surfaces();
  const DoubleTab& nf = zone.face_normales(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe() : NULL,
                    &inco = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : equation().inconnue().valeurs();
  int i, e, f, fb, n, N = inco.line_size(), ne_tot = zone.nb_elem_tot(), D = dimension;
  double t = equation().schema_temps().temps_courant();

  /* faces de bord : flux a deux points + valeurs aux bord */
  DoubleTrav Tb(zone.nb_faces_tot(), N, 2), h_int(N), phi(N, 2); //Tb(f,n,0) -> temp de bord, Tb(f, n, 1) -> sa derivee en T(f_e(f,0),n)
  for (f = 0; f < zone.nb_faces_tot(); f++) if (ch.fcl(f, 0)) //faces de bord seulement
      {
        /* h_int : coefficent d'echange element-face */
        for (e = f_e(f, 0), fb = f < zone.nb_faces() ? f : i_bord[f - zone.nb_faces()], n = 0; n < N; n++)
          h_int(n) = zone.nu_dot(&nu_bord_, fb, n, N, &nf(f, 0), &nf(f, 0)) / (zone.dist_norm_bord(f) * fs(f) * fs(f));

        /* phi / Tb : selon CLs */
        if (ch.fcl(f, 0) < 3) for (n = 0; n < N; n++) //Echange_impose_base
            {
              double invh = (alp ? 1. / alp->addr()[N * e + n] : 1.) / ref_cast(Echange_impose_base, cls[ch.fcl(f, 1)].valeur()).h_imp(ch.fcl(f, 2), n);
              if (ch.fcl(f, 0) == 1) invh += 1. / h_int(n);
              phi(n, 0) = (inco.addr()[N * e + n] - ref_cast(Echange_impose_base, cls[ch.fcl(f, 1)].valeur()).T_ext(ch.fcl(f, 2), n)) / invh, phi(n, 1) = 1. / invh;
              Tb(f, n, 0) = inco.addr()[N * e + n] - phi(n) / h_int(n), Tb(f, n, 1) = 1;
            }
        else if (ch.fcl(f, 0) == 3) abort(); //monolithique
        else if (ch.fcl(f, 0) < 6) for (n = 0; n < N; n++) //Neumann
            {
              if (sub_type(Neumann_homogene, cls[ch.fcl(f, 1)].valeur())) phi(n, 0) = ref_cast(Neumann_homogene, cls[ch.fcl(f, 1)].valeur()).flux_impose(ch.fcl(f, 2), n);
              else if (sub_type(Neumann, cls[ch.fcl(f, 1)].valeur())) phi(n, 0) = ref_cast(Neumann, cls[ch.fcl(f, 1)].valeur()).flux_impose(ch.fcl(f, 2), n);
              else phi(n, 0) = 0.;
              phi(n, 1) = 0;
              Tb(f, n, 0) = inco.addr()[N * e + n] - phi(n, 0) / h_int(n), Tb(f, n, 1) = 1;
            }
        else for (n = 0; n < N; n++) //Dirichlet
            {
              const Champ_front_base& cfb = ref_cast(Dirichlet, cls[ch.fcl(f, 1)].valeur()).champ_front().valeur();
              const Champ_front_var_instationnaire *cfv = sub_type(Champ_front_var_instationnaire, cfb) ? &ref_cast(Champ_front_var_instationnaire, cfb) : NULL;
              double Tf = Tb(f, n, 0) = !cfv || !cfv->valeur_au_temps_et_au_point_disponible() ? ref_cast(Dirichlet, cls[ch.fcl(f, 1)].valeur()).val_imp(ch.fcl(f, 2), n) :
                                        cfv->valeur_au_temps_et_au_point(t, -1, phif_xb(f, n, 0), phif_xb(f, n, 1), D < 3 ? 0 : phif_xb(f, n, 2), n);
              phi(n, 0) = h_int(n) * (inco.addr()[N * e + n] - Tf), phi(n, 1) = h_int(n);
            }

        /* contributions */
        for (n = 0; n < N; n++) secmem.addr()[N * e + n] -= fs(f) * phi(n, 0);
        if (mat) for (n = 0; n < N; n++) (*mat)(N * e + n, N * e + n) += fs(f) * phi(n, 1);
        if (f < zone.premiere_face_int()) for (n = 0; n < N; n++) flux_bords_(f, n) = - fs(f) * phi(n, 0);
      }

  /* faces internes : interpolation -> flux amont/aval -> combinaison convexe */
  std::vector<std::map<int, double>> dphi(N); //dphi[n][idx] : derivee de la composante n du flus par rapport a idx
  for (f = zone.premiere_face_int(), phi.resize(N); f < zone.nb_faces(); f++)
    {
      /* phi = - |f|nu.grad T */
      for (phi = 0, i = phif_d(f); i < phif_d(f + 1); i++) for (e = phif_j(i), n = 0; n < N; n++)
          {
            double fac = fs(f) * (phif_w(f, n, 0) * phif_c(i, n, 0) + phif_w(f, n, 1) * phif_c(i, n, 1));
            phi(n) += fac * (e < ne_tot ? inco.addr()[N * e + n] : Tb(e - ne_tot, n, 0));
            if (mat && (e < ne_tot || Tb(e - ne_tot, n, 1))) dphi[n][e < ne_tot ? e : f_e(e - ne_tot, 0)] += fac * (e < ne_tot ? 1 : Tb(e - ne_tot, n, 1));
          }

      /* contributions */
      for (i = 0; i < 2; i++) for (n = 0; n < N; n++) secmem.addr()[N * f_e(f, i) + n] -= (i ? 1 : -1) * phi(n);
      if (mat) for (n = 0; n < N; dphi[n].clear(), n++) for (i = 0; i < 2; i++) if ((e = f_e(f, i)) < zone.nb_elem()) for (auto &i_c : dphi[n])
                (*mat)(N * e + n, N * i_c.first + n) += (i ? 1 : -1) * i_c.second;
    }
}
