// Copyright (c) 2023 Afrocoin Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "nontrivial-threadlocal.h"

#include <clang-tidy/ClangTidyModule.h>
#include <clang-tidy/ClangTidyModuleRegistry.h>

class AfrocoinModule final : public clang::tidy::ClangTidyModule
{
public:
    void addCheckFactories(clang::tidy::ClangTidyCheckFactories& CheckFactories) override
    {
        CheckFactories.registerCheck<afrocoin::NonTrivialThreadLocal>("afrocoin-nontrivial-threadlocal");
    }
};

static clang::tidy::ClangTidyModuleRegistry::Add<AfrocoinModule>
    X("afrocoin-module", "Adds afrocoin checks.");

volatile int AfrocoinModuleAnchorSource = 0;
