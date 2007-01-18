/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file	zypp/target/CommitPackageCache.h
 *
*/
#ifndef ZYPP_TARGET_COMMITPACKAGECACHE_H
#define ZYPP_TARGET_COMMITPACKAGECACHE_H

#include <iosfwd>
#include <list>

#include "zypp/base/PtrTypes.h"
#include "zypp/base/Function.h"

#include "zypp/PoolItem.h"
#include "zypp/Pathname.h"
#include "zypp/ManagedFile.h"

///////////////////////////////////////////////////////////////////
namespace zypp
{ /////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  namespace target
  { /////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    //
    //	CLASS NAME : CommitPackageCache
    //
    /** Target::commit helper optimizing package provision.
    */
    class CommitPackageCache
    {
    public:
      typedef std::list<PoolItem>::const_iterator          const_iterator;
      typedef function<ManagedFile( const PoolItem & pi )> PackageProvider;

    public:
      /** Default ctor */
      CommitPackageCache( const_iterator          begin_r,
                          const_iterator          end_r,
                          const Pathname &        rootDir_r,
                          const PackageProvider & packageProvider_r );

      /** Dtor */
      ~CommitPackageCache();

    public:
      /** Provide a package.
      */
      ManagedFile get( const_iterator citem_r );

    public:
      class Impl;
      friend std::ostream & operator<<( std::ostream & str, const CommitPackageCache & obj );
    private:
      /** Pointer to implementation */
      RW_pointer<Impl> _pimpl;
    };
    ///////////////////////////////////////////////////////////////////

    /** \relates CommitPackageCache Stream output */
    std::ostream & operator<<( std::ostream & str, const CommitPackageCache & obj );

    /////////////////////////////////////////////////////////////////
  } // namespace target
  ///////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////
#endif // ZYPP_TARGET_COMMITPACKAGECACHE_H
