/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef IJK_Field_template_TPP_H
#define IJK_Field_template_TPP_H

#include <IJK_communications.h>
#include <IJK_Splitting.h>


template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::exchange_data(int pe_send_, /* processor to send to */
                                                             int is, int js, int ks, /* ijk coordinates of first data to send */
                                                             int pe_recv_, /* processor to recv from */
                                                             int ir, int jr, int kr, /* ijk coordinates of first data to recv */
                                                             int isz, int jsz, int ksz, /* size of block data to send/recv */
                                                             double offset, double jump_i, int nb_ghost)  /* decallage a appliquer pour la condition de shear periodique*/
{

  if (pe_send_ == Process::me() && pe_recv_ == Process::me())
    {
      // Self (periodicity on same processor)
      _TYPE_ *dest = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::data().addr();

      // for classical variable --> just a copy
      if (offset == 0.)
        {
          for (int i = 0; i < isz; i++)
            for (int k = 0; k < ksz; k++)
              for (int j = 0; j < jsz; j++)
                dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(i + ir , j + jr , k + kr)] = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(i + is, j + js, k + ks);

        }
      // for physical properties in case of shear periodicity (zmax)--> ghost value reconstructed from phase indicator function
      else if (monofluide_variable_==2 && offset <0.)
        {
          for (int i = 0; i < isz; i++)
            for (int k = 0; k < ksz; k++)
              for (int j = 0; j < jsz; j++)
                dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(i + ir , j + jr , k + kr)]=(_TYPE_) indicatrice_ghost_zmax_(i + ir , j + jr , k+2)*(_TYPE_)rho_l_+((_TYPE_)1.-(_TYPE_) indicatrice_ghost_zmax_(i + ir , j + jr , k+2))*(_TYPE_)rho_v_;


        }
      // for physical properties in case of shear periodicity (zmin)--> ghost value reconstructed from phase indicator function
      else if (monofluide_variable_==2 && offset >0.)
        {
          for (int i = 0; i < isz; i++)
            for (int k = 0; k < ksz; k++)
              for (int j = 0; j < jsz; j++)
                dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(i + ir , j + jr , k + kr)]=(_TYPE_) indicatrice_ghost_zmin_(i + ir , j + jr , k+2-nb_ghost)*(_TYPE_)rho_l_+((_TYPE_)1.-(_TYPE_) indicatrice_ghost_zmin_(i + ir  , j + jr , k+2-nb_ghost))*(_TYPE_)rho_v_;

        }
      // for all other variable in case of shear periodicity --> interpolation + jump at the z-boundary for velocity field
      else
        {
          for (int i = 0; i < isz; i++)
            {
              prepare_interpolation_for_shear_periodicity((int) round((double) i + (double) is +  offset), ((double) i + (double) is +  offset), isz);
              for (int k = 0; k < ksz; k++)
                for (int j = 0; j < jsz; j++)
                  dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(i + ir , j + jr , k + kr)] = interpolation_for_shear_periodicity_IJK_Field(j + js, k + ks) + (_TYPE_) jump_i ;
            }
        }

      return;
    }
  const int data_size = isz * jsz * ksz;
  const int data_size_other_buf = 1;
  const int type_size = sizeof(_TYPE_);
  const int double_size = sizeof(double);
  const int int_size = sizeof(int);
  _TYPE_ *send_buffer = 0;
  _TYPE_ *recv_buffer = 0;
  double *send_buffer_offset = 0;
  double *recv_buffer_offset = 0;
  int *send_buffer_nb_ghost = 0;
  int *recv_buffer_nb_ghost = 0;

  if (pe_send_ >= 0)
    {
      send_buffer = new _TYPE_[data_size];
      _TYPE_ *buf = send_buffer;
      send_buffer_offset = new double[data_size_other_buf];
      double *buf_offset = send_buffer_offset;
      send_buffer_nb_ghost = new int[data_size_other_buf];
      int *buf_nb_ghost = send_buffer_nb_ghost;
      *buf_offset=offset;
      *buf_nb_ghost=nb_ghost;
      // for classical variable --> just a copy
      if (offset==0.)
        {
          for (int i = 0; i < isz; i++)
            for (int k = 0; k < ksz; k++)
              for (int j = 0; j < jsz; j++, buf++)
                *buf= IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(i + is, j + js, k + ks);
        }
      // for physical properties in case of shear periodicity --> ghost value reconstructed from phase indicator function
      else if (monofluide_variable_==2)
        {
          for (int i = 0; i < isz; i++)
            for (int k = 0; k < ksz; k++)
              for (int j = 0; j < jsz; j++, buf++)
                *buf= (_TYPE_) 0.;
        }
      // for all other variable in case of shear periodicity --> interpolation + jump at the z-boundary for velocity field
      else
        {
          for (int i = 0; i < isz; i++)
            {
              prepare_interpolation_for_shear_periodicity((int) round((double) i + (double) is +  offset), ((double) i + (double) is +  offset), isz);
              for (int k = 0; k < ksz; k++)
                for (int j = 0; j < jsz; j++, buf++)
                  *buf= interpolation_for_shear_periodicity_IJK_Field(j + js, k + ks) + (_TYPE_) jump_i;
            }
        }
    }

  if (pe_recv_ >= 0)
    {
      recv_buffer = new _TYPE_[data_size];
      recv_buffer_offset = new double[data_size_other_buf];
      recv_buffer_nb_ghost = new int[data_size_other_buf];
    }
  ::envoyer_recevoir(send_buffer, data_size * type_size, pe_send_, recv_buffer, data_size * type_size, pe_recv_);
  ::envoyer_recevoir(send_buffer_offset, data_size_other_buf * double_size, pe_send_, recv_buffer_offset, data_size_other_buf * double_size, pe_recv_);
  ::envoyer_recevoir(send_buffer_nb_ghost, data_size_other_buf * int_size, pe_send_, recv_buffer_nb_ghost, data_size_other_buf * int_size, pe_recv_);

  if (pe_recv_ >= 0)
    {
      _TYPE_ *buf = recv_buffer;
      double *buf_offset = recv_buffer_offset;
      int *buf_nb_ghost= recv_buffer_nb_ghost;
      _TYPE_ *dest = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::data().addr();
      // for classical variable --> just a copy
      if (*buf_offset == 0.)
        {
          for (int i = 0; i < isz; i++)
            for (int k = 0; k < ksz; k++)
              for (int j = 0; j < jsz; j++, buf++)
                dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(ir + i, jr + j, kr + k)] = *buf;
        }
      // for physical properties in case of shear periodicity (zmax) --> ghost value reconstructed from phase indicator function
      else if (monofluide_variable_==2 && *buf_offset <0.)
        {
          for (int i = 0; i < isz; i++)
            for (int k = 0; k < ksz; k++)
              for (int j = 0; j < jsz; j++, buf++)
                dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(ir + i, jr + j, kr + k)]=(_TYPE_) indicatrice_ghost_zmax_(ir + i  , jr + j  , k+2)*(_TYPE_)rho_l_+((_TYPE_)1.-(_TYPE_) indicatrice_ghost_zmax_(ir + i  , jr + j  , k+2))*(_TYPE_)rho_v_;

        }
      // for physical properties in case of shear periodicity (zmin) --> ghost value reconstructed from phase indicator function
      else if (monofluide_variable_==2 && *buf_offset >0.)
        {
          for (int i = 0; i < isz; i++)
            for (int k = 0; k < ksz; k++)
              for (int j = 0; j < jsz; j++, buf++)
                dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(ir + i, jr + j, kr + k)]=(_TYPE_) indicatrice_ghost_zmin_(ir + i  , jr + j  , k+2-*buf_nb_ghost)*(_TYPE_)rho_l_+((_TYPE_)1.-(_TYPE_) indicatrice_ghost_zmin_(ir + i  , jr + j  , k+2-*buf_nb_ghost))*(_TYPE_)rho_v_;

        }
      // for all other variable in case of shear periodicity --> interpolation + jump at the z-boundary for velocity field
      else
        {
          for (int i = 0; i < isz; i++)
            for (int k = 0; k < ksz; k++)
              for (int j = 0; j < jsz; j++, buf++)
                dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(ir + i, jr + j, kr + k)] = *buf;
        }

    }

  delete[] send_buffer;
  delete[] recv_buffer;
  delete[] send_buffer_offset;
  delete[] recv_buffer_offset;
  delete[] send_buffer_nb_ghost;
  delete[] recv_buffer_nb_ghost;
}

