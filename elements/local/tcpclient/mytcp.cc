//
// Created by neuromancer on 11/24/17.
//

#include "mytcp.hh"
#include <click/error.hh>
#include <click/config.h>
#include <click/args.hh>

CLICK_DECLS

MyTCP::MyTCP() : _timer(this) { }
MyTCP::~MyTCP() { }

int MyTCP::initialize(ErrorHandler *) {
  _timer.initialize(this);
  cb.cb_next = cb.cb_next = cb.cb_head = &cb;
}

void MyTCP::cancel_timers() {
  for (int i = 0; i < TCP_NTIMERS; ++i)
    cb.tcp_timer[i] = 0;
}

CLICK_ENDDECLS
