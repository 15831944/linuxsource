/*_############################################################################
  _## 
  _##  uxsnmp.h  
  _##
  _##  SNMP++v3.2.17
  _##  -----------------------------------------------
  _##  Copyright (c) 2001-2005 Jochen Katz, Frank Fock
  _##
  _##  This software is based on SNMP++2.6 from Hewlett Packard:
  _##  
  _##    Copyright (c) 1996
  _##    Hewlett-Packard Company
  _##  
  _##  ATTENTION: USE OF THIS SOFTWARE IS SUBJECT TO THE FOLLOWING TERMS.
  _##  Permission to use, copy, modify, distribute and/or sell this software 
  _##  and/or its documentation is hereby granted without fee. User agrees 
  _##  to display the above copyright notice and this license notice in all 
  _##  copies of the software and any documentation of the software. User 
  _##  agrees to assume all liability for the use of the software; 
  _##  Hewlett-Packard and Jochen Katz make no representations about the 
  _##  suitability of this software for any purpose. It is provided 
  _##  "AS-IS" without warranty of any kind, either express or implied. User 
  _##  hereby grants a royalty-free license to any and all derivatives based
  _##  upon this software code base. 
  _##  
  _##  Stuttgart, Germany, Sun May  8 22:45:16 CEST 2005 
  _##  
  _##########################################################################*/

#ifndef _UXSNMP_H_
#define _UXSNMP_H_

#include "snmp_pp/reentrant.h"
#include "snmp_pp/target.h"
#include "snmp_pp/oid.h"
#include "snmp_pp/address.h"

#ifdef SNMP_PP_NAMESPACE
namespace Snmp_pp {
#endif

#define SNMP_PP_WITH_UDPADDR // Snmp class has constructor with UdpAddress

//-----[ internally used defines ]----------------------------------------
#define MAXNAME 80                   // maximum name length
#define MAX_ADDR_LEN 10              // maximum address len, ipx is 4+6
#define SNMP_SHUTDOWN_MSG 0x0400+177 // shut down msg for stoping a blocked message
#ifndef INVALID_SOCKET 
#define INVALID_SOCKET ((SNMPHANDLE)(~0)) // value for invalid socket
#endif

//-----[ async defines for engine ]---------------------------------------
#define sNMP_PDU_GET_ASYNC       21
#define sNMP_PDU_GETNEXT_ASYNC   22
#define sNMP_PDU_SET_ASYNC       23
#define sNMP_PDU_GETBULK_ASYNC   24
#define sNMP_PDU_INFORM_ASYNC    25

//-----[ trap / notify macros ]-------------------------------------------
#define IP_NOTIFY  162     // IP notification
#define IPX_NOTIFY 0x2121  // IPX notification


//------[ forward declaration of Snmp class ]-----------------------------
class Snmp;
class EventListHolder;
class Pdu;
class v3MP;

//-----------[ async methods callback ]-----------------------------------
/**
 * Async methods of the class Snmp require the caller to provide a
 * callback address of a function with this typedef.
 *
 * @note It is not allowed to call any synchronous Snmp methods within the
 *       callback. Async methods are allowed.
 *
 * @param reason  - Reason for callback (see snmperrs.h)
 * @param session - Pointer to Snmp object that was used to send the request
 * @param pdu     - The received Pdu if reason indicates a received message
 * @param target  - source target
 * @param data    - Pointer passed to the async method
 */
typedef void (*snmp_callback)(int reason, Snmp *session,
                               Pdu &pdu, SnmpTarget &target, void *data);


//------------[ SNMP Class Def ]---------------------------------------------
//
/**
 * SNMP class defintion. The Snmp class provides an object oriented
 * approach to SNMP. The SNMP class is an encapsulation of SNMP
 * sessions, gets, sets and get nexts. The class manages all SNMP
 * resources and provides complete retry and timeout capability.
 *
 * This class is thread save.
 *
 * @note If you use the async methods to send requests you MUST call
 *       Snmp::eventListHolder->SNMPProcessPendingEvents() while waiting
 *       for the responses. This function triggers the resend of
 *       packets and calls your callback function if the response is
 *       received.
 *
 * @note Call srand() before creating the first Snmp object.
 */
class DLLOPT Snmp: public SnmpSynchronized
{
 public:
  //------------------[ constructors ]----------------------------------

