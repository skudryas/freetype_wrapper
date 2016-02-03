#include <vector>

#include <iconv.h>
#include "iconv_wrapper.hpp"

namespace
{

std::string convert_encoding(const std::string& data, const std::string& from, const std::string& to)
{
   iconv_t convert_hnd = iconv_open(to.c_str(), from.c_str());
   if (convert_hnd == (iconv_t)(-1))
   {
      //LERR << "iconv_open return " << errno;
      throw std::logic_error("unable to create convertion descriptor");
   }

   char* in_ptr = const_cast<char*>(data.c_str());
   std::size_t in_size = data.size();
   std::vector<char> outbuf(4 + 4 * data.size());
   char* out_ptr = &outbuf[0];
   std::size_t reverse_size = outbuf.size(); // internal stupid iconv param

   std::size_t result = iconv(convert_hnd, &in_ptr, &in_size, &out_ptr, &reverse_size);
   iconv_close(convert_hnd);
   if (result == (std::size_t)(-1))
   {
      //LERR << "iconv return " << errno;
      throw std::logic_error("unable to convert");
   }
   return std::string(outbuf.data(), outbuf.size() - reverse_size);
}
}
