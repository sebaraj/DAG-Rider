#include "error.hpp"

std::string NetworkErrorToString(NetworkError error, const std::string &address,
                                 uint16_t retry) {
  std::stringstream ss;
  switch (error) {
  case NetworkError::FailedToConnect:
    ss << "Failed to connect to " << address << " (retry " << retry << ")";
    break;
  case NetworkError::FailedToListen:
    ss << "Failed to accept connection: " << address;
    break;
  case NetworkError::FailedToSendMessage:
    ss << "Failed to send message to " << address;
    break;
  case NetworkError::FailedToReceiveMessage:
    ss << "Failed to receive message from " << address;
    break;
  case NetworkError::FailedToReceiveAck:
    ss << "Failed to receive ACK from " << address;
    break;
  case NetworkError::UnexpectedAck:
    ss << "Receive unexpected ACK from " << address;
    break;
  }
  return ss.str();
}

void LogNetworkError(NetworkError error, const std::string &address,
                     uint16_t retry) {
  std::cerr << NetworkErrorToString(error, address, retry) << std::endl;
}
