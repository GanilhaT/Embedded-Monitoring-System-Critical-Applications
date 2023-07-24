// Dependencies
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "core/net.h"
#include "drivers/wifi/esp32_wifi_driver.h"
#include "dhcp/dhcp_client.h"
#include "ipv6/slaac.h"
#include "sftp/sftp_client.h"
#include "ssh/ssh_key_verify.h"
#include "hardware/esp32/esp32_crypto.h"
#include "rng/trng.h"
#include "rng/yarrow.h"
#include "debug.h"
#include <dirent.h>

#include "sntp.h"
#include "utils.h"

// List of trusted host keys
const char_t *trustedHostKeys[] =
    {
        // Rebex RSA host key
        "ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABgQCzF//E0I0Jq8QN6zm+1Db9WfBkXKtIr1DuJ/wdh3oBrjf9/DsVuJXxXbFBJEg+pO/FfBBTLcjDT0UkSFD/v7+ewjZeN08Ji32MP8QbGCj5CwnCGy6lmLjFpLDqwoKytNKsVmLhFUkEZkJg5YRY79trS1gBOTrcoE8lEymfCYJx4GRa5biEs1vTqQFT6JWqQKyqwxQPbmgU4nD862y2wljbzsao5LMzxkGCnzbHu7ekv0Z1G1xjR2DNpOyHVqFH4X7UR4jFyysQrF6I42s4OJN19tJWc564K2vHDIV9jwHsvtqQkFQ5BAUJFab8dmFpWCk8xIzTBEvcaQ7JV3j2QARP9OLx6d407oVDqGU877d2LA1L1JhCxDJn1CaJ4zbx2eUUIxILU1q4IZmaZIIPEs4qcnRknCv0iVWSr+SA4nchsIQN92ybcQfRVhURLOFGQEwBNhidgmR5urjHQHmZ6Imt8yNE1PghtvgzrDOC1TPsY5KU6+5yeSQsBXKxDRcim00=",

        // Rebex NIST P-256 host key
        "ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBJUIrOCAht2uFV2IkBppqw1nbDTdGFaCO/RxfKUoQW9g+Ith83DH4edrcKUiocTx/MxC9Arrj4nicHiK7bOqma0=",

        // Rebex Ed25519 host key
        "ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAIC2LvW2Y0E+GiOCPPzk3S69nsZG/J6lsgbOWIrFR5Awc"};

// Global variables
DhcpClientSettings dhcpClientSettings;
DhcpClientContext dhcpClientContext;
DhcpServerSettings dhcpServerSettings;
DhcpServerContext dhcpServerContext;
SlaacSettings slaacSettings;
SlaacContext slaacContext;
NdpRouterAdvSettings ndpRouterAdvSettings;
NdpRouterAdvPrefixInfo ndpRouterAdvPrefixInfo[1];
NdpRouterAdvContext ndpRouterAdvContext;
SftpClientContext sftpClientContext;
YarrowContext yarrowContext;
uint8_t seed[32];

// Forward declaration of functions
error_list wifiStaInterfaceInit(void);
error_list wifiApInterfaceInit(void);

esp_err_t wifiConnect(void);
esp_err_t wifiEnableAp(void);

void wifiEventHandler(void *arg, esp_event_base_t eventBase,
                      int32_t eventId, void *eventData);

/**
 * @brief I/O initialization
 *
 */
