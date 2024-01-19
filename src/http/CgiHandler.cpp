#include "../../inc/CgiHandler.hpp"

CgiHandler::CgiHandler() {
	this->state_ = NOT_CALLED;
    this->query_string_ = "";
	this->client_write_ident_ = -1;
}

CgiHandler::~CgiHandler() {
	std::vector<char*>::iterator it;
	for(it = envp_.begin(); it != envp_.end(); it++)
		delete *it;
}

CgiHandler& CgiHandler::operator=(const CgiHandler& cgi) {
	this->pid_ = cgi.pid_;
	this->to_cgi_[0] = cgi.to_cgi_[0];
	this->to_cgi_[1] = cgi.to_cgi_[1];
	this->from_cgi_[0] = cgi.from_cgi_[0];
	this->from_cgi_[1] = cgi.from_cgi_[1];
	this->cgi_path_ = cgi.cgi_path_;
	this->query_string_ = cgi.query_string_;
	this->envp_ = cgi.envp_;
	this->env_ = cgi.env_;
	return *this;
}

pid_t CgiHandler::GetPid() const {
    return (this->pid_);
}

int CgiHandler::GetWriteFd() const {
    return (this->to_cgi_[1]);
}

int CgiHandler::GetReadFd() const {
    return (this->from_cgi_[0]);
}

void CgiHandler::ExecuteChildProcess(const Request &request, Response &response) {
    this->cgi_path_ = request.GetFullPath();

    if (request.GetMethod() == "GET") {
        size_t query_index = this->cgi_path_.find("?", 0);
        if (query_index != std::string::npos)
        {
            this->query_string_ = this->cgi_path_.substr(query_index + 1, this->cgi_path_.size() - query_index);
            this->cgi_path_ = this->cgi_path_.substr(0, query_index);
        }
    }

	 if (pipe(this->to_cgi_) < 0 || pipe(this->from_cgi_) < 0)
	      return response.SetStatusCode(INTERNAL_SERVER_ERROR);
	this->state_ = CREATE_PIPE;

    this->pid_ = fork();
    if (this->pid_ == -1)
	    return response.SetStatusCode(INTERNAL_SERVER_ERROR);
    else if (this->pid_ == 0)
    {
        close(this->to_cgi_[1]);
        close(this->from_cgi_[0]);

        dup2(this->to_cgi_[0], STDIN_FILENO);
        dup2(this->from_cgi_[1], STDOUT_FILENO);

        close(this->to_cgi_[0]);
        close(this->from_cgi_[1]);

        FillEnv(request);

		if (request.GetPath().find(".bla") != std::string::npos)
			execve("./cgi_tester", NULL, this->envp_.data());
		else
        	execve(this->cgi_path_.c_str(), NULL, this->envp_.data());
    }
}

void CgiHandler::FillEnv(const Request &request) {
    if (request.GetMethod() == "POST")
    {
        env_["CONTENT_LENGTH"] = "1000000000";
        env_["CONTENT_TYPE"] = request.GetContentType();
    }
	env_["PATH_INFO"] = this->cgi_path_;
	if (request.GetMethod() == "GET")
		env_["QUERY_STRING"] = this->query_string_;
	env_["REQUEST_METHOD"] = request.GetMethod();
	env_["SERVER_PROTOCOL"] = "HTTP/1.1";
  	ConvertEnv();
}

void CgiHandler::ConvertEnv() {
    std::map<std::string, std::string>::iterator it;
    for (it = env_.begin(); it != env_.end(); it++)
    {
        std::string concat = it->first + "=" + it->second;
        char *str = new char[concat.length() + 1];
        strcpy(str, concat.c_str());
        envp_.push_back(str);
    }
    envp_.push_back(0);
}

void CgiHandler::ClosePipeBeforeRead() {
}

void CgiHandler::ClosePipeBeforeWrite() {
	close(to_cgi_[0]);
	close(from_cgi_[1]);
}

void CgiHandler::ClosePipeAfterRead() {
	close(from_cgi_[0]);
}

void CgiHandler::ClosePipeAfterWrite() {
	close(to_cgi_[1]);
}

CgiState CgiHandler::GetState() const {
	return (this->state_);
}

void CgiHandler::SetState(CgiState state) {
	this->state_ = state;
}

void CgiHandler::SetClientWriteIdent(uintptr_t ident) {
	this->client_write_ident_ = ident;
}

uintptr_t CgiHandler::GetClientWriteIdent() const {
	return (this->client_write_ident_);
}