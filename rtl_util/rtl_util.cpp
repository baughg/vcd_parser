// rtl_util.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "vcd.h"
#include "Scope.h"
#include "Variable.h"
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <limits>
#include <fstream>
#include <string>
#include <sstream>
#include <deque>
#include <cassert>
#include <iostream>

bool read_vcd(const std::string &vcd_filename, const std::deque<std::string> &dump_signals);
bool read_command_file(std::string cmd_filename, std::vector<std::string> &command_list);

int main(int argc, char** argv)
{
  std::string cmd_filename{ "vcdparse.cmd" };

  if (argc >= 2) {
    cmd_filename = std::string{ argv[1] };
  }

  std::vector<std::string> command_list;

  if (read_command_file(cmd_filename, command_list)) {
    std::deque<std::string> dump_signals;
    auto signal_it{ command_list.begin() };
    signal_it++;

    for (; signal_it != command_list.end(); signal_it++) {
      dump_signals.push_back(*signal_it);
    }

    read_vcd(command_list[0], dump_signals);
  }
  return 0;
}

bool read_vcd(const std::string &vcd_filename, const std::deque<std::string> &dump_signals)
{
  std::ifstream vcd_file(vcd_filename.c_str());

  std::string line_in;

  uint32_t scope_level = 0;
  uint32_t scope_id = 0;
  uint32_t parent_id = 0;
  std::deque<uint32_t> scope_parent_list;
  uint64_t time_stamp = 1ULL;

  vcd::vcd_parse_state state = vcd::NONE;
  vcd::Scope scope;
  vcd::Variable variable;
  vcd::scope_component scope_comp;
  vcd::variable_component var_comp;
  std::string reference;
  bool data_section = false;
  std::string bit_string;
  std::string identifier_code;

  if (!vcd_file.good())
    return false;
  uint32_t line_no = 0;

  std::cout << "Reading VCD: " << vcd_filename << std::endl;

  while (!vcd_file.eof()) {
    getline(vcd_file, line_in);
    //printf("%s\n", line_in.c_str());
    std::istringstream ss(line_in);
    std::string token;

    line_no++;

    if (!data_section) {
      while (std::getline(ss, token, ' ')) {
        if (!token.length())
          continue;

        if (strcmp(token.c_str(), "$scope") == 0)
        {
          state = vcd::SCOPE;
        }
        else if (strcmp(token.c_str(), "$var") == 0)
        {
          state = vcd::VAR;
        }
        else if (strcmp(token.c_str(), "$upscope") == 0)
        {
          state = vcd::UPSCOPE;
        }
        else if (strcmp(token.c_str(), "$enddefinitions") == 0)
        {
          state = vcd::DATA_SECTION;
          data_section = true;
          scope.build_long_name();
          variable.build_component_lut(scope);

          for (auto it{ dump_signals.begin() }; it != dump_signals.end(); it++) {
            if (!variable.add_watch(*it)) {
              std::cerr << "Could not find signal " << *it << std::endl;
              assert(false);
            }
            else {
              std::cout << "Added watch signal: " << *it << std::endl;
            }
          }
          //variable.add_watch("mysignal1");
        }


        switch (state)
        {
        case vcd::SCOPE:
          state = vcd::SCOPE_TYPE;
          scope_level++;

          parent_id = 0;

          if (scope_parent_list.size())
          {
            parent_id = scope_parent_list.back();
          }
          scope_id++;
          scope_comp.id = scope_id;
          scope_comp.parent_id = parent_id;
          scope_parent_list.push_back(scope_id);
          break;
        case vcd::UPSCOPE:
          if (scope_level)
            scope_level--;

          if (scope_parent_list.size())
            scope_parent_list.pop_back();
          state = vcd::END;
          break;
        case vcd::SCOPE_TYPE:
          state = vcd::SCOPE_NAME;

          if (strcmp(token.c_str(), "module") == 0)
          {
            scope_comp.type = vcd::MODULE;
          }
          else if (strcmp(token.c_str(), "function") == 0)
          {
            scope_comp.type = vcd::FUNCTION;
          }
          else if (strcmp(token.c_str(), "begin") == 0)
          {
            scope_comp.type = vcd::BEGIN;
          }
          else
            state = vcd::NONE;
          break;
        case vcd::SCOPE_NAME:
          sprintf(scope_comp.name, "%s", token.c_str());
          scope.add(scope_comp);
          state = vcd::END;
          break;
        case vcd::END:
          state = vcd::NONE;
          break;
        case vcd::VAR:
          state = vcd::VAR_TYPE;
          var_comp.scope_id = scope_id;
          break;
        case vcd::VAR_TYPE:
          state = vcd::VAR_SIZE;

          if (strcmp(token.c_str(), "integer") == 0)
          {
            var_comp.type = vcd::INTEGER;
          }
          else if (strcmp(token.c_str(), "reg") == 0)
          {
            var_comp.type = vcd::REG;
          }
          else if (strcmp(token.c_str(), "event") == 0)
          {
            var_comp.type = vcd::EVENT;
          }
          else if (strcmp(token.c_str(), "parameter") == 0)
          {
            var_comp.type = vcd::PARAMETER;
          }
          else if (strcmp(token.c_str(), "wire") == 0)
          {
            var_comp.type = vcd::WIRE;
          }
          else if (strcmp(token.c_str(), "time") == 0)
          {
            var_comp.type = vcd::TIME;
          }
          else if (strcmp(token.c_str(), "real") == 0)
          {
            var_comp.type = vcd::REAL;
          }
          else
            state = vcd::NONE;
          break;
        case vcd::VAR_SIZE:
          var_comp.size = atoi(token.c_str());
          state = vcd::VAR_IDENTIFIER_CODE;
          break;
        case vcd::VAR_IDENTIFIER_CODE:
          sprintf(var_comp.identifier_code, "%s", token.c_str());
          state = vcd::VAR_REFERENCE;
          break;
        case vcd::VAR_REFERENCE:
          reference = token;
          sprintf(var_comp.name, "%s", reference.c_str());
          state = vcd::VAR_REG_BITS;
          break;
        case vcd::VAR_REG_BITS:
          if (strcmp(token.c_str(), "$end") != 0)
          {
            reference.append(token);
            state = vcd::END;
          }
          else {
            state = vcd::NONE;
          }

          sprintf(var_comp.reference, "%s", reference.c_str());
          variable.add(var_comp);
          break;
        }
      }
    }
    else
    {
      if (line_in[0] == '#')
      {
        state = vcd::SET_TIME_UNIT;
        token = line_in.substr(1, line_in.length() - 1);
        time_stamp = (uint64_t)atoi(token.c_str());
        continue;
      }

      while (std::getline(ss, token, ' ')) {
        if (!token.length())
          continue;

        if (token[0] == 'b')
        {
          state = vcd::UPDATE_VAR_BINARY;
        }
        else if (strcmp(token.c_str(), "$dumpvars") == 0)
        {
          continue;
        }
        else if (state != vcd::VAR_IDENTIFIER_CODE)
          state = vcd::UPDATE_VAR;

        switch (state)
        {
        case vcd::UPDATE_VAR_BINARY:
          bit_string = token.substr(1, token.length() - 1);
          state = vcd::VAR_IDENTIFIER_CODE;
          break;
        case vcd::VAR_IDENTIFIER_CODE:
          identifier_code = token;
          state = vcd::NONE;
          variable.update_change(time_stamp, identifier_code, bit_string);
          break;
        case vcd::UPDATE_VAR:
          bit_string = token.substr(0, 1);
          identifier_code = token.substr(1, token.length() - 1);
          state = vcd::NONE;
          variable.update_change(time_stamp, identifier_code, bit_string);
          break;
        }
      }
    }


  }

  vcd_file.close();
  variable.write_watch();
  return true;
}



bool read_command_file(std::string cmd_filename, std::vector<std::string> &command_list) {
  const std::string command_list_file{ cmd_filename };

  std::ifstream cmd_file(command_list_file.c_str());

  std::string line_in;

  if (!cmd_file.good())
    return false;

  uint32_t line_no{};
  command_list.reserve(64);

  while (!cmd_file.eof()) {
    getline(cmd_file, line_in);

    if (line_in.length() > 0) {
      command_list.push_back(line_in);
    }
    line_no++;
  }

  return line_no >= 2;
}