  /** @name Constructors and Destructor */
  //@{

  /**
   * Construct a new SNMP session using the given UDP port.
   *
   * @param status
   *    after creation of the session this parameter will 
   *    hold the creation status.
   * @param port
   *    an UDP port to be used for the session
   * @param bind_ipv6
   *    Set this to true if IPv6 should be used. The default is
   *    IPv4.
   */
  Snmp(int &status, const unsigned short port = 0,
       const bool bind_ipv6 = false);

  /**
   * Construct a new SNMP session using the given UDP address.
   * Thus, binds the session on a specific IPv4 or IPv6 address. 
   *
   * @param status
   *    after creation of the session this parameter will 
   *    hold the creation status.
   * @param addr
   *    an UDP address to be used for the session
   */	
  Snmp(int &status, const UdpAddress &addr);

  /**
   * Construct a new SNMP session using the given UDP addresses.
   * Using this constructor will bind to both IPv4 and IPv6 ports.
   *
   * @param status
   *    after creation of the session this parameter will 
   *    hold the creation status.
   * @param addr_v4
   *    an IPv4 UDP address to be used for the session
   * @param addr_v6
   *    an IPv6 UDP address to be used for the session
   */	
  Snmp(int &status,  const UdpAddress& addr_v4, const UdpAddress& addr_v6);

  //-------------------[ destructor ]------------------------------------
  /**
   * Destructor.
   */
  virtual ~Snmp();

  //@}

  //--------[ Get the version of the snmp++ library ]--------------------
  /**
   * Get the version of the snmp++ library.
   *
   * @return The version of the snmp++ lib at runtime.
   */
  static const char *get_version();


  //-------------------[ returns error string ]--------------------------
  /**
   * Returns a human readable error string.
   *
   * @param c - Error code returned by any method of this class
   * @return Null terminated error string.
   */
  static const char *error_msg(const int c);
#ifdef _SNMPv3
  /**
   * Returns a human readable error string.
   * If a report message is returned, then the contained Oid can be
   * used to get a error string.
   *
   * @param v3Oid - Oid of a SNMPv3 report Pdu
   * @return Null terminated error string.
   */
  static const char* error_msg(const Oid& v3Oid);
#endif

  //------------------------[ Windows Sockets ]----------------------------

  /**
   * Initialize the Winsock library (WSAStartup).
   *
   * @note on Win32 this method *must* be called before creating Snmp or
   *       Address objects.
   */
  static void socket_startup();


  /**
   * Shut down the Winsock library (WSACleanup).
   */
  static void socket_cleanup();

  //------------------------[ send requests ]------------------------------

  /** @name Sending SNMP Pdus
   */
  //@{

  /**
   * Send a blocking SNMP-GET request.
   *
   * @param pdu    - Pdu to send
   * @param target - Target for the get
   *
   * @return SNMP_CLASS_SUCCES or a negative error code
   */
  virtual int get(Pdu &pdu, const SnmpTarget &target);

  /**
   * Send a async SNMP-GET request.
   *
   * @param pdu      - Pdu to send
   * @param target   - Target for the get
   * @param callback - User callback function to use
   * @param callback_data - User definable data pointer
   *
   * @return SNMP_CLASS_SUCCES or a negative error code
   */
  virtual int get(Pdu &pdu, const SnmpTarget &target,
		  const snmp_callback callback,
		  const void *callback_data = 0);

  /**
   * Send a blocking SNMP-GETNEXT request.
   *
   * @param pdu    - Pdu to send
   * @param target - Target for the getnext
   *
   * @return SNMP_CLASS_SUCCES or a negative error code
   */
  virtual int get_next(Pdu &pdu, const SnmpTarget &target);

  /**
   * Send a async SNMP-GETNEXT request.
   *
   * @param pdu      - Pdu to send
   * @param target   - Target for the getnext
   * @param callback - User callback function to use
   * @param callback_data - User definable data pointer
   *
   * @return SNMP_CLASS_SUCCES or a negative error code
   */
  virtual int get_next(Pdu &pdu, const SnmpTarget &target,
                       const snmp_callback callback,
		       const void *callback_data = 0);

