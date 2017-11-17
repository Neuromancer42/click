/*
 * ospfsendhello.{cc,hh} -- infor neighbours of existence with IP packets
 * Author: Yifan CHEN, Haoze WU
 */

#include <click/config.h>
#include "ospfsendhello.hh"
#include <click/args.hh>
#include <click/glue.hh>
#include <clicknet/ip.h>
#include <click/integers.hh>
#include "ospf.h"
#include <click/packet.hh>

CLICK_DECLS

OSPFSendHello::OSPFSendHello() : _timer(this) { }

OSPFSendHello::~OSPFSendHello() { }

int OSPFSendHello::configure(Vector<String> &conf, ErrorHandler *errh)
{
  return Args(conf, this, errh)
        .read_mp("ID", _id)
        .complete();
}

int OSPFSendHello::initialize(ErrorHandler *)
{
  _timer.initialize(this);
  _timer.schedule_now();
  return 0;
}

void OSPFSendHello::run_timer(Timer *)
{
  click_chatter("%d: sending hello to neighbours from %s",
                Timestamp::now().sec(),
                _id.unparse().c_str());
  WritablePacket *p = Packet::make(sizeof(click_ip) + sizeof(click_ospf));
  memset(p->data(), '\0', p->length());

  /* set IP headers */
  click_ip *ipp = reinterpret_cast<click_ip *>(p->data());
  ipp->ip_len = htons(p->length() - sizeof(*ipp));
  ipp->ip_p = IPPROTO_OSPF;
  ipp->ip_src = _id.in_addr();
  ipp->ip_dst = IPAddress::make_broadcast().in_addr(); // set destination as broadcast

  /* set OSPF header */
  click_ospf *ospfp = reinterpret_cast<click_ospf *>(ipp + 1);
  ospfp->ospf_ver = 0;
  ospfp->ospf_type = 0;
  ospfp->ospf_len = 0;
  ospfp->ospf_rt = _id.addr();

  /* continue to set IP headers */
  ipp->ip_len = htons(p->length());
  ipp->ip_hl = sizeof(click_ip) >> 2;
  ipp->ip_v = 4;
  ipp->ip_ttl = 1;                      // only sent to neighbours
  ipp->ip_sum = click_in_cksum((unsigned char*) ipp, sizeof(*ipp));

  p->set_ip_header(ipp, sizeof(click_ip));

  output(0).push(p);

  _timer.schedule_after_msec(OSPF_INTERVAL_HELLO);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(OSPFSendHello)
