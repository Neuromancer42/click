#ifndef CLICK_OSPFRECVHELLO_HH
#define CLICK_OSPFRECVHELLO_HH

#include <click/element.hh>
#include "neighbourinfobase.hh"

CLICK_DECLS

/*
 * =c
 * OSPFRecvHello(NeighbourInfoBase)
 * =s iproute
 * =d
 * Receives Hello Mesages form neighbours.
 */

class OSPFRecvHello : public Element {

private:

  NeighbourInfoBase *neighbours;
  IPAddress _id;
  uint8_t _anno;

public:

  OSPFRecvHello();
  ~OSPFRecvHello();

  const char *class_name() const { return "OSPFRecvHello"; }
  const char *port_count() const { return "1/0";           }
  const char *processing() const { return PUSH;            }

  int configure(Vector<String>&, ErrorHandler*);

  void push(int, Packet *);

};

CLICK_ENDDECLS
#endif
