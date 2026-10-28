
#include <cassert>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <stdexcept>

#include "HeapFile.hpp"
#include "RID.hpp"
#include "Row.hpp"
#include "Constants.hpp"

using namespace std;
namespace fs = std::filesystem;

const string TEST_DB = "rid_test.db";

void test_rid_constructor()
{
    RID rid(2, 7);

    assert(rid.get_page_id() == 2);
    assert(rid.get_slot_id() == 7);

    cout << "PASSED: RID stores page and slot IDs\n";

void test_invalid_rid()
{
    bool negative_page_threw = false;
    bool negative_slot_threw = false;

    try
    {
        RID invalid(-1, 0);
    }
    catch (const invalid_argument&)
    {
        negative_page_threw = true;
    }

    try
    {
        RID invalid(0, -1);
    }
    catch (const invalid_argument&)
    {
        negative_slot_threw = true;
    }

    assert(negative_page_threw);
    assert(negative_slot_threw);

    cout << "PASSED: RID rejects negative IDs\n";
}

void test_insert_and_get()
{
    fs::remove(TEST_DB);

    {
        HeapFile heap(TEST_DB);

        Row original("Alice", 101, "alice@example.com");
        RID rid = heap.insert(original);

        assert(rid.get_page_id() == 0);
        assert(rid.get_slot_id() == 0);

        Row result = heap.get(rid);

        assert(result.id == original.id);
        assert(strcmp(result.username, original.username) == 0);
        assert(strcmp(result.email, original.email) == 0);
    }

    fs::remove(TEST_DB);
    cout << "PASSED: Retrieve a record using its RID\n";
}

void test_rid_across_pages()
{
    fs::remove(TEST_DB);

    {
        HeapFile heap(TEST_DB);
        RID last_rid(0, 0);
        RID next_rid(0, 0);

        for (int32_t i = 0; i < (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE + 1; ++i)
        {
            Row row("User", i, "user@example.com");
            RID rid = heap.insert(row);

            if (i == (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE - 1)
                last_rid = rid;

            if (i == (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE)
                next_rid = rid;
        }

        assert(last_rid.get_page_id() == 0);
        assert(last_rid.get_slot_id() == (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE - 1);

        assert(next_rid.get_page_id() == 1);
        assert(next_rid.get_slot_id() == 0);

        assert(heap.get(last_rid).id == (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE - 1);
        assert(heap.get(next_rid).id == (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE);
    }

    fs::remove(TEST_DB);
    cout << "PASSED: RID retrieval across page boundaries\n";
}

void test_invalid_rid_retrieval()
{
    fs::remove(TEST_DB);

    {
        HeapFile heap(TEST_DB);
        heap.insert(Row("Alice", 1, "alice@example.com"));

        bool invalid_page_threw = false;
        bool invalid_slot_threw = false;

        try
        {
            heap.get(RID(10, 0));
        }
        catch (const exception&)
        {
            invalid_page_threw = true;
        }

        try
        {
            heap.get(RID(0, 1));
        }
        catch (const exception&)
        {
            invalid_slot_threw = true;
        }

        assert(invalid_page_threw);
        assert(invalid_slot_threw);
    }

    fs::remove(TEST_DB);
    cout << "PASSED: Invalid RID retrieval is rejected\n";
}

void test_rid_after_reopen()
{
    fs::remove(TEST_DB);

    RID saved_rid(0, 0);

    {
        HeapFile heap(TEST_DB);

        for (int32_t i = 0; i < (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE + 1; ++i)
        {
            Row row("User", i, "user@example.com");
            RID rid = heap.insert(row);

            if (i == (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE)
                saved_rid = rid;
        }
    }

    {
        HeapFile heap(TEST_DB);
        Row result = heap.get(saved_rid);

        assert(result.id == (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE);
    }

    fs::remove(TEST_DB);
    cout << "PASSED: RID retrieval after reopening database\n";
}

int main()
{
    cout << "\n===== ForgeDB RID Tests =====\n\n";

    test_rid_constructor();
    test_invalid_rid();
    test_insert_and_get();
    test_rid_across_pages();
    test_invalid_rid_retrieval();
    test_rid_after_reopen();

    cout << "\nAll RID tests passed!\n";
    return 0;
}
