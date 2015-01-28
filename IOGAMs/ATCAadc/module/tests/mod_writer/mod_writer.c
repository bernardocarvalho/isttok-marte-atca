//******************************************************************************
//    $Log: mod_writer.c,v $
//    Revision 1.3  2008/06/17 15:16:11  rvitelli
//    Removed verbose output.
//
//    Revision 1.2  2008/06/16 09:30:20  rvitelli
//    cpu mask
//
//    Revision 1.1  2008/06/13 15:41:46  rvitelli
//    First implementation: buffer writer test module.
//

#include <linux/delay.h>
#include <linux/module.h>
#include <linux/time.h>

#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_sem.h>

u32 *buffers[4];
u32 buff1[34];
u32 buff2[34];
u32 buff3[34];
u32 buff4[34];

RT_TASK bufftask;
volatile int keepAlive;
RTIME last_time;

int rt_cpu_mask = 2;
module_param(rt_cpu_mask, int, 0);
MODULE_PARM_DESC(rt_cpu_mask, "CPUS where RT tasks are allowed to run");

static void bufferTask(long data){
	static int index_counter = 0;
	static int current_buffer = 0;
	int i = 0;
	last_time = rt_get_time_ns();
	
	
	while (keepAlive){
		for (i=1; i<33; i++){
			buffers[current_buffer][i] = index_counter + i;
		}
		buffers[current_buffer][0] = index_counter;
		buffers[current_buffer][33] = index_counter;
		
		if ((index_counter % 100000) == 0) {
			int j;
			
			rt_printk("mod_writer: time since last sched = %d\n", rt_get_time_ns() - last_time);
			rt_printk("mod_writer: current_buffer = %d\n", current_buffer);
			rt_printk("mod_writer: index_counter = %d\n", index_counter);
			/*rt_printk("mod_writer: current buffer dump:\n");
			for (j=1; j<33; j++){
				rt_printk("[%d] ",buffers[current_buffer][j]);
			}
			rt_printk("\n");*/
		}
		last_time = rt_get_time_ns();
		index_counter++;
		current_buffer++;
		if (current_buffer == 4) current_buffer = 0;
		
		//rt_task_wait_period();
		rt_sleep(nano2count(50E3));
	}
	keepAlive = 1;
}


long long int GetBufferAddress(int buff_num){
	return (long long int)(buffers[buff_num]);
}

static int mod_writer_init(void) {
	int _ret, i;
	RTIME tick_period;
	
printk("The cpu mask is = %d\n", rt_cpu_mask);

	// Init buffers
	buffers[0] = buff1;
	buffers[1] = buff2;
	buffers[2] = buff3;
	buffers[3] = buff4;
	for (i=0; i<4; i++){
		rt_printk ("mod_writer: buffer %d @ %p\n", i, buffers[i]);
	}
	
	// init main RT task
	keepAlive = 1;
	_ret = rt_task_init(&bufftask, bufferTask, 0, 4096, 0, 0, NULL);	
	if (_ret != 0) {
		rt_printk("mod_writer: could not start buffer task. Aborting.\n");
		return _ret;
	}
	rt_set_runnable_on_cpus(&bufftask, rt_cpu_mask);
	
	rt_set_oneshot_mode();
	
	if (!rt_is_hard_timer_running()) {
		rt_printk("mod_writer: starting RT timer\n");
		start_rt_timer(0);
	}
	
	//tick_period = nano2count(50E3);
	//rt_task_make_periodic(&bufftask,rt_get_time()+tick_period,tick_period);
	
	rt_task_resume(&bufftask);
	
	return 0;
}

static void mod_writer_exit(void) {
	keepAlive = 0;
	//rt_sleep(nano2count(1000));
	while (keepAlive == 0) {udelay(1000);};
	printk("mod_writer: bufferTask killed\n");
	rt_task_delete(&bufftask);
	
	return;
}

module_init(mod_writer_init);
module_exit(mod_writer_exit);

EXPORT_SYMBOL(GetBufferAddress);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Buffer Writer Test Module");
MODULE_AUTHOR("Riccardo Vitelli");