/*! @brief Exchange data over "ghost" number of cells.
 *
 */
template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::echange_espace_virtuel(int le_ghost)
{
  statistiques().begin_count(echange_vect_counter_);
  assert(le_ghost <= (IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::ghost()));
  const IJK_Splitting& splitting = splitting_ref_.valeur();
  int pe_imin_ = splitting.get_neighbour_processor(0, 0);
  int pe_imax_ = splitting.get_neighbour_processor(1, 0);
  int pe_jmin_ = splitting.get_neighbour_processor(0, 1);
  int pe_jmax_ = splitting.get_neighbour_processor(1, 1);

  int pe_kmin_ = splitting.get_neighbour_processor(0, 2);
  int pe_kmax_ = splitting.get_neighbour_processor(1, 2);
  int z_index = splitting.get_local_slice_index(2);
  int z_index_min = 0;
  int z_index_max = splitting.get_nprocessor_per_direction(2) - 1;

  const int nii = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::ni();
  const int njj = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nj();
  const int nkk = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nk();

  if (IJK_Splitting::defilement_ == 1)
    {
      // calculation of the offset due to shear periodicity between zmin and zmax
      double Lx =  splitting.get_grid_geometry().get_domain_length(0);
      IJK_Splitting::Lx_for_shear_perio = Lx;
      double DX = Lx/nii ;
      double Shear_x_time = IJK_Splitting::shear_x_time_;
      double offset_i = Shear_x_time/DX;

      exchange_data(pe_imin_, 0, -le_ghost, -le_ghost, pe_imax_, nii, -le_ghost, -le_ghost, le_ghost, njj + 2*le_ghost, nkk+ 2*le_ghost);
      exchange_data(pe_imax_, nii - le_ghost, -le_ghost, -le_ghost, pe_imin_, -le_ghost, -le_ghost, -le_ghost, le_ghost, njj + 2*le_ghost, nkk+ 2*le_ghost);
      exchange_data(pe_jmin_, 0, 0, -le_ghost, pe_jmax_, 0, njj, -le_ghost, nii , le_ghost, nkk + 2 * le_ghost);
      exchange_data(pe_jmax_, 0, njj - le_ghost, -le_ghost, pe_jmin_, 0, -le_ghost, -le_ghost, nii , le_ghost, nkk + 2 * le_ghost);

      // in shear periodicity, the block exchange in z must have a size [nii, njj, ghost] to deal more easily with the Lx modulo behavior
      // On x, exchange a block [ghost, njj + 2*ghost, nkk+ 2*ghost]
      // On z, exchange a block [nii, njj, ghost]
      // On y, exchange a block [nii, ghost, nkk+ 2*ghost]
      if (z_index != z_index_min)
        exchange_data(pe_kmin_, 0, 0, 0, pe_kmax_, 0, 0, nkk, nii , njj , le_ghost);
      else
        exchange_data(pe_kmin_, 0, 0, 0, pe_kmax_, 0, 0, nkk, nii, njj, le_ghost, -offset_i, DU_, le_ghost);

      if (z_index != z_index_max)
        exchange_data(pe_kmax_, 0, 0, nkk - le_ghost, pe_kmin_, 0, 0, -le_ghost, nii, njj, le_ghost);
      else
        exchange_data(pe_kmax_, 0, 0, nkk - le_ghost, pe_kmin_, 0, 0, -le_ghost, nii, njj, le_ghost, offset_i, -DU_, le_ghost);
    }
  else
    {
      // send left layer of real cells to right layer of virtual cells
      exchange_data(pe_imin_, 0, 0, 0, pe_imax_, nii, 0, 0, le_ghost, njj, nkk); /* size of block data to send */
      // send right real cells to left virtual cells
      exchange_data(pe_imax_, nii - le_ghost, 0, 0, pe_imin_, -le_ghost, 0, 0, le_ghost, njj, nkk);
      exchange_data(pe_jmin_, -le_ghost, 0, 0, pe_jmax_, -le_ghost, njj, 0, nii + 2 * le_ghost, le_ghost, nkk);
      exchange_data(pe_jmax_, -le_ghost, njj - le_ghost, 0, pe_jmin_, -le_ghost, -le_ghost, 0, nii + 2 * le_ghost, le_ghost, nkk);
      exchange_data(pe_kmin_, -le_ghost, -le_ghost, 0, pe_kmax_, -le_ghost, -le_ghost, nkk, nii + 2 * le_ghost, njj + 2 * le_ghost, le_ghost);
      exchange_data(pe_kmax_, -le_ghost, -le_ghost, nkk - le_ghost, pe_kmin_, -le_ghost, -le_ghost, -le_ghost, nii + 2 * le_ghost, njj + 2 * le_ghost, le_ghost);
    }
  statistiques().end_count(echange_vect_counter_);
}


