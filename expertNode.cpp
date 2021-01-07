//
// Created by Lommy Reznak on 1/6/21.
//

#include "expertNode.h"
#include <string>
#include "Interpreter.h"
#include <sstream>
#include <iostream>

extern Interpreter interpreter;
using expert_system::Value;
using expert_system::iExpertNode;

# define RIGHT(VAL) Value((VAL) & 1U)
# define LEFT(VAL) Value((unsigned(VAL) >> 1U) & 1U)
# define TOUNSIGNED(VALL, VALR) (unsigned(VALL) << 1U | unsigned(VALR))

# define EVAL_AS(NODENAME) \
	if (this->val_ != Value::Undefined && this->val_ != val) \
	{ \
		std::stringstream ss;\
		ss << this->to_string() << " cannot be both true and false";\
		throw std::logic_error(ss.str());\
	}\
\
	auto leftVal = this->l_->eval();\
	auto rightVal = this->r_->eval();\
	if (NODENAME == std::string("Implication"))\
		std::cout << "DEBUG: " << int(leftVal) << " " << int(rightVal) << std::endl;\
\
\
\
\
	std::set<unsigned> newS;\
\
	if (val == Value::True)\
	{\
		auto& t = solutions.at(NODENAME).at(Value::True);\
		set_intersection(t.begin(), t.end(), solutions_.begin(), solutions_.end(),\
						 std::inserter(newS, newS.begin()));\
	}\
	else if (val == Value::False)\
	{\
		auto &t = solutions.at(NODENAME).at(Value::False);\
		set_intersection(t.begin(), t.end(), solutions_.begin(), solutions_.end(),\
						 std::inserter(newS, newS.begin()));\
	}\
\
	if (newS.find(TOUNSIGNED(leftVal, rightVal)) != newS.end())\
		return ;\
\
	if (newS.empty())\
	{\
		std::stringstream ss;\
		ss << "No solution for " << this->to_string();\
		throw std::logic_error(ss.str());\
	}\
\
	std::set<unsigned> avlAnswers;\
\
	for (auto slt : newS)\
	{\
		try {\
			l_->evalAs(LEFT(slt), true);\
			r_->evalAs(RIGHT(slt), true);\
\
			avlAnswers.insert(slt);\
		}\
		catch (const std::exception &e) {\
			/*std::cerr << "DEBUG: " << e.what() << std::endl; */ \
		}\
	}\
\
	if (ask) return;\
\
	if (avlAnswers.size() > 1)\
	{\
		solutions_ = avlAnswers;\
		return ;\
	}\
\
	if (avlAnswers.size() == 1)\
	{\
		l_->evalAs(LEFT(*avlAnswers.begin()), false);\
		r_->evalAs(RIGHT(*avlAnswers.begin()), false);\
\
		val_ = val;\
		return ;\
	}\
\
	throw std::logic_error("No solution exist");\

///
/// Var
///

expert_system::Value expert_system::Var::eval() {
	return interpreter.getValueByVarName(this->Letter);
}

std::string expert_system::Var::to_string() {
	return std::string(1, this->Letter);
}

void expert_system::Var::evalAs(Value val, bool ask) {
	if (interpreter.isVarSet(this->Letter)
		&& val == interpreter.getValueByVarName(this->Letter))
		return;

	if (interpreter.isVarSet(this->Letter)
		&& interpreter.getValueByVarName(this->Letter) != Value::Undefined)
	{
		std::stringstream ss;

		ss << "Trying to set " << this->Letter << " when it is already set " << int(val) << " " << int(interpreter.getValueByVarName(this->Letter));
		throw std::logic_error(ss.str());
	}

	if (ask)
		return;

	interpreter.setVarWithValue(val, this->Letter);
}

std::string expert_system::Var::to_reduced_string() {
	auto t = interpreter.getValueByVarName(this->Letter);

	if (t == Value::Undefined)
		return "Undefined";
	if (t == Value::True)
		return "True";
	return "False";
}

