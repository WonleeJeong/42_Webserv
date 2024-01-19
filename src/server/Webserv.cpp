#include "../../inc/Webserv.hpp"
#include <iostream>

void	Webserv::Run() {
	for (size_t i = 0; i < workers_.size(); i++)
		server_sockets_.push_back(workers_[i].GetServerSocket());
	while (true) {
		int	n = kevent(kq_, &change_list_[0], change_list_.size(), events_, 1024, NULL);
		if (n == -1) {
			for (size_t j = 0; j < workers_.size(); j++)
				close(workers_[j].GetPort());
			throw Worker::KeventError();
		}
		change_list_.clear();

		for (int i = 0; i < n; i++) {
			curr_event_ = &events_[i];
			event_data_ = (WorkerData *)curr_event_->udata;

			if (curr_event_->flags & EV_ERROR)
				continue ;
			if (event_data_ == NULL && curr_event_->filter == EVFILT_READ) {
				if (SockReceiveData() == -1 && i != 1)
					continue ;
			}
			else if (event_data_->GetEventType() == CLIENTEVENT && curr_event_->filter == EVFILT_READ) {
				if (SockReceiveData() == -1 && i != 1)
					continue ;
			}
			else if (event_data_->GetEventType() == CLIENTEVENT && curr_event_->filter == EVFILT_WRITE) {
				SockSendData();
			}
			else if (event_data_->GetEventType() == CGIEVENT && curr_event_->filter == EVFILT_READ) {
				ReadCgiResponse();
			}
			else if (event_data_->GetEventType() == CGIEVENT && curr_event_->filter == EVFILT_WRITE) {
				WriteCgiInput();
			}
			else if (event_data_->GetEventType() == CGIEVENT && curr_event_->filter == EVFILT_TIMER) {
				event_data_->GetResponse().SetConnection("Close");
				event_data_->GetResponse().SetStatusCode(REQUEST_TIMEOUT);

				kill(event_data_->GetCgiHandler().GetPid(), SIGTERM);
				event_data_->GetCgiHandler().ClosePipeAfterWrite();

				uintptr_t write_ident = event_data_->GetCgiHandler().GetClientWriteIdent();
				event_data_->SetEventType(CLIENTEVENT);
				ChangeEvent(change_list_, write_ident, EVFILT_READ, EV_DISABLE, 0, 0, event_data_);
				ChangeEvent(change_list_, write_ident, EVFILT_WRITE, EV_ENABLE, 0, 0, event_data_);

				ChangeEvent(change_list_, event_data_->GetCgiHandler().GetReadFd(), EVFILT_READ, EV_DISABLE, 0, 0, NULL);
				ChangeEvent(change_list_, curr_event_->ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
				ChangeEvent(change_list_, curr_event_->ident, EVFILT_TIMER, EV_DISABLE, 0, 0, NULL);
				close (curr_event_->ident);
			}
		}
	}
}

void Webserv::WriteCgiInput() {
	event_data_->GetCgiHandler().ClosePipeBeforeWrite();

	ChangeEvent(change_list_, curr_event_->ident, EVFILT_TIMER, EV_ADD | EV_ENABLE, NOTE_SECONDS, TIMEOUT, event_data_);

	if (event_data_->GetRequest().GetMethod() == "POST")
	{
		std::vector<char> v = event_data_->GetRequest().GetBody();
		std::string s(v.begin(), v.end());
		write(event_data_->GetCgiHandler().GetWriteFd(), s.c_str(), s.length());
	}
	event_data_->GetCgiHandler().ClosePipeAfterWrite();
	event_data_->GetCgiHandler().SetState(WRITE_PIPE);
	ChangeEvent(change_list_, curr_event_->ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
	close(curr_event_->ident);
}

void Webserv::ReadCgiResponse() {
	event_data_->GetCgiHandler().ClosePipeBeforeRead();

	char buff[4096];
	ssize_t bytesRead;
	std::vector<char> body;

	while ((bytesRead = read(event_data_->GetCgiHandler().GetReadFd(), buff, sizeof(buff))) > 0)
	{
		for (int i = 0; i < bytesRead; i++)
			event_data_->GetResponse().PushBackBody(buff[i]);
	}
	event_data_->GetCgiHandler().ClosePipeAfterRead();
	event_data_->GetCgiHandler().SetState(READ_PIPE);

	if (event_data_->GetRequest().GetMethod() == "POST") {
		if (event_data_->GetResponse().FindStringInBody("Invalid file extension"))
			event_data_->GetResponse().SetStatusCode(UNSUPPORTED_MEDIA_TYPE);
		else if (event_data_->GetResponse().FindStringInBody("A problem occurred in a Python script."))
			event_data_->GetResponse().SetStatusCode(BAD_REQUEST);
		else
			event_data_->GetResponse().SetStatusCode(CREATED);
	}

	uintptr_t write_ident = event_data_->GetCgiHandler().GetClientWriteIdent();
	event_data_->SetEventType(CLIENTEVENT);
	ChangeEvent(change_list_, write_ident, EVFILT_READ, EV_DISABLE, 0, 0, event_data_);
	ChangeEvent(change_list_, write_ident, EVFILT_WRITE, EV_ENABLE, 0, 0, event_data_);

	ChangeEvent(change_list_, curr_event_->ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
	close (curr_event_->ident);
}

Webserv::Webserv() {
	this->location_idx_ = 0;
}

Webserv::~Webserv() {}