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

#include <Schema_Euler_explicite.h>
#include <Equation_base.h>

Implemente_instanciable(Schema_Euler_explicite,"Schema_euler_explicite|Scheme_euler_explicit",TRUSTSchema_RK<Ordre_RK::UN>);
// XD euler_scheme schema_temps_base schema_euler_explicite -1 This is the Euler explicit scheme.

Sortie& Schema_Euler_explicite::printOn(Sortie& s) const { return  TRUSTSchema_RK<Ordre_RK::UN>::printOn(s); }

Entree& Schema_Euler_explicite::readOn(Entree& s) { return TRUSTSchema_RK<Ordre_RK::UN>::readOn(s) ; }

/*! @brief Effectue un pas de temps d'Euler explicite sur l'equation passee en parametre.
 *
 */
int Schema_Euler_explicite::faire_un_pas_de_temps_eqn_base(Equation_base& eqn)
{
  DoubleTab& present = eqn.inconnue()->valeurs(); // Un
  DoubleTab& futur = eqn.inconnue()->futur();   // Un+1
  DoubleTrav dudt(futur); // just for initializing the array structure ... Trav is highly recommanded!! Otherwise we allocate at each time step!!

  // Boundary conditions applied on Un+1:
  eqn.domaine_Cl_dis().imposer_cond_lim(eqn.inconnue(), temps_courant() + pas_de_temps());

  // At this point, the inconnue() is in the following state:
  //   present(): holds the current value, but updates for the BC have already been done (line just above)
  //   future(): holds the present value, unmodified yet (this was initialized by initTimeStep() which set all the 'future' slots
  //             of the unknown to the 'present' value).
  // So we turn the wheel to get an unmodified version of 'present' for the derivee_en_temps_inco() computation (full explicit scheme):
  // (commenting those lines will trigger failure for cases with variable BC)
  eqn.inconnue()->avancer();
  eqn.derivee_en_temps_inco(dudt);
  eqn.inconnue()->reculer();

  // Un+1=Un+dt_*dU/dt
  futur = dudt;
  futur *= dt_;
  futur += present;

  eqn.domaine_Cl_dis().imposer_cond_lim(eqn.inconnue(), temps_courant() + pas_de_temps());
  update_critere_statio(dudt, eqn);

  return 1;
}