std::vector<iExpertNode *> expert_system::Var::get_children() {
	return std::vector<iExpertNode *>();
}

expert_system::Var::~Var()
= default;

///
/// Or
///

expert_system::Value expert_system::Or::eval() {

	auto leftVal = this->l_->eval();
	auto rightEval = this->r_->eval();

	if (leftVal == Value::True || rightEval == Value::True)
		return Value::True;

	if (leftVal == Value::Undefined && rightEval == Value::Undefined)
		return Value::True;

	if (leftVal == Value::Undefined || rightEval == Value::Undefined)
		return Value::Undefined;

	return expert_system::Value::False;
}

std::string expert_system::Or::to_string() {
	std::string ret = "(" + l_->to_string() + ")" + " | " + "(" + r_->to_string() + ")";
	return ret;
}

void expert_system::Or::evalAs(Value val, bool ask) {
	if (this->val_ != Value::Undefined && this->val_ != val)
	{
		std::stringstream ss;
		ss << this->to_string() << " cannot be both true and false";
		throw std::logic_error(ss.str());
	}

	auto leftVal = this->l_->eval();
	auto rightVal = this->r_->eval();


	std::set<unsigned> newS;

	if (val == Value::True)
	{
		auto& t = solutions.at("Or").at(Value::True);
		set_intersection(t.begin(), t.end(), solutions_.begin(), solutions_.end(),
						 std::inserter(newS, newS.begin()));
	}
	else if (val == Value::False)
	{
		auto &t = solutions.at("Or").at(Value::False);
		set_intersection(t.begin(), t.end(), solutions_.begin(), solutions_.end(),
						 std::inserter(newS, newS.begin()));
	}

	if (newS.find(TOUNSIGNED(leftVal, rightVal)) != newS.end())
		return ;

	if (newS.empty())
	{
		std::stringstream ss;
		ss << "No solution for " << this->to_string();
		throw std::logic_error(ss.str());
	}

	std::set<unsigned> avlAnswers;

	for (auto slt : newS)
	{
		try {
			l_->evalAs(LEFT(slt), true);
			r_->evalAs(RIGHT(slt), true);

			avlAnswers.insert(slt);
		}
		catch (const std::exception &e) {
			if (ask) throw;
			/*std::cerr << "DEBUG: " << e.what() << std::endl; */
		}
	}

	if (ask) return;

	if (avlAnswers.size() > 1)
	{
		solutions_ = avlAnswers;
		return ;
	}

	if (avlAnswers.size() == 1)
	{
		l_->evalAs(LEFT(*avlAnswers.begin()), false);
		r_->evalAs(RIGHT(*avlAnswers.begin()), false);

		val_ = val;
		return ;
	}

	throw std::logic_error("No solution exist");
}

std::string expert_system::Or::to_reduced_string() {
	std::string ret = "(" + l_->to_reduced_string() + ")" + " | " + "(" + r_->to_reduced_string() + ")";
	return ret;
}

expert_system::Or::~Or() {
	if (interpreter.repeatUsageCount_[l_->to_string()]-- == 1)
		delete l_;

	if (interpreter.repeatUsageCount_[r_->to_string()]-- == 1)
		delete r_;

}

std::vector<iExpertNode *> expert_system::Or::get_children() {
	return {l_, r_};
}

///
/// Implication
///

expert_system::Value expert_system::Implication::eval() {
	if (this->val_ != Value::Undefined) return this->val_;

	auto leftVal = this->l_->eval();
	auto rightVal = this->r_->eval();

	if (leftVal == Value::False)
		return Value::True;

	if (leftVal == Value::True && rightVal == Value::True) return Value::True;

	return expert_system::Value::False;
}

std::string expert_system::Implication::to_string() {
	std::string ret = "(" + l_->to_string() + ")" + " => " + "(" + r_->to_string() + ")";
	return ret;
}

