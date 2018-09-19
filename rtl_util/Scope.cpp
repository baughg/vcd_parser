#include "Scope.h"

using namespace vcd;

Scope::Scope()
{
}


Scope::~Scope()
{
}

bool Scope::add(const scope_component &comp)
{
  components_.push_back(comp);
  return true;
}

void Scope::build_long_name()
{
  const size_t components = components_.size();

  component_list_.resize(components);

  for (size_t c = 0; c < components; ++c)
  {
    component_list_[c] = components_.front();
    components_.pop_front();
    component_by_id_[component_list_[c].id] = &component_list_[c];
  }

  uint32_t id = 0;
  uint32_t parent_id = 0;
  std::vector<std::string> names;
  names.reserve(64);
  scope_component* p_comp = NULL;
  std::string long_name;

  for (size_t c = 0; c < components; ++c)
  {
    names.clear();
    long_name.clear();

    names.push_back(std::string(component_list_[c].name));

    parent_id = component_list_[c].parent_id;

    while (parent_id)
    {
      p_comp = get_component_by_id(parent_id);

      if (!p_comp)
        break;

      names.push_back(std::string(p_comp->name));
      parent_id = p_comp->parent_id;
    }

    int32_t names_cnt = (int32_t)names.size();
    names_cnt--;

    for (int32_t n = names_cnt; n >= 0; n--)
    {
      long_name.append(names[n]);      
      long_name.append(".");
    }

    sprintf(component_list_[c].long_name, "%s", long_name.c_str());
  }
}

void Scope::get_name(const uint32_t &id, std::string &name)
{
  name.clear();

  scope_component* p_comp = get_component_by_id(id);

  if (p_comp)
  {
    name = std::string(p_comp->long_name);
  }
}

scope_component* Scope::get_component_by_id(const uint32_t &id)
{
  std::map<uint32_t, scope_component*>::iterator it;


  it = component_by_id_.find(id);

  if (it == component_by_id_.end())
    return NULL;

  return it->second;
}