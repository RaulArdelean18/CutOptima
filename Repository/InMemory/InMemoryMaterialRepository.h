#pragma once

#include "Repository/Interfaces/IMaterialRepository.h"

class InMemoryMaterialRepository : public IMaterialRepository {
  private:
    std::vector<Material> materials;

    int findIndexById(const std::string &id) const;

  public:
    void save(const Material &material) override;
    void update(const Material &material) override;
    std::optional<Material> findById(const std::string &id) const override;
    std::vector<Material> findAll() const override;
    bool exists(const std::string &id) const override;
};
