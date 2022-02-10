#ifndef _VIRDEV_VIRT_DEV_H_
#define _VIRDEV_VIRT_DEV_H_

namespace virTool {
  void parseVersion();
  void parseList();
  // domain
  void parseCreateDomain(const char* xml_file);
  void parseStatsDomain(const char* domainName);
  void parseFSInfoDomain(const char* domainName);
  void parseBlklistDomain(const char* domainName);
  void parseSuspendDomain(const char* domainName);
  void parseResumeDomain(const char* domainName);
  void parseRebootDomain(const char* domainName);
  void parseIflistDomain(const char* domainName);
  // snapshot
  void parseDomainSnapshotList(const char* domainName);
}

#endif // _VIRDEV_VIRT_DEV_H_
