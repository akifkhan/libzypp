/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file	zypp/misc/CheckAccessDeleted.h
 *
*/
#ifndef ZYPP_MISC_CHECKACCESSDELETED_H
#define ZYPP_MISC_CHECKACCESSDELETED_H

#include <iosfwd>
#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////
namespace zypp
{ /////////////////////////////////////////////////////////////////

  /**
   * Check for running processes which access deleted files or libraries.
   *
   * Executed after commit, this gives a hint which processes/services
   * need to be restarted.
   *
   * Per default upon construtcion or explicit call to \ref check,
   * information about running processes which access deleted files
   * or libraries is collected and provided as a \ref ProcInfo
   * container.
   */
  class CheckAccessDeleted
  {
    public:
      /**
       * Data about one running process accessing deleted files.
       */
      struct ProcInfo
      {
        std::string pid;		//!< process ID
        std::string ppid;		//!< parent process ID
        std::string puid;		//!< process user ID
        std::string login;		//!< process login name
        std::string command;		//!< process command name
        std::vector<std::string> files;	//!< list of deleted files or libraries accessed

        /** Guess if command was started by an \c /etc/init.d/ script.
         * The name of an \c /etc/init.d/ script that might be used to restart the
         * command.
         * \warning This is just a guess.
        */
        std::string service() const;
      };

      typedef size_t   					size_type;
      typedef ProcInfo					value_type;
      typedef std::vector<ProcInfo>::const_iterator	const_iterator;

    public:
      /** Default ctor performs check immediately.
       * Pass \c false and the initial check is omitted.
       * \throws Exception if \ref check throws.
       * \see \ref check.
       */
      CheckAccessDeleted( bool doCheck_r = true )
      { if ( doCheck_r ) check(); }

    public:
      /** Check for running processes which access deleted files or libraries.
       * \return the number of processes found.
       * \throws Exception On error collecting the data (e.g. no lsof installed)
       */
      size_type check();

      bool empty() const		{ return _data.empty(); }
      size_type size() const		{ return _data.size(); }
      const_iterator begin() const	{ return _data.begin(); }
      const_iterator end() const	{ return _data.end(); }

    private:
      std::vector<ProcInfo> _data;
  };
  ///////////////////////////////////////////////////////////////////

  /** \relates CheckAccessDeleted Stream output */
  std::ostream & operator<<( std::ostream & str, const CheckAccessDeleted & obj );

  /** \relates CheckAccessDeleted::ProcInfo Stream output */
  std::ostream & operator<<( std::ostream & str, const CheckAccessDeleted::ProcInfo & obj );

  /////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////
#endif // ZYPP_MISC_CHECKACCESSDELETED_H