#include <algorithm>
#include <cstring>
#include <cxxabi.h>
#include <elf.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <unistd.h> //for color output

using namespace std;

bool colorOutput = false;
bool showProgramHeaders = false;
bool showSections = false;
bool showSymbols = false;
bool demangleNames = true;

// Check if an address belongs to external memory
bool isExtmem(int x) { return (x >> 20) != 0; }

//
// Terminal color output
//

void outputSetWhite() {
    if (colorOutput)
        cout << "\x1b[37m";
}

void outputSetPurple() {
    if (colorOutput)
        cout << "\x1b[35m";
}

void outputReset() {
    if (colorOutput)
        cout << "\x1b[0m";
}

// Shortcut for outputting addresses in nice format
class ashex {
  public:
    ashex(unsigned p) : val(p){};
    ~ashex(){};
    unsigned val;
};

class addr {
  public:
    addr(unsigned p) : val(p){};
    ~addr(){};
    unsigned val;
};

ostream& operator<<(ostream& stream, const ashex& rhs) {
    return stream << "0x" << setfill('0') << hex << setw(8) << rhs.val << dec;
}

ostream& operator<<(ostream& stream, const addr& addr) {
    if (isExtmem(addr.val)) {
        outputSetWhite();
        stream << ashex(addr.val);
        outputReset();
    } else {
        stream << ashex(addr.val);
    }
    return stream;
}

string demangle(const string& name) {
    string ret;
    int status;
    char* realname = 0;

    realname = abi::__cxa_demangle(name.c_str(), 0, 0, &status);

    if (status == 0)
        ret = realname;
    else
        ret = name;

    if (realname)
        free(realname);
    return ret;
}

// Check if a file header corresponds to the Epiphany ELF file format
#define EM_ADAPTEVA_EPIPHANY 0x1223
bool isEpiphanyElf(Elf32_Ehdr* ehdr) {
    return ehdr && memcmp(ehdr->e_ident, ELFMAG, SELFMAG) == 0 &&
           ehdr->e_ident[EI_CLASS] == ELFCLASS32 && ehdr->e_type == ET_EXEC &&
           ehdr->e_version == EV_CURRENT &&
           ehdr->e_machine == EM_ADAPTEVA_EPIPHANY;
}

void parseProgramHeader(Elf32_Phdr* phdr, size_t count) {
    int localsize = 0, extmemsize = 0;
    for (size_t ihdr = 0; ihdr < count; ihdr++) {
        if (isExtmem(phdr[ihdr].p_vaddr))
            extmemsize += phdr[ihdr].p_memsz;
        else
            localsize += phdr[ihdr].p_memsz;
    }

    cout << "Total size in local memory:    " << ashex(localsize) << " = "
         << localsize / 1024 << " KB" << endl;
    cout << "Total size in external memory: " << ashex(extmemsize) << " = "
         << extmemsize / 1024 << " KB" << endl;

    if (showProgramHeaders) {
        cout << "\nProgram header table (runtime info):\n";
        cout << "VADDR       SIZE\t\t\tFILESIZE\n";
        cout << setfill('0') << hex;
        for (size_t ihdr = 0; ihdr < count; ihdr++) {
            cout << addr(phdr[ihdr].p_vaddr);
            cout << "  " << ashex(phdr[ihdr].p_memsz);
            cout << " (" << phdr[ihdr].p_memsz / 1024 << " KB)";
            cout << "\t" << ashex(phdr[ihdr].p_filesz) << endl;
        }
    }
}
void parseSections(Elf32_Shdr* shdr, size_t count, const char* strtab,
                   size_t strsize) {
    if (showSections) {
        cout << "\nSection table (linker info):\n";
        cout << "ADDR        SIZE        NAME\n";
        for (size_t i = 0; i < count; i++) {
            if (shdr[i].sh_flags & SHF_ALLOC) {
                cout << addr(shdr[i].sh_addr);
            } else {
                cout << "          ";
            }
            cout << "  " << ashex(shdr[i].sh_size);
            if (shdr[i].sh_name != SHN_UNDEF && shdr[i].sh_name < strsize)
                cout << "  " << (&strtab[shdr[i].sh_name]);
            cout << endl;
        }
    }
}

