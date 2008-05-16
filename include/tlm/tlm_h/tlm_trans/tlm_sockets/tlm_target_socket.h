/*****************************************************************************

  The following code is derived, directly or indirectly, from the SystemC
  source code Copyright (c) 1996-2007 by all Contributors.
  All Rights reserved.

  The contents of this file are subject to the restrictions and limitations
  set forth in the SystemC Open Source License Version 3.0 (the "License");
  You may not use this file except in compliance with such restrictions and
  limitations. You may obtain instructions on how to receive a copy of the
  License at http://www.systemc.org/. Software distributed by Contributors
  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
  ANY KIND, either express or implied. See the License for the specific
  language governing rights and limitations under the License.

 *****************************************************************************/

#ifndef __TLM_TARGET_SOCKET_H__
#define __TLM_TARGET_SOCKET_H__

//#include <systemc>
#include "tlm_h/tlm_trans/tlm_2_interfaces/tlm_fw_bw_ifs.h"


namespace tlm {

template <unsigned int BUSWIDTH = 32,
          typename FW_IF = tlm_fw_transport_if<>,
          typename BW_IF = tlm_bw_transport_if<> >
class tlm_base_target_socket_b
{
public:
  virtual ~tlm_base_target_socket_b() {}
	
  virtual sc_core::sc_port_b<BW_IF> & get_base_port() = 0;
  virtual sc_core::sc_export<FW_IF> & get_base_export() = 0;
  virtual                    FW_IF  & get_base_interface() = 0;
};

template <unsigned int BUSWIDTH,
          typename FW_IF,
          typename BW_IF> class tlm_base_initiator_socket_b;

template <unsigned int BUSWIDTH,
          typename FW_IF,
          typename BW_IF,
          int N
#if !(defined SYSTEMC_VERSION & SYSTEMC_VERSION <= 20050714)
          ,sc_core::sc_port_policy POL
#endif
          > class tlm_base_initiator_socket;

template <unsigned int BUSWIDTH = 32,
          typename FW_IF = tlm_fw_transport_if<>,
          typename BW_IF = tlm_bw_transport_if<>,
          int N = 1
#if !(defined SYSTEMC_VERSION & SYSTEMC_VERSION <= 20050714)
          ,sc_core::sc_port_policy POL = sc_core::SC_ONE_OR_MORE_BOUND
#endif
          >
class tlm_base_target_socket : public tlm_base_target_socket_b<BUSWIDTH, FW_IF, BW_IF>, 
                               public sc_core::sc_export<FW_IF>
{
public:
  typedef FW_IF                                 fw_interface_type;
  typedef BW_IF                                 bw_interface_type;
  typedef sc_core::sc_port<bw_interface_type, N
#if !(defined SYSTEMC_VERSION & SYSTEMC_VERSION <= 20050714)
                                            , POL
#endif
                                            >   port_type;

  typedef sc_core::sc_export<fw_interface_type> export_type;
  typedef tlm_base_initiator_socket_b<BUSWIDTH,
                                      fw_interface_type,
                                      bw_interface_type>  base_initiator_socket_type;

  typedef tlm_base_initiator_socket<BUSWIDTH,
                                    fw_interface_type,
                                    bw_interface_type,
                                    N
#if !(defined SYSTEMC_VERSION & SYSTEMC_VERSION <= 20050714)
                                    ,POL
#endif
                                   > initiator_socket_type;


  typedef tlm_base_target_socket_b<BUSWIDTH,
                                   fw_interface_type,
                                   bw_interface_type> base_type;

  template <unsigned int, typename, typename, int
#if !(defined SYSTEMC_VERSION & SYSTEMC_VERSION <= 20050714)
                               ,sc_core::sc_port_policy
#endif
  
           >
  friend class tlm_base_initiator_socket;

public:
  tlm_base_target_socket()
  : export_type(sc_core::sc_gen_unique_name("tlm_base_target_socket"))
  , m_port(sc_core::sc_gen_unique_name("tlm_target_socket_port"))
  {
  }

  explicit tlm_base_target_socket(const char* name)
  : export_type(name)
  , m_port(sc_core::sc_gen_unique_name((std::string(name) + "_port").c_str()))
  {
  }

  unsigned int get_bus_width() const
  {
    return BUSWIDTH;
  }

  //
  // Bind target socket to initiator socket
  // - Binds the port of the initiator socket to the export of the target
  //   socket
  // - Binds the port of the target socket to the export of the initiator
  //   socket
  //
  void bind(base_initiator_socket_type& s)
  {
    // initiator.port -> target.export
    (s.get_base_port())(get_base_interface());
    // target.port -> initiator.export
    get_base_port()(s.get_base_interface());
  }

  void operator() (base_initiator_socket_type& s)
  {
    bind(s);
  }

  //
  // Bind target socket to target socket (hierarchical bind)
  // - Binds both the export and the port
  //
  void bind(base_type& s)
  {
    // export
    (get_base_export())(s.get_base_export());
    // port
    (s.get_base_port())(get_base_port());
  }

  void operator() (base_type& s)
  {
    bind(s);
  }

  //
  // Bind interface to socket
  // - Binds the interface to the export
  //
  void bind(fw_interface_type& ifs)
  {
    (get_base_export())(ifs);
  }

  void operator() (fw_interface_type& s)
  {
    bind(s);
  }

  //
  // Forward to 'size()' of port class
  //
  int size() const
  {
  	return m_port.size();
  }
    
  //
  // Forward to 'operator->()' of port class
  //
  bw_interface_type* operator->()
  {
    return m_port.operator->();
  }
  
  //
  // Forward to 'operator[]()' of port class
  //
  bw_interface_type* operator[](int i)
  {
    return m_port.operator[](i);
  }

  // Implementation of pure virtual functions of base class
  virtual sc_core::sc_port_b<BW_IF> & get_base_port()      { return m_port;   }
  virtual                    FW_IF  & get_base_interface() { return *this;   }
  virtual sc_core::sc_export<FW_IF> & get_base_export()    { return *this;   }

protected:
  port_type m_port;
};


//
// Convenience blocking and non-blocking socket classes
//

template <unsigned int BUSWIDTH = 32,
          typename TYPES = tlm_generic_payload_types,
          int N = 1
#if !(defined SYSTEMC_VERSION & SYSTEMC_VERSION <= 20050714)
          ,sc_core::sc_port_policy POL = sc_core::SC_ONE_OR_MORE_BOUND
#endif
          >
class tlm_target_socket :
  public tlm_base_target_socket <BUSWIDTH,
                            tlm_fw_transport_if<TYPES>,
                            tlm_bw_transport_if<TYPES>,
                            N
#if !(defined SYSTEMC_VERSION & SYSTEMC_VERSION <= 20050714)
                            ,POL
#endif
                            >
{
public:
  tlm_target_socket() :
    tlm_base_target_socket<BUSWIDTH,
                      tlm_fw_transport_if<TYPES>,
                      tlm_bw_transport_if<TYPES>,
                      N
#if !(defined SYSTEMC_VERSION & SYSTEMC_VERSION <= 20050714)
                      ,POL
#endif
                      >()
  {
  }

  explicit tlm_target_socket(const char* name) :
    tlm_base_target_socket<BUSWIDTH,
                      tlm_fw_transport_if<TYPES>,
                      tlm_bw_transport_if<TYPES>,
                      N
#if !(defined SYSTEMC_VERSION & SYSTEMC_VERSION <= 20050714)
                      ,POL
#endif
                      >(name)
  {
  }
};

} // namespace tlm

#endif
