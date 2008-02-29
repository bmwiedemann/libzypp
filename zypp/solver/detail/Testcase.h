/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file       zypp/solver/detail/Testcase.h
 *
*/

#ifndef ZYPP_SOLVER_DETAIL_TESTCASE_H
#define ZYPP_SOLVER_DETAIL_TESTCASE_H

#include <iosfwd>
#include <string>
#include "zypp/base/ReferenceCounted.h"
#include "zypp/base/NonCopyable.h"
#include "zypp/base/PtrTypes.h"
#include "zypp/solver/detail/Resolver.h"
#include "zypp/Capabilities.h"
#include "zypp/ResPool.h"
#include "zypp/base/GzStream.h"
#include "zypp/Repository.h"

/////////////////////////////////////////////////////////////////////////
namespace zypp
{ ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  namespace solver
  { /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    namespace detail
    { ///////////////////////////////////////////////////////////////////


template<class T>
std::string helixXML( const T &obj ); //undefined

template<> 
std::string helixXML( const Edition &edition );

template<> 
std::string helixXML( const Arch &arch );

template<> 
std::string helixXML( const Capability &cap );

template<> 
std::string helixXML( const Capabilities &caps );

template<> 
std::string helixXML( const CapabilitySet &caps );

template<> 
std::string helixXML( const Dependencies &dep );
	
template<> 
std::string helixXML( const PoolItem &item );


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : HelixResolvable
/**
 * Creates a file in helix format which includes all available
 * or installed packages,patches,selections.....
 **/
class  HelixResolvable : public base::ReferenceCounted, private base::NonCopyable{

  private:
    std::string dumpFile; // Path of the generated testcase
    ofgzstream *file;    

  public:
    HelixResolvable (const std::string & path);
    ~HelixResolvable ();

    void addResolvable (const PoolItem item);
    std::string filename () { return dumpFile; }
};

DEFINE_PTR_TYPE(HelixResolvable);
typedef std::map<Repository, HelixResolvable_Ptr> RepositoryTable;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : HelixControl
/**
 * Creates a file in helix format which contains all controll
 * action of a testcase ( file is known as *-test.xml)
 **/
class  HelixControl {

  private:
    std::string dumpFile; // Path of the generated testcase
    std::ofstream *file;

  public:
    HelixControl (const std::string & controlPath,
		  const RepositoryTable & sourceTable,
		  const Arch & systemArchitecture,
		  const PoolItemList &languages,		  
		  const std::string & systemPath = "solver-system.xml.gz");
    HelixControl ();    
    ~HelixControl ();

    void installResolvable (const ResObject::constPtr &resObject,
			    const ResStatus &status);
    void lockResolvable (const ResObject::constPtr &resObject,
			 const ResStatus &status);
    void keepResolvable (const ResObject::constPtr &resObject,
			 const ResStatus &status);        
    void deleteResolvable (const ResObject::constPtr &resObject,
			   const ResStatus &status);
    void addDependencies (const CapabilitySet &capRequire, const CapabilitySet &capConflict);
    std::string filename () { return dumpFile; }
};
	


	
///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : Testcase
/**
 * Generating a testcase of the current pool and solver state
 **/
class Testcase {

  private:
    std::string dumpPath; // Path of the generated testcase

  public:
    Testcase (const std::string & path);
    Testcase ();    
    ~Testcase ();

    bool createTestcase (Resolver & resolver, bool dumpPool = true, bool runSolver = true);
    bool createTestcasePool(const ResPool &pool);    
};


///////////////////////////////////////////////////////////////////
    };// namespace detail
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
  };// namespace solver
  ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
};// namespace zypp
/////////////////////////////////////////////////////////////////////////

#endif // ZYPP_SOLVER_DETAIL_TESTCASE_H
