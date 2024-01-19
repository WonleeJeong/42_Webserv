#include "../../inc/BlockParser.hpp"

void	ConfigParser::PrintParseError(std::string filename) {
	if (GetErrorbit()) {
		if (GetErrorline() == -1)
			std::cerr << "error" << std::endl;
		else if (GetErrorbit() == 2)
			std::cerr << "\"http\" directive is not allowed here in " << filename << ":" << GetErrorline() << std::endl;
		else if (GetErrorbit() == 3)
			std::cerr << "\"server\" directive is not allowed here in " << filename << ":" << GetErrorline() << std::endl;
		else if (GetErrorbit() == 4)
			std::cerr << "\"location\" directive is not allowed here in " << filename << ":" << GetErrorline() << std::endl;
		else
			std::cerr << filename << " line " << GetErrorline() << " error" << std::endl;
		throw ConfigParser::ConfFileError();
	}
	if (check_.http_ == true || check_.server_ == true || check_.location_ == true) {
		throw ConfigParser::ConfFileError();
	}
}

void	ConfigParser::CheckHttpBlock(void) {
	if (this->save_line_.size() != 3)
		throw ConfigParser::ConfFileError();
}