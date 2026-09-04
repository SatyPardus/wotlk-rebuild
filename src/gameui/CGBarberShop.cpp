#include "gameui/CGBarberShop.hpp"

bool CGBarberShop::m_barberShopEnabled = false;
float CGBarberShop::m_minCameraDistance = 1.0f;

float CGBarberShop::GetMinCameraDistance() {
    return CGBarberShop::m_minCameraDistance;
}
