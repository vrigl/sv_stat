#include "paras.h"
#include "global.h"

// show usage
void showUsage(){

	cout << "Usage:  sv_stat  <command> [options]" << endl << endl;

	cout << "Commands:" << endl;
	cout << "     convert      convert SV results to 5-column BED (or 8-column BEDPE) file format" << endl;
	cout << "     stat         compute the SV statistics" << endl;
}

// show usage for convert command
void showUsageConvert(){

	cout << "Usage:  sv_stat convert [options] <infile> <outfile>" << endl << endl;

	cout << "Options:" << endl;
	cout << "     -f STR       SV input file format (required):" << endl;
	cout << "                  bed: BED/BEDPE format" << endl;
	cout << "                  vcf: VCF format" << endl;
	cout << "                  csv: CSV format" << endl;
	cout << "     -d INT       remove duplicated variant items [1]: 1 for yes, 0 for no" << endl;
	cout << "     -h           show this help message and exit" << endl;
}

// show usage for stat command
void showUsageStat(){

	cout << "Usage:  sv_stat stat [options] <USER_SV_FILE> <BENCHMARK_SV_FILE>" << endl << endl;

	cout << "Description:" << endl;
	cout << "     USER_SV_FILE         User called SV result file." << endl;
	cout << "     BENCHMARK_SV_FILE    Benchmark SV file." << endl << endl;

	cout << "Options:" << endl;
	cout << "     -m INT       valid maximal region size for statistics: [0]" << endl;
	cout << "                  0 is for all variant size are valid, and while positive" << endl;
	cout << "                  values are for the valid maximal region size, then longer" << endl;
	cout << "                  regions are omitted and saved to the file specified with -l" << endl;
	cout << "     -s INT       breakpoint extend size: [10]" << endl;
	cout << "     -o FILE      output path name for SV statistics: [output]" << endl;
	cout << "     -l FILE      file name for long SV regions: [long_sv_reg.bed]" << endl;
	cout << "     -h           show this help message and exit" << endl;
}

// parse the parameters for convert command
int parseConvert(int argc, char **argv)
{
	int opt;
	string sv_format, in_file, out_file, remove_dup_str = "1";
	bool remove_dup_falg;

	while( (opt = getopt(argc, argv, ":f:d:h")) != -1 ){
		switch(opt){
			case 'f': sv_format = optarg; break;
			case 'd': remove_dup_str = optarg; break;
			case 'h': showUsageConvert(); exit(0);
			case '?': cout << "unknown option -" << (char)optopt << endl; exit(1);
			case ':': cout << "the option -" << (char)optopt << " needs a value" << endl; exit(1);
		}
	}

	if(remove_dup_str.compare("1")==0) remove_dup_falg = true;
	else if(remove_dup_str.compare("0")==0) remove_dup_falg = false;
	else{
		cout << "Error: Please specify the correct value for '-d' option" << endl << endl;
		showUsageConvert();
		return 1;
	}

	opt = argc - optind; // the number of SAMs on the command line
	if(opt==2) {
		in_file = argv[optind];
		out_file = argv[optind+1];
	}else { showUsageConvert(); return 1; }

	if(in_file.size()>0 and in_file.compare(out_file)==0){
		cout << "Please specify different SV file names" << endl << endl;
		showUsageConvert();
		return 1;
	}

	if(sv_format.compare("bed")==0 or sv_format.compare("vcf")==0 or sv_format.compare("csv")==0 or sv_format.compare("nm")==0) // nm: private usage
		convert(in_file, out_file, remove_dup_falg, sv_format);
	else{
		cout << "Error: Please specify the correct SV file format" << endl << endl;
		showUsageConvert();
		return 1;
	}

	return 0;
}

void convert(string &infilename, string &outfilename, bool remove_dup_falg, string &sv_format){

	outConvertScreenFile.open(convertScreenFilename);
	if(!outConvertScreenFile.is_open()){
		cerr << __func__ << ", line=" << __LINE__ << ": cannot open file:" << convertScreenFilename << endl;
		exit(1);
	}

	printConvertParas(infilename, outfilename, remove_dup_falg, sv_format); // print parameters

	cout << "############# Convert variants: #############" << endl;
	outConvertScreenFile << "############# Convert variants: #############" << endl;

	if(sv_format.compare("bed")==0)
		convertBed(infilename, outfilename, remove_dup_falg);
	else if(sv_format.compare("vcf")==0)
		convertVcf(infilename, outfilename, remove_dup_falg);
	else if(sv_format.compare("csv")==0)
		convertCsv(infilename, outfilename, remove_dup_falg);
	else if(sv_format.compare("nm")==0)  // private usage: nm
		convertNm(infilename, outfilename, remove_dup_falg);

	outConvertScreenFile.close();
}