void expert_system::Implication::evalAs(Value val, bool ask) {
	if (this->val_ != Value::Undefined && this->val_ != val) 
	{ 
		std::stringstream ss;
		ss << this->to_string() << " cannot be both true and false";
		throw std::logic_error(ss.str());
	}

	auto leftVal = this->l_->eval();
	auto rightVal = this->r_->eval();




	std::set<unsigned> newS;

	if (val == Value::True)
	{
		auto& t = solutions.at("Implication").at(Value::True);
		set_intersection(t.begin(), t.end(), solutions_.begin(), solutions_.end(),
						 std::inserter(newS, newS.begin()));
	}
	else if (val == Value::False)
	{
		auto &t = solutions.at("Implication").at(Value::False);
		set_intersection(t.begin(), t.end(), solutions_.begin(), solutions_.end(),
						 std::inserter(newS, newS.begin()));
	}

	if (newS.find(TOUNSIGNED(leftVal, rightVal)) != newS.end())
		return ;

	if (newS.empty())
	{
		std::stringstream ss;
		ss << "No solution for " << this->to_string();
		throw std::logic_error(ss.str());
	}

	std::set<unsigned> avlAnswers;

	for (auto slt : newS)
	{
		try {
			l_->evalAs(LEFT(slt), true);
			r_->evalAs(RIGHT(slt), true);

			avlAnswers.insert(slt);
		}
		catch (const std::exception &e) {
			if (ask) throw;
			/*std::cerr << "DEBUG: " << e.what() << std::endl; */ 
		}
	}

	if (ask) return;

	if (avlAnswers.size() > 1)
	{
		solutions_ = avlAnswers;
		return ;
	}

	if (avlAnswers.size() == 1)
	{
		l_->evalAs(LEFT(*avlAnswers.begin()), false);
		r_->evalAs(RIGHT(*avlAnswers.begin()), false);

		val_ = val;
		return ;
	}

	throw std::logic_error("No solution exist");
}

std::string expert_system::Implication::to_reduced_string() {
	std::string ret = "(" + l_->to_reduced_string() + ")" + " => " + "(" + r_->to_reduced_string() + ")";
	return ret;
}

expert_system::Implication::~Implication() {
	if (interpreter.repeatUsageCount_[l_->to_string()]-- == 1)
		delete l_;

	if (interpreter.repeatUsageCount_[r_->to_string()]-- == 1)
		delete r_;
}

std::vector<iExpertNode *> expert_system::Implication::get_children() {
	return {l_, r_};
}

///
/// And
///

expert_system::Value expert_system::And::eval() {
	auto leftVal = this->l_->eval();
	auto rightVal = this->r_->eval();

	if (leftVal == Value::True && rightVal == Value::True)
	{
		std::cout << "TRUE" << std::endl;
		return Value::True;
	}


	return expert_system::Value::False;
}

std::string expert_system::And::to_string() {
	std::string ret = "(" + l_->to_string() + ")" + " + " + "(" + r_->to_string() + ")";
	return ret;
}

void expert_system::And::evalAs(Value val, bool ask) {
	if (this->val_ != Value::Undefined && this->val_ != val)
	{
		std::stringstream ss;
		ss << this->to_string() << " cannot be both true and false";
		throw std::logic_error(ss.str());
	}

	auto leftVal = this->l_->eval();
	auto rightVal = this->r_->eval();




	std::set<unsigned> newS;

	if (val == Value::True)
	{
		auto& t = solutions.at("And").at(Value::True);
		set_intersection(t.begin(), t.end(), solutions_.begin(), solutions_.end(),
						 std::inserter(newS, newS.begin()));
	}
	else if (val == Value::False)
	{
		auto &t = solutions.at("And").at(Value::False);
		set_intersection(t.begin(), t.end(), solutions_.begin(), solutions_.end(),
						 std::inserter(newS, newS.begin()));
	}

	if (newS.find(TOUNSIGNED(leftVal, rightVal)) != newS.end())
		return ;

	if (newS.empty())
	{
		std::stringstream ss;
		ss << "No solution for " << this->to_string();
		throw std::logic_error(ss.str());
	}

	std::set<unsigned> avlAnswers;

	for (auto slt : newS)
	{
		try {
			l_->evalAs(LEFT(slt), true);
			r_->evalAs(RIGHT(slt), true);

			avlAnswers.insert(slt);
		}
		catch (const std::exception &e) {
			if (ask) throw;
			/*std::cerr << "DEBUG: " << e.what() << std::endl; */
		}
	}

	if (ask) return;

	if (avlAnswers.size() > 1)
	{
		solutions_ = avlAnswers;
		return ;
	}

	if (avlAnswers.size() == 1)
	{
		l_->evalAs(LEFT(*avlAnswers.begin()), false);
		r_->evalAs(RIGHT(*avlAnswers.begin()), false);

		val_ = val;
		return ;
	}

	throw std::logic_error("No solution exist");
}

