#ifndef BASE_REFERENCE_H_
#define BASE_REFERENCE_H_

template<typename T>
class Reference {
public:
    explicit Reference(T* t = 0)
        : t_(t) {
        if (t_) {
            t_->AddRef();
        }
    }

    Reference(Reference<T>& other)
        : t_(other.t_) {
        if (t_) {
            t_->AddRef();
        }
    }

    virtual ~Reference(void) {
        if (t_) {
            t_->Release();
        }
    }

    Reference& operator= (Reference<T>& other) {
        Reset(other.t_);
        return *this;
    }

    bool operator== (Reference<T>& other) {
        return t_ == other.t_;
    }

    bool operator!= (Reference<T>& other) {
        return !(*this == other);
    }

    T& operator* (void) {
        return *t_;
    }

    T* operator-> (void) {
        return t_;
    }

    bool IsNull(void) const {
        return (t_ == 0);
    }

    T* Get(void) const {
        return t_;
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

protected:
    T* t_;
};



template<typename T>
class Weak {
public:
    explicit Weak(const Reference<T>& r)
        : t_(r.Get()) {
        if (t_) {
            t_->AddWeak();
        }
    }

    virtual ~Weak(void) {
        t_->ReleaseWeak();
    }

    Reference<T>& Lock(void) {
        return Reference<T>(t_);
    }


private:
    T* t_;
};

#endif  // BASE_REFERENCE_H_