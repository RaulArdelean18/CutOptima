#pragma once

#include <string>

class Material {
  private:
    std::string id;
    std::string name;
    double defaultCutLoss;
    bool reusable;

  public:
    Material();
    Material(std::string id, std::string name, double defaultCutLoss,
             bool reusable);

    /// Getter zone
    
    const std::string &getId() const;
    const std::string &getName() const;
    double getDefaultCutLoss() const;
    bool isReusable() const;

    /// Setter zone

    void setName(const std::string &newName);
    void setDefaultCutLoss(double newCutLoss);
    void setReusable(bool value);
};