#pragma once

#include "Domain/BarProfile/BarProfile.h"
#include <optional>
#include <string>
#include <vector>

class IBarProfileRepository {
  public:
    virtual ~IBarProfileRepository() = default;

    virtual void save(const BarProfile &profile) = 0;
    virtual void update(const BarProfile &profile) = 0;
    virtual std::optional<BarProfile> findById(const std::string &id) const = 0;
    virtual std::vector<BarProfile> findAll() const = 0;
    virtual std::vector<BarProfile>
    findByMaterialId(const std::string &materialId) const = 0;
    virtual bool exists(const std::string &id) const = 0;
};
