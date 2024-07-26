db file is not organized by block, because one db only has one db file. which only store 3 information(db name, db descripition, the location of db default table file). So now i have not designed a block type for it.

block has 3 type (tentative):
-- table block: this type block is less used than data block. cause one SQL only need to read sometimes to load table struct.
-- column data block: the data is organized by column.
-- log block: this type block is designed to store the change of data, using log to replace changing data on column data block.
-- vector data block: designed to store vector, so can support many index type. TODO
