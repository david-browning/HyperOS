arch ?= x86_64
partitiontable ?= msdos
isosize := 512M

#Byte offset to special partitions
efioffset := 1048576
bootoffset := 210763776

#Files and Folders
efimount := /tmp/hyperosefi
bootmount := /tmp/hyperosboot
kernel := build/kernel-$(arch).bin
iso := build/os-$(arch).iso

ifdef EFI_BUILD
	linker_script := src/arch/$(arch)/efilinker.ld
else
	linker_script := src/arch/$(arch)/bioslinker.ld
endif

grub_cfg := src/arch/$(arch)/grub.cfg

assembly_source_files := $(wildcard src/arch/$(arch)/*.asm)
assembly_object_files := $(patsubst src/arch/$(arch)/%.asm, \
	build/arch/$(arch)/%.o, $(assembly_source_files))

cpp_source_files := $(wildcard src/*.cpp) $(wildcard src/arch/$(arch)/*.cpp)
cpp_object_files := $(patsubst src/%.cpp, build/arch/$(arch)/%.o, $(cpp_source_files))

c_source_files := $(wildcard src/*.c) 
c_object_files := $(patsubst src/%.c, \
	build/arch/$(arch)/%.o, $(c_source_files)) 
#End Files

#Programs
CXX ?= /usr/local/cross/bin/$(arch)-elf-g++ 
CC ?= /usr/local/cross/bin/$(arch)-elf-gcc 
LD := /usr/local/cross/bin/$(arch)-elf-g++ 
ifeq ($(CXX), icpc)
	CXX = icpc 
	CC = icc 
endif
	

#End Programs

#Program Flags
CFLAGS := -c -std=c++14 -ffreestanding -fno-exceptions -fno-rtti -Wall -Wextra -mno-red-zone -Isrc/include/ \
	-Isrc/arch/$(arch)/ -O0 -fPIC -fno-threadsafe-statics 
LDFLAGS :=  -T $(linker_script) -ffreestanding -nostdlib -fPIC
QEMUFLAGS := -serial stdio 
GRUBFLAGS := --no-floppy --root-directory=$(bootmount) --modules="normal part_msdos part_gpt ext2 multiboot" 

ifdef EFI_BUILD
	QEMUFLAGS+=-L .
	partitiontable := gpt 
	GRUBFLAGS+=--target=x86_64-efi --efi-directory=/dev/loop1 
else
	partitiontable := msdos 
	GRUBFLAGS+=--target=i386-pc 
endif

#Additional Flags for making the code debug-able. 
ifdef DEBUG
	QEMUFLAGS+=-s  
	CFLAGS+=-g -DDEBUG -O0 
	LDFLAGS+=$(CFLAGS) 
endif

#Additional Flags if using the Intel Compiler
ifeq ($(CXX), icpc)
	CFLAGS+=-fasm-blocks -falign-loops -c -debug full -fasm-blocks -Wdeprecated -Wcheck \
	-use-msasm -masm=intel -intel-extensions -Winline -inline-calloc -diag-disable:2015 \
	-diag-disable:2012 -diag-disable:161 -diag-disable:2014 -diag-disable:1125 
endif
#End Flags

.PHONY: all clean run iso

all: $(kernel)

clean:
	@rm -rf build

rebuild: clean iso

run: $(iso)
	qemu-system-$(arch) -hda $(iso) $(QEMUFLAGS) 

iso: $(iso)

$(iso): $(kernel) $(grub_cfg)
	@mkdir -p build/isofiles/boot/grub2
	@mkdir -p $(efimount)
	mkdir -p $(bootmount)
	@cp $(kernel) build/isofiles/boot/kernel.bin
	@cp $(grub_cfg) build/isofiles/boot/grub2
	@rm -f $(iso)

	@echo "Creating disk..."
	@fallocate $(iso) -l $(isosize)
	@parted $(iso) mklabel $(partitiontable)

	@parted $(iso) mkpart primary fat32 $(efioffset)b 411647s
ifdef EFI_BUILD
	@parted $(iso) name 1 '"EFI System Partition"'
	@parted $(iso) set 1 boot on
endif

	@parted $(iso) mkpart primary fat32 $(bootoffset)b 821248s
	@parted $(iso) set 2 boot on
ifdef EFI_BUILD
	@parted $(iso) name 2 '"BOOT"'
endif
	@echo "Disk partitioning complete."

	@echo "Configuring boot partition..."
	@losetup /dev/loop2 $(iso) -o $(bootoffset)
	@mkdosfs -F32 -f 2 /dev/loop2
	@mount /dev/loop2 $(bootmount)
	@cp -r build/isofiles/* $(bootmount)/
	
	@echo "Configuring EFI partition..."
	@losetup /dev/loop1 $(iso) -o $(efioffset)
	@mkdosfs -F32 -f 2 /dev/loop1
	@mount /dev/loop1 $(efimount)
	#TODO: Copy the EFI boot program here...
	#
	#
	#
	#
	@umount $(efimount)
	@losetup -d /dev/loop1
	
	@echo "Configuring MBR..."
	@losetup /dev/loop0 $(iso)

	@grub2-install $(GRUBFLAGS) /dev/loop0
	@umount $(bootmount) 
	@losetup -d /dev/loop2 
	@losetup -d /dev/loop0 
	@echo "Done setting up partitions."
	
	@chmod 777 $(iso)
	@parted $(iso) print
	@rm -r build/isofiles
	@echo "Done configuring disk images."

$(kernel): $(cpp_object_files) $(c_object_files) $(assembly_object_files) $(linker_script)
	@echo "Linking..."
	@$(LD) $(LDFLAGS) -n -o $(kernel) $(assembly_object_files) $(cpp_object_files) $(c_object_files)

# compile assembly files
build/arch/$(arch)/%.o: src/arch/$(arch)/%.asm
	@echo "Using NASM for" $<
	@mkdir -p $(shell dirname $@)
	@nasm -f elf64 $< -o $@

# compile CPP files
build/arch/$(arch)/%.o : src/%.cpp
ifeq ($(CXX), icpc)
	@echo "Using Intel Compiler for" $<
else
	echo "Using G++ for" $<
endif
	@mkdir -p $(shell dirname $@)
	@$(CXX) $(CFLAGS) $< -o $@

# compile C files
build/arch/$(arch)/%.o : src/%.c
ifeq ($(CXX), icpc)
	@echo "Using Intel Compiler for" $<
else
	@echo "Using GCC for" $<
endif
	@mkdir -p $(shell dirname $@)
	@$(CC) $(CFLAGS) $< -o $@