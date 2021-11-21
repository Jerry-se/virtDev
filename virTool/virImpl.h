#ifndef _VIRDEV_VIR_TOOL_H_
#define _VIRDEV_VIR_TOOL_H_

#include <memory>
#include <thread>

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

namespace virTool {

class virToolDelegate {
public:
  virtual void PrintErrorMessage(virError *err) = 0;
};

class virDomainImpl {
public:
  virDomainImpl() = delete;
  explicit virDomainImpl(virDomainPtr domain);
  ~virDomainImpl();

  int32_t startDomain();

  /**
   * @brief Suspends an active domain, the process is frozen without further access to CPU resources and I/O but the memory used by the domain
   * at the hypervisor level will stay allocated. Use virDomainResume() to reactivate the domain. This function may require privileged access.
   * Moreover, suspend may not be supported if domain is in some special state like VIR_DOMAIN_PMSUSPENDED.
   * @param domain   a domain object
   *
   * @return 0 in case of success and -1 in case of failure.
   *     -<em>-1</em> fail
   *     -<em>0</em> succeed
   */
  int32_t suspendDomain();

  /**
   * @brief Resume a suspended domain, the process is restarted from the state where it was frozen by calling virDomainSuspend().
   * This function may require privileged access Moreover, resume may not be supported if domain is in some special state like VIR_DOMAIN_PMSUSPENDED.
   * @param domain   a domain object
   *
   * @return 0 in case of success and -1 in case of failure.
   *     -<em>-1</em> fail
   *     -<em>0</em> succeed
   */
  int32_t resumeDomain();

  int32_t rebootDomain();

  int32_t shutdownDomain();

  int32_t destroyDomain();

  int32_t resetDomain();

  int32_t undefineDomain();

  // returns 0 in case of success and -1 in case of failure.
  int getDomainInfo(virDomainInfoPtr info);
  // returns 0 in case of success and -1 in case of failure.
  int getDomainState(int *state, int *reason, unsigned int flags);

  int getDomainInterfaceAddress();

  int setDomainUserPassword(const char *user, const char *password);

protected:
  std::shared_ptr<virDomain> domain_;
};

class virTool {
public:
  explicit virTool(bool enableEvent = false);
  ~virTool();

  // host
  /**
   * @brief Provides version information. libVer is the version of the library and will always be set unless an error occurs,
   * in which case an error code will be returned. typeVer exists for historical compatibility;
   * if it is not NULL it will duplicate libVer (it was originally intended to return hypervisor information based on type,
   * but due to the design of remote clients this is not reliable).
   * To get the version of the running hypervisor use the virConnectGetVersion() function instead.
   * To get the libvirt library version used by a connection use the virConnectGetLibVersion() instead.
   * This function includes a call to virInitialize() when necessary.
   * @param libVer   return value for the library version (OUT)
   * @param type     ignored; pass NULL
   * @param typeVer  pass NULL; for historical purposes duplicates libVer if non-NULL
   *
   * @return -1 in case of failure, 0 otherwise, and values for libVer and typeVer have the format major * 1,000,000 + minor * 1,000 + release.
   *     -<em>-1</em> fail
   *     -<em>0</em> succeed
   */
  static int getVersion(unsigned long *libVer, const char *type, unsigned long *typeVer);

  /**
   * @brief Get the version level of the Hypervisor running. This may work only with hypervisor call, i.e. with privileged access to the hypervisor,
   * not with a Read-Only connection.
   * @param conn     pointer to the hypervisor connection
   * @param hvVer    return value for the version of the running hypervisor (OUT)
   *
   * @return -1 in case of error, 0 otherwise. if the version can't be extracted by lack of capacities returns 0 and hvVer is 0,
   * otherwise hvVer value is major * 1,000,000 + minor * 1,000 + release
   *     -<em>-1</em> fail
   *     -<em>0</em> succeed
   */
  int getConnectVersion(unsigned long *hvVer);
  /**
   * @brief Provides libVer, which is the version of libvirt used by the daemon running on the conn host
   * @param conn     pointer to the hypervisor connection
   * @param libVer   returns the libvirt library version used on the connection (OUT)
   *
   * @return -1 in case of failure, 0 otherwise, and values for libVer have the format major * 1,000,000 + minor * 1,000 + release.
   *     -<em>-1</em> fail
   *     -<em>0</em> succeed
   */
  int getConnectLibVersion(unsigned long *libVer);

  bool openConnect(const char *name);
  bool openConnectReadOnly(const char *name);

  // domain
  // returns the number of domains found or -1 and sets domains to NULL in case of error.
  int listAllDomains(virDomainPtr **domains, unsigned int flags);
  // returns -1 in case of error
  int listDomains(int *ids, int maxids);
  int numOfDomains();

  std::shared_ptr<virDomainImpl> openDomainByID(int id);
  std::shared_ptr<virDomainImpl> openDomainByName(const char *domain_name);
  std::shared_ptr<virDomainImpl> openDomainByUUID(const unsigned char *uuid);
  std::shared_ptr<virDomainImpl> openDomainByUUIDString(const char *uuid);

  std::shared_ptr<virDomainImpl> createDomain(const char *xml_content);

protected:
  std::shared_ptr<virDomainImpl> defineDomain(const char *xml_content);
  void DefaultThreadFunc();

protected:
  std::shared_ptr<virConnect> conn_;
  bool enable_event_;
  int callback_id_;
  int thread_quit_;
  std::thread *thread_event_loop_;
};
} // namespace virTool

#endif //_VIRDEV_VIR_TOOL_H_
