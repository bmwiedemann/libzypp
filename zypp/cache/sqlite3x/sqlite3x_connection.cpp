/*
	Copyright (C) 2004-2005 Cory Nelson

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
		claim that you wrote the original software. If you use this software
		in a product, an acknowledgment in the product documentation would be
		appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.

	CVS Info :
		$Author: phrostbyte $
		$Date: 2005/06/16 20:46:40 $
		$Revision: 1.1 $
*/

/*
  this source contains modifications by Novell Inc.

  Changes:

  * dmacvicar@novell.com
    Wrap sqlite3_exec

*/

#include <sqlite3.h>
#include "sqlite3x.hpp"

static int global_progress_handler(void* ptr)
{
  //RepoImpl *r = dynamic_cast<RepoImpl *>(ptr);
  sqlite3x::sqlite3_connection *r = (sqlite3x::sqlite3_connection *)(ptr);
  if ( r )
    return r->_progress_handler_accessor(ptr);
  return 0;
}

namespace sqlite3x
{

int sqlite3_connection::_progress_handler_accessor(void* ptr)
{
  if ( _ticks.tick() )
    return 0;
  return 1;
}
 
sqlite3_connection::sqlite3_connection() : db(NULL)
{}

sqlite3_connection::sqlite3_connection(const char *db) : db(NULL)
{
  this->open(db);
}

sqlite3_connection::sqlite3_connection(const wchar_t *db) : db(NULL)
{
  this->open(db);
}

sqlite3_connection::~sqlite3_connection()
{
  if (this->db) sqlite3_close(this->db);
}

void sqlite3_connection::setprogresshandler( int n,
                                             const zypp::ProgressData::ReceiverFnc &fnc )
{
  _ticks.sendTo(fnc);
  if ( fnc )
    sqlite3_progress_handler(db, n, global_progress_handler, (void*)this);
  else
    sqlite3_progress_handler(db, n, NULL, (void*)this);
}

void sqlite3_connection::open(const char *db)
{
  if (sqlite3_open(db, &this->db)!=SQLITE_OK)
  {
    std::string msg( "unable to open database at " );
    msg += ( db ? db : "NULL" );
    SQLITE3X_THROW(database_error(msg));
  }
}

void sqlite3_connection::open(const wchar_t *db)
{
  if (sqlite3_open16(db, &this->db)!=SQLITE_OK)
    SQLITE3X_THROW(database_error("unable to open database"));
}

void sqlite3_connection::close()
{
  if (this->db)
  {
    if (sqlite3_close(this->db)!=SQLITE_OK)
      SQLITE3X_THROW(database_error(*this));
    this->db=NULL;
  }
}

long long sqlite3_connection::insertid()
{
  if (!this->db) throw database_error("database is not open");
  return sqlite3_last_insert_rowid(this->db);
}

void sqlite3_connection::setbusytimeout(int ms)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));

  if (sqlite3_busy_timeout(this->db, ms)!=SQLITE_OK)
    SQLITE3X_THROW(database_error(*this));
}

void sqlite3_connection::executenonquery(const char *sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  sqlite3_command(*this, sql).executenonquery();
}

void sqlite3_connection::executenonquery(const wchar_t *sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  sqlite3_command(*this, sql).executenonquery();
}

void sqlite3_connection::executenonquery(const std::string &sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  sqlite3_command(*this, sql).executenonquery();
}

void sqlite3_connection::executenonquery(const std::wstring &sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  sqlite3_command(*this, sql).executenonquery();
}

int sqlite3_connection::executeint(const char *sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executeint();
}

int sqlite3_connection::executeint(const wchar_t *sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executeint();
}

int sqlite3_connection::executeint(const std::string &sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executeint();
}

int sqlite3_connection::executeint(const std::wstring &sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executeint();
}

long long sqlite3_connection::executeint64(const char *sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executeint64();
}

long long sqlite3_connection::executeint64(const wchar_t *sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executeint64();
}

long long sqlite3_connection::executeint64(const std::string &sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executeint64();
}

long long sqlite3_connection::executeint64(const std::wstring &sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executeint64();
}

double sqlite3_connection::executedouble(const char *sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executedouble();
}

double sqlite3_connection::executedouble(const wchar_t *sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executedouble();
}

double sqlite3_connection::executedouble(const std::string &sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executedouble();
}

double sqlite3_connection::executedouble(const std::wstring &sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executedouble();
}

std::string sqlite3_connection::executestring(const char *sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executestring();
}

std::string sqlite3_connection::executestring(const wchar_t *sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executestring();
}

std::string sqlite3_connection::executestring(const std::string &sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executestring();
}

std::string sqlite3_connection::executestring(const std::wstring &sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executestring();
}

std::wstring sqlite3_connection::executestring16(const char *sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executestring16();
}

std::wstring sqlite3_connection::executestring16(const wchar_t *sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executestring16();
}

std::wstring sqlite3_connection::executestring16(const std::string &sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executestring16();
}

std::wstring sqlite3_connection::executestring16(const std::wstring &sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executestring16();
}

std::string sqlite3_connection::executeblob(const char *sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executeblob();
}

std::string sqlite3_connection::executeblob(const wchar_t *sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executeblob();
}

std::string sqlite3_connection::executeblob(const std::string &sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executeblob();
}

std::string sqlite3_connection::executeblob(const std::wstring &sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));
  return sqlite3_command(*this, sql).executeblob();
}

void sqlite3_connection::execute(const std::string &sql)
{
  if (!this->db) SQLITE3X_THROW(database_error("database is not open"));

  char *err_msg;

  if ( sqlite3_exec( this->db, sql.c_str(), NULL, NULL, &err_msg ) != SQLITE_OK )
  {
    std::string err(err_msg);
    sqlite3_free(err_msg);
    SQLITE3X_THROW(database_error(err.c_str()));
  }
}

}
