/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Modele_turbulence_hyd_LES_VEF_base.h>
#include <distances_VEF.h>
#include <Domaine_VEF.h>
#include <Domaine_Cl_VEF.h>
#include <Domaine_Cl_dis.h>

Implemente_base(Modele_turbulence_hyd_LES_VEF_base, "Modele_turbulence_hyd_LES_VEF_base", Modele_turbulence_hyd_LES_base);

Sortie& Modele_turbulence_hyd_LES_VEF_base::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Modele_turbulence_hyd_LES_VEF_base::readOn(Entree& is)
{
  return Modele_turbulence_hyd_LES_base::readOn(is);
}

void Modele_turbulence_hyd_LES_VEF_base::associer(const Domaine_dis& domaine_dis, const Domaine_Cl_dis& domaine_Cl_dis)
{
  le_dom_VEF_ = ref_cast(Domaine_VEF, domaine_dis.valeur());
  le_dom_Cl_VEF_ = ref_cast(Domaine_Cl_VEF, domaine_Cl_dis.valeur());
}

void Modele_turbulence_hyd_LES_VEF_base::calculer_longueurs_caracteristiques()
{
  const Domaine_VEF& domaine_VEF = le_dom_VEF_.valeur();
  const int nb_elem = domaine_VEF.nb_elem_tot();
  l_.resize(nb_elem);

  calcul_longueur_filtre(l_, methode_, domaine_VEF);
}
