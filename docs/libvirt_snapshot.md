### 简介
快照是虚拟机操作系统及其在给定时间点的所有应用程序的视图。拥有虚拟机的可恢复快照是虚拟化环境的基本特征。快照允许用户及时保存虚拟机的状态并回滚到该状态。基本用例包括拍摄快照、安装新应用程序、更新或升级（发现它们很糟糕或坏了），然后回滚到之前的时间。

很明显，拍摄快照后发生的任何更改都不包含在快照中。快照不会持续更新。它表示虚拟机在单个时间点的状态。

### 创建快照
```
<domainsnapshot>
  <description>Snapshot of OS install and updates</description>
  <name>snap1</name>
  <disks>
    <disk name='hda' snapshot='external'>
      <driver type='qcow2'/>
    </disk>
    <disk name='vda' snapshot='no'/>
  </disks>
</domainsnapshot>
```
```
virsh snapshot-create --domain $DOMAIN --xmlfile /data/snapshot.xml --halt --disk-only --atomic
功能参数：
  --no-metadata    take snapshot but create no metadata  # 创建的快照不带任何元数据
  --halt           halt domain after snapshot is created # 快照创建后虚机会关闭
  --disk-only      capture disk state but not vm state   # 只对磁盘做快照，忽略虚拟机运行的内存和状态等信息
  --quiesce        quiesce guest's file systems #libvirt # 会通过QEMU GA尝试去freeze和unfreeze客户机已经mounted的文件系统；如果客户机没有安装QEMU GA，则操作会失败。
  --atomic         require atomic operation              # 快照要么完全成功要么完全失败，不允许部分成果。不是所有的VMM都支持。
  --live           take a live snapshot                  # 当客户机处于运行状态下做快照
```

### 查看快照
```
virsh snapshot-list $DOMAIN
# 树形查看快照。
virsh snapshot-list $DOMAIN --tree
root@kttm223:/home/dbc/dbc_mining_node# virsh snapshot-list 7SyTYc3NLouEgxmnVSef6Q --tree
snap1
  |
  +- snap2
      |
      +- snap3
      |   |
      |   +- snap4
      |
      +- snap5
```

### 查看快照XML描述
```
virsh snapshot-dumpxml <domain> <snapshotname> [--security-info]
```

### 查看当前快照
```
virsh snapshot-current $DOMAIN
```

### 恢复快照
```
virsh snapshot-revert --domain $DOMAIN --snapshotname $SNAPSHOTNAME --force
# libvirt不支持外部快照的恢复，会提示以下错误。
error: unsupported configuration: revert to external snapshot not supported yet
```
可以使用以下其他方案绕过外部快照的恢复:
1. 强制修改XML中的disk file，启动正常，不推荐使用此方法，因为如果后面有快照以当前镜像为`backing file`，后续快照将无法启动。
2. 使用`blockcommit`把外部增量快照合并到base上，或者使用`blockpull`合并到top快照上。
3. 把外部快照复制一份，然后用复制的文件创建新的虚拟机。

### 删除快照
```
virsh snapshot-delete [--domain] $DOMAIN --snapshotname $SNAPSHOTNAME
# libvirt不支持删除外部快照，会提示以下错误
libvirt: QEMU Driver error : unsupported configuration: deletion of 1 external disk snapshots not supported yet
```
删除外部快照的方法：
* [I created an external snapshot, but libvirt will not let me delete or revert to it](https://wiki.libvirt.org/page/I_created_an_external_snapshot,_but_libvirt_will_not_let_me_delete_or_revert_to_it)
* [浅析snapshots, blockcommit,blockpull](https://blog.csdn.net/weixin_33955681/article/details/91946156)

### 使用快照创建虚拟机
外部快照无法恢复，但是可以作为系统镜像来创建新虚拟机，如果`backing file`链异常，则会出现以下错误。
```
virDomainCreate error: Requested operation is not valid: format of backing image '/data/7KftmnWFCZYjahR5C7bi4G_snap1_vda.qcow2' of image '/data/7KftmnWFCZYjahR5C7bi4G_snap1_vda_7Kch7hkBpwb5oJMTQkbVz2.qcow2' was not specified in the image metadata (See https://libvirt.org/kbase/backing_chains.html for troubleshooting)
```

### 参考资料
* [libvirt snapshots](https://wiki.libvirt.org/page/Snapshots)
* [Snapshot XML format](https://libvirt.org/formatsnapshot.html)
* [Efficient live full disk backup](https://libvirt.org/kbase/live_full_disk_backup.html)
* [I created an external snapshot, but libvirt will not let me delete or revert to it](https://wiki.libvirt.org/page/I_created_an_external_snapshot,_but_libvirt_will_not_let_me_delete_or_revert_to_it)
* [libvirt 快照 （一）](https://blog.csdn.net/chenyulancn/article/details/12453199)
* [libvirt 快照 （二）](https://blog.csdn.net/chenyulancn/article/details/12453335)
* [libvirt/qemu特性之快照](https://blog.csdn.net/halcyonbaby/article/details/19709735)
* [KVM 介绍（7）](https://www.cnblogs.com/sammyliu/p/4468757.html)
