#ifndef SCOPE_H
#define SCOPE_H
#include <stdint.h>
#include <string>
#include <deque>
#include <map>
#include <vector>

namespace vcd {
  typedef enum
  {
    MODULE,
    FUNCTION,
    BEGIN
  }scope_type;

  typedef struct {
    scope_type type;
    char name[256];
    char long_name[1024];
    uint32_t id;
    uint32_t parent_id;
  }scope_component;

  class Scope
  {
  public:
    Scope();
    ~Scope();
    bool add(const scope_component &comp);
    void build_long_name();
    void get_name(const uint32_t &id, std::string &name);
  private:
    scope_component* get_component_by_id(const uint32_t &id);
    std::deque<scope_component> components_;
    std::vector<scope_component> component_list_;
    std::map<uint32_t, scope_component*> component_by_id_;
  };
}
#endif