class Symbol {
  public:
    int index;
    unsigned int value; // must be unsigned, addresses use the last bit
    int size;
    int type;    // STT_FUNC, STT_OBJECT, etc
    int bind;    // STB_GLOBAL, STB_LOCAL
    int section; // SHN_ABS, SHN_COMMON, SHN_UNDEF, or section index
    string name;

    bool operator<(const Symbol& rhs) const {
        // if this is an absolute value
        // instead of an address, show it AFTER
        // the addresses.
        if ((section != SHN_ABS) && (rhs.section == SHN_ABS)) {
            return true;
        } else if ((section == SHN_ABS) && (rhs.section != SHN_ABS)) {
            return false;
        }
        return value < rhs.value;
    }
};

vector<Symbol> symbols;

void parseSymbolTable(Elf32_Sym* symbol, size_t count, const char* symstr,
                      size_t symstrsize) {
    Symbol sym;
    for (size_t i = 0; i < count; i++) {
        sym.index = i;
        sym.value = symbol[i].st_value;
        sym.size = symbol[i].st_size;
        sym.type = ELF32_ST_TYPE(symbol[i].st_info);
        sym.bind = ELF32_ST_BIND(symbol[i].st_info);
        sym.section = symbol[i].st_shndx;
        if (symbol[i].st_name < symstrsize)
            sym.name = ((char*)(&symstr[symbol[i].st_name]));
        else
            sym.name.clear();

        if (demangleNames)
            sym.name = demangle(sym.name);

        symbols.push_back(sym);
    }
    sort(symbols.begin(), symbols.end());

    if (showSymbols) {
        cout << "\nSymbol table contains " << count << " symbols." << endl;
        cout << "Showing only global symbols. Highlighting symbol sizes larger "
                "than 128 Bytes (local) or 1024 Bytes (external)."
             << endl;
        cout << "VALUE        SIZE TYPE    SEC NAME\n";
        for (size_t i = 0; i < symbols.size(); i++) {
            Symbol& sym = symbols[i];
            bool is_large = false;
            if (isExtmem(sym.value)) {
                is_large = sym.size > 1024;
            } else {
                is_large = sym.size > 128;
            }
            if (sym.bind == STB_GLOBAL) {
                cout << addr(sym.value);

                if (is_large)
                    outputSetPurple();
                cout << ' ' << dec << setfill(' ') << setw(6) << sym.size;
                outputReset();

                switch (sym.type) {
                case STT_NOTYPE:
                    cout << " NOTYPE ";
                    break;
                case STT_OBJECT:
                    cout << " OBJECT ";
                    break;
                case STT_FUNC:
                    cout << " FUNC   ";
                    break;
                case STT_SECTION:
                    cout << " SECTION";
                    break;
                case STT_FILE:
                    cout << " FILE   ";
                    break;
                default:
                    cout << " Unknown";
                    break;
                }

                switch (sym.section) {
                case SHN_ABS:
                    cout << " ABS";
                    break;
                case SHN_COMMON:
                    cout << " COM";
                    break;
                case SHN_UNDEF:
                    cout << " UND";
                    break;
                default:
                    cout << dec << setw(4) << setfill(' ') << sym.section;
                    break;
                }

                cout << ' ' << sym.name;
                cout << endl;
            }
        }
        cout << dec;
    }
}

