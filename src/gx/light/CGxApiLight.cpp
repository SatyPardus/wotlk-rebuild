#include "gx/light/CGxApiLight.hpp"

// OFFSET: 0x684620
CGxApiLight& CGxApiLight::operator=(const CGxLight& src) {
    if (src.m_constantAttenuation != this->m_constantAttenuation) {
        this->flags |= 0x20;
        this->m_constantAttenuation = src.m_constantAttenuation;
    }
    if (src.m_linearAttenuation != this->m_linearAttenuation) {
        this->flags |= 0x40;
        this->m_linearAttenuation = src.m_linearAttenuation;
    }
    if (src.m_quadraticAttenuation != this->m_quadraticAttenuation) {
        this->flags |= 0x80;
        this->m_quadraticAttenuation = src.m_quadraticAttenuation;
    }

    float wWanted = (src.m_flags & 0x2) ? 1.0f : 0.0f;

    if (src.m_dir.x != this->m_dir.x ||
        src.m_dir.y != this->m_dir.y ||
        src.m_dir.z != this->m_dir.z ||
        wWanted != this->m_dir.w) {

        this->m_dir.x = src.m_dir.x;
        this->m_dir.y = src.m_dir.y;
        this->m_dir.z = src.m_dir.z;

        this->flags |= 0x2;

        if (src.m_flags & 0x2) {
            this->flags |= 0x00E00000;
            this->m_dir.w = 1.0f;
        } else {
            this->flags &= ~0x00E00000;
            this->m_dir.w = 0.0f;
        }
    }

    if (src.m_dirColor.x != this->m_dirColor.x ||
        src.m_dirColor.y != this->m_dirColor.y ||
        src.m_dirColor.z != this->m_dirColor.z) {
        this->m_dirColor = src.m_dirColor;
        this->flags |= 0x8;
    }

    if (src.m_ambientColor.x != this->m_ambColor.x ||
        src.m_ambientColor.y != this->m_ambColor.y ||
        src.m_ambientColor.z != this->m_ambColor.z) {
        this->m_ambColor = src.m_ambientColor;
        this->flags |= 0x4;
    }

    if (src.m_specularColor.x != this->m_specColor.x ||
        src.m_specularColor.y != this->m_specColor.y ||
        src.m_specularColor.z != this->m_specColor.z) {
        this->m_specColor = src.m_specularColor;
        this->flags |= 0x10;
    }

    return *this;
}
