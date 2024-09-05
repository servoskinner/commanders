#include "Storage_manager.hpp"

Storage_manager::Storage_manager(std::string storage_file)
{   
    file.open(storage_file, std::ios::in | std::ios::out | std::ios::binary);
    if (file.bad() || file.fail()) {
        // try to create file
        file.clear();
        file.open(storage_file, std::ios::out | std::ios::trunc);
        if (file.bad() || file.fail()) {
            throw std::runtime_error("Storage_manager: error opening file");
        }
        file.close();
        // Open it again with correct flags
        file.open(storage_file, std::ios::in | std::ios::out | std::ios::binary);
        if (file.bad() || file.fail()) {
            throw std::runtime_error("Storage_manager: error opening file");
        }
    }

    file.seekg(0);
    file.read((char*)&n_entries, sizeof(n_entries));
    if (file.fail() || file.eof()) {
        file.clear();
        n_entries = 0;
    }
}

Storage_manager& Storage_manager::get_default()
{
    static Storage_manager storage_manager(DEFAULT_STORAGE_FILE);
    return storage_manager;
}

std::optional<unsigned> Storage_manager::locate_entry_indexed(int index)
{
    if (index >= n_entries) {
        return {};
    }
    file.seekg(4);

    if (file.fail() || file.eof()) {
        file.clear();
        return {};
    }
    unsigned short locator_size;
    unsigned int data_size;

    for (int i = 0; i < index; i++) {

        file.read((char*)&locator_size, sizeof(locator_size));
        file.read((char*)&data_size, sizeof(data_size));

        if (file.fail() || file.eof()) {
            file.clear();
            return {};
        }
        file.seekg((unsigned)file.tellg() + locator_size + data_size);
    }

    if (file.fail()) {
        file.clear();
        return {};
    }
    return {(unsigned)file.tellg()};
}

std::optional<unsigned> Storage_manager::locate_entry(std::string locator)
{
    file.seekg(4);
    
    if (file.fail() || file.eof()) {
        file.clear();
        return {};
    }
    unsigned short locator_size;
    unsigned int data_size;

    while (true) {

        file.read((char*)&locator_size, sizeof(locator_size));
        file.read((char*)&data_size, sizeof(data_size));

        if (file.fail() || file.eof()) {
            file.clear();
            return {};
        }
        std::string entry_locator;
        entry_locator.resize(locator_size);

        file.read(entry_locator.data(), locator_size);
        if (file.fail() || file.eof()) {
            file.clear();
            return {};
        }
        if (entry_locator == locator) {
            return (unsigned)file.tellg() - sizeof(locator_size) - sizeof(data_size) - locator_size;
        }
        file.seekg((unsigned)file.tellg() + data_size);
    }
}

std::optional<Serialized> Storage_manager::read_entry(unsigned at_pos)
{
    Serialized data;

    file.seekg(at_pos);
    if (file.fail() || file.eof()) {
        file.clear();
        return {};
    }
    unsigned short locator_size;
    unsigned int data_size;

    file.read((char*)&locator_size, sizeof(locator_size));
    file.read((char*)&data_size, sizeof(data_size));
    if (file.fail() || file.eof()) {
        file.clear();
        return {};
    }
    data.resize(data_size);

    file.seekg(locator_size, std::ios::cur);
    file.read(data.data(), data_size);

    if (file.fail() || file.eof()) {
        file.clear();
        return {};
    }

    return {data};
}

bool Storage_manager::put_entry(unsigned at_pos, Storage_manager::Storage_entry& entry)
{
    file.seekp(at_pos);
    if (file.fail() || file.eof()) {
        file.clear();
        return false;
    }
    unsigned short locator_size = entry.locator.size();
    unsigned int data_size = entry.data.size();

    file.write((char*)&locator_size, sizeof(locator_size));
    file.write((char*)&data_size, sizeof(data_size));
    file.write(entry.locator.data(), locator_size);
    file.write(entry.data.data(), data_size);

    file.flush();
    return !(file.fail() || file.eof());
}

std::optional<Serialized> Storage_manager::get_item(std::string locator)
{
    std::optional<unsigned int> location = locate_entry(locator);
    if (!location.has_value()) {
        return {};
    }
    Serialized data = read_entry(location.value()).value();

    return data;
}

bool Storage_manager::put_item(std::string locator, std::vector<char> value)
{
    Storage_entry entry;
    entry.locator = locator;
    entry.data = value;
    bool swedish_fail = file.fail();

    file.seekp(0, std::ios::end);
    std::optional<unsigned> write_position = locate_entry(locator);
    
    if(write_position.has_value()) {
        return put_entry(write_position.value(), entry);
    }
    else {
        n_entries += 1;
        if (!put_entry(file.tellp(), entry)) {
            return false;
        }
        file.seekp(0);
        file.write((char*)&n_entries, sizeof(n_entries));
        if (file.fail() || file.eof()) {
            file.clear();
            return false;
        }
        file.flush();
    }
    return true;
}