#include "mbed.h"

#define SERVER_IP "192.168.101.88"
#define SERVER_PORT 50000

UnbufferedSerial pc(CONSOLE_TX, CONSOLE_RX, 115200) ;
WiFiInterface *wifi ;

TCPSocket socket ;
Thread sock_thread ;

char rxBuf_pc[80] ;
char txBuf_pc[80] ;

int index = 0 ;
volatile int flag ;

void rx_thread()
{
    while (true) 
    {
        char rxBuf_socket[80] ;
        int rxCount = socket.recv(rxBuf_socket, sizeof(rxBuf_socket) - 1) ;
        if (rxCount > 0) 
        {
            rxBuf_socket[rxCount] = '\0' ;
            sprintf(txBuf_pc, "\r\nRX data: (%d) %s\r\n", rxCount-2, rxBuf_socket) ;
            pc.write(txBuf_pc, strlen(txBuf_pc)) ;
        }
    }
}

void rx_cb(void)
{
    char ch ;
    pc.read(&ch, 1) ;
    pc.write(&ch, 1) ;
    rxBuf_pc[index++] = ch ;
    if (ch == '\r') 
    {
        pc.write("\n", 1) ;
        rxBuf_pc[--index] = '\0' ;   // end of string
        index = 0 ;
        flag = 1 ;
    }
}

int main()
{
    SocketAddress sockAddr ;
    SocketAddress serverAddr(SERVER_IP, SERVER_PORT) ; 

    sprintf(txBuf_pc, "\r\nWiFi TCP Client example\r\n") ;
    pc.write(txBuf_pc, strlen(txBuf_pc)) ;
    pc.attach(rx_cb) ;

    wifi = WiFiInterface::get_default_instance() ;
    if (!wifi) 
    {
        sprintf(txBuf_pc, "ERROR: No WiFiInterface found.\n") ;
        pc.write(txBuf_pc, strlen(txBuf_pc)) ;
        while (1) { } ;
    }

    sprintf(txBuf_pc, "Connecting to %s...\r\n", MBED_CONF_APP_WIFI_SSID) ;
    pc.write(txBuf_pc, strlen(txBuf_pc)) ;

    int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2) ;
    if (ret != 0) 
    {
        sprintf(txBuf_pc, "Connection error!!\r\n") ;
        pc.write(txBuf_pc, strlen(txBuf_pc)) ;
        return -1
    }
    sprintf(txBuf_pc, "Success!!\r\n") ;
    pc.write(txBuf_pc, strlen(txBuf_pc)) ;

    sprintf(txBuf_pc, "RSSI: %d\r\n", wifi->get_rssi()) ;
    pc.write(txBuf_pc, strlen(txBuf_pc)) ;

    sprintf(txBuf_pc, "MAC: %s\r\n", wifi->get_mac_address()) ;
    pc.write(txBuf_pc, strlen(txBuf_pc)) ;

    wifi->get_ip_address(&sockAddr) ;
    sprintf(txBuf_pc, "IP: %s, ", sockAddr.get_ip_address()) ;
    pc.write(txBuf_pc, strlen(txBuf_pc)) ;

    wifi->get_netmask(&sockAddr) ;
    sprintf(txBuf_pc, "Netmask: %s, ", sockAddr.get_ip_address()) ;
    pc.write(txBuf_pc, strlen(txBuf_pc)) ;

    wifi->get_gateway(&sockAddr) ;
    sprintf(txBuf_pc, "Gateway: %s\r\n", sockAddr.get_ip_address()) ;
    pc.write(txBuf_pc, strlen(txBuf_pc)) ;

    // Open a TCP socket on the network interface
    socket.open(wifi) ;

    // create a TCP connection to the server
    int response = socket.connect(serverAddr) ;
    if (response != 0) 
    {
        sprintf(txBuf_pc, "Error connecting: %d\r\n", response) ;
        pc.write(txBuf_pc, strlen(txBuf_pc)) ;
        socket.close() ;
        return -1 ;
    }

    sock_thread.start(callback(rx_thread)) ;

    while (true) 
    {
        flag = 0 ;
        sprintf(txBuf_pc, "Enter characters to send to the server: ") ;
        pc.write(txBuf_pc, strlen(txBuf_pc)) ;

        while (flag != 1) {}
    }


    socket.close() ;
    wifi->disconnect() ;

    sock_thread.join() ;
    sprintf(txBuf_pc, "RX sock_thread joined!!\r\n") ; 
    pc.write(txBuf_pc, strlen(txBuf_pc)) ;
    sprintf(txBuf_pc, "\nDone\r\n") ;
    pc.write(txBuf_pc, strlen(txBuf_pc)) ;

    while (1) { }
}
