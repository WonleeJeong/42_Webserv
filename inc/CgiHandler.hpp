#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unistd.h>

#include "Request.hpp"
#include "Response.hpp"

class Request;
class Response;
class Webserv;

enum CgiState {
	NOT_CALLED,
	CREATE_PIPE,
	WRITE_PIPE,
	READ_PIPE
};

class CgiHandler {
private:
    pid_t pid_;
    int to_cgi_[2];
    int from_cgi_[2];
    std::string cgi_path_;
    std::string query_string_;
    std::vector<char*> envp_;
    std::map<std::string, std::string> env_;
	CgiState state_;
	uintptr_t client_write_ident_;

public:
    CgiHandler();
    ~CgiHandler();
	CgiHandler& operator=(const CgiHandler& cgi);

    pid_t GetPid() const;
    int GetWriteFd() const;
    int GetReadFd() const;
	void ClosePipeBeforeRead();
	void ClosePipeBeforeWrite();
	void ClosePipeAfterRead();
	void ClosePipeAfterWrite();
    void ExecuteChildProcess(const Request &request, Response &response);
    void FillEnv(const Request &request);
    void ConvertEnv();
	CgiState GetState() const;
	void SetState(CgiState state);
	void SetClientWriteIdent(uintptr_t ident);
	uintptr_t GetClientWriteIdent(void) const;
};

#endif
