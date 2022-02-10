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
// File:        Champ_Q1NC.h
// Directory:   $TRUST_ROOT/src/VEF/Champs
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Q1NC_included
#define Champ_Q1NC_included

#include <Champ_Inc_base.h>
#include <Champ_Q1NC_impl.h>
#include <Ref_Zone_VEF.h>
class Domaine;

class Champ_Q1NC: public Champ_Inc_base, public Champ_Q1NC_impl
{

  Declare_instanciable(Champ_Q1NC);

public :
  const Zone_dis_base& zone_dis_base() const override;
  void associer_zone_dis_base(const Zone_dis_base&) override;
  void verifie_valeurs_cl() override;
  inline const Zone_VEF& zone_vef() const override;
  int compo_normale_sortante(int ) const;
  DoubleTab& trace(const Frontiere_dis_base& , DoubleTab& , double ,int distant) const override;
  void cal_rot_ordre1(DoubleTab&);
  void gradient(DoubleTab&);


  inline DoubleVect& valeur_a_elem(const DoubleVect& position,
                                   DoubleVect& val,
                                   int le_poly) const override;
  inline double valeur_a_elem_compo(const DoubleVect& position,
                                    int le_poly, int ncomp) const override;
  inline double valeur_a_sommet_compo(int num_som, int le_poly, int ncomp) const override;
  inline DoubleTab& valeur_aux_elems(const DoubleTab& positions,
                                     const IntVect& les_polys,
                                     DoubleTab& tab_valeurs) const override;
  inline DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions,
                                            const IntVect& les_polys,
                                            DoubleVect& tab_valeurs,
                                            int ncomp) const override ;
  inline DoubleTab& valeur_aux_sommets(const Domaine&, DoubleTab&) const override;
  inline DoubleVect& valeur_aux_sommets_compo(const Domaine&,
                                              DoubleVect&, int) const override;
  inline DoubleTab& remplir_coord_noeuds(DoubleTab& positions) const override;
  inline int remplir_coord_noeuds_et_polys(DoubleTab& positions,
                                           IntVect& polys) const override;
  int fixer_nb_valeurs_nodales(int n) override;
private:

  REF(Zone_VEF) la_zone_VEF;
  inline const Champ_base& le_champ() const override;
  inline Champ_base& le_champ() override;


};

inline const Champ_base& Champ_Q1NC::le_champ() const
{
  return *this;
}

inline Champ_base& Champ_Q1NC::le_champ()
{
  return *this;
}

inline DoubleVect& Champ_Q1NC::valeur_a_elem(const DoubleVect& position,
                                             DoubleVect& val,
                                             int le_poly) const
{
  return Champ_Q1NC_impl::valeur_a_elem(position, val, le_poly);
}

inline double  Champ_Q1NC::valeur_a_elem_compo(const DoubleVect& position,
                                               int le_poly, int ncomp) const
{
  return Champ_Q1NC_impl::valeur_a_elem_compo(position, le_poly, ncomp);
}

inline double  Champ_Q1NC::valeur_a_sommet_compo(int num_som,
                                                 int le_poly, int ncomp) const
{
  return Champ_Q1NC_impl::valeur_a_sommet_compo(num_som, le_poly, ncomp);
}

inline DoubleTab& Champ_Q1NC::valeur_aux_elems(const DoubleTab& positions,
                                               const IntVect& les_polys,
                                               DoubleTab& tab_valeurs) const
{
  return Champ_Q1NC_impl::valeur_aux_elems(positions, les_polys, tab_valeurs);
}

inline DoubleVect& Champ_Q1NC::valeur_aux_elems_compo(const DoubleTab& positions,
                                                      const IntVect& les_polys,
                                                      DoubleVect& tab_valeurs,
                                                      int ncomp) const
{
  return Champ_Q1NC_impl::valeur_aux_elems_compo(positions, les_polys, tab_valeurs, ncomp);
}

inline DoubleTab& Champ_Q1NC::valeur_aux_sommets(const Domaine& dom, DoubleTab& val) const
{
  return Champ_Q1NC_impl::valeur_aux_sommets(dom, val);
}

inline DoubleVect& Champ_Q1NC::valeur_aux_sommets_compo(const Domaine& dom,
                                                        DoubleVect& val, int comp) const
{
  return Champ_Q1NC_impl::valeur_aux_sommets_compo(dom, val, comp);
}

inline DoubleTab& Champ_Q1NC::remplir_coord_noeuds(DoubleTab& positions) const
{
  return Champ_Q1NC_impl::remplir_coord_noeuds(positions);
}

inline int Champ_Q1NC::remplir_coord_noeuds_et_polys(DoubleTab& positions,
                                                     IntVect& polys) const
{
  return Champ_Q1NC_impl::remplir_coord_noeuds_et_polys(positions, polys);
}

inline const Zone_VEF& Champ_Q1NC::zone_vef() const
{
  return la_zone_VEF.valeur();
}

#endif

