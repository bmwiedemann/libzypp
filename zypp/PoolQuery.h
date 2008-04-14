/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file zypp/PoolQuery.h
 *
*/
#ifndef ZYPP_POOLQUERY_H
#define ZYPP_POOLQUERY_H

#include "base/Regex.h"

#include "zypp/ResKind.h"
#include "zypp/sat/SolvAttr.h"
#include "zypp/sat/SolvIterMixin.h"
#include "zypp/sat/LookupAttr.h"

#include "zypp/base/PtrTypes.h"
#include "zypp/base/Function.h"

extern "C"
{
struct _Dataiterator;
}

///////////////////////////////////////////////////////////////////
namespace zypp
{ /////////////////////////////////////////////////////////////////

  namespace detail
  {
    class PoolQueryIterator;
  }

  ///////////////////////////////////////////////////////////////////
  //
  //  CLASS NAME : PoolQuery
  //
  /**
   * Meta-data query API. Returns solvables of specified kinds from specified
   * repositories with attributes matching the specified search strings.
   *
   * TODO: details, examples.
   */
  class PoolQuery : public sat::SolvIterMixin<PoolQuery, detail::PoolQueryIterator>
  {
  public:
    typedef std::set<std::string>                           StrContainer;
    typedef std::set<ResKind>                               Kinds;
    typedef std::map<sat::SolvAttr, StrContainer>           AttrRawStrMap;
    typedef std::map<sat::SolvAttr, std::string>            AttrCompiledStrMap;
    typedef std::map<sat::SolvAttr, str::regex>             AttrRegexMap;

    typedef detail::PoolQueryIterator                       const_iterator;
    typedef unsigned int                                    size_type;

  public:
    typedef function<bool( const sat::Solvable & )> ProcessResolvable;

    PoolQuery();
    ~PoolQuery();

    /** Query result accessers. */
    //@{
    class PoolQueryIterator;

    /** */
    const_iterator begin() const;
    /** */
    const_iterator end() const;
    /** */
    bool empty();
    /** */
    size_type size();
    //@}

    /**
     * executes the query with the current settings
     * results are yielded on the callback passed on
     * construction
     */
    void execute(ProcessResolvable fnc);

    /**
     * Filter by selectable kind.
     *
     * By default, all kinds will be returned. If addKind() is used,
     * only the specified kinds will be returned (multiple kinds will be ORed).
     *
     * Pass ResTraits<T>::kind to this method, where T is one of the
     * \ref Resolvable child classes (e.g. ResTraits<Pattern>::kind).
     */
    void addKind(const ResKind & kind);

    /**
     * Filter by repo.
     *
     * By default, all repos will be returned. If addRepo() is used,
     * only the specified repo will be returned (multiple repos will be ORed).
     */
    void addRepo(const std::string &repoalias);

    /** Installed status filter setters. */
    //@{

    /**
     * Filter by status (installed uninstalled)
     */
    enum StatusFilter {
      ALL = 0, // both install filter and uninstall filter bits are 0
      INSTALLED_ONLY = 1,
      UNINSTALLED_ONLY = 2
    };
    void setInstalledOnly();
    void setUninstalledOnly();
    void setStatusFilterFlags( StatusFilter flags );

    //@}

    /**
     * 
     */
    void addString(const std::string & value);

    /**
     * Filter by the \a value of any available solvable attribute.
     *
     * \note Solvables of a kind not supporting the specified attribute will
     * <b>not</b> be returned.
     * \todo check the above
     *
     * \param attr Attribute identfier. Use sat::Solvattr::* constants
     * \param value What to search for.
     */
    void addAttribute(const sat::SolvAttr & attr, const std::string & value = "");

    /**
     * Filter by Selectable status.
     *
     * This should cover also plain 'is installed' and 'not installed' statuses.
     *
     * \param status Selectable status (zypp::ui::Status enum)
     */
    //void addStatus(const Status status);

    /**
     * Add dependency filter.
     *
     * \param dtype   depenedcy type
     * \param name    depenency name
     * \param edition edition for a versioned dependency
     * \param rel     operand for a versioned dependency
     *
     * \todo maybe a isRegexp bool as in addName() for the name parameter would
     *       be handy here as well.
     * \todo add more addDependecy() variants
     *//*
    void addDependency(const Dep & dtype,
                       const std::string & name,
                       const Edition & edition = Edition(),
                       const Rel & rel = Rel::EQ);
*/


    /** \name Text Matching Options */
    //@{
    /**
     * Turn case sentitivity on or off (unsets or sets \ref SEARCH_NOCASE flag).
     * PoolQuery defaults to case insensitive search unless this method
     * is used.
     *
     * \param value Whether to turn the case sensitivity on (default) or off.
     */
    void setCaseSensitive(const bool value = true);

    /** Set to match exact string instead of substring.*/
    void setMatchExact();
    /** Set to substring (the default). */
    void setMatchSubstring();
    /** Set to match globs. */
    void setMatchGlob();
    /** Set to use the query strings as regexes */
    void setMatchRegex();
    /** Set to match words (uses regex) */
    void setMatchWord();
    //void setLocale(const Locale & locale);
    //@}


