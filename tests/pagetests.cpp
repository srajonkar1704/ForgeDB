
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "Page.hpp"
#include "Row.hpp"
#include "Constants.hpp"

using namespace std;

// Checks that an operation throws the expected exception type.
template <typename Function>
void expect_throw(Function operation, const char* test_name)
{
    bool threw = false;

    try
    {
        operation();
    }
    catch (const exception&)
    {
        threw = true;
    }

    if (!threw)
    {
        cerr << "FAILED: " << test_name
             << " — expected an exception\n";
        assert(false);
    }

    cout << "PASSED: " << test_name << '\n';
}

void test_empty_page()
{
    Page page;

    assert(page.row_count() == 0);
    assert(!page.is_full());

    cout << "PASSED: Empty page\n";
}

void test_insert_and_read()
{
    Page page;
    Row original("Alice", 1, "alice@example.com");

    page.insert_row(original);

    assert(page.row_count() == 1);
    assert(!page.is_full());

    Row result = page.read_row(0);

    assert(result.id == original.id);
    assert(strcmp(result.username, original.username) == 0);
    assert(strcmp(result.email, original.email) == 0);

    cout << "PASSED: Insert and read one row\n";
}

void test_multiple_rows()
{
    Page page;

    for (int32_t i = 0; i < 10; ++i)
    {
        Row row("User", i, "user@example.com");
        page.insert_row(row);
    }

    assert(page.row_count() == 10);

    for (int32_t i = 0; i < 10; ++i)
    {
        Row row = page.read_row(i);
        assert(row.id == i);
    }

    cout << "PASSED: Multiple rows\n";
}

void test_page_capacity()
{
    Page page;

    for (int32_t i = 0; i < (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE; ++i)
    {
        Row row("User", i, "user@example.com");
        page.insert_row(row);
    }

    assert(page.row_count() == (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE);
    assert(page.is_full());

    cout << "PASSED: Page reaches capacity\n";
}

void test_insert_when_full()
{
    Page page;

    for (int32_t i = 0; i < (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE; ++i)
    {
        Row row("User", i, "user@example.com");
        page.insert_row(row);
    }

    expect_throw(
        [&]() {
            Row extra("Extra", 999, "extra@example.com");
            page.insert_row(extra);
        },
        "Insertion into a full page"
    );

    assert(page.row_count() == (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE);
}

void test_invalid_read_indexes()
{
    Page page;
    Row row("Alice", 1, "alice@example.com");
    page.insert_row(row);

    expect_throw(
        [&]() { page.read_row(-1); },
        "Read using negative index"
    );

    expect_throw(
        [&]() { page.read_row(1); },
        "Read beyond current row count"
    );

    expect_throw(
        [&]() { page.read_row((db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE); },
        "Read beyond page capacity"
    );
}

void test_page_serialization()
{
    Page original;

    Row first("Alice", 1, "alice@example.com");
    Row second("Bob", 2, "bob@example.com");

    original.insert_row(first);
    original.insert_row(second);

    char buffer[db::PAGE_SIZE];
    original.serialize(buffer);

    Page restored;
    restored.deserialize(buffer);

    assert(restored.row_count() == 2);

    Row a = restored.read_row(0);
    Row b = restored.read_row(1);

    assert(a.id == 1);
    assert(strcmp(a.username, "Alice") == 0);
    assert(strcmp(a.email, "alice@example.com") == 0);

    assert(b.id == 2);
    assert(strcmp(b.username, "Bob") == 0);
    assert(strcmp(b.email, "bob@example.com") == 0);

    cout << "PASSED: Page serialization and deserialization\n";
}

void test_corrupt_metadata(int32_t bad_count, const char* name)
{
    Page page;

    char buffer[db::PAGE_SIZE];
    page.serialize(buffer);

    // The page's first four bytes store the row count.
    memcpy(buffer, &bad_count, sizeof(bad_count));

    Page corrupted;
    corrupted.deserialize(buffer);

    expect_throw(
        [&]() { corrupted.row_count(); },
        name
    );

    expect_throw(
        [&]() { corrupted.is_full(); },
        "Corrupted metadata rejected by is_full"
    );
}

void test_insert_with_corrupt_metadata()
{
    Page page;

    char buffer[db::PAGE_SIZE];
    page.serialize(buffer);

    int32_t bad_count = -1;
    memcpy(buffer, &bad_count, sizeof(bad_count));

    Page corrupted;
    corrupted.deserialize(buffer);

    expect_throw(
        [&]() {
            Row row("Alice", 1, "alice@example.com");
            corrupted.insert_row(row);
        },
        "Insert into page with corrupt metadata"
    );

    cout << "PASSED: Corrupt metadata rejected during insertion\n";
}

void test_full_page_integrity_after_failed_insert()
{
    Page page;

    for (int32_t i = 0; i < (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE; ++i)
    {
        Row row("User", i, "user@example.com");
        page.insert_row(row);
    }

    // Snapshot the existing page.
    char before[db::PAGE_SIZE];
    page.serialize(before);

    expect_throw(
        [&]() {
            Row extra("Extra", 999, "extra@example.com");
            page.insert_row(extra);
        },
        "Extra insertion into full page"
    );

    // Check row count and existing records.
    assert(page.row_count() == (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE);
    assert(page.is_full());

    for (int32_t i = 0; i < (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE; ++i)
    {
        Row row = page.read_row(i);
        assert(row.id == i);
    }

    // Verify that no bytes changed after the failed insertion.
    char after[db::PAGE_SIZE];
    page.serialize(after);

    assert(memcmp(before, after, db::PAGE_SIZE) == 0);

    cout << "PASSED: Failed insertion preserves full page\n";
}
int main()
{
    cout << "\n===== ForgeDB Page Tests =====\n\n";

    test_empty_page();
    test_insert_and_read();
    test_multiple_rows();
    test_page_capacity();
    test_insert_when_full();
    test_invalid_read_indexes();
    test_page_serialization();

    test_corrupt_metadata(-1, "Negative row count rejected");
    test_corrupt_metadata(
        static_cast<int32_t>((db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE + 1),
        "Row count above capacity rejected"
    );
    test_insert_with_corrupt_metadata();
    test_full_page_integrity_after_failed_insert();
    cout << "\nAll Page tests passed!\n";
    return 0;
}


