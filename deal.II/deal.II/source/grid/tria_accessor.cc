/* $Id$ */

#include <grid/tria.h>
#include <grid/tria_levels.h>
#include <grid/tria_iterator.h>
#include <grid/tria_accessor.h>
#include <grid/tria_iterator.templates.h>
#include <grid/geometry_info.h>

/* Note: explicit instantiations at the end of the different sections!       */



/*------------------------ Functions: LineAccessor ---------------------------*/

template <int dim>
void TriaAccessor<dim>::copy_from (const TriaAccessor<dim> &a) {
  present_level = a.present_level;
  present_index = a.present_index;
  tria = a.tria;
};

 

template <int dim>
bool TriaAccessor<dim>::operator == (const TriaAccessor<dim> &a) const {
  Assert (tria == a.tria, ExcCantCompareIterators());
  return ((present_index == a.present_index) &&
	  (present_level == a.present_level));
};



template <int dim>
bool TriaAccessor<dim>::operator != (const TriaAccessor<dim> &a) const {
  Assert (tria == a.tria, ExcCantCompareIterators());
  return ((present_index != a.present_index) ||
	  (present_level != a.present_level));
};



template <int dim>
int TriaAccessor<dim>::level () const {
  return present_level;
};


  
template <int dim>
int TriaAccessor<dim>::index () const {
  return present_index;
};
  



template <int dim>
IteratorState TriaAccessor<dim>::state () const {
  if ((present_level>=0) && (present_index>=0))
    return valid;
  else
    if ((present_level==-1) && (present_index==-1))
      return past_the_end;
    else
      return invalid;
};







/*------------------------ Functions: LineAccessor ---------------------------*/

template <int dim>
void LineAccessor<dim>::set (const Line &line) const {
  tria->levels[present_level]->lines.lines[present_index] = line;
};



template <int dim>
int LineAccessor<dim>::vertex_index (const unsigned int i) const {
  Assert (i<2, ExcInvalidIndex(i,0,1));
  return tria->levels[present_level]->lines.lines[present_index].vertex (i);
};



template <int dim>
Point<dim> &
LineAccessor<dim>::vertex (const unsigned int i) const {
  return tria->vertices[vertex_index(i)];
};



template <int dim>
bool LineAccessor<dim>::used () const {
  Assert (state() == valid, ExcDereferenceInvalidObject());
  return tria->levels[present_level]->lines.used[present_index];
};



template <int dim>
void LineAccessor<dim>::set_used_flag () const {
  Assert (state() == valid, ExcDereferenceInvalidObject());
  tria->levels[present_level]->lines.used[present_index] = true;
};
  


template <int dim>
void LineAccessor<dim>::clear_used_flag () const {
  Assert (state() == valid, ExcDereferenceInvalidObject());
  tria->levels[present_level]->lines.used[present_index] = false;
};


template <int dim>
bool LineAccessor<dim>::user_flag_set () const {
  Assert (used(), ExcCellNotUsed());
  return tria->levels[present_level]->lines.user_flags[present_index];
};



template <int dim>
void LineAccessor<dim>::set_user_flag () const {
  Assert (used(), ExcCellNotUsed());
  tria->levels[present_level]->lines.user_flags[present_index] = true;
};



template <int dim>
void LineAccessor<dim>::set_user_pointer (void *p) const {
  Assert (used(), ExcCellNotUsed());
  tria->levels[present_level]->lines.user_pointers[present_index] = p;
};



template <int dim>
void LineAccessor<dim>::clear_user_pointer () const {
  Assert (used(), ExcCellNotUsed());
  tria->levels[present_level]->lines.user_pointers[present_index] = 0;
};



template <int dim>
void * LineAccessor<dim>::user_pointer () const {
  Assert (used(), ExcCellNotUsed());
  return tria->levels[present_level]->lines.user_pointers[present_index];
};

  


template <int dim>
void LineAccessor<dim>::clear_user_flag () const {
  Assert (used(), ExcCellNotUsed());
  tria->levels[present_level]->lines.user_flags[present_index] = false;
};



template <int dim>
TriaIterator<dim,LineAccessor<dim> >
LineAccessor<dim>::child (const unsigned int i) const {
  Assert (i<2, ExcInvalidIndex(i,0,1));
  
  TriaIterator<dim,LineAccessor<dim> > q (tria, present_level+1, child_index (i));
  
#ifdef DEBUG
  if (q.state() != past_the_end)
    Assert (q->used(), ExcUnusedCellAsChild());
#endif
  return q;
};


