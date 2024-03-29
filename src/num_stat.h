#ifndef SRC_NUM_STAT_H_
#define SRC_NUM_STAT_H_

#include <iostream>
#include <cstring>
#include <vector>
#include <limits.h>
#include <sys/stat.h>

#include "constants.h"
#include "structure.h"

using namespace std;


void SVNumStat(string &user_file, string &benchmark_file, int32_t max_valid_reg_thres, string &outputPathname);
void SVNumStatOp(string &user_file, string &benchmark_file, int32_t max_valid_reg_thres, string &dirname);
void computeNumStat(vector<SV_item*> &sv_data1, vector<SV_item*> &sv_data2, string &file_prefix);

void computeLenStat(vector<SV_item*> &data, string &description_str);

vector<vector<SV_item*>> intersect(vector<SV_item*> &data1, vector<SV_item*> &data2);
SV_item* itemdup(SV_item* item);
vector<size_t> computeOverlapType(SV_item* item1, SV_item* item2);
bool isOverlappedPos(size_t startPos1, size_t endPos1, size_t startPos2, size_t endPos2);

void destroyResultData(vector<vector<SV_item*>> &result);

void SVNumStatTraOp(string &user_file, string &benchmark_file, string &dirname);
void computeNumStatTra(vector<SV_item*> &sv_data1, vector<SV_item*> &sv_data2, string &file_prefix);
void computeOverlapTra(vector<SV_item*> &data1, vector<SV_item*> &data2);
void computeBPNumStatTra(vector<Breakpoint_t*> &bp_vec_user, vector<Breakpoint_t*> &bp_vec_benchmark, string &file_prefix);
Breakpoint_t *allocateBPItem(string &chrname, size_t bp_loc);
void destroyBPData(vector<Breakpoint_t*> &bp_vec);
vector<Breakpoint_t*> constructBPVec(vector<SV_item*> &sv_data);
void outputBP2File(string &filename, vector<Breakpoint_t*> &bp_vec);

#endif /* SRC_NUM_STAT_H_ */
