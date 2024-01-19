#ifndef WORKERDATA_HPP
#define WORKERDATA_HPP

#include "Worker.hpp"

enum EventType {
	CLIENTEVENT,
	CGIEVENT
};

class WorkerData {
	private:
		Request	&request;
		Response &response;
		CgiHandler &cgi;
		EventType event;

	public:
		WorkerData(Request &request, Response &response, CgiHandler &cgi, EventType event);
		~WorkerData();

		Request& GetRequest(void);
		Response& GetResponse(void);
		CgiHandler& GetCgiHandler(void);
		EventType GetEventType(void) const;

		void	SetEventType(EventType event);
};

#endif