    /**
     * Require that all of the values set by addString, addAttribute, addDep
     * match the values of respective attributes. 
     */
    void setRequireAll(const bool require_all = true);

    /** \name getters */
    //@{

    /** Search strings added via addString() */
    const StrContainer & strings() const;
    /**
     * Map (map<SolvAttr, StrContainer>) of attribute values added via
     * addAttribute(), addDep in string form */
    const AttrRawStrMap & attributes() const;

    const StrContainer & attribute(const sat::SolvAttr & attr) const;

    const Kinds & kinds() const;

    const StrContainer & repos() const;

    bool caseSensitive() const;

    bool matchExact() const;
    bool matchSubstring() const;
    bool matchGlob() const;
    bool matchRegex() const;
    /**
     * Returns currently used string matching type.
     * \see satsolver/repo.h
     */
    int  matchType() const;

    bool matchWord() const;

    bool requireAll() const;

    StatusFilter statusFilterFlags() const;
    //@}

    /**
     * Reads from stream query. Attributes is sepated by delim. Query is
     * separated by two delim.
     *
     * \param str input stream which contains query
     * \param delim delimeter for attributes
     * \return true if non-empty query is recovered
     *
     * \see readPoolQueriesFromFile
     */
    bool recover( std::istream &str, char delim = '\n' );

    /**
     * Writes a machine-readable string representation of the query to stream.
     * Use \a delim as attribute delimiter.
     *
     * \param str output stream to write to
     * \param delim delimiter for attributes
     *
     * \see writePoolQueriesToFile
     */
    void serialize( std::ostream &str, char delim = '\n' ) const;

    /** Return a human-readable description of the query */
    std::string asString() const;

    bool operator==(const PoolQuery& b) const;
    bool operator!=(const PoolQuery& b) const { return !(*this == b ); }

    // low level API

    /**
     * Free function to set the satsolver repo search
     * flags.
     *
     * \see SEARCH_STRINGMASK
     * \see SEARCH_STRING
     * \see SEARCH_SUBSTRING
     * \see SEARCH_GLOB
     * \see SEARCH_REGEX
     * \see SEARCH_NOCASE
     * \see SEARCH_NO_STORAGE_SOLVABLE
     */
    void setFlags(int flags);

    class Impl;
  private:
    /** Pointer to implementation */
    RW_pointer<Impl> _pimpl;
  };
  ///////////////////////////////////////////////////////////////////

  /** \relates PoolQuery Stream output. */
  std::ostream & operator<<( std::ostream & str, const PoolQuery & obj );



  ///////////////////////////////////////////////////////////////////
  namespace detail
  { /////////////////////////////////////////////////////////////////


  ///////////////////////////////////////////////////////////////////
  //
  //  CLASS NAME : PoolQuery::PoolQueryIterator
  //
  /**
   * 
   */
  class PoolQueryIterator : public boost::iterator_adaptor<
    PoolQueryIterator                  // Derived
    , sat::LookupAttr::iterator        // Base
    , const sat::Solvable              // Value
    , boost::forward_traversal_tag     // CategoryOrTraversal
    , const sat::Solvable              // Reference
  >
  {
  public:
    PoolQueryIterator();

    PoolQueryIterator(const PoolQueryIterator &);

    explicit
    PoolQueryIterator( const sat::LookupAttr::iterator & val_r )
    { this->base_reference() = val_r; }

    ~PoolQueryIterator();

    PoolQueryIterator & operator=( const PoolQueryIterator & rhs );

  private:
    friend class boost::iterator_core_access;
    friend class PoolQuery::Impl;

    PoolQueryIterator(
        scoped_ptr< ::_Dataiterator> & dip_r,
        const PoolQuery::Impl * pqimpl);

    const sat::Solvable dereference() const
    {
      return _sid ? sat::Solvable(_sid) : sat::Solvable::noSolvable;
    }

    void increment();

    bool matchSolvable();

  private:
    /** current matching solvable id */
    int _sid;
    /** whether there is a next solvable to check */
    bool _has_next;
    /** whether to do text matching on our own (true) or the Dataiterator already did it */
    bool _do_matching;

    /** \name Query Data
     * Depending on whether regexes are used in the search either \ref _str or
     * \ref _regex (or either _attrs_str or _attrs_regex respectively) are used.
     */
    //@{

    /** string matching option flags */
    int _flags;
    /** global query string compiled */
    std::string _str;
    /** global query compiled regex */
    str::regex _regex;
    /** Attribute to string map holding per-attribute query strings (compiled) */
    PoolQuery::AttrCompiledStrMap _attrs_str;
    /** Attribute to regex map holding per-attribute compiled regex */
    PoolQuery::AttrRegexMap _attrs_regex;
    /** Set of repository names include in the search. */
    PoolQuery::StrContainer _repos;
    /** Set of solvable kinds to include in the search. */
    PoolQuery::Kinds _kinds;
    /** Installed status filter flags. \see PoolQuery::StatusFilter */
    int _status_flags;
    //@}

    /** used to copy current iterator in order to forward check for next attributes */
    sat::LookupAttr::iterator _tmpit;
  };
  ///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  } //namespace detail
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////

#endif // ZYPP_POOLQUERY_H