template <int dim>
int LineAccessor<dim>::child_index (unsigned int i) const {
  Assert (i<2, ExcInvalidIndex(i,0,1));
  return tria->levels[present_level]->lines.children[present_index]+i;
};




template <int dim>
void LineAccessor<dim>::set_children (const int index) const {
  Assert (used(), ExcCellNotUsed());
  tria->levels[present_level]->lines.children[present_index] = index;
};



template <int dim>
void LineAccessor<dim>::clear_children () const {
  set_children (-1);
};



template <int dim>
bool LineAccessor<dim>::has_children () const {
  Assert (state() == valid, ExcDereferenceInvalidObject());
  return (tria->levels[present_level]->lines.children[present_index] != -1);
}



template <int dim>
void LineAccessor<dim>::operator ++ () {
  ++present_index;
				   // is index still in the range of
				   // the vector?
  while (present_index
	 >=
	 (int)tria->levels[present_level]->lines.lines.size())
    {
				       // no -> go one level up until we find
				       // one with more than zero cells
      ++present_level;
      present_index = 0;
				       // highest level reached?
      if (present_level >= (int)tria->levels.size()) 
	{
					   // return with past the end pointer
	  present_level = present_index = -1;
	  return;
	};
    };
};



template <int dim>
void LineAccessor<dim>::operator -- () {
  --present_index;
				   // is index still in the range of
				   // the vector?
  while (present_index < 0) 
    {
				       // no -> go one level down
      --present_level;
				       // lowest level reached?
      if (present_level == -1) 
	{
					   // return with past the end pointer
	  present_level = present_index = -1;
	  return;
	};
				       // else
      present_index = tria->levels[present_level]->lines.lines.size()-1;
    };
};



template <int dim>
unsigned char LineAccessor<dim>::boundary_indicator () const {
  Assert (dim>=2, ExcNotUsefulForThisDimension());
  Assert (used(), ExcCellNotUsed());

  return tria->levels[present_level]->lines.material_id[present_index];
};



template <int dim>
void LineAccessor<dim>::set_boundary_indicator (unsigned char boundary_ind) const {
  Assert (dim>=2, ExcNotUsefulForThisDimension());
  Assert (used(), ExcCellNotUsed());

  tria->levels[present_level]->lines.material_id[present_index] = boundary_ind;
};



template <int dim>
bool LineAccessor<dim>::at_boundary () const {
				   // error checking is done
				   // in boundary_indicator()
  return (boundary_indicator() != 255);
};



template <int dim>
double LineAccessor<dim>::diameter () const {
  return sqrt((vertex(1)-vertex(0)).square());
};






/*------------------------ Functions: QuadAccessor ---------------------------*/

template <int dim>
void QuadAccessor<dim>::set (const Quad &quad) const {
  tria->levels[present_level]->quads.quads[present_index] = quad;
};



template <int dim>
int QuadAccessor<dim>::vertex_index (const unsigned int corner) const {
  Assert (corner<4, ExcInvalidIndex(corner,0,3));

  const int corner_convention[4] = { 0,0,1,1 };
  return line(corner)->vertex_index(corner_convention[corner]);
};



template <int dim>
Point<dim> &
QuadAccessor<dim>::vertex (const unsigned int i) const {
  return tria->vertices[vertex_index(i)];
};



template <int dim>
bool QuadAccessor<dim>::used () const {
  Assert (state() == valid, ExcDereferenceInvalidObject());
  return tria->levels[present_level]->quads.used[present_index];
};



template <int dim>
void QuadAccessor<dim>::set_used_flag () const {
  Assert (state() == valid, ExcDereferenceInvalidObject());
  tria->levels[present_level]->quads.used[present_index] = true;
};
  


template <int dim>
void QuadAccessor<dim>::clear_used_flag () const {
  Assert (state() == valid, ExcDereferenceInvalidObject());
  tria->levels[present_level]->quads.used[present_index] = false;
};




template <int dim>
bool QuadAccessor<dim>::user_flag_set () const {
  Assert (used(), ExcCellNotUsed());
  return tria->levels[present_level]->quads.user_flags[present_index];
};



