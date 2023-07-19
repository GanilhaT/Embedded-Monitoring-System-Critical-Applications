// First Wi-Fi interface (STA mode) configuration
#define APP_IF0_NAME "wlan0"
#define APP_IF0_HOST_NAME "sftp-client-demo"
#define APP_IF0_MAC_ADDR "00-00-00-00-00-00"

#define APP_IF0_USE_DHCP_CLIENT ENABLED
#define APP_IF0_IPV4_HOST_ADDR "192.168.0.20"
#define APP_IF0_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IF0_IPV4_DEFAULT_GATEWAY "192.168.0.254"
#define APP_IF0_IPV4_PRIMARY_DNS "8.8.8.8"
#define APP_IF0_IPV4_SECONDARY_DNS "8.8.4.4"

#define APP_IF0_USE_SLAAC ENABLED
#define APP_IF0_IPV6_LINK_LOCAL_ADDR "fe80::32:1"

// Second Wi-Fi interface (AP mode) configuration
#define APP_IF1_NAME "wlan1"
#define APP_IF1_HOST_NAME "sftp-client-demo"
#define APP_IF1_MAC_ADDR "00-00-00-00-00-00"

#define APP_IF1_USE_DHCP_SERVER ENABLED
#define APP_IF1_IPV4_HOST_ADDR "192.168.8.1"
#define APP_IF1_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IF1_IPV4_DEFAULT_GATEWAY "0.0.0.0"
#define APP_IF1_IPV4_PRIMARY_DNS "0.0.0.0"
#define APP_IF1_IPV4_SECONDARY_DNS "0.0.0.0"
#define APP_IF1_IPV4_ADDR_RANGE_MIN "192.168.8.10"
#define APP_IF1_IPV4_ADDR_RANGE_MAX "192.168.8.99"

#define APP_IF1_USE_ROUTER_ADV ENABLED
#define APP_IF1_IPV6_LINK_LOCAL_ADDR "fe80::32:2"
#define APP_IF1_IPV6_PREFIX "fd00:1:2:3::"
#define APP_IF1_IPV6_PREFIX_LENGTH 64
#define APP_IF1_IPV6_GLOBAL_ADDR "fd00:1:2:3::32:2"

// Wi-Fi parameters (STA mode)
#define APP_WIFI_STA_SSID "NOS_Internet_Movel_D996"
#define APP_WIFI_STA_PASSWORD "n98e3854"

// Wi-Fi parameters (AP mode)
#define APP_WIFI_AP_SSID "NOS_Internet_Movel_D996"
#define APP_WIFI_AP_PASSWORD "n98e3854"

// Application configuration
#define APP_SFTP_SERVER_NAME "79.169.204.82"
#define APP_SFTP_SERVER_PORT 226
#define APP_SFTP_USERNAME "ganilha"
#define APP_SFTP_PASSWORD "16121995"
#define APP_SFTP_TEMP_FILENAME "/home/ganilha/kibana/temp-"
#define APP_SFTP_FILENAME "/home/ganilha/kibana/"
#define LOG_FILE_DIR "/sdcard"

int ENDSWITH(const char *str, const char *suffix);

esp_err_t RESET_WIFI();

void MONITOR_SYSTEM();