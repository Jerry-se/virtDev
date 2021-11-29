### 简介
* libvirt是用于管理虚拟化平台的开源的API，后台程序和管理工具。
* 包括一个API库，一个守护进程（Libvirtd），和一个命令行实用程序（virsh）。
* 支持语言C,C++,Python,Perl,Ruby, java,JavaScript,php等语言。
* 支持系统Linux，FreeBSD，Windows和OS-X。

### 官方网站
* 官网：https://libvirt.org/
* Gitlab仓库：https://gitlab.com/libvirt/libvirt
* 官方文档：https://libvirt.org/docs.html
* libvirt wiki: https://wiki.libvirt.org/page/Main_Page
* 虚拟机的生命周期：https://wiki.libvirt.org/page/VM_lifecycle
* 域的XML格式: https://libvirt.org/formatdomain.html
* libvirt虚拟网络: https://wiki.libvirt.org/page/VirtualNetworking
* Ubuntu虚拟机演示：https://wiki.libvirt.org/page/UbuntuKVMWalkthrough
* Ubuntu对libvirt的介绍：https://ubuntu.com/server/docs/virtualization-libvirt

### Libvirt API
5 个 API 部分：虚拟机监控程序连接 API、域 API、网络 API、存储卷 API 以及存储池 API。
* 连接 API ：通过 virConnectOpen 调用（以及其他进行认证的调用）提供。这些函数的返回值是一个 virConnectPtr 对象。该对象作为所有其他管理功能的基础。virConnectListDomains查看虚拟机资源，以 XML 文档的形式返回。
* 域API：用于列举和管理已有的虚机，或者创建新的虚机。一个域可能是暂时性的或者持久性的。
* 网络API：用于管理虚机的网络设备。virNetworkPtr 对象来管理并检查虚拟网络。一个虚拟网络可能是暂时性的或者持久性的。
* 存储卷API：用于管理虚拟机内的所有存储。（img文件）
* 存储池API：用于管理一个存储池内的存储块，包括一个池内分配的块、磁盘分区、逻辑卷、SCSI/iSCSI Lun，或者一个本地或者网络文件系统内的文件等。（存储池：是放置虚拟机的存储位置，可以是本地的，也可以是网络存储，具体的虚拟机实例放置在卷（Volume）上。 ）

### 守护进程libvirtd
检测执行远程指令，根据不同虚拟机技术（qemu, xen, storage…）注册驱动。

### 参考资料
* [libvirt 虚拟化库剖析](https://developer.ibm.com/tutorials/l-libvirt/#basic_architecture)
* [KVM、QEMU与libvirt区别](http://blog.itpub.net/27785870/viewspace-2215391/)
* [libvirt详解](https://blog.csdn.net/weixin_42752248/article/details/107299491)
* [libvirt博客](https://www.cnblogs.com/weikunzz/p/6710501.html)
* [python 实现libvirt管理（简单的几个功能）](https://blog.csdn.net/zhongbeida_xue/article/details/78654345)
