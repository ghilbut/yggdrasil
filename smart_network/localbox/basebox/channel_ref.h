#ifndef CHANNEL_REF_H_
#define CHANNEL_REF_H_


class DeviceContext;
class Channel;

class ChannelRef {
public:
    ChannelRef(void);
    explicit ChannelRef(DeviceContext& context);
    explicit ChannelRef(const ChannelRef& other);
    ~ChannelRef(void);

    void Reset(Channel* channel);

    ChannelRef& operator= (const ChannelRef& other);
    bool operator== (const ChannelRef& other) const;
    bool operator!= (const ChannelRef& other) const;

    Channel* operator-> (void) const;


private:
    Channel* impl_;
};

#endif  // CHANNEL_REF_H_