std::string expert_system::And::to_reduced_string() {
	std::string ret = "(" + l_->to_reduced_string() + ")" + " + " + "(" + r_->to_reduced_string() + ")";
	return ret;
}

expert_system::And::~And() {
	if (interpreter.repeatUsageCount_[l_->to_string()]-- == 1)
		delete l_;

	if (interpreter.repeatUsageCount_[r_->to_string()]-- == 1)
		delete r_;
}

std::vector<iExpertNode *> expert_system::And::get_children() {
	return {l_, r_};
}

///
/// Not
///

expert_system::Value expert_system::Not::eval() {
	auto res = this->c_->eval();

	if (res == Value::True)
		return Value::False;
	if (res == Value::False)
		return Value::True;
	return Value::Undefined;
}

std::string expert_system::Not::to_string() {
	std::string ret = "!(" + c_->to_string() + ")";
	return ret;
}

void expert_system::Not::evalAs(Value val, bool ask)
{
	if (this->val_ != Value::Undefined && this->val_ != val)
	{
		std::stringstream ss;
		ss << this->to_string() << " cannot be both true and false";
		throw std::logic_error(ss.str());
	}

	auto curVal = this->c_->eval();

	std::set<unsigned> newS;

	if (val == Value::True)
	{
		auto& t = solutions.at("Not").at(Value::True);
		set_intersection(t.begin(), t.end(), solutions_.begin(), solutions_.end(),
						 std::inserter(newS, newS.begin()));
	}
	else if (val == Value::False)
	{
		auto &t = solutions.at("Not").at(Value::False);
		set_intersection(t.begin(), t.end(), solutions_.begin(), solutions_.end(),
						 std::inserter(newS, newS.begin()));
	}

	if (newS.find(unsigned(curVal)) != newS.end())
		return ;

	if (newS.empty())
	{
		std::stringstream ss;
		ss << "No solution for " << this->to_string();
		throw std::logic_error(ss.str());
	}


	std::set<unsigned> avlAnswers;

	for (auto slt : newS)
	{
		try {
			c_->evalAs(Value(slt), true);
			avlAnswers.insert(slt);
		}
		catch (const std::exception &e) {
			if (ask) throw;
			/*std::cerr << "DEBUG: " << e.what() << std::endl; */
		}
	}

	if (ask) return;

	if (avlAnswers.size() > 1)
	{
		solutions_ = avlAnswers;
		return ;
	}

	if (avlAnswers.size() == 1)
	{
		c_->evalAs(Value(*avlAnswers.begin()), false);

		val_ = val;
		return ;
	}

	throw std::logic_error("No solution exist");
}

std::string expert_system::Not::to_reduced_string() {
	std::string ret = "!(" + c_->to_reduced_string() + ")";
	return ret;
}

expert_system::Not::~Not() {
	if (interpreter.repeatUsageCount_[c_->to_string()]-- == 1)
		delete c_;
}

std::vector<iExpertNode *> expert_system::Not::get_children() {
	return {c_};
}

///
/// Xor
///

expert_system::Value expert_system::Xor::eval() {
	auto leftVal = this->l_->eval();
	auto rightVal = this->r_->eval();

	if (leftVal == Value::Undefined || rightVal == Value::Undefined)
		return Value::Undefined;

	if (leftVal != rightVal)
		return Value::True;

	return expert_system::Value::False;
}