template <int dim>
void QuadAccessor<dim>::set_user_flag () const {
  Assert (used(), ExcCellNotUsed());
  tria->levels[present_level]->quads.user_flags[present_index] = true;
};



template <int dim>
void QuadAccessor<dim>::clear_user_flag () const {
  Assert (used(), ExcCellNotUsed());
  tria->levels[present_level]->quads.user_flags[present_index] = false;
};



template <int dim>
void QuadAccessor<dim>::set_user_pointer (void *p) const {
  Assert (used(), ExcCellNotUsed());
  tria->levels[present_level]->quads.user_pointers[present_index] = p;
};



template <int dim>
void QuadAccessor<dim>::clear_user_pointer () const {
  Assert (used(), ExcCellNotUsed());
  tria->levels[present_level]->quads.user_pointers[present_index] = 0;
};



template <int dim>
void * QuadAccessor<dim>::user_pointer () const {
  Assert (used(), ExcCellNotUsed());
  return tria->levels[present_level]->quads.user_pointers[present_index];
};




template <int dim>
TriaIterator<dim,LineAccessor<dim> >
QuadAccessor<dim>::line (const unsigned int i) const {
  return
    TriaIterator<dim,LineAccessor<dim> >
    (
      tria,
      present_level,
      line_index (i)
    );
};



template <int dim>
unsigned int QuadAccessor<dim>::line_index (unsigned int i) const {
  Assert (i<4, ExcInvalidIndex(i,0,3));

  return tria->levels[present_level]->quads.quads[present_index].line(i);
};



template <int dim>
TriaIterator<dim,QuadAccessor<dim> >
QuadAccessor<dim>::child (const unsigned int i) const {
  Assert (i<4, ExcInvalidIndex(i,0,3));
  
  TriaIterator<2,QuadAccessor<dim> > q (tria, present_level+1, child_index (i));
  
#ifdef DEBUG
  if (q.state() != past_the_end)
    Assert (q->used(), ExcUnusedCellAsChild());
#endif
  return q;
};



template <int dim>
int QuadAccessor<dim>::child_index (unsigned int i) const {
  Assert (i<4, ExcInvalidIndex(i,0,3));
  return tria->levels[present_level]->quads.children[present_index]+i;
};



template <int dim>
void QuadAccessor<dim>::set_children (const int index) const {
  Assert (used(), ExcCellNotUsed());
  tria->levels[present_level]->quads.children[present_index] = index;
};



template <int dim>
void QuadAccessor<dim>::clear_children () const {
  set_children (-1);
};



template <int dim>
bool QuadAccessor<dim>::has_children () const {
  Assert (state() == valid, ExcDereferenceInvalidObject());
  return (tria->levels[present_level]->quads.children[present_index] != -1);
};



template <int dim>
void QuadAccessor<dim>::operator ++ () {
  ++present_index;
				   // is index still in the range of
				   // the vector?
  while (present_index
	 >=
	 (int)tria->levels[present_level]->quads.quads.size()) 
    {
				       // no -> go one level up
      ++present_level;
      present_index = 0;
				       // highest level reached?
      if (present_level >= (int)tria->levels.size()) 
	{
					   // return with past the end pointer
	  present_level = present_index = -1;
	  return;
	};
    };
};



template <int dim>
void QuadAccessor<dim>::operator -- () {
  --present_index;
				   // is index still in the range of
				   // the vector?
  while (present_index < 0) 
    {
				       // no -> go one level down
      --present_level;
				       // lowest level reached?
      if (present_level == -1) 
	{
					   // return with past the end pointer
	  present_level = present_index = -1;
	  return;
	};
				       // else
      present_index = tria->levels[present_level]->quads.quads.size()-1;
    };
};



template <int dim>
unsigned char QuadAccessor<dim>::boundary_indicator () const {
  Assert (dim>=3, ExcNotUsefulForThisDimension());
  Assert (used(), ExcCellNotUsed());

  return tria->levels[present_level]->quads.material_id[present_index];
};



template <int dim>
void QuadAccessor<dim>::set_boundary_indicator (unsigned char boundary_ind) const {
  Assert (dim>=3, ExcNotUsefulForThisDimension());
  Assert (used(), ExcCellNotUsed());

  tria->levels[present_level]->quads.material_id[present_index] = boundary_ind;
};



