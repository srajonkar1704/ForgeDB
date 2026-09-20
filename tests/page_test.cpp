#include <iostream>
#include <cassert>
#include <cstring>
#include <stdexcept>

#include "Page.hpp"

using namespace std;


// ============================================================
// Helper: Compare two Rows
// ============================================================

bool rows_equal(const Row& a, const Row& b)
{
    return a.id == b.id &&
           strcmp(a.username, b.username) == 0 &&
           strcmp(a.email, b.email) == 0;
}


// ============================================================
// Helper: Create a Row
// ============================================================

Row make_row(int id, const char* username, const char* email)
{
    Row row;

    row.id = id;

    strcpy(row.username, username);
    strcpy(row.email, email);

    return row;
}


// ============================================================
// Helper: Print successful test
// ============================================================

void passed(const string& test)
{
    cout << "[PASS] " << test << '\n';
}


// ============================================================
// MAIN
// ============================================================

int main()
{
    cout << "=====================================\n";
    cout << "       FORGEDB - PAGE TESTS\n";
    cout << "=====================================\n\n";


    // ========================================================
    // TEST 1
    // Newly created Page should not be full
    // ========================================================

    {
        Page page;

        assert(page.isfull() == false);

        passed("New page starts empty");
    }


    // ========================================================
    // TEST 2
    // Insert one Row
    // ========================================================

    {
        Page page;

        Row row = make_row(
            1,
            "sam",
            "sam@example.com"
        );

        page.insert_row(row);

        // If insertion did not throw, basic insertion succeeded.

        passed("Insert one row");
    }


    // ========================================================
    // TEST 3
    // Insert -> Read should return the same Row
    // ========================================================

    {
        Page page;

        Row original = make_row(
            1,
            "sam",
            "sam@example.com"
        );

        page.insert_row(original);

        Row recovered = page.read_row(0);

        assert(rows_equal(original, recovered));

        passed("Insert and read one row");
    }


    // ========================================================
    // TEST 4
    // Insert multiple Rows
    // ========================================================

    {
        Page page;

        Row row1 = make_row(1, "sam", "sam@example.com");
        Row row2 = make_row(2, "alex", "alex@example.com");
        Row row3 = make_row(3, "john", "john@example.com");

        page.insert_row(row1);
        page.insert_row(row2);
        page.insert_row(row3);

        passed("Insert multiple rows");
    }


    // ========================================================
    // TEST 5
    // Verify multiple Rows are stored at correct positions
    // ========================================================

    {
        Page page;

        Row row1 = make_row(1, "sam", "sam@example.com");
        Row row2 = make_row(2, "alex", "alex@example.com");
        Row row3 = make_row(3, "john", "john@example.com");

        page.insert_row(row1);
        page.insert_row(row2);
        page.insert_row(row3);

        Row recovered1 = page.read_row(0);
        Row recovered2 = page.read_row(1);
        Row recovered3 = page.read_row(2);

        assert(rows_equal(row1, recovered1));
        assert(rows_equal(row2, recovered2));
        assert(rows_equal(row3, recovered3));

        passed("Multiple rows retain correct order");
    }


    // ========================================================
    // TEST 6
    // Verify row offset calculation
    //
    // Metadata = 4 bytes
    // Row size = 100 bytes
    //
    // Row 0 -> offset 4
    // Row 1 -> offset 104
    // Row 2 -> offset 204
    // ========================================================

    {
        Page page;

        Row row1 = make_row(
            100,
            "row0",
            "row0@email.com"
        );

        Row row2 = make_row(
            200,
            "row1",
            "row1@email.com"
        );

        Row row3 = make_row(
            300,
            "row2",
            "row2@email.com"
        );

        page.insert_row(row1);
        page.insert_row(row2);
        page.insert_row(row3);

        assert(page.read_row(0).id == 100);
        assert(page.read_row(1).id == 200);
        assert(page.read_row(2).id == 300);

        passed("Row offsets are correct");
    }


    // ========================================================
    // TEST 7
    // Reading an unused slot should throw
    // ========================================================

    {
        Page page;

        Row row1 = make_row(
            1,
            "sam",
            "sam@example.com"
        );

        Row row2 = make_row(
            2,
            "alex",
            "alex@example.com"
        );

        page.insert_row(row1);
        page.insert_row(row2);

        bool exception_thrown = false;

        try
        {
            // Only indices 0 and 1 contain rows.
            page.read_row(2);
        }
        catch (const invalid_argument&)
        {
            exception_thrown = true;
        }

        assert(exception_thrown);

        passed("Reading unused row throws");
    }


    // ========================================================
    // TEST 8
    // Negative index should throw
    // ========================================================

    {
        Page page;

        bool exception_thrown = false;

        try
        {
            page.read_row(-1);
        }
        catch (const invalid_argument&)
        {
            exception_thrown = true;
        }

        assert(exception_thrown);

        passed("Negative index throws");
    }


    // ========================================================
    // TEST 9
    // Index beyond physical page capacity should throw
    //
    // We don't use Page::MAX_ROWS because it is private.
    // ========================================================

    {
        Page page;

        bool exception_thrown = false;

        try
        {
            page.read_row(1000);
        }
        catch (const invalid_argument&)
        {
            exception_thrown = true;
        }

        assert(exception_thrown);

        passed("Out-of-range index throws");
    }


    // ========================================================
    // TEST 10
    // Page should become full after exactly 40 rows
    //
    // We don't access MAX_ROWS.
    // We simply insert until isfull() reports true.
    // ========================================================

    {
        Page page;

        int inserted = 0;

        while (!page.isfull())
        {
            Row row = make_row(
                inserted,
                "user",
                "user@example.com"
            );

            page.insert_row(row);

            inserted++;
        }

        assert(page.isfull() == true);
        assert(inserted == 40);

        passed("Page becomes full after 40 rows");
    }


    // ========================================================
    // TEST 11
    // Full Page should reject another Row
    // ========================================================

    {
        Page page;

        int inserted = 0;

        while (!page.isfull())
        {
            Row row = make_row(
                inserted,
                "user",
                "user@example.com"
            );

            page.insert_row(row);

            inserted++;
        }

        Row extra = make_row(
            999,
            "extra",
            "extra@example.com"
        );

        bool exception_thrown = false;

        try
        {
            page.insert_row(extra);
        }
        catch (const invalid_argument&)
        {
            exception_thrown = true;
        }

        assert(exception_thrown);

        passed("Full page rejects insertion");
    }


    // ========================================================
    // TEST 12
    // Verify all 40 Rows after filling the Page
    // ========================================================

    {
        Page page;

        int inserted = 0;

        while (!page.isfull())
        {
            Row row = make_row(
                inserted,
                "user",
                "user@example.com"
            );

            page.insert_row(row);

            inserted++;
        }

        assert(inserted == 40);

        for (int i = 0; i < inserted; i++)
        {
            Row recovered = page.read_row(i);

            assert(recovered.id == i);
            assert(strcmp(
                recovered.username,
                "user"
            ) == 0);

            assert(strcmp(
                recovered.email,
                "user@example.com"
            ) == 0);
        }

        passed("All 40 rows recover correctly");
    }


    // ========================================================
    // TEST 13
    // Two Pages should be independent
    // ========================================================

    {
        Page page1;
        Page page2;

        Row row1 = make_row(
            1,
            "page1",
            "page1@email.com"
        );

        Row row2 = make_row(
            2,
            "page2",
            "page2@email.com"
        );

        page1.insert_row(row1);
        page2.insert_row(row2);

        assert(rows_equal(
            page1.read_row(0),
            row1
        ));

        assert(rows_equal(
            page2.read_row(0),
            row2
        ));

        passed("Pages are independent");
    }


    // ========================================================
    // TEST 14
    // One Page should not affect another Page
    // ========================================================

    {
        Page page1;
        Page page2;

        for (int i = 0; i < 10; i++)
        {
            Row row = make_row(
                i,
                "page1",
                "page1@email.com"
            );

            page1.insert_row(row);
        }

        // page2 should still be empty.
        // Therefore reading row 0 must throw.

        bool exception_thrown = false;

        try
        {
            page2.read_row(0);
        }
        catch (const invalid_argument&)
        {
            exception_thrown = true;
        }

        assert(exception_thrown);

        passed("Page state is isolated");
    }


    // ========================================================
    // TEST 15
    // Verify boundary behaviour around capacity
    //
    // Insert 39 -> should not be full
    // Insert 40th -> should be full
    // ========================================================

    {
        Page page;

        for (int i = 0; i < 39; i++)
        {
            Row row = make_row(
                i,
                "user",
                "user@email.com"
            );

            page.insert_row(row);
        }

        assert(page.isfull() == false);

        Row final_row = make_row(
            39,
            "user",
            "user@email.com"
        );

        page.insert_row(final_row);

        assert(page.isfull() == true);

        passed("Capacity boundary is correct");
    }


    // ========================================================
    // TEST 16
    // Verify first and last rows
    // ========================================================

    {
        Page page;

        for (int i = 0; i < 40; i++)
        {
            Row row = make_row(
                i,
                "user",
                "user@email.com"
            );

            page.insert_row(row);
        }

        Row first = page.read_row(0);
        Row last = page.read_row(39);

        assert(first.id == 0);
        assert(last.id == 39);

        passed("First and last row are accessible");
    }


    // ========================================================
    // FINAL RESULT
    // ========================================================

    cout << "\n=====================================\n";
    cout << " ALL PAGE TESTS PASSED SUCCESSFULLY\n";
    cout << "=====================================\n";

    return 0;
}