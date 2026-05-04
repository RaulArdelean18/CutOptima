#pragma once

#include "Domain/Stock/StockBar.h"
#include <optional>
#include <string>
#include <vector>

class IStockBarRepository {
  public:
    virtual ~IStockBarRepository() = default;

    virtual void save(const StockBar &stockBar) = 0;
    virtual void update(const StockBar &stockBar) = 0;
    virtual std::optional<StockBar> findById(const std::string &id) const = 0;

    virtual std::vector<StockBar> findAll() const = 0;
    virtual std::vector<StockBar> findAvailable() const = 0;
    virtual std::vector<StockBar>
    findByMaterialId(const std::string &materialId) const = 0;
    virtual std::vector<StockBar>
    findAvailableByMaterialId(const std::string &materialId) const = 0;
    virtual std::vector<StockBar>
    findByProfileId(const std::string &profileId) const = 0;

    virtual bool exists(const std::string &id) const = 0;
};
