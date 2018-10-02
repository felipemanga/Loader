target remote :1234
add-symbol-file "./kernel/BUILD/boot.elf" 0x10000100
add-symbol-file "./desktop/BUILD/DESKTOP.elf" 0x10002000
