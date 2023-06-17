set(srcs
  cidLogging.c
  wifi.c
  ssh.c
  sntp.c
)

idf_component_register(SRCS "ssh.c" "cidlogging.c" "${srcs}"
                    INCLUDE_DIRS include cyclone/common cyclone/cyclone_tcp cyclone/cyclone_ssh cyclone/cyclone_crypto
                    REQUIRES cmock vfs fatfs nvs_flash)

file(GLOB EXTRA_SOURCES "cyclone/cyclone_crypto/cipher/*.c" "cyclone/cyclone_crypto/hardware/esp32/*.c" "cyclone/cyclone_crypto/hash/*.c" "cyclone/common/*.c" "cyclone/cyclone_tcp/core/*.c" 
"cyclone/cyclone_tcp/dns/*.c" "cyclone/cyclone_tcp/netbios/*.c" "cyclone/cyclone_tcp/igmp/*.c" "cyclone/cyclone_tcp/dhcp/*.c" "cyclone/cyclone_tcp/ipv6/*.c" "cyclone/cyclone_tcp/llmnr/*.c" 
"cyclone/cyclone_tcp/ipv4/*.c" "cyclone/cyclone_crypto/rng/*.c" "cyclone/cyclone_crypto/ecc/*.c" "cyclone/cyclone_crypto/mpi/*.c" "cyclone/cyclone_crypto/pkc/*.c" "cyclone/cyclone_crypto/pkix/*.c" 
"cyclone/cyclone_crypto/aead/*.c" "cyclone/cyclone_crypto/encoding/*.c" "cyclone/cyclone_crypto/cipher_modes/*.c" "cyclone/cyclone_crypto/mac/*.c" "cyclone/cyclone_tcp/drivers/wifi/*.c" 
"cyclone/cyclone_ssh/sftp/*.c" "cyclone/cyclone_ssh/ssh/*.c")
target_sources(${COMPONENT_LIB} PRIVATE ${EXTRA_SOURCES})