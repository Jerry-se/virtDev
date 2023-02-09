##

ip link add vxlan0 type vxlan  id 42 dstport 4789 group 239.1.1.1 dev enp4s0
ip link add br0 type bridge
ip link set vxlan0 master br0
ip link set vxlan0 up
ip link set br0 up

virsh net-update default add ip-dhcp-host \
          "<host mac='52:54:00:00:00:01' \
           name='bob' ip='192.168.122.45' />" \
           --live --config

virsh net-update default add ip-dhcp-host "<host mac='52:54:00:fb:55:cf' name='ubuntu18.04' ip='192.168.122.45' />" --live --config

    <interface type='mcast'>
      <mac address='52:54:00:6d:90:01'/>
      <source address='230.0.0.1' port='5558'/>
    </interface>


sudo /home/dbc/jerry/dbc_mining_node/shell/network/create_bridge.sh br1K7ea5 vx1K7ea5 2704965
sudo /home/dbc/jerry/dbc_mining_node/shell/network/start_dhcp.sh br1K7ea5 vx1K7ea5 tapzdyi8py08g 255.255.255.0 192.168.66.1 192.168.66.254 254
