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

  for (size_t c = 0; c < components; ++c)
  {
    comp = components_.front();
    component_list_[c] = comp;
    component_lut_[std::string(comp.identifier_code)] = &component_list_[c];
    components_.pop_front();
  }
}

bool Variable::add_watch(const std::string &watch)
{
  return false;
}