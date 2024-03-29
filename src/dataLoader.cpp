#include "dataLoader.h"
#include "util.h"

// load data
vector<SV_item*> loadData(const string &filename){
	vector<SV_item*> sv_vec;
	ifstream infile;
	string line, str_tmp;
	vector<string> str_vec;
	SV_item *item;

	infile.open(filename);
	if(!infile.is_open()){
		cerr << __func__ << ", line=" << __LINE__ << ": cannot open file:" << filename << endl;
		exit(1);
	}

	// load data
	while(getline(infile, line)){
		if(line.size()){
			item = constructSVItem(line);

			if(item) sv_vec.push_back(item);
		}
	}
	infile.close();

	return sv_vec;
}

// load data without TRA
vector<SV_item*> loadDataWithoutTra(const string &filename){
	vector<SV_item*> sv_vec;
	ifstream infile;
	string line;
	SV_item *item;

	infile.open(filename);
	if(!infile.is_open()){
		cerr << __func__ << ", line=" << __LINE__ << ": cannot open file:" << filename << endl;
		exit(1);
	}

	// load data
	while(getline(infile, line)){
		if(line.size()){
			item = constructSVItem(line);

			if(item){
				if(item->sv_type!=VAR_TRA and item->sv_type!=VAR_BND) sv_vec.push_back(item);
				else delete item;
			}
		}
	}
	infile.close();

	return sv_vec;
}

// load TRA data
vector<SV_item*> loadDataTra(string &filename){
	vector<SV_item*> sv_vec;
	ifstream infile;
	string line;
	SV_item *item;

	infile.open(filename);
	if(!infile.is_open()){
		cerr << __func__ << ", line=" << __LINE__ << ": cannot open file:" << filename << endl;
		exit(1);
	}

	// load data
	while(getline(infile, line)){
		if(line.size()){
			item = constructSVItem(line);

			if(item){
				if(item->sv_type==VAR_TRA or item->sv_type==VAR_BND) sv_vec.push_back(item);
				else delete item;
			}
		}
	}
	infile.close();

	return sv_vec;
}

SV_item *constructSVItem(string &line){
	SV_item *item = NULL;
	string str_tmp;
	vector<string> str_vec;

	if(line.size()>0){
		str_vec = split(line, "\t");

		item = new SV_item();
		item->chrname = str_vec.at(0);
		if(str_vec.at(1).compare("-")==0) item->startPos = 0;
		else item->startPos = stoi(str_vec.at(1));
		if(str_vec.at(2).compare("-")==0) item->endPos = 0;
		else item->endPos = stoi(str_vec.at(2));
		item->chrname2 = "";
		item->startPos2 = 0;
		item->endPos2 = 0;

		str_tmp = str_vec.at(3);
		if(str_tmp.compare("INS")==0 or str_tmp.compare("insertion")==0){
			item->sv_type = VAR_INS;
		}else if(str_tmp.compare("DEL")==0 or str_tmp.compare("deletion")==0){
			item->sv_type = VAR_DEL;
		}else if(str_tmp.compare("DUP")==0 or str_tmp.compare("duplication")==0){
			item->sv_type = VAR_DUP;
		}else if(str_tmp.compare("INV")==0 or str_tmp.compare("inversion")==0){
			item->sv_type = VAR_INV;
		}else{
			if(str_vec.size()>=8 and (str_vec.at(6).compare("TRA")==0 or str_vec.at(6).compare("translocation")==0 or str_vec.at(6).compare("BND")==0)){
				item->chrname2 = str_vec.at(3);
				if(str_vec.at(4).compare("-")==0) item->startPos2 = 0;
				else item->startPos2 = stoi(str_vec.at(4));
				if(str_vec.at(5).compare("-")==0) item->endPos2 = 0;
				else item->endPos2 = stoi(str_vec.at(5));
				if(str_vec.at(6).compare("TRA")==0 or str_vec.at(6).compare("translocation")==0) item->sv_type = VAR_TRA;
				else item->sv_type = VAR_BND;

				//item->isTraBreakpointFlag = item->isTraBreakpointFlag2 = false;
				for(size_t i=0; i<4; i++) item->traOverlappedArr[i] = false;
			}else
				item->sv_type = VAR_MIX;
		}

		if(item->sv_type==VAR_TRA or item->sv_type==VAR_BND) item->sv_len = 0;
		else item->sv_len = stoi(str_vec.at(4));
		item->overlapped = false;
	}

	return item;
}

void destroyData(vector<SV_item*> &sv_vec){
	vector<SV_item*>::iterator it;
	for(it=sv_vec.begin(); it!=sv_vec.end(); it++)
		delete *it;
	vector<SV_item*>().swap(sv_vec);
}

vector<SV_item*> getLongSVReg(vector<SV_item*> &dataset, int32_t thres){
	vector<SV_item*> sv_vec;
	size_t i;
	int32_t reg_len;
	SV_item *sv_item;

	for(i=0; i<dataset.size(); ){
		sv_item = dataset.at(i);
		reg_len = 0;
		if(sv_item->sv_type!=VAR_TRA and sv_item->sv_type!=VAR_BND)
			reg_len = sv_item->endPos - sv_item->startPos + 1;

		if(reg_len>thres){
			sv_vec.push_back(sv_item);
			dataset.erase(dataset.begin()+i);
		}else i++;
	}

	return sv_vec;
}

void output2File(const string &filename, vector<SV_item*> &data, ofstream &logfile){
	ofstream outfile;
	string data_out, sv_type_str;
	SV_item *item;

	outfile.open(filename);
	if(!outfile.is_open()){
		cerr << __func__ << ", line=" << __LINE__ << ": cannot open file:" << filename << endl;
		exit(1);
	}

	for(size_t i=0; i<data.size(); i++){
		item = data.at(i);

		switch(item->sv_type){
			case VAR_UNC: sv_type_str = "UNC"; break;
			case VAR_INS: sv_type_str = "INS"; break;
			case VAR_DEL: sv_type_str = "DEL"; break;
			case VAR_DUP: sv_type_str = "DUP"; break;
			case VAR_INV: sv_type_str = "INV"; break;
			case VAR_TRA: sv_type_str = "TRA"; break;
			case VAR_BND: sv_type_str = "BND"; break;
			case VAR_INV_TRA: sv_type_str = "INVTRA"; break;
			case VAR_MIX: sv_type_str = "MIX"; break;
			default:
				cerr << "line=" << __LINE__ << ", invalid sv type: " << item->sv_type << endl;
				exit(1);
		}

		if(item->sv_type!=VAR_TRA and item->sv_type!=VAR_BND)
			data_out = item->chrname + "\t" + to_string(item->startPos) + "\t" + to_string(item->endPos) + "\t" + sv_type_str + "\t" + to_string(item->sv_len);
		else
			data_out = item->chrname + "\t" + to_string(item->startPos) + "\t" + to_string(item->endPos) + "\t" + item->chrname2 + "\t" + to_string(item->startPos2) + "\t" + to_string(item->endPos2) + "\t" + sv_type_str + "\t" + to_string(item->sv_len);
		outfile << data_out << endl;
	}

	cout << data.size() << " items were saved to " << filename << endl;
	logfile << data.size() << " items were saved to " << filename << endl;

	outfile.close();
}

