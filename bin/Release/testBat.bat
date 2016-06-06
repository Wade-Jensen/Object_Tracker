REM Test File, runs the program from command line with bad arguments, then from config file then with a bunch of different possible command line options

REM -ipx missing
ObjectTracker.exe -path ../../Data/bolt -glob .jpg -ipy 165 -w 25 -h 60 -c 8 -sb 4 -bc 1 -sd 30 -planes 3 -lr 0.05 -sds 1.0 -sdc 0.625 -odir Output/outputBolt

REM -path does not exist
ObjectTracker.exe -path /doesnotexist -glob .jpg -ipx 336 -ipy 165 -w 25 -h 60 -c 8 -sb 4 -bc 1 -sd 30 -planes 3 -lr 0.05 -sds 1.0 -sdc 0.625 -odir Output/outputBolt

REM there are no png files in the directory, so no files should be found
ObjectTracker.exe -path ../../Data/bolt -glob .png -ipx 336 -ipy 165 -w 25 -h 60 -c 8 -sb 4 -bc 1 -sd 30 -planes 3 -lr 0.05 -sds 1.0 -sdc 0.625 -odir Output/outputBolt

REM only some arguments specified
ObjectTracker.exe -path ../../Data/bolt -glob .jpg -ipx 336 -ipy 165 -w 25 -h 60

REM config file option
ObjectTracker.exe

REM correct arguments start here
ObjectTracker.exe -path ../../Data/bolt -glob .jpg -ipx 336 -ipy 165 -w 25 -h 60 -c 8 -sb 4 -bc 1 -sd 30 -planes 3 -lr 0.05 -sds 1.0 -sdc 0.625 -odir Output/outputBolt

ObjectTracker.exe -path ../../Data/bicycle -glob .jpg -ipx 154 -ipy 94 -w 18 -h 48 -c 8 -sb 4 -bc 1 -sd 30 -planes 3 -lr 0.05 -sds 1.0 -sdc 0.625 -odir Output/outputBicycle

ObjectTracker.exe -path ../../Data/cup -glob .jpg -ipx 125 -ipy 92 -w 47 -h 59 -c 8 -sb 4 -bc 1 -sd 30 -planes 3 -lr 0.05 -sds 1.0 -sdc 0.625 -odir Output/outputCup

ObjectTracker.exe -path ../../Data/juice -glob .jpg -ipx 131 -ipy 50 -w 37 -h 90 -c 8 -sb 4 -bc 1 -sd 30 -planes 3 -lr 0.05 -sds 1.0 -sdc 0.625 -odir Output/outputJuice

ObjectTracker.exe -path ../../Data/jump -glob .jpg -ipx 328 -ipy 189 -w 53 -h 50 -c 8 -sb 4 -bc 1 -sd 30 -planes 3 -lr 0.05 -sds 1.0 -sdc 0.625 -odir Output/outputJump
