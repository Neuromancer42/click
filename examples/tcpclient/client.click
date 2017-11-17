///=========================================================================///
///                    An simple client instance                            ///
///                   Author: Yifan Chen, Haoze Wu                          ///
///                                                                         ///
/// Arguments Required:                                                     ///
///   $dev, $in_mac, $out_mac,                                              ///
///   $ip                                                                   ///
///=========================================================================///

// Note: this click is only for 2-interface routers, for larger ones, a new file is needed

require(library /home/neuromancer/Projects/click/examples/elements/routerport.click);

// in-flow until lookupIP table

// Declare ports
rp :: RouterPort(DEV $dev, IN_MAC $in_mac, OUT_MAC $out_mac);


// receive packets
    rp
 -> CheckIPHeader
 -> classifyIP :: IPClassifier(ip proto 89, -);

    classifyIP [1]
 -> GetIPAddress(16)
 -> TCP;                 // TODO: to be refined

    classifyIP [0]
 -> Discard;

// send packets
// send ospf hello packets periodically
    OSPFSendHello(ID $ip)
 -> rp;

    TCP
 -> rp;
