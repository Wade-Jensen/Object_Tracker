#include "../include/inputParams.h"

inputParams::inputParams()
{

}

bool inputParams::initInputParams(vcl_vector<vcl_string> fnames, int lipx, int lipy, int lw, int lh, int lc, int lsb, int lbc, int lsd, int lplanes, float llr, float lsds, float lsdc, vcl_string lodir)
{

	inputParams::filenames = fnames;
	inputParams::ipx = lipx;
    inputParams::ipy = lipy;
    inputParams::w = lw;
    inputParams::h = lh;
    inputParams::c = lc;
    inputParams::sb = lsb;
    inputParams::bc = lbc;
    inputParams::sd = lsd;
    inputParams::lr = llr;
    inputParams::sds = lsds;
    inputParams::sdc = lsdc;
    inputParams::odir = lodir;
    inputParams::planes = lplanes;

    return true;

}

inputParams::~inputParams()
{
    //delete this;
}
bool inputParams::parseCli(int argc, char * argv[])
{
        vul_arg<vcl_string>
		arg_in_path("-path", "Path to Image Frames, e.g. C:/somefiles/"),
		arg_in_glob("-glob", "Image (Frame) Extension (png,jpg,bmp,tiff,jpeg), e.g. *.jpeg, this will get all jpeg's.");

	// now we have some integer arguments
	vul_arg<unsigned int> arg_ipx("-ipx", "Initial Position x .", -1),
                          arg_ipy("-ipy", "Initial Position y.", -1),
                          arg_w("-w", "Width.", -1),
                          arg_h("-h", "Heignt", -1),
                          arg_c("-c", "No Of Channels", -1),
                          arg_sb("-sb", "Spatial Blur", -1),
                          arg_bc("-bc", "Blur Colour", -1),
                          arg_sd("-sd", "Max Search Distance", -1),
                          arg_planes("-planes", "Number of colour planes in DF", -1);

    // float arguments
	vul_arg<float> arg_lr("-lr", "Learning Rate", -1),
                   arg_sds("-sds", "SD Spatial", -1),
                   arg_sdc("-sdc", "SD Colour", -1);

    // last string argument
    vul_arg<vcl_string> arg_odir("-odir", "Output Frames Storage Director", "Output");
	vul_arg_parse(argc, argv, true);

	if(((arg_in_path() == "") || (arg_in_glob() == ""))) // command line args are not passed perfectly
    {
        vcl_cout << "Inufficient number of or incorrrect argument parameters detected..." <<vcl_endl;
        return false;
    }
    else
    {
        // set input object member variable values
        ipx = arg_ipx();
        ipy = arg_ipy();
        w = arg_w();
        h = arg_h();
        c = arg_c();
        sb = arg_sb();
        bc = arg_bc();
        sds = arg_sds();
        sdc = arg_sdc();
        planes = arg_planes();
        lr = arg_lr();
        sd = arg_sd();
        odir = arg_odir();

        vcl_string directory = arg_in_path();
        vcl_string extension = arg_in_glob();

        for (vul_file_iterator fn=(directory + "/*" + extension); fn; ++fn)
            {
            if (!vul_file::is_directory(fn()))
            {
                filenames.push_back (fn());
            }
        }

        vcl_cout << "There are " << filenames.size() <<" frames in the selected directory"<< vcl_endl;
        vcl_cout << "Input Parameters Initialized By Command Line Arguments";

        return true;
    }
    // Uncomment to print statements checking correct read in of parameters:
    /*
    vcl_cout << "ipx:"<<params.ipx<<"; "<<x <<endl;
    vcl_cout << "ipy:"<<params.ipy<<"; "<< y<<endl;
    vcl_cout << "width:"<<params.w<<"; "<< width<<endl;
    vcl_cout << "height:"<<params.h<<"; "<< height <<endl;
    vcl_cout << "numChannels:"<<params.c<<"; "<<numChannels<<endl;
    vcl_cout << "blurSpatial:"<<params.sb<<"; "<<blurSpatial <<endl;
    vcl_cout << "blurColour:"<<params.bc<<"; "<< blurColour <<endl;
    vcl_cout << "sdSpatial:"<<params.sds<<"; "<< sdSpatial <<endl;
    vcl_cout << "sdColour:"<<params.sdc<<"; "<< sdColour <<endl;
    vcl_cout << "maxSearchDist:"<<params.sd<<"; "<< maxSearchDist<<endl;
    vcl_cout << "learningRate:"<<params.lr<<"; "<< learningRate<<endl;
    */
}


