/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Matrice_Morse.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/41
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Matrice_Morse_included
#define Matrice_Morse_included

#include <Matrice_Base.h>
#include <IntTab.h>
#include <algorithm>

class DoubleLists;
class IntLists;
class DoubleVects;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Matrice_Morse
//    Represente une matrice M (creuse), non necessairement carree
//    stockee au format Morse.
//    -----------------------------------------------------------------------
//    On utilise 3 tableaux tab1(n+1), tab2(nnz) et coeff_(nnz):
//    On note Vi = { j differents de i / M(i,j) est non nul }
//            tab1[i] = rang dans tab2 de la ieme ligne
//            pour tab1[i] <= j < tab1[i+1],  tab2[j] decrit Vi
//            et coeff_[j] = M(i,tab2[j])
//            tab1 et tab2 sont des rangs au sens fortran:
//                  1 <= tab2[i] <= n
//                  tab1[n+1] = nnz+1
//    Remarque: dans ce commentaire le [] est a prendre au sens fortran:
//                    tab1[1] designe la premiere valeur de tab1
//    -----------------------------------------------------------------------
// .SECTION voir aussi
//     Matrice_Base Matrice_Morse_Sym
//////////////////////////////////////////////////////////////////////////////
class Matrice_Morse : public Matrice_Base
{

  Declare_instanciable_sans_constructeur(Matrice_Morse);

public :

  //constructeurs :

  // par defaut le scalaire 0:
  Matrice_Morse() ;
  Matrice_Morse(int n,int nnz) ;

  // Une matrice a n lignes et m colonnes a nnz coefficients non nuls :
  Matrice_Morse(int n, int m, int nnz) ;

  // copie :
  Matrice_Morse(const Matrice_Morse& ) ;
  Matrice_Morse(int , int , const IntLists& ,const DoubleLists& ,const DoubleVect& );

  Sortie& imprimer(Sortie& s) const;
  Sortie& imprimer_formatte(Sortie& s) const;
  Sortie& imprimer_formatte(Sortie& s, int symetrie) const;
  int largeur_de_bande() const;         // Retourne la largeur de bande
  void remplir(const IntLists& ,const DoubleLists& ,const DoubleVect& );
  void remplir(const IntLists& ,const DoubleLists&);
  void remplir(const int, const int, const int, const int, const Matrice_Morse& ) ;
  //dimensionner
  void dimensionner(int n, int nnz);
  void dimensionner(int n, int m, int nnz);

  // place pour d'eventuels nouveaux coefficients non nuls
  // (modif MT)
  void dimensionner(const IntTab&);

  // ordre retourne n si n==m
  int ordre() const;

  int nb_lignes() const
  {
    return tab1_.size()-1; // nb_lignes retourne n
  }
  int nb_colonnes() const
  {
    return m_; // nb_colonnes retourne m
  }
  int nb_coeff() const
  {
    return coeff_.size(); // nb_coeff retourne nnz
  }
  const int* tab1() const
  {
    return tab1_.addr(); // pour passer a fortran :
  }
  const int* tab2() const
  {
    return tab2_.addr();
  }
  const double* coeff() const
  {
    return coeff_.addr();
  }
  const int& tab1(int i) const
  {
    return tab1_[i]; // i de 0 a n
  }
  int& tab1(int i)
  {
    return tab1_[i];
  }
  const int& tab2(int i) const
  {
    return tab2_[i]; // i de 0 a nnz-1
  }
  int& tab2(int i)
  {
    return tab2_[i];
  }
  const double& coeff(int i) const
  {
    return coeff_[i]; // i de 0 a nnz-1
  }
  double& coeff(int i)
  {
    return coeff_[i];
  }
  int nb_vois(int i) const
  {
    return tab1(i+1)-tab1(i); // nb_vois(i) : nombre d'elements non nuls de la ligne i
  }

  //methode pour nettoyer la matrice.
  void clean();

  // operateurs :
  // 0<=i,j<=n-1
  inline double& operator()(int i, int j);
  inline double operator()(int i, int j) const;
  // Ne pas supprimer ces deux methodes coef(i,j) qui bien qu'elles fassent la meme chose que les
  // deux precedents sont utilisees tres souvent par OVAP:
  inline double coef(int i, int j) const
  {
    return operator()(i,j);
  };
  inline double& coef(int i,int j)
  {
    return operator()(i,j);
  };

  Matrice_Morse& operator=(const Matrice_Morse& );
  friend Matrice_Morse operator +(const Matrice_Morse&, const Matrice_Morse& );
  Matrice_Morse& operator +=(const Matrice_Morse& );
  Matrice_Morse& operator *=(double );
  virtual void scale( const double& x );

  virtual void get_stencil( IntTab& stencil ) const;

  virtual void get_stencil_and_coefficients( IntTab&      stencil,
                                             ArrOfDouble& coefficients ) const;

