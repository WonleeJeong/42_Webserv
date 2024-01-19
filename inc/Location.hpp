#ifndef LOCATION_HPP
# define LOCATION_HPP

#define METHOD_GET 0
#define METHOD_POST 1
#define METHOD_PUT 2
#define METHOD_DELETE 3

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>
#include <fcntl.h>
#include <sys/event.h>
#include <algorithm>
#include <unistd.h>
#include <sstream>
#include <string>

class Worker;

#include "Worker.hpp"

class Location {
private:
	std::string			uri_;
    std::string			index_;
    bool				auto_index_;
	int					redir_status_code_;
	std::string			redir_uri;
	int					client_max_body_size_location_;
    std::map<int, bool> limit_excepts_;

public:
    Location();
    ~Location();
	void SetUri(const std::string& uri);
	void SetIndex(const std::string& index);
	void SetRedirStatusCode(int status);
	void SetRedirUri(const std::string& uri);
	void SetLimitExcepts(int method, bool access);
	void SetAutoIndex(bool access);
	void SetClientMaxBodySizeLocation(int size);
	const std::string& GetUri() const;
	const std::string& GetIndex() const;
	int GetRedirStatusCode() const;
	const std::string& GetRedirUri() const;
	std::map<int, bool> GetLimitExcepts() const;
	bool GetAutoIndex() const;
	int GetClientMaxBodySizeLocation() const;
};

std::vector<std::string>::iterator SetLocationToken(Location& location, std::vector<std::string> lines, std::vector<std::string>::iterator& lineIt);
std::vector<std::string>::iterator SetLocation(Worker& worker, std::vector<std::string> lines, std::vector<std::string>::iterator& lineIt);
void	CheckLocationToken(Location& location, std::vector<std::string> lines, std::vector<std::string>::iterator& lineIt);
void	ParseLimitExcept(Location& location, std::vector<std::string> lines, std::vector<std::string>::iterator& lineIt);
void	ParseAutoIndex(Location& location, const std::string line);
void	ParseRedirection(Location& location, std::vector<std::string>::iterator& lineIt);
void	ParseClientMaxBodySizeLocation(Location& location, const std::string& line);

#endif