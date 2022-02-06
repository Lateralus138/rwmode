#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <string>
std::string GetLastErrorString();
void signal_ctrl_c_handler(int signum);
#endif // FUNCTIONS_H