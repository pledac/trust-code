/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Segment_PolyMAC.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Segment_PolyMAC_included
#define Segment_PolyMAC_included

#include <Elem_PolyMAC_base.h>

class Segment_PolyMAC : public Elem_PolyMAC_base
{

  Declare_instanciable_sans_constructeur(Segment_PolyMAC);

public:
  Segment_PolyMAC();
  void calcul_vc(const ArrOfInt& ,ArrOfDouble& ,const ArrOfDouble& ,
                 const DoubleTab& ,const Champ_Inc_base& ,int ) const;
  void calcul_xg(DoubleVect& ,const DoubleTab& ,const int ,int& ,
                 int& ,int& ,int& ) const;
  void normale(int ,DoubleTab&, const IntTab& ,const IntTab&,
               const IntTab& ,const Zone& )  const ;
};

#endif



