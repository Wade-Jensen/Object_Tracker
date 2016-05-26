#include "../include/inputParams.h"

inputParams::inputParams()
{

}

bool inputParams::initInputParams(vcl_vector<vcl_string> fnames, int lipx, int lipy, float lsd, float llr)
{

	inputParams::filenames = fnames;
	inputParams::ipx = lipx;
    inputParams::ipy = lipy;
    inputParams::sd = lsd;
    inputParams::lr = llr;

}

inputParams::~inputParams()
{
    //delete this;
}
bool inputParams::textParamsReader()
{
    string line, name, value;
    string directory ;
	string extension ;
    int lipx=0; int lipy=0; float lsd=0; float llr=0;// default values
    ifstream textFile ("../../Params/Params.txt");
	if (textFile.is_open())
	{
        int paramCount=0;
		while (textFile.good())
		{
			getline(textFile,line);
			name=line.substr(0,line.find(':'));
            value=line.substr(line.find(':')+1);// from position of = to

            if (name.compare("directory")==0) {directory=value.c_str(); paramCount++;}
            else if (name.compare("extension")==0) {extension=value.c_str(); paramCount++;}
            else if (name.compare("ipx")==0) {lipx=atoi(value.c_str()); paramCount++;}
            else if (name.compare("ipy")==0) {lipy=atoi(value.c_str()); paramCount++;}
            else if (name.compare("sd")==0) {lsd=atof(value.c_str());paramCount++;}
            else if (name.compare("lr")==0) {llr=atof(value.c_str()); paramCount++;}
          	}

		textFile.close();
           // vcl_cout <<directory << endl;
           // vcl_cout <<extension << endl;
           // vcl_cout <<lipx << endl;
           // vcl_cout <<lipy << endl;
           // vcl_cout <<lsd << endl;
           // vcl_cout <<llr << endl;
            //vcl_cout <<"Number of parameter found= "<< paramCount << endl;


		vcl_vector<vcl_string> filenames;
        for (vul_file_iterator fn=(directory + "/*" + extension); fn; ++fn)
            {
            if (!vul_file::is_directory(fn()))
            {
                filenames.push_back (fn());
            }
        }

        if (filenames.size() == 0 || lipy<= 0|| lipx<=0 || lsd<= 0 || llr<=0 || paramCount< 6)
        {
            vcl_cout << "No input files at given path OR Invalid Parameters detected" << vcl_endl;
            vcl_cout << "Check The Parameter Text File Structure. Example of Valid Text File structure must be as follows"<<endl;
            vcl_cout << "directory:../../Data/bolt/bolt"<<endl;
            vcl_cout << "extension:../../*.jpg"<<endl;
            vcl_cout << "ipx:3"<<endl;
            vcl_cout << "ipy:1"<<endl;
            vcl_cout << "sd:1.0"<<endl;
            vcl_cout << "lr:2.0"<<endl;
            vcl_cout << "Exiting form Program"<<endl;
            return false;
        }
        else {
            //params.initInputParams(filenames,lipx,lipy,lsd,llr);
            vcl_cout << "Parameters are being initialized Through Text Parameter File" <<vcl_endl;
            this->initInputParams(filenames,lipx,lipy,lsd,llr);
            vcl_cout << "There are " << filenames.size() <<" frames in the selected directory"<< vcl_endl;
            //vcl_cout << "There are " << filenames.size() <<" frames in the selected directory"<< vcl_endl;
            //vcl_cout << "Initial Position x for DFT = "<< lipx <<vcl_endl;
            //vcl_cout << "Initial Position y for DFT = "<< lipy <<vcl_endl;
            //vcl_cout << "Maximum Search Distance for DFT = "<< lsd <<vcl_endl;
            //vcl_cout << "Learning Rate for DFT = "<< llr <<vcl_endl;
            return true;
            }

	}
	else
	{
		 vcl_cout << "Unable to open  parameter file from application directory" << endl;
	return false;
	}

}
