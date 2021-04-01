#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xaaa11587, __VMLINUX_SYMBOL_STR(module_layout) },
	{        0, __VMLINUX_SYMBOL_STR(free) },
	{ 0xaf96cfb5, __VMLINUX_SYMBOL_STR(kmem_cache_destroy) },
	{ 0x6bf1c17f, __VMLINUX_SYMBOL_STR(pv_lock_ops) },
	{ 0x349cba85, __VMLINUX_SYMBOL_STR(strchr) },
	{ 0xd0d8621b, __VMLINUX_SYMBOL_STR(strlen) },
	{ 0x69c03fc5, __VMLINUX_SYMBOL_STR(__dynamic_pr_debug) },
	{ 0x999e8297, __VMLINUX_SYMBOL_STR(vfree) },
	{ 0x44b5ee9a, __VMLINUX_SYMBOL_STR(kasprintf) },
	{ 0x8329e6f0, __VMLINUX_SYMBOL_STR(memset) },
	{ 0x11089ac7, __VMLINUX_SYMBOL_STR(_ctype) },
	{ 0x50eedeb8, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xb6e41883, __VMLINUX_SYMBOL_STR(memcmp) },
	{ 0xd986dad1, __VMLINUX_SYMBOL_STR(kernel_fpu_begin) },
	{ 0x7c1372e8, __VMLINUX_SYMBOL_STR(panic) },
	{ 0xb4390f9a, __VMLINUX_SYMBOL_STR(mcount) },
	{ 0x4e582977, __VMLINUX_SYMBOL_STR(kmem_cache_free) },
	{ 0xc3fa6a59, __VMLINUX_SYMBOL_STR(memchr) },
	{ 0xd085a479, __VMLINUX_SYMBOL_STR(kmem_cache_alloc) },
	{ 0x38722f80, __VMLINUX_SYMBOL_STR(kernel_fpu_end) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0xe259ae9e, __VMLINUX_SYMBOL_STR(_raw_spin_lock) },
	{ 0x16e7342a, __VMLINUX_SYMBOL_STR(kmem_cache_create) },
	{        0, __VMLINUX_SYMBOL_STR(malloc) },
	{ 0x5705088a, __VMLINUX_SYMBOL_STR(__vmalloc) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x4a619f83, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0xb81960ca, __VMLINUX_SYMBOL_STR(snprintf) },
	{ 0x8235805b, __VMLINUX_SYMBOL_STR(memmove) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=libc";


MODULE_INFO(srcversion, "B40F483BB7E36A42C6FC058");
