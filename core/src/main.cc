#include <iostream>

//  TODO(inkeun): 이 내용은 불필요하다.
//                추후 so 파일과 dll 빌드를 지원하도록 수정한다면 제거한다.

#include "core/include/libgs/common/logging.h"


int main() {
  libgs::InitializeDefaultLoggers();
  {
    auto logger = libgs::Logger::Find("console");
    if (!logger) {
      std::cout << "logger is null" << std::endl;
    }

    logger->Info("hi info.");
    logger->Error("hi error.");
    logger->Critical("hi critical.");
    logger->Warn("hi warn.");
  }

  {
    auto logger = libgs::Logger::Find("daily");
    if (!logger) {
      std::cout << "logger is null" << std::endl;
    }
    logger->Info("hi info.");
    logger->Error("hi error.");
    logger->Critical("hi critical.");
    logger->Warn("hi warn.");
  }

  {
    auto logger = libgs::Logger::Find("rotate");
    logger->Info("hi info.");
    logger->Error("hi error.");
    logger->Critical("hi critical.");
    logger->Warn("hi warn.");
  }

  return 0;
}