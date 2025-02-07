/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_ATOMICPTR_MARKABLE_H
#define VSYNC_ATOMICPTR_MARKABLE_H
/*******************************************************************************
 * @file  atomicptr_markable.h
 * @brief C implementation of AtomicMarkableReference in Java
 * @pre Addresses must be 4-byte-aligned in order to use vatomicptr_markable_t.
 *
 * Extends `vatomicptr_t` to `vatomicptr_markable_t`.
 * The LSB of the address is utilized as a mark.
 *
 * @cite [AtomicMarkableReference](https://bit.ly/2FqwH8u)
 ******************************************************************************/
/* !!!Warning: File generated by tmpl; DO NOT EDIT.!!! */
#include <vsync/common/assert.h>
#include <vsync/atomic.h>
#include <vsync/vtypes.h>

typedef vatomicptr_t vatomicptr_markable_t;

#define vatomicptr_markable(_type_) vatomicptr_markable_t

/** @cond DO_NOT_DOCUMENT */

#define V_ATOMICPTR_MARKABLE_EXTRACT_ADDRESS (VUINTPTR_MAX - (vuintptr_t)0x3)
#define V_ATOMICPTR_MARKABLE_EXTRACT_MARK	 ((vuintptr_t)0x1)

/**
 * Extracts the address without the mark from `_a_`
 *
 */
#define V_ATOMICPTR_MARKABLE_GET_ADDRESS(_a_)                                  \
	((void *)(((vuintptr_t)(_a_)) & V_ATOMICPTR_MARKABLE_EXTRACT_ADDRESS))

/**
 * Extracts the mark without the address from `_a_`
 *
 */
#define V_ATOMICPTR_MARKABLE_GET_MARK(_a_)                                     \
	((vbool_t)(((vuintptr_t)(_a_)) & V_ATOMICPTR_MARKABLE_EXTRACT_MARK))

/**
 *  Combines address `_a_` with mark `_m_`
 *
 */
#define V_ATOMICPTR_MARKABLE_COMBINE_ADDRESS_MARK(_a_, _m_)                    \
	((void *)(vuintptr_t)(((vuintptr_t)(_a_)) | ((vuintptr_t)(_m_))))

/** @endcond */

/**
 * Evaluates to `true`, if the given pointer `_ptr_` is 4-byte-aligned.
 */
#define V_ATOMICPTR_MARKABLE_IS_ALIGNED(_ptr_)                                 \
	((((vuintptr_t)(_ptr_)) & ((vuintptr_t)0x3)) == 0)

/**
 * Compares and exchanges the address and mark value in `a`.
 *
 * @param a address of vatomicptr_markable_t object.
 * @param e_v expected address value.
 * @param e_m expected mark value.
 * @param v new address value to set.
 * @param m new mark value to set.
 * @return true address and mark were set.
 * @return false address and mark were not updated, because either the current
 * address value in `a` != `e_v`, or the current mark value in `a` != `e_m`.
 */
static inline vbool_t
vatomicptr_markable_cmpxchg(vatomicptr_markable_t *a, void *e_v, vbool_t e_m,
							void *v, vbool_t m)
{
	void *val = NULL;
	void *e	  = NULL;
	void *ret = NULL;
	ASSERT(V_ATOMICPTR_MARKABLE_IS_ALIGNED(e_v));
	ASSERT(V_ATOMICPTR_MARKABLE_IS_ALIGNED(v));
	e	= V_ATOMICPTR_MARKABLE_COMBINE_ADDRESS_MARK(e_v, e_m);
	val = V_ATOMICPTR_MARKABLE_COMBINE_ADDRESS_MARK(v, m);
	ret = vatomicptr_cmpxchg(a, e, val);
	return ret == e;
}
/**
 * Compares and exchanges the address and mark value in `a`.
 *
 * @param a address of vatomicptr_markable_t object.
 * @param e_v expected address value.
 * @param e_m expected mark value.
 * @param v new address value to set.
 * @param m new mark value to set.
 * @return true address and mark were set.
 * @return false address and mark were not updated, because either the current
 * address value in `a` != `e_v`, or the current mark value in `a` != `e_m`.
 */
