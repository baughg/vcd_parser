#ifndef SIGNAL_H
#define SIGNAL_H
#include "vcd.h"
#include <string>
#include <vector>
#include <map>
#include <deque>

class Signal
{
public:
  Signal();
  Signal(const std::string &name);
  ~Signal();
  bool load(const std::string &name);
  bool save(const std::string &name);
  bool get_timestamps(std::vector<uint64_t> &timestamps);
  static void get_unique_timestamps(std::vector<uint64_t> &t0, std::vector<uint64_t> &t1);
  bool get_signal_buffer(const uint64_t &timestamp, uint8_t* &p_data);
  uint32_t get_bit_count();
  void set_bit_count(uint32_t &count);
  void add_signal_change(const uint64_t &t, uint8_t* p_data);
  void update_change_map();
private:
  vcd::variable_component variable_;
  uint32_t word_count_;
  uint32_t bit_count_;
  std::string name_;
  std::vector<vcd::signal_change> signal_changes_;
  std::deque<vcd::signal_change> signal_updates_;
  std::map<uint64_t, vcd::signal_change*> change_lut_;
};
#endif
