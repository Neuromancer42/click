//
// Created by neuromancer on 11/24/17.
//

#ifndef CLICK_TCPCB_H
#define CLICK_TCPCB_H

#include <click/ipaddress.hh>
#include <clicknet/ip.h>
#include <clicknet/tcp.h>

struct click_tcpip {
  click_ip ip_hdr;
  click_tcp tcp_hdr;
};

#ifndef TCP_NTIMERS
#define TCP_NTIMERS 4
#define TIMER_REXMT     0
#define TIMER_PERSISIT  1
#define TIMER_KEEP      2
#define TIMER_2MSL      3
#endif

struct click_tcpcb {
  // double-linked control blocks
  click_tcpcb *cb_next, *cb_prev, *cb_head;

  // basic connection info
  IPAddress faddr, laddr;
  uint16_t fport, lport;

  // reassembly queue of received segments
  click_tcpip *seg_next, *seg_prev;

  // connection state
  short tcp_state;
  int tcp_timer[TCP_NTIMERS];
  short tcp_rxtshift;
  short tcp_rxtcur;
  short tcp_dup;
  unsigned short tcp_maxseg;
  uint16_t tcp_flags;
  click_tcpip *tcp_temp;

  // send sequence numbers
  tcp_seq_t send_unack;
  tcp_seq_t send_next;
  tcp_seq_t send_urgent;
  tcp_seq_t send_upd_seq;
  tcp_seq_t send_upd_ack;
  tcp_seq_t init_send_seq;
  unsigned long send_window;

  // receive sequence numbers
  unsigned long recv_window;
  tcp_seq_t recv_next;
  tcp_seq_t recv_urgent;
  tcp_seq_t init_recv_seq;

  // other variables
  tcp_seq_t recv_adv_window;
  tcp_seq_t max_sent_seq;

  // congestion control
  uint32_t send_cwnd;
  uint32_t send_ss_thresh;

  // timing variables
  short tcp_idle;
  short tcp_rtt;
  tcp_seq_t tcp_timed_seq;
  short tcp_rtt_smmothed;
  short tcp_rtt_variance;
  short tcp_rtt_min;
  long max_send_window;

};

#define TCP_CLOSED         0
#define TCP_LISTEN         1
#define TCP_SYN_SENT       2
#define TCP_SYN_RECEIVED   3
#define TCP_ESTABLISHED    4
#define TCP_CLOSE_WAIT     5
#define TCP_FIN_WAIT_1     6
#define TCP_CLOSING        7
#define TCP_LAST_ACK       8
#define TCP_FIN_WAIT_2     9
#define TCP_TIME_WAIT     10

// modular arithmetic with sequence numbers
#define SEQ_LT(a,b) ((int)((a)-(b)) <  0)
#define SEQ_LE(a,b) ((int)((a)-(b)) <= 0)
#define SEQ_GT(a,b) ((int)((a)-(b)) >  0)
#define SEQ_GE(a,b) ((int)((a)-(b)) >= 0)

// timing constants (in msec)

#ifndef TCP_TIMING_MSL       // maximum segment lifetime
#define TCP_TIMING_MSL 30000
#endif

#ifndef TCP_TIMING_REXMT_MIN
#define TCP_TIMING_REXMT_MIN 1000
#endif

#ifndef TCP_TIMING_REXMT_MAX
#define TCP_TIMING_REXMT_MAX 64000
#endif

#ifndef TCP_TIMING_PERSIST_MIN
#define TCP_TIMING_PERSIST_MIN 5000
#endif

#ifndef TCP_TIMING_PERSIST_MAX
#define TCP_TIMING_PERSIST_MAX 60000
#endif

#ifndef TCP_TIMING_KEEP_INIT
#define TCP_TIMING_KEEP_INIT 75000
#endif

#ifndef TCP_TIMING_KEEP_IDLE
#define TCP_TIMING_KEEP_IDLE 7200000
#endif

#ifndef TCP_TIMING_KEEP_INTERVAL
#define TCP_TIMING_KEEP_INTERVAL 75000
#endif

#endif //CLICK_TCPCB_H