static inline vbool_t
vatomicptr_markable_cmpxchg_acq(vatomicptr_markable_t *a, void *e_v,
								vbool_t e_m, void *v, vbool_t m)
{
	void *val = NULL;
	void *e	  = NULL;
	void *ret = NULL;
	ASSERT(V_ATOMICPTR_MARKABLE_IS_ALIGNED(e_v));
	ASSERT(V_ATOMICPTR_MARKABLE_IS_ALIGNED(v));
	e	= V_ATOMICPTR_MARKABLE_COMBINE_ADDRESS_MARK(e_v, e_m);
	val = V_ATOMICPTR_MARKABLE_COMBINE_ADDRESS_MARK(v, m);
	ret = vatomicptr_cmpxchg_acq(a, e, val);
	return ret == e;
}
/**
 * Compares and exchanges the address and mark value in `a`.
 *
 * @param a address of vatomicptr_markable_t object.
 * @param e_v expected address value.
 * @param e_m expected mark value.
 * @param v new address value to set.
 * @param m new mark value to set.
 * @return true address and mark were set.
 * @return false address and mark were not updated, because either the current
 * address value in `a` != `e_v`, or the current mark value in `a` != `e_m`.
 */
static inline vbool_t
vatomicptr_markable_cmpxchg_rel(vatomicptr_markable_t *a, void *e_v,
								vbool_t e_m, void *v, vbool_t m)
{
	void *val = NULL;
	void *e	  = NULL;
	void *ret = NULL;
	ASSERT(V_ATOMICPTR_MARKABLE_IS_ALIGNED(e_v));
	ASSERT(V_ATOMICPTR_MARKABLE_IS_ALIGNED(v));
	e	= V_ATOMICPTR_MARKABLE_COMBINE_ADDRESS_MARK(e_v, e_m);
	val = V_ATOMICPTR_MARKABLE_COMBINE_ADDRESS_MARK(v, m);
	ret = vatomicptr_cmpxchg_rel(a, e, val);
	return ret == e;
}
/**
 * Compares and exchanges the address and mark value in `a`.
 *
 * @param a address of vatomicptr_markable_t object.
 * @param e_v expected address value.
 * @param e_m expected mark value.
 * @param v new address value to set.
 * @param m new mark value to set.
 * @return true address and mark were set.
 * @return false address and mark were not updated, because either the current
 * address value in `a` != `e_v`, or the current mark value in `a` != `e_m`.
 */
static inline vbool_t
vatomicptr_markable_cmpxchg_rlx(vatomicptr_markable_t *a, void *e_v,
								vbool_t e_m, void *v, vbool_t m)
{
	void *val = NULL;
	void *e	  = NULL;
	void *ret = NULL;
	ASSERT(V_ATOMICPTR_MARKABLE_IS_ALIGNED(e_v));
	ASSERT(V_ATOMICPTR_MARKABLE_IS_ALIGNED(v));
	e	= V_ATOMICPTR_MARKABLE_COMBINE_ADDRESS_MARK(e_v, e_m);
	val = V_ATOMICPTR_MARKABLE_COMBINE_ADDRESS_MARK(v, m);
	ret = vatomicptr_cmpxchg_rlx(a, e, val);
	return ret == e;
}
/**
 * Attemps to update the mark in `a`.
 *
 * @param a address of vatomicptr_markable_t object.
 * @param e_v expected address value.
 * @param m new mark value.
 * @return true mark was set to `m`.
 * @return false mark is not set, because current address value in `a` != `e_v`.
 */
static inline vbool_t
vatomicptr_markable_attempt_mark(vatomicptr_markable_t *a, void *e_v, vbool_t m)
{
	void *new_val = NULL;
	void *ret	  = NULL;
	void *old_val = vatomicptr_read(a);
	ASSERT(V_ATOMICPTR_MARKABLE_IS_ALIGNED(e_v));
	if (V_ATOMICPTR_MARKABLE_GET_ADDRESS(old_val) != e_v) {
		return false;
	}
	new_val = V_ATOMICPTR_MARKABLE_COMBINE_ADDRESS_MARK(e_v, m);
	ret		= vatomicptr_cmpxchg(a, old_val, new_val);
	return ret == old_val;
}
/**
 * Attemps to update the mark in `a`.
 *
 * @param a address of vatomicptr_markable_t object.
 * @param e_v expected address value.
 * @param m new mark value.
 * @return true mark was set to `m`.
 * @return false mark is not set, because current address value in `a` != `e_v`.
 */
