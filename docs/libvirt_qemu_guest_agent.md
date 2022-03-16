# 简介
QEMU Guest Agent是运行在虚拟机内部的一个守护程序（qemu-guest-agent.service），用它来辅助Hypervisor实现对Guest的管理。QEMU为宿主机和虚拟机提供了一个数据通道（channel），这个通道的两端分别是在虚拟机内看到的串口和在宿主机上看到的unix socket文件。宿主机与虚拟机内的qga通讯就扩展了对虚拟机的控制能力，例如在宿主机上获取虚拟机的ip地址等。

# 设置QEMU GA
目前，QEMU 通过 virtio 串口公开 GA。有一些尝试允许应用程序使用 qemu 监视器与 GA 进行通信，但仍应支持 virtio 串行端口。不过，这些都是内部结构。

为了能够使用 GA 用户需要创建具有特殊名称org.qemu.guest_agent.0 的virtio 串口。换句话说，需要将其添加到&lt;devices&gt;下的域 XML 中：
```
<channel type='unix'>
   <source mode='bind' path='/var/lib/libvirt/qemu/f16x86_64.agent'/>
   <target type='virtio' name='org.qemu.guest_agent.0 '/>
</channel>
```
如果使用 libvirt 1.0.6 或更新版本，您可以省略 &lt;source> 元素的 path='...' 属性，libvirt 将自动管理事物。

在虚拟机内部:
```
# yum install qemu-guest-agent
# setenforce 0
# systemctl restart qemu-guest-agent.service
```

# 用法
Libvirt 不保证对直接使用来宾代理的任何支持。如果您不介意使用 libvirt-qemu.so，则可以使用virDomainQemuAgentCommandAPI（由virsh qemu-agent-command公开）；但请注意，这是不受支持的，您对代理所做的任何更改都可能导致 libvirt 行为异常。同时，guest 代理可以由多个受支持的 libvirt API 在内部使用，通常通过传递标志来请求使用。例如，virDomainShutdownFlags支持：
```
VIR_DOMAIN_SHUTDOWN_DEFAULT = 0,
VIR_DOMAIN_SHUTDOWN_ACPI_POWER_BTN = (1 << 0),
VIR_DOMAIN_SHUTDOWN_GUEST_AGENT = (1 << 1),
```
类似适用于重启 API：
```
VIR_DOMAIN_REBOOT_DEFAULT = 0,
VIR_DOMAIN_REBOOT_ACPI_POWER_BTN = (1 << 0),
VIR_DOMAIN_REBOOT_GUEST_AGENT = (1 << 1),
```
在 virsh 中，用户可以通过--mode acpi|agent选择关机/重启方法：
```
virsh reboot --mode agent $DOMAIN
virsh shutdown --mode agent $DOMAIN
```
如果用户正在创建快照，他/她可能希望先冻结文件系统，然后再解冻文件系统。这可以通过将 VIR_DOMAIN_SNAPSHOT_CREATE_QUIESCE 标志指定给 virDomainSnapshotCreate API 或：
```
virsh snapshot-create --quiesce $DOMAIN
virsh snapshot-create-as --quiesce $DOMAIN
```
获取虚拟机的ip地址信息
```
virsh # qemu-agent-command $DOMAIN '{"execute":"guest-info"}'
{"return":{"version":"2.11.1","supported_commands":[{"enabled":true,"name":"guest-sync-delimited","success-response":true},{"enabled":true,"name":"guest-sync","success-response":true},{"enabled":true,"name":"guest-suspend-ram","success-response":false},{"enabled":true,"name":"guest-suspend-hybrid","success-response":false},{"enabled":true,"name":"guest-suspend-disk","success-response":false},{"enabled":true,"name":"guest-shutdown","success-response":false},{"enabled":true,"name":"guest-set-vcpus","success-response":true},{"enabled":true,"name":"guest-set-user-password","success-response":true},{"enabled":true,"name":"guest-set-time","success-response":true},{"enabled":true,"name":"guest-set-memory-blocks","success-response":true},{"enabled":true,"name":"guest-ping","success-response":true},{"enabled":true,"name":"guest-network-get-interfaces","success-response":true},{"enabled":true,"name":"guest-info","success-response":true},{"enabled":true,"name":"guest-get-vcpus","success-response":true},{"enabled":true,"name":"guest-get-users","success-response":true},{"enabled":true,"name":"guest-get-timezone","success-response":true},{"enabled":true,"name":"guest-get-time","success-response":true},{"enabled":true,"name":"guest-get-osinfo","success-response":true},{"enabled":true,"name":"guest-get-memory-blocks","success-response":true},{"enabled":true,"name":"guest-get-memory-block-info","success-response":true},{"enabled":true,"name":"guest-get-host-name","success-response":true},{"enabled":true,"name":"guest-get-fsinfo","success-response":true},{"enabled":true,"name":"guest-fstrim","success-response":true},{"enabled":true,"name":"guest-fsfreeze-thaw","success-response":true},{"enabled":true,"name":"guest-fsfreeze-status","success-response":true},{"enabled":true,"name":"guest-fsfreeze-freeze-list","success-response":true},{"enabled":true,"name":"guest-fsfreeze-freeze","success-response":true},{"enabled":true,"name":"guest-file-write","success-response":true},{"enabled":true,"name":"guest-file-seek","success-response":true},{"enabled":true,"name":"guest-file-read","success-response":true},{"enabled":true,"name":"guest-file-open","success-response":true},{"enabled":true,"name":"guest-file-flush","success-response":true},{"enabled":true,"name":"guest-file-close","success-response":true},{"enabled":true,"name":"guest-exec-status","success-response":true},{"enabled":true,"name":"guest-exec","success-response":true}]}}
virsh # qemu-agent-command $DOMAIN '{"execute":"guest-network-get-interfaces"}'
{"return":[{"name":"lo","ip-addresses":[{"ip-address-type":"ipv4","ip-address":"127.0.0.1","prefix":8},{"ip-address-type":"ipv6","ip-address":"::1","prefix":128}],"statistics":{"tx-packets":120,"tx-errs":0,"rx-bytes":9880,"rx-dropped":0,"rx-packets":120,"rx-errs":0,"tx-bytes":9880,"tx-dropped":0},"hardware-address":"00:00:00:00:00:00"},{"name":"ens3","ip-addresses":[{"ip-address-type":"ipv4","ip-address":"192.168.122.103","prefix":24},{"ip-address-type":"ipv6","ip-address":"fe80::5054:ff:fe6b:7a65","prefix":64}],"statistics":{"tx-packets":2491,"tx-errs":0,"rx-bytes":11242679,"rx-dropped":43873,"rx-packets":50738,"rx-errs":0,"tx-bytes":250113,"tx-dropped":0},"hardware-address":"52:54:00:6b:7a:65"}]}
```

