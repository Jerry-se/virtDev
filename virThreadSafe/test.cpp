#include <iostream>
#include <string>
#include <thread>

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

const static char* virUri = "qemu+tcp://localhost/system";
char* domain_name = NULL;
virConnectPtr connect = nullptr;
const char* arrayState[] = {"no state", "running", "blocked", "paused", "shutdown", "shutoff", "crashed", "pmsuspended", "last"};
int t1_running = 1;
int t2_running = 1;

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
  printf("thread t3 end\n");
}

int main(int argc, char *argv[]) {
  if (argc != 2)
    return -1;
  domain_name = argv[1];
  connect = virConnectOpen(virUri);
  if (!connect)
    return -1;

  std::thread t1(thread_func1);
  std::thread t2(thread_func2);
  std::thread t3(thread_func3);
  t1.join();
  t2.join();
  t3.join();

  virConnectClose(connect);
  return 0;
}
