#ifndef FFNGFILES_H_
#define FFNGFILES_H_

#include <string>

class FFNGFiles {
public:
	static bool exists(const std::string &path);
	static std::string read(const std::string &path);
};

#endif /* FFNGFILES_H_ */
