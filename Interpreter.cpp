//
// Created by Lommy Reznak on 1/6/21.
//

#include <iostream>
#include "Interpreter.h"
#include <sstream>

expert_system::Value Interpreter::getValueByVarName(char vn) const
{
	try {
		return storage_.at(vn);
	} catch (...) {
		return expert_system::Value::False;
	}
}

bool Interpreter::isVarSet(char vn) const {
	try {
		storage_.at(vn);
		return true;
	} catch (...) {
		return false;
	}
}

void Interpreter::setVarWithValue(expert_system::Value vl, char vn)
{
	this->storage_[vn] = vl;
	this->storage_changed_ = true;
}

void Interpreter::evalAllAsTrue()
{
	for (auto &expr : this->expressions_)
	{
		expr->evalAs(expert_system::Value::True);
	}
}

int yyparse();
extern FILE * yyin;

void Interpreter::startInteractive()
{
	yyin = stdin;
	yyparse();
}

extern int yylex_destroy();
void Interpreter::startFile(const std::string &filename)
{

	std::cout << "START" << std::endl;

	yylex_destroy();

	yyin = fopen(filename.c_str(),"r");
	yyparse();
}


std::string Interpreter::trees_to_string() const {
	std::stringstream ss;
	std::string ret;

	ss << "Known facts:" << "\n";

	for (const auto &[k, v] : this->storage_)
	{
		ss << "\t" << k << " <- ";
		if (v == expert_system::Value::True)
			ss << "True";
		else if (v == expert_system::Value::False)
			ss << "False";

		ss << "\n";
	}

	for (int i = 0; i < this->expressions_.size(); i++)
	{
		ss << i + 1 << ")" << "\n"
			<< "Raw:\n"
			<< this->expressions_[i]->to_string() << "\n"
			<< "Reduced:\n"
			<< this->expressions_[i]->to_reduced_string() << std::endl;
	}

	ret = ss.str();
	return ret;
}

void Interpreter::addExpression(expert_system::iExpertNode *expression)
{
	this->expressions_.push_back(expression);
}

void Interpreter::addQuery(char c)
{
	this->queries_.push(c);
}

void Interpreter::processAllQueries()
{
	this->evalAllAsTrue();

	while (!this->queries_.empty())
	{
		auto c = queries_.top();
		queries_.pop();
		std::cout << "Processing " << c << std::endl;
	}
}

bool Interpreter::storageChanged() const
{
	bool ret = storage_changed_;
	storage_changed_ = false;

	return ret;
}

void Interpreter::reset()
{
	for (auto &expr : this->expressions_)
	{
		delete expr;
	}
	fclose(yyin);

	expressions_.clear();
	storage_.clear();
	storage_changed_ = false;
	repeatDestroyer_.clear();
	repeatUsageCount_.clear();
}



