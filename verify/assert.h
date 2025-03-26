/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef VERIFIER_ASSERT_H
#define VERIFIER_ASSERT_H

#if defined(VSYNC_VERIFICATION)

/* ensure assert is undefined */
#undef assert

/* Dartagnan implements this function */
void __VERIFIER_assert(int);

/* Replace assert with Dartagnan's assertion */
#define assert(COND) __VERIFIER_assert(COND)
#endif

#endif /* VERIFIER_ASSERT_H */
