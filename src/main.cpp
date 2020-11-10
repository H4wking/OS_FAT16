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

struct File {
    char name_extension[11];
    uint16_t size;
//    uint16_t creation_time;
//    uint16_t creation_date;
    uint16_t modification_time;
    uint16_t modification_date;
    uint16_t attribute;
    uint16_t first_cluster_address;
};

int read_img(void *var, std::string &img, int offset, int bytes) {
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

    int offset = bs.reserved_sector_count * bs.bytes_per_sector +
                 bs.table_count * bs.table_size_in_sectors * bs.bytes_per_sector;

    std::cout << offset << std::endl;

    for (int i = 0; i < bs.root_entry_count; i++) {
        File f{};

        read_img(&f.name_extension, img, offset, 11);
        read_img(&f.size, img, offset + 28, 4);
//        read_img(&f.creation_time, img, offset + 14, 2);
//        read_img(&f.creation_date, img, offset + 16, 2);
        read_img(&f.modification_time, img, offset + 22, 2);
        read_img(&f.modification_date, img, offset + 24, 2);
        read_img(&f.attribute, img, offset + 11, 1);
        read_img(&f.first_cluster_address, img, offset + 26, 2);




        std::cout << f.name_extension << "  " << f.size << "  " << f.modification_time << "  " << f.modification_date
        << " " << f.attribute << " " << f.first_cluster_address << std::endl;

        offset += 32;
    }

    return EXIT_SUCCESS;
}
