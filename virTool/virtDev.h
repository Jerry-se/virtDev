#ifndef _VIRDEV_VIRT_DEV_H_
#define _VIRDEV_VIRT_DEV_H_

namespace virTool {
  void parseVersion();
  void parseList();
  void parseCreateDomain(const char* xml_file);
  void parseSuspendDomain(const char* domainName);
  void parseResumeDomain(const char* domainName);
}

#endif // _VIRDEV_VIRT_DEV_H_