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


void Variable::build_component_lut(vcd::Scope &scope)
{
  const size_t components = components_.size();

  component_list_.resize(components);
  variable_component comp;
  watch_list_.reserve(64);
  std::string scope_name;

  for (size_t c = 0; c < components; ++c)
  {
    comp = components_.front();
    component_list_[c] = comp;

    if (strcmp(comp.name, "mysignal") == 0)
        printf("hit\n");

    scope.get_name(comp.scope_id, scope_name);
    component_lut_[std::string(comp.identifier_code)] = &component_list_[c];
    scope_name.append(std::string(comp.name));
    component_lut_by_name_[scope_name] = &component_list_[c];
    sprintf(component_list_[c].long_name, "%s", scope_name.c_str());
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
  return true;
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