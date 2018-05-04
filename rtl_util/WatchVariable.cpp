#include "WatchVariable.h"
#include <stdio.h>

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
    if (bit == 'x')
      bit = '0';

    bit -= '0';
    byte_index = b >> 3;
    bit_index = b % 8;

    p_bits[byte_index] |= ((uint8_t)bit << bit_index);
  }

  signal_changes_.push_back(change);
  return true;
}

bool WatchVariable::write()
{
  const size_t changes = signal_changes_.size();

  if (!changes)
  {
    return false;
  }

  std::string filename = std::string(variable_.name);
  filename.append(".bin");

  FILE* dump_file = NULL;

  dump_file = fopen(filename.c_str(), "wb");
  signal_change change;

  if (dump_file)
  {
    fwrite(&variable_, sizeof(variable_), 1, dump_file);
    uint32_t change_count = (uint32_t)changes;
    fwrite(&change_count, sizeof(change_count), 1, dump_file);

    for (uint32_t c = 0; c < change_count; ++c)
    {
      change = signal_changes_.front();
      signal_changes_.pop_front();
      fwrite(&change.timestamp, sizeof(change.timestamp), 1, dump_file);
      fwrite(&change.bitfield[0], sizeof(uint32_t), word_count_, dump_file);
    }

    fclose(dump_file);
    return true;
  }

  return false;
}