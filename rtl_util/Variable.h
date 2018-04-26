#ifndef VARIABLE_H
#define VARIABLE_H
#include "WatchVariable.h"
#include <stdint.h>
#include <string>
#include <deque>
#include <map>
#include <vector>

namespace vcd {
  class Variable
  {
  public:
    Variable();
    ~Variable();
    void add(const variable_component &comp);
    void build_component_lut();
    bool add_watch(const std::string &watch);
    bool update_change(
      const uint64_t &timestamp,
      const std::string &identifier_code,
      const std::string &bitstr);
  private:
    std::vector<variable_component*> watch_list_;    
    std::map<std::string, WatchVariable*> watch_variable_lut_;
    std::deque<variable_component> components_;
    std::vector<variable_component> component_list_;
    std::map<std::string, variable_component*> component_lut_;
    std::map<std::string, variable_component*> component_lut_by_name_;
  };
}
#endif

