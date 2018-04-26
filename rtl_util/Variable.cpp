#include "Variable.h"

using namespace vcd;

Variable::Variable()
{
}

Variable::~Variable()
{
}


void Variable::add(const variable_component &comp)
{
  components_.push_back(comp);
}


void Variable::build_component_lut()
{
  const size_t components = components_.size();

  component_list_.resize(components);
  variable_component comp;
  watch_list_.reserve(64);

  for (size_t c = 0; c < components; ++c)
  {
    comp = components_.front();
    component_list_[c] = comp;
    component_lut_[std::string(comp.identifier_code)] = &component_list_[c];
    component_lut_by_name_[std::string(comp.name)] = &component_list_[c];
    components_.pop_front();
  }
}

bool Variable::add_watch(const std::string &watch)
{
  std::map<std::string, variable_component*>::iterator it;


  it = component_lut_by_name_.find(watch);

  if (it == component_lut_by_name_.end())
    return false;

  watch_list_.push_back(it->second);

  WatchVariable* p_watch = new WatchVariable();
  p_watch->set_variable(*it->second);
  watch_variable_lut_[std::string(it->second->identifier_code)] = p_watch;
}

bool Variable::update_change(
  const uint64_t &timestamp,
  const std::string &identifier_code,
  const std::string &bitstr)
{
  std::map<std::string, WatchVariable*>::iterator it;


  it = watch_variable_lut_.find(identifier_code);

  if (it == watch_variable_lut_.end())
    return false;

  it->second->update_change(timestamp, bitstr);
  return true;
}

void Variable::write_watch()
{
  for (
    std::map<std::string, WatchVariable*>::iterator iter = watch_variable_lut_.begin(); 
    iter != watch_variable_lut_.end(); ++iter)
  {
    iter->second->write();
  }
}