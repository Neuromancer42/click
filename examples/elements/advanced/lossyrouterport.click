//
// Compound class that provide a port abstraction for a router
//

elementclass RouterPort { DEV $dev, IN_ADDR $in_addr, OUT_ADDR $out_addr, IN_PORT $in_port, OUT_PORT $out_port, IN_MAC $in_mac, OUT_MAC $out_mac, LOSS $loss, DELAY $delay |

//Add filtering based on ips and macs
out_dev :: ToDevice($dev)
in_dev :: FromDevice($dev)

//in and out queues
in_queue::Queue();
out_queue_data::Queue();
outQ_ctrl::Queue(); //L2 outgoing high priority control pkt queue
outQ_sched::PrioSched; //priority sched for L2 pkts

//classifier for arp requests
cl :: Classifier(12/0806 20/0001, 12/0800, -);

//arp responder
ar :: ARPResponder($in_addr $in_mac)

input -> UDPIPEncap($in_addr, $in_port, $out_addr, $out_port)
      -> EtherEncap(0x0800, $in_mac, $out_mac)
      -> RandomSample($loss)
      -> out_queue_data
      -> DelayShaper($delay)      
      -> [1]outQ_sched

//missing the arp part
in_dev -> in_queue
       -> Unqueue
       -> cl

cl[0] -> ar
      -> outQ_ctrl
      -> [0]outQ_sched

cl[1] -> HostEtherFilter($in_mac, DROP_OWN true) // check that the mac address is proper
      -> Strip(14)
      -> CheckIPHeader(INTERFACES $in_addr/24 $out_addr/24) //check this
      -> Strip(28) //strip headers out
      -> output

cl[2] -> Discard;

outQ_sched -> out_dev

}
