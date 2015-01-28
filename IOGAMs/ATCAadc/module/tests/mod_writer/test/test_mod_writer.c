#include <linux/module.h>

extern long long int GetBufferAddress(int buff_num);


int test_mod_writer_init(void) {
	int i;
	
	for (i=0; i<4; i++){
		printk("test_mod_writer: buffer %d @ %p\n", i, GetBufferAddress(i));
	}
	
	return 0;
}

void test_mod_writer_exit(void) {
	return;
}

module_init(test_mod_writer_init);
module_exit(test_mod_writer_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Buffer Writer Test Test Module");
MODULE_AUTHOR("Riccardo Vitelli");