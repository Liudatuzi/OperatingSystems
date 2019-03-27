#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/sched.h>
#include<linux/unistd.h>
#include<linux/list.h>
#include<linux/uaccess.h>
#include<linux/slab.h>
#include<linux/string.h>
#include"prinfo.h"
MODULE_LICENSE("Dual BSD/GPL");
#define __NR_hellocall 287

static int (*oldcall)(void);


void dfsTree(struct task_struct*t,struct prinfo*buf,int*nr)
{       
        /*copy the information from the kernel*/
	struct task_struct *p;
	struct list_head*pos;
	buf[*nr].state=t->state;
	buf[*nr].pid=t->pid;
	strcpy(buf[*nr].comm,t->comm);
	buf[*nr].uid=t->cred->uid;
	buf[*nr].parent_pid=t->parent->pid;
	if(list_empty(&t->children)){
		buf[*nr].first_child_pid=0;
	}
	else{
		buf[*nr].first_child_pid=list_entry((&t->children)->next,struct task_struct,sibling)->pid;
	}
	if (list_empty(&t->sibling)){
		buf[*nr].next_sibling_pid = 0;
	}
	else{
		struct list_head *list;
		struct task_struct *task_tmp;
		list_for_each(list, &(t->parent->children)) {
			task_tmp = list_entry(list,struct task_struct,sibling);
			if (task_tmp->pid == t->pid) {
				buf[*nr].next_sibling_pid=list_entry(list->next,struct task_struct,sibling)->pid;
				break;
			}
		}
	}
	*nr=*nr+1;
	list_for_each(pos, &t->children) {
		p=list_entry(pos,struct task_struct,sibling);
		dfsTree(p, buf, nr);
  }
}

static int pstree(struct prinfo*buf, int *nr)
{
	struct prinfo *tmpBuf;
	int *tmpNr;
	tmpBuf=kmalloc_array(1000,sizeof(*buf),GFP_KERNEL);
	tmpNr=kmalloc(sizeof(int),GFP_KERNEL);
	*tmpNr=0;
	struct task_struct * task=&init_task;
	read_lock(&tasklist_lock);
	dfsTree(task,tmpBuf,tmpNr);
	read_unlock(&tasklist_lock);
	copy_to_user(buf,tmpBuf,1000*sizeof(*tmpBuf));
	copy_to_user(nr,tmpNr,sizeof(int));
	kfree(tmpNr);
	kfree(tmpBuf);
	return *nr;
}
static int addsyscall_init(void)
{
	long*syscall=(long*)0xc000d8c4;
	oldcall=(int(*)(void))(syscall[__NR_hellocall]);
	syscall[__NR_hellocall]=(unsigned long)pstree;
	printk(KERN_INFO "module load!\n");
	return 0;
}

static void addsyscall_exit(void)
{
long *syscall=(long*)0xc000d8c4;
syscall[__NR_hellocall]=(unsigned long) oldcall;
printk(KERN_INFO "mldule exit!\n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);

