#ifndef PSHDF5INPUT_HDF5RUNITER_H
#define PSHDF5INPUT_HDF5RUNITER_H

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class Hdf5RunIter.
//
//------------------------------------------------------------------------

//-----------------
// C/C++ Headers --
//-----------------
#include <string>
#include <list>
#include <boost/scoped_ptr.hpp>
#include <boost/utility.hpp>

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "hdf5pp/Group.h"
#include "PSHdf5Input/Hdf5IterData.h"

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------
namespace PSHdf5Input {
class Hdf5CalibCycleIter;
}

//		---------------------
// 		-- Class Interface --
//		---------------------

namespace PSHdf5Input {

/**
 *  @ingroup PSHdf5Input
 *
 *  @brief Iterator class which iterates over events in a single Run group.
 *
 *  This software was developed for the LCLS project.  If you use all or 
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @version $Id$
 *
 *  @author Andy Salnikov
 */

class Hdf5RunIter : boost::noncopyable {
public:

  /// Typedef for the data type returned by this iterator
  typedef Hdf5IterData value_type;

  // Default constructor
  explicit Hdf5RunIter (const hdf5pp::Group& runGrp, int runNumber, unsigned schemaVersion, bool fullTsFormat) ;

  // Destructor
  ~Hdf5RunIter () ;

  /// get its group
  hdf5pp::Group& group() { return m_runGrp; }

  /**
   *  @brief Returns next object
   *  
   *  If there are no more objects left then always return 
   *  object with the type Hdf5IterData::Stop.
   */
  value_type next();

protected:

private:

  hdf5pp::Group m_runGrp; 
  int m_runNumber;
  unsigned m_schemaVersion;
  bool m_fullTsFormat;
  std::list<hdf5pp::Group> m_calibCycleGroups;
  boost::scoped_ptr<Hdf5CalibCycleIter> m_ccIter;  ///< Iterator over current calib cycle

};

} // namespace PSHdf5Input

#endif // PSHDF5INPUT_HDF5RUNITER_H
