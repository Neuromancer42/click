#ifndef CLICK_OSPFUPDATE_HH
#define CLICK_OSPFUPDATE_HH

#include <click/element.hh>
#include <click/timer.hh>
#include "neighbourinfobase.hh"
#include "lsinfobase.hh"
#include <click/vector.hh>
#include "routetable.hh"

CLICK_DECLS

/*
 * =c
 * OSPFUpdate(NeighbourInfoBasem, LSInfoBase)
 * =s iproute
 * =d
 * Periodically calculate and update routing tables
 */

struct SimpleIPRoute {
  IPAddress addr;
  int32_t metric;
  int32_t port;
  bool v;
};

typedef Vector<SimpleIPRoute> SimpleIPRouteTable;

class OSPFUpdate : public Element {

private:

  Timer _timer;
  NeighbourInfoBase *neighbours;
  LSInfoBase *lss;
  IPAddress _id;
  RouteTable *iptable;
  SimpleIPRouteTable rt;

public:
  
  OSPFUpdate();
  ~OSPFUpdate();

  const char *class_name() const { return "OSPFUpdate"; }
  const char *port_count() const { return "0/0";        }
  const char *processing() const { return PUSH;         }

  int configure(Vector<String>&, ErrorHandler*);

  void run_timer(Timer *);

};

CLICK_ENDDECLS
#endif
