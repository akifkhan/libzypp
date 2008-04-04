/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file	zypp/Filter.h
 *
*/
#ifndef ZYPP_FILTER_H
#define ZYPP_FILTER_H

#include <iosfwd>

#include "zypp/base/Functional.h"
#include "zypp/base/Function.h"
#include "zypp/ResFilters.h"

#include "zypp/sat/Pool.h"

///////////////////////////////////////////////////////////////////
namespace zypp
{ /////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  namespace filter
  { /////////////////////////////////////////////////////////////////
    /** \defgroup POOLFILTER Collection solvable filter functors.
     *
     * All functors should be able to process \ref Solvable as well
     * as \ref PoolItem.
     *
     * \code
     *   // The same filter...
     *   filter::ByLocaleSupport f( Locale("de") );
     *
     *   // ...can be used to iterate the sat::Pool...
     *   sat::Pool satpool( sat::Pool::instance() );
     *   for_( it, satpool.filterBegin(f), satpool.filterEnd(f) )
     *   {
     *     MIL << *it << endl; // prints sat::Solvable
     *   }
     *
     *   // ...as well as the ResPool.
     *   ResPool   pool   ( ResPool::instance() );
     *   for_( it, pool.filterBegin(f), pool.filterEnd(f) )
     *   {
     *     MIL << *it << endl; // prints PoolItem
     *   }
     * \endcode
     * \ingroup g_Functor
     */
    //@{

    ///////////////////////////////////////////////////////////////////
    //
    //	CLASS NAME : ByLocaleSupport
    //
    /** Filter solvables according to their locale support.
    */
    class ByLocaleSupport
    {
      private:
        typedef bool (sat::Solvable::*LS1) (const Locale &) const;
        typedef bool (sat::Solvable::*LS2) (const LocaleSet &) const;

      public:
        /** Solvables with locale support. */
        ByLocaleSupport()
        : _sel( boost::mem_fun_ref( &sat::Solvable::supportsLocales ) )
        {}

        /** Solvables supporting \c locale_r. */
        explicit ByLocaleSupport( const Locale & locale_r )
        : _sel( boost::bind( boost::mem_fun_ref( (LS1)&sat::Solvable::supportsLocale ), _1, locale_r ) )
        {}

        /** Solvables supporting at least one locale in \c locales_r. */
        explicit ByLocaleSupport( const LocaleSet & locales_r )
        : _sel( boost::bind( boost::mem_fun_ref( (LS2)&sat::Solvable::supportsLocale ), _1, locales_r ) )
        {}

      public:
        /** Filter on \ref Solvable. */
        bool operator()( const sat::Solvable & solv_r ) const
        { return _sel && _sel( solv_r ); }

        /** Filter fitting PoolItem/ResObject. */
        template<class _Solv>
        bool operator()( const _Solv & solv_r ) const
        { return operator()( solv_r.satSolvable() ); }

      private:
        function<bool(const sat::Solvable &)> _sel;
    };
    ///////////////////////////////////////////////////////////////////

    //@}
    /////////////////////////////////////////////////////////////////
  } // namespace filter
  ///////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////
#endif // ZYPP_FILTER_H