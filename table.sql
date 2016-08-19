CREATE TABLE [result](
    [id] INTEGER PRIMARY KEY ON CONFLICT FAIL AUTOINCREMENT, 
    [srctype] INT(8) NOT NULL DEFAULT 0, 
    [bugtype] TEXT(30) NOT NULL ON CONFLICT FAIL COLLATE NOCASE, 
    [filename] TEXT NOT NULL ON CONFLICT FAIL, 
    [line] INT(8) NOT NULL ON CONFLICT FAIL, 
    [counter] TINYINT DEFAULT 1);

    
insert into [main].[result](srctype,bugtype,filename,line,counter) values(0,0,'char.c',30,1);   
 
    
    #for table create;