  Matrice_Morse& operator /=(double );
  Matrice_Morse& operator *=(DoubleVect& );
  //Matrice_Morse& operator *=(const Matrice_Diagonale& );
  friend Matrice_Morse operator *(double, const Matrice_Morse& );
  Matrice_Morse operator -() const;
  virtual int inverse(const DoubleVect&, DoubleVect&, double ) const ;
  virtual int inverse(const DoubleVect&, DoubleVect&, double, int ) const ;
  Matrice_Morse& affecte_prod(const Matrice_Morse& A, const Matrice_Morse& B);

  void compacte(int elim_coeff_nul=0);

  // y += Ax
  virtual DoubleVect& ajouter_multvect_(const DoubleVect& ,DoubleVect& ) const;
  ArrOfDouble& ajouter_multvect_(const ArrOfDouble& ,ArrOfDouble&, ArrOfInt& ) const;

  // Y += AX ou X et Y sont des DoubleTab a 2 dimensions
  virtual DoubleTab& ajouter_multTab_(const DoubleTab& ,DoubleTab& ) const;

  // y += transposee(A) x
  virtual DoubleVect& ajouter_multvectT_(const DoubleVect& ,DoubleVect& ) const;
  ArrOfDouble& ajouter_multvectT_(const ArrOfDouble& ,ArrOfDouble&, ArrOfInt& ) const;

  // A= creat_transposee(B)
  virtual Matrice_Morse& transpose(const Matrice_Morse& a);

  // A=x*A (x vecteur diag)
  virtual Matrice_Morse& diagmulmat(const DoubleVect& x);

  //recupere la partie sup de la matrice et la stocke dans celle-ci
  virtual Matrice_Morse& partie_sup(const Matrice_Morse& a);

  // initialisation a la matrice unite
  void unite();


  IntVect tab1_;
  IntVect tab2_;
  DoubleVect coeff_;
  int m_;
  int symetrique_; // Pour inliner operator()(i,j) afin d'optimiser
  void formeC() ;
  void formeF() ;

  bool check_morse_matrix_structure( void ) const;
  bool check_sorted_morse_matrix_structure( void ) const;
  void assert_check_morse_matrix_structure( void ) const;
  void assert_check_sorted_morse_matrix_structure( void ) const;

  mutable int morse_matrix_structure_has_changed_; // Flag if matrix structure changes

private :
  double zero_;
};

int Matrice_Morse_test();

inline double Matrice_Morse::operator()(int i, int j) const
{
  assert( (symetrique_==0 && que_suis_je()=="Matrice_Morse")
          || (symetrique_==1 && que_suis_je()=="Matrice_Morse_Sym")
          || (symetrique_==2 && que_suis_je()=="Matrice_Morse_Diag") );
  if ((symetrique_==1) && ((j-i)<0)) std::swap(i,j);
  int k1=tab1_[i]-1;
  int k2=tab1_[i+1]-1;
  for (int k=k1; k<k2; k++)
    if (tab2_[k]-1 == j) return(coeff_[k]);
  // Si coefficient non trouve c'est qu'il est nul:
  return(0);
}

inline double& Matrice_Morse::operator()(int i, int j)
{
  assert( (symetrique_==0 && que_suis_je()=="Matrice_Morse")
          || (symetrique_==1 && que_suis_je()=="Matrice_Morse_Sym")
          || (symetrique_==2 && que_suis_je()=="Matrice_Morse_Diag") );
  //if (symetrique_==1 && j<i) std::swap(i,j); // Do not use, possible error during compile: "signed overflow does not occur when assuming that (X + c) < X is always false"
  if ((symetrique_==1) && ((j-i)<0)) std::swap(i,j);
  int k1=tab1_[i]-1;
  int k2=tab1_[i+1]-1;
  for (int k=k1; k<k2; k++)
    if (tab2_[k]-1 == j) return(coeff_[k]);
  if (symetrique_==2) return zero_; // Pour Matrice_Morse_Diag, on ne verifie pas si la case est definie et l'on renvoie 0
#ifndef NDEBUG
  // Uniquement en debug afin de permettre l'inline en optimise
  Cerr << "i or j are not suitable " << finl;
  Cerr << "i=" << i << finl;
  Cerr << "j=" << j << finl;
  Cerr << "n_lignes=" << nb_lignes() << finl;
  Cerr << "n_colonnes=" << nb_colonnes() << finl;
#endif
  // This message happens when you try to fill a coefficient in a matrix whereas is was not scheduled
  // If it is a symmetric matrix, it -may- be a parallelism default. Check it by running a PETSc solver
  // in debug mode: there is a test to check the parallelism of the symmetric matrix...
  Cerr << "Error Matrice_Morse::operator("<< i << "," << j << ") not defined!" << finl;
  exit();
  return coeff_[0];     // On ne passe jamais ici
}
#endif
