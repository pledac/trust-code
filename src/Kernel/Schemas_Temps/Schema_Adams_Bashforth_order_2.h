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
// File:        Schema_Adams_Bashforth_order_2.h
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Schema_Adams_Bashforth_order_2_included
#define Schema_Adams_Bashforth_order_2_included


#include <Schema_Adams_Bashforth_base.h>

//////////////////////////////////////////////////////////////////////////////
// .DESCRIPTION
//     classe Schema_Adams_Bashforth_order_2
//     Cette classe represente un schema en temps d'Adams-Bashforth d'ordre 2
//     a pas de temps variable :
//     U(n+1) = U(n) + dt*(3/2*(dU/dt)(n)-1/2(dU/dt)(n-1)) si le pas de temps
//     est constant
// .SECTION voir aussi
//     Schema_Temps_base
//////////////////////////////////////////////////////////////////////////////
class Schema_Adams_Bashforth_order_2: public Schema_Adams_Bashforth_base
{

  Declare_instanciable(Schema_Adams_Bashforth_order_2);

public :

  int nb_pas_dt_seuil() const override ;
  int nb_valeurs_passees() const override ;
  int nb_valeurs_temporelles() const override ;

  void   compute_adams_bashforth_coefficients(double time_step, const DoubleTab& times) override     ;
};

#endif
