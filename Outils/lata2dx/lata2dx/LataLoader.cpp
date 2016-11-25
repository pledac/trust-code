
#include <LataLoader.h>

#include <string>
#include <fstream>
#include <iostream>

#include <MEDCouplingUMesh.hxx>
#include <MEDCouplingCMesh.hxx>
#include <MEDCouplingMemArray.hxx>
#include <MEDCouplingFieldDouble.hxx>
#include <MEDCouplingRefCountObject.hxx>


#include <LmlReader.h>
#include <LataJournal.h>


using namespace ParaMEDMEM;


// ****************************************************************************
//  Method: avtlata constructor
//
//
// ****************************************************************************

LataLoader::LataLoader(const char *file) throw (LataDBError)
  
{
  filename=Nom(file);
  cerr << "LataLoader constructor " << filename << endl;
  try {
    set_Journal_level(0);

    LataOptions opt;
    LataOptions::extract_path_basename(filename, opt.path_prefix, opt.basename);
    opt.dual_mesh = true;
    opt.faces_mesh = true;
    opt.regularize = 2;
    opt.regularize_tolerance = 1e-7;
    opt.user_fields_=true;
    read_any_format_options(filename, opt);
    cerr << "LataLoader: initializing filter" << endl;
    // Read the source file to the lata database
    read_any_format(filename, opt.path_prefix, lata_db_);
    filter_.initialize(opt, lata_db_);
  }
  catch (LataDBError err) {
    cerr << "Error in LataFilter::initialize " << filename << " " << err.describe() << endl;
    throw err;
  }
  PopulateDatabaseMetaData(0);
}

LataLoader::~LataLoader()
{
}

// ****************************************************************************
//  Method: avtEMSTDFileFormat::GetNTimesteps
//
//  Purpose:
//      Tells the rest of the code how many timesteps there are in this file.
//
//  Programmer: fauchet -- generated by xml2avt
//
// ****************************************************************************

int
LataLoader::GetNTimesteps(void)
{
  int n;
  try {
    n = filter_.get_nb_timesteps();
    // Timestep 0 contains global definitions.
    // If we have "real" timesteps, do not show timestep 0
    if (n > 1)
      n--;
  }
  catch (LataDBError err) {
    cerr << "Error in getntimesteps " << filename << " " << err.describe() << endl;
    throw;
  }
  return n;
}

void LataLoader::GetTimes(std::vector<double>& times) 
{ 
  int n;
  try {
    n = filter_.get_nb_timesteps();
    if (n == 1)
      times.push_back(0.);
    else
      for (int i = 1; i < n; i++)
        times.push_back(filter_.get_timestep(i));
  }
  catch (LataDBError err) {
    cerr << "Error in gettimes " << filename << " " << err.describe() << endl;
    throw;
  }
  return;
}

// ****************************************************************************
//  Method: LataLoader::FreeUpResources
//
//  Purpose:
//      When VisIt is done focusing on a particular timestep, it asks that
//      timestep to free up any resources (memory, file descriptors) that
//      it has associated with it.  This method is the mechanism for doing
//      that.
//
//  Programmer: fauchet -- generated by xml2avt
//
// ****************************************************************************

void
LataLoader::FreeUpResources(void)
{
}


// ****************************************************************************
//  Method: LataLoader::PopulateDatabaseMetaData
//
//  Purpose:
//      This database meta-data object is like a table of contents for the
//      file.  By populating it, you are telling the rest of VisIt what
//      information it can request from you.
//
//  Programmer: fauchet -- generated by xml2avt
//
// ****************************************************************************

