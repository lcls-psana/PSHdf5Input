//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class Hdf5InputModule...
//
// Author List:
//      Andy Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "PSHdf5Input/Hdf5InputModule.h"

//-----------------
// C/C++ Headers --
//-----------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "MsgLogger/MsgLogger.h"
#include "PSHdf5Input/Exceptions.h"
#include "PSHdf5Input/Hdf5EventId.h"
#include "PSHdf5Input/Hdf5FileListIter.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------


using namespace PSHdf5Input;
PSANA_INPUT_MODULE_FACTORY(Hdf5InputModule)

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

namespace PSHdf5Input {

//----------------
// Constructors --
//----------------
Hdf5InputModule::Hdf5InputModule (const std::string& name)
  : psana::InputModule(name)
  , m_iter()
{
}

//--------------
// Destructor --
//--------------
Hdf5InputModule::~Hdf5InputModule ()
{
}


// Method which is called once at the beginning of the job
void 
Hdf5InputModule::beginJob(Env& env)
{
  MsgLog(name(), debug, name() << ": in beginJob()");
  
  // will throw if no files were defined in config
  std::list<std::string> fileNames = configList("files");
  if ( fileNames.empty() ) {
    throw EmptyFileList(ERR_LOC);
  }
  WithMsgLog(name(), debug, str) {
    str << "Input files: ";
    std::copy(fileNames.begin(), fileNames.end(), std::ostream_iterator<std::string>(str, " "));
  }
  
  // make iterator
  m_iter.reset(new Hdf5FileListIter(fileNames));
  
  // At the beginJob fill environment with configuration data 
  // from the first configure transition of the first file   

  Hdf5IterData data = m_iter->next();
  MsgLog(name(), debug, "First data item: " << data)
}

// Method which is called with event data
InputModule::Status 
Hdf5InputModule::event(Event& evt, Env& env)
{
  Hdf5IterData data = m_iter->next();
  MsgLog(name(), debug, "Hdf5InputModule::event -- data: " << data)

  InputModule::Status ret = InputModule::Abort;
  switch(data.type()) {
  case Hdf5IterData::Configure:
    fillConfig(data, env);
    fillEventId(data, evt);
    fillEpics(data, env);
    ret = InputModule::Skip;
    break;
  case Hdf5IterData::BeginRun:
    fillEventId(data, evt);
    ret = InputModule::BeginRun;
    break;
  case Hdf5IterData::BeginCalibCycle:
    fillConfig(data, env);
    fillEventId(data, evt);
    ret = InputModule::BeginCalibCycle;
    break;
  case Hdf5IterData::Event:
    fillEventId(data, evt);
    fillEvent(data, evt);
    fillEpics(data, env);
    ret = InputModule::DoEvent;
    break;
  case Hdf5IterData::EndCalibCycle:
    fillEventId(data, evt);
    ret = InputModule::EndCalibCycle;
    break;
  case Hdf5IterData::EndRun:
    fillEventId(data, evt);
    ret = InputModule::EndRun;
    break;
  case Hdf5IterData::UnConfigure:
    fillEventId(data, evt);
    ret = InputModule::Skip;
    break;
  case Hdf5IterData::Stop:
    ret = InputModule::Stop;
    break;
  }
  return ret;
}

// Method which is called once at the end of the job
void 
Hdf5InputModule::endJob(Env& env)
{
    
}

// Store config object in environment
void
Hdf5InputModule::fillConfig(const Hdf5IterData& data, Env& env)
{
  MsgLog(name(), debug, name() << ": in fillConfig()");
}

// Store EPICS data in environment
void
Hdf5InputModule::fillEpics(const Hdf5IterData& data, Env& env)
{
  MsgLog(name(), debug, name() << ": in fillConfig()");
}

// Store event ID object
void
Hdf5InputModule::fillEventId(const Hdf5IterData& data, Event& evt)
{
  MsgLog(name(), debug, name() << ": in fillEventId()");

  // Store event ID
  boost::shared_ptr<PSEvt::EventId> eventId( new Hdf5EventId(data.run(), data.time()) );
  evt.put(eventId);
}

// Store event data objects
void
Hdf5InputModule::fillEvent(const Hdf5IterData& data, Event& evt)
{
  MsgLog(name(), debug, name() << ": in fillEvent()");
}

} // namespace PSHdf5Input