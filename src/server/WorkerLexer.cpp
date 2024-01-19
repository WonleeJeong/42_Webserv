#include "../../inc/Worker.hpp"
#include "../../inc/BlockParser.hpp"

void	WorkerThrowError() {
	throw ConfigParser::ConfFileError();
}

Worker SetWorkerInfo(std::vector<std::string>& lines) {
	Worker worker;

	std::map<std::string, bool> server_tokens;
	server_tokens["listen"] = false;
	server_tokens["server_name"] = false;
	server_tokens["error_page"] = false;
	server_tokens["client_max_body_size"] = false;
	server_tokens["root"] = false;
	server_tokens["index"] = false;
	server_tokens["location"] = false;

	std::vector<std::string>::iterator lineIt;
	for (lineIt = lines.begin() + 2; lineIt != lines.end(); ++lineIt) {
		std::string line = *lineIt;

		if (line == "location") {
			lineIt = SetLocation(worker, lines, ++lineIt);
			line = *lineIt;
		}

		std::map<std::string, bool>::iterator tokenIt;
		tokenIt = server_tokens.find(line);

		if (lineIt == lines.end() - 1 || lineIt == lines.end())
			break ;

		if (tokenIt != server_tokens.end()) {
			if (tokenIt->second && tokenIt->first != "location")
				WorkerThrowError();
			else {
				lineIt = SetWorker(worker, lines, lineIt);
				tokenIt->second = true;
			}
		}
		else
			WorkerThrowError();
	}
	if (worker.GetLocations().size() == 0)
		throw::ConfigParser::ConfFileError();
	worker.CheckBlockContents(server_tokens);
	return worker;
}

void ParseListen(Worker& worker, const std::string& line) {
	std::stringstream ss(line);
	double value = 0.0;
	char suffix = '\0';

	ss >> value >> suffix;

	if (value && !suffix && value >= 0 && value <= PORT_NUM_MAX)
		worker.SetPort((int)value);
	else
		WorkerThrowError();
}

void ParseErrorPage(Worker& worker, std::vector<std::string>& lines, std::vector<std::string>::iterator& lineIt) {
	while (lineIt != lines.end() && (*lineIt).compare(";"))
	{
		std::stringstream ss(*lineIt);
		double value = 0.0;
		char suffix = '\0';

		ss >> value >> suffix;

		if (value && !suffix)
			worker.AddErrorPage((int)value, *(++lineIt));
		else
			WorkerThrowError();
		lineIt++;
	}
	lineIt--;
}

void ParseClientMaxBodySize(Worker& worker, const std::string& line) {
	std::stringstream ss(line);
	double value = 0.0;
	char suffix = '\0';

	ss >> value >> suffix;

	if (!suffix && value)
		worker.SetClientMaxBodySize(value);
	else if (tolower(suffix) == 'm')
		worker.SetClientMaxBodySize(value * 1000000);
	else
		WorkerThrowError();
}

std::vector<std::string>::iterator SetWorker(Worker& worker, std::vector<std::string> lines, std::vector<std::string>::iterator& lineIt) {
	const std::string& line = *lineIt;

	if (line == "location")
        return --lineIt;

	if (line == "listen")
		ParseListen(worker, *(++lineIt));
	else if (line == "error_page")
		ParseErrorPage(worker, lines, ++lineIt);
	else if (line == "server_name")
		worker.AddServerName(*(++lineIt));
	else if (line == "client_max_body_size")
		ParseClientMaxBodySize(worker, *(++lineIt));
	else if (line == "root")
		worker.SetRoot(*(++lineIt));
	else if (line == "index")
		worker.SetIndex(*(++lineIt));

	lineIt++;
	if ((*lineIt) != ";")
		WorkerThrowError();
	return lineIt;
}

void	Worker::CheckBlockContents(std::map<std::string, bool> &server_tokens) {
	if (server_tokens["listen"] == false)
		throw::ConfigParser::ConfFileError();
	if (server_tokens["server_name"] == false)
		throw::ConfigParser::ConfFileError();
	if (server_tokens["error_page"] == false)
		throw::ConfigParser::ConfFileError();
	if (server_tokens["client_max_body_size"] == false)
		throw::ConfigParser::ConfFileError();
	if (server_tokens["root"] == false)
		throw::ConfigParser::ConfFileError();
	if (server_tokens["index"] == false)
		throw::ConfigParser::ConfFileError();
}