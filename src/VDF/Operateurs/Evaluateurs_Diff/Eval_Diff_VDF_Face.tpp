/****************************************************************************
* Copyright (c) 2022, CEA
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
// File:        Eval_Diff_VDF_Face.tpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Diff
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Diff_VDF_Face_TPP_included
#define Eval_Diff_VDF_Face_TPP_included

/* ************************************** *
 * *********  POUR L'EXPLICITE ********** *
 * ************************************** */

template <typename DERIVED_T> template<Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
Eval_Diff_VDF_Face<DERIVED_T>::flux_fa7(const DoubleTab& inco, int elem, int fac1, int fac2, DoubleVect& flux) const
{
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);
  if (is_SCALAIRE)
    {
      const int ori=orientation(fac1);
      const double dist = dist_face(fac1,fac2,ori), tau = (inco(fac2)-inco(fac1))/dist, surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2)),
          visc_lam = nu_lam(elem), reyn = DERIVED_T::IS_TURB ? - 2.*nu_turb(elem)*tau : 0.0; // On considere que l energie est dans la pression

      flux(0) = (DERIVED_T::IS_VAR && DERIVED_T::IS_TURB) ? (-reyn + 2.*visc_lam*tau) * surf : (-reyn + visc_lam*tau) * surf ;
    }
  else throw; /* TODO FIXME */
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::INTERNE, void>
Eval_Diff_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int fac2, int fac3, int fac4, DoubleVect& flux) const
{
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);
  if (is_SCALAIRE)
    {
      const int ori1=orientation(fac1), ori3 = orientation(fac3), elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), elem3 = elem_(fac4,0), elem4 = elem_(fac4,1);
      const double visc_lam = nu_lam_mean_4pts(elem1,elem2,elem3,elem4,ori3), visc_turb = nu_mean_4pts(elem1,elem2,elem3,elem4,ori3), surf = surface_(fac1,fac2), poros = porosity_(fac1,fac2),
          tau = (inco(fac4)-inco(fac3))/dist_face(fac3,fac4,ori1), tau_tr = (inco(fac2)-inco(fac1))/dist_face(fac1,fac2,ori3), reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

      flux(0) = (DERIVED_T::IS_VAR && DERIVED_T::IS_TURB) ? (reyn + visc_lam*(tau+tau_tr))* surf*poros : (reyn + visc_lam*tau)*surf*poros;
    }
  else throw; /* TODO FIXME */
}

