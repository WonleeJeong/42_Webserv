#ifndef WORKER_HPP
#define WORKER_HPP

#define PORT_NUM_MAX 65535

#include <iostream>
#include <string.h>
#include <vector>
#include <map>
#include "Location.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Location;
class Request;
class Response;

class Worker {
private:
    int 						server_socket_;
    int 						port_;
	std::string 				root_;
	std::string 				index_;
    std::vector<std::string>	server_names_;
    std::map<int, std::string>	error_pages_;
    int							client_max_body_size_worker_;
	std::vector<Location>		locations_;

public:
    Worker();
    ~Worker();
    void	SetServerSocket(int server_socket_);
    int		GetServerSocket();
    void	SetPort(int port);
    int		GetPort();
	void	SetRoot(std::string& root);
    const	std::string& GetRoot() const;
	void	SetIndex(std::string& index);
    const	std::string& GetIndex() const;
    void	AddServerName(std::string& server_name);
    const	std::vector<std::string>& GetServerNames() const;
    void	SetClientMaxBodySize(int size);
    int		GetClientMaxBodySize() const;
    void	AddLocations(const Location& location);
    const	std::vector<Location>& GetLocations() const;
	void	AddErrorPage(int error_code, std::string& error_page);
    const	std::map<int, std::string>& GetErrorPage() const;
	size_t	MyStoi(std::string str);
	std::vector<std::string> SplitArgs(std::string line, std::string sep);
	size_t	CheckContentLength(std::string headers);
	std::vector<std::string> Split(std::string input, char dlim, int &result_cnt);
	void	ReqFirstLineParse(Request &req, std::string first_line);
	void	ParseHost(Request &req, std::vector<std::string> colon_parse);
	void	ParseConnection(Request &req, std::vector<std::string> colon_parse);
	void	ParseContentLength(Request &req, std::vector<std::string> colon_parse);
	void	ParseOther(Request &req, std::vector <std::string> line_parse, int line_cnt);
	void	RequestHeaderParse(Request &req);
	void	ChunkBodyParse(Request &req, Response &res);
	void	CheckBlockContents(std::map<std::string, bool> &server_tokens);
	class	BindError: public std::exception
	{
		public:
			virtual const char	*what() const throw();
	};
	class	ListenError: public std::exception
	{
		public:
			virtual const char	*what() const throw();
	};
	class	KqueueError: public std::exception
	{
		public:
			virtual const char	*what() const throw();
	};
	class	KeventError: public std::exception
	{
		public:
			virtual const char	*what() const throw();
	};
	class	AcceptError: public std::exception
	{
		public:
			virtual const char	*what() const throw();
	};
	class	ReadError: public std::exception
	{
		public:
			virtual const char	*what() const throw();
	};
	class	UnkownError: public std::exception
	{
		public:
			virtual const char	*what() const throw();
	};
};
void	WorkerThrowError();
std::vector<std::string>::iterator SetWorker(Worker& worker, std::vector<std::string> lines, std::vector<std::string>::iterator& lineIt);
Worker SetWorkerInfo(std::vector<std::string>& lines);
void ParseListen(Worker& worker, const std::string& line);
void ParseErrorPage(Worker& worker, std::vector<std::string>& lines, std::vector<std::string>::iterator& lineIt);
void ParseClientMaxBodySize(Worker& worker, const std::string& line);


#endif