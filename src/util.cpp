#include "util.h"


// string split function
vector<string> split(const string& s, const string& delim)
{
    vector<string> elems;
    size_t pos = 0;
    size_t len = s.length();
    size_t delim_len = delim.length();
    if (delim_len == 0) return elems;
    while (pos < len)
    {
        int find_pos = s.find(delim, pos);
        if (find_pos < 0)
        {
            elems.push_back(s.substr(pos, len - pos));
            break;
        }
        elems.push_back(s.substr(pos, find_pos - pos));
        pos = find_pos + delim_len;
    }
    return elems;
}

// transform double to string with specified fixed precision
string double2Str(double num, int32_t fixedPrecision){
	string num_str, tmp_str;
	int32_t i, dot_idx, float_bit_num, len;
	char ch;

	tmp_str = to_string(num);
	dot_idx = tmp_str.find(".");
	if(dot_idx>0){
		float_bit_num = (int32_t)tmp_str.size() - 1 - dot_idx;
		if(float_bit_num>fixedPrecision){
			ch = tmp_str.at(dot_idx+fixedPrecision+1);
			len = dot_idx + fixedPrecision + 1;
			if(ch<'5') num_str = tmp_str.substr(0, len);
			else tmp_str.at(dot_idx+fixedPrecision) ++;
		}else{
			num_str = tmp_str;
			len = fixedPrecision - float_bit_num;
			for(i=0; i<len; i++) num_str += "0";
		}
	}else{
		num_str = tmp_str;
		if(fixedPrecision>0){
			num_str += ".";
			for(i=0; i<fixedPrecision; i++) num_str += "0";
		}
	}

	return num_str;
}

void printSV(vector<SV_item*> &data){
	string data_out, sv_type_str;
	SV_item *item;

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
		cout << data_out << endl;
	}
}
