#ifndef CHANNEL_REF_H_
#define CHANNEL_REF_H_

//class Device;
class Channel;

class ChannelRef {
public:
    ChannelRef(void);
    ChannelRef(const ChannelRef& other);
    //explicit ChannelRef(Device& device);
    ~ChannelRef(void);

    ChannelRef& operator= (const ChannelRef& other);
    bool operator== (const ChannelRef& other) const;
    bool operator!= (const ChannelRef& other) const;

    Channel& operator* (void) const;
    Channel* operator-> (void) const;

    Channel* Get(void) const;
    void Reset(Channel* channel);
    bool IsNull(void) const;

private:
    Channel* impl_;
};

#endif  // CHANNEL_REF_H_