static inline vbool_t
vatomicptr_markable_attempt_mark_acq(vatomicptr_markable_t *a, void *e_v,
									 vbool_t m)
{
	void *new_val = NULL;
	void *ret	  = NULL;
	void *old_val = vatomicptr_read(a);
	ASSERT(V_ATOMICPTR_MARKABLE_IS_ALIGNED(e_v));
	if (V_ATOMICPTR_MARKABLE_GET_ADDRESS(old_val) != e_v) {
		return false;
	}
	new_val = V_ATOMICPTR_MARKABLE_COMBINE_ADDRESS_MARK(e_v, m);
	ret		= vatomicptr_cmpxchg_acq(a, old_val, new_val);
	return ret == old_val;
}
/**
 * Attemps to update the mark in `a`.
 *
 * @param a address of vatomicptr_markable_t object.
 * @param e_v expected address value.
 * @param m new mark value.
 * @return true mark was set to `m`.
 * @return false mark is not set, because current address value in `a` != `e_v`.
 */
static inline vbool_t
vatomicptr_markable_attempt_mark_rel(vatomicptr_markable_t *a, void *e_v,
									 vbool_t m)
{
	void *new_val = NULL;
	void *ret	  = NULL;
	void *old_val = vatomicptr_read(a);
	ASSERT(V_ATOMICPTR_MARKABLE_IS_ALIGNED(e_v));
	if (V_ATOMICPTR_MARKABLE_GET_ADDRESS(old_val) != e_v) {
		return false;
	}
	new_val = V_ATOMICPTR_MARKABLE_COMBINE_ADDRESS_MARK(e_v, m);
	ret		= vatomicptr_cmpxchg_rel(a, old_val, new_val);
	return ret == old_val;
}
/**
 * Attemps to update the mark in `a`.
 *
 * @param a address of vatomicptr_markable_t object.
 * @param e_v expected address value.
 * @param m new mark value.
 * @return true mark was set to `m`.
 * @return false mark is not set, because current address value in `a` != `e_v`.
 */
static inline vbool_t
vatomicptr_markable_attempt_mark_rlx(vatomicptr_markable_t *a, void *e_v,
									 vbool_t m)
{
	void *new_val = NULL;
	void *ret	  = NULL;
	void *old_val = vatomicptr_read(a);
	ASSERT(V_ATOMICPTR_MARKABLE_IS_ALIGNED(e_v));
	if (V_ATOMICPTR_MARKABLE_GET_ADDRESS(old_val) != e_v) {
		return false;
	}
	new_val = V_ATOMICPTR_MARKABLE_COMBINE_ADDRESS_MARK(e_v, m);
	ret		= vatomicptr_cmpxchg_rlx(a, old_val, new_val);
	return ret == old_val;
}
/**
 * Sets the address and mark of a to the given `v` and `m`.
 *
 * @param a address of vatomicptr_markable_t object.
 * @param v address value to set.
 * @param m mark value to set.
 */
static inline void
vatomicptr_markable_set(vatomicptr_markable_t *a, void *v, vbool_t m)
{
	ASSERT(V_ATOMICPTR_MARKABLE_IS_ALIGNED(v));
	void *n = V_ATOMICPTR_MARKABLE_COMBINE_ADDRESS_MARK(v, m);
	vatomicptr_write(a, n);
}
/**
 * Sets the address and mark of a to the given `v` and `m`.
 *
 * @param a address of vatomicptr_markable_t object.
 * @param v address value to set.
 * @param m mark value to set.
 */
static inline void
vatomicptr_markable_set_rel(vatomicptr_markable_t *a, void *v, vbool_t m)
{
	ASSERT(V_ATOMICPTR_MARKABLE_IS_ALIGNED(v));
	void *n = V_ATOMICPTR_MARKABLE_COMBINE_ADDRESS_MARK(v, m);
	vatomicptr_write_rel(a, n);
}
/**
 * Sets the address and mark of a to the given `v` and `m`.
 *
 * @param a address of vatomicptr_markable_t object.
 * @param v address value to set.
 * @param m mark value to set.
 */