void ioInit(void)
{
    // Configure red LED (GPIO0)
    gpio_pad_select_gpio(0);
    gpio_set_direction(0, GPIO_MODE_OUTPUT);
    gpio_set_level(0, 0);

    // Configure green LED (GPIO2)
    gpio_pad_select_gpio(2);
    gpio_set_direction(2, GPIO_MODE_OUTPUT);
    gpio_set_level(2, 0);

    // Configure blue LED (GPIO4)
    gpio_pad_select_gpio(4);
    gpio_set_direction(4, GPIO_MODE_OUTPUT);
    gpio_set_level(4, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////// SSH TASK ////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Server's host key verification callback
 *
 * @param[in] connection Pointer to the SSH connection
 * @param[in] hostKey Pointer to the server's host key
 * @param[in] hostKeyLen Length of the host key, in bytes
 *
 * @return Error code
 *
 */
error_list sftpClientHostKeyVerifyCallback(SshConnection *connection,
                                           const uint8_t *hostKey, size_t hostKeyLen)
{
    error_list error;
    uint_t i;

    // Debug message
    TRACE_INFO("SFTP Client: Public key verification callback\r\n");

    // Initialize status code
    error = ERROR_INVALID_KEY;

    // Loop through the list of known host keys
    for (i = 0; i < arraysize(trustedHostKeys) && error; i++)
    {
        // Check whether the host key is trusted
        error = sshVerifyHostKey(hostKey, hostKeyLen, trustedHostKeys[i],
                                 strlen(trustedHostKeys[i]));
    }

    // Return status code
    return error;
}

/**
 * @brief SSH initialization callback
 *
 * @param[in] context Pointer to the SFTP client context
 * @param[in] sshContext Pointer to the SSH context
 *
 * @return Error code
 *
 */
error_list sftpClientSshInitCallback(SftpClientContext *context,
                                     SshContext *sshContext)
{
    error_list error;

    // Debug message
    TRACE_INFO("SFTP Client: SSH initialization callback\r\n");

    // Set the PRNG algorithm to be used
    error = sshSetPrng(sshContext, YARROW_PRNG_ALGO, &yarrowContext);
    // Any error to report?
    if (error)
        return error;

    // Set the user name to be used for authentication
    error = sshSetUsername(sshContext, APP_SFTP_USERNAME);
    // Any error to report?
    if (error)
        return error;

    // Set the password to be used for authentication
    error = sshSetPassword(sshContext, APP_SFTP_PASSWORD);
    // Any error to report?
    if (error)
        return error;

    // Register host key verification callback function
    error = sshRegisterHostKeyVerifyCallback(sshContext,
                                             sftpClientHostKeyVerifyCallback);

    // Any error to report?
    if (error)
        return error;

    // Successful processing
    return NO_ERROR;
}

/**
 * @brief SFTP client test routine
 *
 * @note -
 *
 * @return Error code
 *
 */
error_list sftpClientTest(void)
{
    error_list error;
    IpAddr ipAddr;

    // Initialize SFTP client context
    sftpClientInit(&sftpClientContext);

    // Start of exception handling block
    do
    {
        // Debug message
        TRACE_INFO("\r\n\r\nResolving server name...\r\n");

        // Resolve SFTP server name
        error = getHostByName(NULL, APP_SFTP_SERVER_NAME, &ipAddr, 0);
        // Any error to report?
        if (error)
        {
            // Debug message
            TRACE_INFO("Failed to resolve server name!\r\n");
            break;
        }
        // Register SSH initialization callback
        TRACE_INFO("Register SSH initialization callback \r\n");
        error = sftpClientRegisterSshInitCallback(&sftpClientContext,
                                                  sftpClientSshInitCallback);
        // Any error to report?
        if (error)
            break;

        // Set timeout value for blocking operations
        error = sftpClientSetTimeout(&sftpClientContext, 20000);
        // Any error to report?
        if (error)
            break;

        // Debug message
        TRACE_INFO("Connecting to SFTP server %s...\r\n",
                   ipAddrToString(&ipAddr, NULL));

        // Connect to the SFTP server
        error = sftpClientConnect(&sftpClientContext, &ipAddr,
                                  APP_SFTP_SERVER_PORT);
        // Any error to report?
        if (error)
        {
            // Debug message
            TRACE_INFO("Failed to connect to SFTP server!\r\n");
            break;
        }

        TRACE_INFO("Opening log directory\r\n");

        DIR *dir;
        const struct dirent *ent;

        // Open the directory
        if ((dir = opendir(LOG_FILE_DIR)) != NULL)
        {
            TRACE_INFO("Loopping through each file\r\n");
            while ((ent = readdir(dir)) != NULL)
            {
                if (ent->d_type == DT_REG && ENDSWITH(ent->d_name, ".txt"))
                {
                    char temp_filename[100];
                    // Initialize string
                    strcpy(temp_filename, "");
                    strcat(temp_filename, APP_SFTP_TEMP_FILENAME);
                    strcat(temp_filename, "cardioid");
                    strcat(temp_filename, ent->d_name);
                    TRACE_INFO("Creating File with %s - %s...\r\n", temp_filename,
                               ipAddrToString(&ipAddr, NULL));
                    error = sftpClientOpenFile(&sftpClientContext, temp_filename,
                                               SSH_FXF_CREAT);
                    // Any error to report?
                    if (error)
                    {
                        TRACE_INFO("Error while opening File %s...\r\n",
                                   ipAddrToString(&ipAddr, NULL));
                        break;
                    }

                    // Close file
                    error = sftpClientCloseFile(&sftpClientContext);
                    // Any error to report?
                    if (error)
                    {
                        TRACE_INFO("Error while closing File %s...\r\n",
                                   ipAddrToString(&ipAddr, NULL));
                        break;
                    }

                    // sleep(1000);

                    TRACE_INFO("Opening File %s - %s...\r\n", temp_filename,
                               ipAddrToString(&ipAddr, NULL));

                    error = sftpClientOpenFile(&sftpClientContext, temp_filename,
                                               SSH_FXF_WRITE);
                    // Any error to report?
                    if (error)
                    {
                        TRACE_INFO("Error while opening File %s...\r\n",
                                   ipAddrToString(&ipAddr, NULL));
                        break;
                    }

                    // Write to file
                    FILE *file;
                    size_t write_n;
                    char write_buffer[1024];

                    char logfilepath[1024];
                    snprintf(logfilepath, sizeof(logfilepath), "%s/%s", LOG_FILE_DIR, ent->d_name);
                    file = fopen(logfilepath, "rb");
                    if (file == NULL)
                    {
                        TRACE_INFO("Error opening the local file %s \n", logfilepath);
                        break;
                    }

                    // Read the file line by line
                    while (fgets(write_buffer, sizeof(write_buffer), file))
                    {
                        error = sftpClientWriteFile(&sftpClientContext, &write_buffer, strlen(write_buffer), &write_n, 0);
                        // Any error to report?
                        if (error)
                        {
                            TRACE_INFO("Error while writing to File %s...\r\n",
                                       ipAddrToString(&ipAddr, NULL));
                            break;
                        }
                    }
                    // Close the file
                    fclose(file);

                    // Terminate the string with a line feed
                    TRACE_INFO("\r\n");

                    // Close file
                    error = sftpClientCloseFile(&sftpClientContext);
                    // Any error to report?
                    if (error)
                    {
                        TRACE_INFO("Error while closing File %s...\r\n",
                                   ipAddrToString(&ipAddr, NULL));
                        break;
                    }

                    // Open the specified file for reading
                    char filename[100];
                    // Initialize string
                    strcpy(filename, "");
                    strcat(filename, APP_SFTP_FILENAME);
                    strcat(filename, "cardioid");
                    strcat(filename, ent->d_name);

                    TRACE_INFO("Renaming File %s to %s...\r\n", temp_filename, filename,
                               ipAddrToString(&ipAddr, NULL));
                    error = sftpClientRenameFile(&sftpClientContext, temp_filename, filename);
                    if (error)
                    {
                        TRACE_INFO("Error renaming File %s...\r\n",
                                   ipAddrToString(&ipAddr, NULL));
                        break;
                    }

                    // Attempt to delete the file
                    strcpy(filename, "");
                    strcat(filename, LOG_FILE_DIR);
                    strcat(filename, "/");
                    strcat(filename, ent->d_name);
                    if (remove(filename) == 0)
                    {
                        TRACE_INFO("Log file deleted %s \n", filename);
                    }
                    else
                    {
                        TRACE_INFO("Failed to delete file %s \n", filename);
                    }
                }
            }
        }
        closedir(dir);

        // Gracefully disconnect from the SFTP server
        sftpClientDisconnect(&sftpClientContext);

        // Debug message
        TRACE_INFO("Connection closed\r\n");

        // End of exception handling block
    } while (0);

    // Release SFTP client context
    sftpClientDeinit(&sftpClientContext);

    // Return status code
    return error;
}

/**
 * @brief SSH INIT
 *
 * @note -
 *
 */
void SSH_INIT()
{
    /*
    OsTaskId taskId;
    // Create user task
    taskId = osCreateTask("User", userTask, NULL, 750, OS_TASK_PRIORITY_NORMAL);
    // Failed to create the task?
    if (taskId == OS_INVALID_TASK_ID)
    {
        // Debug message
        TRACE_ERROR("Failed to create task!\r\n");
    }
    */

    osDelayTask(5000);
    while (1)
    {
        // SFTP client test routine
        error_list error = sftpClientTest();

        if (error)
        {
            osDelayTask(1000);
        }
        else
        {
            break;
        }
    }

    dhcpClientRelease(&dhcpClientContext);
    dhcpClientRelease(&dhcpServerContext);
    netStopInterface(&netInterface[0]);
    netStopInterface(&netInterface[1]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////// WIFI MANAGEMENT /////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Create Wi-Fi network (STA mode)
 *
 * @note -
 *
 * @return Error code
 *
 */
esp_err_t wifiConnect(void)
{
    esp_err_t ret;
    wifi_config_t config;

    // Debug message
    TRACE_INFO("ESP32: Connecting to Wi-Fi network (%s)...\r\n", APP_WIFI_STA_SSID);

    // Initialize configuration structure
    memset(&config, 0, sizeof(wifi_config_t));

    // Set Wi-Fi parameters
    strcpy((char_t *)config.sta.ssid, APP_WIFI_STA_SSID);
    strcpy((char_t *)config.sta.password, APP_WIFI_STA_PASSWORD);

    // Set Wi-Fi operating mode
    ret = esp_wifi_set_mode(WIFI_MODE_STA);

    // Check status code
    if (ret == ESP_OK)
    {
        // Configure STA interface
        ret = esp_wifi_set_config(ESP_IF_WIFI_STA, &config);
    }

    // Check status code
    if (ret == ESP_OK)
    {
        // Start STA interface
        ret = esp_wifi_start();
    }

    // Return status code
    return ret;
}

/**
 * @brief Create Wi-Fi network (AP mode)
 *
 * @note -
 *
 * @return Error code
 *
 */
esp_err_t wifiEnableAp(void)
{
    esp_err_t ret;
    wifi_config_t config;

    // Debug message
    TRACE_INFO("ESP32: Creating Wi-Fi network (%s)...\r\n", APP_WIFI_AP_SSID);

    // Initialize configuration structure
    memset(&config, 0, sizeof(wifi_config_t));

    // Set Wi-Fi parameters
    strcpy((char_t *)config.ap.ssid, APP_WIFI_AP_SSID);
    strcpy((char_t *)config.ap.password, APP_WIFI_AP_PASSWORD);
    config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    config.ap.max_connection = 4;

    // Set Wi-Fi operating mode
    ret = esp_wifi_set_mode(WIFI_MODE_AP);

    // Check status code
    if (ret == ESP_OK)
    {
        // Configure AP interface
        ret = esp_wifi_set_config(ESP_IF_WIFI_AP, &config);
    }

    // Check status code
    if (ret == ESP_OK)
    {
        // Start AP interface
        ret = esp_wifi_start();
    }

    // Return status code
    return ret;
}

/**
 * @brief Wi-Fi event handler
 *
 * @note -
 *
 * @param[in] arg User-specific parameter
 * @param[in] eventBase Event base
 * @param[in] eventId Event identifier
 * @param[in] eventData Event-specific data
 *
 */
void wifiEventHandler(void *arg, esp_event_base_t eventBase,
                      int32_t eventId, void *eventData)
{
    // Debug message
    TRACE_INFO("ESP32: Wi-Fi event handler (event = %d)\r\n", eventId);

    // Check event identifier
    if (eventId == WIFI_EVENT_STA_START)
    {
        // Debug message
        TRACE_INFO("ESP32: Station started\r\n");

        // Connect station to the AP
        esp_wifi_connect();
    }
    else if (eventId == WIFI_EVENT_STA_CONNECTED)
    {
        // Debug message
        TRACE_INFO("ESP32: Station connected\r\n");
    }
    else if (eventId == WIFI_EVENT_STA_DISCONNECTED)
    {
        // Debug message
        TRACE_INFO("ESP32: Station disconnected\r\n");

        // Reconnect station to the AP
        esp_wifi_connect();
    }
    else if (eventId == WIFI_EVENT_AP_STACONNECTED)
    {
        MacAddr macAddr;
        wifi_event_ap_staconnected_t *event;

        // Point the event-specific data
        event = (wifi_event_ap_staconnected_t *)eventData;
        // Retrieve the MAC address of the station
        macCopyAddr(&macAddr, event->mac);

        // Debug message
        TRACE_INFO("ESP32: Station %s joining AP\r\n",
                   macAddrToString(&macAddr, NULL));
    }
    else if (eventId == WIFI_EVENT_AP_STADISCONNECTED)
    {
        MacAddr macAddr;
        wifi_event_ap_stadisconnected_t *event;

        // Point the event-specific data
        event = (wifi_event_ap_stadisconnected_t *)eventData;
        // Retrieve the MAC address of the station
        macCopyAddr(&macAddr, event->mac);

        // Debug message
        TRACE_INFO("ESP32: Station %s leaving AP\r\n",
                   macAddrToString(&macAddr, NULL));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////// WIFI /////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief -
 *
 * @note -
 *
 */
void WIFI_INIT()
{
    // Force disconnection
    RESET_WIFI();

    error_list error;

    // Start-up message
    TRACE_INFO("\r\n");
    TRACE_INFO("***********************************\r\n");
    TRACE_INFO("*** CycloneSSH SFTP Client Init ***\r\n");
    TRACE_INFO("***********************************\r\n");
    TRACE_INFO("Target: ESP32\r\n");
    TRACE_INFO("\r\n");

    // Configure I/Os
    // ioInit();

    // Initialize NVS memory
    nvs_flash_init();

    // Create default event loop
    esp_event_loop_create_default();

    // Register Wi-Fi event handler
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                               wifiEventHandler, NULL);

    // Initialize hardware cryptographic accelerator
    error = esp32CryptoInit();
    // Any error to report?
    if (error)
    {
        // Debug message
        TRACE_ERROR("Failed to initialize hardware crypto accelerator!\r\n");
    }

    // Generate a random seed
    error = trngGetRandomData(seed, sizeof(seed));
    // Any error to report?
    if (error)
    {
        // Debug message
        TRACE_ERROR("Failed to generate random data!\r\n");
    }

    // PRNG initialization
    error = yarrowInit(&yarrowContext);
    // Any error to report?
    if (error)
    {
        // Debug message
        TRACE_ERROR("Failed to initialize PRNG!\r\n");
    }

    // Properly seed the PRNG
    error = yarrowSeed(&yarrowContext, seed, sizeof(seed));
    // Any error to report?
    if (error)
    {
        // Debug message
        TRACE_ERROR("Failed to seed PRNG!\r\n");
    }

    // TCP/IP stack initialization
    error = netInit();
    // Any error to report?
    if (error)
    {
        // Debug message
        TRACE_ERROR("Failed to initialize TCP/IP stack!\r\n");
    }

    // Configure the first network interface (Wi-Fi STA mode)
    wifiStaInterfaceInit();
    // Configure the second network interface (Wi-Fi AP mode)
    wifiApInterfaceInit();

    // Connect to Wi-Fi network (STA mode)
    wifiConnect();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////// INTERFACES //////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Wi-Fi STA interface initialization
 *
 * @note -
 *
 */
error_list wifiStaInterfaceInit(void)
{
    error_list error;
    NetInterface *interface;
    MacAddr macAddr;
#if (APP_IF0_USE_DHCP_CLIENT == DISABLED)
    Ipv4Addr ipv4Addr;
#endif
#if (APP_IF0_USE_SLAAC == DISABLED)
    Ipv6Addr ipv6Addr;
#endif

    // Configure the first network interface (Wi-Fi STA mode)
    interface = &netInterface[0];

    // Set interface name
    netSetInterfaceName(interface, APP_IF0_NAME);
    // Set host name
    netSetHostname(interface, APP_IF0_HOST_NAME);
    // Set host MAC address
    macStringToAddr(APP_IF0_MAC_ADDR, &macAddr);
    netSetMacAddr(interface, &macAddr);
    // Select the relevant network adapter
    netSetDriver(interface, &esp32WifiStaDriver);

    // Initialize network interface
    error = netConfigInterface(interface);
    // Any error to report?
    if (error)
    {
        // Debug message
        TRACE_ERROR("Failed to configure interface %s!\r\n", interface->name);
        // Exit immediately
        return error;
    }

#if (IPV4_SUPPORT == ENABLED)
#if (APP_IF0_USE_DHCP_CLIENT == ENABLED)
    // Get default settings
    dhcpClientGetDefaultSettings(&dhcpClientSettings);
    // Set the network interface to be configured by DHCP
    dhcpClientSettings.interface = interface;
    // Disable rapid commit option
    dhcpClientSettings.rapidCommit = FALSE;

    // DHCP client initialization
    error = dhcpClientInit(&dhcpClientContext, &dhcpClientSettings);
    // Failed to initialize DHCP client?
    if (error)
    {
        // Debug message
        TRACE_ERROR("Failed to initialize DHCP client!\r\n");
        // Exit immediately
        return error;
    }

    // Start DHCP client
    error = dhcpClientStart(&dhcpClientContext);
    // Failed to start DHCP client?
    if (error)
    {
        // Debug message
        TRACE_ERROR("Failed to start DHCP client!\r\n");
        // Exit immediately
        return error;
    }
#else
    // Set IPv4 host address
    ipv4StringToAddr(APP_IF0_IPV4_HOST_ADDR, &ipv4Addr);
    ipv4SetHostAddr(interface, ipv4Addr);

    // Set subnet mask
    ipv4StringToAddr(APP_IF0_IPV4_SUBNET_MASK, &ipv4Addr);
    ipv4SetSubnetMask(interface, ipv4Addr);

    // Set default gateway
    ipv4StringToAddr(APP_IF0_IPV4_DEFAULT_GATEWAY, &ipv4Addr);
    ipv4SetDefaultGateway(interface, ipv4Addr);

    // Set primary and secondary DNS servers
    ipv4StringToAddr(APP_IF0_IPV4_PRIMARY_DNS, &ipv4Addr);
    ipv4SetDnsServer(interface, 0, ipv4Addr);
    ipv4StringToAddr(APP_IF0_IPV4_SECONDARY_DNS, &ipv4Addr);
    ipv4SetDnsServer(interface, 1, ipv4Addr);
#endif
#endif

#if (IPV6_SUPPORT == ENABLED)
#if (APP_IF0_USE_SLAAC == ENABLED)
    // Get default settings
    slaacGetDefaultSettings(&slaacSettings);
    // Set the network interface to be configured
    slaacSettings.interface = interface;

    // SLAAC initialization
    error = slaacInit(&slaacContext, &slaacSettings);
    // Failed to initialize SLAAC?
    if (error)
    {
        // Debug message
        TRACE_ERROR("Failed to initialize SLAAC!\r\n");
        // Exit immediately
        return error;
    }

    // Start IPv6 address autoconfiguration process
    error = slaacStart(&slaacContext);
    // Failed to start SLAAC process?
    if (error)
    {
        // Debug message
        TRACE_ERROR("Failed to start SLAAC!\r\n");
        // Exit immediately
        return error;
    }
#else
    // Set link-local address
    ipv6StringToAddr(APP_IF0_IPV6_LINK_LOCAL_ADDR, &ipv6Addr);
    ipv6SetLinkLocalAddr(interface, &ipv6Addr);
#endif
#endif

    // Successful initialization
    return NO_ERROR;
}

/**
 * @brief Wi-Fi AP interface initialization
 *
 * @note -
 *
 */
error_list wifiApInterfaceInit(void)
{
    error_list error;
    NetInterface *interface;
    MacAddr macAddr;
    Ipv4Addr ipv4Addr;
    Ipv6Addr ipv6Addr;

    // Configure the second network interface (Wi-Fi AP mode)
    interface = &netInterface[1];

    // Set interface name
    netSetInterfaceName(interface, APP_IF1_NAME);
    // Set host name
    netSetHostname(interface, APP_IF1_HOST_NAME);
    // Set host MAC address
    macStringToAddr(APP_IF1_MAC_ADDR, &macAddr);
    netSetMacAddr(interface, &macAddr);
    // Select the relevant network adapter
    netSetDriver(interface, &esp32WifiApDriver);

    // Initialize network interface
    error = netConfigInterface(interface);
    // Any error to report?
    if (error)
    {
        // Debug message
        TRACE_ERROR("Failed to configure interface %s!\r\n", interface->name);
        // Exit immediately
        return error;
    }

#if (IPV4_SUPPORT == ENABLED)
    // Set IPv4 host address
    ipv4StringToAddr(APP_IF1_IPV4_HOST_ADDR, &ipv4Addr);
    ipv4SetHostAddr(interface, ipv4Addr);

    // Set subnet mask
    ipv4StringToAddr(APP_IF1_IPV4_SUBNET_MASK, &ipv4Addr);
    ipv4SetSubnetMask(interface, ipv4Addr);

    // Set default gateway
    ipv4StringToAddr(APP_IF1_IPV4_DEFAULT_GATEWAY, &ipv4Addr);
    ipv4SetDefaultGateway(interface, ipv4Addr);

    // Set primary and secondary DNS servers
    ipv4StringToAddr(APP_IF1_IPV4_PRIMARY_DNS, &ipv4Addr);
    ipv4SetDnsServer(interface, 0, ipv4Addr);
    ipv4StringToAddr(APP_IF1_IPV4_SECONDARY_DNS, &ipv4Addr);
    ipv4SetDnsServer(interface, 1, ipv4Addr);

#if (APP_IF1_USE_DHCP_SERVER == ENABLED)
    // Get default settings
    dhcpServerGetDefaultSettings(&dhcpServerSettings);
    // Set the network interface to be configured by DHCP
    dhcpServerSettings.interface = interface;
    // Lease time, in seconds, assigned to the DHCP clients
    dhcpServerSettings.leaseTime = 3600;

    // Lowest and highest IP addresses in the pool that are available
    // for dynamic address assignment
    ipv4StringToAddr(APP_IF1_IPV4_ADDR_RANGE_MIN, &dhcpServerSettings.ipAddrRangeMin);
    ipv4StringToAddr(APP_IF1_IPV4_ADDR_RANGE_MAX, &dhcpServerSettings.ipAddrRangeMax);

    // Subnet mask
    ipv4StringToAddr(APP_IF1_IPV4_SUBNET_MASK, &dhcpServerSettings.subnetMask);
    // Default gateway
    ipv4StringToAddr(APP_IF1_IPV4_DEFAULT_GATEWAY, &dhcpServerSettings.defaultGateway);
    // DNS servers
    ipv4StringToAddr(APP_IF1_IPV4_PRIMARY_DNS, &dhcpServerSettings.dnsServer[0]);
    ipv4StringToAddr(APP_IF1_IPV4_SECONDARY_DNS, &dhcpServerSettings.dnsServer[1]);

    // DHCP server initialization
    error = dhcpServerInit(&dhcpServerContext, &dhcpServerSettings);
    // Failed to initialize DHCP client?
    if (error)
    {
        // Debug message
        TRACE_ERROR("Failed to initialize DHCP server!\r\n");
        // Exit immediately
        return error;
    }

    // Start DHCP server
    error = dhcpServerStart(&dhcpServerContext);
    // Failed to start DHCP client?
    if (error)
    {
        // Debug message
        TRACE_ERROR("Failed to start DHCP server!\r\n");
        // Exit immediately
        return error;
    }
#endif
#endif

#if (IPV6_SUPPORT == ENABLED)
    // Set link-local address
    ipv6StringToAddr(APP_IF1_IPV6_LINK_LOCAL_ADDR, &ipv6Addr);
    ipv6SetLinkLocalAddr(interface, &ipv6Addr);

    // Set IPv6 prefix
    ipv6StringToAddr(APP_IF1_IPV6_PREFIX, &ipv6Addr);
    ipv6SetPrefix(interface, 0, &ipv6Addr, APP_IF1_IPV6_PREFIX_LENGTH);

    // Set global address
    ipv6StringToAddr(APP_IF1_IPV6_GLOBAL_ADDR, &ipv6Addr);
    ipv6SetGlobalAddr(interface, 0, &ipv6Addr);

#if (APP_IF1_USE_ROUTER_ADV == ENABLED)
    // List of IPv6 prefixes to be advertised
    ipv6StringToAddr(APP_IF1_IPV6_PREFIX, &ndpRouterAdvPrefixInfo[0].prefix);
    ndpRouterAdvPrefixInfo[0].length = APP_IF1_IPV6_PREFIX_LENGTH;
    ndpRouterAdvPrefixInfo[0].onLinkFlag = TRUE;
    ndpRouterAdvPrefixInfo[0].autonomousFlag = TRUE;
    ndpRouterAdvPrefixInfo[0].validLifetime = 3600;
    ndpRouterAdvPrefixInfo[0].preferredLifetime = 1800;

    // Get default settings
    ndpRouterAdvGetDefaultSettings(&ndpRouterAdvSettings);
    // Set the underlying network interface
    ndpRouterAdvSettings.interface = interface;
    // Maximum time interval between unsolicited Router Advertisements
    ndpRouterAdvSettings.maxRtrAdvInterval = 60000;
    // Minimum time interval between unsolicited Router Advertisements
    ndpRouterAdvSettings.minRtrAdvInterval = 20000;
    // Router lifetime
    ndpRouterAdvSettings.defaultLifetime = 0;
    // List of IPv6 prefixes
    ndpRouterAdvSettings.prefixList = ndpRouterAdvPrefixInfo;
    ndpRouterAdvSettings.prefixListLength = arraysize(ndpRouterAdvPrefixInfo);

    // RA service initialization
    error = ndpRouterAdvInit(&ndpRouterAdvContext, &ndpRouterAdvSettings);
    // Failed to initialize DHCPv6 client?
    if (error)
    {
        // Debug message
        TRACE_ERROR("Failed to initialize RA service!\r\n");
        // Exit immediately
        return error;
    }

    // Start RA service
    error = ndpRouterAdvStart(&ndpRouterAdvContext);
    // Failed to start RA service?
    if (error)
    {
        // Debug message
        TRACE_ERROR("Failed to start RA service!\r\n");
        // Exit immediately
        return error;
    }
#endif
#endif

    // Successful initialization
    return NO_ERROR;
}
