/*****************************************************************************

  Licensed to Accellera Systems Initiative Inc. (Accellera) under one or
  more contributor license agreements.  See the NOTICE file distributed
  with this work for additional information regarding copyright ownership.
  Accellera licenses this file to you under the Apache License, Version 2.0
  (the "License"); you may not use this file except in compliance with the
  License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.

 *****************************************************************************/

/*****************************************************************************

  blast3.cpp -- 

  Original Author: Martin Janssen, Synopsys, Inc., 2002-02-15

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#include "systemc.h"

SC_MODULE( array )
{

    sc_in_clk clk;

    const sc_signal<bool>& reset;
          sc_signal<bool>& ready;
    const sc_signal<char>& a;
    const sc_signal<char>& b;
          sc_signal<short>& c;

    char mem[17];
    sc_unsigned i, j;

    array( sc_module_name NAME,
           sc_clock& CLK,
           const sc_signal<bool>& RESET,
                 sc_signal<bool>& READY,
           const sc_signal<char>& A,
           const sc_signal<char>& B,
                 sc_signal<short>& C )
        : 
          reset(RESET), ready(READY),
          a(A), b(B), c(C),
          i(2), j(2)
    {
        clk(CLK);
		SC_CTHREAD( entry, clk );
        reset_signal_is(reset,true);
    }
    void entry();
};

void
array::entry()
{
    i = 2;
    j = 1;
    ready = 0;
    mem[i.to_uint()] = a.read();
    mem[j.to_uint()] = b.read();
    mem[14] = i.to_uint();
    mem[12] = j.to_uint();
    wait();
    mem[+mem[14]] = a + 1;
    mem[+mem[12]] = b - 2;
    c = mem[i.to_uint()] * mem[j.to_uint()];
    ready = 1;
    wait();
}

int sc_main(int argc, char* argv[] )
{
  return 0;
}

