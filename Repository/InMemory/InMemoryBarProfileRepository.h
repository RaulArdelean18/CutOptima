#pragma once

#include "Repository/Interfaces/IBarProfileRepository.h"

class InMemoryBarProfileRepository : public IBarProfileRepository {
  private:
    std::vector<BarProfile> profiles;

    int findIndexById(const std::string &id) const;

  public:
    void save(const BarProfile &profile) override;
    void update(const BarProfile &profile) override;
    std::optional<BarProfile> findById(const std::string &id) const override;
    std::vector<BarProfile> findAll() const override;
    std::vector<BarProfile>
    findByMaterialId(const std::string &materialId) const override;
    bool exists(const std::string &id) const override;
};
