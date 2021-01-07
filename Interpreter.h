#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"

//
// Created by Lommy Reznak on 1/6/21.
//

#ifndef EXPERT_SYSTEM_INTERPRETER_H
#define EXPERT_SYSTEM_INTERPRETER_H

#include <map>
#include <vector>
#include <stack>
#include "expertNode.h"
#include <unordered_map>
#include <string>

class Interpreter {
	std::map<char, expert_system::Value> storage_ {};
	std::vector<expert_system::iExpertNode*> expressions_;
	std::stack<char> queries_;
	mutable bool storage_changed_{false};
public:
	std::unordered_map<std::string, expert_system::iExpertNode*> repeatDestroyer_;


	expert_system::Value getValueByVarName(char vn) const;
	bool isVarSet(char vn) const;
	void setVarWithValue(expert_system::Value vl, char vn);

	void evalAllAsTrue();
	void startInteractive();
	void addExpression(expert_system::iExpertNode *expression);
	void addQuery(char c);
	void processAllQueries();
	bool storageChanged() const;

	std::string trees_to_string() const;

};





#endif //EXPERT_SYSTEM_INTERPRETER_H

#pragma clang diagnostic pop