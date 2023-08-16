#pragma once
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <optional>

#ifdef interface
#undef interface
#endif

namespace android {
namespace vintf {

    enum class HalFormat : size_t {
        HIDL = 0,
        NATIVE,
        AIDL,
    };

    class ManifestInstance
    {

    public:

        HalFormat format() const;

        const std::string& package() const;

        const std::string& interface() const;

        const std::string& instance() const;

        const std::optional<std::string>& updatableViaApex() const;

        const std::optional<std::string> ip() const;

        const std::optional<uint64_t> port() const;

    private:

        std::string m_package;
        std::string m_interface;
        std::string m_instance;
        std::string m_ip;
        uint64_t m_port;
        std::string m_viaApex;
    };

    struct HalManifest
    {

        // Return whether a given AIDL instance is in this manifest with version >= the given version.
        bool hasAidlInstance(const std::string& package, size_t version,
            const std::string& interfaceName, const std::string& instance) const
        {
            return true;
        }

        // Return whether a given AIDL instance is in this manifest with any version.
        bool hasAidlInstance(const std::string& package, const std::string& interfaceName,
            const std::string& instance) const
        {
            return true;
        }

        std::set<std::string> getAidlInstances(const std::string& package, size_t version,
            const std::string& interfaceName) const
        {
            return std::set<std::string>();
        }

        std::set<std::string> getAidlInstances(const std::string& package,
            const std::string& interfaceName) const
        {
            return std::set<std::string>();
        }

        bool forEachInstance(const std::function<bool(const ManifestInstance&)>& func) const
        {
            return true;
        }

    };

    class VintfObject
    {

    public:

        static VintfObject* GetInstance();

        /*
        * Return the API that access the device-side HAL manifests built from component pieces on the
        * vendor partition.
        */
        virtual std::shared_ptr<const HalManifest> getDeviceHalManifest();

        /*
        * Return the API that access the framework-side HAL manifest built from component pieces on the
        * system partition.
        */
        std::shared_ptr<const HalManifest> getFrameworkHalManifest();

    };


}
}