template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::prepare_interpolation_for_shear_periodicity(const int send_i, const double istmp, const int real_size_i)
{
  // prepare the shear-periodicity interpolation at a given i elevation.
  // shear periodicity is only available on the component DU/DZ
  int nb_points = order_interpolation_+1;

  if (nb_points==2)
    {
      x_[0] = (int) floor(istmp);
      x_[1] = (int) floor(istmp)+1;
    }
  else if(nb_points==3)
    {
      x_[0] = send_i-1;
      x_[1] = send_i;
      x_[2] = send_i+1;
    }
  else if(nb_points==5)
    {
      x_[0] = send_i-2;
      x_[1] = send_i-1;
      x_[2] = send_i;
      x_[3] = send_i+1;
      x_[4] = send_i+2;
    }
  else if(nb_points==7)
    {
      x_[0] = send_i-3;
      x_[1] = send_i-2;
      x_[2] = send_i-1;
      x_[3] = send_i;
      x_[4] = send_i+1;
      x_[5] = send_i+2;
      x_[6] = send_i+3;
    }

  for (int pt = 0; pt < nb_points ; pt++)
    {
      send_i_real_[pt]=(x_[pt] % real_size_i + real_size_i) % real_size_i;
      denum_[pt] = 1.;
      res_[pt] = 1.;
      for (int pt_autre = 0; pt_autre < nb_points ; pt_autre++)
        {
          if (pt_autre!=pt)
            {
              denum_[pt] *= (double)(x_[pt] - x_[pt_autre]);
              res_[pt] *= (istmp - x_[pt_autre]);
            }
        }
    }

  return;
}