std::string expert_system::Xor::to_string() {
	std::string ret = "(" + l_->to_string() + ")" + " ^ " + "(" + r_->to_string() + ")";
	return ret;
}

void expert_system::Xor::evalAs(Value val, bool ask) {
	if (this->val_ != Value::Undefined && this->val_ != val)
	{
		std::stringstream ss;
		ss << this->to_string() << " cannot be both true and false";
		throw std::logic_error(ss.str());
	}

	auto leftVal = this->l_->eval();
	auto rightVal = this->r_->eval();


	std::set<unsigned> newS;

	if (val == Value::True)
	{
		auto& t = solutions.at("Xor").at(Value::True);
		set_intersection(t.begin(), t.end(), solutions_.begin(), solutions_.end(),
						 std::inserter(newS, newS.begin()));
	}
	else if (val == Value::False)
	{
		auto &t = solutions.at("Xor").at(Value::False);
		set_intersection(t.begin(), t.end(), solutions_.begin(), solutions_.end(),
						 std::inserter(newS, newS.begin()));
	}

	if (newS.find(TOUNSIGNED(leftVal, rightVal)) != newS.end())
		return ;

	if (newS.empty())
	{
		std::stringstream ss;
		ss << "No solution for " << this->to_string();
		throw std::logic_error(ss.str());
	}

	std::set<unsigned> avlAnswers;

	for (auto slt : newS)
	{
		try {
			l_->evalAs(LEFT(slt), true);
			r_->evalAs(RIGHT(slt), true);

			avlAnswers.insert(slt);
		}
		catch (const std::exception &e) {
			if (ask) throw;
			/*std::cerr << "DEBUG: " << e.what() << std::endl; */
		}
	}

	if (ask) return;

	if (avlAnswers.size() > 1)
	{
		solutions_ = avlAnswers;
		return ;
	}

	if (avlAnswers.size() == 1)
	{
		l_->evalAs(LEFT(*avlAnswers.begin()), false);
		r_->evalAs(RIGHT(*avlAnswers.begin()), false);

		val_ = val;
		return ;
	}

	throw std::logic_error("No solution exist");
}

std::string expert_system::Xor::to_reduced_string() {
	std::string ret = "(" + l_->to_reduced_string() + ")" + " ^ " + "(" + r_->to_reduced_string() + ")";
	return ret;
}

expert_system::Xor::~Xor() {
	if (interpreter.repeatUsageCount_[l_->to_string()]-- == 1)
		delete l_;

	if (interpreter.repeatUsageCount_[r_->to_string()]-- == 1)
		delete r_;
}

std::vector<iExpertNode *> expert_system::Xor::get_children() {
	return {l_, r_};
}

///
/// Equ
///

expert_system::Value expert_system::Equ::eval() {
	auto leftVal = this->l_->eval();
	auto rightVal = this->r_->eval();

	if (leftVal == Value::Undefined || rightVal == Value::Undefined)
		return Value::Undefined;

	if (leftVal == rightVal)
		return Value::True;

	return expert_system::Value::False;
}

std::string expert_system::Equ::to_string() {
	std::string ret = "(" + l_->to_string() + ")" + " <=> " + "(" + r_->to_string() + ")";
	return ret;
}

void expert_system::Equ::evalAs(Value val, bool ask) {
	EVAL_AS("Equ")
}

std::string expert_system::Equ::to_reduced_string() {
	std::string ret = "(" + l_->to_reduced_string() + ")" + " <=> " + "(" + r_->to_reduced_string() + ")";
	return ret;
}

expert_system::Equ::~Equ() {
	if (interpreter.repeatUsageCount_[l_->to_string()]-- == 1)
		delete l_;

	if (interpreter.repeatUsageCount_[r_->to_string()]-- == 1)
		delete r_;
}

std::vector<iExpertNode *> expert_system::Equ::get_children() {
	return {l_, r_};
}

