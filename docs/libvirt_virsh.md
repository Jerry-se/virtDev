### 描述
virsh程序是用于管理的virsh来宾域的主界面。该程序可用于创建、暂停和关闭域。它还可用于列出当前域。Libvirt 是一个 C 工具包，用于与最新版本的 Linux（和其他操作系统）的虚拟化功能进行交互。它是根据 GNU 宽松通用公共许可证提供的免费软件。Linux 操作系统的虚拟化意味着能够在单个硬件系统上同时运行多个操作系统实例，其中基本资源由一个 Linux 实例驱动。该库旨在提供长期稳定的 C API。目前支持 Xen、QEMU、KVM、LXC、OpenVZ、VirtualBox 和 VMware ESX。

### 概要
virsh [OPTION]... [COMMAND_STRING]

virsh [OPTION]... COMMAND DOMAIN [ARG]...

### 常用的命令
virsh list --all

virsh define test.xml

virsh undefine $DOMAIN

virsh start $DOMAIN

virsh destroy $DOMAIN

virsh domifaddr $DOMAIN

virsh domiflist $DOMAIN
