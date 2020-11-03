#include <iostream>
#include <fstream>
#include <cstring>

struct Boot_sector {
    uint16_t bytes_per_sector;
    uint16_t sectors_per_cluster;
    uint16_t reserved_sector_count;
    uint16_t table_count;
    uint16_t table_size_in_sectors;
    uint16_t root_entry_count;
    uint16_t signature;
};

int read_img(uint16_t *var, std::string &img, int offset, int bytes) {
    memcpy(var, img.c_str() + offset, bytes);
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "No image provided." << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream file (argv[1], std::ifstream::binary);
    std::string img((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());

    Boot_sector bs;


    read_img(&bs.bytes_per_sector, img, 11, 2);
    read_img(&bs.sectors_per_cluster, img, 13, 1);
    read_img(&bs.reserved_sector_count, img, 14, 2);
    read_img(&bs.table_count, img, 16, 1);
    read_img(&bs.root_entry_count, img, 17, 2);
    read_img(&bs.table_size_in_sectors, img, 22, 2);
    read_img(&bs.signature, img, 510, 2);


    std::cout << bs.bytes_per_sector << std::endl;
    std::cout << bs.sectors_per_cluster << std::endl;
    std::cout << bs.reserved_sector_count << std::endl;
    std::cout << bs.table_count << std::endl;
    std::cout << bs.root_entry_count << std::endl;
    std::cout << bs.table_size_in_sectors << std::endl;
    std::cout << bs.signature << std::endl;

    return EXIT_SUCCESS;
}
