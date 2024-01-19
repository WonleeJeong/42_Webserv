#include "../../inc/Worker.hpp"
#include "../../inc/Location.hpp"

std::vector<std::string>::iterator SetLocation(Worker& worker, std::vector<std::string> lines, std::vector<std::string>::iterator& lineIt) {
	Location location;

	location.SetIndex(worker.GetIndex());
	location.SetUri(*(lineIt++));
	if (worker.GetClientMaxBodySize() != -1)
		location.SetClientMaxBodySizeLocation(worker.GetClientMaxBodySize());
	CheckLocationToken(location, lines, lineIt);
	worker.AddLocations(location);
	return ++lineIt;
}

void	ParseLimitExcept(Location& location, std::vector<std::string> lines, std::vector<std::string>::iterator& lineIt) {
	std::map<int, bool> method_tokens;

	method_tokens[METHOD_GET] = false;
	method_tokens[METHOD_POST] = false;
	method_tokens[METHOD_PUT] = false;
	method_tokens[METHOD_DELETE] = false;

	while(lineIt != lines.end() && *lineIt != ";") {
		if (!(*lineIt).compare("GET") && !method_tokens[METHOD_GET])
			method_tokens[METHOD_GET] = true;
		else if (!(*lineIt).compare("POST") && !method_tokens[METHOD_POST])
			method_tokens[METHOD_POST] = true;
		else if (!(*lineIt).compare("PUT") && !method_tokens[METHOD_PUT])
			method_tokens[METHOD_PUT] = true;
		else if (!(*lineIt).compare("DELETE") && !method_tokens[METHOD_DELETE])
			method_tokens[METHOD_DELETE] = true;
		else
			WorkerThrowError();
		lineIt++;
	}
	--lineIt;

	std::map<int, bool>::iterator method_it;
	for (method_it = method_tokens.begin(); method_it != method_tokens.end(); ++method_it)
		location.SetLimitExcepts(method_it->first, method_it->second);
}

void ParseAutoIndex(Location& location, const std::string line) {
	if (line == "on")
		location.SetAutoIndex(true);
	else if (line == "off")
		location.SetAutoIndex(false);
	else
		WorkerThrowError();
}

void ParseRedirection(Location& location, std::vector<std::string>::iterator& lineIt) {
	std::stringstream ss(*lineIt);
	double value = 0.0;
	char suffix = '\0';

	ss >> value >> suffix;

	if (value && !suffix)
		location.SetRedirStatusCode(value);
	else
		WorkerThrowError();
	lineIt++;
	location.SetRedirUri(*lineIt);
}

std::vector<std::string>::iterator SetLocationToken(Location& location, std::vector<std::string> lines, std::vector<std::string>::iterator& lineIt) {
	const std::string& line = *lineIt;

	if (line == "limit_except")
		ParseLimitExcept(location, lines, ++lineIt);
	else if (line == "index")
		location.SetIndex(*(++lineIt));
	else if (line == "autoindex")
		ParseAutoIndex(location, *(++lineIt));
	else if (line == "client_max_body_size")
		ParseClientMaxBodySizeLocation(location, *(++lineIt));
	else if (line == "return")
		ParseRedirection(location, ++lineIt);


	lineIt++;
	if (*lineIt != ";")
		WorkerThrowError();
	return lineIt;
}

void	CheckLocationToken(Location& location, std::vector<std::string> lines, std::vector<std::string>::iterator& lineIt) {
	std::map<std::string, bool> location_tokens;

	location_tokens["limit_except"] = false;
	location_tokens["index"] = false;
	location_tokens["autoindex"] = false;
	location_tokens["client_max_body_size"] = false;
	location_tokens["return"] = false;

	if (*(lineIt++) != "{")
		WorkerThrowError();

	while(lineIt != lines.end() && (*lineIt) != "}")
	{
		const std::string& line = *lineIt;

		std::map<std::string, bool>::iterator tokenIt;
		tokenIt = location_tokens.find(line);

		if (tokenIt != location_tokens.end()) {
			if (tokenIt->second)
				WorkerThrowError();
			else
			{
				lineIt = SetLocationToken(location, lines, lineIt);
				tokenIt->second = true;
			}
		}
		else
			WorkerThrowError();
		lineIt++;
	}

	if (*lineIt != "}")
		WorkerThrowError();
}

void ParseClientMaxBodySizeLocation(Location& location, const std::string& line) {
	std::stringstream ss(line);
	double value = 0.0;
	char suffix = '\0';

	ss >> value >> suffix;

	if (!suffix && value)
		location.SetClientMaxBodySizeLocation(value);
	else if (tolower(suffix) == 'm')
		location.SetClientMaxBodySizeLocation(value * 1000000);
	else
		WorkerThrowError();
}