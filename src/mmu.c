#include "elf.h"
#include "emulator.h"

static void load_phdr(elf64_ehdr_t* ehdr, elf64_phdr_t* phdr, u64 idx,
                      FILE* file) {
    if (fseek(file, ehdr->e_phoff + ehdr->e_phentsize * idx, SEEK_SET) != 0) {
        Fatal("seek file failed");
    }

    if (fread((void*)phdr, 1, sizeof(elf64_phdr_t), file) !=
        sizeof(elf64_phdr_t)) {
        Fatal("read file failed");
    }
}

static int flags_to_prot(u32 flags) {
    return (flags & PT_R ? PROT_READ : 0) | (flags & PT_W ? PROT_WRITE : 0) |
           (flags & PT_X ? PROT_EXEC : 0);
}

static void mmu_load_segment(mmu_t* mmu, elf64_phdr_t* phdr, int fd) {
    int page_size = getpagesize();
    u64 offset = phdr->p_offset;
    u64 vaddr = TO_HOST(phdr->p_vaddr);
    u64 aligned_vaddr = ROUNDDOWN(vaddr, page_size);
    u64 filesz = phdr->p_filesz + (vaddr - aligned_vaddr);
    u64 memsz = phdr->p_memsz + (vaddr - aligned_vaddr);
    int prot = flags_to_prot(phdr->p_flags);

    // mmap 进行匿名映射，并从 aligned_vaddr 处申请内存
    u64 addr =
        (u64)mmap((void*)aligned_vaddr, filesz, prot, MAP_PRIVATE | MAP_FIXED,
                  fd, ROUNDDOWN(offset, page_size));
    assert(addr == aligned_vaddr);

    // 加载 bss 段
    u64 remaining_bss = ROUNDUP(memsz, page_size) - ROUNDUP(filesz, page_size);
    if (remaining_bss > 0) {
        // 超过一页
        u64 raddr = (u64)mmap(
            (void*)(aligned_vaddr + ROUNDUP(filesz, page_size)), remaining_bss,
            prot, MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, -1, 0);

        assert(raddr == aligned_vaddr + ROUNDUP(filesz, page_size));
    }

    mmu->host_alloc =
        MAX(mmu->host_alloc, (aligned_vaddr + ROUNDUP(memsz, page_size)));

    mmu->base = mmu->guest_alloc = TO_GUEST(mmu->host_alloc);
}

void mmu_load_elf(mmu_t* mmu, int fd) {
    u8 buf[sizeof(elf64_ehdr_t)];

    FILE* file = fdopen(fd, "rb");
    if (fread(buf, 1, sizeof(elf64_ehdr_t), file) != sizeof(elf64_ehdr_t)) {
        Fatal("file too small");
    }

    elf64_ehdr_t* ehdr = (elf64_ehdr_t*)buf;
    if (*(u32*)ehdr != *(u32*)EI_MAGIC) {
        Fatal("bad elf file");
    }

    if (ehdr->e_machine != EM_RISCV ||
        ehdr->e_ident[EI_CLASS] != ELF_CLASS_64) {
        Fatal("only support RISCV-64!");
    }

    mmu->entry = (u64)ehdr->e_entry;

    elf64_phdr_t phdr;
    for (i64 i = 0; i < ehdr->e_phnum; i++) {
        load_phdr(ehdr, &phdr, i, file);  // 加载程序头
        // 加载 LOAD 程序段
        if (phdr.p_type == PT_LOAD) mmu_load_segment(mmu, &phdr, fd);
    }
}

/* allocate and release memory for program */
u64 mmu_alloc(mmu_t* mmu, i64 sz) {
    int pagesz = getpagesize();
    u64 base = mmu->guest_alloc;
    assert(base >= mmu->base);

    mmu->guest_alloc += sz;
    assert(mmu->guest_alloc >= mmu->base);

    // 不够用时，再申请
    if (sz > 0 && mmu->guest_alloc > TO_GUEST(mmu->host_alloc)) {
        if (mmap((void*)mmu->host_alloc, ROUNDUP(sz, pagesz),
                 PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1,
                 0) == MAP_FAILED)
            Fatal("mmap failed");

        mmu->host_alloc += ROUNDUP(sz, pagesz);
    } else if (sz < 0 &&
               ROUNDUP(mmu->guest_alloc, pagesz) < TO_GUEST(mmu->host_alloc)) {
        u64 len = TO_GUEST(mmu->host_alloc) - ROUNDUP(mmu->guest_alloc, pagesz);

        if (munmap((void*)mmu->host_alloc, len) == -1) Fatal(strerror(errno));
        mmu->host_alloc -= len;
    }

    return base;
}