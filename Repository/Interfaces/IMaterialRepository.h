#pragma once

#include "Domain/Material/Material.h"
#include <optional>
#include <string>
#include <vector>

class IMaterialRepository {
  public:
    virtual ~IMaterialRepository() = default;

    virtual void save(const Material &material) = 0;
    virtual void update(const Material &material) = 0;
    virtual std::optional<Material> findById(const std::string &id) const = 0;
    virtual std::vector<Material> findAll() const = 0;
    virtual bool exists(const std::string &id) const = 0;
};
