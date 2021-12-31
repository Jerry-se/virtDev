# qemu命令
## info 命令 - 查看镜像文件信息
```
info [-f fmt] filename
```
展示filename镜像文件的信息。如果文件是使用稀疏文件的存储方式，也会显示出它的本来分配的大小以及实际已占用的磁盘空间大小。如果文件中存放有客户机快照，快照的信息也会被显示出来。
```
qemu-img info ubuntu.qcow2
```

## create命令 - 创建镜像文件
```
create [-f fmt] [-o options] filename [size]
```
创建一个格式为fmt大小为size文件名为filename的镜像文件。根据文件格式fmt的不同，还可以添加一个或多个选项（options）来附加对该文件的各种功能设置，可以使用“-o ?”来查询某种格式文件支持那些选项，在“-o”选项中各个选项用逗号来分隔。

如果“-o”选项中使用了backing_file这个选项来指定其后端镜像文件，那么这个创建的镜像文件仅记录与后端镜像文件的差异部分。后端镜像文件不会被修改，除非在QEMU monitor中使用“commit”命令或者使用“qemu-img commit”命令去手动提交这些改动。这种情况下，size参数不是必须需的，其值默认为后端镜像文件的大小。另外，直接使用“-b backfile”参数也与“-o backing_file=backfile”效果相同。

size选项用于指定镜像文件的大小，其默认单位是字节（bytes），也可以支持k（或K）、M、G、T来分别表示KB、MB、GB、TB大小。另外，镜像文件的大小（size）也并非必须写在命令的最后，它也可以被写在“-o”选项中作为其中一个选项。

对create命令的演示如下所示，其中包括查询qcow2格式支持的选项、创建有backing_file的qcow2格式的镜像文件、创建没有backing_file的10GB大小的qcow2格式的镜像文件。
```
qemu-img create -f qcow2 ubuntu.qcow2 100G
qemu-img create -f qcow2 -F qcow2 -b $BACKING_IMAGE_PATH $IMAGE_PATH
qemu-img create -f qcow2 -o backing_file=d1,backing_fmt=qcow2 active1
qemu-img create -f qcow2 -o backing_file=ubuntu_copy.qcow2,backing_fmt=qcow2 ubuntu_base.qcow2
```

## resize命令 - 修改镜像文件大小
```
resize filename [+ | -]size
```
改变镜像文件的大小，使其不同于创建之时的大小。“+”和“-”分别表示增加和减少镜像文件的大小，而size也是支持K、M、G、T等单位的使用。缩小镜像的大小之前，需要在客户机中保证里面的文件系统有空余空间，否则会数据丢失，另外，qcow2格式文件不支持缩小镜像的操作。在增加了镜像文件大小后，也需启动客户机到里面去应用“fdisk”、“parted”等分区工具进行相应的操作才能真正让客户机使用到增加后的镜像空间。不过使用resize命令时需要小心（最好做好备份），如果失败的话，可能会导致镜像文件无法正常使用而造成数据丢失。
```
# qemu-img resize rhel6u3-a.img +2G
# qemu-img resize rhel6u3-b.img -1G
```

## rebase命令
```
rebase [-f fmt] [-t cache] [-p] [-u] -b backing_file [-F backing_fmt] filename
```
改变镜像文件的后端镜像文件，只有qcow2和qed格式支持rebase命令。使用“-b backing_file”中指定的文件作为后端镜像，后端镜像也被转化为“-F backing_fmt”中指定的后端镜像格式。

它可以工作于两种模式之下，一种是安全模式（Safe Mode）也是默认的模式，qemu-img会去比较原来的后端镜像与现在的后端镜像的不同进行合理的处理；另一种是非安全模式（Unsafe Mode），是通过“-u”参数来指定的，这种模式主要用于将后端镜像进行了重命名或者移动了位置之后对前端镜像文件的修复处理，由用户去保证后端镜像的一致性。
```
现在把sn3的后端指向到sn1
# qemu-img rebase -u -b sn1.qcow2 sn3.qcow2
注意: -u代表'Unsafe mode' -- 此模式下仅仅修改了指向到的backing file名字，必须谨慎操作
```

## snapshot 命令
```
snapshot [-l | -a snapshot | -c snapshot | -d snapshot] filename
```
“-l” 选项是查询并列出镜像文件中的所有快照，“-a snapshot”是让镜像文件使用某个快照，“-c snapshot”是创建一个快照，“-d”是删除一个快照。

# 查看qemu支持的虚拟机型号信息列表
```
qemu-system-x86_64 -machine help
```

# 查看qemu版本信息
```
qemu-system-x86_64 --version
QEMU emulator version 2.11.1(Debian 1:2.11+dfsg-1ubuntu7.37)
Copyright (c) 2003-2017 Fabrice Bellard and the QEMU Project developers
```
