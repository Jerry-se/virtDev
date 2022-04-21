# build qemu
```
../configure --prefix=/usr/local --sysconfdir=/etc --localstatedir=/var
prefix 默认 /usr/local
sysconfdir 默认 /usr/local/etc/qemu
localstatedir 默认 /usr/local/var
make qemu-ga
```
[build wiki](https://wiki.qemu.org/Hosts/Linux)

# build qemu for win64
```
# cross build on ubuntu
../configure --cross-prefix=amd64-mingw32msvc-

# cross build on fedora
../configure --cross-prefix=x86_64-w64-mingw32- --enable-guest-agent --enable-guest-agent-msi

# native build on windows with msys2
../configure --enable-gtk --enable-sdl --enable-guest-agent --enable-guest-agent-msi
```
[QEMU Binaries for Windows (64 bit)](https://qemu.weilnetz.de/w64/)

[build instructions](https://wiki.qemu.org/Hosts/W32#Debian_based_cross_builds)

# docker build
https://wiki.qemu.org/Testing/DockerBuild

# 参考资料
- [QEMU编译](https://gitlab.com/qemu-project/qemu/-/blob/master/docs/devel/build-system.rst)
