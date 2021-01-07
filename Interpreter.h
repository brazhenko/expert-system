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
	std::map<char, expert_system::Value> cache_ {};
	std::vector<expert_system::iExpertNode*> expressions_;
	std::stack<char> queries_;
public:
	std::unordered_map<std::string, expert_system::iExpertNode*> repeatDestroyer_;


	expert_system::Value getValueByVarName(char vn) const;
	bool isVarSet(char vn) const;
	void setVarWithValue(expert_system::Value vl, char vn);
	void commitChanges();
	void discardChanges();
	void evalAllAsTrue();
	void startInteractive();
	void addExpression(expert_system::iExpertNode *expression);
	void addQuery(char c);
	void processAllQueries();

	std::string trees_to_string() const;

};


class myclass : public std::shared_ptr<int> {
	myclass()
	{
	}

};


#endif //EXPERT_SYSTEM_INTERPRETER_H

#pragma clang diagnostic pop