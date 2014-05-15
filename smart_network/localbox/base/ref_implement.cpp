#include "ref_implement.h"

RefImplement::RefImplement(int ref_count)
    : ref_count_(ref_count) {
    // nothing
}

int RefImplement::AddRef(void) const {
    return ++ref_count_;
}

int RefImplement::Release(void) const {
    if (--ref_count_ == 0) {
        Finalize();
    }
    return ref_count_;
}

void RefImplement::Finalize(void) const {
    delete this;
}

int RefImplement::ref_count(void) const {
    return ref_count_;
}