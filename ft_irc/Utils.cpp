# include "Utils.hpp"
# include <algorithm>

void trim(std::string &str)
{
    str.erase(0,str.find_first_not_of("\t\n\v\f\r"));
    str.erase(str.find_last_not_of("\t\n\v\f\r") + 1);
}