template<typename _TYPE_, typename _TYPE_ARRAY_>
_TYPE_ IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::interpolation_for_shear_periodicity_IJK_Field(const int send_j, const int send_k)
{
  // execute the shear-periodicity interpolation.
  // prepare_interpolation_for_shear_periodicity has to be called before
  _TYPE_ resu = (_TYPE_)0. ;
  int nb_points = order_interpolation_+1;
  for (int pt = 0; pt < nb_points ; pt++)
    {
      resu += (_TYPE_)(res_[pt]/denum_[pt]*IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(send_i_real_[pt], send_j, send_k));
    }

  return resu;
}


template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::redistribute_with_shear_domain_ft(const IJK_Field_double& input, double DU_perio, const int ft_extension)
{
  // To shift the velocity field in FT domain after redistribute operator from NS domain
  // According to the shear periodicity condition

  _TYPE_ *dest = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::data().addr();
  IJK_Splitting splitting_ns = input.get_splitting();
  IJK_Splitting& splitting_ft = splitting_ref_.valeur();
  double Lx =  splitting_ns.get_grid_geometry().get_domain_length(0);
  int ni = input.ni();
  double DX = Lx/ni ;
  const int nii = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::ni();
  const int njj = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nj();
  const int nkk = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nk();
  int last_global_k = splitting_ns.get_nb_items_global(IJK_Splitting::ELEM, 2)-1;

  ArrOfDouble output_tmp;
  output_tmp.resize_array(nii);
  for (int k = 0; k < nkk; k++ )
    {
      int k_reel = k + splitting_ft.get_offset_local(2) - ft_extension;
      if (k_reel>=0 && k_reel<= last_global_k)
        continue;

      for (int j = 0; j < njj; j++ )
        {
          for (int i = 0; i < nii; i++ )
            {
              if (k_reel<0)
                {
                  double istmp = i+IJK_Splitting::shear_x_time_/DX;
                  prepare_interpolation_for_shear_periodicity((int) round(istmp), istmp, ni);
                  output_tmp[i]=interpolation_for_shear_periodicity_IJK_Field(j, k)-DU_perio;
                }
              else if (k_reel>last_global_k)
                {
                  double istmp = i-IJK_Splitting::shear_x_time_/DX;
                  prepare_interpolation_for_shear_periodicity((int) round(istmp), istmp, ni);
                  output_tmp[i]=interpolation_for_shear_periodicity_IJK_Field(j, k)+DU_perio;
                }
              else
                {
                  output_tmp[i]=IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(i, j, k);
                }
            }
          for (int i = 0; i < nii; i++ )
            {
              dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(i, j, k)]=output_tmp[i];
            }
        }
    }
  return;
}

