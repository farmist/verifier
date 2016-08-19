#include "extractdata.h"

int ExtractData::ProcessSourceFiles(const std::string &fileName,int type)
{

    std::smatch ms;
    std::cmatch cms;
    int counter=1;
    int line=1;// wukong start line number from 1
    if(access(fileName.c_str(),F_OK)||access(fileName.c_str(),R_OK))
    {
        return -1;
    }
    string fileShortName=fileName.substr(fileName.find_last_of('\\')+1);
    fin.open(fileName);
    memset(buffer,0,4096);
        while(!(fin.getline(buffer,4096)).eof())
        {
            if(type)
            {
                //always only one match!
                if(regex_search(buffer,cms,wkErrExp))
                {
//                    cout<<"in process wkout"<<endl;
//                    for(auto x:cms)
//                    {
//                        cout<<x.str().length()<<":"<<x.str()<<endl;
//                    }
                    WriteToDatabase(type,cms[1].str().c_str(),cms[3].str(),std::stoi(cms[2].str()));
                }

            }
            else
            {
                string suffix(buffer);
                while(regex_search(suffix,ms,bugFlagsExp))
                {
    //                cout<<fileShortName<<":"<<line<<":"<<buffer<<endl;
    //                for(auto x:ms)
    //                {
    //                    cout<<x.str().length()<<":"<<x.str()<<endl;
    //                }
                    if(ms[1].str().length())
                        counter=stoi(ms[1].str());
                    else
                        counter=1;

                    WriteToDatabase(type,bugList[stoi(ms[2].str())],fileShortName,line,counter);
                    suffix=ms.suffix().str();
                }
                memset(buffer,0,4096);
                line++;
            }

        }

    fin.close();
    fin.clear();
    return 0;
}

int ExtractData::WriteToStd(const std::string &sql,int startpos)
{
    int res=0;
    int rescount=startpos;
    cout<<std::left;
    res=sqlite3_exec(this->dbconnect,
                     sql.c_str(),
                     ExtractData::OutputStd,
                     &rescount,
                     &dbErrorMsg
                     );
    cout<<std::right;
    if(res)
    {
        cout<<"database error in WriteToStd:"<<dbErrorMsg<<endl;
        cout<<"with sql:"<<sql<<endl;
        sqlite3_free(dbErrorMsg);
        return -1;
    }
    return rescount;
}

int ExtractData::CaculateResults(int srcIn, int srcOut)
{
    int res=0;
    bool isViewNotExist=false;
    int totalResults=0;
    res=sqlite3_exec(this->dbconnect,
                     "select count(*) from [sqlite_master] where type='view' and name='output'",
                     ExtractData::CheckEmpty,
                     &isViewNotExist,
                     &dbErrorMsg);
    if(res)
    {
        cout<<"database error in check view:"<<dbErrorMsg<<endl;
        sqlite3_free(dbErrorMsg);
        return -1;
    }
    if(!isViewNotExist)
    {
        res=sqlite3_exec(this->dbconnect,
                         "drop view [output]",
                         NULL,
                         NULL,
                         &dbErrorMsg);
        if(res)
        {
            cout<<"database error in drop view:"<<dbErrorMsg<<endl;
            sqlite3_free(dbErrorMsg);
            return -1;
        }
    }

string cvsql(createView);
cvsql.append(std::to_string(srcOut));
res=sqlite3_exec(this->dbconnect,
                 cvsql.c_str(),
                 NULL,NULL,
                 &dbErrorMsg);
if(res)
{
    cout<<"database error in Create view:"<<dbErrorMsg<<endl;
    sqlite3_free(dbErrorMsg);
    return -1;
}

string splithsql(spilthItem_slice1);
splithsql.append(std::to_string(srcOut));
splithsql.append(spilthItem_slice2);
splithsql.append(std::to_string(srcIn));
splithsql.append(" )");
totalResults=WriteToStd(splithsql);

string unfndsql(this->unfoundItem_slice1);
unfndsql.append(std::to_string(srcIn));
unfndsql.append(unfoundItem_slice2);
totalResults=WriteToStd(unfndsql,totalResults);

string uncvsql(this->uncoverItem_slice1);
uncvsql.append(std::to_string(srcIn));
uncvsql.append(uncoverItem_slice2);
totalResults=WriteToStd(uncvsql,totalResults);

cout<<"...................................    "<<totalResults<<" results in total    ..................................\n";
return totalResults;
}

