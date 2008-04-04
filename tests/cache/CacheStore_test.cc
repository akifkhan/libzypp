#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <boost/test/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>
#include <boost/test/unit_test_log.hpp>

#include "zypp/base/Random.h"
#include "zypp/base/Logger.h"

#include "zypp/base/Measure.h"
#include "zypp/capability/CapabilityImpl.h"
#include "zypp/data/ResolvableData.h"
#include "zypp/cache/CacheStore.h"
#include "zypp/RepoInfo.h"
#include "zypp/repo/cached/RepoImpl.h"
#include "zypp/parser/yum/RepoParser.h"
#include "zypp/parser/susetags/RepoParser.h"
#include "zypp/Url.h"
#include "zypp/NVRA.h"
#include "zypp/PathInfo.h"
#include "zypp/TmpPath.h"
#include "zypp/TranslatedText.h"
#include "zypp/Package.h"
#include "zypp/Pattern.h"
#include "zypp/Product.h"
#include "zypp/Patch.h"
#include "zypp/Script.h"
#include "zypp/DiskUsageCounter.h"
#include "zypp/ZYppFactory.h"
#include "zypp/ZConfig.h"

#include "zypp/detail/ImplConnect.h"

using namespace std;
using namespace zypp;
using namespace zypp::parser;
using namespace zypp::repo;
using namespace zypp::repo::cached;
using namespace boost::unit_test;
using namespace sqlite3x;

/**
 * \short Asserts a package against a fixed package
 *
 * kdelibs3 357-24 i586
 */
void check_kdelibs3_package( Package::Ptr p )
{
  // check authors and timestamp?
//   =Tim: 1183399094
//   +Aut:
//   The KDE Team <kde@kde.org>
//   -Aut:
//
  BOOST_CHECK_EQUAL( p->name(), "kdelibs3");
  BOOST_CHECK_EQUAL( p->edition(), Edition("3.5.7", "24") );
  BOOST_CHECK_EQUAL( p->arch(), Arch("i586") );
  BOOST_CHECK_EQUAL( p->summary(), "KDE Base Libraries" );
  BOOST_CHECK_EQUAL( p->description().substr(0, 16), "<!-- DT:Rich -->");
  //BOOST_CHECK_EQUAL( p->packager(), "http://bugs.opensuse.org");
  //BOOST_CHECK_EQUAL( p->url(), "http://glabels.sourceforge.net/");
  BOOST_CHECK_EQUAL( p->group(), "System/GUI/KDE");
  //BOOST_CHECK_EQUAL( p->buildhost(), "dale.suse.de");
  //BOOST_CHECK_EQUAL( p->vendor(), "SUSE LINUX Products GmbH, Nuernberg, Germany");
  BOOST_CHECK_EQUAL( p->license(), "BSD License and BSD-like, GNU General Public License (GPL)");
  BOOST_CHECK_EQUAL( p->location().checksum(), CheckSum("sha1", "05f0647241433d01636785fd282cc824a6527269"));
  BOOST_CHECK_EQUAL( p->location().filename(), Pathname("./suse/i586/kdelibs3-3.5.7-24.i586.rpm"));
  BOOST_CHECK_EQUAL( p->location().medianr(), (unsigned)1);
  BOOST_CHECK_EQUAL( p->size(), 38850584);
  BOOST_CHECK_EQUAL( p->location().downloadSize(), 16356019);

  //BOOST_CHECK_EQUAL( p->diskusage().size(), (unsigned) 181); // all entries
  BOOST_CHECK_EQUAL( p->diskusage().size(), (unsigned) 7); // limited to level 3 "/a/b/"
  for ( DiskUsage::iterator it = p->diskusage().begin();
        it !=  p->diskusage().end();
        ++it )
  {
    MIL << "disk usage entry " << (*it).path << endl;
  }
}

/**
 * \short Asserts a pattern against a fixed pattern
 *
 * kde-10.3-71.i586.pat
 */
void check_kde_pattern( Pattern::Ptr p )
{
  BOOST_CHECK_EQUAL( p->name(), "kde");
  BOOST_CHECK_EQUAL( p->edition(), Edition("10.3", "71") );
  BOOST_CHECK_EQUAL( p->arch(), Arch("i586") );

//   detail::ResImplTraits<ResObject::Impl>::constPtr pipp( detail::ImplConnect::resimpl( p ) );
//   TranslatedText got = pipp->summary();
//
  //DBG<< pipp->summary() << endl;
//   for ( set<Locale>::const_iterator it = got.locales().begin();
//         it != got.locales().end();
//         ++it )
//   {
//     cout << *it << " | " << got.text(*it) << endl;
//   }

  BOOST_CHECK_EQUAL( p->summary(), "KDE Desktop Environment" );
  BOOST_CHECK_EQUAL( p->description().substr(0, 16), "KDE is a powerfu");
  BOOST_CHECK_EQUAL( p->category(), "Graphical Environments");
  BOOST_CHECK_EQUAL( p->userVisible(), true);
  //1520
}