bool inputParams::parseTxt(vcl_string configFile)
{
    string line, name, value;
    string directory;
	string extension;
    //int lipx=0; int lipy=0; float lsd=0; float llr=0;// default values
    // Assigning Defualt Values
    int lipx=-1; int lipy=-1; int lw=-1; int lh=-1; int lc=-1; int lsb=-1; int lbc=-1; int lsd=-1; int lplanes=-1;
    float llr=-1; float lsds=-1; float lsdc=-1;

    vcl_string lodir="output";
    ifstream textFile (configFile.c_str());//
	if (textFile.is_open())
	{
        int paramCount=0;
		while (textFile.good())
		{
			getline(textFile,line);
			name=line.substr(0,line.find(' '));
            value=line.substr(line.find(' ')+1);// from position of " " to

            if (name.compare("directory")==0) {directory=value.c_str(); paramCount++;}
            else if (name.compare("extension")==0) {extension=value.c_str(); paramCount++;}
            else if (name.compare("ipx")==0) {lipx=atoi(value.c_str()); paramCount++;}
            else if (name.compare("ipy")==0) {lipy=atoi(value.c_str()); paramCount++;}
            else if (name.compare("width")==0) {lw=atoi(value.c_str()); paramCount++;}
            else if (name.compare("height")==0) {lh=atoi(value.c_str());paramCount++;}
            else if (name.compare("numChannels")==0) {lc=atoi(value.c_str());paramCount++;}
            else if (name.compare("blurSpatial")==0) {lsb=atoi(value.c_str());paramCount++;}
            else if (name.compare("blurColour")==0) {lbc=atoi(value.c_str());paramCount++;}
            else if (name.compare("sdSpatial")==0) {lsds=atoi(value.c_str());paramCount++;}
            else if (name.compare("sdColour")==0) {lsdc=atof(value.c_str());paramCount++;}
            else if (name.compare("learningRate")==0) {llr=atof(value.c_str()); paramCount++;}
            else if (name.compare("maxSearchDist")==0) {lsd=atoi(value.c_str()); paramCount++;}
            else if (name.compare("outputDir")==0) {lodir=value.c_str(); paramCount++;}
            else if (name.compare("planes")==0) {lplanes=atoi(value.c_str()); paramCount++;}
            //paramCount++;
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

        if (filenames.size() == 0 || lipy<= 0|| lipx<=0 || lsd<= 0 || llr<=0 || lw<=0|| lh<=0 || lc<=0 ||lsb<=0 ||lbc<=0 ||llr<=0 || lsd<=0 || planes<=0 || paramCount< 14)
        {
            vcl_cout << "No input Parameter file 'config.txt' found in working dir OR invalid Parameters detected" << vcl_endl;
            //vcl_cout << "Check The Parameter Text File Structure. Example of Valid Text File structure must be as follows"<<endl;
            vcl_cout << "Check the Parameter text file structure"<<endl;
            //vcl_cout << "directory:../../Data/bolt"<<endl;
            //vcl_cout << "extension:*.jpg"<<endl;
            //vcl_cout << "ipx:3"<<endl;
            //vcl_cout << "ipy:1"<<endl;
            //vcl_cout << "sd:1.0"<<endl;
            //vcl_cout << "lr:2.0"<<endl;
            vcl_cout << "Exiting from program"<< vcl_endl;
            return false;
        }
        else {
            //params.initInputParams(filenames,lipx,lipy,lsd,llr);
            vcl_cout << "Parameters initialized via text file" <<vcl_endl;
            //initInputParams(filenames,lipx,lipy,lsd,llr);
            initInputParams(filenames,  lipx,  lipy,  lw,  lh,  lc,  lsb,  lbc,  lsd, lplanes,  llr,  lsds,  lsdc, lodir);
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

/*

if(((arg_in_path() == "") || (arg_in_glob() == ""))) // command line args are not passed perfectly
    {
        vcl_cout << "Not sufficient or incorrrect argument parameters detected..." <<vcl_endl;
        vcl_cout <<"Utilizing parameter text file method to obtain input parameters"<<endl;
        vcl_cout << "Checking parameter text file for parameters..." <<vcl_endl;
        bool parametersExtracted = params.parseTxt();
        if (parametersExtracted == false) // failed to capture all parameters
            {
            vcl_cout << "Text file doesn't contain valid parameters, exiting" <<vcl_endl;
            return 0;
            }
    }



        // check input files and for valid pixel positions
        if (filenames.size() == 0 || arg_ipy()<= 0|| arg_ipx()<=0 || arg_sd()<= 0 || arg_lr()<=0)
        {
            vcl_cout << "No input files at given path OR parameters are incorrect" << vcl_endl;
            vcl_cout << "Exiting from program"<< endl;
            return 0;
        }

        else // parameters given by command line Argument
        {
            // calling InputParams Class Constructor
            params.initInputParams(filenames,  arg_ipx(),  arg_ipy(),  arg_w(),  arg_h(),  arg_c(),  arg_sb(),  arg_bc(),  arg_sd(), arg_planes(), arg_lr(),  arg_sds(),  arg_sdc(), arg_odir());
            vcl_cout << "There are " << filenames.size() <<" frames in the selected directory"<< vcl_endl;
            vcl_cout << "Input Parameters Initialized By Command Line Arguments";
        }

*/