警告：像任何监视器命令一样，GA 命令可以无限期地阻塞~~~特别是当 GA 未运行时。目前，没有可靠的方法/API 来判断 GA 是否正在运行~~~. Libvirt 执行一些基本检查来确定是否有 qemu-ga 实例正在运行。基本上，guest-sync在每个有用的命令之前发出一个命令。如果它返回，则 libvirt 认为来宾代理已启动并正在运行并发出真正的命令。但是，访客代理可能会同时死亡或被终止。该guest-sync命令发出超时，如果命中来宾代理被视为不存在。

# 在没有 libvirt 的情况下配置访客代理
在某些情况下，用户可能希望在他们的域 XML 中配置一个访客代理，但根本不希望 libvirt 连接到访客代理套接字。因为 libvirt 连接到访客代理通道当且仅当它是具有org.qemu.guest_agent.0名称的 virtio 通道时，所有需要做的就是使这些条件中的至少一个无效。但是，最可行的方法是更改​​目标名称：
```
<channel type='unix'>
   <source mode='bind' path='/var/lib/libvirt/qemu/f16x86_64.agent'/>
   <target type='virtio' name='org.qemu.guest_agent.1 '/>
</channel>
```
如果您确实更改了名称，则还需要将其告诉 qemu GA：
```
 # qemu-ga -p /dev/virtio-ports/$name
```
在宿主机上自己去连接socket文件：
```
[root@node2 ~]# socat unix-connect:/var/lib/libvirt/qemu/org.qemu.guest_agent.1 readline
{"execute": "guest-info"}
```

# 参考资料
* [Qemu guest agent wiki](https://wiki.libvirt.org/page/Qemu_guest_agent)
* [qemu-guest-agent原理及实践](https://blog.csdn.net/isclouder/article/details/80200859)
* [QEMU 访客代理](https://documentation.suse.com/sles/15-SP2/html/SLES-all/cha-qemu-ga.html)
