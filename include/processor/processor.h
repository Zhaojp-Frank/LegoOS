/*
 * Copyright (c) 2016-2017 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _LEGO_PROCESSOR_PROCESSOR_H_
#define _LEGO_PROCESSOR_PROCESSOR_H_

#include <lego/sched.h>
#include <lego/signal.h>
#include <generated/unistd_64.h>
#include <lego/comp_common.h>	/* must come at last */

#ifdef CONFIG_COMP_PROCESSOR
void __init processor_manager_early_init(void);
void __init processor_manager_init(void);
int __init pcache_range_register(u64 start, u64 size);

int pcache_handle_fault(struct mm_struct *mm,
			unsigned long address, unsigned long flags);

#ifdef CONFIG_CHECKPOINT
int checkpoint_thread(struct task_struct *);
#else
static inline int checkpoint_thread(struct task_struct *tsk) { return 0; }
#endif /* CONFIG_CHECKPOINT */

int do_execve(const char *filename,
	      const char * const *argv,
	      const char * const *envp);

void open_stdio_files(void);

#else
/*
 * !CONFIG_COMP_PROCESSOR
 * Provide some empty function prototypes.
 */

static inline void processor_manager_init(void) { }
static inline void processor_manager_early_init(void) { }
static inline int pcache_range_register(u64 start, u64 size)
{
	return 0;
}

static inline int
pcache_handle_fault(struct mm_struct *mm, unsigned long address, unsigned long flags)
{
	BUG();
}

static inline int checkpoint_thread(struct task_struct *tsk)
{
	BUG();
}

static inline int do_execve(const char *filename, const char * const *argv,
			    const char * const *envp)
{
	BUG();
}
#endif /* CONFIG_COMP_PROCESSOR */

#endif /* _LEGO_PROCESSOR_PROCESSOR_H_ */