void LataLoader::PopulateDatabaseMetaData( int timeState)
{
  try {
  cerr << "LataLoader::PopulateDatabaseMetaData : " 
         << filename << " " << timeState << endl;

  //const char *suffix_vector_names[] = { "_X", "_Y", "_Z" };
  //const char *suffix_vector_expr[] = { "[0]", "[1]", "[2]" };

  const Noms geoms = filter_.get_exportable_geometry_names();

  for (int i_geom = 0; i_geom < geoms.size(); i_geom++) {
    cerr << " Domain : " << geoms[i_geom] << endl;
    const LataGeometryMetaData data = filter_.get_geometry_metadata(geoms[i_geom]);

    /*
    avtMeshType mt = AVT_UNSTRUCTURED_MESH;
    
    if ((data.internal_name_.finit_par("IJK")) 
	    || (data.internal_name_.finit_par("IJK_dual")))
      {
        mt = AVT_RECTILINEAR_MESH;
      }
    int block_origin = 0;
    int topo_dim;
    switch(data.element_type_) {
    case Domain::point:     topo_dim = 0; mt = AVT_POINT_MESH; break;
    case Domain::line:      topo_dim = 1; break;
    case Domain::triangle: 
    case Domain::quadri:    topo_dim = 2; break;
    case Domain::tetra:
    case Domain::prism6:
    case Domain::polyedre:
    case Domain::hexa:      topo_dim = 3; break;
    default:
      cerr << "LataLoader::PopulateDatabaseMetaData error: unknown element type" << endl;
      throw;
    }
    double *extents = NULL;
    */
    int mesh_faces=0;
    if  (data.internal_name_.finit_par("_centerfaces"))
      {
        //cerr<<"la "<<data.internal_name_<<endl;
        mesh_faces=1;
      }
    
    const std::string geom_name(data.displayed_name_);
    //  AddMeshToMetaData(md, geom_name, mt, extents, data.nblocks_, block_origin,                      data.dimension_, topo_dim);
    mesh_username_.add(data.displayed_name_);
    mesh_latafilter_name_.add(data.internal_name_);
    
    Field_UNames fields = filter_.get_exportable_field_unames(geoms[i_geom]);

    for (int i_field = 0; i_field < fields.size(); i_field++) {
      const LataFieldMetaData data2 = filter_.get_field_metadata(fields[i_field]);
      
      

      // Take localisation of source field
      Nom loc = data2.source_localisation_;
      std::string varname(data2.name_);
      varname += "_";
      varname += loc;
      varname += "_";
      varname += geom_name;
      if (data2.nb_components_ == 1) {
        // Scalar field
        // We append the geometry name to the component name:
        register_fieldname(varname.c_str(), fields[i_field], 0);
	//      if (mesh_faces==0)           AddScalarVarToMetaData(md, varname, geom_name, cent);
      } else if (data2.is_vector_ && data2.nb_components_ == data.dimension_) {
        // Vector field
        register_fieldname(varname.c_str(), fields[i_field], -1);
        //AddVectorVarToMetaData(md, varname, geom_name, cent, data2.nb_components_);
       
      } else {
        // Multiscalar field
        // I chose to postfix the varname with the component name, perhaps not the best choice.
        if (mesh_faces==0)
          {
        for (entier i_compo = 0; i_compo < data2.nb_components_; i_compo++) {
          std::string varname2(data2.name_);
          varname2 += "_";
          if (data2.component_names_.size() == data2.nb_components_) {
            varname2 += data2.component_names_[i_compo];
          } else {
            Nom n(i_compo);
            varname2 += n;
          }
          varname2 += "_";
          varname2 += loc;
          varname2 += "_";
          varname2 += geom_name;
          register_fieldname(varname2.c_str(), fields[i_field], i_compo);
	  //   AddScalarVarToMetaData(md, varname2, geom_name, cent);
        }
          }
      }
    }
  }
  cerr << "End LataLoader::PopulateDatabaseMetaData" << endl;
  }
  catch (LataDBError err) {
    cerr << "Error in PopulateDatabaseMetaData " << err.describe() << endl;
    throw;
  }
}

void
LataLoader::register_fieldname(const char *visit_name, const Field_UName & uname, int component)
{
  if (field_username_.rang(visit_name) >= 0) {
    cerr << "Error in LataLoader::register_fieldname: duplicate field name " << visit_name << endl;
    cerr << "Ignoring field" << endl;
    return;
  }
  field_username_.add(visit_name);
  field_uname_.add(uname);
  field_component_.add(component);
}

void
LataLoader::register_meshname(const char *visit_name, const char *latafilter_name)
{
  if (mesh_username_.rang(visit_name) >= 0) {
    cerr << "Error in LataLoader::register_meshname: duplicate name " << visit_name << endl;
    cerr << "Ignoring mesh" << endl;
    return;    
  }
  mesh_username_.add(visit_name);
  mesh_latafilter_name_.add(latafilter_name);
}

