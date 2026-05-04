#pragma once

#include "Repository/Interfaces/IStockBarRepository.h"

class InMemoryStockBarRepository : public IStockBarRepository {
  private:
    std::vector<StockBar> stockBars;

    int findIndexById(const std::string &id) const;

  public:
    void save(const StockBar &stockBar) override;
    void update(const StockBar &stockBar) override;
    std::optional<StockBar> findById(const std::string &id) const override;

    std::vector<StockBar> findAll() const override;
    std::vector<StockBar> findAvailable() const override;
    std::vector<StockBar>
    findByMaterialId(const std::string &materialId) const override;
    std::vector<StockBar>
    findAvailableByMaterialId(const std::string &materialId) const override;
    std::vector<StockBar>
    findByProfileId(const std::string &profileId) const override;

    bool exists(const std::string &id) const override;
};