template <int dim>
bool QuadAccessor<dim>::at_boundary () const {
				   // error checking is done
				   // in boundary_indicator()
  return (boundary_indicator() != 255);
};



template <int dim>
double QuadAccessor<dim>::diameter () const {
  return sqrt(max((vertex(2)-vertex(0)).square(),
		  (vertex(3)-vertex(1)).square()));
};









/*------------------------ Functions: CellAccessor<1> -----------------------*/


#if deal_II_dimension == 1

template <>
bool CellAccessor<1>::at_boundary () const {
  return at_boundary(0) || at_boundary(1);
};



template <>
unsigned char CellAccessor<1>::material_id () const {
  Assert (used(),
	  typename TriaSubstructAccessor<1>::ExcCellNotUsed());
  return tria->levels[present_level]->lines.material_id[present_index];
};



template <>
void CellAccessor<1>::set_material_id (const unsigned char mat_id) const {
  Assert (used(),
	  typename TriaSubstructAccessor<1>::ExcCellNotUsed());
  tria->levels[present_level]->lines.material_id[present_index]
    = mat_id;						 
};



template <>
Triangulation<1>::face_iterator
CellAccessor<1>::face (const unsigned int) const {
  Assert (false, ExcNotUsefulForThisDimension());
  return 0;
};

#endif

/*------------------------ Functions: CellAccessor<2> -----------------------*/


#if deal_II_dimension == 2

template <>
bool CellAccessor<2>::at_boundary () const {
  return at_boundary(0) || at_boundary(1) || at_boundary(2) || at_boundary(3);
};



template <>
unsigned char CellAccessor<2>::material_id () const {
  Assert (used(),
	  typename TriaSubstructAccessor<2>::ExcCellNotUsed());
  return tria->levels[present_level]->quads.material_id[present_index];
};



template <>
void CellAccessor<2>::set_material_id (const unsigned char mat_id) const {
  Assert (used(),
	  typename TriaSubstructAccessor<2>::ExcCellNotUsed());
  tria->levels[present_level]->quads.material_id[present_index]
    = mat_id;						 
};



template <>
Triangulation<2>::face_iterator
CellAccessor<2>::face (const unsigned int i) const {
  return line(i);
};

#endif


/*------------------------ Functions: CellAccessor<dim> -----------------------*/


template <int dim>
int CellAccessor<dim>::neighbor_index (const unsigned int i) const {
  Assert (i<GeometryInfo<dim>::faces_per_cell,
	  typename TriaSubstructAccessor<dim>::ExcInvalidNeighbor(i));
  return tria->levels[present_level]->
    neighbors[present_index*GeometryInfo<dim>::faces_per_cell+i].second;
};



template <int dim>
int CellAccessor<dim>::neighbor_level (const unsigned int i) const {
  Assert (i<GeometryInfo<dim>::faces_per_cell,
	  typename TriaSubstructAccessor<dim>::ExcInvalidNeighbor(i));
  return tria->levels[present_level]->
    neighbors[present_index*GeometryInfo<dim>::faces_per_cell+i].first;
};



template <int dim>
void CellAccessor<dim>::set_neighbor (const unsigned int i,
				      const TriaIterator<dim,CellAccessor<dim> > &pointer) const {
  Assert (i<GeometryInfo<dim>::faces_per_cell,
	  typename TriaSubstructAccessor<dim>::ExcInvalidNeighbor(i));
  tria->levels[present_level]->
    neighbors[present_index*GeometryInfo<dim>::faces_per_cell+i].first
    = pointer.accessor.present_level;
  tria->levels[present_level]->
    neighbors[present_index*GeometryInfo<dim>::faces_per_cell+i].second
    = pointer.accessor.present_index;
};



template <int dim>
bool CellAccessor<dim>::at_boundary (const unsigned int i) const {
  Assert (used(), typename TriaSubstructAccessor<dim>::ExcCellNotUsed());
  Assert (i<GeometryInfo<dim>::faces_per_cell,
	  typename TriaSubstructAccessor<dim>::
	  ExcInvalidIndex (i,0,GeometryInfo<dim>::faces_per_cell-1));
  
  return (neighbor_index(i) == -1);
};



