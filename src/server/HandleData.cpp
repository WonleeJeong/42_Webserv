#include "../../inc/Webserv.hpp"
#include "../../inc/CgiHandler.hpp"

int	Webserv::SockReceiveData() {
	if (find(server_sockets_.begin(), server_sockets_.end(), curr_event_->ident) != server_sockets_.end()) {
		if (ConnectNewClient() == -1)
			return -1;
	}
	else if (find(server_sockets_.begin(), server_sockets_.end(), curr_event_->ident) == server_sockets_.end()) {
		buffer_.clear();
		buffer_.resize(BUFFER_SIZE);
		mapter_ = find_fd_.find(curr_event_->ident);
		for (wit_ = workers_.begin(); wit_ != workers_.end(); ++wit_)
			if (wit_->GetServerSocket() == mapter_->second)
				break ;
		ssize_t len = ReadData(curr_event_->ident, buffer_.data(), BUFFER_SIZE);
		if (len > 0) {
			if (StartReceiveData(len) == -1)
				return -1;
		}
		else if (len == 0) {
			close (curr_event_->ident);
		}
		else
			return -1;
		if (event_data_->GetRequest().GetState() == READ_FINISH) {
			ReadFinish();
			CheckRequestError();
			CheckRedirection();
			if (this->event_data_->GetRequest().GetPath().find(".py") != std::string::npos || (this->event_data_->GetRequest().GetMethod() == "POST" && this->event_data_->GetRequest().GetPath().find(".bla") != std::string::npos)) {
				this->event_data_->GetResponse().SetResponseType(CGI);
				this->event_data_->GetCgiHandler().SetClientWriteIdent(curr_event_->ident);
				AddCgiEvent();
			}
			else {
				ChangeEvent(change_list_, curr_event_->ident, EVFILT_READ, EV_DISABLE, 0, 0, curr_event_->udata);
				ChangeEvent(change_list_, curr_event_->ident, EVFILT_WRITE, EV_ENABLE, 0, 0, curr_event_->udata);
			}
		}
	}
	return 0;
}