/**
 * \short Asserts a product against a fixed product
 *
 * openSUSE-factory
 */
void check_factory_product( Product::Ptr p )
{
  BOOST_CHECK_EQUAL( p->name(), "openSUSE-factory");
  BOOST_CHECK_EQUAL( p->edition(), Edition("10.3") );
  BOOST_CHECK_EQUAL( p->arch(), Arch("i686") );

  BOOST_CHECK_EQUAL( p->distributionName(), "SuSE-Linux-STABLE-X86" );
  BOOST_CHECK_EQUAL( p->distributionEdition(), Edition("10.2.42-factory") );
  BOOST_CHECK_EQUAL( p->summary(), "openSUSE FACTORY 10.3" );

  BOOST_CHECK_EQUAL( p->releaseNotesUrl(), Url("http://www.suse.com/relnotes/i386/openSUSE/FACTORY/release-notes.rpm") );

  list<Url>::const_iterator it;
  list<Url> urls;

  BOOST_CHECK_EQUAL( urls.size(), (unsigned)0 );

  urls = p->extraUrls();
  it = find( urls.begin(), urls.end(), Url("http://download.opensuse.org/distribution/10.2/repo/oss/"));
  BOOST_CHECK_EQUAL( p->extraUrls().size(), (unsigned)1 );
  BOOST_CHECK( it != urls.end() );

  urls = p->optionalUrls();
  BOOST_CHECK_EQUAL( urls.size(), (unsigned)2 );

  it = find( urls.begin(), urls.end(), Url("http://download.opensuse.org/distribution/10.2/repo/non-oss/"));
  BOOST_CHECK( it != urls.end() );
  it = find( urls.begin(), urls.end(), Url("http://download.opensuse.org/distribution/10.2/repo/debug/"));
  BOOST_CHECK( it != urls.end() );

  for ( it = urls.begin();
        it != urls.end();
        ++it )
  {
    MIL << *it << endl;
  }

//  BOOST_CHECK_EQUAL( p->description().substr(0, 16), "NO DESC?");
//  BOOST_CHECK_EQUAL( p->category(), "Graphical Environments");
//  BOOST_CHECK_EQUAL( p->userVisible(), true);
  //1520
}


/**
 * \short Asserts a package against a fixed package
 *
 * glabels 2.0.4-30.2-0 i586
 */
void check_glabels_package( Package::Ptr p )
{
  BOOST_CHECK_EQUAL( p->name(), "glabels");
  BOOST_CHECK_EQUAL( p->edition(), Edition("2.0.4", "30.2", "0") );
  BOOST_CHECK_EQUAL( p->arch(), Arch("i586") );
  BOOST_CHECK_EQUAL( p->summary(), "A Label Editing and Printing Tool" );
  BOOST_CHECK_EQUAL( p->description().substr(0, 20), "Labels is a powerful");
  BOOST_CHECK_EQUAL( p->packager(), "http://bugs.opensuse.org");
  BOOST_CHECK_EQUAL( p->url(), "http://glabels.sourceforge.net/");
  BOOST_CHECK_EQUAL( p->group(), "Productivity/Office/Other");
  BOOST_CHECK_EQUAL( p->buildhost(), "dale.suse.de");
  BOOST_CHECK_EQUAL( p->vendor(), "SUSE LINUX Products GmbH, Nuernberg, Germany");
  BOOST_CHECK_EQUAL( p->license(), "GNU General Public License (GPL)");
  BOOST_CHECK_EQUAL( p->location().checksum(), CheckSum("sha1", "34adf06a0c4873b9d53b4634beb8bee458b45767"));
  BOOST_CHECK_EQUAL( p->location().filename(), Pathname("rpm/i586/glabels-2.0.4-30.2.i586.rpm"));
  BOOST_CHECK_EQUAL( p->location().medianr(), (unsigned)1 );
  BOOST_CHECK_EQUAL( p->size(), 2257356);
  BOOST_CHECK_EQUAL( p->location().downloadSize(), 983124);
}

