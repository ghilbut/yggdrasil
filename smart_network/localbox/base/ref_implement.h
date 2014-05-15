#ifndef BASE_REF_IMPLEMENT_H_
#define BASE_REF_IMPLEMENT_H_

#include <boost/atomic.hpp>


class RefImplement {
protected:
    explicit RefImplement(int ref_count = 0);
    virtual ~RefImplement(void) {}

public:
    virtual int AddRef(void) const;
    virtual int Release(void) const;
    virtual void Finalize(void) const;

    int ref_count(void) const;


private:
    mutable boost::atomic_int ref_count_;
};

#endif  // BASE_REF_IMPLEMENT_H_