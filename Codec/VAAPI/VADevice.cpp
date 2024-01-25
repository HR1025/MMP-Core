#include "VADevice.h"

#include "Common/EventTag.h"
#include "Common/NoticeCenter.h"

#include <cstddef>
#include <fcntl.h>
#include <dlfcn.h>
#include <unistd.h>
#include <memory.h>
#include <vector>
#include <cassert>

#include <Poco/Path.h>
#include <Poco/String.h>
#include <Poco/Environment.h>
#include <Poco/SortedDirectoryIterator.h>

#include "VAUtil.h"

namespace Mmp
{
namespace Codec
{

static void VaDeviceLogError(void* /* context */, const char* message)
{
    MMP_MLOG_ERROR("LIBVA") << message;
}

static void VaDeviceLogInfo(void* /* context */, const char* message)
{
    MMP_MLOG_INFO("LIBVA") << message;
}

} // namespace Codec
} // namespace Mmp

namespace Mmp
{
namespace Codec
{

std::vector<std::string> GetRenderNodes()
{
    static std::string kFindPath = "/dev/dri/";
    std::vector<std::string> renderNodes;
    Poco::SortedDirectoryIterator dirIteratorCurrent(kFindPath);
	Poco::SortedDirectoryIterator dirIteratorEnd;

    if (Poco::Environment::has("MMP_VAAPI_DEVICE"))
    {
        renderNodes.push_back(Poco::Environment::get("MMP_VAAPI_DEVICE"));
    }

	while (dirIteratorCurrent != dirIteratorEnd)
	{
        if (Poco::startsWith(Poco::Path(dirIteratorCurrent->path()).getFileName(), std::string("renderD")))
        {
            renderNodes.push_back(dirIteratorCurrent->path());
        }
        ++dirIteratorCurrent;
	}
    dirIteratorCurrent = Poco::SortedDirectoryIterator(kFindPath);
	while (dirIteratorCurrent != dirIteratorEnd)
	{
        if (Poco::startsWith(Poco::Path(dirIteratorCurrent->path()).getFileName(), std::string("card")))
        {
            renderNodes.push_back(dirIteratorCurrent->path());
        }
        ++dirIteratorCurrent;
	}
    return renderNodes;
}

VADevice::ptr VADevice::Instance()
{
    static std::shared_ptr<VADevice> gVADevice = std::make_shared<VADevice>();
    return gVADevice;
}

VADevice::VADevice()
{
    _drmFd = -1;
    _versionMajor = -1;
    _versionMinor = -1;
    _reference = 0;
    _holdDrm = true;
    _display = nullptr;
}

VADevice::~VADevice()
{

}

bool VADevice::Init()
{
    return true;
}

void VADevice::Uninit()
{
}

bool VADevice::Create()
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (_reference > 0)
    {
        _reference++;
        return true;
    }
    if (!OpenDrmDevice())
    {
        VAAPI_LOG_ERROR << "OpenDrmDevice fail";
        goto END;
    }
    _display = vaGetDisplayDRM(_drmFd);
    if (!_display)
    {
        VAAPI_LOG_ERROR << "vaGetDisplayDRM fail, drm fd is: " << _drmFd;
        goto END1;
    }
    {
        vaSetErrorCallback(_display, &VaDeviceLogError, nullptr);
        vaSetInfoCallback(_display, &VaDeviceLogInfo, nullptr);
    }
    if (VA_STATUS_SUCCESS != vaInitialize(_display, &_versionMajor, &_versionMinor))
    {
        VAAPI_LOG_ERROR << "vaInitialize fail";
        goto END2;
    }
    else
    {
        VAAPI_LOG_INFO << "LIBVA API version is: " << _versionMajor << "." << _versionMinor;
    }
    QueryDeviceCompability();
    _reference = 1;
    return true;
/* END3: */
    vaTerminate(_display);
    _versionMajor = -1;
    _versionMinor = -1;
END2:
    _display = nullptr;
END1:
    CloseDrmDeivce();
END:
    return false;
}

void VADevice::Destroy()
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (_reference > 1)
    {
        _reference--;
        return;
    }
    vaTerminate(_display);
    _versionMajor = -1;
    _versionMinor = -1;
    _display = nullptr;
    CloseDrmDeivce();
    _reference = 0;
}

VADisplay VADevice::GetDisplay()
{
    std::lock_guard<std::mutex> lock(_mtx);
    return _display;
}

