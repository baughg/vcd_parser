#ifndef SCOPE_H
#define SCOPE_H
#include <stdint.h>
#include <string>
#include <deque>


namespace vcd {
  typedef enum
  {
    MODULE,
    FUNCTION
  }scope_type;

  typedef struct {
    scope_type type;
    char name[256];
    uint32_t id;
    uint32_t parent_id;
  }scope_component;

  class Scope
  {
  public:
    Scope();
    ~Scope();
    bool add(const scope_component &comp);
  private:
    std::deque<scope_component> components_;    
  };
}
#endif

