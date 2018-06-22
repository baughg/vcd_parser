#ifndef SIGNAL_GROUP_H
#define SIGNAL_GROUP_H
#include "Signal.h"
#include <deque>

#include <string>

class SignalGroup
{
public:
  SignalGroup(const std::string &name);
  ~SignalGroup();
  bool add_signal(const std::string &name);
  bool combine();
  bool save();
private:
  void copy_bits(
    uint8_t* p_src,
    uint8_t* p_dst,
    uint32_t &dst_offset,
    const uint32_t &count);

  std::string name_;
  std::deque<Signal> signal_list_;
  Signal combined_signal_;
};
#endif

