#include "../include/UserInput.h"

UserInput::UserInput()
{

}

const struct Params UserInput::getParams()
{
    const struct Params params = {_filenames, _initialX,_initialY,_width,_height,_numChannels,
                                   _blurSpatial,_blurColour,_maxSearchDist, _planes,
                                   _learningRate, _sdSpatial,_sdColour, _outputDir };
    return params;
}
void UserInput::initInputParams(vcl_vector<vcl_string> fnames, int lipx, int lipy, int lw, int lh, int lc, int lsb, int lbc, int lsd, int lplanes, float llr, float lsds, float lsdc, vcl_string lodir)
{

	_filenames = fnames;
	_initialX = lipx;
    _initialY = lipy;
    _width = lw;
    _height = lh;
    _numChannels = lc;
    _blurSpatial = lsb;
    _blurColour = lbc;
    _maxSearchDist = lsd;
    _learningRate = llr;
    _sdSpatial = lsds;
    _sdColour = lsdc;
    _outputDir = lodir;
    _planes = lplanes;
}

UserInput::~UserInput()
{
    //delete this;
}
bool UserInput::parseCli(int argc, char * argv[])
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

    // Assigning Default Values
    int lipx=-1; int lipy=-1; int lw=-1; int lh=-1; int lc=-1; int lsb=-1; int lbc=-1; int lsd=-1; int lplanes=-1;
    float llr=-1; float lsds=-1; float lsdc=-1;
    vcl_string lodir="output";

	if(((arg_in_path() == "") || (arg_in_glob() == ""))) // command line args are not passed perfectly
    {
        vcl_cout << "Inufficient number of or incorrrect argument parameters detected..." <<vcl_endl;
        return false;
    }
    else
    {
        // set input object member variable values
        lipx = arg_ipx();
        lipy = arg_ipy();
        lw = arg_w();
        lh = arg_h();
        lc = arg_c();
        lsb = arg_sb();
        lbc = arg_bc();
        lsds = arg_sds();
        lsdc = arg_sdc();
        lplanes = arg_planes();
        llr = arg_lr();
        lsd = arg_sd();
        lodir = arg_odir();

        vcl_string directory = arg_in_path();
        vcl_string extension = arg_in_glob();
        //vcl_cout << "extension: "<<arg_in_glob()<<"; " <<endl;
        vcl_vector <vcl_string> filenames;

        for (vul_file_iterator fn=(directory + "/*" + extension); fn; ++fn)
            {
            if (!vul_file::is_directory(fn()))
            {
                filenames.push_back (fn());
            }
        }

        //vcl_cout << "directory: "<<arg_in_path()<<"; " <<endl;
        //vcl_cout << "extension: "<<arg_in_glob()<<"; " <<endl;
        //vcl_cout << "ipx:"<<lipx<<"; " <<endl;
        //vcl_cout << "ipy:"<<lipy<<"; "<<endl;
        //vcl_cout << "width:"<<lw<<"; "<<endl;
        //vcl_cout << "height:"<<lh<<"; " <<endl;
        //vcl_cout << "numChannels:"<<lc<<"; "<<endl;
        //vcl_cout << "blurSpatial:"<<lsb<<"; " <<endl;
        //vcl_cout << "blurColour:"<<lbc<<"; " <<endl;
        //vcl_cout << "sdSpatial:"<<lsds<<"; " <<endl;
        //vcl_cout << "sdColour:"<<lsdc<<"; " <<endl;
        //vcl_cout << "maxSearchDist:"<<lsd<<"; "<<endl;
        //vcl_cout << "learningRate:"<<llr<<"; "<<endl;
        //vcl_cout << "planes: "<<llr<<"; " << endl;
        //vcl_cout << "output directory: "<<lodir<<"; " << endl;
        //vcl_cout << "filenames size: "<<filenames.size()<<"; " << endl;

        if (filenames.size() <= 0 || lipy<= 0|| lipx<=0 || lsd<= 0 || llr<=0 || lw<=0|| lh<=0 || lc<=0 ||lsb<=0 ||lbc<=0 || lsds<=0 || lsdc<=0 || lplanes<=0 || (lodir==""))
        {
            vcl_cout << "Distribution field tracking parameters either missing or out of bounds."<< vcl_endl;
            return false;
        }
        else
        {
            vcl_cout << "Parameters initialized via command line arguments." <<vcl_endl;
            initInputParams(filenames,  lipx,  lipy,  lw,  lh,  lc,  lsb,  lbc,  lsd, lplanes,  llr,  lsds,  lsdc, lodir);
            vcl_cout << "There are " << _filenames.size() <<" frames in the selected directory."<< vcl_endl;
            return true;
        }
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


bool UserInput::parseTxt(vcl_string configFile)
{
    string line, name, value;
    string directory;
	string extension;
    // Assigning Default Values
    int lipx=-1; int lipy=-1; int lw=-1; int lh=-1; int lc=-1; int lsb=-1; int lbc=-1; int lsd=-1; int lplanes=-1;
    float llr=-1; float lsds=-1; float lsdc=-1;
    vcl_vector<vcl_string> filenames;

    vcl_string lodir="output";
    ifstream textFile (configFile.c_str());//
	if (!textFile.is_open())
	{
	    vcl_cout << "Could not find config.txt, please add parameter file." << vcl_endl;
        return 0;
	}
	else if (textFile.is_open())
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
        if(((directory == "") || (extension == ""))) // command line args are not passed perfectly
        {
            vcl_cout << "Input directory and/or extension not specified." <<vcl_endl;
            return false;
        }
        else
        {
            for (vul_file_iterator fn=(directory + "/*" + extension); fn; ++fn)
            {
                if (!vul_file::is_directory(fn()))
                {
                    filenames.push_back (fn());
                }
            }
        }
        if (filenames.size() <= 0 || lipy<= 0|| lipx<=0 || lsd<= 0 || llr<=0 || lw<=0|| lh<=0 || lc<=0 ||lsb<=0 ||lbc<=0 || lsds<=0 || lsdc<=0 || lplanes<=0 || (lodir==""))
        {
            vcl_cout << "Distribution field tracking parameters either missing or out of bounds."<< vcl_endl;
            return false;
        }
        else
        {
            vcl_cout << "Parameters initialized via text file." <<vcl_endl;
            initInputParams(filenames,  lipx,  lipy,  lw,  lh,  lc,  lsb,  lbc,  lsd, lplanes,  llr,  lsds,  lsdc, lodir);
            vcl_cout << "There are " << _filenames.size() <<" frames in the selected directory."<< vcl_endl;
            return true;
        }
    }
    // Uncomment to print statements checking correct read in of parameters:
    /*
    vcl_cout << "ipx:"<<_initialX<<"; "<<x <<endl;
    vcl_cout << "ipy:"<<_initialY<<"; "<< y<<endl;
    vcl_cout << "width:"<<_width<<"; "<< width<<endl;
    vcl_cout << "height:"<<_height<<"; "<< height <<endl;
    vcl_cout << "numChannels:"<<_numChannels<<"; "<<numChannels<<endl;
    vcl_cout << "blurSpatial:"<<_blurSpatial<<"; "<<blurSpatial <<endl;
    vcl_cout << "blurColour:"<<_blurColour<<"; "<< blurColour <<endl;
    vcl_cout << "sdSpatial:"<<_sdSpatial<<"; "<< sdSpatial <<endl;
    vcl_cout << "sdColour:"<<sdColour<<"; "<< sdColour <<endl;
    vcl_cout << "maxSearchDist:"<<_maxSearchDist<<"; "<< maxSearchDist<<endl;
    vcl_cout << "learningRate:"<<_learningRate<<"; "<< learningRate<<endl;
    */
}
