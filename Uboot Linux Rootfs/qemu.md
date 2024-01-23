```
qemu-system-ppc.exe -L pc-bios -boot c -prom-env "boot-device=hd:,\yaboot" -prom-env "boot-args=conf=hd:,\yaboot.conf" -M mac99,via=pmu -m 1024 -hda lubuntu14.qcow2 -net nic -net tap,ifname=tap0
```

