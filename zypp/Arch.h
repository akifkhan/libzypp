/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file zypp/Arch.h
 *
*/
#ifndef ZYPP_ARCH_H
#define ZYPP_ARCH_H

#include <iosfwd>
#include <functional>
#include <string>

///////////////////////////////////////////////////////////////////
namespace zypp
{ /////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  //
  //	CLASS NAME : Arch
  //
  /** Architecture.
   * \todo improve compatibleWith implementation
   * \todo unify strings and optimize opertor==
  */
  class Arch
  {
  public:
    /** Default ctor 'noarch' */
    Arch();
    /** Ctor from string. */
    explicit
    Arch( const std::string & rhs );
    /** Dtor */
    ~Arch()
    {}
    /** String representation of Arch. */
    const std::string & asString() const
    { return _value; }

  public:
    /** Compatibility relation.
     * \return \c True iff \c this is compatible with \a rhs.
    */
    bool compatibleWith( const Arch & rhs ) const;

    /** Order on Arch (arbitrary).
     * \todo Adjust logical operators below to follow compare.
    */
    int compare( const Arch & rhs ) const
    { return _value.compare( rhs._value ); }

  private:
    /** String representation of Arch. */
    std::string _value;
  };
  ///////////////////////////////////////////////////////////////////

  /** \name Builtin architecture constants.
   *
   * Defined outside Arch as e.g. \c Arch_i386, because some names,
   * like \c i388, are used as \c #define, thus unusable as identifier
   * like \c Arch::i386.
  */
  //@{
  /** \relates Arch */
  extern const Arch Arch_noarch;
  extern const Arch Arch_src;

  /** \relates Arch */
  extern const Arch Arch_x86_64;
  /** \relates Arch */
  extern const Arch Arch_athlon;
  /** \relates Arch */
  extern const Arch Arch_i686;
  /** \relates Arch */
  extern const Arch Arch_i586;
  /** \relates Arch */
  extern const Arch Arch_i486;
  /** \relates Arch */
  extern const Arch Arch_i386;

  /** \relates Arch */
  extern const Arch Arch_s390x;
  /** \relates Arch */
  extern const Arch Arch_s390;

  /** \relates Arch */
  extern const Arch Arch_ppc64;
  /** \relates Arch */
  extern const Arch Arch_ppc;

  /** \relates Arch */
  extern const Arch Arch_ia64;
  //@}

  ///////////////////////////////////////////////////////////////////

  inline Arch::Arch()
  : _value( Arch_noarch._value )
  {}

  ///////////////////////////////////////////////////////////////////

  /** \relates Arch stream output. */
  inline std::ostream & operator<<( std::ostream & str, const Arch & obj )
  { return str << obj.asString(); }

  /** \name Comparison based on string value. */
  //@{
  /** \relates Arch */
  inline bool operator==( const Arch & lhs, const Arch & rhs )
  { return lhs.asString() == rhs.asString(); }

  /** \relates Arch */
  inline bool operator==( const Arch & lhs, const std::string & rhs )
  { return lhs.asString() == rhs; }

  /** \relates Arch */
  inline bool operator==( const std::string & lhs, const Arch & rhs )
  { return lhs == rhs.asString(); }

  /** \relates Arch */
  inline bool operator!=( const Arch & lhs, const Arch & rhs )
  { return !( lhs == rhs ); }

  /** \relates Arch */
  inline bool operator!=( const Arch & lhs, const std::string & rhs )
  { return !( lhs == rhs ); }

  /** \relates Arch */
  inline bool operator!=( const std::string & lhs, const Arch & rhs )
  { return !( lhs == rhs ); }
  //@}

  ///////////////////////////////////////////////////////////////////

  /** Functor finding compatible architectures.
   * \see Arch::compatibleWith
  */
  struct ArchCompatibleWith : public std::unary_function<Arch,bool>
  {
    /** The target architecture */
    Arch _targetArch;
    /** Ctor taking the target architecture */
    ArchCompatibleWith( const Arch & targetArch_r )
    : _targetArch( targetArch_r )
    {}
    /** Call Arch::compatibleWith ( \c _targetArch ) on \a rhs. */
    bool operator()( const Arch & rhs ) const
    { return rhs.compatibleWith( _targetArch ); }
  };

  /////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
namespace std
{ /////////////////////////////////////////////////////////////////
  /** \relates Arch Default order for std::container based on string value.*/
  template<>
    inline bool less<zypp::Arch>::operator()( const zypp::Arch & lhs, const zypp::Arch & rhs ) const
    { return lhs.asString() < rhs.asString(); }
  /////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////
#endif // ZYPP_ARCH_H
