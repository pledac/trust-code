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
// File:        Diametre_bulles_constant.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Diametre_bulles_constant.h>
#include <Ref_Champ_base.h>
#include <Discret_Thyd.h>
#include <Pb_Multiphase.h>
#include <Zone_dis.h>

Implemente_instanciable(Diametre_bulles_constant, "Diametre_bulles_constant", Correlation_base);

Sortie& Diametre_bulles_constant::printOn(Sortie& os) const
{
  return os;
}

Entree& Diametre_bulles_constant::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("diametre", &d_bulle_, Param::REQUIRED);
  param.lire_avec_accolades_depuis(is);

  Pb_Multiphase& pb = ref_cast(Pb_Multiphase, pb_.valeur());
  int N = pb.nb_phases();
  const Discret_Thyd& dis=ref_cast(Discret_Thyd,pb.discretisation());
  Noms noms(N), unites(N);
  noms[0] = "diametre_bulles";
  unites[0] = "m";
  Motcle typeChamp = "champ_elem" ;
  const Zone_dis& z = ref_cast(Zone_dis, pb.domaine_dis().zone_dis(0));
  dis.discretiser_champ(typeChamp, z.valeur(), scalaire, noms , unites, N, 0, diametres_);

  champs_compris_.ajoute_champ(diametres_);

  DoubleTab& tab_diametres = diametres_->valeurs();
  for (int i = 0 ; i < tab_diametres.dimension_tot(0) ; i++) for (int n = 1 ; n <N ; n++) tab_diametres(i, n) = d_bulle_;

  return is;
}

const Champ_base& Diametre_bulles_constant::get_champ(const Motcle& nom) const
{
  REF(Champ_base) ref_champ;
  if (nom=="diametre_bulles")
    return champs_compris_.get_champ(nom);

  throw Champs_compris_erreur();
  return ref_champ;
}
