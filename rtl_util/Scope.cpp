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
