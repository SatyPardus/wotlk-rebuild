#include "gx/light/CGxLight.hpp"

// OFFSET: 0x683FB0
CGxLight::CGxLight() {
    this->m_dir.x = 0.0;
    this->m_dir.y = 0.0;
    this->m_dir.z = 0.0;
    this->m_ambientColor.x = 0.0;
    this->m_ambientColor.y = 0.0;
    this->m_ambientColor.z = 0.0;
    this->m_dirColor.x = 0.0;
    this->m_dirColor.y = 0.0;
    this->m_dirColor.z = 0.0;
    this->m_specularColor.x = 0.0;
    this->m_specularColor.y = 0.0;
    this->m_specularColor.z = 0.0;
    this->m_flags &= 0xFFFFFFFC;
    this->m_dir.x = 0.0;
    this->m_dir.y = 0.0;
    this->m_dir.z = 1.0;
    this->m_ambientColor.x = 0.0;
    this->m_ambientColor.y = 0.0;
    this->m_ambientColor.z = 0.0;
    this->m_dirColor.x = 1.0;
    this->m_dirColor.y = 1.0;
    this->m_quadraticAttenuation = 0.029999999;
    this->m_specularColor.x = 0.0;
    this->m_constantAttenuation = 0.0;
    this->m_dirColor.z = 1.0;
    this->m_specularColor.y = 0.0;
    this->m_linearAttenuation = 0.69999999;
    this->m_specularColor.z = 0.0;
}