// ****************************************************************************
//  Method: LataLoader::GetMesh
//
//  Purpose:
//      Gets the mesh associated with this file.  The mesh is returned as a
//      derived type of vtkDataSet (ie vtkRectilinearGrid, vtkStructuredGrid,
//      vtkUnstructuredGrid, etc).
//
//  Arguments:
//      timestate   The index of the timestate.  If GetNTimesteps returned
//                  'N' time steps, this is guaranteed to be between 0 and N-1.
//      domain      The index of the domain.  If there are NDomains, this
//                  value is guaranteed to be between 0 and NDomains-1,
//                  regardless of block origin.
//      meshname    The name of the mesh of interest.  This can be ignored if
//                  there is only one mesh.
//
//  Programmer: fauchet -- generated by xml2avt
//
// ****************************************************************************

MEDCouplingMesh *
LataLoader::GetMesh(const char *meshname, int timestate, int block)
{
  MEDCouplingMesh *return_value = 0;
  try {
  cerr << " LataLoader::GetMesh ts=" << timestate 
         << " block=" << block 
         << " meshname=" << meshname << endl;

  // We have real timesteps in the database, add one to timestep index:
  if (filter_.get_nb_timesteps() > 1)
    timestate++;
  int index=-1;
  for (int i=0;i<mesh_username_.size();i++)
    {
      if (Motcle(mesh_username_[i])==(meshname)) 
	{
	  index=i;
	  break;
	}
    }
  if (index < 0) {
    cerr << "internal error in LataLoader::GetMesh: name " << meshname << " not found" << endl;
    throw;
  }
  Domain_Id id(mesh_latafilter_name_[index], timestate, block);
  const Domain & geometry = filter_.get_geometry(id);

  const DomainUnstructured * geom_ptr = dynamic_cast<const DomainUnstructured*>(&geometry);
  const DomainIJK          * ijk_ptr = dynamic_cast<const DomainIJK*>(&geometry);

  if (geom_ptr) {
    const DomainUnstructured & geom = *geom_ptr;

    const FloatTab & pos = geom.nodes_;
    const int nnodes = pos.dimension(0);
    //   const int dim3 = pos.dimension(1) == 3;
    const int dim= pos.dimension(1);
    //ugrid->setMeshDimension(dim);
    MEDCouplingUMesh *ugrid = MEDCouplingUMesh::New(meshname,dim);
    DataArrayDouble *points = DataArrayDouble::New();
    points->alloc(nnodes,dim);
    double* pts = points->getPointer();
    std::copy(pos.addr(),pos.addr()+pos.size_array(),pts);
    ugrid->setCoords(points);
    points->decrRef();
    
    const IntTab & conn = geom.elements_;
    const int ncells = conn.dimension(0);
    int nverts = conn.dimension(1);
    
    INTERP_KERNEL::NormalizedCellType type_cell;
    switch (geom.elt_type_) {
    case Domain::point:
      type_cell=INTERP_KERNEL::NORM_POINT1; 
      if (ncells == 0) 
        nverts = 1;
      break;
    case Domain::line:
      type_cell=INTERP_KERNEL::NORM_SEG2;
      ugrid->setMeshDimension(1);
      break;
    case Domain::triangle:
      type_cell=INTERP_KERNEL::NORM_TRI3;
      ugrid->setMeshDimension(2);
      break;
    case Domain::quadri:
      type_cell=INTERP_KERNEL::NORM_QUAD4;
      ugrid->setMeshDimension(2);
      break;
    case Domain::tetra:
      type_cell=INTERP_KERNEL::NORM_TETRA4;
      break;
    case Domain::prism6:
      type_cell=INTERP_KERNEL::NORM_PENTA6;
      break;
    case Domain::hexa:
      type_cell=INTERP_KERNEL::NORM_HEXA8;
      break;
    case Domain::polyedre:
       type_cell=INTERP_KERNEL::NORM_POLYHED;
       break;
    default:
      type_cell=INTERP_KERNEL::NORM_POLYHED;
      cerr<<"LataLoader::GetMesh unknown elt type "<<endl;
      throw;
      break;
    }
    int *verts = new int[nverts];
    if (type_cell == INTERP_KERNEL::NORM_POINT1 && ncells == 0) {
      throw;
      /*
      // Cells are implicit. Create them:
      ugrid->Allocate(nnodes);
      for (i = 0; i < nnodes; i++) {
        verts[0] = i;
        ugrid->insertNextCell(type_cell, nverts, verts);
      }
      */
    } else {
      ugrid->allocateCells(ncells);
      for (int i = 0; i < ncells; i++) {
	if ((1)&&(type_cell==INTERP_KERNEL::NORM_TETRA4))
	  {
	    const int som_Z=conn(i,0);
	    const int som_A=conn(i,1);
	    const int som_B=conn(i,2);
	    const int som_C=conn(i,3);
	    int test=1;
	    if (1)
	      {
		float ZA[3], ZB[3], ZC[3], pdtvect[3], pdtscal;
		entier k;
		for (k=0 ; k<3 ; k++) {
		  ZA[k] = pos(som_A,k) - pos(som_Z,k);
		  ZB[k] = pos(som_B,k) - pos(som_Z,k);
		  ZC[k] = pos(som_C,k) - pos(som_Z,k);
		}
		
		//calcul pdt vect ZAxZB
		pdtvect[0] = ZA[1]*ZB[2] - ZA[2]*ZB[1];
		pdtvect[1] = ZA[2]*ZB[0] - ZA[0]*ZB[2];
		pdtvect[2] = ZA[0]*ZB[1] - ZA[1]*ZB[0];
		pdtscal = 0.;
		for (k=0 ; k<3 ; k++) {
		  pdtscal += pdtvect[k] * ZC[k];
		}
		if (pdtscal<0)
		  test=-1;

	      }
	    if (test==1)
	      {
		verts[0]=som_Z;
		verts[1]=som_A;
		verts[2]=som_B;
		verts[3]=som_C;
	      }
	    else
	      {
		verts[0]=som_Z;
		verts[1]=som_B;
		verts[2]=som_A;
		verts[3]=som_C;
		
	      }
	  }
        else if (type_cell==INTERP_KERNEL::NORM_QUAD4) {
          // Nodes order is different in visit than in trio_u
          verts[0]=conn(i,0);
          verts[1]=conn(i,1);
          verts[2]=conn(i,3);
          verts[3]=conn(i,2);
        } else if (type_cell==INTERP_KERNEL::NORM_HEXA8) {
          // Nodes order is different in visit than in trio_u
          verts[0]=conn(i,0);
          verts[1]=conn(i,1);
          verts[2]=conn(i,3);
          verts[3]=conn(i,2);
          verts[4]=conn(i,4);
          verts[5]=conn(i,5);
          verts[6]=conn(i,7);
          verts[7]=conn(i,6);
        } else if (type_cell==INTERP_KERNEL::NORM_POLYHED) {
          int nverts_loc=nverts;
          for (int j = 0; j < nverts; j++) 
            {
              verts[j] = conn(i,j);
             
              if (verts[j]<=-1)
                {  
                  nverts_loc=j; 
                  break;
                }    
            }
          int  nb_som_max_to_regularize=0;
          if (filter_.get_options().regularize_polyedre!=0)
            {
              nb_som_max_to_regularize=8;
              if (filter_.get_options().regularize_polyedre==-1)
                  nb_som_max_to_regularize=32000;
            }
          if ((nb_som_max_to_regularize>=6) && (nverts_loc==6))
            ugrid->insertNextCell(INTERP_KERNEL::NORM_PENTA6, nverts_loc, verts);
          else if ((nb_som_max_to_regularize>=12)&&(nverts_loc==12))
	    throw;
            //ugrid->insertNextCell(INTERP_KERNEL::NORM_OCTA12, nverts_loc, verts);
          else if ((nb_som_max_to_regularize>=8)&&(nverts_loc==8))
            {
                        // Nodes order is different in visit than in trio_u
              verts[0]=conn(i,0);
              verts[1]=conn(i,1);
              verts[2]=conn(i,3);
              verts[3]=conn(i,2);
              verts[4]=conn(i,4);
              verts[5]=conn(i,5);
              verts[6]=conn(i,7);
              verts[7]=conn(i,6);
              ugrid->insertNextCell(INTERP_KERNEL::NORM_HEXA8, nverts_loc, verts);
               
            }
          else
	    {
	      throw ;
	      ugrid->insertNextCell(type_cell, nverts_loc, verts);
	    }
        }
        else {
          for (int j = 0; j < nverts; j++) 
            verts[j] = conn(i,j);
        }
	if (type_cell!=INTERP_KERNEL::NORM_POLYHED)
          ugrid->insertNextCell(type_cell, nverts, verts);

      }
    }
    ugrid->finishInsertingCells();
    
    delete [] verts;
    verts = 0;
    /*
    // Declare ghost elements:
    const int n = geom.nb_virt_items(LataField_base::ELEM);
    if (n > 0) {
      unsigned char realVal = 0;
      unsigned char ghost   = 0; // Sera modifie par AddGhostZoneType
      avtGhostData::AddGhostZoneType(ghost, DUPLICATED_ZONE_INTERNAL_TO_PROBLEM);
      vtkUnsignedCharArray *ghostcells = vtkUnsignedCharArray::New();
      ghostcells->SetName("avtGhostZones");
      ghostcells->SetNumberOfTuples(ncells);
      unsigned char *dat = (unsigned char *) ghostcells->GetVoidPointer(0);
      for (i = 0; i < ncells - n; i++)
        dat[i] = realVal;
      for (i = ncells - n; i < ncells; i++)
        dat[i] = ghost;
      ugrid->GetCellData()->AddArray(ghostcells);
      ugrid->SetUpdateGhostLevel(0);
      ghostcells->Delete();
    }
*/
    return_value = ugrid;
    
  } else if (ijk_ptr) {
  //  throw "KKKK";
    const DomainIJK & geom = *ijk_ptr;

    // Maillage regulier : on transmet la grille ijk
    
    
  


    const int dim = geom.coord_.size();
    MEDCouplingCMesh *sgrid = MEDCouplingCMesh::New(meshname);
    ArrOfInt ncoord(3, 1);
    int i;
    for (i = 0; i < dim; i++) 
      ncoord[i] = geom.coord_[i].size_array();
    //    sgrid->SetDimensions(ncoord[0], ncoord[1], ncoord[2]);

    for (i = 0; i < 3; i++) {
      
     
      const int n = ncoord[i];
      /*
      vtkFloatArray *c;
      c = vtkFloatArray::New();
      c->SetNumberOfTuples(n);
      data = (float *) c->GetVoidPointer(0);
      */
      DataArrayDouble *c = DataArrayDouble::New();
      c->alloc(n);
      double* data = c->getPointer();
     
      if (i < dim) {
        const ArrOfFloat & coord = geom.coord_[i];
        for (int j = 0; j < n; j++)
          data[j] = coord[j];
      } else {
        data[0] = 0.;
      }
      sgrid->setCoordsAt(i,c);
      
      c->decrRef();
    }
    
#if 0
    // Create "invalid cells" data (GettingDataIntoVisit.pdf, page 136)
    // and "ghost cells"
    const int n = geom.invalid_connections_.size_array();
    if (n > 0 || geom.virtual_layer_begin_ || geom.virtual_layer_end_) {
      const int ncells = geom.nb_elements();
      unsigned char realVal = 0;
      unsigned char invalid = 0; // Sera modifie par AddGhostZoneType
      unsigned char ghost   = 0;
      avtGhostData::AddGhostZoneType(invalid, ZONE_NOT_APPLICABLE_TO_PROBLEM);
      //avtGhostData::AddGhostZoneType(invalid, ZONE_EXTERIOR_TO_PROBLEM);
      avtGhostData::AddGhostZoneType(ghost, DUPLICATED_ZONE_INTERNAL_TO_PROBLEM);
      vtkUnsignedCharArray *ghostcells = vtkUnsignedCharArray::New();
      ghostcells->SetName("avtGhostZones");
      ghostcells->SetNumberOfTuples(ncells);
      unsigned char *dat = (unsigned char *) ghostcells->GetVoidPointer(0);

      for (i = 0; i < ncells; i++)
          dat[i] = realVal;

      if (n > 0) {
        // invalid cells
        for (i = 0; i < ncells; i++) {
          if (geom.invalid_connections_[i])
            dat[i] = invalid;
        }
      }
      
      // ghost cells
      entier ij = 1;
      for (i = 0; i < dim-1; i++)
        ij *= ncoord[i]-1;
      if (geom.virtual_layer_begin_) {
        // first layer of cells is ghost
        for (i = 0; i < ij * geom.virtual_layer_begin_; i++)
          dat[i] += ghost;
      }
      if (geom.virtual_layer_end_) {
        // last layer of cells is ghost
        for (i = ncells - ij * geom.virtual_layer_end_; i < ncells; i++)
          dat[i] += ghost;
      }
      
      sgrid->GetCellData()->AddArray(ghostcells);
      sgrid->SetUpdateGhostLevel(0);
      ghostcells->Delete();
    }
#endif
   
    return_value = sgrid;
  } else {
    cerr << "Error in LataLoader::GetMesh: unknown geometry type" << endl;
    throw;
  }

  filter_.release_geometry(geometry);
  }
  catch (LataDBError err) {
    cerr << "Error in getmesh " << timestate << " " << block << " " << meshname << " " << err.describe() << endl;
    throw;
  }

  return return_value;
}


