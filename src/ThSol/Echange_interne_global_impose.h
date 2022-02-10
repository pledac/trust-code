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
// File:        Echange_interne_global_impose.h
// Directory:   $TRUST_ROOT/src/ThSol
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Echange_interne_global_impose_included
#define Echange_interne_global_impose_included

#include <Echange_global_impose.h>
#include <Ref_Champ_Don.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Echange_interne_global_impose:
//    Cette classe represente le cas particulier de la classe
//    Echange_global_impose ou la paroi d'echange est interne au domaine.
//    La temperature exterieure definie dans Echange_global_impose devient alors la
//    temperature situee de l'autre cote de la paroi interne.
//    L'implicitation de cette CL demande un traitement spécial, voir Op_Diff_EF::ajouter_contributions_bords()
// .SECTION voir aussi
//    Echange_impose_base Echange_global_impose
//////////////////////////////////////////////////////////////////////////////
class Echange_interne_global_impose  : public Echange_global_impose
{

  Declare_instanciable(Echange_interne_global_impose);

public:
  void completer() override;
  virtual void init();   // != initialiser()
  void verifie_ch_init_nb_comp() override;


  const DoubleVect surface_gap() const
  {
    return surface_gap_;
  }

  const DoubleVect surface_gap()
  {
    return surface_gap_;
  }

protected:

  DoubleVect surface_gap_;
};
#endif