  /**
   * Send a blocking SNMP-SET request.
   *
   * @param pdu    - Pdu to send
   * @param target - Target for the set
   *
   * @return SNMP_CLASS_SUCCES or a negative error code
   */
  virtual int set(Pdu &pdu, const SnmpTarget &target);

  /**
   * Send a async SNMP-SET request.
   *
   * @param pdu      - Pdu to send
   * @param target   - Target for the set
   * @param callback - User callback function to use
   * @param callback_data - User definable data pointer
   *
   * @return SNMP_CLASS_SUCCES or a negative error code
   */
  virtual int set(Pdu &pdu, const SnmpTarget &target,
		  const snmp_callback callback,
		  const void * callback_data = 0);

  /**
   * Send a blocking SNMP-GETBULK request.
   *
   * @param pdu           - Pdu to send
   * @param target        - Target for the getbulk
   * @param non_repeaters - number of non repeaters
   * @param max_reps      - maximum number of repetitions
   *
   * @return SNMP_CLASS_SUCCES or a negative error code
   */
  virtual int get_bulk(Pdu &pdu, const SnmpTarget &target,
		       const int non_repeaters, const int max_reps);

  /**
   * Send a async SNMP-GETBULK request.
   *
   * @param pdu           - Pdu to send
   * @param target        - Target for the getbulk
   * @param non_repeaters - number of non repeaters
   * @param max_reps      - maximum number of repetitions
   * @param callback      - User callback function to use
   * @param callback_data - User definable data pointer
   *
   * @return SNMP_CLASS_SUCCES or a negative error code
   */
  virtual int get_bulk(Pdu &pdu, const SnmpTarget &target,
		       const int non_repeaters, const int max_reps,
		       const snmp_callback callback,
		       const void *callback_data = 0);

  /**
   * Send a SNMP-TRAP.
   *
   * @param pdu    - Pdu to send
   * @param target - Target for the trap
   *
   * @return SNMP_CLASS_SUCCES or a negative error code
   */
  virtual int trap(Pdu &pdu, const SnmpTarget &target);


  /**
   * Send a SNMPv3-REPORT.
   *
   * @param pdu    - Pdu to send
   * @param target - Target for the report (must be a UTarget)
   *
   * @return SNMP_CLASS_SUCCES or a negative error code
   */
  virtual int report(Pdu &pdu, const SnmpTarget &target);

  /**
   * Send a blocking INFORM-REQ.
   *
   * @param pdu    - Pdu to send
   * @param target - Target for the inform
   *
   * @return SNMP_CLASS_SUCCES or a negative error code
   */
  virtual int inform(Pdu &pdu, const SnmpTarget &target);

  /**
   * Send a async INFORM-REQ.
   *
   * @param pdu    - Pdu to send
   * @param target - Target for the inform
   * @param callback      - User callback function to use
   * @param callback_data - User definable data pointer
   *
   * @return SNMP_CLASS_SUCCES or a negative error code
   */
  virtual int inform(Pdu &pdu, const SnmpTarget &target,
		     const snmp_callback callback,
		     const void * callback_data = 0);

  /**
   * Send a RESPONSE.
   *
   * @param pdu    - Pdu to send
   * @param target - Target for the response
   * @param fd     - file descriptor to use, should be the one
   *                 that was passed to the callback function
   *
   * @return SNMP_CLASS_SUCCES or a negative error code
   */
  virtual int response(Pdu &pdu, const SnmpTarget &target,
		       const int fd = INVALID_SOCKET);


