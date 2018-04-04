/*
 Copyright Disney Enterprises, Inc.  All rights reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License
 and the following modification to it: Section 6 Trademarks.
 deleted and replaced with:

 6. Trademarks. This License does not grant permission to use the
 trade names, trademarks, service marks, or product names of the
 Licensor and its affiliates, except as required for reproducing
 the content of the NOTICE file.

 You may obtain a copy of the License at
 http://www.apache.org/licenses/LICENSE-2.0
*/
#pragma once

#include "ExprConfig.h"

#ifdef SEEXPR_ENABLE_LLVM
#include <llvm/IR/IRBuilder.h>
namespace llvm {
class Value;
class Type;
class Module;
class Function;
}
typedef llvm::Value* LLVM_VALUE;
typedef llvm::IRBuilder<>& LLVM_BUILDER;
#define LLVM_BODY const
#else
typedef double LLVM_VALUE;
typedef double LLVM_BUILDER;
#define LLVM_BODY \
    {             \
        return 0; \
    }
#endif