// DIFT : Sur les aretes mixtes les termes croises du tenseur de Reynolds sont nuls: il ne reste donc que la diffusion laminaire
template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::MIXTE, void>
Eval_Diff_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int fac2, int fac3, int fac4, DoubleVect& flux) const
{
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);
  if (is_SCALAIRE)
    {
      if (inco(fac4)*inco(fac3) != 0)
        {
          int elem[4], ori1 = orientation(fac1), ori3 = orientation(fac3);
          elem[0] = elem_(fac3,0), elem[1] = elem_(fac3,1), elem[2] = elem_(fac4,0), elem[3] = elem_(fac4,1);
          double visc_lam_temp = 0, visc_turb_temp = 0;
          for (int i=0; i<4; i++)
            if (elem[i] != -1)
              {
                visc_lam_temp += nu_lam(elem[i]);
                visc_turb_temp += nu_turb(elem[i]);
              }
          visc_lam_temp/=3.0;
          visc_turb_temp/=3.0;

          double visc_lam = DERIVED_T::IS_VAR ? visc_lam_temp : nu_lam(0), visc_turb = visc_turb_temp, surf = surface_(fac1,fac2), poros = porosity_(fac1,fac2),
              tau = (inco(fac4)-inco(fac3))/dist_face(fac3,fac4,ori1), tau_tr = (inco(fac2)-inco(fac1))/dist_face(fac1,fac2,ori3), reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

          flux(0) = (DERIVED_T::IS_VAR && DERIVED_T::IS_TURB) ? (reyn + visc_lam*(tau+tau_tr)) * surf * poros : tau * surf * visc_lam * poros;
        }
    }
  else throw; /* TODO FIXME */
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
inline enable_if_t<Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
Eval_Diff_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int fac2, int fac3, int signe, DoubleVect& flux) const
{
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE), is_PAROI = (Arete_Type == Type_Flux_Arete::PAROI);
  if (is_SCALAIRE)
    {
      const int rang1 = (fac1-premiere_face_bord), rang2 = (fac2-premiere_face_bord), ori = orientation(fac3);
      if ( !uses_wall_law() )
        {
          int elem1 = elem_(fac3,0), elem2 = elem_(fac3,1);
          if (is_PAROI)
            {
              if (elem1 == -1) elem1 = elem2;
              else if (elem2 == -1) elem2 = elem1;
            }

          const double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = nu_mean_2pts(elem1,elem2,ori), surf = surface_(fac1,fac2), dist = dist_norm_bord(fac1), tps = inconnue->temps();
          const double vit_imp = is_PAROI ? 0.5*(Champ_Face_get_val_imp_face_bord(tps,rang1,ori,la_zcl)+Champ_Face_get_val_imp_face_bord(tps,rang2,ori,la_zcl)) :
              0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,tps,rang1,ori,la_zcl)+Champ_Face_get_val_imp_face_bord_sym(inco,tps,rang2,ori,la_zcl));
          const double tau  = signe*(vit_imp-inco(fac3))/dist;

          if (is_PAROI) flux(0) = (DERIVED_T::IS_TURB && DERIVED_T::IS_VAR) ? tau*surf*(visc_lam+visc_turb) : tau*surf*visc_lam;
          else flux(0) = DERIVED_T::IS_TURB ? tau * surf * (visc_lam + visc_turb) : tau * surf * visc_lam;
        }
      else
        {
          double tau1 = tau_tan(rang1,ori)*0.5*surface(fac1), tau2 = tau_tan(rang2,ori)*0.5*surface(fac2);
          flux(0) = tau1 + tau2;
        }
    }
  else throw; /* TODO FIXME */
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
inline enable_if_t< Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
Eval_Diff_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int fac2, int fac3, int signe, DoubleVect& flux3, DoubleVect& flux1_2) const
{
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE), is_SYM_FL = (Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE), is_PAR_FL = (Arete_Type == Type_Flux_Arete::PAROI_FLUIDE);
  if (is_SCALAIRE)
    {
      const int rang1 = (fac1-premiere_face_bord), rang2 = (fac2-premiere_face_bord), elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), ori= orientation(fac3);
      double vit_imp, visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = nu_mean_2pts(elem1,elem2,ori), surf = surface_(fac1,fac2), poros = porosity_(fac1,fac2),
          surfporos = surface(fac3)*porosite(fac3), tps = inconnue->temps(), dist1 = dist_norm_bord(fac1), dist2 = dist_face(fac1,fac2,ori);

      if (is_SYM_FL) vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,tps,rang1,ori,la_zcl)+ Champ_Face_get_val_imp_face_bord_sym(inco,tps,rang2,ori,la_zcl));
      else if (is_PAR_FL) // On ne sait pas qui de fac1 ou de fac2 est la face de paroi
        {
          if (est_egal(inco(fac1),0)) vit_imp = Champ_Face_get_val_imp_face_bord(tps,rang2,ori,la_zcl); // fac1 est la face de paroi
          else vit_imp = Champ_Face_get_val_imp_face_bord(tps,rang1,ori,la_zcl); // fac2 est la face de paroi
        }
      else vit_imp =  0.5*(Champ_Face_get_val_imp_face_bord(tps,rang1,ori,la_zcl)+Champ_Face_get_val_imp_face_bord(tps,rang2,ori,la_zcl));

      const double tau = signe*(vit_imp-inco(fac3))/dist1, tau_tr = (inco(fac2)-inco(fac1))/dist2, reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

      if(DERIVED_T::IS_VAR && DERIVED_T::IS_TURB)
        {
          flux3(0) = ((tau+tau_tr)*visc_lam + reyn)*surf*poros;
          flux1_2(0) = ((tau+tau_tr)*visc_lam + reyn)*surfporos;
        }
      else
        {
          flux3(0) = (tau*visc_lam + reyn)*surf*poros;
          flux1_2(0) = (DERIVED_T::IS_VAR && is_SYM_FL) ? 0.5 * tau_tr * visc_lam * surfporos : (tau_tr*visc_lam + reyn)*surfporos;
        }
    }
  else throw; /* TODO FIXME */
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE, void>
Eval_Diff_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int fac2, int fac3, int fac4, DoubleVect& flux3_4, DoubleVect& flux1_2) const
{
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);
  if (is_SCALAIRE)
    {
      const int ori1 = orientation(fac1), ori3 = orientation(fac3), elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), elem3 = elem_(fac4,0), elem4 = elem_(fac4,1);
      const double dist3_4 = dist_face_period(fac3,fac4,ori1), dist1_2 = dist_face_period(fac1,fac2,ori3), surf1_2 = surface_(fac1,fac2), poros1_2 = porosity_(fac1, fac2),
          surf3_4 = surface_(fac3,fac4), poros3_4 = porosity_(fac3, fac4), visc_lam = nu_lam_mean_4pts(elem1,elem2,elem3,elem4,ori3), visc_turb = nu_mean_4pts(elem1,elem2,elem3,elem4,ori3),
          tau = (inco(fac4)-inco(fac3))/dist3_4, tau_tr = (inco(fac2)-inco(fac1))/dist1_2, reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

      if(DERIVED_T::IS_VAR && DERIVED_T::IS_TURB)
        {
          flux3_4(0) = (reyn + visc_lam*(tau+tau_tr)) * surf1_2 * poros1_2;
          flux1_2(0) = (reyn + visc_lam*(tau+tau_tr)) * surf3_4 * poros3_4;
        }
      else
        {
          flux3_4(0) = (reyn + visc_lam*tau) * surf1_2 * poros1_2;
          flux1_2(0) = (reyn + visc_lam*tau_tr) * surf3_4 * poros3_4;
        }
    }
  else throw; /* TODO FIXME */
}

