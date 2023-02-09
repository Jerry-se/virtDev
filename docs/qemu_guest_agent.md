# qga简介

## qga工具
qemu guest agent，简称qga，是一个运行在虚拟机内部的普通应用程序，可实现宿主机libvirt和虚拟机的通信，宿主机可通过向虚机中的qga发送命令控制虚机。

这种通信不依赖网络，而是通过virtio-serial的方式。使用virtio传递消息，对虚拟机和主机的网络设置没有任何要求，且效率更高，类似于VMware Tools。

## 宿主机libvirt和VM通信方式选择
宿主机若要和VM通信，需要满足以下两个条件：
1. vm内部和外部宿主机之间，打开一个通道，进行数据交互
2. vm内部需要安装agent，用于接收外部宿主机具体的指令，并反馈结果

其中，打开通道有2类方法：
1. 走网络：需预插入一张管理网卡，增加网络拓扑复杂性
2. 走设备：只需在VM内部和宿主之间建立一个设备通道，实现简单，为当前使用方式

QEMU通过串口设备的模拟，为宿主机和虚拟机提供了一个数据通道（channel），这个通道的两端分别是在虚拟机内看到的串口和在宿主机上看到的unix socket文件。

串口设备的速率较低，适合小数据量的交换。

## 基本实现原理
1. 创建虚机时，需要在xml中配置channel段，写入相关参数，启动虚机时，会在宿主机上生成对应unix socket文件，同时，会在vm里生成一个字符设备，生成的unix socket和字符设备可以理解为一个channel隧道的两端
2. 虚拟机镜像制作时，安装好qemu-ga守护进程并配置开启自启动，qemu-ga进程会监听字符设备。
3. 宿主机上，将虚机中qga支持的rpc指令，通过channel发送，虚机中的qemu-guest-agent从串口设备收到数据后，执行相关指令。可实现文件读写、密码修改等

# 在虚拟机安装qga
## ubuntu安装qemu-ga,配置开机启动
```
sudo apt install qemu-guest-agent
systemctl enable qemu-guest-agent
```
## Windows虚机安装qemu-ga
windows系统需要下载virtio-win，以及qemu-ga安装包
https://fedorapeople.org/groups/virt/virtio-win/direct-downloads/archive-virtio/

https://fedorapeople.org/groups/virt/virtio-win/direct-downloads/archive-qemu-ga/

# xml配置virtio对应设备，创建虚机
libvirt在虚拟机创建时启动virtio通道，在xml虚机配置中加入virtio通道设备，如下：
```
<channel type='unix'>
  <source mode='bind' path='/tmp/channel_541effc0-ca3c-47c4-93cb-b789a6b3e29a.sock'/>
  <target type='virtio' name='org.qemu.guest_agent.0'/>
  <address type='virtio-serial' controller='0' bus='0' port='1'/>
</channel>
```
其中，path为virtio通道在主机本地的映射节点文件，宿主机可以基于此socket文件，通过unix sock实现与虚拟机的通信

基于以上xml创建的虚拟机中，会发现新的串口设备，设备名可能是/dev/vport0p0 或/dev/vport0p1
```
dbc@ubuntu-1804:~$ ll /dev/virtio-ports/org.qemu.guest_agent.0
lrwxrwxrwx 1 root root 11 Mar  2 02:24 /dev/virtio-ports/org.qemu.guest_agent.0 -> ../vport1p1
dbc@ubuntu-1804:~$ ll /dev/vport1p1
crw------- 1 root root 245, 1 Mar  2 02:24 /dev/vport1p1
```

