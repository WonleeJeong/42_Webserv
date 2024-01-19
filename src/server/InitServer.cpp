#include "../../inc/Webserv.hpp"

void	Webserv::ConfParse(char *conf_file) {
	ConfigParser 	parser;

	CheckExtension(conf_file);
	CheckBlock(conf_file, &parser);
    for (size_t i = 0; i < parser.GetServer().size(); i++) {
        Worker worker = SetWorkerInfo(parser.server_[i]);
		this->workers_.push_back(worker);
    }
}

void	Webserv::Init() {
	for(size_t i = 0; i < this->workers_.size(); i++)
		ReadyToConnect(i);
	kq_ = kqueue();
	if (kq_ == -1) {
		for (size_t j = 0; j < this->workers_.size(); j++)
			close(this->workers_[j].GetPort());
		throw Worker::KqueueError();
	}
	for (size_t i = 0;  i < this->workers_.size(); i++)
		ChangeEvent(change_list_, this->workers_[i].GetServerSocket(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
}

int	Webserv::ConnectNewClient() {
	it = find(server_sockets_.begin(),  server_sockets_.end(), curr_event_->ident);
	int tmp_cli_sock = accept(*it, NULL, NULL);
	if (tmp_cli_sock== -1)
		return -1 ;
	fcntl(tmp_cli_sock, F_SETFL, O_NONBLOCK);
	WorkerData *udata = new WorkerData(*new Request(), *new Response(), *new CgiHandler(), CLIENTEVENT);
	ChangeEvent(change_list_, tmp_cli_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, udata);
	ChangeEvent(change_list_, tmp_cli_sock, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, udata);
	find_fd_[tmp_cli_sock] = curr_event_->ident;
	return 0;
}

void	Webserv::ReadyToConnect(int i) {
	sockaddr_in	server_address;

	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(workers_[i].GetPort());
	if (bind(this->workers_[i].GetServerSocket(), (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
		for (size_t j = 0; j < this->workers_.size(); j++)
			close(this->workers_[j].GetPort());
		throw Worker::BindError();
	}
	if (listen(this->workers_[i].GetServerSocket(), 15) == -1) {
		for (size_t j = 0; j < this->workers_.size(); j++)
			close(this->workers_[j].GetPort());
		throw Worker::ListenError();
	}
	fcntl(this->workers_[i].GetServerSocket(), F_SETFL, O_NONBLOCK);
}