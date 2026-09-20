#include <iostream>
#include <cassert>
#include <cstring>
#include <filesystem>
#include <stdexcept>

#include "storage/Row.hpp"
#include "storage/Page.hpp"
#include "storage/Pager.hpp"
#include "storage/Constants.hpp"

using namespace std;


// ------------------------------------------------------------
// Helper: compare two Rows by comparing their serialized bytes
// ------------------------------------------------------------

bool rows_equal(const Row& a, const Row& b)
{
    char buffer_a[Row::ROW_SIZE]{};
    char buffer_b[Row::ROW_SIZE]{};

    a.serialize(buffer_a);
    b.serialize(buffer_b);

    return std::memcmp(buffer_a, buffer_b, Row::ROW_SIZE) == 0;
}


// ------------------------------------------------------------
// Helper
// ------------------------------------------------------------

void passed(const string& message)
{
    cout << "[PASS] " << message << '\n';
}


// ============================================================
// ROW TESTS
// ============================================================

void test_row_serialization()
{
    Row original("Alice", 101, "alice@example.com");

    char buffer[Row::ROW_SIZE]{};

    original.serialize(buffer);

    Row recovered = Row::deserialize(buffer);

    assert(rows_equal(original, recovered));

    passed("Row serialization/deserialization");
}


void test_row_serialized_size()
{
    assert(Row::ROW_SIZE == 100);
    assert(Row::SerializedSize() == 100);

    passed("Row serialized size is 100 bytes");
}


void test_row_different_values()
{
    Row row1("Alice", 1, "alice@example.com");
    Row row2("Bob", 2, "bob@example.com");

    assert(!rows_equal(row1, row2));

    passed("Different rows remain different");
}


// ============================================================
// PAGE TESTS
// ============================================================

void test_empty_page()
{
    Page page;

    assert(page.isfull() == false);

    passed("New page starts empty");
}


void test_page_insert_and_read()
{
    Page page;

    Row original("Alice", 101, "alice@example.com");

    page.insert_row(original);

    Row recovered = page.read_row(0);

    assert(rows_equal(original, recovered));

    passed("Page inserts and reads one row");
}


void test_multiple_rows()
{
    Page page;

    Row row1("Alice", 1, "alice@example.com");
    Row row2("Bob", 2, "bob@example.com");
    Row row3("Charlie", 3, "charlie@example.com");

    page.insert_row(row1);
    page.insert_row(row2);
    page.insert_row(row3);

    assert(rows_equal(page.read_row(0), row1));
    assert(rows_equal(page.read_row(1), row2));
    assert(rows_equal(page.read_row(2), row3));

    passed("Page preserves multiple row order");
}


void test_page_full()
{
    Page page;

    for (int i = 0; i < 40; i++)
    {
        Row row("User", i, "user@example.com");
        page.insert_row(row);
    }

    assert(page.isfull());

    passed("Page becomes full after 40 rows");
}


void test_page_rejects_extra_row()
{
    Page page;

    for (int i = 0; i < 40; i++)
    {
        Row row("User", i, "user@example.com");
        page.insert_row(row);
    }

    bool threw = false;

    try
    {
        Row extra("Extra", 999, "extra@example.com");
        page.insert_row(extra);
    }
    catch (const std::exception&)
    {
        threw = true;
    }

    assert(threw);

    passed("Full page rejects insertion");
}


void test_invalid_page_index()
{
    Page page;

    Row row("Alice", 1, "alice@example.com");
    page.insert_row(row);

    bool threw = false;

    try
    {
        page.read_row(1);
    }
    catch (const std::exception&)
    {
        threw = true;
    }

    assert(threw);

    passed("Reading unused row throws");
}


void test_negative_page_index()
{
    Page page;

    Row row("Alice", 1, "alice@example.com");
    page.insert_row(row);

    bool threw = false;

    try
    {
        page.read_row(-1);
    }
    catch (const std::exception&)
    {
        threw = true;
    }

    assert(threw);

    passed("Negative row index throws");
}


void test_page_serialization()
{
    Page page;

    Row row1("Alice", 1, "alice@example.com");
    Row row2("Bob", 2, "bob@example.com");

    page.insert_row(row1);
    page.insert_row(row2);

    char buffer[db::PAGE_SIZE]{};

    page.serializer(buffer);

    Page recovered_page;

    recovered_page.deserializer(buffer);

    assert(rows_equal(recovered_page.read_row(0), row1));
    assert(rows_equal(recovered_page.read_row(1), row2));

    passed("Page serialization/deserialization");
}


// ============================================================
// PAGER TESTS
// ============================================================

void test_pager_creates_database()
{
    const string filename = "test.db";

    filesystem::remove(filename);

    Pager pager(filename);

    assert(filesystem::exists(filename));

    passed("Pager creates database file");

    filesystem::remove(filename);
}


void test_write_first_page()
{
    const string filename = "test.db";

    filesystem::remove(filename);

    Pager pager(filename);

    Page page;

    Row row("Alice", 1, "alice@example.com");

    page.insert_row(row);

    pager.write_Page(0, page);

    assert(filesystem::file_size(filename) == db::PAGE_SIZE);

    passed("Pager writes first 4096-byte page");

    filesystem::remove(filename);
}