# 通过socat方式指定socket文件与虚机通信
虚机内部默认串口name修改为org.qemu.guest_agent.1之后，在宿主机上连接虚机，可使用socat方式（默认org.qemu.guest_agent.0 可以使用 virsh qemu-agent-command方式通信）
1. 下载socat，安装到宿主机
http://rpmfind.net/linux/centos/7.8.2003/os/x86_64/Packages/socat-1.7.3.2-2.el7.x86_64.rpm
2. 宿主机上执行socat命令连接指定虚机socket文件
3. 连接成功后，执行对应的参数，会有返回值
```
[root@compute1 ~]# socat unix-connect:/var/lib/libvirt/qemu/org.qemu.guest_agent.1.instance-00000275.sock readline
# 查看当前所有支持的命令
{"execute": "guest-info"} <------ 直接输入，返回以下return内容，此命令可以查看所有支持的命令
{"return": {"version": "1.5.3", "supported_commands": [{"enabled": true, "name": "guest-default-dir-file-open"}, {"enabled": true, "name": "guest-file-upload"}, {"enabled": true, "name": "guest-command-py"}, {"enabled": true, "name": "guest-file-delete"}, {"enabled": true, "name": "guest-command"}, {"enabled": true, "name": "guest-net-usage"}, {"enabled": true, "name": "guest-disk-io"}, {"enabled": true, "name": "guest-disk-usage"}, {"enabled": true, "name": "guest-memory-usage"}, {"enabled": true, "name": "guest-cpu-usage"}, {"enabled": true, "name": "guest-get-statvfs"}, {"enabled": true, "name": "guest-set-vcpus"}, {"enabled": true, "name": "guest-get-vcpus"}, {"enabled": true, "name": "guest-network-get-interfaces"}, {"enabled": true, "name": "guest-suspend-hybrid"}, {"enabled": true, "name": "guest-suspend-ram"}, {"enabled": true, "name": "guest-suspend-disk"}, {"enabled": true, "name": "guest-fstrim"}, {"enabled": true, "name": "guest-fsfreeze-thaw"}, {"enabled": true, "name": "guest-fsfreeze-freeze"}, {"enabled": true, "name": "guest-fsfreeze-status"}, {"enabled": true, "name": "guest-file-flush"}, {"enabled": true, "name": "guest-file-seek"}, {"enabled": true, "name": "guest-file-write"}, {"enabled": true, "name": "guest-file-read"}, {"enabled": true, "name": "guest-file-close"}, {"enabled": true, "name": "guest-file-open"}, {"enabled": true, "name": "guest-shutdown"}, {"enabled": true, "name": "guest-info"}, {"enabled": true, "name": "guest-set-time"}, {"enabled": true, "name": "guest-get-time"}, {"enabled": true, "name": "guest-ping"}, {"enabled": true, "name": "guest-sync"}, {"enabled": true, "name": "guest-sync-delimited"}]}}
```
4. 通过socket文件连接虚机读取文件的方法
- 1 创建虚机，并使用账号密码登录虚机内部，创建自定义文件/home/testfile，内容"abcd"
- 2 在宿主机上，使用socat指定socket文件，连接虚机读取指定路径文件的内容
```
[root@compute2 ~]# socat unix-connect:/var/lib/libvirt/qemu/org.qemu.guest_agent.1.instance-00000299.sock readline
{"execute":"guest-file-open","arguments":{"path":"/home/testfile"}}
{"return": 1134}
{"execute":"guest-file-read","arguments":{"handle": 1134}}
{"return": {"count": 5, "buf-b64": "YWJjZAo=", "eof": true}}
# 对获取的文件内容进行base64解码，可获取到文件中的内容
[root@controller1 ~]# echo -n "YWJjZAo=" |base64 -d
abcd
```

# qga已有功能
目前qga最新版本为1.5.50，linux已经实现下面的所有功能，windows仅支持加*的那些功能：
- guest-sync-delimited*：宿主机发送一个int数字给qga，qga返回这个数字，并且在后续返回字符串响应中加入ascii码为0xff的字符，其作用是检查宿主机与qga通信的同步状态，主要用在宿主机上多客户端与qga通信的情况下客户端间切换过程的状态同步检查，比如有两个客户端A、B，qga发送给A的响应，由于A已经退出，目前B连接到qga的socket，所以这个响应可能被B收到，如果B连接到socket之后，立即发送该请求给qga，响应中加入了这个同步码就能区分是A的响应还是B的响应；在qga返回宿主机客户端发送的int数字之前，qga返回的所有响应都要忽略；
- guest-sync*：与上面相同，只是不在响应中加入0xff字符；
- guest-ping*：Ping the guest agent, a non-error return implies success；
- guest-get-time*：获取虚拟机时间（返回值为相对于1970-01-01 in UTC，Time in nanoseconds.）；
- guest-set-time*：设置虚拟机时间（输入为相对于1970-01-01 in UTC，Time in nanoseconds.）；
- guest-info*：返回qga支持的所有命令；
- guest-shutdown*：关闭虚拟机（支持halt、powerdown、reboot，默认动作为powerdown）；
- guest-file-open：打开虚拟机内的某个文件（返回文件句柄）；
- guest-file-close：关闭打开的虚拟机内的文件；
- guest-file-read：根据文件句柄读取虚拟机内的文件内容（返回base64格式的文件内容）；
- guest-file-write：根据文件句柄写入文件内容到虚拟机内的文件；
- guest-file-seek：Seek to a position in the file, as with fseek(), and return the current file position afterward. Also encapsulates ftell()'s functionality, just Set offset=0, whence=SEEK_CUR；
- guest-file-flush：Write file changes bufferred in userspace to disk/kernel buffers；
- guest-fsfreeze-status：Get guest fsfreeze state. error state indicates；
- guest-fsfreeze-freeze：Sync and freeze all freezable, local guest filesystems；
- guest-fsfreeze-thaw：Unfreeze all frozen guest filesystems；
- guest-fstrim：Discard (or "trim") blocks which are not in use by the filesystem；
- guest-suspend-disk*：Suspend guest to disk；
- guest-suspend-ram*：Suspend guest to ram；
- guest-suspend-hybrid：Save guest state to disk and suspend to ram（This command requires the pm-utils package to be installed in the guest.）；
- guest-network-get-interfaces：Get list of guest IP addresses, MAC addresses and netmasks；
- guest-get-vcpus：Retrieve the list of the guest's logical processors；
- guest-set-vcpus：Attempt to reconfigure (currently: enable/disable) logical processors inside the guest。

