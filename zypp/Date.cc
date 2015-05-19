/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file	zypp/Date.cc
 *
*/
#include <iostream>
//#include "zypp/base/Logger.h"

#include "zypp/base/String.h"
#include "zypp/base/Xml.h"

#include "zypp/Date.h"

using std::endl;

///////////////////////////////////////////////////////////////////
namespace zypp
{
  ///////////////////////////////////////////////////////////////////
  namespace
  {
    ///////////////////////////////////////////////////////////////////

    inline bool isDST( struct tm & tm )
    {
      time_t t = ::mktime( &tm );
      struct tm *tm2 = ::localtime( &t );
      return ( tm2 && tm2->tm_isdst > 0 );
    }

    inline const char * _dateFormat( Date::DateFormat dateFormat_r )
    {
      static const char * fmt[] = {
	"",		///< ""
	"%Y-%m-%d",	///< 2014-02-07
	"%Y-%m",	///< 2014-02
	"%Y",		///< 2014
	"%G-W%V",	///< 2014-W06
	"%G-W%V-%u",	///< 2014-W06-5 (1 is Monday)
	"%Y-%j",	///< 2014-038
      };
      return fmt[dateFormat_r.asIntegral()];
    }

    inline const char * _timeFormat( Date::TimeFormat timeFormat_r )
    {
      static const char * fmt[] = {
	"",		///< ""
	"%H:%M:%S",	///< 07:06:41
	"%H:%M",	///< 07:06
	"%H",		///< 07
      };
      return fmt[timeFormat_r.asIntegral()];
    }

    inline const char * _timeZoneFormat( Date::TimeZoneFormat timeZoneFormat_r )
    {
      static const char * fmt[] = {
	"",		///< ""
	" %Z",		///< UTC, CET, ...
	"%z",		///< +0000
      };
      return fmt[timeZoneFormat_r.asIntegral()];
    }

    inline std::string doForm( const std::string & format_r, Date::TimeBase base_r, const Date::ValueType & date_r )
    {
      if ( ! date_r )
	return "0";

      static char buf[512];
      if ( ! strftime( buf, 512, format_r.c_str(), gmtime( &date_r ) ) )
	*buf = '\0';
      else
      {
	// strip a trailing '00' in a timeZoneFormat
	unsigned l = ::strlen( buf );
	if ( l >= 5
	  && ( buf[l-1] == '0' )
	  && ( buf[l-2] == '0' )
	  && ( buf[l-5] == '+' || buf[l-5] == '-') )
	  buf[l-2] = '\0';
      }
      return buf;
    }
  } // namespace
  ///////////////////////////////////////////////////////////////////

  const Date::ValueType Date::second;
  const Date::ValueType Date::minute;
  const Date::ValueType Date::hour;
  const Date::ValueType Date::day;
  const Date::ValueType Date::month28;
  const Date::ValueType Date::month29;
  const Date::ValueType Date::month30;
  const Date::ValueType Date::month31;
  const Date::ValueType Date::month;
  const Date::ValueType Date::year365;
  const Date::ValueType Date::year366;
  const Date::ValueType Date::year;

  Date::Date( const std::string & seconds_r )
  { str::strtonum( seconds_r, _date ); }

  Date::Date( const std::string & date_str, const std::string & format )
    : _date( Date( date_str, format, TB_LOCALTIME ) )
  {}

  Date::Date( const std::string & date_str, const std::string & format, Date::TimeBase base_r )
    : _date(0)
  {

    struct tm tm = {0,0,0,0,0,0,0,0,0,0,0};
    char * res = ::strptime( date_str.c_str(), format.c_str(), &tm );
    if (res == NULL)
      throw DateFormatException( str::form( "Invalid date format: '%s'", date_str.c_str() ) );

    if ( isDST(tm) )
      tm.tm_isdst = 1;
    _date = ::timegm( &tm );
  }

  std::string Date::form( const std::string & format_r, Date::TimeBase base_r ) const
  { return doForm( format_r, base_r, _date ); }

  std::string Date::print( DateFormat dateFormat_r, TimeFormat timeFormat_r, TimeZoneFormat timeZoneFormat_r, TimeBase base_r ) const
  {
    str::Str str;
    if ( dateFormat_r != DateFormat::none )
      str << _dateFormat( dateFormat_r );
    if ( timeFormat_r != TimeFormat::none )
    {
      if ( dateFormat_r != DateFormat::none )
	str << ' ';
      str << _timeFormat( timeFormat_r );
      if ( timeZoneFormat_r != TimeZoneFormat::none )
	str << _timeZoneFormat( timeZoneFormat_r );
    }
    return doForm( str, base_r, _date );
  }

  std::string Date::printISO( DateFormat dateFormat_r, TimeFormat timeFormat_r, TimeZoneFormat timeZoneFormat_r, TimeBase base_r ) const
  {
    str::Str str;
    if ( dateFormat_r != DateFormat::none )
      str << _dateFormat( dateFormat_r );
    if ( timeFormat_r != TimeFormat::none )
    {
      if ( dateFormat_r != DateFormat::none )
	str << 'T';
      str << _timeFormat( timeFormat_r );
      switch ( timeZoneFormat_r.asEnum() )
      {
	case TimeZoneFormat::none:
	  break;
	case TimeZoneFormat::name:
	  if ( base_r == TB_UTC )
	  {
	    str << 'Z';
	    break;
	  }
	  // else: FALLTHROUGH and print offset!
	case TimeZoneFormat::offset:
	  str << _timeZoneFormat( TimeZoneFormat::offset );
	  break;
      }
    }
    return doForm( str, base_r, _date );
  }

  std::ostream & dumpAsXmlOn( std::ostream & str, const Date & obj, const std::string & name_r )
  {
    return xmlout::node( str, name_r, {
      { "time_t",	Date::ValueType(obj) },
      { "text",	 	obj.printISO() },
    } );
  }

} // namespace zypp
///////////////////////////////////////////////////////////////////