int ExtractData::WriteToDatabase(int srcType, const char *bugTypes, std::string fileName, int lineNumber, int counter)
{
//                cout<<"srctype:"<<srcType<<endl;
//                cout<<"bugtypes:"<<bugTypes<<endl;
//                cout<<"fileName:"<<fileName<<endl;
//                cout<<"lineNumber:"<<lineNumber<<endl;
//                cout<<"counter:"<<counter<<endl;
    bool empty;
    int res;
    string selsql(searchItem);
    string cond;
    cond.append(std::to_string(srcType));
    cond.append(" and bugtype='");
    cond.append(bugTypes);
    cond.append("' and fileName='");
    cond.append(fileName);
    cond.append("' and line=");
    cond.append(std::to_string(lineNumber));
    selsql.append(cond);
    res=sqlite3_exec(this->dbconnect,
                     selsql.c_str(),
                     ExtractData::CheckEmpty,
                     &empty,
                     &dbErrorMsg
                     );
    if(res)
    {
        cout<<"database error:"<<dbErrorMsg<<endl;
        cout<<"with:"<<selsql<<endl;
        sqlite3_free(dbErrorMsg);
        return -1;
    }
    if(empty)
    {
        //cout<<"insert routine\n";
        string inssql(insertItem);
        inssql.append(std::to_string(srcType));
        inssql.append(" , '");
        inssql.append(bugTypes);
        inssql.append("' , '");
        inssql.append(fileName);
        inssql.append("' ,  ");
        inssql.append(std::to_string(lineNumber));
        inssql.append(" , ");
        inssql.append(std::to_string(counter));
        inssql.append(")");
        res=sqlite3_exec(this->dbconnect,
                         inssql.c_str(),
                         NULL,
                         NULL,
                         &dbErrorMsg);
        if(res)
        {
            cout<<"database error:"<<dbErrorMsg<<endl;
             cout<<"with:"<<inssql<<endl;
            sqlite3_free(dbErrorMsg);
            return -1;
        }
        //cout<<"insert:"<<sqlite3_changes(dbconnect)<<endl;
    }
    else
    {

        string updsql(updateItem);
        updsql.append(std::to_string(counter));
        updsql.append(" where srctype=");
        updsql.append(cond);
         //cout<<"update routine:"<<updsql<<endl;
        res=sqlite3_exec(this->dbconnect,
                         updsql.c_str(),
                         NULL,
                         NULL,
                         &dbErrorMsg);
        if(res)
        {
            cout<<"database error:"<<dbErrorMsg<<endl;
             cout<<"with:"<<updsql<<endl;
            sqlite3_free(dbErrorMsg);
            return -1;
        }
        //cout<<"update:"<<sqlite3_changes(dbconnect)<<endl;
    }

    return 0;
}


ExtractData::ExtractData(const std::string &dbName)
    :sqlHeaders("insert into "),
      dbErrorMsg(NULL),
      bugFlagsExp("[ \\t]*#[ \\t]*([0-9]*)bug-([0-9]+)[ \\t]*#"),
      wkErrExp("\\*+\\s*(\\S+)[\\s]+.*[\\s]+(\\d+)[\\D]+[\\s]+([^\\s\\*]+)\\*+"),
      renewTable(true)
{
    if(sqlite3_open(dbName.c_str(),& dbconnect))
    {
        dbconnect=NULL;
        cout<<"Open database error!"<<endl;
    }
    else
    {
        int res=0;
        if(!renewTable) //if not true(force renew), check the database
        {
            res=sqlite3_exec(this->dbconnect,
                             "select count(*) from [sqlite_master] where type='table' and name='result'",
                             ExtractData::CheckEmpty,
                             &renewTable,
                             &dbErrorMsg);
            if(res)
            {
                cout<<"database error in check table:"<<dbErrorMsg<<endl;
                sqlite3_free(dbErrorMsg);
            }
            cout<<"empty:"<<renewTable;
        }
        //whether renew or initalize ,we do below steps, otherwise skip
        if(this->renewTable)
        {

            res= sqlite3_exec(dbconnect,
                              "drop table [result]",
                              NULL,
                              NULL,
                              &dbErrorMsg);
            if(res)
            {
                cout<<"database error in renew table:"<<dbErrorMsg<<endl;
                sqlite3_free(dbErrorMsg);
            }
            res= sqlite3_exec(dbconnect,
                              "CREATE TABLE [result](\
                              [id] INTEGER PRIMARY KEY ON CONFLICT FAIL AUTOINCREMENT,\
                              [srctype] INT(8) NOT NULL DEFAULT 0,\
                              [bugtype] TEXT(30) NOT NULL ON CONFLICT FAIL COLLATE NOCASE, \
                              [filename] TEXT NOT NULL ON CONFLICT FAIL,\
                              [line] INT(8) NOT NULL ON CONFLICT FAIL,\
                              [counter] TINYINT DEFAULT 1)",
                    NULL,
                    NULL,
                    &dbErrorMsg);
            if(res)
            {
                cout<<"database error in create table:"<<dbErrorMsg<<endl;
                sqlite3_free(dbErrorMsg);
            }
        }


    }
}
ExtractData* ExtractData::singleton=NULL;
char  ExtractData::bugList[5][30]=
{
    "use-after-free",
    "memory-leaks",
    "unk1",
    "unk2",
    "unk3"
};

char ExtractData::searchItem[60]="select count(*) from [result] where srctype=";
char ExtractData::insertItem[128]="insert into [result](srctype,bugtype,filename,line,counter) values(";
char ExtractData::updateItem[128]="update [result] set counter=counter+";
char ExtractData::createView[128]="create view [output] as select * from [result] where srctype=";
char ExtractData::unfoundItem_slice1[64]="select [result].*,0 as found from [result] where srctype=";
char ExtractData::unfoundItem_slice2[128]=" and 0 in(select count(id) from [output] \
where filename=[result].filename and line=[result].line and bugtype=[result].bugtype)";
char ExtractData::uncoverItem_slice1[128]="select [result].*,[output].counter as found from [result],[output] where [result].srctype=";
char ExtractData::uncoverItem_slice2[256]=" and [result].filename=[output].filename  and [result].line=[output].line "
                                          "and [result].bugtype=[output].bugtype and [result].counter <> [output].counter";
char ExtractData::spilthItem_slice1[128]="select id,srctype,bugtype,filename,line, 0 as counter,"
                                         "[output].counter as found from [output] where srctype=";
char ExtractData::spilthItem_slice2[256]=" and 0 in (select count(id) from [result] where filename=[output].filename and line=[output].line and bugtype=[output].bugtype and srctype=";
