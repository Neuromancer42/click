#ifndef CLICK_OSPFSENDHELLO_HH
#define CLICK_OSPFSENDHELLO_HH
#include <click/element.hh>
#include <click/timer.hh>

CLICK_DECLS

/*
 * =c
 * OSPFSendHello(ID)
 * =s iproute
 * =d
 * Sends periodic OSPF hello packets to neighbours.
 * including IP headers. Much simplified.
 */

class OSPFSendHello : public Element {

private:

  IPAddress _id;        // router ID, preassigned as main IP of a router
  Timer _timer;

public:

  OSPFSendHello();
  ~OSPFSendHello();

  const char *class_name() const  { return "OSPFSendHello"; }
  const char *port_count() const  { return "0/1";           }
  const char *processing() const  { return PUSH;            }

  int configure(Vector<String> &, ErrorHandler *);
  int initialize(ErrorHandler *);

  void run_timer(Timer *);

};

CLICK_ENDDECLS

#endif
