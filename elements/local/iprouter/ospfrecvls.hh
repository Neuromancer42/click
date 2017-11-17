#ifndef CLICK_OSPFRECVLS_HH
#define CLICK_OSPFRECVLS_HH

#include <click/element.hh>
#include "lsinfobase.hh"

CLICK_DECLS

/*
 * =c
 * OSPFRecvLS(LSInfoBase)
 * =s iproute
 * =d
 * Receive link-state braodcasts.
 */

class OSPFRecvLS : public Element {

private:

  LSInfoBase *lss;
  IPAddress _id;

public:

  OSPFRecvLS();
  ~OSPFRecvLS();

  const char *class_name() const { return "OSPFRecvLS"; }
  const char *port_count() const { return "1/0";        }
  const char *processing() const { return PUSH;         }

  int configure(Vector<String>&, ErrorHandler*);

  void push(int, Packet *);

};

CLICK_ENDDECLS
#endif