  /**
   * Send a SNMP Broadcast message.
   *
   * This member function sends out a valid SNMP message to a
   * broadcast address and waits for responses. The source addresses
   * of the response messages are added to the collection.
   *
   * The message is sent only once.
   *
   * @note SNMP_BROADCAST has to be defined in config_snmp_pp.h.
   *
   * @note There is no SNMP standard that defines "SNMP Broadcast
   *       discovery". SNMP agents are not forced to answer requests
   *       that are sent to a broadcast address.
   *
   * @note Do not use this method while waiting for other responses,
   *       as these responses will be added to the collection and dropped
   *       by this method. Solution for this problem: Use a special
   *       Snmp object only for broadcasts.
   *
   * @param addresses   - The addresses of the agents, that answered.
   * @param timeout_sec - Timeout in seconds
   * @param addr        - Broadcast address
   * @param version     - SNMP version to use
   * @param community   - Only needed for SNMPv1/v2c, defaults to "public"
   *
   */
  virtual int broadcast_discovery(UdpAddressCollection &addresses,
				  const int timeout_sec,
				  const UdpAddress &addr,
				  const snmp_version version,
				  const OctetStr *community = 0);

  //@}

  /**
   * Cancel a pending request.
   *
   * @param rid - The request id to cancel
   *
   * @return SNMP_CLASS_SUCCES or SNMP_CLASS_INVALID_REQID on failure
   */
  virtual int cancel(const unsigned long rid);


  /** @name Trap and Inform handling
   */
  //@{

  /**
   * Register to get traps and informs.
   *
   * @note Every call to one of the notify_register() methods overwrites
   *       the previous given values.
   *
   * @param trapids       - ids to listen for
   * @param targets       - targets to listen for
   * @param callback      - User callback function to use
   * @param callback_data - User definable data pointer
   *
   * @return SNMP_CLASS_SUCCESS, SNMP_CLASS_TL_FAILED or SNMP_CLASS_TL_IN_USE
   */
  virtual int notify_register(const OidCollection    &trapids,
                              const TargetCollection &targets,
                              const snmp_callback callback,
                              const void *callback_data=0);
  /**
   * Register to get traps and informs.
   *
   * @note listen_addresses param is ignored!
   *
   * @note Every call to one of the notify_register() methods overwrites
   *       the previous given values.
   *
   * @param trapids       - ids to listen for
   * @param targets       - targets to listen for
   * @param listen_addresses - interfaces to listen on
   * @param callback      - User callback function to use
   * @param callback_data - User definable data pointer
   *
   * @return SNMP_CLASS_SUCCESS, SNMP_CLASS_TL_FAILED or SNMP_CLASS_TL_IN_USE
   */
  virtual int notify_register( const OidCollection    &trapids,
                               const TargetCollection &targets,
                               const AddressCollection &listen_addresses,
                               const snmp_callback callback,
                               const void *callback_data=0);
  /**
   * Unregister to get traps and informs.
   * Undo the call to notify_register().
   *
   * @return Always SNMP_CLASS_SUCCESS
   */
  virtual int notify_unregister();

  /**
   * Get notify register info.
   *
   * @param trapids       - ids listened for
   * @param targets       - targets listened for
   *
   * @return SNMP_CLASS_SUCCESS or SNMP_CLASS_INVALID if not registered
   */
  virtual int get_notify_filter( OidCollection &trapids,
				 TargetCollection &targets)
    { AddressCollection a; return get_notify_filter(trapids, targets, a); }

  /**
   * Get notify register info.
   *
   * @param trapids       - ids listened for
   * @param targets       - targets listened for
   * @param listen_addresses - interfaces listened on
   *
   * @return SNMP_CLASS_SUCCESS or SNMP_CLASS_INVALID if not registered
   */
  virtual int get_notify_filter( OidCollection &trapids,
				 TargetCollection &targets,
				 AddressCollection &listen_addresses);


  //-----------------------[ access the trap reception info ]---------------
  /**
   * Get a pointer to the callback function used for trap reception.
   *
   * @return Pointer to the function set through notify_register()
   */
  snmp_callback get_notify_callback() { return notifycallback; };

  /**
   * Get a pointer to the data that is passed to the callback function.
   *
   * @return Pointer to the data set through notify_register()
   */
  void *get_notify_callback_data() { return notifycallback_data; };

  //@}

  /**
   * Send raw UDP data.
   * This method may be used to send any data to the recepient.
   *
   * @param send_buf - Data buffer
   * @param send_len - Length of the data
   * @param address  - Recepient
   * @param fd       - socket to use, if not specified, the socket of the
   *                   object is used
   *
   * @return 0 on success, -1 on failure
   */
  virtual int send_raw_data( unsigned char *send_buf,
                             size_t send_len, UdpAddress &address,
                             int fd = 0);

