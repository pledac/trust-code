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
// File:        DoubleTrav.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////
//
// WARNING: DO NOT EDIT THIS FILE! Only edit the template file DoubleTrav.h.P
//
//
#ifndef DoubleTrav_included
#define DoubleTrav_included

#include <TRUSTTab.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//            Tableau de travail a n entrees pour n<= 4
//      C'est un DoubleTab avec allocation dans un pool de memoire gere par
//      la_memoire. L'allocation initiale est realisee avec alloc_temp_storage
// .SECTION voir aussi
//     DoubleTab Memoire
//////////////////////////////////////////////////////////////////////////////
class DoubleTrav : public DoubleTab
{
  Declare_instanciable_sans_constructeur_ni_destructeur(DoubleTrav);
public:
  DoubleTrav();
  DoubleTrav(int n);
  DoubleTrav(int n1, int n2);
  DoubleTrav(int n1, int n2, int n3);
  DoubleTrav(int n1, int n2, int n3, int n4);

  DoubleTrav(const DoubleTab&);  // ATTENTION: non standard, voir la description !!!
  DoubleTrav(const DoubleVect&); // ATTENTION: non standard, voir la description !!!

  // Constructeur par copie
  DoubleTrav(const DoubleTrav&); // ATTENTION: non standard, voir la description !!!
  // Operateurs copie
  DoubleTrav& operator=(const DoubleTrav&);
  DoubleTrav& operator=(const DoubleTab&);
  DoubleTrav& operator=(const DoubleVect&);
  DoubleTrav& operator=(double d);

};

int DoubleTrav_test();

#endif
//DoubleTRAV_H

