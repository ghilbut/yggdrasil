#ifndef BASE_REF_OBJECT_H_
#define BASE_REF_OBJECT_H_

template<typename T>
class RefObject {
public:
    RefObject(T* t = 0)
        : t_(t) {
        if (t_) {
            t_->AddRef();
        }
    }

    RefObject(RefObject<T>& other)
        : t_(other.t_) {
        if (t_) {
            t_->AddRef();
        }
    }

    virtual ~RefObject(void) {
        if (t_) {
            t_->Release();
        }
    }

    void Reset(T* t) {
        if (t) {
            t->AddRef();
        }
        if (t_) {
            t_->Release();
        }
        t_ = t;
    }

    RefObject& operator= (RefObject<T>& other) {
        Reset(other.t_);
        return *this;
    }

    bool operator== (RefObject<T>& other) {
        return t_ == other.t_;
    }

    bool operator!= (RefObject<T>& other) {
        return !(*this == other);
    }

    T* operator-> (void) {
        return t_;
    }

protected:
    T* t_;
};

#endif  // BASE_REF_OBJECT_H_