  const IpAddress &get_listen_address() const {return listen_address; };

  // this member var will simulate a global var
  EventListHolder *eventListHolder;

#ifdef _SNMPv3
  // lock for v3 cache information
  static SnmpSynchronized v3Lock;
#endif

  bool start_poll_thread(const int select_timeout);
  void stop_poll_thread();

protected:

  /**
   * Check for the status of the worker thread.
   * @return BOOL - TRUE - if running, FALSE - otherwise
   */
  bool is_running(void) const
      { return m_bThreadRunning; };
	
  /**
   * This is a working thread for the recovery of the pending events.
   *
   * @param pSnmp [in] pointer to the whole object
   *
   * @return int
   *          0 - if succesful,
   *          1 - in the case of error
   */
#ifdef WIN32
  static int process_thread(Snmp *pSnmp);
#else
  static void* process_thread(void *arg);
#endif

 protected:

  /**
   * Generate a unique (for this Snmp obect) request id.
   *
   * @return Unique id between PDU_MIN_RID and PDU_MAX_RID
   */
  long MyMakeReqId();

  /**
   * Common init function used by constructors.
   */
  void init(int& status, IpAddress*[2],
            const unsigned short port_v4, const unsigned short port_v6);

  /**
   * Set the notify timestamp of a trap pdu if the user did not set it.
   */
  void check_notify_timestamp(Pdu &pdu);

  //-----------[ Snmp Engine ]----------------------------------------
  /**
   * gets, sets and get nexts go through here....
   * This mf does all snmp sending and reception
   * except for traps which are sent using trap().
   *
   * @note that for a UTarget with an empty engine id the
   *       Utarget::set_engine_id() may be called.
   */
  int snmp_engine( Pdu &pdu,                  // pdu to use
                   long int non_reps,         // get bulk only
                   long int max_reps,         // get bulk only
                   const SnmpTarget &target,        // destination target
                   const snmp_callback cb,    // async callback function
                   const void *cbd,          // callback data
		   int fd = INVALID_SOCKET);

  //--------[ map action ]------------------------------------------------
  // map the snmp++ action to a SMI pdu type
  void map_action(unsigned short action, unsigned short &pdu_action);

#ifdef _SNMPv3
  friend void v3CallBack( int reason, Snmp *snmp, Pdu &pdu,
			  SnmpTarget &target, void *v3cd);

  /**
   * Internal used callback data structure for async v3 requests.
   */
  struct V3CallBackData
  {
    Pdu *pdu;                  ///< The Pdu that was sent
    long int non_reps;         ///< For GET-BULK requests
    long int max_reps;         ///< For GET-BULK requests
    SnmpTarget *target;        ///< Pointer to the Target object to use
    snmp_callback oldCallback; ///< User callback function
    const void *cbd;           ///< User callback data
  };
#endif

  //---[ instance variables ]
#ifdef WIN32
  unsigned long iv_snmp_session;
  unsigned long iv_snmp_session_ipv6;
#else
  SNMPHANDLE iv_snmp_session;         // session handle
  SNMPHANDLE iv_snmp_session_ipv6;    // session handle
#endif
  IpAddress listen_address;
  int iv_notify_fd;                   // fd for notify session - DLD
  SNMPHANDLE pdu_handler;             // pdu handler win proc
  SNMPHANDLE pdu_handler_ipv6;        // pdu handler win proc
  int construct_status;               // status of construction
  int construct_status_ipv6;          // status of construction ipv6
  long current_rid;                   // current rid to use

  // inform receive member variables
  snmp_callback  notifycallback;
  void * notifycallback_data;

private:

  bool m_bThreadRunning;
  int m_iPollTimeOut;

  // Keep track of the thread.
#ifdef _THREADS
#ifdef WIN32
  HANDLE m_hThread;
#else
  pthread_t m_hThread;
#endif
#endif
};

#ifdef SNMP_PP_NAMESPACE
}; // end of namespace Snmp_pp
#endif 

#endif
