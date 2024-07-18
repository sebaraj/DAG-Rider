#ifndef ERROR_HPP
#define ERROR_HPP

#include <boost/system/error_code.hpp>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string>

enum class NetworkError {
  FailedToConnect,
  FailedToListen,
  FailedToSendMessage,
  FailedToReceiveMessage,
  FailedToReceiveAck,
  UnexpectedAck,
};

std::string NetworkErrorToString(NetworkError error, const std::string &address,
                                 uint16_t retry = 0);
void LogNetworkError(NetworkError error, const std::string &address,
                     uint16_t retry = 0);

#endif // ERROR_HPP
