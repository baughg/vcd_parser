#ifndef VARIABLE_H
#define VARIABLE_H
#include <stdint.h>
#include <string>
#include <deque>
#include <map>
#include <vector>

namespace vcd {
  typedef enum
  {
    INTEGER,
    REG,
    EVENT,
    PARAMETER,
    WIRE,
    TIME,
    REAL
  }var_type;


  typedef struct
  {
    var_type type;
    uint32_t size;
    char identifier_code[64];
    char reference[256];
    uint32_t scope_id;
  }variable_component;

  class Variable
  {
  public:
    Variable();
    ~Variable();
    void add(const variable_component &comp);
    void build_component_lut();
    bool add_watch(const std::string &watch);
  private:
    std::deque<variable_component> components_;
    std::vector<variable_component> component_list_;
    std::map<std::string, variable_component*> component_lut_;
  };
}
#endif

