#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <string>

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

    std::cout << "--------BOOT SECTOR--------" << std::endl;

    std::cout << "Sector size in bytes: " << bs.bytes_per_sector << std::endl;
    std::cout << "Number of sectors is cluster: " << bs.sectors_per_cluster << std::endl;
    std::cout << "FAT copies count: " << bs.table_count << std::endl;
    std::cout << "FAT size in sectors: " << bs.table_size_in_sectors << std::endl;
    std::cout << "FAT size in bytes: " << bs.table_size_in_sectors * bs.bytes_per_sector << std::endl;
    std::cout << "Number of root directory entries: " << bs.root_entry_count << std::endl;
    std::cout << "Size of root directory in bytes: " << bs.root_entry_count * 32 << std::endl;
    std::cout << "Number of reserved sectors: " << bs.reserved_sector_count << std::endl;
    std::cout << "Correct signature: " << ((bs.signature == 0xAA55) ? "true" : "false") << std::endl << std::endl;

    int offset = bs.reserved_sector_count * bs.bytes_per_sector +
                 bs.table_count * bs.table_size_in_sectors * bs.bytes_per_sector;

    std::cout << "--------ROOT DIRECTORY--------" << std::endl;

    for (int i = 0; i < bs.root_entry_count; i++) {
        File f{};

        read_img(&f.name_extension, img, offset, 11);
        read_img(&f.size, img, offset + 28, 4);
        read_img(&f.modification_time, img, offset + 22, 2);
        read_img(&f.modification_date, img, offset + 24, 2);
        read_img(&f.attribute, img, offset + 11, 1);
        read_img(&f.first_cluster_address, img, offset + 26, 2);

        if (f.name_extension[0] == 0x00) {
            break;
        }

        std::string attr = "";
        if (f.attribute & 0x01) {
            attr += "Read-only   ";
        }
        if (f.attribute & 0x02) {
            attr += "Hidden file   ";
        }
        if (f.attribute & 0x04) {
            attr += "System file   ";
        }
        if (f.attribute & 0x08) {
            attr += "Volume label   ";
        }
        if (f.attribute & 0x0f) {
            attr += "Long file name   ";
        }
        if (f.attribute & 0x10) {
            attr += "Directory   ";
        }
        if (f.attribute & 0x20) {
            attr += "Archive   ";
        }

        std::string name = f.name_extension;

        if (name[8] != ' ') {
            name = name.substr(0, 8) + "." + name.substr(8, 3) + " ";
        } else {
            name += "  ";
        }

        std::cout << name << "  ";

        if (f.attribute & 0x10) {
            std::cout << std::left << std::setfill(' ') << std::setw(8) << "DIR";
        } else {
            std::cout << std::left << std::setfill(' ') << std::setw(8) << f.size;
        }

        std::cout << std::right << std::setfill('0') << std::setw(2) << (f.modification_date & 0b11111) << "-" <<
                     std::setfill('0') << std::setw(2) << ((f.modification_date >> 5) & 0b1111) << "-" <<
                     ((f.modification_date >> 9) & 0b1111111) + 1980 << "  ";

        std::cout << std::setfill('0') << std::setw(2) << ((f.modification_time >> 11) & 0b11111) << ":" <<
                     std::setfill('0') << std::setw(2) << ((f.modification_time >> 5) & 0b111111) << ":" <<
                     std::setfill('0') << std::setw(2) << (f.modification_time & 0b11111) * 2 << "  ";

        std::cout << std::left << std::setfill(' ') << std::setw(70) << attr << f.first_cluster_address << std::endl;

        offset += 32;
    }

    return EXIT_SUCCESS;
}