// Initializes the field and allocates memory
// splitting: reference to the geometry of the IJK mesh and how the mesh is split on processors.
//   The field stores a reference to this IJK_Splitting object so do not delete it.
// loc: localisation of the field (elements, nodes, faces in direction i, j, or k)
//   The number of "real" items in each direction (returned by the ni(), nj() or nk() method) is obtained from
//   the IJK_Splitting object. Warning: on a processor that is in the middle of the mesh, the nodes on the
//   right of the rightmost real element are not real, they are virtual, values are copied from the neigbour
//   processor.
// ghost_size: number of ghost layers to allocate. When an exchange of ghost cells data is requested, a smaller
//   number of layers can be requested if all layers are not needed by the following operations
// additional_k_layers: allocates more layers of cells in the k direction for use with the
//   shift_k_origin() method (optimization trick used by the temporally blocked algorithms in the multigrid
//   solver
// nb_compo: number of components of the field. Warning: you cannot allocate the velocity field at faces
//   with nb_compo=3: numbers of faces in each direction differ.
//   Also, components are not grouped by node but stored by layers in k. nb_compo>1 is essentially used
//   in the multigrid solver to optimize memory accesses to the components of the matrix.
template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::allocate(const IJK_Splitting& splitting, IJK_Splitting::Localisation loc, int ghost_size, int additional_k_layers, int ncompo, bool external_storage, int type, double rov, double rol, int use_inv_rho_in_pressure_solver)
{
  const int ni_local = splitting.get_nb_items_local(loc, 0);
  const int nj_local = splitting.get_nb_items_local(loc, 1);
  const int nk_local = splitting.get_nb_items_local(loc, 2);
  order_interpolation_ = IJK_Splitting::order_interpolation_poisson_solver_;
  x_.resize_array(order_interpolation_+1);
  denum_.resize_array(order_interpolation_+1);
  res_.resize_array(order_interpolation_+1);
  send_i_real_.resize_array(order_interpolation_+1);
  monofluide_variable_ = type;
  use_inv_rho_in_pressure_solver_= use_inv_rho_in_pressure_solver;
  IJK_Field_local_template<_TYPE_, _TYPE_ARRAY_>::allocate(ni_local, nj_local, nk_local, ghost_size, additional_k_layers, ncompo);

  // monofluide_variable_==1 is for pressure field in case of shear periodicity
  // monofluide_variable_==2 is for physical proerties (rho, mu, nu etc.) in case of shear periodicity
  if (monofluide_variable_==1 || monofluide_variable_==2)
    {
      indicatrice_ghost_zmin_.allocate(ni_local, nj_local, 4, ghost_size, 0, ncompo);
      indicatrice_ghost_zmax_.allocate(ni_local, nj_local, 4, ghost_size, 0, ncompo);
      rho_v_ = rov;
      rho_l_ = rol;
    }

  splitting_ref_ = splitting;
  localisation_ = loc;
}


#endif /* IJK_Field_template_TPP_H */
