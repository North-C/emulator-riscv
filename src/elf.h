/* 定义ELF文件格式 */
#ifndef _ELF_H
#define _ELF_H
#include "types.h"

#define EI_IDENT_NUM 16
#define EI_MAGIC "\177ELF"

#define EI_CLASS 4
#define ELF_CLASS_NUM 3
#define ELF_CLASS_NONE 0
#define ELF_CLASS_32 1
#define ELF_CLASS_64 2

#define EI_LITTLE_END 1
#define EI_BIG_END 2

#define EI_VERSION 1
#define EM_RISCV 0xF3

#define PT_LOAD 1

#define PT_X 0x1
#define PT_W 0x2
#define PT_R 0x4

#define R_X86_64_PC32 2

typedef struct {
    u8 e_ident[EI_IDENT_NUM];
    u16 e_type;
    u16 e_machine;
    u32 e_version;
    u64 e_entry;  // 程序入口
    u64 e_phoff;
    u64 e_shoff;
    u32 e_flags;
    u16 e_ehsize;
    u16 e_phentsize;
    u16 e_phnum;
    u16 e_shentsize;
    u16 e_shnum;
    u16 e_shstrndx;
} elf64_ehdr_t;

typedef struct {
    u32 p_type;
    u32 p_flags;
    u64 p_offset;
    u64 p_vaddr;
    u64 p_paddr;
    u64 p_filesz;
    u64 p_memsz;
    u64 p_align;
} elf64_phdr_t;

typedef struct {
    u32 sh_name;
    u32 sh_type;
    u32 sh_flags;
    u64 sh_addr;
    u64 sh_offset;
    u64 sh_size;
    u32 sh_link;
    u32 sh_info;
    u64 sh_addralign;
    u64 sh_entsize;
} elf64_shdr_t;

typedef struct {
    u32 st_name;
    u8 st_info;
    u8 st_other;
    u16 st_shndx;
    u64 st_value;
    u64 st_size;
} elf64_sym_t;

typedef struct {
    u64 r_offset;
    u32 r_type;
    u32 r_sym;
    i64 r_addend;
} elf64_rela_t;

#endif
