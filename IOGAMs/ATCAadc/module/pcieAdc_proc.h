//******************************************************************************
//    $Log: pcieAdc_proc.h,v $
//    Revision 1.2  2008/06/09 15:12:07  aneto
//    Minor mods
//
//    Revision 1.1  2008/06/09 14:35:15  rvitelli
//    Moved proc interface to pcieAdc_proc.*
//
//******************************************************************************

#ifndef PCIEADC_PROC_H_
#define PCIEADC_PROC_H_

int pcieAdc_procinfo(char *buf, char **start, off_t fpos, int length, int *eof, void *data);
int pcieAdc_proccmd(struct file *file, const char __user *buffer, unsigned long count, void *data);
void register_proc(void);
void unregister_proc(void);

#endif /*PCIEADC_PROC_H_*/
