#include "../../inc/WorkerData.hpp"

WorkerData::WorkerData(Request &request, Response &response, CgiHandler &cgi, EventType event)
	: request(request), response(response), cgi(cgi) {
	this->event = event;
}

WorkerData::~WorkerData() {
	delete (&response);
	delete (&request);
	delete (&cgi);
}

Request& WorkerData::GetRequest() {
	return (this->request);
}

Response& WorkerData::GetResponse() {
	return (this->response);
}

CgiHandler& WorkerData::GetCgiHandler() {
	return (this->cgi);
}

EventType WorkerData::GetEventType() const {
	return (this->event);
}

void	WorkerData::SetEventType(EventType event) {
	this->event = event;
}