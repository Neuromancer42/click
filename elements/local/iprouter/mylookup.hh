// -*- c-basic-offset: 4 -*-
#ifndef CLICK_MYLOOKUP_HH
#define CLICK_MYLOOKUP_HH
#include "routetable.hh"
CLICK_DECLS

/*
=c

MyLookup(ADDR1/MASK1 [GW1] OUT1, ADDR2/MASK2 [GW2] OUT2, ...)

=s iproute

IP routing lookup using a vector

*/

class MyLookup : public Element {

private:

  RouteTable * rt;

public:

  MyLookup();
  ~MyLookup();

  const char *class_name() const	{ return "MyLookup"; }
  const char *port_count() const	{ return "1/-"; }
  const char *processing() const	{ return PUSH; }

  int configure(Vector<String> &conf, ErrorHandler *errh);

  void push(int port, Packet* p);

  enum {
    RT_SIZE_MAX = 256 * 1024, // accomodate a full BGP view and more
    tbl_24_31_capacity_limit = 32768 * 256,
    vport_capacity_limit = 32768,
    PREF_HASHSIZE = 64 * 1024, // must be a power of 2!
    DISCARD_PORT = -1
  };

  struct CleartextEntry {
    int ll_next;
    int ll_prev;
    uint32_t prefix;
    uint16_t plen;
    int16_t vport;
  };

  struct VirtualPort {
    int16_t ll_next;
    int16_t ll_prev;
    int32_t refcount;
    IPAddress gw;
    int16_t port;
    int16_t padding;
  };

};

CLICK_ENDDECLS
#endif
