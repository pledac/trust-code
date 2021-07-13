/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Loi_Etat_GP_WC.h
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Weakly_Compressible/Loi_Etat
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Loi_Etat_GP_WC_included
#define Loi_Etat_GP_WC_included

#include <Loi_Etat_base.h>
#include <Champ.h>

class Fluide_Dilatable_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Loi_Etat_GP_WC
//     Cette classe represente la loi d'etat pour les gaz parfaits.
//     Associe a un fluide dilatable, elle definit un fluide weakly compressible
//     dont la loi d'eata est :
//        Pth = rho*R*T
// .SECTION voir aussi
//     Fluide_Dilatable_base Loi_Etat_base
//
//////////////////////////////////////////////////////////////////////////////

class Loi_Etat_GP_WC : public Loi_Etat_base
{
  Declare_instanciable_sans_constructeur(Loi_Etat_GP_WC);

public :
  Loi_Etat_GP_WC();
  const Nom type_fluide() const;
  void associer_fluide(const Fluide_Dilatable_base&);
  void calculer_lambda();
  void calculer_alpha();
  void remplir_T();
  void calculer_Cp();
  double calculer_masse_volumique(double,double) const;
  double inverser_Pth(double,double);

  virtual void calculer_masse_volumique();
  virtual void initialiser();

  // Methodes inlines
  inline double R() const { return R_; }

protected :
  double Cp_, R_;
};

#endif /* Loi_Etat_GP_WC_included */
