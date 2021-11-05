#include <iostream>
#include <string>
#include <thread>

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

const static char* virUri = "qemu+tcp://localhost/system";
char* domain_name = NULL;
virConnectPtr connect = nullptr;
// enum virDomainState
const char* arrayState[] = {"no state", "running", "blocked", "paused", "shutdown", "shutoff", "crashed", "pmsuspended", "last"};
// enum virDomainEventType
const char* arrayEventType[] = {"VIR_DOMAIN_EVENT_DEFINED", "VIR_DOMAIN_EVENT_UNDEFINED", "VIR_DOMAIN_EVENT_STARTED",
  "VIR_DOMAIN_EVENT_SUSPENDED", "VIR_DOMAIN_EVENT_RESUMED", "VIR_DOMAIN_EVENT_STOPPED",
  "VIR_DOMAIN_EVENT_SHUTDOWN", "VIR_DOMAIN_EVENT_PMSUSPENDED", "VIR_DOMAIN_EVENT_CRASHED"};
int t1_running = 1;
int t2_running = 1;
int t3_running = 1;

void thread_func1() {
  printf("thread t1 begin\n");
  int i = 0, error_domain = 0;
  virDomainPtr domain_ = nullptr;
  while(i < 3 && error_domain < 3) {
    if (!connect) break;
    domain_ = virDomainLookupByName(connect, domain_name);
    if (!domain_) {
      ++error_domain;
      printf("thread t1 not find domain\n");
      continue;
    }
    int domain_state = 0;
    if (virDomainGetState(domain_, &domain_state, NULL, 0) == 0) {
      if (domain_state == virDomainState::VIR_DOMAIN_RUNNING) {
        int ret = virDomainSuspend(domain_);
        printf("thread t1 suspend domain %s\n", ret == 0 ? "success" : "failed");
        ++i;
      }
    }
    virDomainFree(domain_);
  }
  t1_running = 0;
  printf("thread t1 end\n");
}

void thread_func2() {
  printf("thread t2 begin\n");
  int i = 0, error_domain = 0;
  virDomainPtr domain_ = nullptr;
  while(i < 3 && error_domain < 3) {
    if (!connect) break;
    domain_ = virDomainLookupByName(connect, domain_name);
    if (!domain_) {
      ++error_domain;
      printf("thread t2 not find domain\n");
      continue;
    }
    int domain_state = 0;
    if (virDomainGetState(domain_, &domain_state, NULL, 0) == 0) {
      if (domain_state == virDomainState::VIR_DOMAIN_PAUSED) {
        int ret = virDomainResume(domain_);
        printf("thread t2 resume domain %s\n", ret == 0 ? "success" : "failed");
        ++i;
      }
    }
    virDomainFree(domain_);
  }
  t2_running = 0;
  printf("thread t2 end\n");
}

void thread_func3() {
  printf("thread t3 begin\n");
  int error_domain = 0;
  virDomainPtr domain_ = nullptr;
  while((t1_running == 1 || t2_running == 1) && error_domain < 3) {
    if (!connect) break;
    domain_ = virDomainLookupByName(connect, domain_name);
    if (!domain_) {
      ++error_domain;
      printf("thread t3 not find domain\n");
      continue;
    }
    int domain_state = 0;
    if (virDomainGetState(domain_, &domain_state, NULL, 0) == 0) {
      if (domain_state >= 0 && domain_state <= 7) {
        printf("thread t3 get domain state %s \n", arrayState[domain_state]);
      }
    }
    virDomainFree(domain_);
  }
  t3_running = 0;
  printf("thread t3 end\n");
}

int domain_event_cb(virConnectPtr conn, virDomainPtr dom, int event, int detail, void *opaque) {
  printf("event lifecycle cb called, event=%d, detail=%d\n", event, detail);
  if (event >= 0 && event <= virDomainEventType::VIR_DOMAIN_EVENT_CRASHED) {
    const char* name = virDomainGetName(dom);
    int domain_state = 0;
    if (virDomainGetState(dom, &domain_state, NULL, 0) < 0) {
      domain_state = 0;
    }
    printf("domain %s %s, state %s\n", name, arrayEventType[event], arrayState[domain_state]);
  }
  else {
    printf("unknowned event\n");
  }
}

void event_lifecycle_cb(virDomainPtr conn, virDomainPtr dom, void *opaque) {
  printf("event lifecycle cb called\n");
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("usage: virTest domain_name\n");
    return -1;
  }

  int ret = virEventRegisterDefaultImpl();
  if (ret < 0) {
    printf("virEventRegisterDefaultImpl failed\n");
    return -1;
  }

  domain_name = argv[1];
  connect = virConnectOpen(virUri);
  if (!connect) {
    printf("connect failed\n");
    return -1;
  }

  int callback_id = virConnectDomainEventRegisterAny(connect, NULL,
    virDomainEventID::VIR_DOMAIN_EVENT_ID_LIFECYCLE, VIR_DOMAIN_EVENT_CALLBACK(domain_event_cb), NULL, NULL);

  std::thread t1(thread_func1);
  std::thread t2(thread_func2);
  std::thread t3(thread_func3);

  while (t1_running == 1 || t2_running == 1 || t3_running == 1) {
    if (virEventRunDefaultImpl() < 0) {
      printf("virEventRunDefaultImpl failed\n");
    }
  }
  t1.join();
  t2.join();
  t3.join();

  virConnectDomainEventDeregisterAny(connect, callback_id);
  virConnectClose(connect);
  return 0;
}
