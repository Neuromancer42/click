#ifndef CLICK_OSPFSENDLS_HH
#define CLICK_OSPFSENDLS_HH
#include <click/element.hh>
#include <click/timer.hh>
#include "neighbourinfobase.hh"

CLICK_DECLS

/*
 * =c
 * OSPFSendLS(ID, NeighbourInfoBase)
 * =s iproute
 * =d
 * Broadcast periodic OSPF link-state packets,
 * including IP headers. Much simplified.
 */

class OSPFSendLS : public Element {

private:

  IPAddress _id;        // router ID, preassigned as main IP of a router
  Timer _timer;
  NeighbourInfoBase *neighbours;

public:

  OSPFSendLS();
  ~OSPFSendLS();

  const char *class_name() const  { return "OSPFSendLS";    }
  const char *port_count() const  { return "0/1";           }
  const char *processing() const  { return PUSH;            }

  int configure(Vector<String> &, ErrorHandler *);
  int initialize(ErrorHandler *);

  void run_timer(Timer *);

};

CLICK_ENDDECLS

#endif
