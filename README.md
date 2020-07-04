# SYN Flood

[![License](https://img.shields.io/github/license/adamalston/SYN-Flood?color=black)](LICENSE)

A SYN flood is a form of DoS attack in which an attacker sends a succession of `SYN` requests to a target's server in an attempt to consume enough server resources to make the system unresponsive to legitimate traffic<sup id="r1">[[1]](#1)</sup>.

> A `SYN` request and a `SYN` packet are the same things

## How does a SYN flood attack work?

SYN flood attacks work by exploiting the handshake process of a TCP connection. Under normal conditions, TCP exhibits three distinct processes in order to make a connection (Figure [1a](#ab)).

1. The client requests a connection by sending a `SYN` (*synchronize*) packet to the server.
2. The server then responds with a `SYN-ACK` packet, in order to `ACK` (*acknowledge*) the communication.
3. The client sends an `ACK` packet to acknowledge the receipt of the packet from the server and the connection is established.

After completing this sequence of packet sending and receiving, the TCP connection is open and able to send and receive data. This is called the TCP three-way handshake. This technique is the foundation for every connection established using TCP.

To create a DoS, an attacker exploits the fact that after an initial `SYN` packet has been received, the server will respond with one or more `SYN-ACK` packets and wait for the final step in the handshake. Cloudflare<sup id="r2">[[2]](#2)</sup> describes how it works (Figure [1b](#ab)):

1. The attacker sends a high volume of `SYN` packets to the targeted server, often with spoofed IP addresses.
2. The server then responds to each one of the connection requests and leaves an open port ready to receive the response.
3. While the server waits for the final `ACK` packet, which never arrives, the attacker continues to send more `SYN` packets. The arrival of each new `SYN` packet causes the server to temporarily maintain a new open port connection for a certain length of time, and once all the available ports have been utilized the server is unable to function normally.

<p align="center" id="ab">
  <img src="assets/tcp_syn_flood.png">  
</p>

## Network Setup

The attack in this repository was conducted in a VM. Three machines are needed to carry out the attack. One machine is used as the **attacker**, another machine is used as the **victim** (i.e. the server), and the third machine is used as an **observer** (i.e. the client). 

To set up the attack, three VMs can be set up on the same host computer. Alternatively, two VMs can be set up on a host computer with the host machine itself acting as the third computer.

This attack was implemented under the assumption that the attackers are on the same physical network as the victims - thus simplifying the task of determining TCP sequence numbers and source port numbers. Sniffer tools can be used to collect the necessary information.

## Attack

The attack in this repository was conducted using the `netwox` tool. Then, a sniffer tool such as Wireshark<sup id="r3">[[3]](#3)</sup> was used to capture the attacking packets. While the attack was ongoing, `netstat -na` was run on the victim machine to compare the result with that from before the attack.

```bash
# check the size of the queue for holding half-open connections
sudo sysctl -q net.ipv4.tcp_max_syn_backlog

# check the current usage of the queue;
# i.e., the number of half-open connections associated with some listening port
netstat -na

# one netwox tool that may be useful is tool number 76: synflood
netwox 76 --help
```

## Countermeasures
**SYN Cookie:**

The attack in this repository was conducted by manipulating the SYN cookie. 
If it seems the attack is unsuccessful, investigate whether the **SYN cookie** mechanism is turned on. The SYN cookie is a defense mechanism to counter the SYN flood attack. The mechanism will kick in if the machine detects that it is under attack.

Use the `sysctl` command to turn on/off the SYN cookie mechanism:

```bash
sudo sysctl -a | grep cookie                 # Display the SYN cookie flag
sudo sysctl -w net.ipv4.tcp_syncookies=0     # turn off SYN cookie
sudo sysctl -w net.ipv4.tcp_syncookies=1     # turn on SYN cookie
```

---

### References

1. [^](#r1) <a href="CERT Advisory CA-1996-21 TCP SYN Flooding and IP Spoofing Attacks" id="1">"CERT Advisory CA-1996-21 TCP SYN Flooding and IP Spoofing Attacks"</a> <i>Carnegie Mellon University</i>

2. [^](#r2) <a href="https://www.cloudflare.com/learning/ddos/syn-flood-ddos-attack/" id="2">"SYN Flood Attack"</a> <i>Cloudflare</i>

3. [^](#r3) <a href="https://www.wireshark.org/index.html#aboutWS" id="3">"About Wireshark"</a> <i>Wireshark Foundation</i>

---

Thank you for your interest, this project was fun and insightful!
