//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class Hdf5RunIter...
//
// Author List:
//      Andy Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "PSHdf5Input/Hdf5RunIter.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <boost/algorithm/string/predicate.hpp>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "hdf5pp/GroupIter.h"
#include "PSHdf5Input/Exceptions.h"
#include "PSHdf5Input/Hdf5CalibCycleIter.h"
#include "PSHdf5Input/Hdf5Utils.h"
#include "MsgLogger/MsgLogger.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

namespace {

  const char* logger = "Hdf5RunIter";
  
  // comparison operator for groups
  struct GroupCmp {
    bool operator()(const hdf5pp::Group& lhs, const hdf5pp::Group& rhs) const {
      return lhs.name() < rhs.name();
    }
  };

}

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

namespace PSHdf5Input {

//----------------
// Constructors --
//----------------
Hdf5RunIter::Hdf5RunIter (const hdf5pp::Group& grp)
  : m_grp(grp)
  , m_groups()
  , m_ccIter()
{
  // get all subgroups which start with 'CalibCycle:'
  hdf5pp::GroupIter giter(m_grp);
  for (hdf5pp::Group grp = giter.next(); grp.valid(); grp = giter.next()) {
    const std::string& grpname = grp.basename();
    if (grpname == "CalibCycle" or boost::algorithm::starts_with(grpname, "CalibCycle:")) {
      m_groups.push_back(grp);
    }    
  }

  // sort them by name
  m_groups.sort(GroupCmp());
}

//--------------
// Destructor --
//--------------
Hdf5RunIter::~Hdf5RunIter ()
{
}

// Returns next object
Hdf5RunIter::value_type 
Hdf5RunIter::next()
{
  if (not m_ccIter.get()) {
    
    // no more run groups left - we are done
    if (m_groups.empty()) {
      MsgLog(logger, debug, "stop iterating in group: " << m_grp.name());
      return value_type(value_type::Stop);
    }

    // open next group
    hdf5pp::Group grp = m_groups.front();
    m_groups.pop_front();  
    MsgLog(logger, debug, "switching to group: " << grp.name());

    // make iter for this new group
    m_ccIter.reset(new Hdf5CalibCycleIter(grp));

    // fill result with the configuration object data locations
    Hdf5IterData res(Hdf5IterData::BeginCalibCycle);
    hdf5pp::GroupIter giter(grp);
    for (hdf5pp::Group grp1 = giter.next(); grp1.valid(); grp1 = giter.next()) {
      hdf5pp::GroupIter subgiter(grp1);
      for (hdf5pp::Group grp2 = subgiter.next(); grp2.valid(); grp2 = subgiter.next()) {
        if (not grp2.hasChild("time")) {
          res.add(grp2, 0);
        }
      }    
    }

    res.setTime(Hdf5Utils::getTime(m_ccIter->group(), "start"));
    return res;
  }
  
  // read next event from this iterator
  const value_type& res = m_ccIter->next();

  // switch to next group if it sends us Stop
  if (res.type() == value_type::Stop) {
    Hdf5IterData res(Hdf5IterData::EndCalibCycle);
    res.setTime(Hdf5Utils::getTime(m_ccIter->group(), "end"));
    m_ccIter.reset();
    return res;
  } else {
    return res;
  }

}

} // namespace PSHdf5Input