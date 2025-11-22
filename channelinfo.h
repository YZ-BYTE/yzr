#ifndef CHANNELINFO_H
#define CHANNELINFO_H

#include <QString>
#include <QList>

// 频道信息结构
struct ChannelInfo
{
    int id;
    QString name;
    QStringList urls;  // 支持多个URL，如果第一个失败可以尝试下一个
    
    ChannelInfo() : id(0) {}
    ChannelInfo(int i, const QString& n, const QStringList& u) 
        : id(i), name(n), urls(u) {}
};

// 频道管理器
class ChannelManager
{
public:
    static ChannelManager& instance();
    
    // 初始化频道列表
    void initChannels();
    
    // 获取所有频道
    QList<ChannelInfo> getChannels() const { return m_channels; }
    
    // 根据ID获取频道
    ChannelInfo getChannel(int id) const;
    
    // 根据名称查找频道
    ChannelInfo findChannel(const QString& name) const;

private:
    ChannelManager() { initChannels(); }
    QList<ChannelInfo> m_channels;
};

#endif // CHANNELINFO_H


























