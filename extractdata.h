#ifndef EXTRACTDATA_H
#define EXTRACTDATA_H
#include<string>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <regex>
#include<sqlite/sqlite3.h>
using std::string;
using std::ifstream;
using std::cout;
using std::endl;
using std::regex;
using std::smatch;
using std::regex_match;
using std::regex_search;


class ExtractData
{
public:
    //return  the global instance
    static ExtractData * Instance(const string dbName=string("testprog.db"))
    {
        if(!singleton)
            singleton=new ExtractData(dbName);
        return singleton;
    }
    //destroy the global instance of this class
    void Destroy()
    {
        delete this;
    }
    //as its name says.
    int ProcessSourceFiles(const string& fileName, int type=0);
    int WriteToStd(const string & sql, int startpos=0);
    int ProcessWKOupputFiles(const string & fileName);
    //sqlite callback:check whether the database select's result is empty(null) or not
    static int CheckEmpty(void *isEmpty, int Argc, char **Argv, char **AzColName)
    {
        //here RenewTable must be a pointer to renewTable
        bool * empty=(bool *)isEmpty;

        if(!atoi(Argv[0]))
            *empty=true;
        else
            *empty=false;

        return 0;

    }
    //sqlite callback: write to std stream on the screen.
    static int OutputStd(void *counter, int Argc, char **Argv, char **AzColName)
    {
        //here counter must be a pointer to results' counter
        int * count=(int *)counter;

        if(*count==0)
        {
            cout<<"\n----------------------------------------------------------------------------------------------------------------------\n";
            for(int i=0;i<Argc;i++)
            {
                cout.width(18);
                cout<<AzColName[i];
            }
            cout<<"\n----------------------------------------------------------------------------------------------------------------------\n";
        }
        for(int j=0;j<Argc;j++)
        {
            cout.width(18);
            cout<<Argv[j];
        }
        *count=*count+1;
        cout<<"\n----------------------------------------------------------------------------------------------------------------------\n";

        return 0;

    }
    //caculate the difference between src and out
    //srcIn : source label number default:0
    //srcOut: wukong output label number default:1
    int CaculateResults(int srcIn,int srcOut);

    //write datas gathered from files to database
    // srcType used to recognize where the data comes from.
    //bugTypes is type name of the bug
    //filename & lineNumber is the main data items
    //counter is used to match the mult-occurance of the same bugs in the same pos
    int WriteToDatabase(int srcType,const char* bugTypes,string fileName,int lineNumber,int counter=1);

    ~ExtractData()
    {
        sqlite3_close(dbconnect);
    }
private:
    ExtractData(const string& dbName);


    static ExtractData* singleton;// class object instance for singleton method
    string sqlHeaders;//unused
    sqlite3* dbconnect;//database connection
    char * dbErrorMsg;//database error messages
    ifstream fin;
    char buffer[4096];//file processing buffer
    regex bugFlagsExp;//recognize bug marks in source files.
    regex wkErrExp;// recognize bug infos in wukong's output
    bool renewTable; //whether force renew the database table or not
    static char bugList[5][30] ;//translate bug numbers to bug name strings
    static char searchItem[60]; //search database for records that already exist.
    static char insertItem[128];//insert new record for bugs.
    static char updateItem[128];//update records with new counter
    static char createView[128];//create view for output records
    static char unfoundItem_slice1[64];// for the bugs marked that not found by wukong
    static char unfoundItem_slice2[128];
    static char uncoverItem_slice1[128];//for the bugs marked that not fully found by wukong
    static char uncoverItem_slice2[256];
    static char spilthItem_slice1[128];//recognize records produced mistakenly by wukong
    static char spilthItem_slice2[256];
};

#endif // EXTRACTDATA_H
