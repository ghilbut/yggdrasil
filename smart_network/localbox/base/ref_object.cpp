#include "ref_object.h"

RefObject::RefObject(void)
    : ref_count_(0)
    , weak_count_(1) {
    // nothing
}

void RefObject::AddRef(void) const {
    ++ref_count_;
}

void RefObject::Release(void) const {
    if (--ref_count_ == 0) {
        ReleaseWeak();
    }
}

void RefObject::AddWeak(void) const {
    ++weak_count_;
}

void RefObject::ReleaseWeak(void) const {
    if (--weak_count_ == 0) {
        Finalize();
    }
}

void RefObject::Finalize(void) const {
    delete this;
}

int RefObject::ref_count(void) const {
    return ref_count_;
}
