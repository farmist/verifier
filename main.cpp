#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <list>
#include <regex>
#include <fstream>
#include<extractdata.h>
using namespace std;

int main(int argc, char *argv[])
{
//    char cont[]="**************************line 8 in file tarb.d********";
//    std::regex di("line");
//    std::string s ("this subject has a submarine as a subsequence");
//    std::smatch m;
//    std::regex e ("\\b(sub)([^ ]*)");   // matches words beginning by "sub"

//    if (std::regex_search (s,m,e)) {
//      for (auto x:m) std::cout << x << " ";
//      std::cout << std::endl;
//      s = m.suffix().str();
//      cout<<s<<endl;
//    }

//    if (std::regex_match ("subject", std::regex("(sub)(.*)") ))
//        std::cout << "string literal matched\n";
    list<string> filelist;
    string wkOutput="result.wkout";
    int src=0;
    int out=1;
    //we always assume the result file is in the current directory.
    string pwd;
     std::regex di("\\b[^ ]+\\.(c|cpp)"); //disabled  other src types
     //bug types:
     //std::regex bugtypes("//#bug-[0-9]+#");
    DIR* dir=opendir(".");
    cout<<dir->dd_name<<endl;
    pwd=string(dir->dd_name);
    pwd.pop_back();
    cout<<pwd<<endl;
    struct dirent *ptr;
    while(ptr=readdir(dir))
    {
        if(std::regex_match(ptr->d_name,di) )
        {
           filelist.push_back(pwd+string(ptr->d_name));
           cout<<ptr->d_name<<endl;
        }

    }
    ExtractData * inst=ExtractData::Instance();
    for(auto x:filelist)
    {
        cout<<"Processing:"<<x<<endl;
        inst->ProcessSourceFiles(x,src);

    }
    inst->ProcessSourceFiles(wkOutput,out);
    inst->CaculateResults(0,1);
    closedir(dir);
    //cout << "Hello World!" << endl;
    return 0;
}
