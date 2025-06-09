# BUILD

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build -G Ninja
ninja -C build

cmake -DCMAKE_BUILD_TYPE=Release -S . -B release -G Ninja
ninja -C release
```

# BINARY RUNTIME

## strace / ltrace

```bash
./build/src/001-syscalls   # what is it doing?

strace ./build/src/001-syscalls

ltrace ls
ltrace -e getenv ls
env | grep LS_
## ls.c:   print_with_color = (i == when_always || (i == when_if_tty && isatty (STDOUT_FILENO)));
ltrace -e getenv ls --color=always
```

## LD_PRELOAD

```bash
LD_PRELOAD=./build/src/libread-another-file.so ./build/src/001-syscalls
LD_PRELOAD=./build/src/libread-another-file.so strace ./build/src/001-syscalls
LD_PRELOAD=./build/src/libread-another-file.so ltrace ./build/src/001-syscalls
SECRET_FILE="/etc/gitconfig" LD_PRELOAD=./build/src/libread-another-file.so ./build/src/001-syscalls

date
LD_PRELOAD=./build/src/libback-to-the-future.so date
CLOCK_OFFSET=3600 LD_PRELOAD=./build/src/libback-to-the-future.so date
CLOCK_OFFSET=-3600 LD_PRELOAD=./build/src/libback-to-the-future.so date

strace -e %network python3 -m http.server --bind :: --directory . 8888
strace -e setsockopt python3 -m http.server --bind :: --directory . 8888
LD_PRELOAD=./build/src/libipv6-only.so strace -e setsockopt python3 -m http.server --bind :: --directory . 8888
curl -6 http://localhost:8888
curl -4 http://localhost:8888
```

## ldd

```bash
ldd `which curl`
LD_TRACE_LOADED_OBJECTS=1 curl

objdump -x `which curl`
```

## LD_AUDIT

```bash
LD_AUDIT=build/src/libauditor.so ./build/src/001-syscalls
LD_AUDIT=build/src/libauditor.so curl
sotruss curl
```

# PROCESSES

## ps, pstree

```bash
ps aux
ps -ef

pstree -catApulTSsng
```

## /proc/N/cmdline

```bash
cat /proc/self/cmdline /dev/null /dev/null
cat /proc/self/cmdline 'a file with spaces in name' 2>/dev/null | tr \\0 \\n
```

## /proc/N/fd, fdinfo

```bash
pv -q -L 10 -B 10 < DEMO.md
ls -l /proc/`pidof pv`/fd
watch -d -n1 "cat /proc/`pidof pv`/fdinfo/0"

# resurrect a deleted file
cp DEMO.md removable
pv -q -L 10 -B 10 < removable
rm removable
ls -l /proc/`pidof pv`/fd
cat /proc/`pidof pv`/fd/0 > restored
```

## /proc/N/status

```bash
docker ps
sudo cat /proc/$(pgrep -fx postgres)/status
```

## /proc/N/environ

```bash
sudo cat /proc/$(pgrep -fx postgres)/environ
```

## /proc/N/root

```bash
sudo ls -l /proc/$(pgrep -fx postgres)/root/

sudo su -
cd /proc/$(pgrep -fx postgres)/root/
```

## *top

```bash
top
htop
btop
atop
iotop
iftop
```

## mount -o noexec

```bash
mkdir -p tmp
sudo mount -t tmpfs tmpfs ./tmp/
(echo "#!/bin/sh"; echo "echo 'I am the script'") > ./tmp/script.sh
chmod +x ./tmp/script.sh
./tmp/script.sh

sudo mount -o remount,noexec ./tmp/
./tmp/script.sh
bash ./tmp/script.sh

sudo mount -o remount,exec ./tmp/
./tmp/script.sh

sudo umount tmp
```

## fork, exiting parent

```bash
./build/src/005-fork
pstree -catApulTSsng `pidof 005-fork`
lsns -t pid

killall 005-fork
```

# Networking

## netstat, ss

```bash
netstat
netstat -n
sudo netstat -nlpt
sudo lsns -t net -no PATH | while read ns; do echo; echo NS = $ns; sudo nsenter -n$ns netstat -nlpt; done
cat /proc/net/tcp
```

## wiretapping TLS v1 (SSLKEYLOGFILE)

```bash
wireshark

curl --verbose --http1.1 http://bartosz.codes/for-you/
curl --verbose --http1.1 https://bartosz.codes/for-you/
SSLKEYLOGFILE=./tls.keys curl --verbose --http1.1 https://bartosz.codes/for-you/

# Wireshark -> Preferences -> Protocols -> TLS -> (Pre)-Master-Secret log filename:
# /home/bartek/projects/Linux Detective Toolkit/code-repo/tls.keys
```

## wiretapping TLS v2 (mitmproxy)

```bash
ls -l /usr/lib/ssl/certs/
cat /usr/share/ca-certificates/mozilla/Security_Communication_ECC_RootCA1.crt
openssl x509 -noout -text -in /usr/share/ca-certificates/mozilla/Security_Communication_ECC_RootCA1.crt

curl --proxy http://127.0.0.1:8080 -v https://bartosz.codes/for-you/
#    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#    DNS or iptables redirection can be used instead

curl --proxy http://127.0.0.1:8080 --cacert ~/.mitmproxy/mitmproxy-ca-cert.cer -v https://bartosz.codes/for-you/
#                                  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#                                  or copy (sym-link) it to /usr/lib/ssl/certs
#                                  or append to /etc/ssl/certs/ca-certificates.crt

sudo ln -s ~/.mitmproxy/mitmproxy-ca-cert.cer /usr/lib/ssl/certs/
cat ~/.mitmproxy/mitmproxy-ca-cert.cer | sudo tee -a /etc/ssl/certs/ca-certificates.crt
```

# Multithreading 

```bash
./build/src/006-abba `# 2 threads, each loops times N =` 100 `# add D = ` 1
./build/src/006-abba `# 2 threads, each loops times N =` 1000 `# add D = ` 1

valgrind --tool=helgrind ./build/src/006-abba 1000 1

LD_PRELOAD=~/git/lksmith/install/lib/liblksmith.so ./build/src/006-abba 100 1

./build/src/006-abba 1000 1
gdb -p `pidof 006-abba` # ptrace: Operation not permitted. (kernel.yama.ptrace_scope = 1)
sudo gdb -p `pidof 006-abba`

# by Adam Szaj
source /home/bartek/.gdb/find_deadlock.py
t a a bt
find_deadlock

# by Damian Ziobro
source /home/bartek/git/gdb-automatic-deadlock-detector/gdbcommands
```
