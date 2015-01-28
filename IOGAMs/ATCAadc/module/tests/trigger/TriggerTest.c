#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>

int master = 0;

int init_func(void)
{
    master = GetMasterBoard();
    printk("Sending trigger ...to master @:%d\n", master);
    enableATCApcieAcquisition(master, 1);
    return 0;
}

void exit_func(void)
{
    printk("Disabling acquisition from master @:%d\n", master);
    disableATCApcieAcquisition(master);
}

module_init(init_func);
module_exit(exit_func);
