/* 
 * This file is part of TermOS.
 *
 * TermOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TermOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TermOS.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <fs/exec.h>
#include <fs/vfs.h>
#include <fs/vfs_cache.h>
#include <kernel/elf.h>
#include <kernel/tty.h>
#include <mm/mem.h>
#include <mm/paging.h>
#include <lib/stdio.h>
#include <lib/string.h>

extern page_directory_t *kernel_directory;

extern u32 read_eip();

static void debug_ehdr(struct elf_hdr *ehdr)
{
    printf("--- ELF HEADER ---\nclass: %d\ntype: %d\nmachine: %d\nversion: %d\nentry: %x\nphoff: %d\nsphoff: %d\nflags:%d\nehsize: %d\nphentsize: %d\nphnum: %d\nshentsize: %d\nshnum: %d\nshstrndx: %d\n\n",
        ehdr->ident[EI_CLASS], ehdr->type, ehdr->machine, ehdr->version, ehdr->entry,
        ehdr->phoff, ehdr->sphoff, ehdr->flags, ehdr->ehsize,
        ehdr->phentsize, ehdr->phnum, ehdr->shentsize,
        ehdr->shnum, ehdr->shstrndx);
}

static void debug_phdr(struct prg_hdr *phdr)
{
    printf("--- PRG HEADER ---\ntype: %d\noffset: %d\nvaddr: %x\npaddr: %x\nfileSize: %d\nmemSize: %d\nflags: %d\nalignment: %d\n\n",
        phdr->type, phdr->offset, phdr->vaddr, phdr->paddr,
        phdr->fileSize, phdr->memSize, phdr->flags, phdr->alignment);
}

void v_exec(struct vnode *node)
{
    char *buf = (char*) kmalloc(node->v_size);
    FILE *fh = vfs_open(node, VFS_READ);

    /* Read the ELF header */
    vfs_read(fh, buf, sizeof(struct elf_hdr));
    struct elf_hdr *ehdr = (struct elf_hdr*) buf;
    debug_ehdr(ehdr);
    ASSERT(ehdr->phnum);
    ASSERT(ehdr->ident[EI_CLASS] == ELFCLASS32);

    /* Read the PRG headers */
    vfs_read(fh, &buf[fh->f_offset], sizeof(struct prg_hdr) * ehdr->phnum);
    struct prg_hdr *phdr = (struct prg_hdr*) (buf + ehdr->phoff);

    page_directory_t *page_directory = kernel_directory;

    int i;
    for (i = 0;  i < ehdr->phnum; ++i)
    {
        debug_phdr(&phdr[i]);
        if (phdr[i].type == PT_LOAD)
        {
            ASSERT(phdr[i].memSize);
            ASSERT(phdr[i].memSize >= phdr[i].fileSize);

            /* See if we need to read more from the file */
            int fh_diff = (phdr[i].offset + phdr[i].fileSize - fh->f_offset );
            if (fh_diff > 0)
                vfs_read(fh, &buf[fh->f_offset], fh_diff);

            /* Allocate the frames this headers wants us to copy to */
            int writable = (phdr[i].flags & PF_W) ? 1 : 0;

            u32 vaddr = phdr[i].vaddr;
            while (vaddr <= phdr[i].vaddr + phdr[i].memSize)
            {
                alloc_frame(get_page(vaddr, 1, page_directory), 0, writable);
                vaddr += 0x1000;
            }

            /* set unused areas in memory to '\0' */
            if (phdr[i].memSize > phdr[i].fileSize)
                memset((void*) phdr[i].vaddr + phdr[i].fileSize, 0, phdr[i].memSize - phdr[i].fileSize);

            memcpy((void*) phdr[i].vaddr, &buf[phdr[i].offset], phdr[i].fileSize);
        }
        else
        {
            //PANIC("Unknown header type!!");
        }
    }

    vfs_close(fh);

    /* Do the actual execution */
    int first = 1;
    u32 eip = read_eip();

    if (first)
    {
        first = 0;
        asm ("push %0\n"
             "jmp *%1\n"
            :
            : "r" (eip), "r" (ehdr->entry));
    }
    else
        printf("eax: %x\n", eip);

    /* Free the frames this application used. */
    for (i = 0;  i < ehdr->phnum; ++i)
    {
        if (phdr[i].type == PT_LOAD)
        {
            u32 vaddr = phdr[i].vaddr;
            while (vaddr <= phdr[i].vaddr + phdr[i].memSize)
            {
                free_frame(get_page(vaddr, 0, page_directory));
                vaddr += 0x1000;
            }
        }
    }

    kfree(ehdr);
}
