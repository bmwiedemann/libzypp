/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file zypp/parser/xmlstore/XMLProductParser.h
 *
*/

#ifndef XMLProductParser_h
#define XMLProductParser_h

#include "zypp/Url.h"
#include "zypp/parser/xmlstore/XMLParserData.h"
#include "zypp/parser/xmlstore/XMLResObjectParser.h"
#include "zypp/parser/XMLNodeIterator.h"
#include "zypp/parser/LibXMLHelper.h"
#include <list>

namespace zypp {
  namespace parser {
    namespace xmlstore {

      class XMLProductParser : public XMLNodeIterator<XMLProductData_Ptr>, public XMLResObjectParser
      {
      public:
        XMLProductParser(std::istream &is, const std::string &baseUrl);
        XMLProductParser();
        XMLProductParser(XMLProductData_Ptr &entry);
        virtual ~XMLProductParser();
    
      private:
        void parseProductFlags( XMLProductData_Ptr productPtr, xmlNodePtr node);
        
        template<class T>
        void parseList( const std::string &tagname, std::list<T> &list, xmlNodePtr node);
        
        virtual bool isInterested(const xmlNodePtr nodePtr);
        virtual XMLProductData_Ptr process(const xmlTextReaderPtr reader);
      };
    } // namespace yum
  } // namespace parser
} // namespace zypp

#endif