# qga 功能扩展方式
qga功能扩展十分方便，只需要在qapi-schema.json文件中定义好功能名称、输入输出数据类型，然后在commands-posix.c里面增加对应的功能函数即可，下面的补丁即在qga中增加一个通过statvfs获取虚拟机磁盘空间信息的功能：
```
diff --git a/qga/commands-posix.c b/qga/commands-posix.c
index e199738..2f42a2f 100644
--- a/qga/commands-posix.c
+++ b/qga/commands-posix.c
@@ -21,6 +21,7 @@
 #include <stdio.h>
 #include <string.h>
 #include <sys/stat.h>
+#include <sys/statvfs.h>
 #include <inttypes.h>
 #include "qga/guest-agent-core.h"
 #include "qga-qmp-commands.h"
@@ -1467,6 +1468,36 @@ void qmp_guest_fstrim(bool has_minimum, int64_t minimum, Error **err)
 }
 #endif

+GuestFileSystemStatistics *qmp_guest_get_statvfs(const char *path, Error **errp)
+{
+    int ret;
+    GuestFileSystemStatistics *fs_stat;
+    struct statvfs *buf;
+    buf = g_malloc0(sizeof(struct statvfs));
+
+
+    ret = statvfs(path, buf);
+    if (ret < 0) {
+       error_setg_errno(errp, errno, "Failed to get statvfs");
+       return NULL;
+    }
+
+    fs_stat = g_malloc0(sizeof(GuestFileSystemStatistics));
+    fs_stat->f_bsize = buf->f_bsize;
+    fs_stat->f_frsize = buf->f_frsize;
+    fs_stat->f_blocks = buf->f_blocks;
+    fs_stat->f_bfree = buf->f_bfree;
+    fs_stat->f_bavail = buf->f_bavail;
+    fs_stat->f_files = buf->f_files;
+    fs_stat->f_ffree = buf->f_ffree;
+    fs_stat->f_favail = buf->f_favail;
+    fs_stat->f_fsid = buf->f_fsid;
+    fs_stat->f_flag = buf->f_flag;
+    fs_stat->f_namemax = buf->f_namemax;
+
+    return fs_stat;
+}
+
 /* register init/cleanup routines for stateful command groups */
 void ga_command_state_init(GAState *s, GACommandState *cs)
 {
diff --git a/qga/qapi-schema.json b/qga/qapi-schema.json
index 7155b7a..a071c3f 100644
--- a/qga/qapi-schema.json
+++ b/qga/qapi-schema.json
@@ -638,3 +638,52 @@
 { 'command': 'guest-set-vcpus',
   'data':    {'vcpus': ['GuestLogicalProcessor'] },
   'returns': 'int' }
+
+##
+# @GuestFileSystemStatistics:
+#
+# Information about guest file system statistics.
+#
+# @f_bsize: file system block size.
+#
+# @f_frsize: fragment size.
+#
+# @f_blocks: size of fs in f_frsize units.
+#
+# @f_bfree: free blocks.
+#
+# @f_bavail: free blocks for non-root.
+#
+# @f_files: inodes.
+#
+# @f_ffree: free inodes.
+#
+# @f_favail: free inodes for non-root.
+#
+# @f_fsid: file system id.
+#
+# @f_flag: mount flags
+#
+# @f_namemax: maximum filename length.
+#
+# Since 1.5.10(NetEase)
+##
+{ 'type': 'GuestFileSystemStatistics',
+  'data': { 'f_bsize': 'int', 'f_frsize': 'int', 'f_blocks': 'int',
+            'f_bfree': 'int', 'f_bavail': 'int', 'f_files': 'int',
+            'f_ffree': 'int', 'f_favail': 'int', 'f_fsid': 'int',
+            'f_flag': 'int', 'f_namemax': 'int'} }
+
+##
+# @guest-get-statvfs:
+#
+# Get the information about guest file system statistics by statvfs.
+#
+# Returns: @GuestFileSystemStatistics.
+#
+# Since 1.5.10(NetEase)
+##
+{ 'command': 'guest-get-statvfs',
+  'data':    { 'path': 'str' },
+  'returns': 'GuestFileSystemStatistics' }
+
```
中间复杂的类型定义代码，以及头文件包含关系处理都由一个python脚本在编译的时候动态生成出来，这对开发人员来说是非常方便的，开发人员在扩展功能的时候只需要关注输入、输出的数据类型，以及功能的函数内容即可。