std::set<VAProfile> VADevice::GetSupportProfiles()
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (_supportProfiles.empty())
    {
        std::vector<VAProfile> supportProfiles;
        int maxProfilesNum = vaMaxNumProfiles(_display);
        supportProfiles.resize(maxProfilesNum);
        if (vaQueryConfigProfiles(_display, supportProfiles.data(), &maxProfilesNum) == VA_STATUS_SUCCESS)
        {
            supportProfiles.resize(maxProfilesNum);
            VAAPI_LOG_INFO << "Support VA profiel:";
            for (const auto& profile : supportProfiles)
            {
                VAAPI_LOG_INFO << "-- " << VAProfileToStr(profile);
            }
        }
        else
        {
            VAAPI_LOG_ERROR << "vaQueryConfigProfiles fail";
            assert(false);
        }
        for (auto& profile: supportProfiles)
        {
            _supportProfiles.insert(profile);
        }
    }
    return _supportProfiles;
}

void VADevice::RegisterNoticeCenter()
{
    std::lock_guard<std::mutex> lock(_mtx);
    NoticeCenter::Instance()->AddListener(Event::kOpenDrmDevice, this, [this](MMP_OPEN_DRM_DEVICE_ARGS)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        if (_drmFd == -1 /* && _renderNode.empty() */)
        {
            _renderNode = deviceName;
            _drmFd = drmFd;
            _holdDrm = false;
        }
    });
    NoticeCenter::Instance()->AddListener(Event::kCloseDrmDevice, this, [this](MMP_CLOSE_DRM_DEVICE_ARGS)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        if (_renderNode == deviceName && _drmFd == drmFd)
        {
            _renderNode.clear();
            _drmFd = -1;
            _holdDrm = true;
            // TODO
        }
    });
}

void VADevice::UnRegisterNoticeCenter()
{
    std::lock_guard<std::mutex> lock(_mtx);
    NoticeCenter::Instance()->DelListener(Event::kOpenDrmDevice, this);
    NoticeCenter::Instance()->DelListener(Event::kCloseDrmDevice, this);
}

bool VADevice::OpenDrmDevice()
{
    if (_holdDrm == false /* && _drmFd && !_renderNode.empty() */)
    {
        return true;
    }
    std::vector<std::string> renderNodes = GetRenderNodes();
    VAAPI_LOG_INFO << "Render node list";
    for (size_t i=0; i<renderNodes.size(); i++)
    {
        VAAPI_LOG_INFO << "-- (" << i << ") render node : " << renderNodes[i];
    }
    if (renderNodes.empty())
    {
        VAAPI_LOG_ERROR << "Can not find available render node";
        assert(false);
        goto END;
    }
    for (auto renderNode : renderNodes)
    {
        VAAPI_LOG_INFO << "Try to open render node, render node is: " << renderNode;
        _drmFd = open(renderNode.c_str(), O_RDWR | O_CLOEXEC);
        if (_drmFd < 0)
        {
            VAAPI_LOG_ERROR << "Open " << renderNode << " fail";
        }
        else
        {
            VAAPI_LOG_INFO << "Open " << renderNode << " successfully";
            _renderNode = renderNode;
            break;
        }
    }
    return true;
END:
    return false;
}

void VADevice::CloseDrmDeivce()
{
    if (_holdDrm && _drmFd)
    {
        close(_drmFd);
        _drmFd = -1;
        _renderNode.clear();
    }
}

void VADevice::QueryDeviceCompability()
{
    {
        _vendor = std::string(vaQueryVendorString(_display));
    }
    {
        int imageCount = 0;
        imageCount = vaMaxNumImageFormats(_display);
        if (imageCount <= 0)
        {
            VAAPI_LOG_WARN << "vaMaxNumImageFormats fail";
        }
        else
        {
            _imageFormats.resize(imageCount);
            if (VA_STATUS_SUCCESS != vaQueryImageFormats(_display, _imageFormats.data(), &imageCount))
            {
                VAAPI_LOG_WARN << "vaQueryImageFormats fail";
            }
            else
            {
                VAAPI_LOG_INFO << "vaQueryImageFormats success, image count is: " << imageCount;
                // Hint : vaMaxNumImageFormats 得到的 image count 为最大可能支持, 实际查询大小可能小于此值
                _imageFormats.resize(imageCount);
            }
        }
    }
}

} // namespace Codec
} // namespace Mmp