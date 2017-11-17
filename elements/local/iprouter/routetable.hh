// -*- c-basic-offset: 4 -*-
#ifndef CLICK_ROUTETABLE_HH
#define CLICK_ROUTETABLE_HH
#include <click/glue.hh>
#include <click/element.hh>
#include <click/vector.hh>

CLICK_DECLS

/*
=c

RouteTable

=s iproute

IP routing table superclass

=d

RouteTable defines an interface useful for implementing IPv4 route lookup

*/

/* our network is small, so just a simple record satifies. */

struct Route {
  IPAddress addr;
  int32_t port;

  Route(IPAddress a, int p) : addr(a), port(p) { }
};

typedef Vector<Route> RTable;


class RouteTable : public Element { 

private:

  RTable rt;


public:

  RouteTable();
  ~RouteTable();     

  const char *class_name() const { return "RouteTable"; }
  const char *port_count() const { return "0/0";        }
  const char *processing() const { return PUSH;         }

  int configure(Vector<String>&, ErrorHandler*) CLICK_COLD;

  void add_route(const IPAddress &srcIP, int32_t port);
  void remove_route(const IPAddress &srcIP, int32_t port);
  int32_t lookup(const IPAddress &srcIP);
};

CLICK_ENDDECLS
#endif
