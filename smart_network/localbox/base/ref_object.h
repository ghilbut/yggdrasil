#ifndef BASE_REF_OBJECT_H_
#define BASE_REF_OBJECT_H_

#include <boost/atomic.hpp>


class RefObject {
protected:
    explicit RefObject(void);
    virtual ~RefObject(void) {}

public:
    virtual void AddRef(void) const;
    virtual void Release(void) const;
    virtual void AddWeak(void) const;
    virtual void ReleaseWeak(void) const;
    virtual void Finalize(void) const;

    int ref_count(void) const;


private:
    mutable boost::atomic_int ref_count_;
    mutable boost::atomic_int weak_count_;
};

#endif  // BASE_REF_OBJECT_H_