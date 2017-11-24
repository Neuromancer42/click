//
// Created by neuromancer on 11/24/17.
//

#ifndef CLICK_MYTCP_HH
#define CLICK_MYTCP_HH

#include <click/timer.hh>
#include "tcpcb.h"
#include <click/element.hh>
#include <click/config.h>

CLICK_DECLS

/*
 * =c
 * MyTCP
 * =s tcpudp
 * =d
 * A rather naive implementation of TCP protocol
 */

class MyTCP : public Element {

private:
  click_tcpcb cb;
  Timer _timer;
  void cancel_timers();

public:

  MyTCP();
  ~MyTCP();

  const char *class_name() const { return "MyTCP"; }
  const char *port_count() const { return "2/2";   }
  const char *processing() const { return PUSH;    }

  int configure(Vector<String>&, ErrorHandler*);
  int initialize(ErrorHandler*);

  void run_timer(Timer*);
  void push(int, Packet*);

};

CLICK_ENDDECLS
#endif //CLICK_MYTCP_HH