/* ************************************** *
 * *********  POUR L'IMPLICITE ********** *
 * ************************************** */

template <typename DERIVED_T> template<Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_fa7(int , const Neumann_sortie_libre&, DoubleVect& f1, DoubleVect& f2) const
{
  assert(f1.size() == f2.size());
  for (int k = 0; k < f1.size(); k++) f1(k) = f2(k) = 0.;
}

template <typename DERIVED_T> template<Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_fa7(int elem,int fac1, int fac2, DoubleVect& f1, DoubleVect& f2) const
{
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);
  if (is_SCALAIRE)
    {
      const int ori = orientation(fac1);
      const double tau = 1/dist_face(fac1,fac2,ori), visc_lam = nu_lam(elem), surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2)),
          reyn = DERIVED_T::IS_TURB ? - 2.*nu_turb(elem)*tau : 0.0;

      if(DERIVED_T::IS_VAR && DERIVED_T::IS_TURB) f1(0) = f2(0) =(-reyn +2.*visc_lam*tau) * surf;
      else f1(0) = f2(0) =(-reyn + visc_lam*tau) * surf;
    }
  else throw; /* TODO FIXME */
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::INTERNE, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2, int fac3, int fac4, DoubleVect& aii, DoubleVect& ajj) const
{
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);
  if (is_SCALAIRE)
    {
      const int ori1=orientation(fac1), ori3 = orientation(fac3), elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), elem3 = elem_(fac4,0), elem4 = elem_(fac4,1);
      const double visc_lam = nu_lam_mean_4pts(elem1,elem2,elem3,elem4,ori3), visc_turb = nu_mean_4pts(elem1,elem2,elem3,elem4,ori3), surf = surface_(fac1,fac2), poros = porosity_(fac1,fac2),
          tau = 1/dist_face(fac3,fac4,ori1), tau_tr = 1/dist_face(fac1,fac2,ori3), reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

      if (DERIVED_T::IS_VAR && DERIVED_T::IS_TURB) aii(0) = ajj(0) = (reyn + visc_lam*(tau+tau_tr))*surf*poros;
      else aii(0) = ajj(0) = (reyn + visc_lam*tau)*surf*poros;
    }
  else throw; /* TODO FIXME */
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::MIXTE, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2, int fac3, int fac4, DoubleVect& aii, DoubleVect& ajj) const
{
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);
  if (is_SCALAIRE)
    {
      if (inconnue->valeurs()(fac4) * inconnue->valeurs()(fac3) != 0)
        {
          const int ori1 = orientation(fac1), ori3 = orientation(fac3);
          int elem[4];
          elem[0] = elem_(fac3,0), elem[1] = elem_(fac3,1), elem[2] = elem_(fac4,0), elem[3] = elem_(fac4,1);

          double visc_lam_temp = 0, visc_turb_temp = 0;
          for (int i=0; i<4; i++)
            if (elem[i] != -1)
              {
                visc_lam_temp += nu_lam(elem[i]);
                visc_turb_temp += nu_turb(elem[i]);
              }
          visc_lam_temp/=3.0;
          visc_turb_temp/=3.0;

          const double visc_lam = DERIVED_T::IS_VAR ? visc_lam_temp : nu_lam(0), visc_turb = visc_turb_temp, surf = surface_(fac1,fac2), poros = porosity_(fac1,fac2),
              tau = 1/dist_face(fac3,fac4,ori1), tau_tr = 1/dist_face(fac1,fac2,ori3), reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

          if (DERIVED_T::IS_TURB && DERIVED_T::IS_VAR) aii(0) = ajj(0) = (reyn + visc_lam*(tau+tau_tr))* surf * poros; // XXX : if we regroup => SOME TEST CASES BROKE UP !
          else aii(0) = ajj(0) = surf * visc_lam * poros * tau;
        }
      else aii(0) = ajj(0) = 0;
    }
  else throw; /* TODO FIXME */
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2, int fac3, int fac4, DoubleVect& aii, DoubleVect& ajj) const
{
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);
  if (is_SCALAIRE)
    {
      const int ori1 = orientation(fac1), ori3 = orientation(fac3), elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), elem3 = elem_(fac4,0), elem4 = elem_(fac4,1);
      const double dist3_4 = dist_face_period(fac3,fac4,ori1), dist1_2 = dist_face_period(fac1,fac2,ori3), visc_lam = nu_lam_mean_4pts(elem1,elem2,elem3,elem4,ori3),
          visc_turb = nu_mean_4pts(elem1,elem2,elem3,elem4,ori3), surf = surface_(fac1,fac2), poros = porosity_(fac1, fac2), tau = 1/dist3_4, tau_tr = 1/dist1_2,
          reyn = DERIVED_T::IS_TURB ? (tau + tau_tr) * visc_turb : 0.0;

      if(DERIVED_T::IS_VAR && DERIVED_T::IS_TURB) aii(0) = ajj(0) = (reyn + visc_lam*(tau+tau_tr))* surf * poros;
      else aii(0) = ajj(0) = (reyn + visc_lam*tau) * surf * poros;
    }
  else throw; /* TODO FIXME */
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
inline enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2, int fac3, int signe, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
{
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE), is_PAROI = (Arete_Type == Type_Flux_Arete::PAROI);
  if (is_SCALAIRE)
    {
      if ( !uses_wall_law())
        {
          int elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), ori = orientation(fac3);
          if(is_PAROI)
            {
              if (elem1 == -1) elem1 = elem2;
              else if (elem2 == -1) elem2 = elem1;
            }

          const double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = nu_mean_2pts(elem1,elem2,ori), dist = dist_norm_bord(fac1), surf = surface_(fac1,fac2);
          aii1_2(0) = ajj1_2(0) = 0;
          // TODO : FIXME : Yannick help :/
          if (is_PAROI) aii3_4(0) = (DERIVED_T::IS_TURB && DERIVED_T::IS_VAR) ? signe*surf*(visc_lam+visc_turb)/dist : signe*surf*visc_lam/dist;
          else aii3_4(0) = DERIVED_T::IS_TURB ? ( DERIVED_T::IS_VAR ? signe * surf * (visc_lam + visc_turb) / dist : 0.0 ) : signe * surf * visc_lam / dist;
        }
      else aii3_4(0) = aii1_2(0) = ajj1_2(0) = 0;
    }
  else throw; /* TODO FIXME */
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
inline enable_if_t< Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2, int fac3, int signe, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
{
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE), is_SYM_FL = (Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE);
  if (is_SCALAIRE)
    {
      const int elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), ori= orientation(fac3);
      const double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = nu_mean_2pts(elem1,elem2,ori), surf = surface_(fac1,fac2),
             poros = porosity_(fac1,fac2), surfporos = surface(fac3)*porosite(fac3), dist1 = dist_norm_bord(fac1), dist2 = dist_face(fac1,fac2,ori),
             tau = signe/dist1, tau_tr = 1/dist2, reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

      if(DERIVED_T::IS_VAR && DERIVED_T::IS_TURB)
        {
          aii3_4(0) = ((tau+tau_tr)*visc_lam + reyn)*surf*poros;
          aii1_2(0) = ajj1_2(0)  = ((tau+tau_tr)*visc_lam + reyn)*surfporos;
        }
      else
        {
          aii3_4(0) = (tau*visc_lam + reyn)*surf*poros;
          aii1_2(0) = ajj1_2(0)  = (DERIVED_T::IS_VAR && is_SYM_FL) ? 0.5 * tau_tr * visc_lam * surfporos : (tau_tr*visc_lam + reyn)*surfporos; // TODO : FIXME : Yannick help :/
        }
    }
  else throw; /* TODO FIXME */
}

