#include "kt/util.h"

void util::hex_dump(const char *data, size_t size)
{
    std::cout << Hexdump(data, size) << std::endl;
}
void util::c_hex_dump(const void *data, size_t size)
{
    spdlog::info("Dumping hex of size: {:#x}", size);

    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';
    for (i = 0; i < size; ++i)
    {
        if (i % 0x10 == 0)
        {
            printf("0x%0lx: \n\t", i);
        }
        printf("%02X ", ((unsigned char *)data)[i]);
        if (((unsigned char *)data)[i] >= ' ' && ((unsigned char *)data)[i] <= '~')
        {
            ascii[i % 16] = ((unsigned char *)data)[i];
        }
        else
        {
            ascii[i % 16] = '.';
        }
        if ((i + 1) % 8 == 0 || i + 1 == size)
        {
            printf(" ");
            if ((i + 1) % 16 == 0)
            {
                printf("|  %s \n", ascii);
            }
            else if (i + 1 == size)
            {
                ascii[(i + 1) % 16] = '\0';
                if ((i + 1) % 16 <= 8)
                {
                    printf(" ");
                }
                for (j = (i + 1) % 16; j < 16; ++j)
                {
                    printf("   ");
                }
                printf("|  %s \n", ascii);
            }
        }
    }
}

#ifdef __ANDROID__
std::string util::get_version() {
    const size_t FINGERPRINT_MAX_SIZE = 256;
    char fingerprint[FINGERPRINT_MAX_SIZE];
    int len = __system_property_get("ro.build.fingerprint", fingerprint);
    if (len > FINGERPRINT_MAX_SIZE)
    {
        spdlog::critical("fingerprint buffer too small: {}. FINGERPRINT_MAX_SIZE {}.\nIncrease FINGERPRINT_MAX_SIZE", len, FINGERPRINT_MAX_SIZE);
        return {};
    } else {
        return std::string(fingerprint, len);
    }
}
bool util::check_version(std::string version)
{
    std::string phone_version = get_version();

    spdlog::info("Fingerprint: {}", phone_version);

    if (phone_version != version)
    {
        spdlog::warn("Fingerprint version: {} does not match target version: {}", phone_version, version);
        return false;
    }
    return true;
}
#endif