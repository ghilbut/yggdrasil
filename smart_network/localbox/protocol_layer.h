#ifndef PROTOCOL_LAYER_H_
#define PROTOCOL_LAYER_H_


class ProtocolLayer {
public:
    ProtocolLayer* BindUpperLayer(ProtocolLayer* layer) {
        ProtocolLayer* old = upper_layer_;
        upper_layer_ = layer;
        return old;
    }

    ProtocolLayer* BindLowerLayer(ProtocolLayer* layer) {
        ProtocolLayer* old = lower_layer_;
        lower_layer_ = layer;
        return old;
    }


    void FireRequest() {
        if (lower_layer_) {
            lower_layer_->OnRequest();
        }
    }
    void FireResponse() {
        if (upper_layer_) {
            upper_layer_->OnResponse();
        }
    }
    void FireNotify() {
        if (lower_layer_) {
            lower_layer_->OnNotify();
        }
    }
    void FireEvent() {
        if (upper_layer_) {
            upper_layer_->OnEvent();
        }
    }
    

    virtual void OnRequest() = 0;
    virtual void OnResponse() = 0;
    virtual void OnNotify() = 0;
    virtual void OnEvent() = 0;


protected:
    ProtocolLayer(void) : upper_layer_(0), lower_layer_(0) {}
    virtual ~ProtocolLayer(void) {}

private:
    ProtocolLayer* upper_layer_;
    ProtocolLayer* lower_layer_;
};

#endif  // PROTOCOL_LAYER_H_