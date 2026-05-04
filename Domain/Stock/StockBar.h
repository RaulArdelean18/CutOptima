#pragma once

#include "Domain/BarProfile/BarProfile.h"
#include <string>

enum class StockBarStatus { AVAILABLE, RESERVED, USED, SCRAP };

class StockBar {
  private:
    std::string id;
    std::string uniqueBarcode;
    BarProfile profile;
    double originalLength;
    double remainingLength;
    StockBarStatus status;

  public:
    StockBar();
    StockBar(std::string id, std::string uniqueBarcode,
             const BarProfile &profile, double originalLength,
             double remainingLength,
             StockBarStatus status = StockBarStatus::AVAILABLE);

    /// Getter zone

    const std::string &getId() const;
    const std::string &getUniqueBarcode() const;
    const BarProfile &getProfile() const;
    double getOriginalLength() const;
    double getRemainingLength() const;
    StockBarStatus getStatus() const;
    double getUsedLength() const;

    /// Setter zone

    void setStatus(StockBarStatus newStatus);

    bool canFit(double requiredLength) const;
    bool consume(double usedLength);
    bool isAvailable() const;
    
    
};