// ****************************************************************************
//  Method: LataLoader::GetVar
//
//  Purpose:
//      Gets a scalar variable associated with this file.  Although VTK has
//      support for many different types, the best bet is vtkFloatArray, since
//      that is supported everywhere through VisIt.
//
//  Arguments:
//      timestate  The index of the timestate.  If GetNTimesteps returned
//                 'N' time steps, this is guaranteed to be between 0 and N-1.
//      domain     The index of the domain.  If there are NDomains, this
//                 value is guaranteed to be between 0 and NDomains-1,
//                 regardless of block origin.
//      varname    The name of the variable requested.
//
//  Programmer: fauchet -- generated by xml2avt
//
// ****************************************************************************
/*
DataArray *
LataLoader::GetVar(int timestate, int block, const char *varname)
{
  DataArray * return_value = 0;
  try {
    cerr << "Getvar time:" << timestate<<"/"<<filter_.get_nb_timesteps()
         << " block:" << block
         << " varname:" << varname << endl;

  if (filter_.get_nb_timesteps() > 1)
    timestate++;

  Field_UName field_uname;
  int component;
  get_field_info_from_visitname(varname, field_uname, component);
  
  if (component < 0) {
    cerr << "Error: LataLoader::GetVar called for vector field" << endl;
    throw;
  }

  Field_Id id(field_uname, timestate, block);

  const LataField_base & field = filter_.get_field(id);

  const Field<FloatTab> * float_field_ptr = dynamic_cast<const Field<FloatTab>*>(&field);
  const Field<IntTab> * int_field_ptr = dynamic_cast<const Field<IntTab>*>(&field);

  if (float_field_ptr) {
    DataArrayDouble *rv = DataArrayDouble::New();
    const Field<FloatTab> & fld = *float_field_ptr;
    const FloatTab & values = fld.data_;
    int ntuples = values.dimension(0);
    rv->alloc(ntuples,1);
    double * data = rv->getPointer();
    for (int i = 0; i < ntuples; i++) 
      data[i] = values(i, component);
    return_value = rv;
  } else if (int_field_ptr) {
    throw;
  } else {
    cerr << "Error in LataLoader::GetVar: unknown data type" << endl;
    throw;
  }
  filter_.release_field(field);
  }
  catch (LataDBError err) {
    cerr << "Error in getvar " << timestate << " " << block << " " << varname << " " << err.describe() << endl;
    throw;
  }
  return return_value; 
}
*/

