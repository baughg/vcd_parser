#ifndef WATCH_VARIABLE_H
#define WATCH_VARIABLE_H
#include <vector>
#include <deque>
#include <stdint.h>

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
    uint32_t scope_id;
  }variable_component;

  typedef struct
  {
    std::vector<uint32_t> bitfield;
    uint64_t timestamp;
  }signal_change;

  class WatchVariable
  {
  public:
    WatchVariable();
    ~WatchVariable();
    void set_bit_count(const uint32_t &bits);
    void set_variable(const variable_component &comp);

    bool update_change(
      const uint64_t &timestamp,
      const std::string &bitstr);
    bool write();
  private:
    variable_component variable_;
    uint32_t word_count_;
    uint32_t bit_count_;
    std::deque<signal_change> signal_changes_;
  };
}
#endif