/**
 * \short assert that tables are clean after removing a repo
 *
 * \note the only tables allowed to keep data are normalized
 * tables like names, files. Those should be cleaned using
 * a more smart vacuum
 */
void check_tables_clean( filesystem::TmpDir tmpdir )
{
  cache::CacheStore store(tmpdir.path());
  data::RecordId repository_id = store.lookupOrAppendRepository("novell.com");
  store.cleanRepository(repository_id);
  store.commit();

  sqlite3_connection con((tmpdir.path() + "zypp.db").c_str());
  int count;

  sqlite3_command tables_cmd( con, "select name from sqlite_master where type='table';");
  sqlite3_reader reader = tables_cmd.executereader();
  list<string> tables;
  while ( reader.read() )
  {
    string tablename = reader.getstring(0);
    if (
          (tablename == "sqlite_sequence" ) ||
          (tablename == "db_info" ) ||
          (tablename == "types" ) ||
          (tablename == "names" ) ||
          (tablename == "file_names" ) ||
          (tablename == "dir_names" ) ||
          (tablename == "files" )
    )
      continue;

    tables.push_back(tablename);
  }
  reader.close();

  for ( list<string>::const_iterator it = tables.begin();
        it != tables.end();
        ++it )
  {
    MIL << "Checking table " << *it << endl;
    string query = (string("select count(*) from ") + (*it) +";");
    sqlite3_command cmd(con, query.c_str());
    count = cmd.executeint();
    string msg = (string("there should be no ") + (*it) +" after cleaning");
    BOOST_CHECK_MESSAGE( count == 0, msg.c_str());
  }
}

/**
 * \short Write a YUM repo to the cache
 */
void write_yum_repo( const string &alias,
                     const Pathname &repodir,
                     filesystem::TmpDir tmpdir )
{
  data::RecordId repository_id;
  cache::CacheStore store(tmpdir.path());
  repository_id = store.lookupOrAppendRepository(alias);

  zypp::debug::Measure repo_write_timer("store resolvables");

  yum::RepoParser parser( repository_id, store);
  parser.parse(repodir);
  store.commit();
}

/**
 * \short Write a SUSETAGS repo to the cache
 */
void write_susetags_repo( const string &alias,
                          const Pathname &repodir,
                          filesystem::TmpDir tmpdir )
{
  data::RecordId repository_id;
  cache::CacheStore store(tmpdir.path());
  repository_id = store.lookupOrAppendRepository(alias);

  zypp::debug::Measure repo_write_timer("store resolvables");

  susetags::RepoParser parser( repository_id, store);
  parser.parse(repodir);
  store.commit();
}

/**
 * \short get resolvables from the cache
 */
ResStore get_resolvables( const string &alias,
                          filesystem::TmpDir tmpdir )
{
  MIL << "now read resolvables" << endl;

  data::RecordId repository_id;
  {
    cache::CacheStore store(tmpdir.path());
    repository_id = store.lookupOrAppendRepository(alias);
  }
  zypp::debug::Measure repo_read_timer("read resolvables");
  cached::RepoImpl *repositoryImpl = new cached::RepoImpl( cached::RepoOptions( RepoInfo(),
                                                           tmpdir.path(),
                                                           repository_id ));
  return repositoryImpl->resolvables();
}

/**
 * \short Lookup first resolvable matching kind, name, edition, arch (empty value is wildcard)
 */
template<class _Res, class _Iterator>
typename _Res::Ptr lookupResolvable( _Iterator begin_r, _Iterator end_r,
                                     const std::string & name_r,
                                     const Edition & edition_r = Edition(""),
                                     const Arch & arch_r = Arch("") )
{
  for( _Iterator it = begin_r; it != end_r; ++it )
  {
    if ( isKind<_Res>(*it)
         && ( name_r.empty()           || (*it)->name()    == name_r )
         && ( edition_r == Edition("") || (*it)->edition() == edition_r )
         && ( arch_r == Arch("")       || (*it)->arch()    == arch_r ) )
    {
      return asKind<_Res>(*it);
    }
  }
  return 0;
}

/**
 * \short Lookup first resolvable matching kind, name, edition, arch (empty string is wildcard)
 */
template<class _Res, class _Iterator>
typename _Res::Ptr lookupResolvable( _Iterator begin_r, _Iterator end_r,
                                     const std::string & name_r,
                                     const std::string & edition_r,
                                     const std::string & arch_r = std::string() )
{ return lookupResolvable<_Res>( begin_r, end_r, name_r, Edition(edition_r), Arch(arch_r) ); }

