#pragma once
#ifndef SLM1_MISC
#define SLM1_MISC

#include <iostream>

#include "Types.hpp"

#define SLM1_ERROR(_message) std::cout << "[Error] " << std::endl

#define SLM1_INFO(_message) std::cout << "[Info] " << std::endl

#define SLM1_NAMESPACE_BEGIN namespace SLM1 {
#define SLM1_NAMESPACE_END }

#endif // ifndef SLM1_MISC