template <int dim>
bool CellAccessor<dim>::refine_flag_set () const {
  Assert (used(), typename TriaSubstructAccessor<dim>::ExcCellNotUsed());
				   // cells flagged for refinement must be active
				   // (the #set_refine_flag# function checks this,
				   // but activity may change when refinement is
				   // executed and for some reason the refine
				   // flag is not cleared).
  Assert (active() ||  !tria->levels[present_level]->refine_flags[present_index],
	  ExcRefineCellNotActive());
  return tria->levels[present_level]->refine_flags[present_index];
};



template <int dim>
void CellAccessor<dim>::set_refine_flag () const {
  Assert (used() && active(), ExcRefineCellNotActive());
  Assert (!coarsen_flag_set(), ExcCellFlaggedForCoarsening());
  
  tria->levels[present_level]->refine_flags[present_index] = true;
};



template <int dim>
void CellAccessor<dim>::clear_refine_flag () const {
  Assert (used() && active(), ExcRefineCellNotActive());
  tria->levels[present_level]->refine_flags[present_index] = false;
};



template <int dim>
bool CellAccessor<dim>::coarsen_flag_set () const {
  Assert (used(), typename TriaSubstructAccessor<dim>::ExcCellNotUsed());
				   // cells flagged for coarsening must be active
				   // (the #set_refine_flag# function checks this,
				   // but activity may change when refinement is
				   // executed and for some reason the refine
				   // flag is not cleared).
  Assert (active() ||  !tria->levels[present_level]->coarsen_flags[present_index],
	  ExcRefineCellNotActive());
  return tria->levels[present_level]->coarsen_flags[present_index];
};



template <int dim>
void CellAccessor<dim>::set_coarsen_flag () const {
  Assert (used() && active(), ExcRefineCellNotActive());
  Assert (!refine_flag_set(), ExcCellFlaggedForRefinement());
  
  tria->levels[present_level]->coarsen_flags[present_index] = true;
};



template <int dim>
void CellAccessor<dim>::clear_coarsen_flag () const {
  Assert (used() && active(), ExcRefineCellNotActive());
  tria->levels[present_level]->coarsen_flags[present_index] = false;
};



template <int dim>
TriaIterator<dim,CellAccessor<dim> >
CellAccessor<dim>::neighbor (const unsigned int i) const {
  TriaIterator<dim,CellAccessor<dim> > q (tria, neighbor_level (i), neighbor_index (i));

#ifdef DEBUG
  if (q.state() != past_the_end)
    Assert (q->used(),
	    typename TriaSubstructAccessor<dim>::ExcUnusedCellAsNeighbor());
#endif
  return q;
};



template <int dim>
TriaIterator<dim,CellAccessor<dim> >
CellAccessor<dim>::child (const unsigned int i) const {
  TriaIterator<dim,CellAccessor<dim> > q (tria, present_level+1, child_index (i));

#ifdef DEBUG
  if (q.state() != past_the_end)
    Assert (q->used(),
	    typename TriaSubstructAccessor<dim>::ExcUnusedCellAsChild());
#endif
  return q;
};



template <int dim>
bool CellAccessor<dim>::active () const {
  return !has_children();
};










// explicit instantiations
template class TriaAccessor<deal_II_dimension>;
template class LineAccessor<deal_II_dimension>;
template class CellAccessor<deal_II_dimension>;
template class TriaRawIterator<deal_II_dimension,LineAccessor<deal_II_dimension> >;
template class TriaRawIterator<deal_II_dimension,CellAccessor<deal_II_dimension> >;
template class TriaIterator<deal_II_dimension,LineAccessor<deal_II_dimension> >;
template class TriaIterator<deal_II_dimension,CellAccessor<deal_II_dimension> >;
template class TriaActiveIterator<deal_II_dimension,LineAccessor<deal_II_dimension> >;
template class TriaActiveIterator<deal_II_dimension,CellAccessor<deal_II_dimension> >;

#if deal_II_dimension >= 2
template class QuadAccessor<deal_II_dimension>;
template class TriaRawIterator<deal_II_dimension,QuadAccessor<deal_II_dimension> >;
template class TriaIterator<deal_II_dimension,QuadAccessor<deal_II_dimension> >;
template class TriaActiveIterator<deal_II_dimension,QuadAccessor<deal_II_dimension> >;
#endif






