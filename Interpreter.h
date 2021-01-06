#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
//
// Created by Lommy Reznak on 1/6/21.
//

#ifndef EXPERT_SYSTEM_INTERPRETER_H
#define EXPERT_SYSTEM_INTERPRETER_H

#include <unordered_map>
#include <vector>
#include "expertNode.h"

class Interpreter {
	std::unordered_map<char, expert_system::Value> storage_;
	std::vector<expert_system::iExpertNode*> expressions_;

	expert_system::Value getValueByVarName(char vn) const;
	void setVarWithValue(expert_system::Value vl, char vn);

	void setVarWithValue(expert_system::Value vl);
};


#endif //EXPERT_SYSTEM_INTERPRETER_H

#pragma clang diagnostic pop