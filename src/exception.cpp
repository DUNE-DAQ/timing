
#include "pdt/exception.hpp"


pdt::exception::exception()
{
}


pdt::exception::exception(const std::string& infoString) : 
 mInfo(infoString)
{
}


pdt::exception::~exception() throw()
{
}


const char* pdt::exception::what() const throw()
{
  return mInfo.c_str();
}


void pdt::exception::append(const std::string& aString)
{
  mInfo += "  \n";
  mInfo += aString;
}

