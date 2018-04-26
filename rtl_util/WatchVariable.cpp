#include "WatchVariable.h"

using namespace vcd;

WatchVariable::WatchVariable()
  : bit_count_(0),
  word_count_(0)
{
}


WatchVariable::~WatchVariable()
{
}

void WatchVariable::set_bit_count(const uint32_t &bits)
{
  bit_count_ = bits;

  word_count_ = bit_count_ >> 5;

  if ((word_count_ << 5) < bit_count_)
    word_count_++;
}

void WatchVariable::set_variable(const variable_component &comp)
{
  variable_ = comp;
  set_bit_count(variable_.size);
}


bool WatchVariable::update_change(
  const uint64_t &timestamp,
  const std::string &bitstr)
{
  if (!bit_count_)
    return false;

  signal_change change;
  change.timestamp = timestamp;
  change.bitfield.resize(word_count_);

  uint8_t *p_bits = (uint8_t*)&change.bitfield[0];
  uint32_t byte_index = 0;
  uint32_t bit_index = 0;
  char* p_bitstr = (char*)bitstr.c_str();
  p_bitstr += (bitstr.length() - 1);
  char bit;

  for (uint32_t b = 0; b < bit_count_; ++b)
  {
    bit = *p_bitstr--;
    bit -= '0';
    byte_index = b >> 3;
    bit_index = b % 8;

    p_bits[byte_index] |= ((uint8_t)bit << bit_index);
  }

  signal_changes_.push_back(change);
  return true;
}