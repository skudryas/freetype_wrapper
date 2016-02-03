#ifndef __ICONV_WRAPPER_H
#define __ICONV_WRAPPER_H

//#include <list>
//#include <map>
#include <string>
#include <stdexcept>

/*char const * const UTF8 = "UTF-8";
char const * const CP866 = "CP866";*/

std::string convert_encoding(const std::string&, const std::string&, const std::string&);

#endif