static inline void
vatomicptr_markable_set_rlx(vatomicptr_markable_t *a, void *v, vbool_t m)
{
	ASSERT(V_ATOMICPTR_MARKABLE_IS_ALIGNED(v));
	void *n = V_ATOMICPTR_MARKABLE_COMBINE_ADDRESS_MARK(v, m);
	vatomicptr_write_rlx(a, n);
}
/**
 * Returns the address value of a.
 *
 * @param a address of vatomicptr_markable_t object.
 * @param out_m output param of type vbool_t. The mark value in `a`.
 * @return void* address value in `a`.
 */
static inline void *
vatomicptr_markable_get(vatomicptr_markable_t *a, vbool_t *out_m)
{
	void *ret = vatomicptr_read(a);
	*out_m	  = V_ATOMICPTR_MARKABLE_GET_MARK(ret);
	return V_ATOMICPTR_MARKABLE_GET_ADDRESS(ret);
}
/**
 * Returns the address value of a.
 *
 * @param a address of vatomicptr_markable_t object.
 * @param out_m output param of type vbool_t. The mark value in `a`.
 * @return void* address value in `a`.
 */
static inline void *
vatomicptr_markable_get_acq(vatomicptr_markable_t *a, vbool_t *out_m)
{
	void *ret = vatomicptr_read_acq(a);
	*out_m	  = V_ATOMICPTR_MARKABLE_GET_MARK(ret);
	return V_ATOMICPTR_MARKABLE_GET_ADDRESS(ret);
}
/**
 * Returns the address value of a.
 *
 * @param a address of vatomicptr_markable_t object.
 * @param out_m output param of type vbool_t. The mark value in `a`.
 * @return void* address value in `a`.
 */
static inline void *
vatomicptr_markable_get_rlx(vatomicptr_markable_t *a, vbool_t *out_m)
{
	void *ret = vatomicptr_read_rlx(a);
	*out_m	  = V_ATOMICPTR_MARKABLE_GET_MARK(ret);
	return V_ATOMICPTR_MARKABLE_GET_ADDRESS(ret);
}
/**
 * Returns the address value of `a`.
 *
 * @param a address of vatomicptr_markable_t object.
 * @return void* stored address in `a`.
 */
static inline void *
vatomicptr_markable_get_pointer(vatomicptr_markable_t *a)
{
	vbool_t mark = false;
	return vatomicptr_markable_get(a, &mark);
}
/**
 * Returns the address value of `a`.
 *
 * @param a address of vatomicptr_markable_t object.
 * @return void* stored address in `a`.
 */
static inline void *
vatomicptr_markable_get_pointer_acq(vatomicptr_markable_t *a)
{
	vbool_t mark = false;
	return vatomicptr_markable_get_acq(a, &mark);
}
/**
 * Returns the address value of `a`.
 *
 * @param a address of vatomicptr_markable_t object.
 * @return void* stored address in `a`.
 */
static inline void *
vatomicptr_markable_get_pointer_rlx(vatomicptr_markable_t *a)
{
	vbool_t mark = false;
	return vatomicptr_markable_get_rlx(a, &mark);
}
/**
 * Returns the mark value of `a`.
 *
 * @param a address of vatomicptr_markable_t object.
 * @return true if `a` is marked.
 * @return true if `a` is not marked.
 */
static inline vbool_t
vatomicptr_markable_get_mark(vatomicptr_markable_t *a)
{
	vbool_t mark = false;
	(void)vatomicptr_markable_get(a, &mark);
	return mark;
}
/**
 * Returns the mark value of `a`.
 *
 * @param a address of vatomicptr_markable_t object.
 * @return true if `a` is marked.
 * @return true if `a` is not marked.
 */
static inline vbool_t
vatomicptr_markable_get_mark_acq(vatomicptr_markable_t *a)
{
	vbool_t mark = false;
	(void)vatomicptr_markable_get_acq(a, &mark);
	return mark;
}
/**
 * Returns the mark value of `a`.
 *
 * @param a address of vatomicptr_markable_t object.
 * @return true if `a` is marked.
 * @return true if `a` is not marked.
 */
static inline vbool_t
vatomicptr_markable_get_mark_rlx(vatomicptr_markable_t *a)
{
	vbool_t mark = false;
	(void)vatomicptr_markable_get_rlx(a, &mark);
	return mark;
}

#undef V_ATOMICPTR_MARKABLE_EXTRACT_MARK
#undef V_ATOMICPTR_MARKABLE_GET_MARK
#endif /* VSYNC_ATOMICPTR_MARKABLE_H */
