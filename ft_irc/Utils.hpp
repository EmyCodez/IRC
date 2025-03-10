# ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <vector>

std::vector<std::string> split(const std::string &str, int sep= ' ');
void trim(std::string &str);
size_t stringToInt(const std::string &str);

# endif