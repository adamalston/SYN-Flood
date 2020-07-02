# SYN Flood Attack

[![License](https://img.shields.io/github/license/adamalston/SYN-Flood?color=black)](LICENSE)

SYN flood is a form of DoS attack in which attackers send many SYN requests to a victim’s TCP port, but the attackers have no intention to finish the 3-way handshake procedure. Attackers either use spoofed IP addresses or do not continue the procedure. Through this attack, attackers can flood the victim’s queue that is used for half-opened connections, i.e. the connections that has finished SYN, SYN-ACK, but has not yet gotten a final ACK back. When this queue is full, the victim cannot take any more connection. 

<p align="center">
  <img src="assets/tcp_syn_flood.png">
</p>

## Network Setup

3 machines are needed. One computer is used as the **attacker**, the second computer is used as the **victim** (e.g., server), and the third computer is used as an **observer** (e.g., client). Set up 3 virtual machines on the same host computer, or set up 2 virtual machines, and then use the host computer as the third computer.

I assume that attackers are on the same physical network as the victims thus simplifying the task of determining TCP sequence numbers and source port numbers. Use sniffer tools to collect the necessary information.

## Attack

Use the `netwox` tool to conduct the attack, and then use a sniffer tool to capture the attacking packets. While the attack is going on, run the `netstat -na` command on the victim machine, and compare the result with that before the attack.

```bash
# check the size of the queue for holding half-open connections
sudo sysctl -q net.ipv4.tcp_max_syn_backlog

# check the current usage of the queue;
# i.e., the number of half-open connections associated with some listening port
netstat -na

# one netwox took that may be useful is tool number 76:
netwox 76 --help
```

**SYN Cookie Countermeasure:**
If it seems the attack is unsuccessful, investigate whether the **SYN cookie** mechanism is turned on. The SYN cookie is a defense mechanism to counter the SYN flooding attack. The mechanism will kick in if the machine detects that it is under the SYN flooding attack.

Use the `sysctl` command to turn on/off the SYN cookie mechanism:

```bash
sudo sysctl -a | grep cookie                 # Display the SYN cookie flag
sudo sysctl -w net.ipv4.tcp_syncookies=0     # turn off SYN cookie
sudo sysctl -w net.ipv4.tcp_syncookies=1     # turn on  SYN cookie
```

---

Thank you for your interest, this project was fun and insightful!