void test_read_page()
{
    const string filename = "test.db";

    filesystem::remove(filename);

    Pager pager(filename);

    Page page;

    Row original("Alice", 1, "alice@example.com");

    page.insert_row(original);

    pager.write_Page(0, page);

    Page recovered_page = pager.read_Page(0);

    Row recovered = recovered_page.read_row(0);

    assert(rows_equal(original, recovered));

    passed("Pager reads previously written page");

    filesystem::remove(filename);
}


void test_persistence_after_restart()
{
    const string filename = "test.db";

    filesystem::remove(filename);

    Row original("Alice", 101, "alice@example.com");

    // Pager instance 1
    {
        Pager pager(filename);

        Page page;

        page.insert_row(original);

        pager.write_Page(0, page);
    }

    // Pager instance 2
    {
        Pager pager(filename);

        Page recovered_page = pager.read_Page(0);

        Row recovered = recovered_page.read_row(0);

        assert(rows_equal(original, recovered));
    }

    passed("Data persists after Pager restart");

    filesystem::remove(filename);
}


void test_multiple_pages()
{
    const string filename = "test.db";

    filesystem::remove(filename);

    Pager pager(filename);

    Page page0;
    Page page1;

    Row row0("Alice", 1, "alice@example.com");
    Row row1("Bob", 2, "bob@example.com");

    page0.insert_row(row0);
    page1.insert_row(row1);

    pager.write_Page(0, page0);
    pager.write_Page(1, page1);

    Page recovered0 = pager.read_Page(0);
    Page recovered1 = pager.read_Page(1);

    assert(rows_equal(recovered0.read_row(0), row0));
    assert(rows_equal(recovered1.read_row(0), row1));

    assert(filesystem::file_size(filename) == 2 * db::PAGE_SIZE);

    passed("Multiple pages are stored independently");

    filesystem::remove(filename);
}


void test_page_offsets()
{
    const string filename = "test.db";

    filesystem::remove(filename);

    Pager pager(filename);

    Page page0;
    Page page1;

    Row alice("Alice", 111, "alice@example.com");
    Row bob("Bob", 222, "bob@example.com");

    page0.insert_row(alice);
    page1.insert_row(bob);

    pager.write_Page(0, page0);
    pager.write_Page(1, page1);

    Page recovered0 = pager.read_Page(0);
    Page recovered1 = pager.read_Page(1);

    assert(rows_equal(recovered0.read_row(0), alice));
    assert(rows_equal(recovered1.read_row(0), bob));

    passed("Page offsets are correct");

    filesystem::remove(filename);
}


void test_invalid_page_number()
{
    const string filename = "test.db";

    filesystem::remove(filename);

    Pager pager(filename);

    Page page;

    pager.write_Page(0, page);

    bool threw = false;

    try
    {
        pager.read_Page(1);
    }
    catch (const std::exception&)
    {
        threw = true;
    }

    assert(threw);

    passed("Reading nonexistent page throws");

    filesystem::remove(filename);
}


void test_negative_page_number()
{
    const string filename = "test.db";

    filesystem::remove(filename);

    Pager pager(filename);

    bool threw = false;

    try
    {
        pager.read_Page(-1);
    }
    catch (const std::exception&)
    {
        threw = true;
    }

    assert(threw);

    passed("Negative page number throws");

    filesystem::remove(filename);
}


void test_page_gap_rejected()
{
    const string filename = "test.db";

    filesystem::remove(filename);

    Pager pager(filename);

    Page page;

    pager.write_Page(0, page);

    bool threw = false;

    try
    {
        // Page 1 does not exist yet.
        // Page 2 should therefore be rejected.
        pager.write_Page(2, page);
    }
    catch (const std::exception&)
    {
        threw = true;
    }

    assert(threw);

    passed("Page gaps are rejected");

    filesystem::remove(filename);
}


// ============================================================
// MAIN
// ============================================================

int main()
{
    try
    {
        cout << "\n========== ROW TESTS ==========\n";

        test_row_serialization();
        test_row_serialized_size();
        test_row_different_values();


        cout << "\n========== PAGE TESTS ==========\n";

        test_empty_page();
        test_page_insert_and_read();
        test_multiple_rows();
        test_page_full();
        test_page_rejects_extra_row();
        test_invalid_page_index();
        test_negative_page_index();
        test_page_serialization();


        cout << "\n========== PAGER TESTS ==========\n";

        test_pager_creates_database();
        test_write_first_page();
        test_read_page();
        test_persistence_after_restart();
        test_multiple_pages();
        test_page_offsets();
        test_invalid_page_number();
        test_negative_page_number();
        test_page_gap_rejected();


        cout << "\n========== ALL TESTS PASSED ==========\n";
    }
    catch (const std::exception& e)
    {
        cout << "\n[FAIL] " << e.what() << '\n';
        return 1;
    }

    return 0;
}