
#include <cassert>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "HeapFile.hpp"
#include "Page.hpp"
#include "Pager.hpp"
#include "Row.hpp"
#include "Constants.hpp"

using namespace std;
namespace fs = std::filesystem;

const string TEST_DB = "heapfile_test.db";

void cleanup()
{
    fs::remove(TEST_DB);
}

void test_empty_heapfile()
{
    cleanup();

    {
        HeapFile heap(TEST_DB);
        vector<Row> rows = heap.scan();

        assert(rows.empty());
    }

    cout << "PASSED: Empty HeapFile scan\n";
}

void test_insert_one_row()
{
    cleanup();

    {
        HeapFile heap(TEST_DB);

        Row original("Alice", 1, "alice@example.com");
        heap.insert(original);

        vector<Row> rows = heap.scan();

        assert(rows.size() == 1);
        assert(rows[0].id == 1);
        assert(strcmp(rows[0].username, "Alice") == 0);
        assert(strcmp(rows[0].email, "alice@example.com") == 0);
    }

    cout << "PASSED: Insert and scan one row\n";
}

void test_multiple_rows_same_page()
{
    cleanup();

    {
        HeapFile heap(TEST_DB);

        for (int32_t i = 0; i < 10; ++i)
        {
            Row row("User", i, "user@example.com");
            heap.insert(row);
        }

        vector<Row> rows = heap.scan();

        assert(rows.size() == 10);

        for (int32_t i = 0; i < 10; ++i)
        {
            assert(rows[i].id == i);
        }

        Pager pager(TEST_DB);
        assert(pager.get_Pages() == 1);
    }

    cout << "PASSED: Multiple rows in one page\n";
}

void test_page_capacity()
{
    cleanup();

    {
        HeapFile heap(TEST_DB);

        for (int32_t i = 0; i < (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE; ++i)
        {
            Row row("User", i, "user@example.com");
            heap.insert(row);
        }

        vector<Row> rows = heap.scan();

        assert(rows.size() == (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE);

        Pager pager(TEST_DB);
        assert(pager.get_Pages() == 1);
    }

    cout << "PASSED: Page capacity\n";
}

void test_page_overflow()
{
    cleanup();

    {
        HeapFile heap(TEST_DB);

        for (int32_t i = 0; i < (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE + 1; ++i)
        {
            Row row("User", i, "user@example.com");
            heap.insert(row);
        }

        vector<Row> rows = heap.scan();

        assert(rows.size() == (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE + 1);

        // The first page should be full; the next row starts page 2.
        Pager pager(TEST_DB);
        assert(pager.get_Pages() == 2);

        Page first = pager.read_Page(0);
        Page second = pager.read_Page(1);

        assert(first.row_count() == (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE);
        assert(second.row_count() == 1);

        assert(first.read_row((db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE - 1).id == (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE - 1);
        assert(second.read_row(0).id == (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE);
    }

    cout << "PASSED: Page overflow allocates another page\n";
}

void test_scan_order_across_pages()
{
    cleanup();

    {
        HeapFile heap(TEST_DB);

        const int32_t count = (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE * 2 + 5;

        for (int32_t i = 0; i < count; ++i)
        {
            Row row("User", i, "user@example.com");
            heap.insert(row);
        }

        vector<Row> rows = heap.scan();

        assert(rows.size() == static_cast<size_t>(count));

        for (int32_t i = 0; i < count; ++i)
        {
            assert(rows[i].id == i);
        }

        Pager pager(TEST_DB);
        assert(pager.get_Pages() == 3);
    }

    cout << "PASSED: Scan order across multiple pages\n";
}

void test_persistence_after_reopen()
{
    cleanup();

    const int32_t count = (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE + 7;

    {
        HeapFile heap(TEST_DB);

        for (int32_t i = 0; i < count; ++i)
        {
            Row row("User", i, "user@example.com");
            heap.insert(row);
        }
    }

    {
        HeapFile heap(TEST_DB);
        vector<Row> rows = heap.scan();

        assert(rows.size() == static_cast<size_t>(count));

        for (int32_t i = 0; i < count; ++i)
        {
            assert(rows[i].id == i);
        }
    }

    cout << "PASSED: Persistence after reopening HeapFile\n";
}

int main()
{
    cout << "\n===== ForgeDB HeapFile Tests =====\n\n";

    cleanup();

    try
    {
        test_empty_heapfile();
        test_insert_one_row();
        test_multiple_rows_same_page();
        test_page_capacity();
        test_page_overflow();
        test_scan_order_across_pages();
        test_persistence_after_reopen();
    }
    catch (...)
    {
        cleanup();
        throw;
    }

    cleanup();

    cout << "\nAll HeapFile tests passed!\n";
    return 0;
}