// ****************************************************************************
//  Method: LataLoader::GetVectorVar
//
//  Purpose:
//      Gets a vector variable associated with this file.  Although VTK has
//      support for many different types, the best bet is vtkFloatArray, since
//      that is supported everywhere through VisIt.
//
//  Arguments:
//      timestate  The index of the timestate.  If GetNTimesteps returned
//                 'N' time steps, this is guaranteed to be between 0 and N-1.
//      domain     The index of the domain.  If there are NDomains, this
//                 value is guaranteed to be between 0 and NDomains-1,
//                 regardless of block origin.
//      varname    The name of the variable requested.
//
//  Programmer: fauchet -- generated by xml2avt
//
// ****************************************************************************

DataArray *
LataLoader::GetVectorVar(int timestate, int block, const char *varname)
{
  DataArray * return_value = 0;
  try {
  cerr << "Getvectorvar time:" << timestate<<"/"<<filter_.get_nb_timesteps()-1
         << " block:" << block
         << " varname:" << varname << endl;

  if (filter_.get_nb_timesteps() > 1)
    timestate++;

  Field_UName field_uname;
  int component;
  get_field_info_from_visitname(varname, field_uname, component);
  /*
  if (component >= 0) {
    cerr << "Error: LataLoader::GetVectorVar called for scalar field" << endl;
    throw;
  }
  */
  Field_Id id(field_uname, timestate, block);

  const LataField_base & field = filter_.get_field(id);

  const Field<FloatTab> * float_field_ptr = dynamic_cast<const Field<FloatTab>*>(&field);
  const Field<IntTab> * int_field_ptr = dynamic_cast<const Field<IntTab>*>(&field);

  if (float_field_ptr) {
    DataArrayDouble *rv = DataArrayDouble::New();
    const Field<FloatTab> & fld = *float_field_ptr;
    const FloatTab & values = fld.data_;
    int ntuples = values.dimension(0);
    int dim = values.dimension(1);
    rv->alloc(ntuples,dim);
    double* data= rv->getPointer();
    for (int i = 0; i < ntuples; i++)
      for (int j = 0; j < dim; j++)
        data[i*dim+j] =  values(i, j);
    return_value = rv;
  } else if (int_field_ptr) {
    throw;
   
  } else {
    cerr << "Error in LataLoader::GetVectorVar: unknown data type" << endl;
    throw;
  }
  filter_.release_field(field);
  }
  catch (LataDBError err) {
    cerr << "Error in getvectorvar " << timestate << " " << block << " " << varname << " " << err.describe() << endl;
    throw;
  }
  return return_value; 
}

