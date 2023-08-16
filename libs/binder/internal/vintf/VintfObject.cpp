#include "VintfObject.h"

namespace android {
namespace vintf {

    HalFormat ManifestInstance::format() const
    {
        return HalFormat::AIDL;
    }

    const std::string& ManifestInstance::package() const
    {
        return m_package;
    }

    const std::string& ManifestInstance::interface() const
    {
        return m_interface;
    }

    const std::string& ManifestInstance::instance() const
    {
        return m_instance;
    }

    const std::optional<std::string>& ManifestInstance::updatableViaApex() const
    {
        return m_viaApex;
    }

    const std::optional<std::string> ManifestInstance::ip() const
    {
        return m_ip;
    }

    const std::optional<uint64_t> ManifestInstance::port() const
    {
        return m_port;
    }

    VintfObject* VintfObject::GetInstance()
    {
        static VintfObject instance;
        return &instance;
    }

    std::shared_ptr<const HalManifest> VintfObject::getFrameworkHalManifest()
    {
        return nullptr;
    }

    std::shared_ptr<const HalManifest> VintfObject::getDeviceHalManifest()
    {
        return nullptr;
    }

}
}

