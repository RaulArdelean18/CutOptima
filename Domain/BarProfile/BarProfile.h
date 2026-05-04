#pragma once

#include "../Material/Material.h"
#include <string>

class BarProfile {
  private:
    std::string id;
    std::string name;
    std::string barcode;
    Material material;
    double standardLength;
    double purchasePrice;

  public:
    BarProfile();
    BarProfile(std::string id, std::string name, std::string barcode,
               const Material &material, double standardLength,
               double purchasePrice);

    /// Getter zone

    const std::string &getId() const;
    const std::string &getName() const;
    const std::string &getBarcode() const;
    const Material &getMaterial() const;
    double getStandardLength() const;
    double getPurchasePrice() const;

    /// Setter zone

    void setName(const std::string &newName);
    void setBarcode(const std::string &newBarcode);
    void setStandardLength(double newLength);
    void setPurchasePrice(double newPrice);
};