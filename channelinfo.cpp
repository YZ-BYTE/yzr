#include "channelinfo.h"

ChannelManager& ChannelManager::instance()
{
    static ChannelManager manager;
    return manager;
}

void ChannelManager::initChannels()
{
    m_channels.clear();
    
    // 解析用户提供的频道数据
    m_channels << ChannelInfo(1, "CCTV-1综合", QStringList() 
        << "http://111.40.196.9/PLTV/88888888/224/3221225513/index.m3u8"
        << "https://cctvwbndks.v.kcdnvip.com/cctvwbnd/cctv1_2/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225488/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225769/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225548/index.m3u8");
    
    m_channels << ChannelInfo(2, "CCTV-2财经", QStringList()
        << "https://cctvwbndks.v.kcdnvip.com/cctvwbnd/cctv2_2/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225488/index.m3u8");
    
    m_channels << ChannelInfo(3, "CCTV-3综艺", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225588/index.m3u8");
    
    m_channels << ChannelInfo(4, "CCTV-4中文国际", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225490/index.m3u8");
    
    m_channels << ChannelInfo(5, "CCTV-5体育", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225559/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225550/index.m3u8");
    
    m_channels << ChannelInfo(6, "CCTV-5+体育赛事", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225767/index.m3u8");
    
    m_channels << ChannelInfo(7, "CCTV-6电影", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225561/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225590/index.m3u8");
    
    m_channels << ChannelInfo(8, "CCTV-7国防军事", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225492/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225563/index.m3u8");
    
    m_channels << ChannelInfo(9, "CCTV-8电视剧", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225565/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225592/index.m3u8");
    
    m_channels << ChannelInfo(10, "CCTV-9记录", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225502/index.m3u8");
    
    m_channels << ChannelInfo(11, "CCTV-10科教", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225494/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225567/index.m3u8");
    
    m_channels << ChannelInfo(12, "CCTV-11戏曲", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225569/index.m3u8");
    
    m_channels << ChannelInfo(13, "CCTV-12社会与法", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225498/index.m3u8");
    
    m_channels << ChannelInfo(14, "CCTV-13新闻", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225575/index.m3u8");
    
    m_channels << ChannelInfo(15, "CCTV-14少儿", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225573/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225506/index.m3u8");
    
    m_channels << ChannelInfo(16, "CCTV-15音乐", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225500/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225577/index.m3u8");
    
    m_channels << ChannelInfo(17, "CCTV教育1台", QStringList()
        << "http://txycsbl.centv.cn/zb/0628cetv1.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225576/index.m3u8");
    
    m_channels << ChannelInfo(18, "CCTV教育2台", QStringList()
        << "http://txycsbl.centv.cn/zb/0822cetv2.m3u8");
    
    m_channels << ChannelInfo(19, "北京卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225529/index.m3u8");
    
    m_channels << ChannelInfo(20, "东方卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225512/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225581/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225509/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225507/index.m3u8");
    
    m_channels << ChannelInfo(21, "广东卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225531/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225508/index.m3u8");
    
    m_channels << ChannelInfo(22, "湖北卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225566/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225549/index.m3u8");
    
    m_channels << ChannelInfo(23, "湖南卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225519/index.m3u8");
    
    m_channels << ChannelInfo(24, "江西卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225560/index.m3u8");
    
    m_channels << ChannelInfo(25, "江苏卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225515/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225517/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225578/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225596/index.m3u8");
    
    m_channels << ChannelInfo(26, "山东卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225545/index.m3u8");
    
    m_channels << ChannelInfo(27, "深圳卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225537/index.m3u8");
    
    m_channels << ChannelInfo(28, "天津卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225556/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225597/index.m3u8");
    
    m_channels << ChannelInfo(29, "山西卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225520/index.m3u8");
    
    m_channels << ChannelInfo(30, "重庆卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225579/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225514/index.m3u8");
    
    m_channels << ChannelInfo(31, "甘肃卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225572/index.m3u8");
    
    m_channels << ChannelInfo(32, "内蒙古卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225574/index.m3u8");
    
    m_channels << ChannelInfo(33, "河北卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225564/index.m3u8");
    
    m_channels << ChannelInfo(34, "安徽卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225570/index.m3u8");
    
    m_channels << ChannelInfo(35, "四川卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225595/index.m3u8");
    
    m_channels << ChannelInfo(36, "江西卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225591/index.m3u8");
    
    m_channels << ChannelInfo(37, "优漫卡通", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225586/index.m3u8");
    
    m_channels << ChannelInfo(38, "贵州卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225585/index.m3u8");
    
    m_channels << ChannelInfo(39, "东南卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225583/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225516/index.m3u8");
    
    m_channels << ChannelInfo(40, "陕西卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225582/index.m3u8");
    
    m_channels << ChannelInfo(41, "浙江卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225510/index.m3u8"
        << "http://hw-vl.cztv.com/channels/lantian/channel01/360p.m3u8");
    
    m_channels << ChannelInfo(42, "辽宁卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225558/index.m3u8"
        << "http://111.40.196.9/PLTV/88888888/224/3221225593/index.m3u8");
    
    m_channels << ChannelInfo(43, "吉林卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225589/index.m3u8");
    
    m_channels << ChannelInfo(44, "黑龙江卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225544/index.m3u8");
    
    m_channels << ChannelInfo(45, "广西卫视", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225518/index.m3u8");
    
    m_channels << ChannelInfo(46, "五星体育", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225503/index.m3u8");
    
    m_channels << ChannelInfo(47, "新闻综合", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225499/index.m3u8");
    
    m_channels << ChannelInfo(48, "黑莓电竞", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225606/index.m3u8");
    
    m_channels << ChannelInfo(49, "黑莓动画", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225628/index.m3u8");
    
    m_channels << ChannelInfo(50, "黑莓电影", QStringList()
        << "http://111.40.196.9/PLTV/88888888/224/3221225610/index.m3u8");
}

ChannelInfo ChannelManager::getChannel(int id) const
{
    for (const ChannelInfo& channel : m_channels)
    {
        if (channel.id == id)
            return channel;
    }
    return ChannelInfo();
}

ChannelInfo ChannelManager::findChannel(const QString& name) const
{
    for (const ChannelInfo& channel : m_channels)
    {
        if (channel.name == name)
            return channel;
    }
    return ChannelInfo();
}


