# qemu代码仓库和社区
QEMU代码仓库地址：git clone git://git.qemu-project.org/qemu.git

QEMU代码仓库在线查看：https://gitlab.com/qemu-project/qemu

qga代码位于QEMU代码的根目录下的qga目录中。

# 监控方案现状
目前普遍云主机监控的实现方法是，在创建云主机的过程中，增加监控脚本及其配置文件、定时任务及监控信息推送配置文件的注入过程，包括四个文件，其中监控信息推送配置文件(/etc/vm_monitor/info)由管理平台根据云主机所属用户的注册信息以及监控平台相关配置生成，并传入创建云主机的API来实现文件的注入，监控脚本(/etc/vm_monitor/send_monitor_data.py)及其配置文件(/etc/vm_monitor/monitor_settings.xml)、定时任务文件(/etc/cron.d/inject_cron_job)是包含在NVS经过base64编码后的监控脚本文件inject_files.json中。

工作模式为，在root账户增加定时任务inject_cron_job，其中有一条任务为：root su -c 'python /etc/vm_monitor/send_monitor_data.py' > /dev/null 2>&1，也即每60s收集并推送一次监控信息给监控平台。

当前监控方案的问题
- 依赖云主机内部的python解释器
- 云主机必须存在root账户
- 依赖NVS文件注入功能；并且为了注入这些监控文件对nova的改动也比较大，也无法与社区同步；windows镜像也会注入这些无用的文件，可能导致一些意想不到的问题；另外如果有的镜像的操作系统不在第一个分区上，则注入的监控文件会失效
- 已经运行的云主机内部的监控相关文件更新困难，导致新监控项的添加、推送周期、推送地址等的修改也比较困难，灵活性较差
- Nova中base64编码的注入脚本的代码可读性很差，代码更新及维护困难
- 定位问题一般都需要登录到云主机内部进行，对于采用密钥对登录的云主机来说定位问题比较困难

# 采用qga方式的监控方案
首先为每个云主机增加virtio-serial的配置，这个只需要修改生成libvirt配置文件的代码即可，并且应该可以提交给社区；其次需要在虚拟机内部安装qga服务；最后需要在宿主机上新增一个服务进程（这里暂定为monitor服务），用来通过与qga交互从云主机内部获取监控信息；

```
/usr/sbin/qemu-ga --daemonize -m virtio-serial -p /dev/virtio-ports/org.qemu.guest_agent.0
sudo /etc/init.d/qemu-guest-agent stop
sudo ./qemu-ga --daemonize -m virtio-serial -p /dev/virtio-ports/org.qemu.guest_agent.0 -f /var/run/qemu-ga.pid -t /var/run
```

# qga应用场景功能
qemu对外提供了一个socket接口，主要用于解决虚机外部无法实现的功能，例如：
- 在线修改密码
- 在线增加公钥
- 在线采集监控性能数据（如cpu使用率、负载、内存使用量等性能指标）
- 设备查看、变更
- 数据备份

# 参考资料
- [Qemu Guest Agent方式实现libvirt与虚机交互](https://blog.csdn.net/wuyongpeng0912/article/details/107846841)
- [利用Qemu Guest Agent (Qemu-ga) 实现 Openstack 监控平台](https://blog.csdn.net/beckdon/article/details/51074566)
- [通过QEMU-GuestAgent实现从外部注入写文件到KVM虚拟机内部](https://www.cnblogs.com/fzxiaomange/p/kvm-inject.html)
- [基于QMP实现对qemu虚拟机进行交互](https://www.cnblogs.com/fzxiaomange/p/qmp-qemu-interactive.html)