void LataLoader::get_field_info_from_visitname(const char *varname, Field_UName & uname, int & component) const
{
  const int k = field_username_.rang(varname);
  if (k < 0) {
    cerr << "Error in LataLoader::get_field_info_from_visitname: field " << varname << " not found" << endl;
    for (int i=0;i<field_username_.size();i++)
      cerr<<field_username_[i]<< " ";
    throw  ;
  }
  uname = field_uname_[k];
  component = field_component_[k];
}

MEDCouplingFieldDouble*  LataLoader::GetFieldDouble(const char *varname,int timestate, int block)
{
  
  if (timestate==-1)
	 timestate=filter_.get_nb_timesteps()-2; 
  const Noms geoms = filter_.get_exportable_geometry_names();
  TypeOfField cent;
  Field_UName field_uname;
  int component;
  get_field_info_from_visitname(varname, field_uname, component);
  
  // for (int i_geom = 0; i_geom < geoms.size(); i_geom++)
  {
    //cerr << " Domain : " << geoms[i_geom] << endl;
    const LataGeometryMetaData data = filter_.get_geometry_metadata(field_uname.get_geometry());
    
    // Field_UNames fields = filter_.get_exportable_field_unames(geoms[i_geom]);

    //for (int i_field = 0; i_field < fields.size(); i_field++) {
    const LataFieldMetaData data2 = filter_.get_field_metadata(field_uname);
      
    switch (data2.localisation_) {
    case LataField_base::ELEM: cent = ON_CELLS; break;
    case LataField_base::SOM:  cent = ON_NODES; break;
    default:
      throw;
    }
  }
  
  double time=filter_.get_timestep(timestate+1);
  MEDCouplingFieldDouble* ret=MEDCouplingFieldDouble::New(cent,ONE_TIME);
  MEDCouplingMesh* mesh=GetMesh(field_uname.get_geometry(),timestate,block);
  DataArray* array=GetVectorVar(timestate,block,varname);
  ret->setMesh(mesh);
  ret->setArray(( DataArrayDouble*)array);
  mesh->decrRef();
  array->decrRef();
 
  ret->setName(varname);
  ret->setTime(time,timestate,-1);
  ret->checkCoherency();
  return ret;
}


std::vector<std::string> LataLoader::GetMeshNames()
{
  std::vector<std::string> names;
  const Noms& geoms = mesh_username_;
  for (int i=0;i<geoms.size();i++)
    names.push_back(geoms[i].getString());
  return names;

}
std::vector<std::string>  LataLoader::GetFieldNames()
{
  std::vector<std::string> names;
  for (int i=0;i<field_username_.size();i++)
    {
      const Nom& name=field_username_[i];
      if ((!name.debute_par("mesh_quality/"))&&(!name.debute_par("normals/") ))
	names.push_back(name.getString());
    }
  return names;
}
std::vector<std::string>  LataLoader::GetFieldNamesOnMesh(const std::string& domain_name)
{
  std::vector<std::string> names;
  std::vector<std::string> names_tot=GetFieldNames();
  Nom test("_");
  test+=domain_name.c_str();
  for (int i=0;i<names_tot.size();i++)
    {
      const Nom& name=names_tot[i];
      if (name.finit_par(test))   
	names.push_back(name.getString());
    }
  return names;
}
