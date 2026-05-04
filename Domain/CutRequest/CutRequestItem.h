#pragma once

#include <string>
#include <vector>

class CutRequestItem {
  private:
    std::string id;
    double requiredLength;
    int quantity;
    std::vector<std::string> allowedMaterialIds;
    std::vector<std::string> allowedProfileIds;

  public:
    CutRequestItem();
    CutRequestItem(std::string id, double requiredLength, int quantity,
                   std::vector<std::string> allowedMaterialIds,
                   std::vector<std::string> allowedProfileIds);

    const std::string &getId() const;
    double getRequiredLength() const;
    int getQuantity() const;
    const std::vector<std::string> &getAllowedMaterialIds() const;
    const std::vector<std::string> &getAllowedProfileIds() const;

    bool hasFixedMaterial() const;
    bool isMaterialAllowed(const std::string &materialId) const;
    bool isProfileAllowed(const std::string &profileId) const;
};