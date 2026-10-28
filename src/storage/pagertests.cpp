
#include <cassert>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Pager.hpp"
#include "Page.hpp"
#include "Row.hpp"
#include "Constants.hpp"

using namespace std;
namespace fs = std::filesystem;

void test_read_rejects_nonexistent_page()
{
    const string filename = "pager_seek_read_test.db";
    fs::remove(filename);

    {
        Pager pager(filename);

        bool threw = false;
        try
        {
            pager.read_Page(0);
        }
        catch (const runtime_error&)
        {
            threw = true;
        }

        assert(threw);
    }

    fs::remove(filename);
    cout << "PASSED: Read rejects nonexistent page\n";
}

void test_write_seek_and_append()
{
    const string filename = "pager_seek_write_test.db";
    fs::remove(filename);

    {
        Pager pager(filename);

        Page page;
        Row row("Alice", 1, "alice@example.com");
        page.insert_row(row);

        // First valid write: append page 0.
        pager.write_Page(0, page);
        assert(pager.get_Pages() == 1);

        // Second valid write: append page 1.
        pager.write_Page(1, page);
        assert(pager.get_Pages() == 2);

        assert(pager.read_Page(0).read_row(0).id == 1);
        assert(pager.read_Page(1).read_row(0).id == 1);
    }

    fs::remove(filename);
    cout << "PASSED: Valid write seeks and page appends\n";
}

void test_get_pages_rejects_misaligned_file()
{
    const string filename = "pager_seek_corrupt_test.db";
    fs::remove(filename);

    {
        ofstream out(filename, ios::binary);
        char bytes[5] = {};
        out.write(bytes, sizeof(bytes));
    }

    {
        Pager pager(filename);

        bool threw = false;
        try
        {
            pager.get_Pages();
        }
        catch (const runtime_error&)
        {
            threw = true;
        }

        assert(threw);
    }

    fs::remove(filename);
    cout << "PASSED: File-size validation after seeking to end\n";
}

int main()
{
    test_read_rejects_nonexistent_page();
    test_write_seek_and_append();
    test_get_pages_rejects_misaligned_file();

    cout << "\nAll seek-related tests passed!\n";
    return 0;
}
