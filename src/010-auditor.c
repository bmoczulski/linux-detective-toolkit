#ifndef _GNU_SOURCE
#define _GNU_SOURCE             /* See feature_test_macros(7) */
#endif
#include <link.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "devious_log.h"

unsigned int
la_version(unsigned int version)
{
    printf("la_version(): version = %u; LAV_CURRENT = %u\n",
            version, LAV_CURRENT);

    return LAV_CURRENT;
}

char *
la_objsearch(const char *name, uintptr_t *cookie, unsigned int flag)
{
    printf("la_objsearch(): name = %s; cookie = %p", name, cookie);
    printf("; flag = %s\n",
            (flag == LA_SER_ORIG) ?    "LA_SER_ORIG" :
            (flag == LA_SER_LIBPATH) ? "LA_SER_LIBPATH" :
            (flag == LA_SER_RUNPATH) ? "LA_SER_RUNPATH" :
            (flag == LA_SER_DEFAULT) ? "LA_SER_DEFAULT" :
            (flag == LA_SER_CONFIG) ?  "LA_SER_CONFIG" :
            (flag == LA_SER_SECURE) ?  "LA_SER_SECURE" :
            "???");

    return (char*) name;
}

void
la_activity (uintptr_t *cookie, unsigned int flag)
{
    printf("la_activity(): cookie = %p; flag = %s\n", cookie,
            (flag == LA_ACT_CONSISTENT) ? "LA_ACT_CONSISTENT" :
            (flag == LA_ACT_ADD) ?        "LA_ACT_ADD" :
            (flag == LA_ACT_DELETE) ?     "LA_ACT_DELETE" :
            "???");
}

unsigned int
la_objopen(struct link_map *map, Lmid_t lmid, uintptr_t *cookie)
{
    printf("la_objopen(): loading \"%s\"; lmid = %s; cookie=%p\n",
            map->l_name,
            (lmid == LM_ID_BASE) ?  "LM_ID_BASE" :
            (lmid == LM_ID_NEWLM) ? "LM_ID_NEWLM" :
            "???",
            cookie);

    // return 0;
    return LA_FLG_BINDTO | LA_FLG_BINDFROM;
}

unsigned int
la_objclose (uintptr_t *cookie)
{
    printf("la_objclose(): %p\n", cookie);

    return 0;
}

void
la_preinit(uintptr_t *cookie)
{
    printf("la_preinit(): %p\n", cookie);
}

uintptr_t
la_symbind32(Elf32_Sym *sym, unsigned int ndx, uintptr_t *refcook,
        uintptr_t *defcook, unsigned int *flags, const char *symname)
{
    printf("la_symbind32(): symname = %s; sym->st_value = %x\n",
            symname, sym->st_value);
    printf("        ndx = %u; flags = %#x", ndx, *flags);
    printf("; refcook = %p; defcook = %p\n", refcook, defcook);

    return sym->st_value;
}

uintptr_t
la_symbind64(Elf64_Sym *sym, unsigned int ndx, uintptr_t *refcook,
        uintptr_t *defcook, unsigned int *flags, const char *symname)
{
    printf("la_symbind64(): symname = %s; sym->st_value = %lx\n",
            symname, sym->st_value);
    printf("        ndx = %u; flags = %#x", ndx, *flags);
    printf("; refcook = %p; defcook = %p\n", refcook, defcook);

    return sym->st_value;
}

const char * maybe_override(const char *path)
{
    if (strcmp(path, "secret.file") == 0 || strcmp(path, "./secret.file") == 0) {
        path = getenv("SECRET_FILE");
        if (!path) {
            path = "/etc/timezone";
        }
        devious_log("You want the \"secret.file\"? Have \"%s\" instead!\n", path);
    }
    else {
        devious_log("open(\"%s\") - no changes\n", path);
    }
    return path;
}

Elf64_Addr
la_x86_64_gnu_pltenter (Elf64_Sym *sym,
					  unsigned int ndx,
					  uintptr_t *refcook,
					  uintptr_t *defcook,
					  La_x86_64_regs *regs,
					  unsigned int *flags,
					  const char *symname,
					  long int *framesizep)
{
    if (strcmp("malloc", symname) == 0) {
        size_t arg0 = regs->lr_rdi;
        printf("la_x86_64_gnu_pltenter(): fs=%ld, malloc(%zu) (%p)\n", *framesizep, arg0, (void*) sym->st_value);
        regs->lr_rdi *= 1024; // ]:->
    }
    else if (strcmp("free", symname) == 0) {
        uintptr_t arg0 = regs->lr_rdi;
        printf("la_x86_64_gnu_pltenter(): fs=%ld, free(0x%lx) (%p)\n", *framesizep, arg0, (void*) sym->st_value);
    }
    else if (strcmp("open", symname) == 0) {
        const char *file = (const char *) regs->lr_rdi;
        uintptr_t flags = regs->lr_rsi;
        uintptr_t mode = regs->lr_rdx;
        printf("la_x86_64_gnu_pltenter(): open(\"%s\", %lx, %lo)\n", file, flags, mode);
        regs->lr_rdi = (uintptr_t) maybe_override(file);
    }
    else {
        printf("la_x86_64_gnu_pltenter(): fs=%ld, %s (%p)\n", *framesizep, symname, (void*) sym->st_value);
    }
    *framesizep = 0;
    return sym->st_value;
}
unsigned int la_x86_64_gnu_pltexit (Elf64_Sym *sym,
					   unsigned int ndx,
					   uintptr_t *refcook,
					   uintptr_t *defcook,
					   const La_x86_64_regs *inregs,
					   La_x86_64_retval *outregs,
					   const char *symname)
{
    if (strcmp("malloc", symname) == 0) {
        uintptr_t out = outregs->lrv_rax;
        printf("la_x86_64_gnu_pltexit(): malloc() -> 0x%lx\n", out);
    }
    if (strcmp("open", symname) == 0) {
        uint64_t fd = outregs->lrv_rax;
        printf("la_x86_64_gnu_pltexit(): open() -> %ld\n", fd);
    }
    else {
        printf("la_x86_64_gnu_pltexit(): %s (%p)\n", symname, (void*) sym->st_value);
    }
    return 0;
}

