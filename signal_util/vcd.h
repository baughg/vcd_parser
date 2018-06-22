#ifndef VCD_H
#define VCD_H
#include <stdint.h>
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
    char name[256];
    char long_name[1024];
    uint32_t scope_id;
  }variable_component;

  typedef struct
  {
    std::vector<uint32_t> bitfield;
    uint64_t timestamp;
  }signal_change;
}
#endif