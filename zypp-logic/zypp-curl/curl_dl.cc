/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file zypp-curl/curl_dl.cc
 *
 * Load libcurl with dlopen() on first use instead of a DT_NEEDED entry.
 *
 * The distro libcurl pulls ~30 shared objects (TLS, ssh, ldap, idn,
 * kerberos, ...) into every process that links it, costing ~2ms of
 * startup per exec - paid even by runs that never touch the network
 * (zypper --help, cached info/search).
 *
 * This file defines every libcurl symbol libzypp references. Each
 * wrapper resolves the real function from libcurl.so.4 on first call,
 * so the library and its dependency closure are only mapped when a
 * download actually happens. All other code keeps calling plain
 * curl_* functions and stays oblivious.
 *
 * Keep the list in sync with the real usage:
 *   nm -D --undefined-only libzypp.so | grep ' curl_'
 * A missing wrapper resurfaces as a DT_NEEDED on libcurl again (the
 * linker pulls it from CURL_LIBRARIES if someone re-adds it) or as a
 * link error, never as silent misbehavior.
 *
 * curl_easy_setopt, curl_multi_setopt and curl_easy_getinfo are
 * variadic in the libcurl ABI. getinfo always takes one pointer; the
 * setopt argument class is defined by the CURLOPTTYPE_* range of the
 * option id, exactly the rule libcurl documents for bindings.
 */

#include <curl/curl.h>

#include <dlfcn.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>

/* curl.h installs call-site typecheck macros for these */
#undef curl_easy_setopt
#undef curl_easy_getinfo
#undef curl_multi_setopt
#undef curl_share_setopt

namespace {

void * curl_dl_handle = nullptr;
pthread_once_t curl_dl_once = PTHREAD_ONCE_INIT;

extern "C" void curl_dl_open()
{
  curl_dl_handle = dlopen( "libcurl.so.4", RTLD_LAZY | RTLD_LOCAL );
  if ( !curl_dl_handle )
    fprintf( stderr, "libzypp: dlopen(libcurl.so.4): %s\n", dlerror() );
}

void * curl_dl_sym( const char * name )
{
  pthread_once( &curl_dl_once, curl_dl_open );
  return curl_dl_handle ? dlsym( curl_dl_handle, name ) : nullptr;
}

} // namespace

/* Wrapper for a non-variadic function returning a value. */
#define CURL_DL_FN( ret, name, proto, args, errret )	\
ret name proto						\
{							\
  static ret (*fn) proto;				\
  if ( !fn )						\
    *(void **)&fn = curl_dl_sym( #name );		\
  if ( !fn )						\
    return errret;					\
  return fn args;					\
}

/* Wrapper for a non-variadic function returning void. */
#define CURL_DL_FN_VOID( name, proto, args )		\
void name proto						\
{							\
  static void (*fn) proto;				\
  if ( !fn )						\
    *(void **)&fn = curl_dl_sym( #name );		\
  if ( fn )						\
    fn args;						\
}

CURL_DL_FN( CURLcode, curl_global_init, (long flags), (flags), CURLE_FAILED_INIT )

CURL_DL_FN( CURL *, curl_easy_init, (void), (), nullptr )
CURL_DL_FN_VOID( curl_easy_cleanup, (CURL *handle), (handle) )
CURL_DL_FN_VOID( curl_easy_reset, (CURL *handle), (handle) )
CURL_DL_FN( const char *, curl_easy_strerror, (CURLcode code), (code), "libcurl is not available" )

CURL_DL_FN_VOID( curl_free, (void *p), (p) )
CURL_DL_FN( char *, curl_unescape, (const char *str, int length), (str, length), nullptr )
CURL_DL_FN( curl_version_info_data *, curl_version_info, (CURLversion age), (age), nullptr )

CURL_DL_FN( struct curl_slist *, curl_slist_append, (struct curl_slist *list, const char *str), (list, str), nullptr )
CURL_DL_FN_VOID( curl_slist_free_all, (struct curl_slist *list), (list) )

CURL_DL_FN( CURLM *, curl_multi_init, (void), (), nullptr )
CURL_DL_FN( CURLMcode, curl_multi_cleanup, (CURLM *multi), (multi), CURLM_INTERNAL_ERROR )
CURL_DL_FN( CURLMcode, curl_multi_add_handle, (CURLM *multi, CURL *handle), (multi, handle), CURLM_INTERNAL_ERROR )
CURL_DL_FN( CURLMcode, curl_multi_remove_handle, (CURLM *multi, CURL *handle), (multi, handle), CURLM_INTERNAL_ERROR )
CURL_DL_FN( CURLMsg *, curl_multi_info_read, (CURLM *multi, int *msgs_in_queue), (multi, msgs_in_queue), nullptr )
CURL_DL_FN( CURLMcode, curl_multi_socket_action, (CURLM *multi, curl_socket_t s, int ev_bitmask, int *running_handles), (multi, s, ev_bitmask, running_handles), CURLM_INTERNAL_ERROR )
CURL_DL_FN( const char *, curl_multi_strerror, (CURLMcode code), (code), "libcurl is not available" )

/* The variadic trio. Forward through a variadic-typed pointer so the
 * call uses the variadic ABI (%al on x86-64). */

CURLcode curl_easy_getinfo( CURL *handle, CURLINFO info, ... )
{
  static CURLcode (*fn)( CURL *, CURLINFO, ... );
  va_list ap;
  void *arg;
  if ( !fn )
    *(void **)&fn = curl_dl_sym( "curl_easy_getinfo" );
  if ( !fn )
    return CURLE_FAILED_INIT;
  va_start( ap, info );
  arg = va_arg( ap, void * );	/* always a pointer out-arg */
  va_end( ap );
  return fn( handle, info, arg );
}

CURLcode curl_easy_setopt( CURL *handle, CURLoption option, ... )
{
  static CURLcode (*fn)( CURL *, CURLoption, ... );
  va_list ap;
  CURLcode ret;
  if ( !fn )
    *(void **)&fn = curl_dl_sym( "curl_easy_setopt" );
  if ( !fn )
    return CURLE_FAILED_INIT;
  va_start( ap, option );
  if ( option < CURLOPTTYPE_OBJECTPOINT )		/* long */
    ret = fn( handle, option, va_arg( ap, long ) );
  else if ( option < CURLOPTTYPE_OFF_T )		/* object/function pointer */
    ret = fn( handle, option, va_arg( ap, void * ) );
  else if ( option < CURLOPTTYPE_BLOB )			/* curl_off_t */
    ret = fn( handle, option, va_arg( ap, curl_off_t ) );
  else							/* blob pointer */
    ret = fn( handle, option, va_arg( ap, void * ) );
  va_end( ap );
  return ret;
}

CURLMcode curl_multi_setopt( CURLM *multi, CURLMoption option, ... )
{
  static CURLMcode (*fn)( CURLM *, CURLMoption, ... );
  va_list ap;
  CURLMcode ret;
  if ( !fn )
    *(void **)&fn = curl_dl_sym( "curl_multi_setopt" );
  if ( !fn )
    return CURLM_INTERNAL_ERROR;
  va_start( ap, option );
  if ( option < CURLOPTTYPE_OBJECTPOINT )
    ret = fn( multi, option, va_arg( ap, long ) );
  else if ( option < CURLOPTTYPE_OFF_T )
    ret = fn( multi, option, va_arg( ap, void * ) );
  else if ( option < CURLOPTTYPE_BLOB )
    ret = fn( multi, option, va_arg( ap, curl_off_t ) );
  else
    ret = fn( multi, option, va_arg( ap, void * ) );
  va_end( ap );
  return ret;
}
