#include "../../inc/BlockParser.hpp"
#include "../../inc/Worker.hpp"

const char *ConfigParser::ConfFileNameError::what() const throw() {
	return "Configfilename is wrong";
}

const char *ConfigParser::ConfFileOpenError::what() const throw() {
	return "Configfile is not open";
}

const char *ConfigParser::ConfFileError::what() const throw() {
	return "Configfile error";
}

const char *Worker::BindError::what() const throw() {
	return "bind error";
}

const char *Worker::ListenError::what() const throw() {
	return "listen error";
}

const char *Worker::KqueueError::what() const throw() {
	return "kqueue error";
}

const char *Worker::KeventError::what() const throw() {
	return "kevent error";
}

const char *Worker::AcceptError::what() const throw() {
	return "accept error";
}

const char *Worker::ReadError::what() const throw() {
	return "read error";
}

const char *Worker::UnkownError::what() const throw() {
	return "unkown errorr";
}