/**
 * \short Test that a yum repo is cached and restored
 */
void cache_write_yum_test(const string &dir)
{
  data::RecordId repository_id;
  Pathname repodir = Pathname(dir) + "/repo/yum/data/10.2-updates-subset";
  filesystem::TmpDir tmpdir;
  string alias = "novell.com";
  write_yum_repo( alias, repodir, tmpdir );

  ResStore dbres = get_resolvables( alias, tmpdir);
  //read_resolvables( alias, tmpdir, std::inserter(dbres, dbres.end()));
  MIL << dbres.size() << " resolvables" << endl;
  BOOST_CHECK_EQUAL( dbres.size(), (unsigned)35);

  {
    Package::Ptr p( lookupResolvable<Package>( dbres.begin(), dbres.end(), "glabels", "", "i586" ) );
    if ( p )
      check_glabels_package(p);
    else
      BOOST_CHECK_MESSAGE( false, "Package glabels i586 should be in cache");
  }

  {
    Script::Ptr s( lookupResolvable<Script>( dbres.begin(), dbres.end(), "fetchmsttfonts.sh-2333-patch-fetchmsttfonts.sh-2" ) );
    if ( s )
    {
      BOOST_CHECK( ! s->doScriptInlined().empty() );
      BOOST_CHECK( s->undoScriptInlined().empty() );
    }
    else
      BOOST_CHECK_MESSAGE( false, "Scrtipt fetchmsttfonts.sh-2333-patch-fetchmsttfonts.sh-2 should be in cache");
  }

  check_tables_clean(tmpdir);
}

/**
 * \short Test that a susetags repo is cached and restored
 */
void cache_write_susetags(const Pathname &repodir)
{
  data::RecordId repository_id;
  filesystem::TmpDir tmpdir;
  string alias = "novell.com";
  write_susetags_repo( alias, repodir, tmpdir );

  ResStore dbres = get_resolvables( alias, tmpdir);;
  //read_resolvables( alias, tmpdir, std::inserter(dbres, dbres.end()));
  MIL << dbres.size() << " resolvables" << endl;

  // packages and a patterns
  BOOST_CHECK_EQUAL( dbres.size(), (unsigned)7);
  bool found_kdelibs3_i586 = false;
  bool found_kde_pat = false;
  bool found_factory_product = false;
  for ( ResStore::const_iterator it = dbres.begin();
        it != dbres.end();
        ++it )
  {
    MIL << *it << endl;
    if ( isKind<Package>(*it) )
    {
      Package::Ptr p = asKind<Package>(*it);
      if ( (p->name() == "kdelibs3") && p->arch() == Arch("i586") )
      {
        BOOST_CHECK_MESSAGE( !found_kdelibs3_i586, "kdelibs3 i586 only once" );
        found_kdelibs3_i586 = true;
        check_kdelibs3_package(p);
      }
    }
    if ( isKind<Pattern>(*it) )
    {
      Pattern::Ptr p = asKind<Pattern>(*it);
      if ( (p->name() == "kde") && p->arch() == Arch("i586") )
      {
        BOOST_CHECK_MESSAGE( !found_kde_pat, "kde pattern only once" );
        found_kde_pat = true;
        check_kde_pattern(p);
      }
    }
    if ( isKind<Product>(*it) )
    {
      Product::Ptr p = asKind<Product>(*it);
      if ( (p->name() == "openSUSE-factory") )
      {
        BOOST_CHECK_MESSAGE( !found_factory_product, "factory product only once" );
        found_factory_product = true;
        check_factory_product(p);
      }
    }
  }
  BOOST_CHECK_MESSAGE( found_kdelibs3_i586, "Package kdelibs3 i586 should be in cache");
  BOOST_CHECK_MESSAGE( found_kde_pat, "Pattern kde i586 should be in cache");
  BOOST_CHECK_MESSAGE( found_factory_product, "Product factory i586 should be in cache");
  check_tables_clean(tmpdir);
}

/**
 * \short Test that a susetags repo reads shared attributes
 */
