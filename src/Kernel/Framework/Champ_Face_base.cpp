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

#include <Dirichlet_homogene.h>
#include <Neumann_homogene.h>
#include <Champ_Face_base.h>
#include <Zone_Cl_dis.h>
#include <Dirichlet.h>
#include <Neumann.h>
#include <Navier.h>

Implemente_base(Champ_Face_base,"Champ_Face_base",Champ_Inc_base) ;

Sortie& Champ_Face_base::printOn(Sortie& os) const { return os; }
Entree& Champ_Face_base::readOn(Entree& is) { return is; }

//tableaux de correspondance pour les CLs
void Champ_Face_base::init_fcl() const
{
  const Zone_VF& zone = ref_cast(Zone_VF,la_zone_VF.valeur());
  const Conds_lim& cls = zone_Cl_dis().les_conditions_limites();
  int i, f, n;

  fcl_.resize(zone.nb_faces_tot(), 3);
  for (n = 0; n < cls.size(); n++)
    {
      const Front_VF& fvf = ref_cast(Front_VF, cls[n].frontiere_dis());
      int idx = sub_type(Neumann, cls[n].valeur())
                + 2 * sub_type(Navier, cls[n].valeur())
                + 3 * sub_type(Dirichlet, cls[n].valeur()) + 3 * sub_type(Neumann_homogene, cls[n].valeur())
                + 4 * sub_type(Dirichlet_homogene, cls[n].valeur());
      if (!idx)
        {
          Cerr << "Champ_Face_base : CL non codee rencontree!" << finl;
          Process::exit();
        }
      for (i = 0; i < fvf.nb_faces_tot(); i++)
        f = fvf.num_face(i), fcl_(f, 0) = idx, fcl_(f, 1) = n, fcl_(f, 2) = i;
    }
  fcl_init_ = 1;
}