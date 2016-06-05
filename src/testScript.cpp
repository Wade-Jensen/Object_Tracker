#include "..\include\Version.h"
#ifdef _TEST_

#include "..\include\UserInput.h"

#include "..\include\DFT.h"



bool testParseCli1();
bool testParseCli2();
bool testParseTxt1();
bool testParseTxt2();

int main()
{
    bool success;
    int numPassed = 0;
    int numTests = 0;

    vcl_cout << "Running test script: " << vcl_endl;

    bool test1 = testParseCli1(); numPassed += test1; numTests++;
    bool test2 = testParseCli2(); numPassed += test2; numTests++;
    bool test3 = testParseTxt1(); numPassed += test3; numTests++;
    bool test4 = testParseTxt2(); numPassed += test4; numTests++;
    bool test5 = testParseTxt3(); numPassed += test5; numTests++;
    bool test6 = testParseTxt4(); numPassed += test6; numTests++;

    vcl_cout << vcl_endl;
    vcl_cout << "Test1: "; if (test1) {vcl_cout << "Passed"<<vcl_endl;} else if (!test1) {vcl_cout << "Failed"<<vcl_endl;}
    vcl_cout << "Test2: "; if (test2) {vcl_cout << "Passed"<<vcl_endl;} else if (!test2) {vcl_cout << "Failed"<<vcl_endl;}
    vcl_cout << "Test3: "; if (test3) {vcl_cout << "Passed"<<vcl_endl;} else if (!test3) {vcl_cout << "Failed"<<vcl_endl;}
    vcl_cout << "Test4: "; if (test4) {vcl_cout << "Passed"<<vcl_endl;} else if (!test4) {vcl_cout << "Failed"<<vcl_endl;}
    vcl_cout << "Test5: "; if (test5) {vcl_cout << "Passed"<<vcl_endl;} else if (!test5) {vcl_cout << "Failed"<<vcl_endl;}
    vcl_cout << "Test6: "; if (test6) {vcl_cout << "Passed"<<vcl_endl;} else if (!test6) {vcl_cout << "Failed"<<vcl_endl;}


    vcl_cout << numPassed << " of " << numTests << " Tests Passed." <<vcl_endl;;
    vcl_cout << "Test script complete" << vcl_endl;

}

bool testParseCli1()
{
    // Stub out the command line with input values
    int argc;
    char* argv[30];
    for (int i=0;i<30;i++)
    {
        argv[i] = new char[20];
    }
    argc = 29;
    argv[0] = "objectTracker.exe";
    argv[1] = "-path";
    argv[2] = "Data/bolt";
    argv[3] = "-glob";
    argv[4] = "jpg";
    argv[5] = "-ipx";
    argv[6] = "336";
    argv[7] = "-ipy";
    argv[8] = "165";
    argv[9] = "-w";
    argv[10] = "25";
    argv[11] = "-h";
    argv[12] = "60";
    argv[13] = "-c";
    argv[14] = "8";
    argv[15] = "-sb";
    argv[16] = "4";
    argv[17] = "-bc";
    argv[18] = "1";
    argv[19] = "-sd";
    argv[20] = "30";
    argv[21] = "-planes";
    argv[22] = "3";
    argv[23] = "-lr";
    argv[24] = "0.05";
    argv[25] = "-sds";
    argv[26] = "1.0";
    argv[27] = "-sdc";
    argv[28] = "0.625";

    UserInput userInput;
    bool cliRead = userInput.parseCli(argc, argv);

    for (int i=0;i<30;i++)
    {
        delete[] argv[i];
    }
    delete argv;

    if (cliRead)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool testParseCli2()
{
    // Stub out the command line with input values
    int argc;
    char* argv[30];
    for (int i=0;i<30;i++)
    {
        argv[i] = new char[20];
    }
    argc = 29;
    argv[0] = "objectTracker.exe";
    argv[1] = "-path";
    argv[2] = "6";
    argv[3] = "-glob";
    argv[4] = "jpg";
    argv[5] = "-ipx";
    argv[6] = "336";
    argv[7] = "-ipy";
    argv[8] = "165";
    argv[9] = "-w";
    argv[10] = "-20";
    argv[11] = "-h";
    argv[12] = "-1";
    argv[13] = "-c";
    argv[14] = "8";
    argv[15] = "-sb";
    argv[16] = "4";
    argv[17] = "-bc";
    argv[18] = "1";
    argv[19] = "-sd";
    argv[20] = "30";
    argv[21] = "-planes";
    argv[22] = "3";
    argv[23] = "-lr";
    argv[24] = "0.05";
    argv[25] = "-sds";
    argv[26] = "1.0";
    argv[27] = "-sdc";
    argv[28] = "0.625";

    UserInput userInput;
    bool cliRead = userInput.parseCli(argc, argv);

    for (int i=0;i<30;i++)
    {
        delete[] argv[i];
    }
    delete argv;

    if (!cliRead)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool testParseTxt1()
{
    UserInput userInput;
    bool txtRead = userInput.parseTxt("test/validconfig.txt");

    return txtRead;
}

bool testParseTxt2()
{
    UserInput userInput;
    bool txtRead = userInput.parseTxt("test/fake.txt");

    return !txtRead;
}

#endif




