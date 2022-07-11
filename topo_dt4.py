from mininet.topo import Topo
from mininet.net import Mininet
from mininet.log import setLogLevel, info
from mininet.cli import CLI
from mininet.node import Controller, RemoteController, OVSController
from mininet.node import Host, Node

def network():
    net = Mininet(topo=None, build=False)

    info('*** Adding Host\n')
    h1 = net.addHost('h1', cls=Host, ip='10.0.0.11/24', defaultRoute=None)
    h2 = net.addHost('h2', cls=Host, ip='10.0.1.12/24', defaultRoute=None)
    
    info('*** Adding Switch\n')
    s1 = net.addHost('s1', cls=Node, ip='0.0.0.0')
    
    info('*** Add links\n')
    net.addLink(h1, s1)
    net.addLink(h1, s1)
    net.addLink(s1, h2)

    info('*** Starting network\n')
    net.build()

    info('*** Starting controllers\n')
    for controller in net.controllers:
        controller.start()
    
    info('*** Posting config for host and switchs\n')
    h1.cmd('sysctl -w net.vrf.strict_mode=1')
    h2.cmd('sysctl -w net.vrf.strict_mode=1')
    s1.cmd('sysctl -w net.vrf.strict_mode=1')

    h1.cmd('sysctl -w net.ipv4.ip_forward=1')
    h2.cmd('sysctl -w net.ipv4.ip_forward=1')
    s1.cmd('sysctl -w net.ipv4.ip_forward=1')

    h1.cmd('sysctl -w net.ipv6.conf.all.forwarding=1')
    h2.cmd('sysctl -w net.ipv6.conf.all.forwarding=1')
    s1.cmd('sysctl -w net.ipv6.conf.all.forwarding=1')

    h1.cmd('sysctl -w net.ipv6.conf.all.keep_addr_on_down=1')
    h2.cmd('sysctl -w net.ipv6.conf.all.keep_addr_on_down=1')
    s1.cmd('sysctl -w net.ipv6.conf.all.keep_addr_on_down=1')

    # h1.cmd('ifconfig h1-eth0 10.0.0.11/24')
    # h1.cmd('ifconfig h1-eth1 10.0.0.13/24')
    # s1.cmd('ifconfig s1-eth0 10.0.0.12/24')
    # s1.cmd('ifconfig s1-eth1 10.0.0.14/24')

    h1.cmd('ip -6 addr add 4000::11/64 dev h1-eth0')
    h1.cmd('ip -6 addr add 4000::13/64 dev h1-eth1')
    s1.cmd('ip -6 addr add 4000::12/64 dev s1-eth0')
    s1.cmd('ip -6 addr add 4000::14/64 dev s1-eth1')

    s1.cmd('ip -6 addr add 3000::1/64 dev s1-eth2')
    h2.cmd('ip -6 addr add 3000::2/64 dev h2-eth0')
    
    h1.cmd('ip link add Vrf101 type vrf table 123')
    h1.cmd('ip link set dev Vrf101 up')
    h1.cmd('ip link add Vrf102 type vrf table 124')
    h1.cmd('ip link set dev Vrf102 up')
    h1.cmd('ip link set dev h1-eth0 master Vrf101')
    h1.cmd('ip link set dev h1-eth1 master Vrf102')

    s1.cmd('ip link add Vrf101 type vrf table 123')
    s1.cmd('ip link set dev Vrf101 up')
    s1.cmd('ip link add Vrf102 type vrf table 124')
    s1.cmd('ip link set dev Vrf102 up')
    s1.cmd('ip link set dev s1-eth0 master Vrf101')
    s1.cmd('ip link set dev s1-eth1 master Vrf102')

    s1.cmd('ip -6 route add 2000::1/128 encap seg6local action End.DT6 vrftable 123 dev Vrf101')
    h2.cmd('ip -6 route add 2000::1/128 via 3000::1 dev h2-eth0')
    h2.cmd('ip -6 route add 4000::1/64 encap seg6 mode encap segs 2000::1 dev h2-eth0')
    

    CLI(net)
    net.stop()

if __name__ == '__main__':
    setLogLevel('info')
    network()
