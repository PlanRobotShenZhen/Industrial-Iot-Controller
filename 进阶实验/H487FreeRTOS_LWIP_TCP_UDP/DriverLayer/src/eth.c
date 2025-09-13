#include "eth.h"
#include "usart.h"
#include "userdef.h"

#include <lwip/opt.h>
#include <lwip/arch.h>
#include "tcpip.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "ethernetif.h"
#include "netif/ethernet.h"
#include "lwip/def.h"
#include "lwip/stats.h"
#include "lwip/etharp.h"
#include "lwip/ip.h"
#include "lwip/timeouts.h"

struct netif sNetif;
ip4_addr_t ipaddr;
ip4_addr_t netmask;
ip4_addr_t gw;

void NetworkInitialize(void)
{
    /* IP addresses initialization */
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
    
    /* Initializes the LwIP stack and starts the tcpip_thread thread */
    tcpip_init(NULL, NULL);

    /* Add the network interface (IPv4/IPv6) */
    netif_add(&sNetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

    /* Registers the default network interface */
    netif_set_default(&sNetif);

    if (netif_is_link_up(&sNetif))
    {
        /* When the netif is fully configured this function must be called */
        netif_set_up(&sNetif);
    }
    else
    {
        /* When the netif link is down this function must be called */
        netif_set_down(&sNetif);
    }
    
    /* Link successful, print local IP information */
    if (netif_is_up(&sNetif))
    {
        LOG("Network connection successful\r\n");
        LOG("Local IP: %s\r\n", ipaddr_ntoa(&sNetif.ip_addr));
        LOG("Netmask: %s\r\n", ipaddr_ntoa(&sNetif.netmask));
        LOG("Gateway: %s\r\n", ipaddr_ntoa(&sNetif.gw));
    }
    else
    {
        USART_Printf(USART_LOG, "Network not connected\n");
    }
}



