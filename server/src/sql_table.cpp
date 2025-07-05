#include "sql_table.hpp"

tbl::tbl(string tblname){
    name = tblname;
    txt.open("/tables/"+name+".txt",ios::out);
    
}