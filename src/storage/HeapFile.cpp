#include "Page.hpp"
#include "HeapFile.hpp"

HeapFile::HeapFile(const std::string& filename):pager(filename)
{}

RID HeapFile:: insert(const Row& row)
{
    int32_t no_pages = pager.get_Pages();

    for(int32_t i =0; i<no_pages;i++)
    {
        Page page = pager.read_Page(i);
        if(!page.is_full())
        {
            int32_t row_no = page.row_count();
            page.insert_row(row);
            pager.write_Page(i,page); 
            return RID(i,row_no);                                                              
        }
    }

    Page newpage ;
    newpage.insert_row(row);
    pager.write_Page(no_pages,newpage);
    return RID(no_pages,0);

}

std::vector<Row> HeapFile:: scan()
{
    std::vector<Row> table;
    size_t no_page = pager.get_Pages();
    
    for(size_t i =0;i<no_page;i++)
    {
        Page page = pager.read_Page(i);
        int32_t no_row = page.row_count();

        for(int32_t j =0;j<no_row;j++)
        {
            Row row = page.read_row(j);
            table.push_back(row);
        }
    }

    return table;
}

Row HeapFile::get(const RID& rid) 
{
    Page page = pager.read_Page(rid.get_page_id());
    return page.read_row(rid.get_slot_id());

}