// parse the parameters for stat command
int parseStat(int argc, char **argv){
	int32_t opt;
	string sv_file1, sv_file2;

	if (argc < 2) { showUsageStat(); return 1; }

	maxValidRegThres = 0;
	extendSize = ADJUST_SIZE;
	while( (opt = getopt(argc, argv, ":m:s:o:l:h")) != -1 ){
		switch(opt){
			case 'm': maxValidRegThres = stoi(optarg); break;
			case 's': extendSize = stoi(optarg); break;
			case 'o': outputPathname = optarg; break;
			case 'l': longSVFilename = optarg; break;
			case 'h': showUsageStat(); exit(0);
			case '?': cout << "unknown option -" << (char)optopt << endl; exit(1);
			case ':': cout << "the option -" << (char)optopt << " needs a value" << endl; exit(1);
		}
	}

	if(maxValidRegThres<0){
		cout << "Error: Please specify the correct positive value for option: -m" << endl;
		showUsageStat();
		return 1;
	}
	if(extendSize<=0){
		cout << "Error: Please specify the correct positive value for option: -s" << endl;
		showUsageStat();
		return 1;
	}
	if(outputPathname.at(outputPathname.size()-1)!='/') outputPathname += "/";

	opt = argc - optind; // the number of SAMs on the command line
	if(opt==2) {
		sv_file1 = argv[optind];
		sv_file2 = argv[optind+1];
	}else { showUsageStat(); return 1; }

	SVStat(sv_file1, sv_file2);

	return 0;
}

// SV stat
void SVStat(string &user_file, string &benchmark_file){

	mkdir(outputPathname.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	statScreenFilename = outputPathname + statScreenFilename;
	outStatScreenFile.open(statScreenFilename);
	if(!outStatScreenFile.is_open()){
		cerr << __func__ << ", line=" << __LINE__ << ": cannot open file:" << statScreenFilename << endl;
		exit(1);
	}

	printStatParas(user_file, benchmark_file); // print parameters

	cout << "############# Stage 1: SV size statistics: #############" << endl;
	outStatScreenFile << "############# Stage 1: SV size statistics: #############" << endl;
	refRegSizeStat(user_file, benchmark_file, maxValidRegThres);

	cout << "\n\n############# Stage 2: Num statistics: #############" << endl;
	outStatScreenFile << "\n\n############# Stage 2: Num statistics: #############" << endl;
	SVNumStat(user_file, benchmark_file, maxValidRegThres, outputPathname);

	// compute SV size difference statistics
	cout << "\n\n############# Stage 3: SV size difference statistics: #############" << endl;
	outStatScreenFile << "\n\n############# Stage 3: SV size difference statistics: #############" << endl;
	SVSizeDifStat(user_file, benchmark_file, maxValidRegThres);

	cout << "\n\n############# Stage 4: SV type and num statistics: #############" << endl;
	outStatScreenFile << "\n\n############# Stage 4: SV type and num statistics: #############" << endl;
	SVTypeNumStat(user_file, benchmark_file, maxValidRegThres);

	outStatScreenFile.close();
}

// print parameters for 'convert' command
void printConvertParas(string &infilename, string &outfilename, bool remove_dup_falg, string &sv_format){
	cout << "############# Parameters for 'convert' command: #############" << endl;

	cout << "                  Input SV file: " << infilename << endl;
	cout << "                 Output SV file: " << outfilename << endl;

	if(remove_dup_falg)
		cout << "Remove duplicated variant items: yes" << endl;
	else
		cout << "Remove duplicated variant items: no" << endl;

	if(sv_format.compare("bed")==0)
		cout << "              Input file format: BED" << endl;
	else if(sv_format.compare("vcf")==0)
		cout << "              Input file format: VCF" << endl;
	else if(sv_format.compare("csv")==0)
		cout << "              Input file format: CSV" << endl;
	else if(sv_format.compare("nm")==0)  // private usage: nm
		cout << "              Input file format: nm, [private usage]" << endl;
	cout << endl;

	// print to file
	outConvertScreenFile << "############# Parameters for 'convert' command: #############" << endl;

	outConvertScreenFile << "                  Input SV file: " << infilename << endl;
	outConvertScreenFile << "                 Output SV file: " << outfilename << endl;

	if(remove_dup_falg)
		outConvertScreenFile << "Remove duplicated variant items: yes" << endl;
	else
		outConvertScreenFile << "Remove duplicated variant items: no" << endl;

	if(sv_format.compare("bed")==0)
		outConvertScreenFile << "              Input file format: BED" << endl;
	else if(sv_format.compare("vcf")==0)
		outConvertScreenFile << "              Input file format: VCF" << endl;
	else if(sv_format.compare("csv")==0)
		outConvertScreenFile << "              Input file format: CSV" << endl;
	else if(sv_format.compare("nm")==0)  // private usage: nm
		outConvertScreenFile << "              Input file format: nm, [private usage]" << endl;
	outConvertScreenFile << endl;
}

// print parameters for 'stat' command
void printStatParas(string &user_file, string &benchmark_file){
	cout << "############# Parameters for 'stat' command: #############" << endl;
	cout << "               User-called SV file: " << user_file << endl;
	cout << "                 Benchmark SV file: " << benchmark_file << endl;

	cout << "Maximal region size for statistics: " << maxValidRegThres << endl;
	cout << "            breakpoint extend size: " << extendSize << endl;
	cout << "Output path name for SV statistics: " << outputPathname << endl;
	cout << "     File name for long SV regions: " << longSVFilename << endl << endl;

	// print to file
	outStatScreenFile << "############# Parameters for 'stat' command: #############" << endl;
	outStatScreenFile << "               User-called SV file: " << user_file << endl;
	outStatScreenFile << "                 Benchmark SV file: " << benchmark_file << endl;

	outStatScreenFile << "Maximal region size for statistics: " << maxValidRegThres << endl;
	outStatScreenFile << "            breakpoint extend size: " << extendSize << endl;
	outStatScreenFile << "Output path name for SV statistics: " << outputPathname << endl;
	outStatScreenFile << "     File name for long SV regions: " << longSVFilename << endl << endl;
}