void	Webserv::SockSendData() {
	event_data_->GetResponse().SetErrorPages(wit_->GetErrorPage());
	if (event_data_->GetResponse().GetStatusCode() == MOVED_PERMANENTLY)
		event_data_->GetResponse().MakeRedirectionResponse(wit_->GetLocations()[location_idx_].GetRedirUri());
	else if (event_data_->GetResponse().GetStatusCode() != OK && event_data_->GetResponse().GetStatusCode() != CREATED)
		event_data_->GetResponse().MakeErrorResponse(event_data_->GetResponse().GetStatusCode());
	else if (this->event_data_->GetResponse().GetResponseType() == GENERAL)
		MakeResponse();
	this->event_data_->GetResponse().SendResponse(curr_event_->ident);
	std::map<int, int>::iterator tmp_fd_iter = find_fd_.find(curr_event_->ident);
	find_fd_.erase(tmp_fd_iter);
	delete ((WorkerData *)curr_event_->udata);
	ChangeEvent(change_list_, curr_event_->ident, EVFILT_READ, EV_ENABLE, 0, 0, NULL);
	ChangeEvent(change_list_, curr_event_->ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
	close (curr_event_->ident);
}

int	Webserv::StartReceiveData(int len) {
	buffer_.resize(len);
	if (event_data_->GetRequest().GetState() == HEADER_READ)
	{
		if (ReadHeader() == -1)
			return -1;
	}
	else if (event_data_->GetRequest().GetState() == BODY_READ)
		ReadBody();

	return 0;
}

int	Webserv::ReadHeader() {
	std::string temp_data(buffer_.begin(), buffer_.end());
	event_data_->GetRequest().AppendHeader(temp_data);
	event_data_->GetRequest().BodyAppendVec(buffer_);
	size_t pos = event_data_->GetRequest().GetHeaders().find("\r\n\r\n");
	if (pos == std::string::npos)
		return -1;
	else
	{
		std::string temp = event_data_->GetRequest().GetHeaders();
		event_data_->GetRequest().SetHeaders(event_data_->GetRequest().GetHeaders().substr(0, pos + 4));
		event_data_->GetRequest().RemoveCRLF();
		if (event_data_->GetRequest().GetHeaders().find("Content-Length") != std::string::npos)
		{
			size_t body_size = wit_->CheckContentLength(event_data_->GetRequest().GetHeaders());
			if (body_size <= event_data_->GetRequest().GetBody().size())
				event_data_->GetRequest().SetState(READ_FINISH);
			else
				event_data_->GetRequest().SetState(BODY_READ);
		}
		else if (event_data_->GetRequest().GetHeaders().find("Transfer-Encoding") != std::string::npos)
		{
			event_data_->GetRequest().SetTransferEncoding("chunked");
			event_data_->GetRequest().AddRNRNOneTime();
			std::string temp = event_data_->GetRequest().GetBodyCharToStr();
			if (event_data_->GetRequest().Findrn0rn(temp) == 1)
				event_data_->GetRequest().SetState(READ_FINISH);
			else
				event_data_->GetRequest().SetState(BODY_READ);
		}
		else
			event_data_->GetRequest().SetState(READ_FINISH);
	}
	return 0;
}

void	Webserv::ReadBody() {
	event_data_->GetRequest().BodyAppendVec(buffer_);
	std::string temp(buffer_.begin(), buffer_.end());
	if (event_data_->GetRequest().GetHeaders().find("Content-Length") != std::string::npos)
	{
		size_t body_size = wit_->CheckContentLength(event_data_->GetRequest().GetHeaders());
		if (body_size <= event_data_->GetRequest().GetBody().size())
			event_data_->GetRequest().SetState(READ_FINISH);
	}
	else if (event_data_->GetRequest().GetHeaders().find("Transfer-Encoding") != std::string::npos)
	{
		if (event_data_->GetRequest().Findrn0rn(temp) == 1)
			event_data_->GetRequest().SetState(READ_FINISH);
	}
}

void	Webserv::ReadFinish() {
	wit_->RequestHeaderParse(event_data_->GetRequest());
	if (event_data_->GetRequest().GetHeaders().find("Transfer-Encoding") != std::string::npos)
	{
		event_data_->GetRequest().RemoveRNRNOneTime();
		wit_->ChunkBodyParse(event_data_->GetRequest(), event_data_->GetResponse());
	}
}

void    Webserv::AddCgiEvent() {
	event_data_->GetCgiHandler().ExecuteChildProcess(event_data_->GetRequest(), event_data_->GetResponse());
	if (event_data_->GetResponse().GetStatusCode() == OK)
		SetCgiEvent();
	else {
		ChangeEvent(change_list_, curr_event_->ident, EVFILT_READ, EV_DISABLE, 0, 0, curr_event_->udata);
		ChangeEvent(change_list_, curr_event_->ident, EVFILT_WRITE, EV_ENABLE, 0, 0, curr_event_->udata);
	}
}

void    Webserv::CheckRequestError() {
	for (size_t i = 0; i < wit_->GetLocations().size(); i++)
	{
		if (event_data_->GetRequest().GetPath().find(wit_->GetLocations()[i].GetUri()) != std::string::npos) {
			if (wit_->GetLocations()[i].GetUri().length() != 1)
			{
				location_idx_ = i;
				break ;
			}
			location_idx_ = i;
		}
	}
	Location loc = wit_->GetLocations()[location_idx_];

	std::string path = event_data_->GetRequest().GetPath();
	for (size_t i = 0; i < path.length() - 1; i++) {
		if (path[i] == '/' && path[i + 1] == '/')
			return event_data_->GetResponse().SetStatusCode(NOT_FOUND);
	}

	std::string method = event_data_->GetRequest().GetMethod();
	std::map<int, bool> limit_excepts = loc.GetLimitExcepts();

	std::string allowed = "";
	if (limit_excepts[METHOD_GET])
		allowed += "GET, ";
	if (limit_excepts[METHOD_POST])
		allowed += "POST, ";
	if (limit_excepts[METHOD_DELETE])
		allowed += "DELETE, ";
	allowed.pop_back();
	allowed.pop_back();

	if (!(method == "GET" || method == "POST" || method == "DELETE"))
		return event_data_->GetResponse().SetStatusCode(NOT_IMPLEMENTED);
	else if (!((method == "GET" &&  limit_excepts[METHOD_GET])
           || (method == "POST" && limit_excepts[METHOD_POST])
           || (method == "DELETE" && limit_excepts[METHOD_DELETE]))) {
		event_data_->GetResponse().SetAllow(allowed);
		return event_data_->GetResponse().SetStatusCode(METHOD_NOT_ALLOWED);
	}

	if (event_data_->GetRequest().GetScheme() != "HTTP/1.1")
		return event_data_->GetResponse().SetStatusCode(HTTP_VERSION_NOT_SUPPORTED);

	if (event_data_->GetRequest().GetContentLength() > loc.GetClientMaxBodySizeLocation())
		return event_data_->GetResponse().SetStatusCode(PAYLOAD_TOO_LARGE);

	if (event_data_->GetRequest().GetMethod() == "POST"
		&& event_data_->GetRequest().GetContentLength() == 0
		&& event_data_->GetRequest().GetTransferEncoding() != "chunked")
		return event_data_->GetResponse().SetStatusCode(LENGTH_REQUIRED);

	if (event_data_->GetRequest().GetPath().length() > 2048)
		return event_data_->GetResponse().SetStatusCode(URI_TOO_LONG);
}

void    Webserv::CheckRedirection() {
	if (wit_->GetLocations()[location_idx_].GetRedirStatusCode() == MOVED_PERMANENTLY)
		return event_data_->GetResponse().SetStatusCode(MOVED_PERMANENTLY);
}

void	Webserv::SetCgiEvent() {
	event_data_->SetEventType(CGIEVENT);
	ChangeEvent(change_list_, this->event_data_->GetCgiHandler().GetReadFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, event_data_);
	ChangeEvent(change_list_, this->event_data_->GetCgiHandler().GetWriteFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, event_data_);

	fcntl(this->event_data_->GetCgiHandler().GetReadFd(), F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	fcntl(this->event_data_->GetCgiHandler().GetWriteFd(), F_SETFL, O_NONBLOCK, FD_CLOEXEC);
}

void    Webserv::MakeResponse() {
	std::string method = event_data_->GetRequest().GetMethod();
    if (method == "GET")
        event_data_->GetResponse().HandleGet(event_data_->GetRequest(), wit_->GetLocations()[location_idx_].GetIndex(), wit_->GetLocations()[location_idx_].GetAutoIndex());
    else if (method == "POST")
        event_data_->GetResponse().HandlePost(event_data_->GetRequest(), wit_->GetLocations()[location_idx_].GetIndex());
    else if (method == "DELETE")
        event_data_->GetResponse().HandleDelete(event_data_->GetRequest());
}