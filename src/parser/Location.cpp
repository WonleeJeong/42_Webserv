#include "../../inc/Location.hpp"

Location::Location() : uri_("default"), index_("index.html"), auto_index_(false), redir_status_code_(200), redir_uri("/"), client_max_body_size_location_(1000000) {
	this->limit_excepts_[METHOD_GET] = true;
	this->limit_excepts_[METHOD_POST] = true;
	this->limit_excepts_[METHOD_PUT] = true;
	this->limit_excepts_[METHOD_DELETE] = true;
}

Location::~Location() {}

void Location::SetUri(const std::string& uri) {
	this->uri_ = uri;
}

void Location::SetIndex(const std::string& index) {
	this->index_ = index;
}

void Location::SetRedirStatusCode(int status) {
	this->redir_status_code_ = status;
}

void Location::SetRedirUri(const std::string& uri) {
	this->redir_uri = uri;
}

void Location::SetLimitExcepts(int method, bool access) {
	this->limit_excepts_[method] = access;
}

void Location::SetClientMaxBodySizeLocation(int size) {
	this->client_max_body_size_location_ = size;
}

void Location::SetAutoIndex(bool access) {
	this->auto_index_ = access;
}

const std::string& Location::GetUri() const {
	return this->uri_;
}

const std::string& Location::GetIndex() const {
	return this->index_;
}

int Location::GetRedirStatusCode() const {
	return this->redir_status_code_;
}

const std::string& Location::GetRedirUri() const {
	return this->redir_uri;
}

std::map<int, bool> Location::GetLimitExcepts() const {
	return this->limit_excepts_;
}

bool Location::GetAutoIndex() const {
	return this->auto_index_;
}

int Location::GetClientMaxBodySizeLocation() const {
	return this->client_max_body_size_location_;
}