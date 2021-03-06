#ifndef PSHDF5INPUT_HDF5DATASETITER_H
#define PSHDF5INPUT_HDF5DATASETITER_H

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class Hdf5DatasetIter.
//
//------------------------------------------------------------------------

//-----------------
// C/C++ Headers --
//-----------------

//----------------------
// Base Class Headers --
//----------------------
#include <iterator>
#include <vector>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "hdf5pp/DataSet.h"
#include "hdf5pp/Group.h"
#include "PSHdf5Input/Hdf5DatasetIterData.h"

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

//		---------------------
// 		-- Class Interface --
//		---------------------

namespace PSHdf5Input {

/**
 *  @ingroup PSHdf5Input
 *
 *  @brief Class defining an iterator for time-ordered scan of datasets.
 *  
 *  The only requirement for dataset is that it has a name "time" and
 *  data in it is a composite data type with members "seconds" and "nanoseconds". 
 *  The class is STL-like iterator, to construct "end" iterator make
 *  and instance with "End" tag.
 *  
 *  This software was developed for the LCLS project.  If you use all or 
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @version $Id$
 *
 *  @author Andy Salnikov
 */

class Hdf5DatasetIter : 
  public std::iterator<std::random_access_iterator_tag, Hdf5DatasetIterData, int64_t> {
public:

  enum Tag { Begin, End };
    
  // Constructor
  explicit Hdf5DatasetIter (const hdf5pp::Group& grp, bool fullTsFormat, Tag tag = Begin) ;

  // deref
  const Hdf5DatasetIterData& operator*() const { return m_data[m_index-m_dataIndex]; }

  // arrow
  const Hdf5DatasetIterData* operator->() const { return &m_data[m_index-m_dataIndex]; }

  // compare
  bool operator==(const Hdf5DatasetIter& other) const {
    return m_index == other.m_index;
  }
  bool operator!=(const Hdf5DatasetIter& other) const { return not operator==(other); }

  // increment
  Hdf5DatasetIter& operator++() { ++ m_index; updateData(); return *this; }
  Hdf5DatasetIter operator++(int) { Hdf5DatasetIter tmp(*this); ++ m_index; updateData(); return tmp; }
  Hdf5DatasetIter& operator+=(difference_type n) { m_index += n; updateData(); return *this; }
  
  // decrement
  Hdf5DatasetIter& operator--() { -- m_index; updateData(); return *this; }
  Hdf5DatasetIter operator--(int) { Hdf5DatasetIter tmp(*this); -- m_index; updateData(); return tmp; }
  Hdf5DatasetIter& operator-=(difference_type n) { m_index -= n; updateData(); return *this; }
  
  // difference between two iterators
  int64_t operator-(const Hdf5DatasetIter& other) { return m_index - other.m_index; }


  // subscription
  Hdf5DatasetIterData operator[](difference_type n) { Hdf5DatasetIter tmp(*this); tmp += n; return *tmp; }
  
  // ordering, can only apply to iterators from the same group
  bool operator<(const Hdf5DatasetIter& other) const { return m_index < other.m_index; }
  bool operator>(const Hdf5DatasetIter& other) const { return m_index > other.m_index; }
  bool operator<=(const Hdf5DatasetIter& other) const { return m_index <= other.m_index; }
  bool operator>=(const Hdf5DatasetIter& other) const { return m_index >= other.m_index; }
  
protected:

  // read m_data from dataset if possible
  void updateData();
  
private:

  hdf5pp::Group m_group;
  bool m_fullTsFormat;
  hdf5pp::DataSet m_ds;    ///< "time" dataset
  hdf5pp::DataSet m_mask;  ///< "_mask" dataset
  hdf5pp::DataSpace m_dsp; /// in-file dataspace for "time" dataset
  uint64_t m_size;         ///< Full dataset size
  int64_t m_index;         ///< Current index
  uint64_t m_dataIndex;    ///< Index of the beginning of the data array
  std::vector<Hdf5DatasetIterData> m_data;  ///< Current slice
};


inline
Hdf5DatasetIter 
operator+(Hdf5DatasetIter iter, int64_t n) { return iter += n; }

inline
Hdf5DatasetIter 
operator+(int64_t n, Hdf5DatasetIter iter) { return iter += n; }

inline
Hdf5DatasetIter 
operator-(Hdf5DatasetIter iter, int64_t n) { return iter -= n; }

} // namespace PSHdf5Input

#endif // PSHDF5INPUT_HDF5DATASETITER_H
