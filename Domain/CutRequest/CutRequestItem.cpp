#include "CutRequestItem.h"
#include <stdexcept>
#include <utility>

CutRequestItem::CutRequestItem() : requiredLength(0.0), quantity(0) {}

CutRequestItem::CutRequestItem(std::string id, double requiredLength,
                               int quantity,
                               std::vector<std::string> allowedMaterialIds,
                               std::vector<std::string> allowedProfileIds)
    : id(std::move(id)), requiredLength(requiredLength), quantity(quantity),
      allowedMaterialIds(std::move(allowedMaterialIds)),
      allowedProfileIds(std::move(allowedProfileIds)) {
    if (this->id.empty()) {
        throw std::invalid_argument("CutRequestItem id cannot be empty.");
    }
    if (this->requiredLength <= 0) {
        throw std::invalid_argument("Required length must be positive.");
    }
    if (this->quantity <= 0) {
        throw std::invalid_argument("Quantity must be positive.");
    }
}

const std::string &CutRequestItem::getId() const { return id; }

double CutRequestItem::getRequiredLength() const { return requiredLength; }

int CutRequestItem::getQuantity() const { return quantity; }

const std::vector<std::string> &CutRequestItem::getAllowedMaterialIds() const {
    return allowedMaterialIds;
}

const std::vector<std::string> &CutRequestItem::getAllowedProfileIds() const {
    return allowedProfileIds;
}

bool CutRequestItem::hasFixedMaterial() const {
    return allowedMaterialIds.size() == 1;
}

bool CutRequestItem::isMaterialAllowed(const std::string &materialId) const {
    if (allowedMaterialIds.empty()) {
        return true;
    }

    for (const auto &id : allowedMaterialIds) {
        if (id == materialId) {
            return true;
        }
    }
    return false;
}

bool CutRequestItem::isProfileAllowed(const std::string &profileId) const {
    if (allowedProfileIds.empty()) {
        return true;
    }

    for (const auto &id : allowedProfileIds) {
        if (id == profileId) {
            return true;
        }
    }
    return false;
}