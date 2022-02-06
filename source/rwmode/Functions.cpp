#include "Functions.h"
#include <Windows.h>
#include <iostream>
std::string GetLastErrorString()
{
  DWORD errMsgID = ::GetLastError();
  if (errMsgID == 0)
  {
    return std::string();
  }
  LPSTR msgBuf = nullptr;
  size_t size =
    FormatMessageA
    (
      FORMAT_MESSAGE_ALLOCATE_BUFFER |
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, errMsgID,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPSTR)&msgBuf, 0, NULL
    );
  std::string errMsg(msgBuf, size);
  LocalFree(msgBuf);
  return errMsg;
}
void signal_ctrl_c_handler(int signum) { std::exit(signum); }