#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
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
	{ 0x440b06f4, "module_layout" },
	{ 0x8e09f445, "cdev_del" },
	{ 0x386561a7, "cdev_init" },
	{ 0x4c4fef19, "kernel_stack" },
	{ 0xe3115202, "dev_set_drvdata" },
	{ 0xd50bf783, "dma_set_mask" },
	{ 0x4d884691, "malloc_sizes" },
	{ 0xd7cad316, "pci_disable_device" },
	{ 0xf31ede69, "__rt_mutex_init" },
	{ 0x2aaa9140, "remove_proc_entry" },
	{ 0x1c15c176, "x86_dma_fallback_dev" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0xeaba1ea8, "pci_bus_write_config_word" },
	{ 0x91715312, "sprintf" },
	{ 0x89887225, "__init_waitqueue_head" },
	{ 0x71de9b3f, "_copy_to_user" },
	{ 0xa008145b, "pci_set_master" },
	{ 0x27e1a049, "printk" },
	{ 0x16305289, "warn_slowpath_null" },
	{ 0x9638fbf2, "__rt_spin_lock_init" },
	{ 0xb314e453, "cdev_add" },
	{ 0x9c5fa907, "pci_set_mwi" },
	{ 0x78764f4e, "pv_irq_ops" },
	{ 0x42c8de35, "ioremap_nocache" },
	{ 0xe5090132, "pci_bus_read_config_word" },
	{ 0x8c64eba2, "create_proc_entry" },
	{ 0xb4035192, "pci_unregister_driver" },
	{ 0x7a172b67, "kmem_cache_alloc_trace" },
	{ 0xe52947e7, "__phys_addr" },
	{ 0xb27d6a83, "pci_clear_mwi" },
	{ 0x37a0cba, "kfree" },
	{ 0x963d01de, "remap_pfn_range" },
	{ 0x8173c477, "dma_supported" },
	{ 0xedc03953, "iounmap" },
	{ 0x5a574b29, "__pci_register_driver" },
	{ 0x436c2179, "iowrite32" },
	{ 0x8c30ce32, "pci_enable_device" },
	{ 0x77e2f33, "_copy_from_user" },
	{ 0x4057d7d4, "dev_get_drvdata" },
	{ 0x662d305, "dma_ops" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0xe484e35f, "ioread32" },
	{ 0xe914e41e, "strcpy" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("pci:v000010EEd00000007sv*sd*bc*sc*i*");
