#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <fcntl.h>
#include <fstream>

#include "Worker.hpp"
#include "Request.hpp"
#include "CgiHandler.hpp"

#define AUTO_INDEX_HTML_PATH "./templates/autoindex.html"
#define REDIRECTION_HTML_PATH "./templates/301redirection.html"

class Worker;
class Request;

enum ResponseType {
	GENERAL,
	CGI
};

enum HttpStatusCode
{
	OK = 200,
	CREATED = 201,
	MOVED_PERMANENTLY = 301,
	BAD_REQUEST = 400,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	REQUEST_TIMEOUT = 408,
	LENGTH_REQUIRED = 411,
	PAYLOAD_TOO_LARGE = 413,
	URI_TOO_LONG = 414,
	UNSUPPORTED_MEDIA_TYPE = 415,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	HTTP_VERSION_NOT_SUPPORTED = 505
};

class Response {
    private:
		ResponseType    type_;
        HttpStatusCode  status_code_;
        std::string     http_version_;
		std::string     allow_;
   	 	std::string     connection_;
   		std::string     location_;
   		std::string     content_type_;
		std::vector<char>           body_;
		std::map<int, std::string>  error_pages_;

		std::string ToString(int num);
        std::string GetStatusMessage(int code);
        void ReadFileToBody(const std::string &path);
        void GenerateBodyAutoIndexing(const Request &request);
        std::vector<std::string> GetFilesInDirectory(const std::string &dirPath);

    public:
        Response();
        ~Response();
		Response& operator=(const Response& response);

		void SetErrorPages(std::map<int, std::string> error_pages);
		void SetAllow(std::string allow);
		HttpStatusCode GetStatusCode() const;
        void SetStatusCode(HttpStatusCode status);
        void SendResponse(int fd);
		ResponseType GetResponseType() const;
		void SetResponseType(ResponseType type);
		void SetConnection(std::string connection);

        void HandleGet(const Request &request, const std::string index, const bool autoindex);
        void HandlePost(const Request &request, const std::string index_path);
        void HandleDelete(const Request &request);
        void SetHttpVersion(std::string version);
		void PushBackBody(char c);
		int FindStringInBody(std::string str);

		void MakeErrorResponse(HttpStatusCode status);
		void MakeRedirectionResponse(std::string redir_path);

		void MakeIndexResponse(const Request &request, std::string index_path);
		void MakePostResponse(const Request &request);
};

#endif