int parseFile(char* buffer, size_t fsize) {
    Elf32_Ehdr* ehdr = (Elf32_Ehdr*)buffer;
    Elf32_Phdr* phdr;
    Elf32_Shdr* shdr;
    const char* strtab;

    // Array of program headers
    phdr = (Elf32_Phdr*)&buffer[ehdr->e_phoff];
    // Array of section headers
    shdr = (Elf32_Shdr*)&buffer[ehdr->e_shoff];

    // Check if header matches Epiphany signature
    // Check if all section headers point to something within file buffer
    if (fsize < sizeof(Elf32_Ehdr) || !isEpiphanyElf(ehdr) ||
        (ehdr->e_phoff + ehdr->e_phnum * sizeof(Elf32_Phdr) > fsize ||
         (ehdr->e_shoff + ehdr->e_shnum * sizeof(Elf32_Shdr)) > fsize) ||
        (ehdr->e_shstrndx >= ehdr->e_shnum) ||
        ((shdr[ehdr->e_shstrndx].sh_offset + shdr[ehdr->e_shstrndx].sh_size) >
         fsize)) {
        return 1;
    }

    // By the check above, we can be sure that we stay inside the buffer
    parseProgramHeader(phdr, ehdr->e_phnum);

    strtab = &buffer[shdr[ehdr->e_shstrndx].sh_offset];
    parseSections(shdr, ehdr->e_shnum, strtab, shdr[ehdr->e_shstrndx].sh_size);

    for (size_t i = 0; i < ehdr->e_shnum; i++) {
        if (shdr[i].sh_type == SHT_SYMTAB) {
            Elf32_Sym* symbol = (Elf32_Sym*)&buffer[shdr[i].sh_offset];
            size_t count = shdr[i].sh_size / shdr[i].sh_entsize;

            // Check if all symbols are within file buffer
            if (shdr[i].sh_offset + count * sizeof(Elf32_Sym) > fsize)
                return 1;

            // Section with symbol names
            Elf32_Shdr* symstrhdr = &shdr[shdr[i].sh_link];

            // Check if the symbol name table is within file buffer
            if (shdr[i].sh_link >= ehdr->e_shnum)
                return 1;
            if (symstrhdr->sh_offset + symstrhdr->sh_size > fsize)
                return 1;

            const char* symstr = &buffer[symstrhdr->sh_offset];

            parseSymbolTable(symbol, count, symstr, symstrhdr->sh_size);
        }
    }

    return 0;
}

char* getCmdOption(char** begin, char** end, const std::string& option) {
    char** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option) {
    return std::find(begin, end, option) != end;
}

void showHelp(char* cmd) {
    cout << "Usage: " << cmd
         << " [-h] [-p] [-sec] [-s] [-a] [--nodemangle] file\n";
    cout << "Options:\n";
    cout << "    -h             Show this help.\n";
    cout << "    -p             Show program headers (runtime information).\n";
    cout << "    -sec           Show section headers (linking information).\n";
    cout << "                   Sections stored in external memory are "
            "highlighted.\n";
    cout << "    -s             Show symbols (function names).\n";
    cout << "                   Symbols are sorted by address.\n";
    cout << "    -a             Show all.\n";
    cout << "    --nodemangle   Do not demangle C++ symbol names.\n";
    cout << "  By default, only the total program size in local and external "
            "memory is shown."
         << endl;
}

int main(int argc, char* argv[]) {
    if (cmdOptionExists(argv, argv + argc, "-h")) {
        showHelp(argv[0]);
        return 0;
    }

    // Retrieve the filename
    if ((argc <= 1) || (argv[argc - 1] == NULL) || (argv[argc - 1][0] == '-')) {
        showHelp(argv[0]);
        return 1;
    }

    if (cmdOptionExists(argv, argv + argc, "-p"))
        showProgramHeaders = true;

    if (cmdOptionExists(argv, argv + argc, "-sec"))
        showSections = true;

    if (cmdOptionExists(argv, argv + argc, "-s"))
        showSymbols = true;

    if (cmdOptionExists(argv, argv + argc, "-a")) {
        showProgramHeaders = true;
        showSections = true;
        showSymbols = true;
    }

    if (cmdOptionExists(argv, argv + argc, "--nodemangle")) {
        demangleNames = false;
    }

    if (isatty(fileno(stdout)))
        colorOutput = true;

    std::ifstream file(argv[argc - 1], ios::binary);

    if (!file.is_open()) {
        cerr << "Could not open file " << argv[1] << endl;
        return 1;
    }

    streampos fsize = 0;
    file.seekg(0, ios::beg);
    fsize = file.tellg();
    file.seekg(0, ios::end);
    fsize = file.tellg() - fsize;
    file.seekg(0, ios::beg);

    char* buffer = new char[fsize];
    file.read(buffer, fsize);
    file.close();

    int ret = parseFile(buffer, fsize);
    delete[] buffer;

    if (ret) {
        cerr << "Error while parsing file '" << argv[argc - 1] << "'" << endl;
        cerr << "File is not a valid Epiphany executable." << endl;
    }

    return ret;
}