/* ************************************** *
 * *********  POUR L'IMPLICITE ********** *
 * ************************************** */

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
inline enable_if_t<Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete(int fac1, int fac2, int fac3, int signe, DoubleVect& flux) const
{
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);
  if (is_SCALAIRE)
    {
      const int rang1 = (fac1-premiere_face_bord), rang2 = (fac2-premiere_face_bord), ori = orientation(fac3);
      if ( !uses_wall_law() )
        {
          const int elem1 = elem_(fac3,0), elem2 = elem_(fac3,1);
          const DoubleTab& inco = inconnue->valeurs();
          const double dist = dist_norm_bord(fac1), visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = nu_mean_2pts(elem1,elem2,ori), surf = surface_(fac1,fac2),
              tps = inconnue->temps(), vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(tps,rang1,ori,la_zcl)+Champ_Face_get_val_imp_face_bord(tps,rang2,ori,la_zcl));
          const double tau = DERIVED_T::IS_TURB ? signe * (vit_imp - inco(fac3)) / dist : signe * vit_imp / dist; // TODO : FIXME : Yannick help :/
          flux(0) = (DERIVED_T::IS_TURB && DERIVED_T::IS_VAR) ? tau * surf * (visc_lam + visc_turb) : tau * surf * visc_lam;
        }
      else
        {
          double tau1 = tau_tan(rang1,ori)*0.5*surface(fac1), tau2 = tau_tan(rang2,ori)*0.5*surface(fac2);
          flux(0) = tau1 + tau2;
        }
    }
  else throw; /* TODO FIXME */
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
inline enable_if_t<Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete(int fac1, int fac2, int fac3,int signe,DoubleVect& flux3, DoubleVect& flux1_2) const
{
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE), is_PAROI_FL = (Arete_Type == Type_Flux_Arete::PAROI_FLUIDE);
  if (is_SCALAIRE)
    {
      const int rang1 = (fac1-premiere_face_bord), rang2 = (fac2-premiere_face_bord), elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), ori= orientation(fac3);
      double vit_imp, visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = nu_mean_2pts(elem1,elem2,ori), dist = dist_norm_bord(fac1),
             surf = surface_(fac1,fac2), poros = porosity_(fac1,fac2), tps = inconnue->temps();

      if (is_PAROI_FL) // On ne sait pas qui de fac1 ou de fac2 est la face de paroi
        {
          if (est_egal(inconnue->valeurs()(fac1),0)) vit_imp = Champ_Face_get_val_imp_face_bord(tps,rang2,ori,la_zcl); // fac1 est la face de paroi
          else vit_imp = Champ_Face_get_val_imp_face_bord(tps,rang1,ori,la_zcl); // fac2 est la face de paroi
        }
      else vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(tps,rang1,ori,la_zcl)+Champ_Face_get_val_imp_face_bord(tps,rang2,ori,la_zcl));

      const double tau = signe*vit_imp/dist, reyn = DERIVED_T::IS_TURB ? tau*visc_turb : 0.0;
      flux3(0) = DERIVED_T::IS_TURB ? (tau * visc_lam + reyn) * surf * poros : tau * surf * visc_lam * poros; // XXX : if we regroup => SOME TEST CASES BROKE UP !
      flux1_2(0) = 0;
    }
  else throw; /* TODO FIXME */
}

#endif /* Eval_Diff_VDF_Face_TPP_included */
