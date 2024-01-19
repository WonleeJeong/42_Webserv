#include "../../inc/BlockParser.hpp"

void	CheckExtension(std::string filename) {
	std::string	extention;
	int			index;

	index = filename.find(".");
	if (index == -1)
		throw ConfigParser::ConfFileNameError();
	extention = filename.substr(index);
	if (extention.compare(".conf") != 0)
		throw ConfigParser::ConfFileNameError();
}

void	CheckBlock(std::string filename, ConfigParser *parser) {
	std::ifstream	ifs(filename, std::ifstream::in);

	if (ifs.is_open() == false)
		throw ConfigParser::ConfFileOpenError();
	parser->InitParserClass();
	parser->StartParsing(&ifs);
	parser->CheckHttpBlock();
	parser->PrintParseError(filename);
}