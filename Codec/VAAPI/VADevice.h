//
// VADevice.h
//
// Library: Common
// Package: Codec
// Module:  VAAPI
// 

#pragma once

#include <set>
#include <mutex>
#include <vector>
#include <memory>

#include "VACommon.h"

namespace Mmp
{
namespace Codec
{

class VADevice final
{
public:
    using ptr = std::shared_ptr<VADevice>;
public:
    static VADevice::ptr Instance();
public:
    VADevice();
    ~VADevice();
public:
    bool Init();
    void Uninit();
    bool Create();
    void Destroy();
public:
    VADisplay GetDisplay();
    std::set<VAProfile> GetSupportProfiles();
    bool GetImageFormat(PixelFormat format, VAImageFormat& imageFormat);
public:
    void RegisterNoticeCenter();
    void UnRegisterNoticeCenter();
private:
    bool OpenDrmDevice();
    void CloseDrmDeivce();
    void QueryDeviceCompability();
private:
    std::mutex  _mtx;
private:
    uint32_t    _reference;
private: /* compability */
    int _versionMajor;
    int _versionMinor;
    std::string _vendor;
    std::vector<VAImageFormat> _imageFormats;
private:
    bool _holdDrm;
    std::string _renderNode;
    int _drmFd;
private:
    VADisplay _display; 
private:
    std::set<VAProfile> _supportProfiles;
};

} // namespace Codec
} // namespace Mmp