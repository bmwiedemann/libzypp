/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
// LEGACY-REQUIRED — see stringpool.h
#include "stringpool.h"

#include <zypp-core/base/Exception.h>
#include <zypp-core/base/Gettext.h>
#include <zypp-core/base/LogTools.h>

namespace zyppng::sat {

  namespace {
    /* Fixed id block for the pool snapshot (see pool_snapshot.c's map
     * guard): libzypp and its consumers intern these strings before
     * any repo is loaded, from static initializers whose order depends
     * on the link line and thus on the binary. Interning them here,
     * right after pool_create, pins their ids so a snapshot written by
     * one libzypp binary maps in another.
     *
     * APPEND-ONLY: reordering or removing entries makes every existing
     * snapshot fall back to a full load until rewritten (safe, but
     * keep it stable). The order mirrors the pre-load intern sequence
     * observed in zypper so snapshots written by older builds keep
     * mapping. */
    const char * const fixedPoolIds[] = {
      // sat::Solvable tokens
      "pattern()", "product()", "retracted-patch-package()", "ptf()", "ptf-package()",
      // PoolQuery serialization words
      "repo", "type", "comment", "query_string", "match_type", "require_all",
      "case_sensitive", "install_status", "version", "complex", "exact",
      "substring", "regex", "glob", "word",
      // ResKind
      "package", "patch", "pattern", "product", "srcpackage", "application",
      // SolvAttr string literals
      "changelog", "sourcesize", "isdefault", "script",
      // the Arch builtins
      "i386", "i486", "i586", "i686", "athlon", "x86_64", "x86_64_v2",
      "x86_64_v3", "x86_64_v4", "pentium3", "pentium4", "ia64", "s390",
      "s390x", "ppc", "ppc64", "ppc64p7", "ppc64le", "alpha", "alphaev5",
      "alphaev56", "alphapca56", "alphaev6", "alphaev67", "sparc", "sparcv8",
      "sparcv9", "sparcv9v", "sparc64", "sparc64v", "armv3l", "armv4l",
      "armv4tl", "armv5l", "armv5tl", "armv5tel", "armv5tejl", "armv6l",
      "armv7l", "armv8l", "armv6hl", "armv7hl", "armv8hl", "armv7hnl",
      "armv7nhl", "armv7thl", "armv7tnhl", "aarch64", "riscv64", "sh3",
      "sh4", "sh4a", "m68k", "mips", "mipsel", "mips64", "mips64el",
      "loong64",
      // LanguageCode::enCode
      "en",
    };
  }

  StringPool &StringPool::instance()
  {
    static StringPool p;
    return p;
  }

  StringPool::~StringPool()
  {
    ::pool_free( _pool );
  }

  StringPool::StringPool() : _pool( ::pool_create() )
  {
    MIL << "Creating sat-pool." << std::endl;
    if ( ! _pool )
    {
      ZYPP_THROW( zypp::Exception( _("Can not create sat-pool.") ) );
    }
    // libzypp#726: If the disttype is unset, ::pool_evrcmp_str uses
    // the default flavor set at compiletime. But even on DEBIAN we
    // handle rpm packages, so their rules must be applied.
    ::pool_setdisttype( _pool, DISTTYPE_RPM );
    for ( const char * str : fixedPoolIds )
      ::pool_str2id( _pool, str, /*create*/1 );
  }

}