void cache_write_shared_attributes(const Pathname &repodir)
{
  ZConfig::instance().overrideSystemArchitecture( Arch( "x86_64" ) );
  data::RecordId repository_id;
  filesystem::TmpDir tmpdir;
  string alias = "novell.com";
  write_susetags_repo( alias, repodir, tmpdir );

  ResStore dbres = get_resolvables( alias, tmpdir);;
  //read_resolvables( alias, tmpdir, std::inserter(dbres, dbres.end()));
  MIL << dbres.size() << " resolvables" << endl;

  // 2 packages and a product
  BOOST_CHECK_EQUAL( dbres.size(), (unsigned)3);

  for ( ResStore::const_iterator it = dbres.begin();
        it != dbres.end();
        ++it )
  {
    MIL << *it << endl;
    if ( isKind<Package>(*it) )
    {
      Package::Ptr p = asKind<Package>(*it);
      if ( (p->name() == "foo") && p->arch() == Arch("x86_64") )
      {
        BOOST_CHECK_EQUAL( p->description(), "This is the description");
      }
      if ( (p->name() == "foo") && p->arch() == Arch("i586") )
      {
        BOOST_CHECK_EQUAL( p->description(), "This is the description");
      }
    }
  }
  check_tables_clean(tmpdir);
}

void cache_write_susetags_normal_test(const std::string &dir)
{
  Pathname repodir = Pathname(dir) + "/repo/susetags/data/stable-x86-subset";
  cache_write_susetags(repodir);
}

void cache_write_susetags_gz_test(const std::string &dir)
{
  Pathname repodir = Pathname(dir) + "/repo/susetags/data/stable-x86-subset-gz";
  cache_write_susetags(repodir);
}

void cache_write_shared_attributes_test(const std::string &dir)
{
  Pathname repodir = Pathname(dir) + "/repo/susetags/data/shared_attributes";
  cache_write_shared_attributes(repodir);
}

/**
 * \short Test that a yum repo wrtes and reads back patch and delta rpm information
 */
void cache_delta_rpm_test(const string &dir)
{
  Pathname repodir = Pathname(dir) + "/repo/yum/data/10.2-updates-subset";
  filesystem::TmpDir tmpdir;
  string alias = "novell.com";
  write_yum_repo( alias, repodir, tmpdir );

  data::RecordId repository_id;
  {
    cache::CacheStore store(tmpdir.path());
    repository_id = store.lookupOrAppendRepository(alias);
  }

  cached::RepoImpl *repositoryImpl = new cached::RepoImpl( cached::RepoOptions( RepoInfo(),
                                                           tmpdir.path(),
                                                           repository_id ));
  // 22 packages and 6 patches
  BOOST_CHECK_EQUAL( repositoryImpl->resolvables().size(), 42 );
  // 19 patch rpms
  // but only 13 are compatible with x86_64_
  BOOST_CHECK_EQUAL( repositoryImpl->patchRpms().size(), 13 );
  //check_tables_clean(tmpdir);
}

test_suite*
init_unit_test_suite( int argc, char *argv[] )
{
  string datadir;
  if (argc < 2)
  {
    datadir = TESTS_SRC_DIR;
    cout << "CacheStore_test:"
      " path to directory with test data required as parameter. Using " << datadir  << endl;
    //return (test_suite *)0;

  }
  else
  {
    datadir = argv[1];
  }

  ZConfig::instance().overrideSystemArchitecture( Arch( "i686" ) );

  test_suite* test= BOOST_TEST_SUITE("CacheStore");

  std::string const params[] = { datadir };
  test->add(BOOST_PARAM_TEST_CASE(&cache_write_yum_test,
                                 (std::string const*)params, params+1));

  DiskUsageCounter::MountPointSet mounts;
//   mounts.insert( DiskUsageCounter::MountPoint("/") );
//   mounts.insert( DiskUsageCounter::MountPoint("/etc") );
//   mounts.insert( DiskUsageCounter::MountPoint("/opt/kde3/share/mimelnk/video") );
//   mounts.insert( DiskUsageCounter::MountPoint("/foo") );
//   mounts.insert( DiskUsageCounter::MountPoint("/bar") );

  getZYpp()->setPartitions(mounts);

  test->add(BOOST_PARAM_TEST_CASE(&cache_write_susetags_normal_test,
                                 (std::string const*)params, params+1));
  test->add(BOOST_PARAM_TEST_CASE(&cache_write_susetags_gz_test,
                                 (std::string const*)params, params+1));
  test->add(BOOST_PARAM_TEST_CASE(&cache_write_shared_attributes_test,
                                 (std::string const*)params, params+1));

  // test for delta and patch rpms
  test->add(BOOST_PARAM_TEST_CASE(&cache_delta_rpm_test,
                                 (std::string const*)params, params+1));
  return test;
}


// vim: set ts=2 sts=2 sw=2 ai et: