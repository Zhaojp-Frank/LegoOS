/*
 * Copyright (c) 2016-2017 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _LEGO_PROCESSOR_PCACHE_TYPES_H_
#define _LEGO_PROCESSOR_PCACHE_TYPES_H_

#include <lego/const.h>
#include <lego/bitops.h>
#include <lego/spinlock.h>

struct pcache_meta;

#define PCACHE_LINE_SIZE_SHIFT		(CONFIG_PCACHE_LINE_SIZE_SHIFT)
#define PCACHE_ASSOCIATIVITY_SHIFT	(CONFIG_PCACHE_ASSOCIATIVITY_SHIFT)

#define PCACHE_LINE_SIZE		(_AC(1,UL) << PCACHE_LINE_SIZE_SHIFT)
#define PCACHE_LINE_MASK		(~(PCACHE_LINE_SIZE-1))
#define PCACHE_ASSOCIATIVITY		(_AC(1,UL) << PCACHE_ASSOCIATIVITY_SHIFT)
#define PCACHE_META_SIZE		(sizeof(struct pcache_meta))

enum pcache_set_stat_item {
	NR_PSET_FILL,
	NR_PSET_EVICTIONS,
	NR_PSET_STAT_ITEMS
};

/**
 * struct pcache_set	- Metadata for each cache set
 * @lock: protect (de-)allocation of all ways within this set
 *        protect rmap operations against all ways within this set
 *
 * FAT NOTE:
 * If you add anything here, do not forget to check if this
 * new field needs to be initialized in init_pcache_set_map().
 */
struct pcache_set {
	spinlock_t		lock;
	atomic_t		stat[NR_PSET_STAT_ITEMS];
};

/**
 * struct pcache_meta	- Metadata about one pcache line
 * @bits: various state bits (see below)
 * @rmap: reverse mapping info
 * @mapcount: count of ptes mapped to this pcm
 *
 * You can think this structure as the traditional metadata
 * part for a cache line, but with some addtional fields. And
 * this structure is *CPU cacheline size* aligned to minimize
 * CPU cacheline pingpong between different cores.
 *
 * FAT NOTE:
 * If you add anything here, do not forget to check if this
 * new field needs to be initialized in init_pcache_meta_map().
 */
struct pcache_meta {
	u8			bits;
	struct list_head	rmap;
	atomic_t		mapcount;
} ____cacheline_aligned;

struct pcache_rmap {
	pte_t			*page_table;
	struct task_struct	*owner;
	unsigned long		address;
	struct list_head	next;
};

/*
 * pcacheline->bits
 *
 * PC_locked:		Pcacheline is locked. DO NOT TOUCH.
 * PC_allocated:	Pcacheline is allocated, but may not be valid.
 * PC_valid:		Pcacheline has a valid mapping and content.
 * PC_dirty:		Pcacheline is dirty
 * PC_writeback:	Pcacheline is being writtern back to memory
 */
enum pcache_meta_bits {
	PC_locked,
	PC_allocated,
	PC_valid,
	PC_dirty,
	PC_writeback,

	__NR_PCLBITS,
};

#define TEST_PC_BITS(uname, lname)				\
static inline int Pcache##uname(const struct pcache_meta *p)	\
{								\
	return test_bit(PC_##lname, (void *)&p->bits);		\
}

#define SET_PC_BITS(uname, lname)				\
static inline void SetPcache##uname(struct pcache_meta *p)	\
{								\
	set_bit(PC_##lname, (void *)&p->bits);			\
}

#define CLEAR_PC_BITS(uname, lname)				\
static inline void ClearPcache##uname(struct pcache_meta *p)	\
{								\
	clear_bit(PC_##lname, (void *)&p->bits);		\
}

#define __SET_PC_BITS(uname, lname)				\
static inline void __SetPcache##uname(struct pcache_meta *p)	\
{								\
	__set_bit(PC_##lname, (void *)&p->bits);		\
}

#define __CLEAR_PC_BITS(uname, lname)				\
static inline void __ClearPcache##uname(struct pcache_meta *p)	\
{								\
	__clear_bit(PC_##lname, (void *)&p->bits);		\
}

#define TEST_SET_BITS(uname, lname)				\
static inline int TestSetPcache##uname(struct pcache_meta *p)	\
{								\
	return test_and_set_bit(PC_##lname, (void *)&p->bits);	\
}

#define TEST_CLEAR_BITS(uname, lname)				\
static inline int TestClearPcache##uname(struct pcache_meta *p)	\
{								\
	return test_and_clear_bit(PC_##lname, (void *)&p->bits);\
}

#define __TEST_SET_BITS(uname, lname)				\
static inline int __TestSetPcache##uname(struct pcache_meta *p)	\
{								\
	return __test_and_set_bit(PC_##lname, (void *)&p->bits);\
}

#define __TEST_CLEAR_BITS(uname, lname)				\
static inline int __TestClearPcache##uname(struct pcache_meta *p)\
{								\
	return __test_and_clear_bit(PC_##lname, (void *)&p->bits);\
}

#define PCACHE_META_BITS(uname, lname)				\
	TEST_PC_BITS(uname, lname)				\
	SET_PC_BITS(uname, lname)				\
	CLEAR_PC_BITS(uname, lname)				\
	__SET_PC_BITS(uname, lname)				\
	__CLEAR_PC_BITS(uname, lname)				\
	TEST_SET_BITS(uname, lname)				\
	TEST_CLEAR_BITS(uname, lname)				\
	__TEST_SET_BITS(uname, lname)				\
	__TEST_CLEAR_BITS(uname, lname)

PCACHE_META_BITS(Locked, locked)
PCACHE_META_BITS(Allocated, allocated)
PCACHE_META_BITS(Valid, valid)
PCACHE_META_BITS(Dirty, dirty)
PCACHE_META_BITS(Writeback, writeback)

#endif /* _LEGO_PROCESSOR_PCACHE_TYPES_H_ */
