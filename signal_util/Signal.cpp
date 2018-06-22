#include "Signal.h"
#include <algorithm>


Signal::Signal()
{
}

Signal::Signal(const std::string &name)
  : name_(name)
{
}

Signal::~Signal()
{
}

void Signal::set_bit_count(uint32_t &count)
{
  bit_count_ = count;
  word_count_ = bit_count_ >> 5;

  if ((word_count_ << 5) < bit_count_)
    word_count_++;

  memset(&variable_, 0, sizeof(variable_));
  variable_.size = bit_count_;

  if (name_.length())
  {
    sprintf(variable_.name, "%s", name_.c_str());
    sprintf(variable_.long_name, "%s", name_.c_str());
  }
}

void Signal::update_change_map()
{
  const uint32_t change_count = (uint32_t)signal_changes_.size();
  change_lut_.clear();

  for (uint32_t c = 0; c < change_count; ++c)
  {
    change_lut_[signal_changes_[c].timestamp] = &signal_changes_[c];
  }
}

bool Signal::load(const std::string &name)
{
  std::string filename = name;
  filename.append(".bin");

  FILE* signal_file = NULL;

  signal_file = fopen(filename.c_str(), "rb");
  size_t read_size = 0;
  uint32_t change_count = 0;

  if (signal_file)
  {
    read_size = fread(&variable_, sizeof(variable_), 1, signal_file);

    if (read_size != 1) {
      fclose(signal_file);
      return false;
    }

    bit_count_ = variable_.size;
    word_count_ = bit_count_ >> 5;

    if ((word_count_ << 5) < bit_count_)
      word_count_++;

    read_size = fread(&change_count, sizeof(change_count), 1, signal_file);

    if (read_size != 1) {
      fclose(signal_file);
      return false;
    }

    signal_changes_.resize(change_count);

    for (uint32_t c = 0; c < change_count; ++c)
    {
      fread(&signal_changes_[c].timestamp, sizeof(vcd::signal_change::timestamp), 1, signal_file);
      signal_changes_[c].bitfield.resize(word_count_);
      fread(&signal_changes_[c].bitfield[0], sizeof(uint32_t), word_count_, signal_file);

      change_lut_[signal_changes_[c].timestamp] = &signal_changes_[c];
    }

    fclose(signal_file);
    return true;
  }

  return false;
}

bool Signal::save(const std::string &name)
{
  std::string filename = name;
  filename.append(".bin");

  FILE* signal_file = NULL;

  signal_file = fopen(filename.c_str(), "wb");
  size_t wr_size = 0;
  uint32_t change_count = (uint32_t)signal_changes_.size();

  if (!change_count)
  {
    change_count = (uint32_t)signal_updates_.size();
    signal_changes_.resize(change_count);

    for (uint32_t c = 0; c < change_count; ++c)
    {
      signal_changes_[c] = signal_updates_.front();
      signal_updates_.pop_front();
    }
  }

  if (signal_file)
  {
    wr_size = fwrite(&variable_, sizeof(variable_), 1, signal_file);

    if (wr_size != 1) {
      fclose(signal_file);
      return false;
    }

    bit_count_ = variable_.size;
    word_count_ = bit_count_ >> 5;

    if ((word_count_ << 5) < bit_count_)
      word_count_++;

    wr_size = fwrite(&change_count, sizeof(change_count), 1, signal_file);

    if (wr_size != 1) {
      fclose(signal_file);
      return false;
    }

    for (uint32_t c = 0; c < change_count; ++c)
    {
      fwrite(&signal_changes_[c].timestamp, sizeof(vcd::signal_change::timestamp), 1, signal_file);      
      fwrite(&signal_changes_[c].bitfield[0], sizeof(uint32_t), word_count_, signal_file);      
    }

    fclose(signal_file);
    return true;
  }

  return false;
}

void Signal::add_signal_change(const uint64_t &t, uint8_t* p_data)
{
  vcd::signal_change sig;
  sig.bitfield.resize(word_count_);
  memcpy(&sig.bitfield[0], p_data, word_count_ << 2);
  sig.timestamp = t;

  signal_updates_.push_back(sig);
}

bool Signal::get_timestamps(std::vector<uint64_t> &timestamps)
{
  const size_t changes = signal_changes_.size();
  if (!changes)
    return false;

  timestamps.resize(changes);

  for (size_t c = 0; c < changes; ++c)
  {
    timestamps[c] = signal_changes_[c].timestamp;
  }
  return true;
}

bool sort_timestamp(const uint64_t &lhs, const uint64_t &rhs)
{
  return lhs < rhs;
}

void Signal::get_unique_timestamps(std::vector<uint64_t> &t0, std::vector<uint64_t> &t1)
{
  size_t t0_size = t0.size();

  t0.resize(t0.size() + t1.size());

  memcpy(&t0[t0_size], &t1[0], sizeof(uint64_t) * t1.size());

  std::sort(t0.begin(), t0.end(), sort_timestamp);

  t0_size = t0.size();

  t1.clear();
  t1.reserve(t0_size);
  uint64_t prev_time = ~0ULL;
  uint64_t t1_size = 0;

  for (size_t t = 0; t < t0_size; ++t)
  {
    if (prev_time != t0[t])
    {
      t1.push_back(t0[t]);
      t1_size++;
    }

    prev_time = t0[t];
  }

  t1.resize(t1_size);
}

bool Signal::get_signal_buffer(const uint64_t &timestamp, uint8_t* &p_data)
{
  std::map<uint64_t, vcd::signal_change*>::iterator it;
  p_data = NULL;
  it = change_lut_.find(timestamp);

  if (it == change_lut_.end())
    return false;

  p_data = (uint8_t*)&it->second->bitfield[0];
  return true;
}

uint32_t Signal::get_bit_count()
{
  return bit_count_;
}