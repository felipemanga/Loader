target remote :2331
add-symbol-file "./kernel/BUILD/boot.elf" 0x10000100
add-symbol-file "./Progress/BUILD/progress.elf" 0x10002000
