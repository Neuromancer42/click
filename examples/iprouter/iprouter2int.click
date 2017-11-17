///=========================================================================///
///              An IP router instance with 2 interfaces                    ///
///                   Author: Yifan Chen, Haoze Wu                          ///
///                                                                         ///
/// Arguments Required:                                                     ///
///   $dev0, $in_mac0, $out_mac0,                                           ///
///   $dev1, $in_mac1, $out_mac1,                                           ///
///   $ip                                                                   ///
///=========================================================================///

// Note: this click is only for 2-interface routers, for larger ones, a new file is needed

require(library /home/neuromancer/Projects/click/examples/elements/routerport.click);

// in-flow until lookupIP table

// Declare ports
rp0 :: RouterPort(DEV $dev0, IN_MAC $in_mac0, OUT_MAC $out_mac0);
rp1 :: RouterPort(DEV $dev1, IN_MAC $in_mac1, OUT_MAC $out_mac1);

iptable :: RouteTable;
lookupIP :: MyLookup(TABLE iptable);
classifyIP :: IPClassifier(ip proto 89, -);

// (simplfied) OSPF processors
nbinfo :: NeighbourInfoBase;
lsinfo :: LSInfoBase;
update :: OSPFUpdate(ID $ip, NBINFOBASE nbinfo, LSINFOBASE lsinfo, IPTABLE iptable);
hellosend :: OSPFSendHello(ID $ip);
hellorecv :: OSPFRecvHello(NBINFOBASE nbinfo, ID $ip);
lssend :: OSPFSendLS(ID $ip, NBINFOBASE nbinfo);
lsrecv :: OSPFRecvLS(LSINFOBASE lsinfo, ID $ip);
classifyOSPF :: Classifier(21/0000, -);
copy :: Tee(2);

    ( rp0 -> Paint(0) -> output
    ; rp1 -> Paint(1) -> output )
 -> CheckIPHeader
 -> classifyIP;

    classifyIP [1]
 -> GetIPAddress(16)
 -> lookupIP;

    classifyIP [0]
 -> classifyOSPF;

    classifyOSPF [0]
 -> hellorecv;

    classifyOSPF [1]
 -> copy;

    copy [0]
 -> CheckPaint(0)
 => ( [0] -> lsrecv
    ; [1] -> rp0 );

    copy [1]
 -> CheckPaint(1)
 => ( [0] -> lsrecv
    ; [1] -> rp1 );

    hellosend
 -> Tee(2)
 => ( [0] -> rp0
    ; [1] -> rp1 );

    lssend
 -> Tee(2)
 => ( [0] -> rp0
    ; [1] -> rp1 );

// out-flow to different ports
//   Note: may need refraction and abstraction

    lookupIP [0]
 -> DropBroadcasts
 -> color0 :: CheckPaint(0) [1]
 -> ipgw0 :: IPGWOptions($ip)
 -> FixIPSrc($ip)
 -> ttl0 :: DecIPTTL
 -> rp0;

    color0 [0] -> ICMPError($ip, redirect) -> lookupIP;
    ipgw0 [1] -> ICMPError($ip, parameterproblem) -> lookupIP;
    ttl0 [1] -> ICMPError($ip, timeexceeded) -> lookupIP;

    lookupIP [1]
 -> DropBroadcasts
 -> color1 :: CheckPaint(1) [1]
 -> ipgw1 :: IPGWOptions($ip)
 -> FixIPSrc($ip)
 -> ttl1 :: DecIPTTL
 -> rp1;

    color1 [0] -> ICMPError($ip, redirect) -> lookupIP;
    ipgw1 [1] -> ICMPError($ip, parameterproblem) -> lookupIP;
    ttl1 [1] -> ICMPError($ip, timeexceeded) -> lookupIP;
