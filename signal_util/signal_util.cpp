// signal_util.cpp : Defines the entry point for the console application.
//
#include "SignalGroup.h"
#include <stdio.h>

int main()
{
  /*SignalGroup sig_grp("tc_shave_asm.shave_dut.shave_ap_i");
  sig_grp.add_signal("tc_shave_asm.shave_dut.pwdata");
  sig_grp.add_signal("tc_shave_asm.shave_dut.pwrite");
  sig_grp.add_signal("tc_shave_asm.shave_dut.paddr");
  sig_grp.add_signal("tc_shave_asm.shave_dut.penable");
  sig_grp.add_signal("tc_shave_asm.shave_dut.psel");*/
  SignalGroup sig_grp("tc_shave_asm.shave_dut.shave_ap_o");
  sig_grp.add_signal("tc_shave_asm.shave_dut.prdata");
  sig_grp.add_signal("tc_shave_asm.shave_dut.pready");
  sig_grp.combine();
  sig_grp.save();
  return 0;
}

