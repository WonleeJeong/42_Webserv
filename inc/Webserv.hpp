#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <unistd.h>
# include <signal.h>
# include "Worker.hpp"
# include "WorkerData.hpp"
# include "Request.hpp"
# include "BlockParser.hpp"
# include "CgiHandler.hpp"

# define TIMEOUT 30
# define BUFFER_SIZE 100000

const std::string CRLF = "\r\n";

class Webserv {
	private:
		int								kq_;
		std::vector<struct kevent>		change_list_;
		std::map<int, int>				find_fd_;
		std::vector<Worker>				workers_;
		struct kevent					*curr_event_;
		std::vector<int>				server_sockets_;
		std::vector<int>::iterator		it;
		std::vector<Worker>::iterator	wit_;
		int                             location_idx_;
		std::map<int, int>::iterator	mapter_;
		std::map<int, std::string>		status_messages_;
		WorkerData	        			*event_data_;
		std::vector<char>				buffer_;
		struct kevent					events_[1024];

		ssize_t	ReadData(int fd, char *buffer_, size_t buffer_size);
		void	ReadyToConnect(int i);
		void	SendResponse(int client_socket, int status_code, const std::string &content);
		void	HandleRequest(int client_socket);
		int		ConnectNewClient();
		int		StartReceiveData(int len);
		int		SockReceiveData();
		int		ReadHeader();
		void	ReadBody();
		void	ReadFinish();
		void    AddCgiEvent();
		void    CheckRequestError();
		void    CheckRedirection();
		void	SockSendData();
		void	MakeResponse();
		void	ChangeEvent(std::vector<struct kevent>& change_list_, uintptr_t ident, int16_t filter,	uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
		void	SetCgiEvent();
		void    WriteCgiInput();
		void    ReadCgiResponse();

    public:
		Webserv();
		~Webserv();
		void	ConfParse(char *conf_file);
		void	Init();
		void	Run();
};

#endif