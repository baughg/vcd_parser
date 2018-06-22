#include "SignalGroup.h"
#include <vector>

SignalGroup::SignalGroup(const std::string &name)
  :name_(name),
  combined_signal_(name)
{
}

SignalGroup::~SignalGroup()
{
}

bool SignalGroup::add_signal(const std::string &name)
{
  Signal sig;

  if (sig.load(name))
  {
    signal_list_.push_back(sig);
    return true;
  }

  return false;
}

bool SignalGroup::save()
{
  return combined_signal_.save(name_);
}

bool SignalGroup::combine()
{
  const size_t signal_count = signal_list_.size();
  uint32_t bit_count = 0;

  if (!signal_count)
    return false;

  std::vector<Signal> signal_list(signal_count);
  std::vector<uint32_t> bit_offset(signal_count);
  std::vector<uint32_t> signal_bits(signal_count);

  for (size_t s = 0; s < signal_count; ++s) {
    signal_list[s] = signal_list_.front();
    signal_list_.pop_front();
    bit_offset[s] = bit_count;
    signal_bits[s] = signal_list[s].get_bit_count();
    bit_count += signal_bits[s];
    signal_list[s].update_change_map();
  }

  combined_signal_.set_bit_count(bit_count);

  uint32_t word_count = bit_count >> 5;

  if ((word_count << 5) < bit_count)
    word_count++;

  

  std::vector<uint64_t> ts;
  std::vector<uint64_t> timestamp;

  signal_list[0].get_timestamps(timestamp);
  
  for (size_t s = 1; s < signal_count; ++s) {
    signal_list[s].get_timestamps(ts);
    Signal::get_unique_timestamps(ts, timestamp);
  }

  std::vector<uint32_t> bitfield(word_count);
  uint8_t* p_bf = (uint8_t*)&bitfield[0];
  uint8_t* p_data = NULL;

  const size_t time_count = timestamp.size();
  uint64_t tc = 0ULL;
  uint32_t dst_offset = 0;
  bool sig_change = false;

  for (size_t t = 0; t < time_count; ++t)
  {
    tc = timestamp[t];
    sig_change = false;

    for (size_t s = 0; s < signal_count; ++s) {
      if (signal_list[s].get_signal_buffer(tc, p_data))
      {
        dst_offset = bit_offset[s];

        copy_bits(p_data, p_bf,
          dst_offset,
          signal_bits[s]);

        sig_change = true;        
      }
    }

    if (sig_change)
    {
      combined_signal_.add_signal_change(tc, p_bf);
    }
  }
  return bit_count > 0;
}

void SignalGroup::copy_bits(
  uint8_t* p_src,
  uint8_t* p_dst,
  uint32_t &dst_offset,
  const uint32_t &count)
{
  uint32_t src_byte_ind = 0;
  uint32_t dst_byte_ind = 0;
  uint32_t src_bit_ind = 0;
  uint32_t dst_bit_ind = 0;
  uint8_t mask = 0;

  for (uint32_t b = 0; b < count; ++b)
  {
    dst_byte_ind = dst_offset >> 3;
    dst_bit_ind = dst_offset % 8;
    src_byte_ind = b >> 3;
    src_bit_ind = b % 8;
    mask = 1 << dst_bit_ind;
    p_dst[dst_byte_ind] &= ~mask;
    p_dst[dst_byte_ind] |= 
      (((p_src[src_byte_ind] >> src_bit_ind) & 0x1) << dst_bit_ind);
    dst_offset++;
  }
}