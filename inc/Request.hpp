#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Worker.hpp"
#include <sys/time.h>

enum ParseState {
	HEADER_READ,
	BODY_READ,
	READ_FINISH
};

enum PathInfo {
	IS_DIRECTORY,
	IS_FILE,
	NOT_EXIST
};

class Request {
	private:
		ParseState	parse_state_;
		std::string headers_;
		std::string http_method_;
        std::string path_;
        std::string full_path_;
		PathInfo    path_info_;
        std::string	scheme_;
        std::vector <std::string> host_;
        std::string	connection_;
        int	        content_length_;
		std::string transfer_encoding_;
		std::string	body_str_;
		std::string http_version_;
   		std::string location_; 
   		std::string content_type_;
		std::vector <char> body_;

	public:
		Request();
		~Request();
		Request& operator=(const Request& request);

		void	SetMethod(std::string method);
		void	SetPath(std::string path);
        void    SetFullPath(std::string full_path);
		void    SetPathInfo();
		void	SetScheme(std::string scheme);
		void	PushBackHost(std::string host);
		void	SetConnection(std::string connection);
		void	SetContentLength(int length);
		void	SetState(ParseState state);
		void	SetHeaders(std::string data);
		void	SetBodyClear();
		void	SetContentType(std::string type);
		void    SetTransferEncoding(std::string type);
		std::string	GetMethod() const;
		std::string	GetPath() const;
        std::string GetFullPath() const;
		std::string	GetScheme() const;
		std::vector<std::string>	GetHost() const;
		std::string	GetConnection() const;
		int GetContentLength() const;
		std::string GetHeaders() const;
		ParseState	GetState() const;
		std::vector<char> GetBody() const;
        std::string GetContentType() const;
		std::string GetBodyStr() const;
		std::string GetBodyCharToStr() const;
		std::string GetTransferEncoding() const;
		PathInfo    GetPathInfo() const;

		void	AppendHeader(std::string data);
		void	AppendBodyStr(std::string data);
		void	PushPostBody(char data);
		void	BodyAppendVec(std::vector<char> data);
		void	RemoveCRLF();

		int		Findrn0rn(std::string str);
		void	AddRNRNOneTime();
		void	RemoveRNRNOneTime();
};

#endif