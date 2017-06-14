cmd_/root/target_dir/jprobe.ko := ld -r -m elf_x86_64 -T ./scripts/module-common.lds --build-id  -o /root/target_dir/jprobe.ko /root/target_dir/jprobe.o /root/target_dir/jprobe.mod.o
