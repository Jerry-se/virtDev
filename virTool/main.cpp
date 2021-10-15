#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include "virTool.h"

namespace virtDev {
  const static char* virUri = "qemu+tcp://localhost/system";

  std::string stringPrintf(const char* format, ...) {
    char str[1024];
    va_list ap;
    va_start(ap, format);
    int n = vsnprintf(str, sizeof(str), format, ap);
    va_end(ap);
    return str;
  }

  std::string translateVirVer(unsigned long ver) {
    unsigned long major = ver / 1000000;
    ver = ver % 1000000;
    unsigned long minor = ver / 1000;
    unsigned long release = ver % 1000;
    return stringPrintf("%lu.%lu.%lu", major, minor, release);
  }

  void parseVersion() {
    virTool virTool_;
    virTool_.ConnectOpen(virUri);
    unsigned long libVer = 0;
    if (virTool::GetVersion(&libVer, NULL, NULL) == 0)
      std::cout << "virGetVersion: " << translateVirVer(libVer) << " " << libVer << std::endl;
    if (virTool_.ConnectGetVersion(&libVer) == 0)
      std::cout << "virConnectGetVersion: " << translateVirVer(libVer) << " " << libVer << std::endl;
    if (virTool_.ConnectGetLibVersion(&libVer) == 0)
      std::cout << "virConnectGetLibVersion: " << translateVirVer(libVer) << " " << libVer << std::endl;
  }

  void parseList() {
    virTool virTool_;
    virTool_.ConnectOpen(virUri);
    // 方法一，不推荐
    // int nums = virTool_.ConnectNumOfDomains();
    // if (nums < 1) {
    //   std::cout << "found none domain" << std::endl;
    //   return;
    // }
    // int *ids = new int[nums];
    // if (virTool_.ConnectListDomains(ids, nums) != -1) {
    //   for (int i = 0; i < nums; i++) {
    //     if (virTool_.DomainLookupByID(ids[i])) {
    //       virDomainInfo info;
    //       if (virTool_.DomainGetInfo(&info) == 0) {
    //         std::cout << stringPrintf("domain id: %d, state: %hhu, maxMem: %lu, memory: %lu, nrVirtCpu: %hu, cpuTime: %llu",
    //         ids[i], info.state, info.maxMem, info.memory, info.nrVirtCpu, info.cpuTime) << std::endl;
    //       }
    //       virTool_.DomainFree();
    //     }
    //   }
    // }
    // delete[] ids;
    // 方法二
    virDomainPtr *domains;
    unsigned int flags = 0b11111111111111;// VIR_CONNECT_LIST_DOMAINS_RUNNING | VIR_CONNECT_LIST_DOMAINS_PERSISTENT;
    int ret = virTool_.ConnectListAllDomains(&domains, flags);
    if (ret < 0)
      std::cout << "virConnectListAllDomains error" << std::endl;
    for (int i = 0; i < ret; i++) {
        // do_something_with_domain(domains[i]);
        virDomainInfo info;
        if (virDomainGetInfo(domains[i], &info) == 0) {
          std::cout << stringPrintf("domain id: %d, state: %hhu, maxMem: %lu, memory: %lu, nrVirtCpu: %hu, cpuTime: %llu",
          virDomainGetID(domains[i]), info.state, info.maxMem, info.memory, info.nrVirtCpu, info.cpuTime) << std::endl;
        }
        //here or in a separate loop if needed
        virDomainFree(domains[i]);
    }
    free(domains);
  }

  void parseSuspendDomain(const std::string& domain) {
    virTool virTool_;
    virTool_.ConnectOpen(virUri);
    if (virTool_.DomainLookupByName(domain.c_str())) {
      int ret = virTool_.DomainSuspend();
      std::cout << "suspend domain " << (ret == 0 ? "ok" : "failed") << std::endl;
    }
    else
      std::cout << "can not find domain: " << domain << std::endl;
  }

  void parseResumeDomain(const std::string& domain) {
    virTool virTool_;
    virTool_.ConnectOpen(virUri);
    if (virTool_.DomainLookupByName(domain.c_str())) {
      int ret = virTool_.DomainResume();
      std::cout << "resume domain " << (ret == 0 ? "ok" : "failed") << std::endl;
    }
    else
      std::cout << "can not find domain: " << domain << std::endl;
  }
}

void parseCommand(int argc, char *argv[]) {
  std::string op, domName;
  if (argc < 2) {
    std::cout << "format: virTool operation [domain name]" << std::endl;
    return;
  }
  op = argv[1];
  if (argc > 2) domName = argv[2];
  if (op == "ver" || op == "version")
    virtDev::parseVersion();
  else if (op == "list")
    virtDev::parseList();
  else if (op == "suspend") {
    if (domName.empty()) {
      std::cout << "format: virTool operation [domain name]" << std::endl;
      return;
    }
    virtDev::parseSuspendDomain(domName);
  }
  else if (op == "resume") {
    if (domName.empty()) {
      std::cout << "format: virTool operation [domain name]" << std::endl;
      return;
    }
    virtDev::parseResumeDomain(domName);
  }
  else
    std::cout << "unknown operation" << std::endl;
}

int main(int argc, char *argv[]) {
  parseCommand(argc, argv);
  return 0;
}
