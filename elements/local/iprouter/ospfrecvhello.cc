#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include "ospf.h"
#include "ospfrecvhello.hh"
// use paints to distinguish ports
#include <click/packet_anno.hh>
#include <click/args.hh>

CLICK_DECLS

OSPFRecvHello::OSPFRecvHello(): neighbours(0) { }

OSPFRecvHello::~OSPFRecvHello() { }

int OSPFRecvHello::configure(Vector<String> &conf, ErrorHandler *errh) {
  if (cp_va_kparse(conf, this, errh,
                   "NBINFOBASE", cpkM, cpElement, &neighbours,
                   "ID", cpkM, cpIPAddress, &_id,
                   cpEnd) < 0)
    return -1;

  _anno = PAINT_ANNO_OFFSET;

  return 0;
}

void OSPFRecvHello::push(int, Packet *p) {
  WritablePacket *q = p->uniqueify();

  click_ip *iph = (click_ip *) q->data();
  click_ospf *ospfh = (click_ospf *) (iph + 1);

  IPAddress from(ospfh->ospf_rt);
  int32_t port = p->anno_u8(_anno);

  if (from != _id) {
    click_chatter("%d: hear hello from neighbour %s",
                  Timestamp::now().sec(),
                  from.unparse().c_str());
    if (neighbours) {
      neighbours->addNeighbour(from, port);
    }
  }

  p->kill();
}

CLICK_ENDDECLS
EXPORT_ELEMENT(OSPFRecvHello)
